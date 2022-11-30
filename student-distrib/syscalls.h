#ifndef SYSCALLS
#define SYSCALLS

#include "types.h"
#include "fd.h"

#define SYS_HALT 1
#define SYS_EXECUTE 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define SYS_GETARGS 7
#define SYS_VIDMAP 8
#define SYS_SET_HANDLER 9
#define SYS_SIGRETURN 10
#define SYS_ERROR_STAT 256

#define MAX_FD 7
#define EXCEPTION_ERROR 69
#define FD_USED 0x80000000
#define PROGRAM_VIRT_START 0x08048000
#define PROGRAM_SIZE 0x400000-0x48000
#define SPACE 32

#define ELF_HEADER 0x28
#define FOUR_KB 0x1000
#define FOUR_MIB 0x400000
#define EIGHT_MIB 0x800000
#define BASE_VIRT_ADDR 0x08000000
#define BUF_LEN 128

typedef struct __attribute__((packed)) program_header {
	uint32_t p_type; // Type of segment
	uint32_t p_offset; // Offset of this segment in fileimage
	uint32_t p_vaddr; // Virtual address of where to load seg
	uint32_t p_paddr; // Physical address No Need
	uint32_t p_filesz; // Segment size in filesys
	uint32_t p_memsz; // Segment size in memory
	uint32_t p_flags; // Flags
	uint32_t p_align; // If not 0 or 1, where p_vaddr = p_offset - p_align
} program_header_t;

int32_t sys_halt (uint8_t status); // syscall #1
int32_t sys_execute (const uint8_t* command); // syscall #2
int32_t sys_read (uint32_t fd, void* buf, int32_t nbytes); // syscall #3
int32_t sys_write (uint32_t fd, const void* buf, int32_t nbytes); // syscall #4
int32_t sys_open (const uint8_t* filename); // syscall #5
int32_t sys_close (uint32_t fd); // syscall #6
int32_t sys_getargs (uint8_t* buf, int32_t nbytes); // syscall #7
int32_t sys_vidmap (uint8_t** screen_start); // syscall #8
int32_t sys_set_handler (int32_t signum, void* handler_address); // syscall #9
int32_t sys_sigreturn (void); // syscall #10

#endif
