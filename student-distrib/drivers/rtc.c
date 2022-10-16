/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "../lib.h"
#include "../spinlock.h"
#include "../i8259.h"

static spinlock_t rtc_lock = SPIN_LOCK_UNLOCKED;

/* 
 * rtc_init
 * DESCRIPTION: Initialize the RTC
 * INPUTS: none
 * SIDE EFFECTS: enables IRQ for RTC
 * RETURN VALUE: none
 */
/* Initialize the RTC */
void rtc_init(void) {
    //disable interrupts
    spin_lock_irq(&rtc_lock);
    // select register B, and disable NMI
    outb(0x8B, 0x70);
    // read the current value of register B
    char prev=inb(0x71);
    // set the index again (a read will reset the index to register D)
    outb(0x8B, 0x70);
    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    outb(prev | 0x40, 0x71);
    //unmask interrupt line on PIC
    enable_irq(8);
    //reinable NMI
    outb(inb(0x70) & 0x7F, 0x70);
    //garbage throw out to prevent RTC from going into undefined state
    inb(0x71);
    // reenable interrupts
    spin_unlock_irq(&rtc_lock);
}

/* 
 * rtc_handle_interrupt
 * DESCRIPTION: Handle the RTC interrupt
 * INPUTS: none
 * SIDE EFFECTS: clobbers register C
 * RETURN VALUE: none
 */
void rtc_handle_interrupt(void) {
    // select register C
    outb(0x0C, 0x70);
    // just throw away contents
    inb(0x71);
    test_interrupts();
    //send EOI to PIC
    send_eoi(8);
}
