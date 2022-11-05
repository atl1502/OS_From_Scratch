#include "../types.h"
#include "../lib.h"
#include "../fd.h"

#ifndef FILESYSTEM
#define FILESYSTEM

#define BLOCK_SIZE 4096
#define OFFSET_MASK 0xFFF
#define OFFSET_SHIFT 12

#define FILESYSTEM_NAME_MAX 32

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

int32_t file_open(const uint8_t * fname);
int32_t file_close(uint32_t fd);
int32_t file_read (uint32_t fd, void* buf, int32_t nbytes);
int32_t file_write(uint32_t fd, const void* buf, int32_t nbytes);

int32_t dir_open(const uint8_t * dname);
int32_t dir_close(uint32_t fd);
int32_t dir_read (uint32_t fd, void * buf, int32_t nbytes);
int32_t dir_write(uint32_t fd, const void* buf, int32_t nbytes);

int32_t filesystem_init (uint32_t file_start, uint32_t file_end);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, void* buf, uint32_t length);
int32_t read_inode_size (uint32_t inode);

#endif
