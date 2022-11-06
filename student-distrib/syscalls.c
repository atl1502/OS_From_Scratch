#include "lib.h"
#include "syscalls.h"
#include "pcb.h"
#include "fd.h"
#include "x86_desc.h"
#include "drivers/filesystem.h"
#include "drivers/rtc.h"
#include "drivers/terminal.h"
#include "x86_desc.h"
#include "paging.h"

// jump table ptrs for file fd's
static fd_ops_t file_syscalls = {
	.read  = file_read,
	.write = file_write,
	.close = file_close
};

// jump table ptrs for directory fd
static fd_ops_t dir_syscalls = {
	.read  = dir_read,
	.write = dir_write,
	.close = dir_close
};

// jump table ptrs for RTC fd
static fd_ops_t rtc_syscalls = {
	.read = rtc_read,
	.write = rtc_write,
	.close = rtc_close
};

// jump table ptrs for stdin fd
static fd_ops_t file_stdin = {
	.read = terminal_read,
	.write = terminal_bad_write,
	.close = terminal_close
};

// jump table ptrs for stdout fd
static fd_ops_t file_stdout = {
	.read = terminal_bad_read,
	.write = terminal_write,
	.close = terminal_close
};

/*
 * sys_halt
 * DESCRIPTION: terminates a process, returning the specified value to its parent process
 * INPUTS: file descriptor to read from, buffer to fill, # of bytes to read
 * SIDE EFFECTS: expands the 8-bit argument from BL into the 32-bit return value to parent exec
 * RETURN VALUE: -1 on failure, otherwise termination status
 */
int32_t sys_halt (uint8_t status) {
	// printf("HALTING WITH STATUS %d\n", status);
	task_stack_t * curr_task_stack = (task_stack_t*) (K_PAGE_ADDR - (EIGHT_KB * (pid+1)));
	pcb_t curr_pcb = curr_task_stack->task_pcb;

	if (curr_pcb.pid != pid) {
		printf("YOU SHOULD NOT BE HERE !!! PID of HALT != PID GLOBAL\n");
		return -1;
	}

	// Restore Parent Paging
	context_switch_paging(curr_pcb.parent_id);
	dealloc_process(curr_pcb.pid);

	// Restore pid
	pid = curr_pcb.parent_id;

	// Close all files (Nothing happens yay!)

	// Restore Parent Data (esp0) and return to where execute was called
	tss.esp0 = K_PAGE_ADDR - (EIGHT_KB * (pid));
	uint32_t local_status = status;
	if(status == EXCEPTION_ERROR)
		local_status = 256;
	asm volatile(
			"movl %0, %%eax;"
			"movl %1, %%esp;"
			"movl %2, %%ebp;"
			"leave;"
			"ret;"
			:
			: "r" (local_status), "r" ((curr_pcb.esp)), "r" ((curr_pcb.ebp))
			: "memory", "cc"
	);

	return status;
}

/*
 * sys_execute
 * DESCRIPTION: attempts to load and execute a new program, hands the processor to the new program until it terminates
 * INPUTS: command: a space-separated sequence of words
 * The first word is the file name of the program to be executed.
 * The rest of the command should be provided to the new program on request via the getargs system call.
 * SIDE EFFECTS:
 * RETURN VALUE: -1 if unexecutable, 256 if dies by exception,
 * 0 to 255 if the program executes a halt system call, given by the program’s call to halt
 */
