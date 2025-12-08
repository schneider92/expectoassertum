#ifndef EXPECTOASSERTUM_H_INCLUDED
#define EXPECTOASSERTUM_H_INCLUDED

/**
 * @brief Opaque type representing a group in the test framework.
 */
typedef struct ea_group_s ea_group_t;

/**
 * @brief Create the root group for the test framework.
 */
ea_group_t* ea_create_root();

/**
 * @brief Memory allocation function type.
 * @param block Pointer to the memory block to deallocate or NULL for allocation.
 * @param size Size of memory to allocate or 0 for deallocation.
 * @param opaque User-defined pointer passed through.
 * @return Pointer to allocated memory.
 */
typedef void*(*ea_mem_alloc_func_t)(void* block, int size, void* opaque);

/**
 * @brief Create the root group for the test framework without using dynamic memory allocation.
 * @param mem_alloc Custom memory allocation function.
 * @param opaque User-defined pointer passed to the memory allocation function.
 * @return Pointer to the root group.
 */
ea_group_t* ea_create_root_nomalloc(ea_mem_alloc_func_t mem_alloc, void* opaque);

/**
 * @brief Clean up the group and free associated memory. Usually called on the
 * root group, but can be called on any group. Must be called when done with
 * the tests to avoid memory leaks.
 */
void ea_release_group(ea_group_t* group);

/**
 * @brief Parse command line arguments for test filtering.
 * @details Looks for a --filter=<filterstring> argument and returns the filter
 * string, or NULL if not found.
 */
const char* ea_parse_filter_cmdline(int argc, char** argv);

/**
 * @brief Run the test framework starting from the given group (usually the root).
 * @param filterstring The filterstring contains one or more comma-separated
 * filters. Each filter can be a full matcher (e.g "module1/test1"), a prefix
 * matcher (e.g. "module1/test*") or a suffix matcher (e.g. "*test1"). Any
 * filter can be prefixed with a tilde '~' to indicate negation.
 */
void ea_run(ea_group_t* group, const char* filterstring);

/**
 * @brief Create a test group.
 * @param parent Pointer to the parent group.
 * @param name Name of the group.
 * @return Pointer to the created group.
 */
ea_group_t* ea_group_create(ea_group_t* parent, const char* name);
/**
 * @brief Set setup and teardown functions for a test group.
 * @param group Pointer to the group.
 * @param setup Function to call before each test in the group.
 * @param teardown Function to call after each test in the group.
 */
typedef void(*ea_group_setup_teardown_func_t)(void* opaque);

/**
 * @brief Set setup function for a group.
 */
void ea_group_set_setup(ea_group_t* group, ea_group_setup_teardown_func_t setup, void* opaque);

/**
 * @brief Set teardown function for a group.
 */
void ea_group_set_teardown(ea_group_t* group, ea_group_setup_teardown_func_t teardown, void* opaque);

typedef struct ea__test_info_s ea__test_info_t;

#define ea__test_func_name(name) ea__testfunc_ ## name

/**
 * @brief Macro to define a test function.
 */
#define TEST(name) static void ea__test_func_name(name)(ea__test_info_t* ea__current_test_info)

/**
 * @brief Macro to add a test to a group.
 */
#define ea_test_add(group, test) ea__test_add(group, ea__test_func_name(test), #test)

typedef void(*ea__test_func_t)(ea__test_info_t*);
void ea__test_add(ea_group_t* group, ea__test_func_t test_func, const char* test_name);

// assertions
void ea__print_assertion_failed(ea__test_info_t* test_info, const char* file, int line);

enum {
	ea__op_eq,
	ea__op_ne,
	ea__op_lt,
	ea__op_le,
	ea__op_gt,
	ea__op_ge,
};

