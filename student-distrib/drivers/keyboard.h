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

/* Externally-visible functions */

/* Initialize the keyboard */
void keyboard_init(void);

/* Handle the keyboard interrupt */
void keyboard_handle_interrupt(void);

#endif // KEYBOARD_H
