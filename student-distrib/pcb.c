#include "pcb.h"
#include "lib.h"
#include "types.h"

// PCB is located at the TOP of current kernel stack
// stack grows upside down
// PCB is located at top of 8kb block

pcb_t* get_pcb() {
	// gets current stack pointer
	// void* sp = NULL;

	// get curr PCB using bitmask with trick from top of file
	// 2^13 = 8KB
	// 0xE000 clear least significant 13 bits
	// uint32_t pcb = ( ((uint32_t) (&sp)) & 0xE000 );

	// how do i get what i want from this
	return 0;
}
