#include "../types.h"
#include "../lib.h"

#ifndef FILESYSTEM
#define FILESYSTEM


typedef struct dentry {

} dentry_t;

int32_t filesystem_init (uint32_t * file_start, uint32_t * file_end);
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

#endif
