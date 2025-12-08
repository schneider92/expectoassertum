#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expectoassertum.h"

typedef struct ea_test_s {
	// tree
	struct ea_test_s* next;

	// info
	const char* name;

	// test function
	ea__test_func_t test_func;
} ea_test_t;

struct ea__test_info_s {
	int total_count; // total executed test count
	int failed_count; // total failed test count
	int filtered_count; // total filtered out test count

	int current_failed; // current test failed flag
};

struct ea_group_s {
	// tree
	ea_group_t* parent;
	ea_group_t* next_sibling;
	ea_group_t* prev_sibling;
	ea_group_t* children_head;
	ea_group_t* children_tail;
	ea_test_t* tests_head;
	ea_test_t* tests_tail;

	// info
	const char* name;

	// memory
	ea_mem_alloc_func_t mem_alloc;
	void* mem_alloc_opaque;

	// setup/teardown
	ea_group_setup_teardown_func_t setup, teardown;
	void* setup_opaque;
	void* teardown_opaque;
};

static ea_group_t* create_group(ea_group_t* parent, const char* name,
	ea_mem_alloc_func_t mem_alloc, void* mem_alloc_opaque)
{
	ea_group_t* group = (ea_group_t*)mem_alloc(NULL, sizeof(ea_group_t), mem_alloc_opaque);
	group->parent = parent;
	group->next_sibling = NULL;
	group->prev_sibling = NULL;
	group->children_head = NULL;
	group->children_tail = NULL;
	group->tests_head = NULL;
	group->tests_tail = NULL;
	group->name = name;
	group->mem_alloc = mem_alloc;
	group->mem_alloc_opaque = mem_alloc_opaque;
	group->setup = NULL;
	group->teardown = NULL;
	group->setup_opaque = NULL;
	group->teardown_opaque = NULL;

	if (parent) {
		// link into parent's children list
		if (parent->children_tail) {
			parent->children_tail->next_sibling = group;
			group->prev_sibling = parent->children_tail;
			parent->children_tail = group;
		}
		else {
			parent->children_head = group;
			parent->children_tail = group;
		}
	}

	return group;
}

static void* default_mem_alloc_func(void* block, int size, void* opaque) {
	(void)opaque;
	if (block) {
		free(block);
		return NULL;
	}
	else {
		return malloc(size);
	}
}
ea_group_t* ea_create_root() {
	return create_group(NULL, "", default_mem_alloc_func, NULL);
}

ea_group_t* ea_create_root_nomalloc(ea_mem_alloc_func_t mem_alloc, void* opaque) {
	return create_group(NULL, "", mem_alloc, opaque);
}

ea_group_t* ea_group_create(ea_group_t* parent, const char* name) {
	return create_group(parent, name, parent->mem_alloc, parent->mem_alloc_opaque);
}

void ea_release_group(ea_group_t* group) {
	// release children
	while (1) {
		ea_group_t* head = group->children_head;
		if (!head) break;
		ea_release_group(head);
	}

	// release tests
	while (1) {
		ea_test_t* head = group->tests_head;
		if (!head) break;
		group->tests_head = head->next;
		group->mem_alloc(head, 0, group->mem_alloc_opaque);
	}

	// unlink from parent
	if (group->parent) {
		if (group->prev_sibling) {
			group->prev_sibling->next_sibling = group->next_sibling;
		}
		else {
			group->parent->children_head = group->next_sibling;
		}
		if (group->next_sibling) {
			group->next_sibling->prev_sibling = group->prev_sibling;
		}
		else {
			group->parent->children_tail = group->prev_sibling;
		}
	}

	// free group memory
	group->mem_alloc(group, 0, group->mem_alloc_opaque);
}

void ea_group_set_setup(ea_group_t* group, ea_group_setup_teardown_func_t setup, void* opaque) {
	group->setup = setup;
	group->setup_opaque = opaque;
}

void ea_group_set_teardown(ea_group_t* group, ea_group_setup_teardown_func_t teardown, void* opaque) {
	group->teardown = teardown;
	group->teardown_opaque = opaque;
}

