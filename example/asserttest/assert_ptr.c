#include "asserttest.h"

TEST(eq_success) {
	int i = 42;
	void* p = &i;
	ASSERT_PTR_EQ_M(p, &i, "This message is never printed");
	ASSERT_PTR_EQ(p, &i);
}

TEST(eq_fail) {
	int i = 42;
	void* p = &i + 1;
	ASSERT_PTR_EQ_M(p, &i, "Pointer equality failed, it is %p", p);
	ASSERT_PTR_EQ(p, &i);
}

TEST(ne_success) {
	int i = 42;
	void* p = &i + 1;
	ASSERT_PTR_NE_M(p, &i, "This message is never printed");
	ASSERT_PTR_NE(p, &i);
}

TEST(ne_fail) {
	int i = 42;
	void* p = &i;
	ASSERT_PTR_NE_M(p, &i, "Pointer inequality failed, it is %p", p);
	ASSERT_PTR_NE(p, &i);
}

TEST(null_success) {
	void* p = 0;
	ASSERT_PTR_NULL_M(p, "This message is never printed");
	ASSERT_PTR_NULL(p);
}

TEST(null_fail) {
	int i = 42;
	void* p = &i;
	ASSERT_PTR_NULL_M(p, "Pointer null check failed, it is %p", p);
	ASSERT_PTR_NULL(p);
}

TEST(notnull_success) {
	int i = 42;
	void* p = &i;
	ASSERT_PTR_NOTNULL_M(p, "This message is never printed");
	ASSERT_PTR_NOTNULL(p);
}

TEST(notnull_fail) {
	void* p = 0;
	ASSERT_PTR_NOTNULL_M(p, "Pointer not-null check failed, it is %p", p);
	ASSERT_PTR_NOTNULL(p);
}

void register_asserttest_ptr(ea_group_t* parent) {
	ea_group_t* group = ea_group_create(parent, "ptr");
	ea_test_add(group, eq_success);
	ea_test_add(group, eq_fail);
	ea_test_add(group, ne_success);
	ea_test_add(group, ne_fail);
	ea_test_add(group, null_success);
	ea_test_add(group, null_fail);
	ea_test_add(group, notnull_success);
	ea_test_add(group, notnull_fail);
}
