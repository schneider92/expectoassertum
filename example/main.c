#include "asserttest/asserttest.h"
#include "grouplifecycle/grouplifecycle.h"

#include <stdio.h>
#include <stdlib.h>

static int alloc_block_count = 0;
static unsigned long long alloc_size_total = 0;
static void* tracking_mem_alloc(void* block, int size, void* opaque) {
	union block_header_u {
		double _alignment;
		unsigned long long size;
	};
	(void)opaque;
	if (block) {
		union block_header_u* header = ((union block_header_u*)block) - 1;
		unsigned long long block_size = header->size;
		// deallocate
		alloc_block_count--;
		alloc_size_total -= block_size;
		return NULL;
	}
	else {
		// allocate
		unsigned long long block_size = size + sizeof(union block_header_u);
		union block_header_u* new_block = (union block_header_u*)malloc(block_size);
		new_block->size = block_size;
		alloc_block_count++;
		alloc_size_total += block_size;
		return new_block + 1;
	}
}

int main(int argc, char** argv) {
	ea_group_t* root = ea_create_root_nomalloc(tracking_mem_alloc, NULL);
	register_grouplifecycle(root);
	register_asserttest_all(root);
	const char* filterstring = ea_parse_filter_cmdline(argc, argv);
	ea_run(root, filterstring);
	ea_release_group(root);
}
