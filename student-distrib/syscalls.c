#include "syscalls.h"
#include "pcb.h"
#include "fd.h"
#include "drivers/filesystem.h"
#include "drivers/rtc.h"

static int pid = 0;

int32_t sys_execute (const uint8_t* command) {
	return 0;
}

int32_t sys_read (uint32_t fd, void* buf, int32_t nbytes) {
	pcb_t* cur_pcb = get_pcb(pid);
	return 0;
}

int32_t sys_write (uint32_t fd, const void* buf, int32_t nbytes) {
	pcb_t* cur_pcb = get_pcb(pid);
	return 0;
}

// The call should find the directory entry corresponding to the named file,
// allocate an unused file descriptor, and set up any data necessary to handle the given type of file
// (directory, RTC device, or regular file).

// If the named file does not exist or no descriptors are free, the call returns -1.
int32_t sys_open (const uint8_t* filename) {
	int free = 0;
	int i;
	dentry_t dentry;
	pcb_t* curr_pcb = get_pcb(pid);

	if (filename == NULL)
		return -1;

	// find a free descriptor
	for (i = 0; i < MAX_FILES; i++) {
		fd_t curr_fd = curr_pcb->fd[i];

		if ( ~((curr_fd.flags) & 0x80000000) ) { // fd is available (sig bit = 0)
			free = 1;
			curr_fd.file_position = 0;
			curr_fd.flags |= 0x80000000; // mark fd as unavailable (sig bit = 1)

			// read dentry, if null return -1
			if (read_dentry_by_name(filename, &dentry) == -1)
				return -1;

			switch (dentry.filetype) {
				// rtc
				case 0:
					curr_fd.inode_num = dentry.inode_num;
					return rtc_open(filename, &curr_fd);
					break;
				// dir
				case 1:
					curr_fd.inode_num = dentry.inode_num;
					return dir_open(filename, &curr_fd);
					break;
				// file
				case 2:
					curr_fd.inode_num = dentry.inode_num;
					return file_open(filename, &curr_fd);
					break;
				// if we're here it's wrong
				default:
					return -1;
			}
			break;
		}
	}

	// no descriptors are free
	if (free == 0)
		return -1;

	return 0;
}

// The close system call closes the specified file descriptor and makes it
// available for return from later calls to open.
// You should not allow the user to close the default descriptors (0 for input and 1 for output).

// Trying to close an invalid descriptor should result in a return value of -1;
// successful closes should return 0.
int32_t sys_close (uint32_t fd) {
	return 0;
}

int32_t sys_halt (uint8_t status) {
	return 0;
}

int32_t sys_getargs (uint8_t* buf, int32_t nbytes) {
	// TODO: implement in 3.4
	return -1;
}

int32_t sys_vidmap (uint8_t** screen_start) {
	// TODO: implement in 3.4
	return -1;
}

int32_t sys_set_handler (int32_t signum, void* handler_address) {
	// TODO: EXTRA CREDIT
	return -1;
}

int32_t sys_sigreturn (void) {
	// TODO: EXTRA CREDIT
	return -1;
}
