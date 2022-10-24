#include "filesystem.h"
#include "../types.h"
#include "../lib.h"
#include "../fd.h"

static boot_block_t * filesys;
static inode_t * inode_start;
static void * data_start;
static uint32_t inode_count;
static void * filesys_end;

/*
 * file_open
 * DESCRIPTION: Opens the file by searching for dentry from filename
 * and copying info to descriptor
 * INPUTS:
 * fname: Name of file in string
 * fd: File descriptor to be modified
 * SIDE EFFECTS: Changes file descriptor to have inode of desired file
 * RETURN VALUE: 0 or -1 iff NULL input
 */
int32_t file_open(const uint8_t * fname, fd_t * fd) {
	if (!fname || !fd || !(*fname))
		return -1;

	dentry_t dentry;
	read_dentry_by_name (fname, &dentry);
	fd->table_pointer = 0;
	fd->inode_num = dentry.inode_num;
	fd->file_position = 0;
	fd->flags = 0;
	return 0;
}

/*
 * file_close
 * DESCRIPTION: Opens the file by doing nothing
 * INPUTS:
 * fd: File descriptor to be modified (ignored)
 * SIDE EFFECTS: NONE
 * RETURN VALUE: 0
 */
int32_t file_close(fd_t * fd) {
	return 0;
}

/*
 * file_read
 * DESCRIPTION: Reads file of Inode assuming it is on disk
 * INPUTS:
 * fd: File descriptor to be modified
 * buf: buffer to write mem to
 * nbytes: number of bytes to copy
 * SIDE EFFECTS: Fills buffer with nbytes of file starting at last position
 * RETURN VALUE: 0 or -1 iff NULL input
 */
int32_t file_read (fd_t * fd, void* buf, int32_t nbytes) {
	if (!fd || !buf)
		return -1;
	read_data (fd->inode_num, fd->file_position, buf, nbytes);
	fd->file_position += nbytes;
	return 0;
}

/*
 * file_close
 * DESCRIPTION: "Fails" to write (RO filesys)
 * INPUTS:
 * fd: File descriptor to be modified (ignored)
 * SIDE EFFECTS: NONE
 * RETURN VALUE: -1
 */
int32_t file_write(fd_t * fd) {
	return -1;
}

/*
 * filesystem_init
 * DESCRIPTION: Intializes global variables which represent filesystem
 * INPUTS:
 * file_start: start of filesystem in memory
 * file_end: enf of filesystem in memory
 * SIDE EFFECTS: Sets filesystem driver global variables
 * RETURN VALUE: 0 or -1 iff NULL input
 */
int32_t filesystem_init (uint32_t file_start, uint32_t file_end) {

	filesys = (boot_block_t *) file_start;
	inode_count = (unsigned int)filesys->inode_count;
	inode_start = (inode_t *) (file_start + 4096);
	data_start = (void *) (file_start + 4096 + (inode_count * 4096));
	filesys_end = (void *) file_end;

	if (!file_start || !file_end)
		return -1;

	return 0;
}

/*
 * read_dentry_by_name
 * DESCRIPTION: given string returns dentry that has the inode
 * INPUTS:
 * fname: string of filename
 * dentry: pointer to dentry to modify with inode
 * SIDE EFFECTS: Modifies dentry with new inode
 * RETURN VALUE: 0 or -1 iff NULL input
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry) {
	if (!fname || !dentry)
		return -1;
	uint32_t index;
	unsigned char found = 0;
	for (index = 0; index < 63; index++) {
		char * filename = filesys->direntries[index].filename;
		if (!filename)
			continue;
		else if (!strncmp((char *) fname, filename, 32)) {
			found = 1;
			break;
		}
	}
	if (!found) {
		printf("Cannot find file by that name!\n");
		return -1;
	}
	printf("Index is: %d\n", index);
	read_dentry_by_index(index, dentry);
	return 0;
}

/*
 * read_dentry_by_index
 * DESCRIPTION: given index modifies dentry with infomation on file
 * INPUTS: 
 * index: directory index
 * dentry: pointer to dentry to modify with inode
 * SIDE EFFECTS: Modifies dentry with new inode
 * RETURN VALUE: 0 or -1 iff NULL input
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry) {

	if (!dentry)
		return -1;

	/* Get direntry from directory */
	const dentry_t entry = filesys->direntries[index];

	/* Copy the filesys dentry into the desired dentry */
	strcpy(dentry->filename, entry.filename);
	dentry->filetype = entry.filetype;
	dentry->inode_num = entry.inode_num;
	memcpy(dentry->reserved, entry.reserved, 24);

	return 0;
}

/*
 * read_data
 * DESCRIPTION: Reads file of Inode assuming it is on disk
 * INPUTS:
 * inode: Inode to get data from
 * buf: buffer to copy from file to
 * nbytes: number of bytes to copy
 * SIDE EFFECTS: Fills buffer with nbytes of file starting at last position
 * RETURN VALUE: 0 or -1 iff NULL input
 */
int32_t read_data (uint32_t inode, uint32_t offset, void* buf, uint32_t length) {

	if (!buf) {
		printf("Buffer NULL\n");
		return -1;
	}
	if (inode_count < inode) {
		printf("Trying to read illegal inode\n");
		return -1;
	}

	inode_t * inode_cur = inode + inode_start;
	if (offset + length > inode_cur->length) {
		printf("Trying to read past file\n");
		return -1;
	}


	/* Start at offset / 4096 for blocks */
	int cur_block_num = inode_cur->data_block_num[offset >> 12];
	int buffer_offset = 0;

	/* If offset is not 4KB aligned, copy until next 4KB boundary, make length shorter and offset larger */
	if (offset & 0xFFF) {
		memcpy(buf + buffer_offset, data_start + (cur_block_num << 12) + (offset & 0xFFF), 4096 - (offset & 0xFFF));
		buffer_offset += 4096 - (offset & 0xFFF);
		length -= 4096 - (offset & 0xFFF);
		offset += 4096 - (offset & 0xFFF);
	}

	/*
	* Copy bytes until tail end of copying
	*/
	for ( ; length > 4096; length -= 4096) {
		cur_block_num = inode_cur->data_block_num[offset >> 12];
		memcpy(buf + buffer_offset, data_start + (cur_block_num << 12), 4096);
		offset += 4096;
		buffer_offset += 4096;
	}

	/*
	* Copy additional bytes that were not on 4KB boundary at the end.
	*/
	if (length) {
		cur_block_num = inode_cur->data_block_num[offset >> 12];
		memcpy(buf + buffer_offset, data_start + (cur_block_num << 12), length);
	}

	return 0;
}