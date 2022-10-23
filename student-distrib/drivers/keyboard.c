/* keyboard.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "keyboard.h"
#include "../lib.h"
#include "../spinlock.h"
#include "../i8259.h"

void keyboard_handle_interrupt_buffer(uint8_t scan_code);

// reserved key codes 0x01, 0x1D, 0x2A, 0x36, 0x38
static const char scan_code_array[TOTAL_ASCII] = {0x00, 0x00, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
'-', '=', 0x00, '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0x00, 'a', 's', 'd',
'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0x00, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
0x00, '*', 0x00, ' '};
static uint8_t shift_flag = 0;
static uint8_t caps_lock_flag = 0;
static uint8_t alt_flag = 0;
static uint8_t control_flag = 0;
static uint8_t terminal_mode = 0;
static char keyboard_buffer[BUF_LEN] = {0x00};
// current len of buffer (current idx is keyboard_buffer_len-1)
static uint8_t keyboard_buffer_len = 0;

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
 * DESCRIPTION: reads key input, lookup and translate into a character, print to screen, sends EOI
 * INPUTS: none
 * SIDE EFFECTS: pressed key is printed to console
 * RETURN VALUE: none
 */
/* Handle the keyboard interrupt */
void keyboard_handle_interrupt(void) {
    char ascii;
    uint8_t scan_code = inb(0x60);
    char current;
    // get flags
    if (scan_code == 0x3A){                          // caps lock toggle
        caps_lock_flag = caps_lock_flag ? 0 : 1;
    } else if (scan_code == 0x36 || scan_code == 0x2A){     // shift toggle
        shift_flag = 1;
    } else if (scan_code == 0xAA || scan_code == 0xB6){
        shift_flag = 0;
    } else if (scan_code == 0x1D){                          // control toggle
        control_flag = 1;
    } else if (scan_code == 0x9D){
        control_flag = 0;
    } else if (scan_code == 0x38){                          // alt toggle
        alt_flag = 1;
    } else if (scan_code == 0xB8){
        alt_flag = 0;
    }
    // check if in terminal mode since inputs will be handled differently
    if (terminal_mode == 1){
        keyboard_handle_interrupt_buffer(scan_code);
    } else if (scan_code == 0x0E){                                 // backspace
        removec();
    } else if ((scan_code > BOTTOM_ASCII) && (scan_code < TOTAL_ASCII)) {
        // if alt or ctl are being depressed dont print anything (skip rest)
        if(!(alt_flag || control_flag)){
            // if shift or caps lock but not both
            if (shift_flag ^ caps_lock_flag){
                // temp variable of scan code
                current = scan_code_array[scan_code];
                // between ascii values of a and z
                if (current >= 0x61 && current <= 0x7A){
                    ascii = current-CAPS_OFFSET;
                } else {
                    switch(current){
                        case '0': ascii = ')'; break;
                        case '1': ascii = '!'; break;
                        case '2': ascii = '@'; break;
                        case '3': ascii = '#'; break;
                        case '4': ascii = '$'; break;
                        case '5': ascii = '%'; break;
                        case '6': ascii = '^'; break;
                        case '7': ascii = '&'; break;
                        case '8': ascii = '*'; break;
                        case '9': ascii = '('; break;
                        case '-': ascii = '_'; break;
                        case '=': ascii = '+'; break;
                        case '`': ascii = '~'; break;
                        case '[': ascii = '{'; break;
                        case ']': ascii = '}'; break;
                        case '\\': ascii = '|'; break;
                        case ';': ascii = ':'; break;
                        case '\'': ascii = '\"'; break;
                        case ',': ascii = '<'; break;
                        case '.': ascii = '>'; break;
                        case '/': ascii = '?'; break;
                        default: ascii = current;
                    }
                }
            } else {
                ascii = scan_code_array[scan_code];
            }
            // check if it is shift/alt/capslock
            if (ascii != 0x00){
                putc(ascii);
            }
        }
    }
    send_eoi(KB_IRQ);
}

/*
 * keyboard_handle_interrupt_buffer
 * DESCRIPTION: Helper function to handle keyboard interrupt while in terminal
 * INPUTS: The scan code from the interrupt
 * SIDE EFFECTS: pressed key is printed to console
 * RETURN VALUE: none
 */
