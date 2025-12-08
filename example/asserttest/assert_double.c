#include "asserttest.h"

TEST(eq_success) {
	double d = 3.14159;
	ASSERT_DOUBLE_EQ_M(d, 3.14159,"This message is never printed");
	ASSERT_DOUBLE_EQ(d/2, 1.570795);
}

TEST(eq_fail) {
	double d = 3.14159;
	ASSERT_DOUBLE_EQ_M(d, 3.14, "Double equality failed, it is %f", d);
	ASSERT_DOUBLE_EQ(d, 3.15);
}

TEST(eq_tolerance_success) {
	double d = 3.14159;
	ASSERT_DOUBLE_EQ_T_M(d, 3.1416, 1e-4, ea_default_double_tolerance_abs, "This message is never printed");
	ASSERT_DOUBLE_EQ_T(d, 3.1415, 1e-4, ea_default_double_tolerance_abs);
}

TEST(eq_tolerance_fail) {
	double d = 3.14159;
	ASSERT_DOUBLE_EQ_T_M(d, 3.1416, 1e-6, ea_default_double_tolerance_abs, "Double equality with epsilon failed, it is %f", d);
	ASSERT_DOUBLE_EQ_T(d, 3.1415, 1e-6, ea_default_double_tolerance_abs);
}

TEST(ne_success) {
	double d = 3.14159;
	ASSERT_DOUBLE_NE_M(d, 3.14, "This message is never printed");
	ASSERT_DOUBLE_NE(d, 3.15);
}

TEST(ne_fail) {
	double d = 3.14159;
	ASSERT_DOUBLE_NE_M(d, 3.14159, "Double inequality failed, it is %f", d);
	ASSERT_DOUBLE_NE(d, 3.1416);
}

TEST(lt_success) {
	double d = 3.14159;
	ASSERT_DOUBLE_LT_M(d, 3.15, "This message is never printed");
	ASSERT_DOUBLE_LT(d, 4.0);
}

TEST(lt_fail) {
	double d = 3.14159;
	ASSERT_DOUBLE_LT(d, 3.0);
}

TEST(le_success) {
	double d = 3.14159;
	ASSERT_DOUBLE_LE_M(d, 3.15, "This message is never printed");
	ASSERT_DOUBLE_LE(d, 3.14159);
}

TEST(le_fail) {
	double d = 3.14159;
	ASSERT_DOUBLE_LE_M(d, 3.1415, "Double less-than-or-equal failed, it is %f greater", d);
}

TEST(gt_success) {
	double d = 3.14159;
	ASSERT_DOUBLE_GT_M(d, 3.0, "This message is never printed");
	ASSERT_DOUBLE_GT(d, 3.1415);
}

TEST(gt_fail) {
	double d = 3.14159;
	ASSERT_DOUBLE_GT(d, 4.0);
}

TEST(ge_success) {
	double d = 3.14159;
	ASSERT_DOUBLE_GE_M(d, 3.0, "This message is never printed");
	ASSERT_DOUBLE_GE(d, 3.14159);
}

TEST(ge_fail) {
	double d = 3.14159;
	ASSERT_DOUBLE_GE_M(d, 3.1416, "Double greater-than-or-equal failed, it is %f less", d);
}

void register_asserttest_double(ea_group_t* parent) {
	ea_group_t* group = ea_group_create(parent, "double");
	ea_test_add(group, eq_success);
	ea_test_add(group, eq_fail);
	ea_test_add(group, eq_tolerance_success);
	ea_test_add(group, eq_tolerance_fail);
	ea_test_add(group, ne_success);
	ea_test_add(group, ne_fail);
	ea_test_add(group, lt_success);
	ea_test_add(group, lt_fail);
	ea_test_add(group, le_success);
	ea_test_add(group, le_fail);
	ea_test_add(group, gt_success);
	ea_test_add(group, gt_fail);
	ea_test_add(group, ge_success);
	ea_test_add(group, ge_fail);
}
