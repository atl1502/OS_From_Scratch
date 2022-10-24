/* terminal.h - Defines used in interactions with the terminal
 * vim:ts=4 noexpandtab
 */
#include "../types.h"

#ifndef TERMINAL
#define TERMINAL

int terminal_open(const uint8_t* filename);
int terminal_close(int32_t fd);
int terminal_read(int32_t fd, void* buf, int32_t nbytes);
int terminal_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
