/* rtc.h - Defines used in interactions with the RTC
 * vim:ts=4 noexpandtab
 */
 #ifndef RTC_H
 #define RTC_H


#include "../types.h"

#define PORT1 0x70
#define PORT2 0x71
#define REGA 0x8A
#define REGB 0x8B
#define REGC 0x0C
#define RNMI 0x7F
#define RTC_IRQ 8

#define FREQ_ST 1024
#define RT_ST 6
#define FREQ_OP 2
#define RT_OP 15

int rate;
int frequency;

/* Externally-visible functions */

/* Initialize the RTC */
void rtc_init(void);

/* Handle the RTC interrupt */
void rtc_handle_interrupt(void);

/* Open function of rtc, note not currently using spinlock */
int rtc_open(void);

/* Close function of rtc */
int rtc_close(void);

/* Read function of rtc */
int rtc_read(void);

/* Write function of rtc, note not currently using spinlock */
int rtc_write(int*);

/* Helper function of rtc for write */
int ret_rate(int);

#endif // RTC_H
