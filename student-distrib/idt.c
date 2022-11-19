#include "x86_desc.h"
#include "lib.h"
#include "handlers.h"
#include "i8259.h"
#include "idt.h"
#include "syscalls.h"
#include "drivers/pit.h"
#include "drivers/keyboard.h"
#include "drivers/rtc.h"

// Exception handlers

/*
 * handler_divide
 * DESCRIPTION: Indicates the divisor operand for a DIV or IDIV instruction is 0 or that the result cannot be represented
 * in the number of bits specified for the destination operand.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_divide() {
	clear();
	printf("divide by 0 error exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_debug
 * DESCRIPTION: Indicates that one or more of several debug-exception conditions has been detected.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_debug() {
	clear();
	printf("debug exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_nmi
 * DESCRIPTION: The nonmaskable interrupt (NMI) is generated externally by asserting the processor’s NMI pin
 * or through an NMI request set by the I/O APIC to the local APIC. This interrupt causes the NMI
 * interrupt handler to be called.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_nmi() {
	clear();
	printf("NMI exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_breakpoint
 * DESCRIPTION: Indicates that a breakpoint instruction (INT 3) was executed, causing a breakpoint trap to be
 * generated. Typically, a debugger sets a breakpoint by replacing the first opcode byte of an
 * instruction with the opcode for the INT 3 instruction.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_breakpoint() {
	clear();
	printf("breakpoint exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_overflow
 * DESCRIPTION: Indicates that an overflow trap occurred when an INTO instruction was executed. The INTO
 * instruction checks the state of the OF flag in the EFLAGS register. If t`e OF flag is set, an overflow
 * trap is generated.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_overflow() {
	clear();
	printf("overflow exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_bounds
 * DESCRIPTION: Indicates that a BOUND-range-exceeded fault occurred when a BOUND instruction was
 * executed. The BOUND instruction checks that a signed array index is within the upper and
 * lower bounds of an array located in memory.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_bounds() {
	clear();
	printf("BOUND range exceeded exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_inv_opcode
 * DESCRIPTION: Attempted to execute an invalid or reserved opcode.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_inv_opcode() {
	clear();
	printf("invalid opcode exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_dev_na
 * DESCRIPTION: Indicates that the device-not-available exception is generated.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_dev_na() {
	clear();
	printf("device not available exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_doub_fault
 * DESCRIPTION: Indicates that the processor detected a second exception while calling an exception handler for
 * a prior exception.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_doub_fault() {
	clear();
	printf("double fault exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_cso
 * DESCRIPTION: Indicates that an Intel386 CPU-based systems with an Intel 387 math coprocessor detected a
 * page or segment violation while transferring the middle portion of an Intel 387 math coprocessor
 * operand.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_cso() {
	clear();
	printf("coproc segment overrun exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_inv_tss
 * DESCRIPTION: Indicates that there was an error related to a TSS.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_inv_tss() {
	clear();
	printf("invalid TSS exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_seg_np
 * DESCRIPTION: Indicates that the present flag of a segment or gate descriptor is clear.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_seg_np() {
	clear();
	printf("segment not present exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_stk_fault
 * DESCRIPTION: A limit violation is detected during an operation that refers to the SS register OR
 * A not-present stack segment is detected when attempting to load the SS register was detected.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_stk_fault() {
	clear();
	printf("stack fault exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_gen_prot
 * DESCRIPTION: Indicates that the processor detected one of a class of protection violations called “generalprotection
 * violations.”
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_gen_prot() {
	clear();
	printf("general protection fault exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_page_fault
 * DESCRIPTION: Indicates that, with paging enabled (the PG flag in the CR0 register is set), the processor
 * detected a paging translation related error.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_page_fault() {
	clear();
	printf("page fault exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_assertion_failure
 * DESCRIPTION: Generic default assertion failure
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_assertion_failure() {
	clear();
	printf("assertion failure");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_fpu_error
 * DESCRIPTION: Indicates that the x87 FPU has detected a floating-point error.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_fpu_error() {
	clear();
	printf("x87 FPU floating point error exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_align_chk
 * DESCRIPTION: Indicates that the processor detected an unaligned memory operand when alignment checking
 * was enabled.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_align_chk() {
	clear();
	printf("alignment check exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_machine_chk
 * DESCRIPTION: Indicates that the processor detected an internal machine error or a bus error, or that an external
 * agent detected a bus error.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_machine_chk() {
	clear();
	printf("machine check exception");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * handler_simd_fp
 * DESCRIPTION: Indicates the processor has detected an SSE/SSE2/SSE3 SIMD floating-point exception. The
 * appropriate status flag in the MXCSR register must be set and the particular exception
 * unmasked for this interrupt to be generated.
 * INPUTS: no inputs
 * SIDE EFFECTS: clears console, prints exception message
 * RETURN VALUE: never returns
 */
