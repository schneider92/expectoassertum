#include "expectoassertum.h"

void register_asserttest_bool(ea_group_t* parent);
void register_asserttest_int(ea_group_t* parent);
void register_asserttest_uint(ea_group_t* parent);
void register_asserttest_str(ea_group_t* parent);
void register_asserttest_ptr(ea_group_t* parent);
void register_asserttest_double(ea_group_t* parent);

static void register_asserttest_all(ea_group_t* parent) {
	ea_group_t* group = ea_group_create(parent, "asserts");
	register_asserttest_bool(group);
	register_asserttest_int(group);
	register_asserttest_uint(group);
	register_asserttest_str(group);
	register_asserttest_ptr(group);
	register_asserttest_double(group);
}
