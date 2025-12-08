#include "asserttest.h"

TEST(eq_success) {
	int i = 42;
	ASSERT_INT_EQ_M(i, 42, "This message is never printed");
	ASSERT_INT_EQ(i/2, 21);
}

TEST(eq_fail) {
	int i = 42;
	ASSERT_INT_EQ_M(i, 43, "Integer equality failed, it is %d", i);
	ASSERT_INT_EQ(i, 41);
}

TEST(ne_success) {
	int i = 42;
	ASSERT_INT_NE_M(i, 43, "This message is never printed");
	ASSERT_INT_NE(i, 41);
}

TEST(ne_fail) {
	int i = 42;
	ASSERT_INT_NE_M(i, 42, "Integer inequality failed, it is %d", i);
	ASSERT_INT_NE(i, 42);
}

TEST(lt_success) {
	int i = 42;
	ASSERT_INT_LT_M(i, 43, "This message is never printed");
	ASSERT_INT_LT(i, 100);
}

TEST(lt_eq_fail) {
	int i = 42;
	ASSERT_INT_LT_M(i, 42, "Integer less-than failed, it is %d equal", i);
}

TEST(lt_gt_fail) {
	int i = 42;
	ASSERT_INT_LT(i, 41);
}

TEST(le_success) {
	int i = 42;
	ASSERT_INT_LE_M(i, 43, "This message is never printed");
	ASSERT_INT_LE(i, 42);
}

TEST(le_gt_fail) {
	int i = 42;
	ASSERT_INT_LE_M(i, 41, "Integer less-than-or-equal failed, it is %d greater", i);
}

TEST(gt_success) {
	int i = 42;
	ASSERT_INT_GT_M(i, 41, "This message is never printed");
	ASSERT_INT_GT(i, 0);
}

TEST(gt_eq_fail) {
	int i = 42;
	ASSERT_INT_GT_M(i, 42, "Integer greater-than failed, it is %d equal", i);
}

TEST(gt_lt_fail) {
	int i = 42;
	ASSERT_INT_GT(i, 43);
}

TEST(ge_success) {
	int i = 42;
	ASSERT_INT_GE_M(i, 41, "This message is never printed");
	ASSERT_INT_GE(i, 42);
}

TEST(ge_lt_fail) {
	int i = 42;
	ASSERT_INT_GE_M(i, 43, "Integer greater-than-or-equal failed, it is %d less", i);
}

void register_asserttest_int(ea_group_t* parent) {
	ea_group_t* group = ea_group_create(parent, "int");
	ea_test_add(group, eq_success);
	ea_test_add(group, eq_fail);
	ea_test_add(group, ne_success);
	ea_test_add(group, ne_fail);
	ea_test_add(group, lt_success);
	ea_test_add(group, lt_eq_fail);
	ea_test_add(group, lt_gt_fail);
	ea_test_add(group, le_success);
	ea_test_add(group, le_gt_fail);
	ea_test_add(group, gt_success);
	ea_test_add(group, gt_eq_fail);
	ea_test_add(group, gt_lt_fail);
	ea_test_add(group, ge_success);
	ea_test_add(group, ge_lt_fail);
}
