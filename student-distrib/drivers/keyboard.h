/* keyboard.h - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../types.h"

#define TOTAL_ASCII 0x3A
#define BOTTOM_ASCII 0x01
#define KB_IRQ 1
#define CAPS_OFFSET 0x20
#define BUF_LEN 128


/* Externally-visible functions */

/* Initialize the keyboard */
void keyboard_init(void);

/* Handle the keyboard interrupt */
void keyboard_handle_interrupt(void);

/* Set terminal mode for keybaord */
void set_terminal_mode(int mode);

/* Copy keyboard buffer into given buffer */
void get_keyboard_buffer(char* buf);

/* Returns the buffer's current length */
uint8_t get_keyboard_buffer_length();

/* Resets the keyboard buffer (usually after a new line) */
void reset_keyboard_buffer();

#endif // KEYBOARD_H
