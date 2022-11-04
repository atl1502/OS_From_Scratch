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
