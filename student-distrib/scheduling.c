#include "x86_desc.h"
#include "paging.h"
#include "pcb.h"
#include "syscall_wrapper.h"

static uint8_t pit_count = 0;

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

	// First three pit counters should spawn root shell procs
	// Hopefully people cannot type any other program in shell faster than 35 Hz
	if (pit_count < 3) {
		pit_count++;
		execute((unsigned char*)"shell");
	}
	if (pit_count < 3) {
		printf("EXTREMLEY SUSS, you managed to return from SHELL");
	}

	// Switch to new PID
	pid = (pid + 1) % 3;

	// Get new paging directory (reloads TLB)
	context_switch_paging(pid);

	// Gets task stack for new proc
	task_stack = (task_stack_t*) (K_PAGE_ADDR - (EIGHT_KB * (pid+1)));
	pcb = &(task_stack->task_pcb);
	
	// Switch Stacks and set TSS
	asm volatile (
		"movl %1, %0\n\t"
		: "=r"(tss.esp0)
		: "r"(pcb->curr_esp)
		);

	asm volatile (
		"movl %0, %%ebp\n\t"
		"leave\n\t"
		"ret\n\t"
		:
		: "r"(pcb->curr_ebp)
		);

	return 0;
}
