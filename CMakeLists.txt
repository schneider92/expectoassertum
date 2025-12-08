cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(expectoassertum VERSION 1.0 LANGUAGES C)

add_library(expectoassertum STATIC
	include/expectoassertum.h
	src/expectoassertum.c
)

target_compile_options(expectoassertum PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-Werror -Wall -Wextra -Wunused-variable -Wunused-function>
    $<$<C_COMPILER_ID:MSVC>:/WX /W4>
	$<$<C_COMPILER_ID:MSVC>:/external:W0 /external:anglebrackets> # don't trigger errors from external headers
)

target_include_directories(expectoassertum
	PUBLIC include
	PRIVATE src
)

add_subdirectory(example)
