/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "../lib.h"
#include "../spinlock.h"
#include "../i8259.h"

static spinlock_t rtc_lock = SPIN_LOCK_UNLOCKED;

/* Initialize the RTC */
void rtc_init(void) {
    //disable interrupts
    spin_lock_irq(&rtc_lock);
    // select register B, and disable NMI
    outb(REGB, PORT1);
    // read the current value of register B
    char prev=inb(PORT2);
    // set the index again (a read will reset the index to register D)
    outb(REGB, PORT1);
    // write the previous value ORed with 0x40. This turns on bit 6 of register B
    outb(prev | 0x40, PORT2);
    //unmask interrupt line on PIC
    enable_irq(RTC_IRQ);
    //reinable NMI
    outb(inb(PORT1) & RNMI, PORT1);
    //garbage throw out to prevent RTC from going into undefined state
    inb(PORT2);
    // reenable interrupts
    spin_unlock_irq(&rtc_lock);
}

/* Handle the RTC interrupt */
void rtc_handle_interrupt(void) {
    // select register C
    outb(REGC, PORT1);
    // just throw away contents
    inb(PORT2);
    test_interrupts();
    //send EOI to PIC
    send_eoi(RTC_IRQ);
}
