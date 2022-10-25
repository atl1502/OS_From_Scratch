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

    if (strlen(fname) > FILESYSTEM_NAME_MAX)
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
	inode_start = (inode_t *) (file_start + BLOCK_SIZE);
	data_start = (void *) (file_start + BLOCK_SIZE + (inode_count * BLOCK_SIZE));
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
	// 63 is number of files in the system
	for (index = 0; index < 63; index++) {
		char * filename = filesys->direntries[index].filename;
		if (!filename)
			continue;
		else if (!strncmp((char *) fname, filename, 32)) { // 32 bytes being compared
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
	// 24 bytes are reserved
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


	/* Start at offset */
	int cur_block_num = inode_cur->data_block_num[offset >> OFFSET_SHIFT];
	int buffer_offset = 0;

	/* If offset is not 4KB aligned, copy until next 4KB boundary, make length shorter and offset larger */
	if (offset & OFFSET_MASK) {
		memcpy(buf + buffer_offset, data_start + (cur_block_num << OFFSET_SHIFT) + (offset & OFFSET_MASK), BLOCK_SIZE - (offset & OFFSET_MASK));
		buffer_offset += BLOCK_SIZE - (offset & OFFSET_MASK);
		length -= BLOCK_SIZE - (offset & OFFSET_MASK);
		offset += BLOCK_SIZE - (offset & OFFSET_MASK);
	}

	/*
	* Copy bytes until tail end of copying
	*/
	for ( ; length > BLOCK_SIZE; length -= BLOCK_SIZE) {
		cur_block_num = inode_cur->data_block_num[offset >> OFFSET_SHIFT];
		memcpy(buf + buffer_offset, data_start + (cur_block_num << OFFSET_SHIFT), BLOCK_SIZE);
		offset += BLOCK_SIZE;
		buffer_offset += BLOCK_SIZE;
	}

	/*
	* Copy additional bytes that were not on 4KB boundary at the end.
	*/
	if (length) {
		cur_block_num = inode_cur->data_block_num[offset >> OFFSET_SHIFT];
		memcpy(buf + buffer_offset, data_start + (cur_block_num << OFFSET_SHIFT), length);
	}

	return 0;
}

/*
 * dir_read
 * DESCRIPTION: Reads a single file from directory
 * INPUTS:
 * fd - file descriptor that holds desired entry to be read
 * buf - buffer to copy filename to
 * nbytes - number of bytes of filename to copy must be < 32
 * SIDE EFFECTS: Files buf with desired filename with nbytes chars
 * RETURN VALUE: Number of bytes copied
 */
int32_t dir_read(fd_t * fd, void * buf, int32_t nbytes) {
	if (!fd || !buf)
		return -1;
	dentry_t dentry = filesys->direntries[fd->file_position];
	fd->file_position++;
	memcpy(buf, dentry.filename, nbytes);
	return nbytes;
}



/*
 * read_inode_size
 * DESCRIPTION: Reads file of Inode assuming it is on disk
 * INPUTS:
 * inode: Inode to get data from
 * SIDE EFFECTS: None
 * RETURN VALUE: Returns the size of the inode
 */
 int32_t read_inode_size (uint32_t inode){
	 if (inode > inode_count)
		 return -1;
	 return (inode_start+inode)->length;
 }
