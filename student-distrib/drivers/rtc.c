/* rtc.c - Functions to interact with the RTC
 * vim:ts=4 noexpandtab
 */

#include "rtc.h"
#include "../lib.h"
#include "../spinlock.h"
#include "../i8259.h"
#include "../fd.h"

static spinlock_t rtc_lock = SPIN_LOCK_UNLOCKED;

volatile int flag;

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
    // spin_lock_irq(&rtc_lock);
    cli();
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
    //set rate/freq to be default
    frequency = FREQ_ST;
    rate = RT_ST;
    flag = 0;
    print_flag = 0;
    // reenable interrupts
    // spin_unlock_irq(&rtc_lock);
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
    outb(REGC, PORT1);
    // just throw away contents
    inb(PORT2);
    // test_interrupts();
    if (print_flag) {
        printf("1");
    }
    //send EOI to PIC
    flag = 0;
    count--;
    send_eoi(RTC_IRQ);
}

/*
 * rtc_open
 * DESCRIPTION: provides access to the file system
 * INPUTS: none
 * SIDE EFFECTS: initializes RTC frequency to 2Hz
 * RETURN VALUE: 0
 */
int32_t rtc_open(const uint8_t* filename) {
    frequency = FREQ_OP;
    rate = RT_OP;
    //disable interrupts
    cli();
    // select register A, and disable NMI
    outb(REGA, PORT1);
    // read the current value of register B
    char prev=inb(PORT2);
    // set the index again (a read will reset the index to register D)
    outb(REGA, PORT1);
    // write only our rate to A. Note, rate is the bottom 4 bits.
    outb((prev & 0xF0) | rate, PORT2);
    //reinable NMI
    outb(inb(PORT1) & RNMI, PORT1);
    //garbage throw out to prevent RTC from going into undefined state
    inb(PORT2);
    // reenable interrupts
    sti();
    return 0;
}

/*
 * rtc_close
 * DESCRIPTION: closes the specified file descriptor and makes it available for return from later calls to open
 * INPUTS: none
 * SIDE EFFECTS: none
 * RETURN VALUE: 0
 */
int32_t rtc_close(uint32_t fd) {
    return 0;
}

/*
 * rtc_read
 * DESCRIPTION:  reads data from RTC
 * INPUTS: none
 * SIDE EFFECTS: does not return until interrupt handler handled
 * RETURN VALUE: 0
 */
int32_t rtc_read(uint32_t fd, void* buf, int32_t nbytes) {
    flag = 1;
    while (flag != 0){
        continue;
    }
    return 0;
}

/*
 * rtc_write
 * DESCRIPTION:  writes data to RTC
 * INPUTS: none
 * SIDE EFFECTS: changes RTC frequency
 * RETURN VALUE: 0
 */
int32_t rtc_write(uint32_t fd, const void* buf, int32_t nbytes) {
    //get frequency
    if (buf == NULL) {
        return -1;
    }
    frequency = *((int*)buf);
    //check if frequency is multiple of 2
    if ((frequency == 0) || ((frequency & (frequency - 1)) != 0)) {
        return -1;
    }
    //limit rate from 2 to 1024 Hz
    if ((frequency < 2) || (frequency > 1024)) {
        return -1;
    }
    rate = ret_rate(frequency);
    //disable interrupts
    cli();
    // select register A, and disable NMI
    outb(REGA, PORT1);
    // read the current value of register B
    char prev=inb(PORT2);
    // set the index again (a read will reset the index to register D)
    outb(REGA, PORT1);
    // write only our rate to A. Note, rate is the bottom 4 bits.
    outb((prev & 0xF0) | rate, PORT2);
    //reinable NMI
    outb(inb(PORT1) & RNMI, PORT1);
    //garbage throw out to prevent RTC from going into undefined state
    inb(PORT2);
    // reenable interrupts
    sti();
    return 0;
}

/*
 * ret_rate
 * DESCRIPTION:  converts frequency to rate for rtc
 * INPUTS: none
 * SIDE EFFECTS: none
 * RETURN VALUE: rate integer
 */
int ret_rate(int freq) {
    //default ret value
    if (freq <= 1) {
        return RT_ST;
    }
    int log = 0;
    //logarithm implementation
    while (freq > 1) {
        freq >>= 1;
        log++;
    }
    //rate formula from frequency
    return RT_OP + 1 - log;
}

/*
 * counter
 * DESCRIPTION:  returns count
 * INPUTS: none
 * SIDE EFFECTS: none
 * RETURN VALUE: current count
 */
int counter() {
    return count;
}

/*
 * count_init
 * DESCRIPTION:  initiates count for a given frequency
 * INPUTS: none
 * SIDE EFFECTS: none
 * RETURN VALUE: none
 */
void count_init(int freq) {
    count = 3*freq;
    return;
}

/*
 * print_on
 * DESCRIPTION:  turns on print flag
 * INPUTS: none
 * SIDE EFFECTS: none
 * RETURN VALUE: none
 */
void print_on() {
    print_flag = 1;
    return;
}

/*
 * print_flag
 * DESCRIPTION:  turns off print flag
 * INPUTS: none
 * SIDE EFFECTS: none
 * RETURN VALUE: none
 */
void print_off() {
    print_flag = 0;
    return;
}