/* Handle the keyboard interrupt */
void keyboard_handle_interrupt_buffer(uint8_t scan_code){
    char current, ascii;
    int i;
    current = scan_code_array[scan_code];
    // check prev to make sure it wasn't new line skip if it is
    if(keyboard_buffer_len != 0 && keyboard_buffer[keyboard_buffer_len-1] == '\n'){
        return;
    }
    if (current == '\n' && keyboard_buffer_len < BUF_LEN) {
        // add newline to end of buffer
        keyboard_buffer[keyboard_buffer_len] = current;
        keyboard_buffer_len++;
        putc('\n');
    } else if (control_flag == 1 && scan_code == 0x26){ //ctrl+l
        clear();
        // reprint all chars in buffer
        for (i = 0; i < keyboard_buffer_len; i++){
            putc(keyboard_buffer[i]);
        }
    } else if (scan_code == 0x0E && keyboard_buffer_len > 0){ // backspace
        removec();
        // remove from buffer;
        keyboard_buffer_len--;
        keyboard_buffer[keyboard_buffer_len] = 0x00;
    } else if (keyboard_buffer_len < BUF_LEN-1){ // ensure buffer is < 127
        // add char to buffer and prints
        if ((scan_code > BOTTOM_ASCII) && (scan_code < TOTAL_ASCII)) {
            // if alt or ctl are being depressed dont print anything (skip rest)
            if(!(alt_flag || control_flag)){
                // if shift or caps lock but not both
                if (shift_flag ^ caps_lock_flag){
                    // between ascii values of a and z
                    if (current >= 0x61 && current <= 0x7A){
                        ascii = current-CAPS_OFFSET;
                    } else {
                        switch(current){
                            case '0': ascii = ')'; break;
                            case '1': ascii = '!'; break;
                            case '2': ascii = '@'; break;
                            case '3': ascii = '#'; break;
                            case '4': ascii = '$'; break;
                            case '5': ascii = '%'; break;
                            case '6': ascii = '^'; break;
                            case '7': ascii = '&'; break;
                            case '8': ascii = '*'; break;
                            case '9': ascii = '('; break;
                            case '-': ascii = '_'; break;
                            case '=': ascii = '+'; break;
                            case '`': ascii = '~'; break;
                            case '[': ascii = '{'; break;
                            case ']': ascii = '}'; break;
                            case '\\': ascii = '|'; break;
                            case ';': ascii = ':'; break;
                            case '\'': ascii = '\"'; break;
                            case ',': ascii = '<'; break;
                            case '.': ascii = '>'; break;
                            case '/': ascii = '?'; break;
                            default: ascii = current;
                        }
                    }
                } else {
                    ascii = scan_code_array[scan_code];
                }
            }
            // check if it is shift/alt/capslock
            if (ascii != 0x00){
                putc(ascii);
                keyboard_buffer[keyboard_buffer_len] = ascii;
                if(ascii == '\t'){
                    // add four spaces for the tab and add to keyboard_buffer_len
                    for(i = 0; i < TAB_SIZE; i++){
                        keyboard_buffer[keyboard_buffer_len] = ' ';
                        keyboard_buffer_len++;
                    }
                } else{
                    keyboard_buffer[keyboard_buffer_len] = ascii;
                    keyboard_buffer_len++;
                }
            }
        }
    }
}

/*
 * set_terminal_mode
 * DESCRIPTION: Sets the terminal mode in keyboard
 * INPUTS: mode: either 0 or 1, 0 for terminal off, 1 for on
 * SIDE EFFECTS: Changes how keyboard handles printing to screen
 * RETURN VALUE: 0
 */
void set_terminal_mode(int mode){
    terminal_mode = mode;
}

/*
 * get_keyboard_buffer
 * DESCRIPTION: Copy keyboard buffer into given buffer
 * INPUTS: buf location to copy into
 * SIDE EFFECTS: changes values in buf
 * RETURN VALUE: None
 */
void get_keyboard_buffer(char* buf){
    memcpy(buf, keyboard_buffer, keyboard_buffer_len);
}

/*
 * get_keyboard_buffer_length
 * DESCRIPTION: Returns the keyboard buffer length
 * INPUTS: None
 * SIDE EFFECTS: None
 * RETURN VALUE: keyboard buffer length
 */
uint8_t get_keyboard_buffer_length(){
    return keyboard_buffer_len;
}

/*
 * reset_keyboard_buffer
 * DESCRIPTION: Empty keyboard buffer and changing keyboard_buffer_len to 0
 * INPUTS: None
 * SIDE EFFECTS: sets keyboard_buffer_len to 0
 * RETURN VALUE: None
 */
void reset_keyboard_buffer(){
    uint8_t i;
    // 0 all elements in keyboard buffer
    for(i = 0; i < keyboard_buffer_len; i++){
        keyboard_buffer[i] = 0x00;
    }
    keyboard_buffer_len = 0;
}
