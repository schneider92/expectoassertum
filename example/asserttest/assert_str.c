#include <stdlib.h>
#include "asserttest.h"

TEST(z_eq_success) {
	const char* s = "hello";
	ASSERT_STRZ_EQ_M(s, "hello", "This message is never printed");
	ASSERT_STRZ_EQ(s, "hello");
}

TEST(z_eq_fail) {
	const char* s = "hello";
	ASSERT_STRZ_EQ_M(s, "hell", "String equality failed, it is \"%s\"", s);
	ASSERT_STRZ_EQ(s, "helloo");
}

TEST(z_ne_success) {
	const char* s = "hello";
	ASSERT_STRZ_NE_M(s, "hell", "This message is never printed");
	ASSERT_STRZ_NE(s, "helloo");
}

TEST(z_ne_fail) {
	const char* s = "hello";
	ASSERT_STRZ_NE_M(s, "hello", "String inequality failed, it is \"%s\"", s);
	ASSERT_STRZ_NE(s, "hello");
}

TEST(n_eq_success) {
	const char* s = "hello";
	ASSERT_STRN_EQ_M(s, "helicopter", 3, "This message is never printed");
	ASSERT_STRN_EQ(s, "helicopter", 3);
}

TEST(n_eq_fail) {
	const char* s = "hello";
	ASSERT_STRN_EQ_M(s, "helicopter", 4, "String equality failed for first 4 chars, it is \"%s\"", s);
	ASSERT_STRN_EQ(s, "helicopter", 4);
}

TEST(n_ne_success) {
	const char* s = "hello";
	ASSERT_STRN_NE_M(s, "helicopter", 4, "This message is never printed");
	ASSERT_STRN_NE(s, "helicopter", 4);
}

TEST(n_ne_fail) {
	const char* s = "hello";
	ASSERT_STRN_NE_M(s, "helxyz", 3, "String inequality failed for first 3 chars, it is \"%s\"", s);
	ASSERT_STRN_NE(s, "helxyz", 3);
}

void register_asserttest_str(ea_group_t* parent) {
	ea_group_t* group = ea_group_create(parent, "str");
	ea_test_add(group, z_eq_success);
	ea_test_add(group, z_eq_fail);
	ea_test_add(group, z_ne_success);
	ea_test_add(group, z_ne_fail);
	ea_test_add(group, n_eq_success);
	ea_test_add(group, n_eq_fail);
	ea_test_add(group, n_ne_success);
	ea_test_add(group, n_ne_fail);
}