int32_t sys_execute (const uint8_t* command) {

	// Magic string at start of ELF file
	char magic_string[4] = { 0x7F, 'E', 'L', 'F' };

	// ELF Headers
	elf_header_t curr_elf_header = {{ 0 }};
	program_header_t curr_program_header = { 0 };

	// Filesys Dentry
	dentry_t curr_dentry = {{ 0 }};

	// Loop counter
	int i;

	// User address stack and base pointer
	uint32_t user_esp = 0;


	// Get executable file header from filesys
	read_dentry_by_name (command, &curr_dentry);
	if (curr_dentry.filetype != 2) {
		// printf("Filetype incorrect!!!!\n");
		return -1;
	}
	read_data (curr_dentry.inode_num, 0, &curr_elf_header, sizeof(elf_header_t));

	// Verify executable string
	if (strncmp((const char*) curr_elf_header.e_ident, magic_string, 4)) {
		// printf("MAGIC STRING WRONG!!!!\n");
		return -1;
	}
	// printf("LOADING EXEC\n");
	// Continue, the file is correct

	// Allocate new PID and new page directory
	int proc_pid = alloc_new_process();
	if (proc_pid == -1) {
		// printf("PID COULD NOT BE ALLOCATED");
		return -1;
	}
	context_switch_paging(proc_pid);
	// MAGIC
	memset((void*) 0x08048000, 0, 0x400000-0x48000);

	// Load ELF segments into memory
	for (i = 0; i < curr_elf_header.e_phnum-1; i++) {
		read_data(curr_dentry.inode_num, curr_elf_header.e_phoff + (curr_elf_header.e_phentsize * i),
			&curr_program_header, sizeof(program_header_t));
		if (curr_program_header.p_type != 1) {
			// printf("SEGMENT NOT LOADABLE!!!\n");
			continue;
		}
		read_data(curr_dentry.inode_num, curr_program_header.p_offset, (void*)curr_program_header.p_vaddr, curr_program_header.p_memsz);

		if (i == curr_elf_header.e_phnum - 2) {
			user_esp = curr_program_header.p_vaddr + curr_program_header.p_memsz;
		}
	}

	// PCB Address pointer
	task_stack_t * const task_stack = (task_stack_t*) (K_PAGE_ADDR - (EIGHT_KB * (proc_pid+1)));

	// file descriptor set up for 0 and 1
	fd_t * file_array = task_stack->task_pcb.fd_array;
	file_array->table_pointer = file_stdin;
	file_array->flags |= FD_USED;
	(file_array+1)->table_pointer = file_stdout;
	(file_array+1)->flags |= FD_USED;

	task_stack->task_pcb.parent_id = pid;
	task_stack->task_pcb.pid = proc_pid;

	pid = proc_pid;

	tss.esp0 = K_PAGE_ADDR - (EIGHT_KB * (proc_pid));
	// TSS Setup for context switch with PCB init
	asm volatile ("             \n\
			movl %%ebp, %0      \n\
	 		movl %%esp, %1      \n\
            "
            : "=r"(task_stack->task_pcb.ebp), "=r"(task_stack->task_pcb.esp)
            :
            : "memory", "cc"
    );

	__asm__(
		"pushl %%ss\n\t"
		"pushl %[user_esp]\n\t"
		"pushfl\n\t"
		"pushl %%cs\n\t"
		"pushl %[entry]\n\t"
		:
		: [entry] "g"(curr_elf_header.e_entry), [user_esp] "g"(user_esp)
		);

	asm volatile("iret"::);

	return 0;
}

/*
 * sys_read
 * DESCRIPTION: reads data from the keyboard, a file, device (RTC), or directory.
 * INPUTS: file descriptor to read from, buffer to fill, # of bytes to read
 * SIDE EFFECTS: calls corresponding read function
 * RETURN VALUE: the number of bytes read, 0 if RTC, at or beyond end of file
 */
int32_t sys_read (uint32_t fd, void* buf, int32_t nbytes) {
	if (buf == NULL || fd > MAX_FD) {
		return -1;
	}
	pcb_t* curr_pcb = get_pcb(pid);
	// execute via function pointer table
	return (curr_pcb->fd_array)[fd].table_pointer.read(fd, buf, nbytes);
}

