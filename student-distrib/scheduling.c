#include "x86_desc.h"
#include "paging.h"
#include "pcb.h"
#include "syscall_wrapper.h"
#include "scheduling.h"
#include "./drivers/keyboard.h"

uint8_t running_proc = 0;
static uint8_t pit_count = 0;
uint8_t schedule[BASE_PROC] = { 0 };

/*
 * context_switch
 * DESCRIPTION: Allows for switching to the next process without halt
 * INPUTS: None
 * SIDE EFFECTS: Switches execution to a different process based on round robin scheduling
 * RETURN VALUE: 0
 */
int context_switch() {

	cli();

	/* Current Task stack */
	task_stack_t * task_stack = (task_stack_t*) (K_PAGE_ADDR - (EIGHT_KB * (pid+1)));
	pcb_t * pcb = &(task_stack->task_pcb);

	/* Save previous process' stack pointer into PCB */
	asm volatile (
		"movl %%esp, %[pcb_esp]\n\t"
		"movl %%ebp, %[pcb_ebp]\n\t"
		: [pcb_esp] "=g"(pcb->curr_esp), [pcb_ebp] "=g"(pcb->curr_ebp)
		);

	/* Save previously scheduled screen location */
	save_screen(running_proc);

	/*
	* First three pit counters should spawn root shell procs
	* Hopefully people cannot type any other program in shell faster than 35 Hz
	* If they can I am impressed
	*/
	if (pit_count < 3) {
		pit_count++;
		execute((unsigned char*)"shell");
	}
	if (pit_count < 3) {
		printf("EXTREMLEY SUSS, you managed to return from SHELL");
	}

	/* Switch to new process & PID */
	running_proc = (running_proc + 1) % 3;
	pid = schedule[running_proc];

	/* Get new paging directory */
	context_switch_paging(pid);

	/* Gets task stack for new proc */
	task_stack = (task_stack_t*) (K_PAGE_ADDR - (EIGHT_KB * (pid+1)));
	pcb = &(task_stack->task_pcb);

	/* Set screen to currently scheduled process */
	restore_screen(running_proc);

	/* Check current terminal vs proc terminal */
	if (running_proc != term_num) {
		remap(running_proc);
	}
	else {
		unmap();
	}

	/* Restore keyboard buffer to associated process' buffer */
	restoreKeyboardBuf(running_proc);

	/* Switch Stacks and set TSS */
	asm volatile (
		"movl %1, %0\n\t"
		: "=r"(tss.esp0)
		: "r"(pcb->curr_esp)
		);

	/* Return into new context */
	asm volatile (
		"movl %0, %%ebp\n\t"
		"leave\n\t"
		"sti\n\t"
		"ret\n\t"
		:
		: "r"(pcb->curr_ebp)
		: "ebp"
		);

	return 0;
}
