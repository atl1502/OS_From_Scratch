#include "filesystem.h"
#include "../types.h"
#include "../lib.h"
#include "../fd.h"
#include "../pcb.h"
#include "../x86_desc.h"

static boot_block_t * filesys;
static inode_t * inode_start;
static void * data_start;
static uint32_t inode_count;
static void * filesys_end;

/*
 * file_open
 * DESCRIPTION: Does nothing
 * INPUTS:
 * fname: Name of file in string (ignored)
 * RETURN VALUE: 0
 */
int32_t file_open(const uint8_t * fname) {
	return 0;
}

/*
 * file_close
 * DESCRIPTION: Does nothing
 * INPUTS:
 * fd: File descriptor to be modified (ignored)
 * SIDE EFFECTS: NONE
 * RETURN VALUE: 0
 */
int32_t file_close(uint32_t fd) {
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
int32_t file_read (uint32_t fd, void* buf, int32_t nbytes) {
	pcb_t* curr_pcb = get_pcb(pid);
    fd_t* curr_fd = &(curr_pcb->fd_array[fd]);
	// check buffer validity
	if (!fd || !buf)
		return -1;
	// get the data in the file
	read_data (curr_fd->inode_num, curr_fd->file_position, buf, nbytes);
	curr_fd->file_position += nbytes;
	return nbytes;
}

/*
 * file_close
 * DESCRIPTION: "Fails" to write (RO filesys)
 * INPUTS:
 * fd: File descriptor to be modified (ignored)
 * SIDE EFFECTS: NONE
 * RETURN VALUE: -1 (fails)
 */
int32_t file_write(uint32_t fd, const void* buf, int32_t nbytes) {
	return -1;
}

/*
 * dir_open
 * DESCRIPTION: open directory (already open)
 * INPUTS: fd to be closed (ignored)
 * SIDE EFFECTS: NONE
 * RETURN VALUE: 0 (directory open)
 */
int32_t dir_open(const uint8_t * dname) {
	return 0;
}

/*
 * dir_close
 * DESCRIPTION: fails to close directory
 * INPUTS: fd to be closed (ignored)
 * SIDE EFFECTS: NONE
 * RETURN VALUE: -1 (fails)
 */
int32_t dir_close(uint32_t fd) {
	return -1;
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
int32_t dir_read(uint32_t fd, void * buf, int32_t nbytes) {
	dentry_t dentry;
	pcb_t* curr_pcb = get_pcb(pid);
    fd_t* curr_fd = &(curr_pcb->fd_array[fd]);
	// check buffer validity
	if (!curr_fd || !buf || nbytes > FILESYSTEM_NAME_MAX)
		return -1;
	// read dentry values
	read_dentry_by_index (curr_fd->file_position, &dentry);
	// check if the file name is empty
	nbytes = strlen(dentry.filename);
	// copy data into the buffer
	memcpy(buf, dentry.filename, nbytes);
	curr_fd->file_position++;
	if(nbytes == 0)
		curr_fd->file_position = 0;
	return nbytes;
}

/*
 * dir_write
 * DESCRIPTION: Does nothing
 * INPUTS: fd to write to, buf to write from, nbytes to write
 * SIDE EFFECTS: none
 * RETURN VALUE: -1 (never used)
 */
int32_t dir_write(uint32_t fd, const void* buf, int32_t nbytes) {
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
		// printf("Cannot find file by that name!\n");
		return -1;
	}
	// printf("Index is: %d\n", index);
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
 * offset: where in file to read from
 * buf: buffer to copy from file to
 * nbytes: number of bytes to copy
 * SIDE EFFECTS: Fills buffer with nbytes of file starting at last position
 * RETURN VALUE: 0 or -1 iff NULL input
 */
int32_t read_data (uint32_t inode, uint32_t offset, void* buf, uint32_t length) {

	if (!buf) {
		// printf("Buffer NULL\n");
		return -1;
	}
	if (inode_count < inode) {
		// printf("Trying to read illegal inode\n");
		return -1;
	}

	inode_t * inode_cur = inode + inode_start;
	if (offset + length > inode_cur->length) {
		// printf("Trying to read past file\n");
		return -1;
	}


	/* Start at offset */
	int cur_block_num = inode_cur->data_block_num[offset >> OFFSET_SHIFT];
	int buffer_offset = 0;
	int masked_offset = offset & OFFSET_MASK;
	// copy size is either block size-offset of the desired length of copy
	int copy_size = BLOCK_SIZE-(masked_offset) > length ? length : (BLOCK_SIZE - (masked_offset));
	/* If offset is not 4KB aligned, copy until next 4KB boundary, make length shorter and offset larger */
	if (masked_offset != 0) {
		memcpy(buf + buffer_offset, data_start + (cur_block_num << OFFSET_SHIFT) + (masked_offset), copy_size);
		buffer_offset += copy_size;
		length -= copy_size;
		offset += copy_size;
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
