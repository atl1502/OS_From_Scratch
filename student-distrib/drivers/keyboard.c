/* keyboard.c - Functions to interact with the RTC
 */

#include "keyboard.h"
#include "../x86_desc.h"
#include "../scheduling.h"
#include "../lib.h"
#include "../spinlock.h"
#include "../i8259.h"
#include "../paging.h"

/* Handles keyboard buffer in interrupt context */
static void keyboard_handle_interrupt_buffer(uint8_t scan_code);

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

/* Current len of buffer (current idx is keyboard_buffer_len-1) */
static char keyboard_buffers[3][BUF_LEN] = {{0x00}};
static uint8_t keyboard_buffer_lens[3] = {0x00};

/*
 * keyboard_init
 * DESCRIPTION: Initialize the keyboard
 * INPUTS: none
 * SIDE EFFECTS: enables IRQ for keyboard
 * RETURN VALUE: none
 */
void keyboard_init(void) {

	// Unmask interrupt line on PIC
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

	// Keyboard always prints to screen
	unmap();

	// Modifier Key Flags
	switch (scan_code) {
		case 0x3A: // Capslock toggle
			caps_lock_flag = caps_lock_flag ? 0 : 1;
			break;
		case 0x36: // Set Shift
		case 0x2A:
			shift_flag = 1;
			break;
		case 0xB6: // Clear Shift
		case 0xAA:
			shift_flag = 0;
			break;
		case 0x1D:
			control_flag = 1;
			break;
		case 0x9D:
			control_flag = 0;
			break;
		case 0x38:
			alt_flag = 1;
			break;
		case 0xB8:
			alt_flag = 0;
			break;
	}

	// If in terminal mode, (almost always) handle keys differently (buffered)
	if (terminal_mode == 1) {
		keyboard_handle_interrupt_buffer(scan_code);
	}
	// Backspace
	else if (scan_code == 0x0E) {
		removec();
	}
	// Valid ASCII
	else if ((scan_code > BOTTOM_ASCII) && (scan_code < TOTAL_ASCII)) {
		// Don't print key combos with control or alt
		if(!(alt_flag || control_flag)) {

			// Capitalization Logic
			if (shift_flag ^ caps_lock_flag){

				// Scan code char
				current = scan_code_array[scan_code];

				// Between ascii values of a and z
				if (current >= 0x61 && current <= 0x7A){
					ascii = current-CAPS_OFFSET;
				}
				// Handle Special chars
				else {
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
			}
			// Non capital ASCII
			else {
				ascii = scan_code_array[scan_code];
			}
			// Make sure valid ASCII to print
			if (ascii != 0x00){
				putc_colourised(ascii, YELLOW);
			}
		}
	}

	/* Return video mem map to buffer if running process was on different terminal */
	if (term_num != running_proc) {
		remap(running_proc);
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
	char current;
	char ascii = 0;
	int i;
	char prompt[7] = "391OS> ";
	current = scan_code_array[scan_code];

	/* Switching terminals if ALT && F(0-2) key */
	if (alt_flag == 1) {
		switch (scan_code) {
			case 0x3B: //F1 case
				if (term_num != 0) {
					switch_term(0, term_num);
				}
				return;
			case 0x3C: //F2 case
				if (term_num != 1) {
					switch_term(1, term_num);
				}
				return;
			case 0x3D: //F3 case
				if (term_num != 2) {
					switch_term(2, term_num);
				}
				return;
		}
	}

	// Make sure previous key pressed in buffer is not newline
	if(keyboard_buffer_lens[term_num] != 0 && keyboard_buffers[term_num][keyboard_buffer_lens[term_num]-1] == '\n') {
		return;
	}

	/* Save screen location of currently scheduled proc */
	save_screen(running_proc);

	/* Restore terminal screen location */
	restore_screen(term_num);

	/* Set kb_flag to on*/
	flip_kb_flag();

	if (current == '\n' && keyboard_buffer_lens[term_num] < BUF_LEN) {
		// add newline to end of buffer
		keyboard_buffers[term_num][keyboard_buffer_lens[term_num]] = current;
		keyboard_buffer_lens[term_num]++;
		putc('\n');
	}
	// Control L Clears Screen and rewrites terminal
	else if (control_flag == 1 && scan_code == 0x26){
		clear();

		// reprint "391OS> "
		for (i = 0; i < sizeof(prompt); i++) {
			putc_colourised(prompt[i], WHITE);
		}
		// reprint all chars in buffer
		for (i = 0; i < keyboard_buffer_lens[term_num]; i++){
			putc_colourised(keyboard_buffers[term_num][i], YELLOW);
		}
	}
	// Checks for backspace
	else if (scan_code == 0x0E && keyboard_buffer_lens[term_num] > 0){
		// Tab is three spaces
		if (keyboard_buffers[term_num][keyboard_buffer_lens[term_num]-1] == '\t') {
			removec();
			removec();
			removec();
		}
		removec();

		// Remove char from buffer & set entry to zero
		keyboard_buffer_lens[term_num]--;
		keyboard_buffers[term_num][keyboard_buffer_lens[term_num]] = 0x00;

	}
	// keyboard_buffer_lens[term_num] < 127 b/c null termination & 0 indexing
	else if (keyboard_buffer_lens[term_num] < BUF_LEN-1){

		// Add char to buffer and print char

		// Check to make sure is ASCII char to print
		if ((scan_code > BOTTOM_ASCII) && (scan_code < TOTAL_ASCII)) {

			// Don't print if pressing key with control or alt
			if(!(alt_flag || control_flag)){

				// Shift and Capitalizing logic
				if (shift_flag ^ caps_lock_flag){

					// Ascii alphabet print
					if (current >= 0x61 && current <= 0x7A){
						ascii = current-CAPS_OFFSET;
					}
					// Special char print
					else {
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
				}
				// Normal ASCII alphabet print
				else {
					ascii = scan_code_array[scan_code];
				}
			}
			// check if it is shift/alt/capslock
			if (ascii != 0x00){
				putc_colourised(ascii, YELLOW);
				keyboard_buffers[term_num][keyboard_buffer_lens[term_num]] = ascii;
				keyboard_buffer_lens[term_num]++;
			}
		}
	}

	/* Save terminal screen */
	save_screen(term_num);

	/* Restore proc screen */
	restore_screen(running_proc);

	/* Set kb_flag to off*/
	flip_kb_flag();
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
 * get_term_keyboard_buffer
 * DESCRIPTION: Copy proc keyboard buffer into given buffer
 * INPUTS: buf location to copy into
 * SIDE EFFECTS: changes values in buf
 * RETURN VALUE: None
 */
void get_term_keyboard_buffer(char* buf){
	memcpy(buf, keyboard_buffers[term_num], keyboard_buffer_lens[term_num]);
}

/*
 * get_proc_keyboard_buffer
 * DESCRIPTION: Copy terminal keyboard buffer into given buffer
 * INPUTS: buf location to copy into
 * SIDE EFFECTS: changes values in buf
 * RETURN VALUE: None
 */
void get_proc_keyboard_buffer(char* buf){
	memcpy(buf, keyboard_buffers[running_proc], keyboard_buffer_lens[running_proc]);
}

/*
 * get_term_keyboard_buffer_length
 * DESCRIPTION: Returns the keyboard buffer length
 * INPUTS: None
 * SIDE EFFECTS: None
 * RETURN VALUE: keyboard buffer length
 */
uint8_t get_term_keyboard_buffer_length(){
	return keyboard_buffer_lens[term_num];
}

/*
 * get_proc_keyboard_buffer_length
 * DESCRIPTION: Returns the keyboard buffer length
 * INPUTS: None
 * SIDE EFFECTS: None
 * RETURN VALUE: keyboard buffer length
 */
uint8_t get_proc_keyboard_buffer_length(){
	return keyboard_buffer_lens[running_proc];
}


/*
 * reset_keyboard_buffer
 * DESCRIPTION: Empty keyboard buffer and changing keyboard_buffer_lens[term_num] to 0
 * INPUTS: None
 * SIDE EFFECTS: sets keyboard_buffer_lens[term_num] to 0
 * RETURN VALUE: None
 */
void reset_term_keyboard_buffer(){
	uint8_t i;
	// 0 all elements in keyboard buffer
	for(i = 0; i < keyboard_buffer_lens[term_num]; i++){
		keyboard_buffers[term_num][i] = 0x00;
	}
	keyboard_buffer_lens[term_num] = 0;
}

/*
 * reset_keyboard_buffer
 * DESCRIPTION: Empty keyboard buffer and changing keyboard_buffer_lens[term_num] to 0
 * INPUTS: None
 * SIDE EFFECTS: sets keyboard_buffer_lens[term_num] to 0
 * RETURN VALUE: None
 */
void reset_proc_keyboard_buffer(){
	uint8_t i;
	// 0 all elements in keyboard buffer
	for(i = 0; i < keyboard_buffer_lens[running_proc]; i++){
		keyboard_buffers[running_proc][i] = 0x00;
	}
	keyboard_buffer_lens[running_proc] = 0;
}



/*
 * copy_buffer
 * DESCRIPTION: copys a buffer
 * INPUTS: destination and source buffers, size
 * SIDE EFFECTS: none
 * RETURN VALUE: none
 */
void copy_buffer(char* dest, char* src, int size) {
	int i;
	for (i = 0; i < size; i++) {
		dest[i] = src[i];
	}
	return;
}