int ea__assert_bool_check(ea__test_info_t* test_info, int actual, const char* actual_str, int exp, const char* file, int line, const char* msg, ...);
#define ea__assert_bool(actual, exp, msg, ...) if (!ea__assert_bool_check(ea__current_test_info, !!actual, #actual, exp, __FILE__, __LINE__, msg, ##__VA_ARGS__)) return;
#define ASSERT_TRUE_M(value, msg, ...) ea__assert_bool(value, 1, msg, ##__VA_ARGS__)
#define ASSERT_FALSE_M(value, msg, ...) ea__assert_bool(value, 0, msg, ##__VA_ARGS__)
#define ASSERT_TRUE(value) ASSERT_TRUE_M(value, 0)
#define ASSERT_FALSE(value) ASSERT_FALSE_M(value, 0)
#define ASSERT_M(condition, msg, ...) ASSERT_TRUE_M(condition, msg, ##__VA_ARGS__)
#define ASSERT(condition) ASSERT_M(condition, 0)

int ea__assert_int_check(ea__test_info_t* test_info, long long a, long long b, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...);
#define ea__assert_int(a, b, op, msg, ...) if (!ea__assert_int_check(ea__current_test_info, a, b, op, #a, #b, __FILE__, __LINE__, msg, ##__VA_ARGS__)) return;
#define ASSERT_INT_EQ_M(a, b, msg, ...) ea__assert_int(a, b, ea__op_eq, msg, ##__VA_ARGS__)
#define ASSERT_INT_NE_M(a, b, msg, ...) ea__assert_int(a, b, ea__op_ne, msg, ##__VA_ARGS__)
#define ASSERT_INT_LT_M(a, b, msg, ...) ea__assert_int(a, b, ea__op_lt, msg, ##__VA_ARGS__)
#define ASSERT_INT_LE_M(a, b, msg, ...) ea__assert_int(a, b, ea__op_le, msg, ##__VA_ARGS__)
#define ASSERT_INT_GT_M(a, b, msg, ...) ea__assert_int(a, b, ea__op_gt, msg, ##__VA_ARGS__)
#define ASSERT_INT_GE_M(a, b, msg, ...) ea__assert_int(a, b, ea__op_ge, msg, ##__VA_ARGS__)
#define ASSERT_INT_EQ(a, b) ASSERT_INT_EQ_M(a, b, 0)
#define ASSERT_INT_NE(a, b) ASSERT_INT_NE_M(a, b, 0)
#define ASSERT_INT_LT(a, b) ASSERT_INT_LT_M(a, b, 0)
#define ASSERT_INT_LE(a, b) ASSERT_INT_LE_M(a, b, 0)
#define ASSERT_INT_GT(a, b) ASSERT_INT_GT_M(a, b, 0)
#define ASSERT_INT_GE(a, b) ASSERT_INT_GE_M(a, b, 0)

int ea__assert_uint_check(ea__test_info_t* test_info, unsigned long long a, unsigned long long b, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...);
#define ea__assert_uint(a, b, op, msg, ...) if (!ea__assert_uint_check(ea__current_test_info, a, b, op, #a, #b, __FILE__, __LINE__, msg, ##__VA_ARGS__)) return;
#define ASSERT_UINT_EQ_M(a, b, msg, ...) ea__assert_uint(a, b, ea__op_eq, msg, ##__VA_ARGS__)
#define ASSERT_UINT_NE_M(a, b, msg, ...) ea__assert_uint(a, b, ea__op_ne, msg, ##__VA_ARGS__)
#define ASSERT_UINT_LT_M(a, b, msg, ...) ea__assert_uint(a, b, ea__op_lt, msg, ##__VA_ARGS__)
#define ASSERT_UINT_LE_M(a, b, msg, ...) ea__assert_uint(a, b, ea__op_le, msg, ##__VA_ARGS__)
#define ASSERT_UINT_GT_M(a, b, msg, ...) ea__assert_uint(a, b, ea__op_gt, msg, ##__VA_ARGS__)
#define ASSERT_UINT_GE_M(a, b, msg, ...) ea__assert_uint(a, b, ea__op_ge, msg, ##__VA_ARGS__)
#define ASSERT_UINT_EQ(a, b) ASSERT_UINT_EQ_M(a, b, 0)
#define ASSERT_UINT_NE(a, b) ASSERT_UINT_NE_M(a, b, 0)
#define ASSERT_UINT_LT(a, b) ASSERT_UINT_LT_M(a, b, 0)
#define ASSERT_UINT_LE(a, b) ASSERT_UINT_LE_M(a, b, 0)
#define ASSERT_UINT_GT(a, b) ASSERT_UINT_GT_M(a, b, 0)
#define ASSERT_UINT_GE(a, b) ASSERT_UINT_GE_M(a, b, 0)

