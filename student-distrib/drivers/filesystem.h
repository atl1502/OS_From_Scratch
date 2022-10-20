#include "../types.h"
#include "../lib.h"

#ifndef FILESYSTEM
#define FILESYSTEM

typedef struct dentry {
	int8_t filename[32];
	int32_t filetype;
	int32_t inode_num;
	int8_t reserved[24];
} dentry_t;

typedef struct inode {
	int32_t length;
	int32_t data_block_num[1023];
} inode_t;

typedef struct boot_block {
	int32_t dir_count;
	int32_t inode_count;
	int32_t data_count;
	int8_t reserved[52];
	dentry_t direntries[63];
} boot_block_t;


int32_t filesystem_init (uint32_t file_start, uint32_t file_end);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
