/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#include "spinlock.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */
static spinlock_t i8259A_lock = SPIN_LOCK_UNLOCKED;
/* Initialize the 8259 PIC */
void i8259_init(void) {

    unsigned char a1, a2;

    unsigned long flags = 0;
    spin_lock_irqsave(&i8259A_lock, flags);
    // save previous masking
    a1 = inb(MASTER_8259_DATA);
    a2 = inb(SLAVE_8259_DATA);

    // mask pics
    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);

    // init, push start in idt, location of secondary
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW3_MASTER, MASTER_8259_DATA);
    outb(ICW4, MASTER_8259_DATA);

    // init, push start in idt, attach location on primary
    outb(ICW1, SLAVE_8259_PORT);
    outb(ICW2_SLAVE , SLAVE_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    // restore maskings
    outb(a1, MASTER_8259_DATA);
    outb(a2, SLAVE_8259_DATA);

    spin_unlock_irqrestore(&i8259A_lock, flags);

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    // if irq is for master pic
    if(irq_num < 8){
        // if irq is for master pic
        master_mask = inb(MASTER_8259_DATA) & ~(1 << irq_num);
        outb(master_mask, MASTER_8259_DATA);
    } else {
        // if irq is for slave pic, get local irq number
        irq_num = irq_num - 8;
        slave_mask = inb(SLAVE_8259_DATA) & ~(1 << irq_num);
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if(irq_num < 8){
        // if irq is for master pic
        slave_mask = inb(MASTER_8259_DATA) | (1 << irq_num);
        outb(slave_mask, MASTER_8259_DATA);
    } else {
        // if irq is for slave pic, get local irq number
        irq_num = irq_num - 8;
        slave_mask = inb(SLAVE_8259_DATA) | (1 << irq_num);
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // pick which pic to send EOI to
    if(irq_num >= 8)
		outb(EOI, SLAVE_8259_PORT);
	outb(EOI, MASTER_8259_PORT);
}