void handler_simd_fp() {
	clear();
	printf("SIMD floating point exception");
	sys_halt (EXCEPTION_ERROR);
}

void handler_sys_call() {
	clear();
	printf("System Call");
	sys_halt (EXCEPTION_ERROR);
}

/*
 * do_IRQ
 * DESCRIPTION: runs given interrupt handler
 * INPUTS: current registers status
 * SIDE EFFECTS: initialises IRQ pointer array, and calls the given IRQ's handler
 * RETURN VALUE: 1
 */
unsigned int do_IRQ(prev_reg_t regs) {
	int irq = ~(regs.IRQ);
	// check for valid irq
	if (irq > 15){
		asm volatile("int $15");
	}
	// function ptr array
	void (*irq_desc[IRQT_S])(void) = {0};
	// set irqs in the array
	irq_desc[PIT_IRQ] = pit_handle_interrupt;
	irq_desc[KB_IRQ] = keyboard_handle_interrupt;
	irq_desc[RTC_IRQ] = rtc_handle_interrupt;
	// call given handler based on irq
	(*irq_desc[irq])();
	// clear eax back to 0 i think?
	regs.IRQ += irq;
	return 1;
}

/*
 * idt_init
 * DESCRIPTION: creates IDT, fills inital values within IDT
 * INPUTS: none
 * SIDE EFFECTS: IDT is now initialised
 * RETURN VALUE: none
 */
void idt_init() {
	int i;
	// INTERRUPT GATE
	//     P  |DPL| 0  D  1  1 0 0 0 0
	// BIT 15 14 13 12 11 10 9 8 7 6 5
	// based on https://courses.engr.illinois.edu/ece391/fa2022/secure/references/IA32-ref-manual-vol-3.pdf
	// diagram 5-2 pg 156

	// init drivers for rtc/keyboard
	pit_init(1000);
	keyboard_init();
	rtc_init();

	// for every IDT vector
	for (i = 0; i < NUM_VEC; i++) {
		idt[i].present = 1; // bit 15
		idt[i].dpl = 0;
		idt[i].reserved0 = 0; // bit 12
		idt[i].size = 1; // D size of gate: 1 = 32 bits, 0 = 16 bits // bit 11
		idt[i].reserved1 = 1; // bit 10
		idt[i].reserved2 = 1; // bit 9
		idt[i].reserved3 = 0; // bit 8

		idt[i].seg_selector = KERNEL_CS;
	}
	// syscalls are callable from ring 3
	idt[SYSCA].dpl = 3;

	SET_IDT_ENTRY(idt[0], handler_divide); // divide error exception
	SET_IDT_ENTRY(idt[1], handler_debug); // debug exception
	SET_IDT_ENTRY(idt[2], handler_nmi); // NMI exception
	SET_IDT_ENTRY(idt[3], handler_breakpoint); // breakpoint exception
	SET_IDT_ENTRY(idt[4], handler_overflow); // overflow exception
	SET_IDT_ENTRY(idt[5], handler_bounds); // BOUND range exceeded
	SET_IDT_ENTRY(idt[6], handler_inv_opcode); // invalid opcode exception
	SET_IDT_ENTRY(idt[7], handler_dev_na); // device not available exception
	SET_IDT_ENTRY(idt[8], handler_doub_fault); // double fault exception
	SET_IDT_ENTRY(idt[9], handler_cso); // coproc segment overrun exception
	SET_IDT_ENTRY(idt[10], handler_inv_tss); // invalid TSS exception
	SET_IDT_ENTRY(idt[11], handler_seg_np); // segment not present exception
	SET_IDT_ENTRY(idt[12], handler_stk_fault); // stack fault exception
	SET_IDT_ENTRY(idt[13], handler_gen_prot); // general protection fault exception
	SET_IDT_ENTRY(idt[14], handler_page_fault); // page fault exception
	SET_IDT_ENTRY(idt[15], handler_assertion_failure);
	SET_IDT_ENTRY(idt[16], handler_fpu_error); // x87 FPU floating point error exception
	SET_IDT_ENTRY(idt[17], handler_align_chk); // alignment check exception
	SET_IDT_ENTRY(idt[18], handler_machine_chk); // machine check exception
	SET_IDT_ENTRY(idt[19], handler_simd_fp); // SIMD floating point exception

	//irq
	SET_IDT_ENTRY(idt[0x20], handler_pit);
	SET_IDT_ENTRY(idt[0x21], handler_keyboard);
	SET_IDT_ENTRY(idt[0x28], handler_rtc);

	// syscall
	SET_IDT_ENTRY(idt[SYSCA], system_call);

}
