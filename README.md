# ExpectoAssertum

A lightweight, flexible C unit testing framework with support for test groups, filtering, and custom memory allocators.

## Features

- **Simple Test Definition**: Use the `TEST()` macro to define test functions
- **Rich Assertions**: Comprehensive assertion macros for booleans, integers, unsigned integers, pointers, and strings
- **Test Groups**: Organize tests into hierarchical groups
- **Setup/Teardown**: Group-level setup and teardown functions
- **Test Filtering**: Run specific tests using command-line filters with wildcards and negation
- **Custom Memory Allocation**: Optional custom allocator support for embedded systems
- **Zero Dependencies**: Pure C implementation with no external dependencies

## Quick Start

### Basic Example

```c
#include "expectoassertum.h"

// Define a test
TEST(my_first_test) {
    ASSERT_TRUE(1 + 1 == 2);
    ASSERT_INT_EQ(5, 5);
    ASSERT_STRZ_EQ("hello", "hello");
}

// Define another test
TEST(test_pointers) {
    int value = 42;
    int* ptr = &value;
    ASSERT_PTR_NOTNULL(ptr);
    ASSERT_INT_EQ(*ptr, 42);
}

int main(int argc, char** argv) {
    // Create root test group
    ea_group_t* root = ea_create_root();
    
    // Add tests to root group
    ea_test_add(root, my_first_test);
    ea_test_add(root, test_pointers);
    
    // Parse command line filter (optional)
    const char* filter = ea_parse_filter_cmdline(argc, argv);
    
    // Run all tests
    ea_run(root, filter);
    
    // Clean up
    ea_release_group(root);
    
    return 0;
}
```

### Organizing Tests with Groups

```c
#include "expectoassertum.h"

TEST(addition_works) {
    ASSERT_INT_EQ(2 + 2, 4);
}

TEST(subtraction_works) {
    ASSERT_INT_EQ(5 - 3, 2);
}

TEST(multiplication_works) {
    ASSERT_INT_EQ(3 * 4, 12);
}

int main(int argc, char** argv) {
    ea_group_t* root = ea_create_root();
    
    // Create a group for math tests
    ea_group_t* math = ea_group_create(root, "math");
    
    // Add tests to the group
    ea_test_add(math, addition_works);
    ea_test_add(math, subtraction_works);
    ea_test_add(math, multiplication_works);
    
    const char* filter = ea_parse_filter_cmdline(argc, argv);
    ea_run(root, filter);
    ea_release_group(root);
    
    return 0;
}
```

### Setup and Teardown

```c
#include "expectoassertum.h"
#include <stdlib.h>

static int* test_data = NULL;

static void setup(void* opaque) {
    test_data = (int*)malloc(sizeof(int) * 10);
}

static void teardown(void* opaque) {
    free(test_data);
    test_data = NULL;
}

TEST(test_with_setup) {
    ASSERT_PTR_NOTNULL(test_data);
    test_data[0] = 42;
    ASSERT_INT_EQ(test_data[0], 42);
}

int main(int argc, char** argv) {
    ea_group_t* root = ea_create_root();
    ea_group_t* group = ea_group_create(root, "with_lifecycle");
    
    // Set setup and teardown for the group
    ea_group_set_setup(group, setup, NULL);
    ea_group_set_teardown(group, teardown, NULL);
    
    ea_test_add(group, test_with_setup);
    
    const char* filter = ea_parse_filter_cmdline(argc, argv);
    ea_run(root, filter);
    ea_release_group(root);
    
    return 0;
}
```

## Building

### CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Manual Compilation

```bash
# Compile the library
gcc -c src/expectoassertum.c -I include -o expectoassertum.o
ar rcs libexpectoassertum.a expectoassertum.o

# Compile your tests
gcc your_tests.c -I include -L. -lexpectoassertum -o tests

# Run
./tests
```

## Assertion Macros

### Boolean Assertions

| Macro | Description |
|-------|-------------|
| `ASSERT_TRUE(value)` | Assert that value is true |
| `ASSERT_FALSE(value)` | Assert that value is false |
| `ASSERT(condition)` | Alias for `ASSERT_TRUE` |
| `ASSERT_TRUE_M(value, msg, ...)` | Assert with custom message |
| `ASSERT_FALSE_M(value, msg, ...)` | Assert false with custom message |

### Integer Assertions

| Macro | Description |
|-------|-------------|
| `ASSERT_INT_EQ(a, b)` | Assert a == b |
| `ASSERT_INT_NE(a, b)` | Assert a != b |
| `ASSERT_INT_LT(a, b)` | Assert a < b |
| `ASSERT_INT_LE(a, b)` | Assert a <= b |
| `ASSERT_INT_GT(a, b)` | Assert a > b |
| `ASSERT_INT_GE(a, b)` | Assert a >= b |
| `ASSERT_INT_*_M(a, b, msg, ...)` | Variants with custom messages |

