#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "idt.h"
#include "drivers/keyboard.h"
#include "drivers/rtc.h"

// Exception handlers
void handler_divide() {
    clear();
    printf("divide by 0 error exception");
    while(1) {}
}

void handler_debug() {
    clear();
    printf("debug exception");
    while(1) {}
}

void handler_nmi() {
    clear();
    printf("NMI exception");
    while(1) {}
}

void handler_breakpoint() {
    clear();
    printf("breakpoint exception");
    while(1) {}
}

void handler_overflow() {
    clear();
    printf("overflow exception");
    while(1) {}
}

void handler_bounds() {
    clear();
    printf("BOUND range exceeded exception");
    while(1) {}
}

void handler_inv_opcode() {
    clear();
    printf("invalid opcode exception");
    while(1) {}
}

void handler_dev_na() {
    clear();
    printf("device not available exception");
    while(1) {}
}

void handler_doub_fault() {
    clear();
    printf("double fault exception");
    while(1) {}
}

void handler_cso() {
    clear();
    printf("coproc segment overrun exception");
    while(1) {}
}

void handler_inv_tss() {
    clear();
    printf("invalid TSS exception");
    while(1) {}
}

void handler_seg_np() {
    clear();
    printf("segment not present exception");
    while(1) {}
}

void handler_stk_fault() {
    clear();
    printf("stack fault exception");
    while(1) {}
}

void handler_gen_prot() {
    clear();
    printf("general protection fault exception");
    while(1) {}
}

void handler_page_fault() {
    clear();
    printf("page fault exception");
    while(1) {}
}

void handler_fpu_error() {
    clear();
    printf("x87 FPU floating point error exception");
    while(1) {}
}

void handler_align_chk() {
    clear();
    printf("alignment check exception");
    while(1) {}
}

void handler_machine_chk() {
    clear();
    printf("machine check exception");
    while(1) {}
}

void handler_simd_fp() {
    clear();
    printf("SIMD floating point exception");
    while(1) {}
}

void handler_sys_call() {
    clear();
    printf("System Call");
    while(1) {}
}

// runs handler
unsigned int do_IRQ(prev_reg_t regs) {
	/* Save old registers */
	prev_reg_t old_reg = regs;
	int irq = ~(regs.IRQ);
    // function ptr array
    void (*irq_desc[16])(void) = {0};
    // set irqs in the array
    irq_desc[1] = keyboard_handle_interrupt;
    irq_desc[8] = rtc_handle_interrupt;
    // call given handler based on irq
    (*irq_desc[irq])();
    // clear eax back to 0 i think?
    regs.IRQ += irq;
    return 1;
}


void idt_init() {
    int i;
    // INTERRUPT GATE
    //     P  |DPL| 0  D  1  1 0 0 0 0
    // BIT 15 14 13 12 11 10 9 8 7 6 5
    // based on https://courses.engr.illinois.edu/ece391/fa2022/secure/references/IA32-ref-manual-vol-3.pdf
    // diagram 5-2 pg 156

    // init drivers for rtc/keyboard
    keyboard_init();
    // rtc_init();

    // for every IDT vector
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].present = 1; // bit 15
        if (i == 0x80){
          idt[i].dpl = 3; // Descriptor privledge level, bit 14-13
        } else{
          idt[i].dpl = 0;
        }
        idt[i].reserved0 = 0; // bit 12
        idt[i].size = 1; // D size of gate: 1 = 32 bits, 0 = 16 bits // bit 11
        idt[i].reserved1 = 1; // bit 10
        idt[i].reserved2 = 1; // bit 9
        idt[i].reserved3 = 0; // bit 8

        idt[i].seg_selector = KERNEL_CS;
    }

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
    // NO INTERRUPT 15
    SET_IDT_ENTRY(idt[16], handler_fpu_error); // x87 FPU floating point error exception
    SET_IDT_ENTRY(idt[17], handler_align_chk); // alignment check exception
    SET_IDT_ENTRY(idt[18], handler_machine_chk); // machine check exception
    SET_IDT_ENTRY(idt[19], handler_simd_fp); // SIMD floating point exception

    // System call
    SET_IDT_ENTRY(idt[0x80], handler_sys_call);

    //irq
    SET_IDT_ENTRY(idt[0x21], handler_keyboard);
    SET_IDT_ENTRY(idt[0x28], handler_rtc);

}
