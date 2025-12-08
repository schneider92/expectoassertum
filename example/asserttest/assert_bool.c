#include "asserttest.h"

TEST(true_pass) {
	ASSERT_TRUE_M(1, "This message is never printed");
	ASSERT_TRUE(1);
	ASSERT(1);
	ASSERT_M(1, "This message is never printed");
}

TEST(true_fail) {
	ASSERT_TRUE_M(0, "Bool assertion failed: %d", 0);
	ASSERT_TRUE(0);
	ASSERT(0);
}

TEST(false_pass) {
	ASSERT_FALSE_M(0, "This message is never printed");
	ASSERT_FALSE(0);
}

TEST(false_fail) {
	ASSERT_FALSE_M(1, "Bool assertion failed: %d", 1);
	ASSERT_FALSE(1);
}

void register_asserttest_bool(ea_group_t* parent) {
	ea_group_t* group = ea_group_create(parent, "bool");
	ea_test_add(group, true_pass);
	ea_test_add(group, true_fail);
	ea_test_add(group, false_pass);
	ea_test_add(group, false_fail);
}
