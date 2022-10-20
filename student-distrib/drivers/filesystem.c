#include "filesystem.h"
#include "../types.h"
#include "../lib.h"

static uint32_t * filesys_start;
static uint32_t * filesys_end;

int32_t filesystem_init (uint32_t * file_start, uint32_t * file_end) {
	printf("filesystem initialized at address: 0x%#x\n", (unsigned int)file_start);
	printf("filesystem ends at: 0x%#x\n", (unsigned int)file_end);
	filesys_start = file_start;
	filesys_end = file_end;
	return 0;
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
	return 0;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
	return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
	return 0;
}