void ea__test_add(ea_group_t* group, ea__test_func_t test_func, const char* test_name) {
	ea_test_t* test = (ea_test_t*)group->mem_alloc(NULL, sizeof(ea_test_t), group->mem_alloc_opaque);
	test->next = NULL;
	test->name = test_name;
	test->test_func = test_func;

	if (group->tests_tail) {
		group->tests_tail->next = test;
		group->tests_tail = test;
	}
	else {
		group->tests_head = test;
		group->tests_tail = test;
	}
}

const char* ea_parse_filter_cmdline(int argc, char** argv) {
	const char* prefix = "--filter=";
	size_t prefix_len = 9;
	for (int i = 1; i < argc; ++i) {
		if (strncmp(argv[i], prefix, prefix_len) == 0) {
			return argv[i] + prefix_len;
		}
	}
	return NULL;
}

enum {
	filter_mode_full   = 'f',
	filter_mode_prefix = 'p',
	filter_mode_suffix = 's',
};
typedef struct {
	const char* match;
	int length;
	char mode, negated;
} ea_filter_t;

static const ea_filter_t* parse_filters(ea_group_t* group, const char* filterstring) {
	// count filters
	int count = 1;
	for (const char* p = filterstring; *p; ++p) {
		if (*p == ',') {
			count++;
		}
	}

	// allocate filter array
	ea_filter_t* filters = (ea_filter_t*)group->mem_alloc(NULL, sizeof(ea_filter_t) * (count+1), group->mem_alloc_opaque);

	// parse filters
	int filter_index = 0;
	const char* filter_start = filterstring;
	for (const char* p = filterstring; ; ++p) {
		if ((*p == ',') || (*p == '\0')) {
			// end of filter
			const char* filter_end = p;

			// parse filter
			ea_filter_t* filter = &filters[filter_index++];
			filter->negated = 0;
			if (filter_start < filter_end) {
				if (*filter_start == '~') {
					filter->negated = 1;
					filter_start++;
				}
			}
			if ((filter_end > filter_start) && (*(filter_end - 1) == '*')) {
				filter->mode = filter_mode_prefix;
				filter_end--;
			}
			else if ((filter_end > filter_start) && (*filter_start == '*')) {
				filter->mode = filter_mode_suffix;
				filter_start++;
			}
			else {
				filter->mode = filter_mode_full;
			}
			filter->match = filter_start;
			filter->length = (int)(filter_end - filter_start);

			// next filter
			if (*p == '\0') {
				break;
			}
			filter_start = p + 1;
		}
	}

	filters[count].match = NULL; // end marker
	return filters;
}

int match_filters(const ea_filter_t* filters, const char* testname, int testname_len) {
	if (!filters) {
		return 1; // no filters, always match
	}
	int matched = 0;
	for (const ea_filter_t* filter = filters; filter->match; ++filter) {
		int is_match = 0;
		switch (filter->mode) {
		case filter_mode_full:
			if ((int)testname_len == filter->length) {
				if (strncmp(testname, filter->match, filter->length) == 0) {
					is_match = 1;
				}
			}
			break;
		case filter_mode_prefix:
			if (testname_len >= filter->length) {
				if (strncmp(testname, filter->match, filter->length) == 0) {
					is_match = 1;
				}
			}
			break;
		case filter_mode_suffix:
			if (testname_len >= filter->length) {
				if (strncmp(testname + testname_len - filter->length, filter->match, filter->length) == 0) {
					is_match = 1;
				}
			}
			break;
		}
		if (is_match) {
			if (filter->negated) {
				return 0; // negated match fails
			}
			else {
				matched = 1; // positive match
			}
		}
	}
	return matched;
}

#ifndef TESTNAME_BUF_LEN
#define TESTNAME_BUF_LEN 256
#endif

static int append_name_to_buf(char* buf, int pos, const char* name) {
	if ((pos > 0) && (pos < TESTNAME_BUF_LEN)) {
		buf[pos++] = '/';
	}
	for (int i = 0; name[i] && (pos < TESTNAME_BUF_LEN); ++i) {
		buf[pos++] = name[i];
	}
	return pos;
}

#ifndef TESTNAME_WIDTH
#define TESTNAME_WIDTH 65
#endif

