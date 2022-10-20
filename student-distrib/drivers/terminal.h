/* terminal.h - Defines used in interactions with the terminal
 * vim:ts=4 noexpandtab
 */

#ifndef TERMINAL
#define TERMINAL
#define BUF_LEN 128

int terminal_open();
int terminal_close();
int terminal_read();
int terminal_write();

#endif
