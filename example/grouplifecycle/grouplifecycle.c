#include "grouplifecycle.h"

static int value;

static void setup_func(void* opaque) {
	(void)opaque;
	value = 666;
}
static void teardown_func(void* opaque) {
	(void)opaque;
	value = 123;
}

TEST(setup) {
	ASSERT_INT_EQ(value, 666);
}

TEST(torndown) {
	ASSERT_INT_EQ(value, 123);
}

void register_grouplifecycle(ea_group_t* parent) {
	ea_group_t* main = ea_group_create(parent, "grouplifecycle");
	ea_group_t* withlifecycle = ea_group_create(main, "withlifecycle");
	ea_group_set_setup(withlifecycle, setup_func, 0);
	ea_group_set_teardown(withlifecycle, teardown_func, 0);
	ea_test_add(withlifecycle, setup);
	ea_group_t* nolifecycle = ea_group_create(main, "nolifecycle");
	ea_test_add(nolifecycle, torndown);
}
