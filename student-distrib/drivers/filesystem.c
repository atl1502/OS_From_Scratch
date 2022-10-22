#include "filesystem.h"
#include "../types.h"
#include "../lib.h"

static boot_block_t * filesys;
static inode_t * inodes;
static unsigned char * data_blocks;
static uint32_t inode_count;
static void * filesys_end;

int32_t filesystem_init (uint32_t file_start, uint32_t file_end) {
	if (!file_start || !file_end)
		return -1;

	inode_count = (unsigned int)filesys->inode_count;

	filesys = (boot_block_t *) file_start;
	inodes = (inode_t *) (file_start + 4096);
	data_blocks = (unsigned char *) (file_start + 4096 + (inode_count * 4096));
	filesys_end = (void *) file_end;


	printf("filesystem dircount: 0x%#x\n", (unsigned int)filesys->dir_count);
	printf("filesystem inode count: 0x%#x\n", inode_count);
	printf("filesystem data count: 0x%#x\n", (unsigned int)filesys->data_count);
	return 0;
}

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
	if (!fname || !dentry)
		return -1;
	uint32_t index = 0;
	for (index = 0; index < 63; index++) {
		char * filename = filesys->direntries[index].filename;
		if (!filename)
			return -1;
		else if (!strncmp(fname, filename, 32))
			break;
	}
	read_dentry_by_index(index, dentry);
	return 0;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {
	const dentry_t entry = filesys->direntries[index];
	strcpy(dentry->filename, entry.filename);
	dentry->filetype = entry.filetype;
	dentry->inode_num = entry.inode_num;
	memcpy(dentry->reserved, entry.reserved, 24);
	return 0;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length) {
	if (!buf) {
		printf("Buffer NULL\n");
		return -1;
	}
	if (inode_count < inode) {
		printf("Trying to read illegal inode\n");
		return -1;
	}

	inode_t * inode_cur = inode + inodes;
	if (offset + length > inode_cur->length) {
		printf("Trying to read past file\n");
		return -1;
	}
	
	int num_data_blocks = inode_cur->length >> 12;
	int block_index;
	int data_index;
	int block_num;

	for (block_index = 0; block_index < num_data_blocks; block_index++) {
		block_num = inode_cur->data_block_num[block_index];
		unsigned char * data_block_cur = data_blocks + (block_num * 4096);
		for (data_index = 0; data_index < length && data_index < 4096; data_index++) {
			printf("%c", data_block_cur[data_index]);
		}
		length -= 4096;
	}
	return 0;
}
