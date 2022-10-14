#include "idt.h"
#include "x86_desc.h"
#include "lib.h"

static isr_t interrupt_handlers[256]; // array of interrupt handlers, only needs 16 (2 PICs)

void idt_init() {
    int i;
    // INTERRUPT GATE
    //     P  |DPL| 0  D  1  1 0 0 0 0
    // BIT 15 14 13 12 11 10 9 8 7 6 5
    // based on https://courses.engr.illinois.edu/ece391/fa2022/secure/references/IA32-ref-manual-vol-3.pdf
    // diagram 5-2 pg 156

    // for every IDT vector
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].present = 1; // bit 15
        idt[i].dpl = 000; // Descriptor privledge level, bit 14-13
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

    //irq

}

void register_interrupt_handler(int n) {

}

// Exception handlers
void handler_divide() {
    prinf("divide error exception");
    while(1) {}
}

void handler_debug() {
    prinf("debug exception");
    while(1) {}
}

void handler_nmi() {
    prinf("NMI exception");
    while(1) {}
}

void handler_breakpoint() {
    prinf("breakpoint exception");
    while(1) {}
}

void handler_overflow() {
    prinf("overflow exception");
    while(1) {}
}

void handler_bounds() {
    prinf("BOUND range exceeded exception");
    while(1) {}
}

void handler_inv_opcode() {
    prinf("invalid opcode exception");
    while(1) {}
}

void handler_dev_na() {
    prinf("device not available exception");
    while(1) {}
}

void handler_doub_fault() {
    prinf("double fault exception");
    while(1) {}
}

void handler_cso() {
    prinf("coproc segment overrun exception");
    while(1) {}
}

void handler_inv_tss() {
    prinf("invalid TSS exception");
    while(1) {}
}

void handler_seg_np() {
    prinf("segment not present exception");
    while(1) {}
}

void handler_stk_fault() {
    prinf("stack fault exception");
    while(1) {}
}

void handler_gen_prot() {
    prinf("general protection fault exception");
    while(1) {}
}

void handler_page_fault() {
    prinf("page fault exception");
    while(1) {}
}

void handler_fpu_error() {
    prinf("x87 FPU floating point error exception");
    while(1) {}
}

void handler_align_chk() {
    prinf("alignment check exception");
    while(1) {}
}

void handler_machine_chk() {
    prinf("machine check exception");
    while(1) {}
}

void handler_simd_fp() {
    prinf("SIMD floating point exception");
    while(1) {}
}
