/* keyboard.h - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */



#include "../types.h"

/* Externally-visible functions */

/* Initialize the keyboard */
void keyboard_init(void);

/* Handle the keyboard interrupt */
void keyboard_handle_interrupt(void);
