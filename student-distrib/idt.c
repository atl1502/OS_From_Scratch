#include "idt.h"
#include "x86_desc.h"
#include "lib.h"

void idt_init() {
    int i;
    // INTERRUPT GATE
    //     P  |DPL| 0  D  1  1 0 0 0 0
    // BIT 15 14 13 12 11 10 9 8 7 6 5

    // for every IDT vector
    for (i = 0; i < NUM_VEC; i++) {
        idt[i].present = 1; // bit 15
        idt[i].dpl = 000; // Descriptor privledge level, bit 14-13 
        idt[i].reserved0 = 0; // bit 12
        idt[i].size = ; // D size of gate: 1 = 32 bits, 0 = 16 bits // bit 11
        idt[i].reserved1 = 1; // bit 10
        idt[i].reserved2 = 1; // bit 9
        idt[i].reserved3 = 0; // bit 8

        idt[i].seg_selector = KERNEL_CS;
        SET_IDT_ENTRY(idt[i], exception_handler);
    }
}