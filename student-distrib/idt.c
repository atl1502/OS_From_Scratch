#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "idt.h"

// static isr_t interrupt_handlers[256]; // array of interrupt handlers, only needs 16 (2 PICs)

void register_interrupt_handler(int n) {
    
}

// Exception handlers
void handler_divide() {
    printf("divide error exception");
    while(1) {}
}

void handler_debug() {
    printf("debug exception");
    while(1) {}
}

void handler_nmi() {
    printf("NMI exception");
    while(1) {}
}

void handler_breakpoint() {
    printf("breakpoint exception");
    while(1) {}
}

void handler_overflow() {
    printf("overflow exception");
    while(1) {}
}

void handler_bounds() {
    printf("BOUND range exceeded exception");
    while(1) {}
}

void handler_inv_opcode() {
    printf("invalid opcode exception");
    while(1) {}
}

void handler_dev_na() {
    printf("device not available exception");
    while(1) {}
}

void handler_doub_fault() {
    printf("double fault exception");
    while(1) {}
}

void handler_cso() {
    printf("coproc segment overrun exception");
    while(1) {}
}

void handler_inv_tss() {
    printf("invalid TSS exception");
    while(1) {}
}

void handler_seg_np() {
    printf("segment not present exception");
    while(1) {}
}

void handler_stk_fault() {
    printf("stack fault exception");
    while(1) {}
}

void handler_gen_prot() {
    printf("general protection fault exception");
    while(1) {}
}

void handler_page_fault() {
    printf("page fault exception");
    while(1) {}
}

void handler_fpu_error() {
    printf("x87 FPU floating point error exception");
    while(1) {}
}

void handler_align_chk() {
    printf("alignment check exception");
    while(1) {}
}

void handler_machine_chk() {
    printf("machine check exception");
    while(1) {}
}

void handler_simd_fp() {
    printf("SIMD floating point exception");
    while(1) {}
}

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
