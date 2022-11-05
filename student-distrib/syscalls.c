#include "lib.h"
#include "syscalls.h"
#include "pcb.h"
#include "fd.h"
#include "drivers/filesystem.h"
#include "drivers/rtc.h"
#include "drivers/terminal.h"
#include "x86_desc.h"

// table ptrs for fds
static fd_opts_t file_syscalls = {
    .read  = file_read,
    .write = file_write,
    .close = file_close
};

static fd_opts_t dir_syscalls = {
    .read  = dir_read,
    .write = dir_write,
    .close = dir_close
};

static fd_opts_t rtc_syscalls = {
	.read = rtc_read,
	.write = rtc_write,
	.close = rtc_close
};


int32_t sys_halt (uint8_t status){
    return 0;
}

int32_t sys_execute (const uint8_t* command) {
	printf("Execute Syscall: %s\n", command);
	return 0;
}

int32_t sys_read (uint32_t fd, void* buf, int32_t nbytes) {
    pcb_t* curr_pcb = get_pcb(pid);
    // execute via function pointer table
	return (curr_pcb->fd_array)[fd].table_pointer->read(fd, buf, nbytes);
}

int32_t sys_write (uint32_t fd, const void* buf, int32_t nbytes) {
    pcb_t* curr_pcb = get_pcb(pid);
    // execute via function pointer table
	return (curr_pcb->fd_array)[fd].table_pointer->write(fd, buf, nbytes);
}

// The call should find the directory entry corresponding to the named file,
// allocate an unused file descriptor, and set up any data necessary to handle the given type of file
// (directory, RTC device, or regular file).

// If the named file does not exist or no descriptors are free, the call returns -1.
int32_t sys_open (const uint8_t* filename) {

	printf("Open Syscall: Filename: %s\n", filename);
	return 0;

	int free = 0;
	int i;
	dentry_t dentry;
	int32_t fd;
	int32_t open_ret_val = 0;
	pcb_t* curr_pcb = get_pcb(pid);

	if (filename == NULL)
		return -1;

	// find a free descriptor
	for (i = 0; i < MAX_FILES; i++) {
		fd_t* curr_fd = &(curr_pcb->fd_array[i]);

		if ( ~((curr_fd->flags) & FD_USED) ) { // fd is available (sig bit = 0)
			free = 1;
			fd = i;
			curr_fd->file_position = 0;
			curr_fd->flags |= FD_USED; // mark fd as unavailable (sig bit = 1)

			// read dentry, if null return -1
			if (read_dentry_by_name(filename, &dentry) == -1)
				return -1;

			switch (dentry.filetype) {
				// rtc
				case 0:
					curr_fd->inode_num = dentry.inode_num;
					curr_fd->table_pointer = &rtc_syscalls;
					open_ret_val = rtc_open(filename);
					break;
				// dir
				case 1:
					curr_fd->inode_num = dentry.inode_num;
					curr_fd->table_pointer = &dir_syscalls;
					open_ret_val = dir_open(filename);
					break;
				// file
				case 2:
					curr_fd->inode_num = dentry.inode_num;
					curr_fd->table_pointer = &file_syscalls;
					open_ret_val = file_open(filename);
					break;
				// file type not 0-2
				default:
                    //set to unused
                    curr_fd->flags &= ~FD_USED;
					return -1;
			}
			break;
		}
	}

	// no descriptors are free / open returned -1
	if (free == 0 || open_ret_val == -1)
		return -1;
	// return the fd to user space
	return fd;
}

// The close system call closes the specified file descriptor and makes it
// available for return from later calls to open.
// You should not allow the user to close the default descriptors (0 for input and 1 for output).

// Trying to close an invalid descriptor should result in a return value of -1;
// successful closes should return 0.
int32_t sys_close (uint32_t fd) {
    pcb_t* curr_pcb = get_pcb(pid);
    fd_t* curr_fd = &(curr_pcb->fd_array[fd]);
    // check that file is present and not 1 or 0
    if ( ~((curr_fd->flags) & FD_USED) || fd == 0 || fd == 1)
        return -1;
    // set present to 0
    curr_fd->flags &= ~FD_USED;
	return (curr_pcb->fd_array)[fd].table_pointer->close(fd);
}


int32_t sys_getargs (uint8_t* buf, int32_t nbytes) {
	printf("getargs Syscall: buffer: %x nbytes: %d\n", buf, nbytes);
	// TODO: implement in 3.4
	return -1;
}

int32_t sys_vidmap (uint8_t** screen_start) {
	printf("vidmap Syscall: screen_start %x\n", screen_start);
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
