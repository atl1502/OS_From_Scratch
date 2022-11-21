#include "x86_desc.h"
#include "paging.h"
#include "pcb.h"

int context_switch() {

	// Current Task stack
	task_stack_t * task_stack = (task_stack_t*) (K_PAGE_ADDR - (EIGHT_KB * (pid+1)));
	pcb_t * pcb = &(task_stack->task_pcb);

	// Save previous process' stack pointer into PCB
	asm volatile (
		"movl %%esp, %[pcb_esp]\n\t"
		"movl %%ebp, %[pcb_ebp]\n\t"
		: [pcb_esp] "=g"(pcb->curr_esp), [pcb_ebp] "=g"(pcb->curr_ebp)
		);

	// Switch to new PID
	pid = (pid + 1) % 3;

	// Get new paging directory (reloads TLB)
	context_switch_paging(pid);

	// Gets task stack for new proc
	task_stack = (task_stack_t*) (K_PAGE_ADDR - (EIGHT_KB * (pid+1)));
	pcb = &(task_stack->task_pcb);
	
	// Switch Stacks and set TSS
	asm volatile (
		"movl %[pcb_esp], %%esp\n\t"
		"movl %[pcb_ebp], %%ebp\n\t"
		"movl %[pcb_esp], %[tss_esp]\n\t"
		: [tss_esp] "=g"(tss.esp0)
		: [pcb_esp] "g"(pcb->curr_esp), [pcb_ebp] "g"(pcb->curr_ebp)
		);

	return 0;
}
