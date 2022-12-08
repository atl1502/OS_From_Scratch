/* terminal.h - Defines used in interactions with the terminal
 * vim:ts=4 noexpandtab
 */
#include "../types.h"
#include "../fd.h"

#ifndef TERMINAL
#define TERMINAL

#define LT_GREEN 0xA

int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(uint32_t fd);
int32_t terminal_read(uint32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(uint32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_bad_read(uint32_t fd, void* buf, int32_t nbytes);
int32_t terminal_bad_write(uint32_t fd, const void* buf, int32_t nbytes);

#endif