int ea__assert_ptr_check(ea__test_info_t* test_info, const void* a, const void* b, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...);
#define ea__assert_ptr(a, b, op, msg, ...) if (!ea__assert_ptr_check(ea__current_test_info, a, b, op, #a, #b, __FILE__, __LINE__, msg, ##__VA_ARGS__)) return;
#define ASSERT_PTR_EQ_M(a, b, msg, ...) ea__assert_ptr(a, b, ea__op_eq, msg, ##__VA_ARGS__)
#define ASSERT_PTR_NE_M(a, b, msg, ...) ea__assert_ptr(a, b, ea__op_ne, msg, ##__VA_ARGS__)
#define ASSERT_PTR_EQ(a, b) ASSERT_PTR_EQ_M(a, b, 0)
#define ASSERT_PTR_NE(a, b) ASSERT_PTR_NE_M(a, b, 0)

int ea__assert_ptr_null_check(ea__test_info_t* test_info, const void* a, int is_null, const char* sa, const char* file, int line, const char* msg, ...);
#define ea__assert_ptr_null(a, is_null, msg, ...) if (!ea__assert_ptr_null_check(ea__current_test_info, a, is_null, #a, __FILE__, __LINE__, msg, ##__VA_ARGS__)) return;
#define ASSERT_PTR_NULL_M(a, msg, ...) ea__assert_ptr_null(a, 1, msg, ##__VA_ARGS__)
#define ASSERT_PTR_NOTNULL_M(a, msg, ...) ea__assert_ptr_null(a, 0, msg, ##__VA_ARGS__)
#define ASSERT_PTR_NULL(a) ASSERT_PTR_NULL_M(a, 0)
#define ASSERT_PTR_NOTNULL(a) ASSERT_PTR_NOTNULL_M(a, 0)

int ea__assert_str_check(ea__test_info_t* test_info, const char* a, const char* b, int size, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...);
#define ea__assert_strz(a, b, op, msg, ...) if (!ea__assert_str_check(ea__current_test_info, a, b, -1, op, #a, #b, __FILE__, __LINE__, msg, ##__VA_ARGS__)) return;
#define ASSERT_STRZ_EQ_M(a, b, msg, ...) ea__assert_strz(a, b, ea__op_eq, msg, ##__VA_ARGS__)
#define ASSERT_STRZ_NE_M(a, b, msg, ...) ea__assert_strz(a, b, ea__op_ne, msg, ##__VA_ARGS__)
#define ASSERT_STRZ_EQ(a, b) ASSERT_STRZ_EQ_M(a, b, 0)
#define ASSERT_STRZ_NE(a, b) ASSERT_STRZ_NE_M(a, b, 0)
#define ea__assert_strn(a, b, size, op, msg, ...) if (!ea__assert_str_check(ea__current_test_info, a, b, size, op, #a, #b, __FILE__, __LINE__, msg, ##__VA_ARGS__)) return;
#define ASSERT_STRN_EQ_M(a, b, size, msg, ...) ea__assert_strn(a, b, size, ea__op_eq, msg, ##__VA_ARGS__)
#define ASSERT_STRN_NE_M(a, b, size, msg, ...) ea__assert_strn(a, b, size, ea__op_ne, msg, ##__VA_ARGS__)
#define ASSERT_STRN_EQ(a, b, size) ASSERT_STRN_EQ_M(a, b, size, 0)
#define ASSERT_STRN_NE(a, b, size) ASSERT_STRN_NE_M(a, b, size, 0)

