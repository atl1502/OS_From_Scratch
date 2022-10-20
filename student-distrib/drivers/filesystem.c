#include "filesystem.h"
#include "../types.h"
#include "../lib.h"

static boot_block_t * filesys;
static void * filesys_end;

int32_t filesystem_init (uint32_t file_start, uint32_t file_end) {
	printf("filesystem initialized at address: 0x%#x\n", (unsigned int)file_start);
	printf("filesystem ends at: 0x%#x\n", (unsigned int)file_end);

	filesys = (boot_block_t *) file_start;
	filesys_end = (void *) file_end;

	printf("filesystem dircount: 0x%#x\n", (unsigned int)filesys->dir_count);
	printf("filesystem inode count: 0x%#x\n", (unsigned int)filesys->inode_count);
	printf("filesystem data count: 0x%#x\n", (unsigned int)filesys->data_count);
	printf("filesystem direntires addr: 0x%#x\n", (unsigned int)filesys->direntries);
	return 0;
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
	uint32_t index = 0;
	for (index = 0; index < 63; index++) {
	}
	return 0;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
	return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
	return 0;
}
