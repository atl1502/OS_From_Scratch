/* keyboard.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "../lib.h"
#include "../spinlock.h"
#include "../i8259.h"

// reserved key codes 0x01, 0x1D, 0x2A, 0x36, 0x38
static const char scan_code_array[TOTAL_ASCII] = {'\n', '\n', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
'-', '=', '\n', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '\n', 'a', 's', 'd',
'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '\n', '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
'\n', '*', '\n', ' '};
static uint8_t shift_flag = 0;
static uint8_t caps_lock_flag = 0;

/*
 * keyboard_init
 * DESCRIPTION: Initialize the keyboard
 * INPUTS: none
 * SIDE EFFECTS: enables IRQ for keyboard
 * RETURN VALUE: none
 */
void keyboard_init(void) {
    //unmask interrupt line on PIC
    enable_irq(KB_IRQ);
}

/*
 * keyboard_handle_interrupt
 * DESCRIPTION: reads key input, lookup and translate into a charactewr, print to screen, sends EOI
 * INPUTS: none
 * SIDE EFFECTS: pressed key is printed to console
 * RETURN VALUE: none
 */
/* Handle the keyboard interrupt */
void keyboard_handle_interrupt(void) {
    char ascii;
    int scan_code = inb(0x60);
    // TODO: Terminal close on esc key press (0x01)
    if (scan_code == 0x3A){
        caps_lock_flag = caps_lock_flag ? 0 : 1;
    } else if ((scan_code > BOTTOM_ASCII) && (scan_code < TOTAL_ASCII)) {
        if (shift_flag ^ caps_lock_flag){
            ascii = scan_code_array[scan_code]-CAPS_OFFSET;
        } else {
            ascii = scan_code_array[scan_code];
        }
        putc(ascii);
    }
    send_eoi(KB_IRQ);
}