int ea__assert_double_check(ea__test_info_t* test_info, double a, double b, double reltol, double abstol, int op, const char* sa, const char* sb, const char* file, int line, const char* msg, ...);
#define ea__assert_double(a, b, relative_tolerance, absolute_tolerance, op, msg, ...) if (!ea__assert_double_check(ea__current_test_info, a, b, relative_tolerance, absolute_tolerance, op, #a, #b, __FILE__, __LINE__, msg, ##__VA_ARGS__)) return;
#define ASSERT_DOUBLE_EQ_T_M(a, b, relative_tolerance, absolute_tolerance, msg, ...) ea__assert_double(a, b, relative_tolerance, absolute_tolerance, ea__op_eq, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_NE_T_M(a, b, relative_tolerance, absolute_tolerance, msg, ...) ea__assert_double(a, b, relative_tolerance, absolute_tolerance, ea__op_ne, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_LT_T_M(a, b, relative_tolerance, absolute_tolerance, msg, ...) ea__assert_double(a, b, relative_tolerance, absolute_tolerance, ea__op_lt, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_LE_T_M(a, b, relative_tolerance, absolute_tolerance, msg, ...) ea__assert_double(a, b, relative_tolerance, absolute_tolerance, ea__op_le, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_GT_T_M(a, b, relative_tolerance, absolute_tolerance, msg, ...) ea__assert_double(a, b, relative_tolerance, absolute_tolerance, ea__op_gt, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_GE_T_M(a, b, relative_tolerance, absolute_tolerance, msg, ...) ea__assert_double(a, b, relative_tolerance, absolute_tolerance, ea__op_ge, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_EQ_T(a, b, relative_tolerance, absolute_tolerance) ASSERT_DOUBLE_EQ_T_M(a, b, relative_tolerance, absolute_tolerance, 0)
#define ASSERT_DOUBLE_NE_T(a, b, relative_tolerance, absolute_tolerance) ASSERT_DOUBLE_NE_T_M(a, b, relative_tolerance, absolute_tolerance, 0)
#define ASSERT_DOUBLE_LT_T(a, b, relative_tolerance, absolute_tolerance) ASSERT_DOUBLE_LT_T_M(a, b, relative_tolerance, absolute_tolerance, 0)
#define ASSERT_DOUBLE_LE_T(a, b, relative_tolerance, absolute_tolerance) ASSERT_DOUBLE_LE_T_M(a, b, relative_tolerance, absolute_tolerance, 0)
#define ASSERT_DOUBLE_GT_T(a, b, relative_tolerance, absolute_tolerance) ASSERT_DOUBLE_GT_T_M(a, b, relative_tolerance, absolute_tolerance, 0)
#define ASSERT_DOUBLE_GE_T(a, b, relative_tolerance, absolute_tolerance) ASSERT_DOUBLE_GE_T_M(a, b, relative_tolerance, absolute_tolerance, 0)
#define ea_default_double_tolerance_rel 1e-10
#define ea_default_double_tolerance_abs 1e-12
#define ASSERT_DOUBLE_EQ_M(a, b, msg, ...) ASSERT_DOUBLE_EQ_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_NE_M(a, b, msg, ...) ASSERT_DOUBLE_NE_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_LT_M(a, b, msg, ...) ASSERT_DOUBLE_LT_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_LE_M(a, b, msg, ...) ASSERT_DOUBLE_LE_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_GT_M(a, b, msg, ...) ASSERT_DOUBLE_GT_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_GE_M(a, b, msg, ...) ASSERT_DOUBLE_GE_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, msg, ##__VA_ARGS__)
#define ASSERT_DOUBLE_EQ(a, b) ASSERT_DOUBLE_EQ_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, 0)
#define ASSERT_DOUBLE_NE(a, b) ASSERT_DOUBLE_NE_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, 0)
#define ASSERT_DOUBLE_LT(a, b) ASSERT_DOUBLE_LT_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, 0)
#define ASSERT_DOUBLE_LE(a, b) ASSERT_DOUBLE_LE_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, 0)
#define ASSERT_DOUBLE_GT(a, b) ASSERT_DOUBLE_GT_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, 0)
#define ASSERT_DOUBLE_GE(a, b) ASSERT_DOUBLE_GE_T_M(a, b, ea_default_double_tolerance_rel, ea_default_double_tolerance_abs, 0)

#endif // EXPECTOASSERTUM_H_INCLUDED
