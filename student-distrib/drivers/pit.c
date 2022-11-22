/* pit.c - Functions to interact with the PIT */

#include "pit.h"
#include "../lib.h"
#include "../spinlock.h"
#include "../i8259.h"
#include "../fd.h"
#include "../scheduling.h"
#include "../syscall_wrapper.h"

static uint8_t pit_counter = 0;

/*
 * pit_init
 * DESCRIPTION: Initialize the PIT
 * INPUTS: none
 * SIDE EFFECTS: enables IRQ for PIT
 * RETURN VALUE: none
 */
/* Initialize the PIT */
void pit_init(uint32_t freq) {

	//divide max pit frequency by our frequency to get reload value
	uint32_t reload_value = MAX_PIT_FREQ / freq;

	//send command (Channel 0, lobyte/hybyte, square wave generator)
	outb(CMD_NUM, CMD_PORT);

	//split reload_value into top and bottom 8 bits
	uint8_t b = (uint8_t)(reload_value & 0xFF);
	uint8_t t = (uint8_t)((reload_value >> 8) & 0xFF);

	//send to PIT
	outb(b, PIT_PORT);
	outb(t, PIT_PORT);
	enable_irq(PIT_IRQ);

	return;
}

/*
 * pit_handle_interrupt
 * DESCRIPTION: Handle the PIT interrupt
 * INPUTS: none
 * SIDE EFFECTS: none
 * RETURN VALUE: none
 */
void pit_handle_interrupt(void) {

	// First three pit counters should spawn root shell procs
	// Hopefully people cannot type any other program in shell faster than pit
	if (pit_counter < 3) {
		pit_counter++;
		execute((unsigned char*)"shell");
	}
	else {
		context_switch();
	}
	send_eoi(PIT_IRQ);
}
