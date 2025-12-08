cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(expectoassertum_example VERSION 1.0 LANGUAGES C)

add_executable(expectoassertum_example
	main.c

	asserttest/assert_bool.c
	asserttest/assert_int.c
	asserttest/assert_uint.c
	asserttest/assert_str.c
	asserttest/assert_ptr.c
	asserttest/assert_double.c
	asserttest/asserttest.h

	grouplifecycle/grouplifecycle.c
	grouplifecycle/grouplifecycle.h
)

target_link_libraries(expectoassertum_example
	expectoassertum
)