void run_group(ea_group_t* group, char* namebuf, int namebufpos, ea__test_info_t* info, const ea_filter_t* filters) {
	// write group name to name buffer
	namebufpos = append_name_to_buf(namebuf, namebufpos, group->name);

	// run group setup
	if (group->setup) {
		group->setup(group->setup_opaque);
	}

	// run tests in this group
	ea_test_t* test = group->tests_head;
	while (test) {
		// set up test name and check filters
		int testnamepos = append_name_to_buf(namebuf, namebufpos, test->name);
		int match = match_filters(filters, namebuf, testnamepos);
		if (!match) {
			info->filtered_count++;
			test = test->next;
			continue;
		}

		// print test name
		printf("%.*s", testnamepos, namebuf);
		for (int i = testnamepos; i < TESTNAME_WIDTH; ++i) {
			printf(" ");
		}
		printf(" => ");

		// create test info
		ea__test_info_t test_info = { 0 };

		// run test
		test->test_func(&test_info);

		// if success, print result
		if (!test_info.current_failed) {
			printf("OK\n");
		}

		// if failed, increment failed counter
		else {
			info->failed_count++;
		}

		// increment total counter
		info->total_count++;

		test = test->next;
	}

	// run child groups
	ea_group_t* child = group->children_head;
	while (child) {
		run_group(child, namebuf, namebufpos, info, filters);
		child = child->next_sibling;
	}

	// run group teardown
	if (group->teardown) {
		group->teardown(group->teardown_opaque);
	}
}

void ea_run(ea_group_t* group, const char* filterstring) {
	// parse filter string
	const ea_filter_t* filters = 0;
	if (filterstring) {
		printf("Applying test filter: %s\n", filterstring);
		filters = parse_filters(group, filterstring);
	}

	// run the group
	ea__test_info_t test_info = { 0 };
	char namebuf[TESTNAME_BUF_LEN + 1];
	run_group(group, namebuf, 0, &test_info, filters);

	// print summary
	if (test_info.failed_count == 0) {
		printf("All %d tests passed.\n", test_info.total_count);
	}
	else {
		printf("%d test(s) out of %d failed.\n", test_info.failed_count, test_info.total_count);
	}
	if (test_info.filtered_count > 0) {
		printf("%d test(s) were filtered out.\n", test_info.filtered_count);
	}

	// free filters
	if (filters) {
		group->mem_alloc((void*)filters, 0, group->mem_alloc_opaque);
	}
}

void ea__print_assertion_failed(ea__test_info_t* test_info, const char* file, int line) {
	// mark test as failed and print message
	if (!test_info->current_failed) {
		test_info->current_failed = 1;
		printf("FAILED\n");
	}

	// cut filename to last path component
	const char* short_file = file;
	for (const char* p = file; *p; ++p) {
		if ((*p == '/') || (*p == '\\')) {
			short_file = p + 1;
		}
	}

	// print assertion details
	printf("  Assertion failed at %s line %d:\n", short_file, line);
}

#define print_message() if (msg) { \
	printf("  Message: "); \
	va_list args; \
	va_start(args, msg); \
	vprintf(msg, args); \
	va_end(args); \
	printf("\n"); \
}

int ea__assert_bool_check(ea__test_info_t* test_info, int actual, const char* actual_str, int exp, const char* file, int line, const char* msg, ...) {
	if (actual == exp) {
		return 1;
	}
	ea__print_assertion_failed(test_info, file, line);
	const char* boolstrs[] = { "true", "false" };
	printf("  Expected %s (which is %s) to be %s\n", actual_str, boolstrs[!actual], boolstrs[!exp]);
	print_message();
	return 0;
}

static const char* get_opstr(int op) {
	switch (op) {
	case ea__op_eq: return "equal to";
	case ea__op_ne: return "not equal to";
	case ea__op_lt: return "less than";
	case ea__op_le: return "less than or equal to";
	case ea__op_gt: return "greater than";
	case ea__op_ge: return "greater than or equal to";
	default: return "??";
	}
}

int ea__assert_int_check(ea__test_info_t* test_info, long long a, long long b, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...) {
	int res = 0;
	switch (op) {
	case ea__op_eq: res = (a == b); break;
	case ea__op_ne: res = (a != b); break;
	case ea__op_lt: res = (a < b); break;
	case ea__op_le: res = (a <= b); break;
	case ea__op_gt: res = (a > b); break;
	case ea__op_ge: res = (a >= b); break;
	}
	if (res) {
		return 1;
	}
	ea__print_assertion_failed(test_info, file, line);
	printf("  Expected %s (which is %lld)\n  to be %s %s (which is %lld)\n", sa, a, get_opstr(op), sb, b);
	print_message();
	return 0;
}

