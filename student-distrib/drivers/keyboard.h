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
#define TAB_SIZE
#define YELLOW 0xE
#define WHITE 0xF

/* Externally-visible functions */

/* Initialize the keyboard */
void keyboard_init(void);

/* Handle the keyboard interrupt */
void keyboard_handle_interrupt(void);

/* Set terminal mode for keybaord */
void set_terminal_mode(int mode);

/* Copy term keyboard buffer into given buffer */
void get_term_keyboard_buffer(char* buf);

/* Copy proc keyboard buffer into given buffer */
void get_proc_keyboard_buffer(char* buf);

/* Returns the proc buffer's current length */
uint8_t get_proc_keyboard_buffer_length();

/* Returns the term buffer's current length */
uint8_t get_term_keyboard_buffer_length();

/* Resets the keyboard buffer (usually after a new line) */
void reset_term_keyboard_buffer();
void reset_proc_keyboard_buffer();

/* Returns which terminal we are using*/
uint8_t get_terminal_num();

/* Copies a buffer */
void copy_buffer(char* dest, char* src, int size);

#endif // KEYBOARD_H
