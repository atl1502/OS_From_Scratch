#include "pcb.h"
#include "lib.h"
#include "types.h"

// PCB is located at the TOP of current kernel stack
// stack grows upside down
// PCB is located at top of 8kb block

pcb_t* get_pcb(int pid) {
	// 8MB - 8KB times the pid number plus one since its at bottom
	return ((pcb_t*)K_PAGE_ADDR-((pid+1)*EIGHT_KB));
}
