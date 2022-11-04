#ifndef SYSWRAPPER
#define SYSWRAPPER

int32_t halt (uint8_t status); // syscall #1
int32_t execute (const uint8_t* command); // syscall #2
int32_t read (int fd, void* buf, int32_t nbytes); // syscall #3
int32_t write (int fd, const void* buf, int32_t nbytes); // syscall #4
int32_t open (const uint8_t* filename); // syscall #5
int32_t close (int fd); // syscall #6
int32_t getargs (uint8_t* buf, int32_t nbytes); // syscall #7
int32_t vidmap (uint8_t** screen_start); // syscall #8
int32_t set_handler (int32_t signum, void* handler_address); // syscall #9
int32_t sigreturn (void); // syscall #10

#endif