int ea__assert_uint_check(ea__test_info_t* test_info, unsigned long long a, unsigned long long b, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...) {
	int res = 0;
	switch (op) {
	case ea__op_eq: res = (a == b); break;
	case ea__op_ne: res = (a != b); break;
	case ea__op_lt: res = (a < b); break;
	case ea__op_le: res = (a <= b); break;
	case ea__op_gt: res = (a > b); break;
	case ea__op_ge: res = (a >= b); break;
	}
	if (res) {
		return 1;
	}
	ea__print_assertion_failed(test_info, file, line);
	printf("  Expected %s (which is %llu)\n  to be %s %s (which is %llu)\n", sa, a, get_opstr(op), sb, b);
	print_message();
	return 0;
}

int ea__assert_ptr_check(ea__test_info_t* test_info, const void* a, const void* b, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...) {
	int res = 0;
	switch (op) {
	case ea__op_eq: res = (a == b); break;
	case ea__op_ne: res = (a != b); break;
	}
	if (res) {
		return 1;
	}
	ea__print_assertion_failed(test_info, file, line);
	printf("  Expected %s (which is %p)\n  to be %s %s (which is %p)\n", sa, a, get_opstr(op), sb, b);
	print_message();
	return 0;
}

int ea__assert_ptr_null_check(ea__test_info_t* test_info, const void* a, int is_null, const char* sa, const char* file, int line, const char* msg, ...) {
	int res = is_null ? (a == NULL) : (a != NULL);
	if (res) {
		return 1;
	}
	ea__print_assertion_failed(test_info, file, line);
	if (is_null) {
		printf("  Expected %s (which is %p) to be NULL\n", sa, a);
	}
	else {
		printf("  Expected %s (which is NULL) to be not NULL\n", sa);
	}
	print_message();
	return 0;
}

int ea__assert_str_check(ea__test_info_t* test_info, const char* a, const char* b, int size, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...) {
	// determine comparison result
	int cmpres;
	if (size < 0) {
		cmpres = strcmp(a, b);
	}
	else {
		cmpres = strncmp(a, b, (size_t)size);
	}

	// evaluate comparison result
	int res = 0;
	if (op == ea__op_eq) {
		res = (cmpres == 0);
	}
	else if (op == ea__op_ne) {
		res = (cmpres != 0);
	}
	if (res) {
		return 1;
	}

	// assertion failed
	ea__print_assertion_failed(test_info, file, line);
	if (size < 0) {
		printf("  Expected %s (which is \"%s\")\n  to be %s %s (which is \"%s\")\n", sa, a, get_opstr(op), sb, b);
	}
	else {
		printf("  Expected first %d characters of %s (which is \"%.*s\")\n  to be %s first %d characters of %s (which is \"%.*s\")\n", size, sa, size, a, get_opstr(op), size, sb, size, b);
	}
	print_message();
	return 0;
}

int ea__assert_double_check(ea__test_info_t* test_info, double a, double b, double reltol, double abstol, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...) {
	// find effective tolerance
	{
		double aa = (a >= 0.0) ? a : -a;
		double ab = (b >= 0.0) ? b : -b;
		double tol = ((aa > ab) ? aa : ab) * reltol;
		if (abstol < tol) {
			abstol = tol;
		}
	}

	// determine comparison result
	int eqres = (a-b <= abstol) && (b-a <= abstol);
	int ltres = a < b + abstol;
	int gtres = a + abstol > b;

	// evaluate comparison result
	int res = 0;
	switch (op) {
	case ea__op_eq: res = eqres; break;
	case ea__op_ne: res = !eqres; break;
	case ea__op_lt: res = ltres; break;
	case ea__op_le: res = ltres || eqres; break;
	case ea__op_gt: res = gtres; break;
	case ea__op_ge: res = gtres || eqres; break;
	}
	if (res) {
		return 1;
	}

	// assertion failed
	ea__print_assertion_failed(test_info, file, line);
	printf("  Expected %s (which is %f)\n  to be %s %s (which is %f)\n", sa, a, get_opstr(op), sb, b);
	print_message();
	return 0;
}
