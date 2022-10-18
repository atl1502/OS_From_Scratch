/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#include "spinlock.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
static uint8_t master_mask; /* IRQs 0-7  */
static uint8_t slave_mask;  /* IRQs 8-15 */
static spinlock_t i8259A_lock = SPIN_LOCK_UNLOCKED;

/*
 * i8259_init
 * DESCRIPTION: Initialize both PICs (slave connected to IRQ2)
 * INPUTS: none
 * SIDE EFFECTS: Masks out all of PICs IRQs except for IRQ2 on master
 * RETURN VALUE: none
 */
/* Initialize the 8259 PIC */
void i8259_init(void) {

    unsigned long flags;
    spin_lock_irqsave(&i8259A_lock, &flags);

    // mask pics
    master_mask = 0xFF;
    slave_mask = 0xFF;
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

    // mask pics
    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);

    // enable slave connected port
    enable_irq(SLAVE_ATTACHED);

    spin_unlock_irqrestore(&i8259A_lock, &flags);

}

/*
 * enable_irq
 * DESCRIPTION: Enables a given IRQ
 * INPUTS: irq_num: number of IRQ to enable (0-15)
 * SIDE EFFECTS: Unmasks the given IRQ
 * RETURN VALUE: none
 */
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    // check for valid irq num
    if (irq_num > 15){
        asm volatile("int $15");
    }
    // if irq is for master pic
    if(irq_num < PIC_SIZE){
        // if irq is for master pic
        master_mask = inb(MASTER_8259_DATA) & ~(MASK_RIGHT << irq_num);
        outb(master_mask, MASTER_8259_DATA);
    } else {
        // if irq is for slave pic, get local irq number
        irq_num = irq_num - PIC_SIZE;
        slave_mask = inb(SLAVE_8259_DATA) & ~(MASK_RIGHT << irq_num);
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/*
 * disable_irq
 * DESCRIPTION: Disables a given IRQ
 * INPUTS: irq_num: number of IRQ to disable (0-15)
 * SIDE EFFECTS: Masks the given IRQ
 * RETURN VALUE: none
 */
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    // check for valid irq num
    if (irq_num > 15){
        asm volatile("int $15");
    }
    if(irq_num < PIC_SIZE){
        // if irq is for master pic
        master_mask = inb(MASTER_8259_DATA) | (MASK_RIGHT << irq_num);
        outb(master_mask, MASTER_8259_DATA);
    } else {
        // if irq is for slave pic, get local irq number
        irq_num = irq_num - PIC_SIZE;
        slave_mask = inb(SLAVE_8259_DATA) | (MASK_RIGHT << irq_num);
        outb(slave_mask, SLAVE_8259_DATA);
    }
}

/*
 * send_eoi
 * DESCRIPTION: Ends EOI to PIC
 * INPUTS: irq_num: IRQ which EOI  is being used
 * SIDE EFFECTS: Renabled PIC since EOI is sent
 * RETURN VALUE: none
 */
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // check for valid irq num
    if (irq_num > 15){
        asm volatile("int $15");
    }
    // pick which pic to send EOI to
    if(irq_num >= PIC_SIZE){
		outb(EOI|(irq_num - PIC_SIZE), SLAVE_8259_PORT);
        outb(EOI|SLAVE_ATTACHED, MASTER_8259_PORT);
    } else {
        outb(EOI|irq_num, MASTER_8259_PORT);
    }
}