### Unsigned Integer Assertions

| Macro | Description |
|-------|-------------|
| `ASSERT_UINT_EQ(a, b)` | Assert a == b (unsigned) |
| `ASSERT_UINT_NE(a, b)` | Assert a != b (unsigned) |
| `ASSERT_UINT_LT(a, b)` | Assert a < b (unsigned) |
| `ASSERT_UINT_LE(a, b)` | Assert a <= b (unsigned) |
| `ASSERT_UINT_GT(a, b)` | Assert a > b (unsigned) |
| `ASSERT_UINT_GE(a, b)` | Assert a >= b (unsigned) |
| `ASSERT_UINT_*_M(a, b, msg, ...)` | Variants with custom messages |

### Pointer Assertions

| Macro | Description |
|-------|-------------|
| `ASSERT_PTR_EQ(a, b)` | Assert pointers are equal |
| `ASSERT_PTR_NE(a, b)` | Assert pointers are not equal |
| `ASSERT_PTR_NULL(ptr)` | Assert pointer is NULL |
| `ASSERT_PTR_NOTNULL(ptr)` | Assert pointer is not NULL |
| `ASSERT_PTR_*_M(a, b, msg, ...)` | Variants with custom messages |

### String Assertions

| Macro | Description |
|-------|-------------|
| `ASSERT_STRZ_EQ(a, b)` | Assert null-terminated strings are equal |
| `ASSERT_STRZ_NE(a, b)` | Assert null-terminated strings are not equal |
| `ASSERT_STRN_EQ(a, b, n)` | Assert first n characters are equal |
| `ASSERT_STRN_NE(a, b, n)` | Assert first n characters are not equal |
| `ASSERT_STR*_M(a, b, msg, ...)` | Variants with custom messages |

## Test Filtering

Run tests with filtering using the `--filter` command line argument:

```bash
# Run all tests
./tests

# Run all tests in the "math" group
./tests --filter=math/*

# Run a specific test
./tests --filter=math/addition_works

# Run multiple filters (comma-separated)
./tests --filter=math/*,string/*

# Exclude tests (use ~ prefix)
./tests --filter=*,~*/slow_test

# Complex filtering
./tests --filter=math/*,~math/slow_test,string/test1
```

Filter patterns:
- `exact/match` - Exact match
- `prefix/*` - Prefix match (wildcard at end)
- `*/suffix` - Suffix match (wildcard at start)
- `~pattern` - Negation (exclude matching tests)

## Custom Memory Allocator

For embedded systems or custom memory management:

```c
void* my_allocator(void* block, int size, void* opaque) {
    if (block) {
        // Deallocate: block is non-NULL, size should be 0
        my_free(block);
        return NULL;
    } else {
        // Allocate: block is NULL, size is the requested size
        return my_malloc(size);
    }
}

int main(int argc, char** argv) {
    ea_group_t* root = ea_create_root_nomalloc(my_allocator, NULL);
    
    // ... add tests ...
    
    ea_run(root, NULL);
    ea_release_group(root);
    
    return 0;
}
```

## API Reference

### Core Functions

```c
// Create root group (with malloc)
ea_group_t* ea_create_root();

// Create root group (with custom allocator)
ea_group_t* ea_create_root_nomalloc(ea_mem_alloc_func_t mem_alloc, void* opaque);

// Clean up and free memory
void ea_release_group(ea_group_t* group);

// Parse command line for --filter argument
const char* ea_parse_filter_cmdline(int argc, char** argv);

// Run tests
void ea_run(ea_group_t* group, const char* filterstring);
```

### Group Management

```c
// Create a test group
ea_group_t* ea_group_create(ea_group_t* parent, const char* name);

// Set setup function (called before each test in the group)
void ea_group_set_setup(ea_group_t* group, ea_group_setup_teardown_func_t setup, void* opaque);

// Set teardown function (called after each test in the group)
void ea_group_set_teardown(ea_group_t* group, ea_group_setup_teardown_func_t teardown, void* opaque);
```

### Test Definition

```c
// Define a test function
TEST(test_name) {
    // test code with assertions
}

// Add a test to a group
ea_test_add(group, test_name);
```

## Examples

See the `example/` directory for complete working examples:

- `example/main.c` - Main test runner
- `example/asserttest/` - Tests demonstrating all assertion types
- `example/grouplifecycle/` - Tests demonstrating setup and teardown

## License

This project is available under your chosen license. See LICENSE file for details.

## Contributing

Contributions are welcome! Please ensure:
- Code follows the existing style
- All tests pass
- New features include tests

## Name Origin

*Expecto Assertum* - A playful combination of Latin-inspired words meaning "I expect assertion" or "expecting assertions", reminiscent of spell incantations while being descriptive of the library's purpose.