/*
 * sys_write
 * DESCRIPTION: The write system call writes data to the terminal or to a device (RTC)
 * INPUTS: file descriptor to write to, buffer to write to, # of bytes to write
 * SIDE EFFECTS: calls corresponding write function (i.e. sets rtc  rate, terminal putc)
 * RETURN VALUE: the number of bytes written, or -1 on failure.
 */
int32_t sys_write (uint32_t fd, const void* buf, int32_t nbytes) {
	if (buf == NULL || fd > MAX_FD) {
		return -1;
	}
	pcb_t* curr_pcb = get_pcb(pid);
	// execute via function pointer table
	fd_t * curr_fd = curr_pcb->fd_array;
	int val = curr_fd[fd].table_pointer.write(fd, buf, nbytes);
	return val;
}
/*
 * sys_open
 * DESCRIPTION: finds the directory entry corresponding to the named file, allocate an
 * unused file descriptor, and set up any data necessary to handle the given type of file
 * (directory, RTC device, or regular file).
 * INPUTS: filename
 * SIDE EFFECTS: fd is filled out, calls associated file open function
 * RETURN VALUE: returns the open file descriptor index, -1 if named file does not exist or no descriptors are free
 */
int32_t sys_open (const uint8_t* filename) {
	// printf("Open Syscall: Filename: %s\n", filename);

	int free = 0;
	int i;
	dentry_t dentry;
	int32_t fd;
	int32_t open_ret_val = 0;
	pcb_t* curr_pcb = get_pcb(pid);

    // filename null check
	if (filename == NULL)
		return -1;

	// go through all FDs to find a free descriptor
	for (i = 0; i < MAX_FILES; i++) {
		fd_t* curr_fd = &(curr_pcb->fd_array[i]);

		if (((curr_fd->flags) & FD_USED) != FD_USED) { // fd is available (sig bit = 0)
			free = 1; // flag that we found an open FD, save fd index
			fd = i;
			curr_fd->file_position = 0; // file_position is always 0
			curr_fd->flags |= FD_USED; // mark fd as unavailable (sig bit = 1)

			// read dentry, if null return -1
			if (read_dentry_by_name(filename, &dentry) == -1)
				return -1;

            // switch based on filetype; assign assign inode and table pointers
			switch (dentry.filetype) {
				// rtc
				case 0:
					curr_fd->inode_num = dentry.inode_num;
					curr_fd->table_pointer = rtc_syscalls;
					open_ret_val = rtc_open(filename);
					break;
				// directory
				case 1:
					curr_fd->inode_num = dentry.inode_num;
					curr_fd->table_pointer = dir_syscalls;
					open_ret_val = dir_open(filename);
					break;
				// file
				case 2:
					curr_fd->inode_num = dentry.inode_num;
					curr_fd->table_pointer = file_syscalls;
					open_ret_val = file_open(filename);
					break;
				// invalid file type not 0-2
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
	// return the fd index to user space
	return fd;
}

/*
 * sys_close
 * DESCRIPTION: The close system call closes the specified file descriptor
 * and makes it available for return from later calls to open.
 * You should not allow the user to close the default descriptors (0 for input and 1 for output).
 * INPUTS: file descriptor to be cloesd
 * SIDE EFFECTS: none
 * RETURN VALUE: 0 if successful, -1 if descriptor is invalid
 */
int32_t sys_close (uint32_t fd) {
	if(fd > MAX_FD)
		return -1;
    // get current FD and PCB with PID
    pcb_t* curr_pcb = get_pcb(pid);
    fd_t* curr_fd = &(curr_pcb->fd_array[fd]);

    // check that file is present and not 1 or 0 (output or input should not be closed)
    if ( ~((curr_fd->flags) & FD_USED) || fd == 0 || fd == 1)
        return -1;

    // set present to 0
    curr_fd->flags &= ~FD_USED;

    // call corresponding close function within jump table
	return (curr_pcb->fd_array)[fd].table_pointer.close(fd);
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
