/* terminal.h - Defines used in interactions with the terminal
 * vim:ts=4 noexpandtab
 */
#include "../types.h"
#include "../fd.h"

#ifndef TERMINAL
#define TERMINAL

int terminal_open(const uint8_t* filename);
int terminal_close(uint32_t fd);
int terminal_read(uint32_t fd, void* buf, int32_t nbytes);
int terminal_write(uint32_t fd, const void* buf, int32_t nbytes);

#endif
