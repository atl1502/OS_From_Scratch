/* keyboard.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "../lib.h"
#include "../spinlock.h"
#include "../i8259.h"

#define TOTAL_ASCII 0x33
#define ROW1 0x01
#define ROW2 0x0C
#define ROW3 0x0F
#define ROW4 0x1A
#define ROW5 0x1D
#define ROW6 0x27
#define ROW7 0x2B
const int keyboard_interrupt_number = 1;
const char scan_code_array[TOTAL_ASCII] = {'\n', '\n', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
'\n', '\n', '\n', '\n', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\n', '\n', '\n', '\n', 'a', 's', 'd',
'f', 'g', 'h', 'j', 'k', 'l', '\n', '\n', '\n', '\n', '\n', 'z', 'x', 'c', 'v', 'b', 'n', 'm'};

/* Initialize the keyboard */
void keyboard_init(void) {
    //unmask interrupt line on PIC
    enable_irq(keyboard_interrupt_number);
}

/* Handle the keyboard interrupt */
void keyboard_handle_interrupt(void) {
    char ascii;
    int scan_code = inb(0x60);
    if (((scan_code > ROW1) && (scan_code < ROW2)) || ((scan_code > ROW3) && (scan_code < ROW4)) || ((scan_code > ROW5) && (scan_code < ROW6)) || ((scan_code > ROW7) && (scan_code < TOTAL_ASCII))) {
        ascii = scan_code_array[scan_code];
        putc(ascii);
    }
    send_eoi(keyboard_interrupt_number);
}
