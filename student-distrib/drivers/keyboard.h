/* keyboard.h - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "../types.h"

#define TOTAL_ASCII 0x3A
#define ROW1 0x01
#define ROW2 0x0C
#define ROW3 0x0F
#define ROW4 0x1A
#define ROW5 0x1D
#define ROW6 0x27
#define ROW7 0x2B
#define KB_IRQ 1

/* Externally-visible functions */

/* Initialize the keyboard */
void keyboard_init(void);

/* Handle the keyboard interrupt */
void keyboard_handle_interrupt(void);
