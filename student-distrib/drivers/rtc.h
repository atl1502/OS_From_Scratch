/* rtc.h - Defines used in interactions with the RTC
 * vim:ts=4 noexpandtab
 */
 #ifndef RTC_H
 #define RTC_H


#include "../types.h"
#include "../fd.h"

#define PORT1 0x70
#define PORT2 0x71
#define REGA 0x8A
#define REGB 0x8B
#define REGC 0x0C
#define RNMI 0x7F
#define RTC_IRQ 8

#define FREQ_ST 1024
#define FREQ_MAX 32768
#define RT_ST 6
#define FREQ_OP 2
#define RT_OP 15

/* Externally-visible functions */
int rate;
int frequency;
int count;
int print_flag;

/* Initialize the RTC */
void rtc_init(void);

/* Handle the RTC interrupt */
void rtc_handle_interrupt(void);

/* Open function of rtc, note not currently using spinlock */
int32_t rtc_open(const uint8_t* filename);

/* Close function of rtc */
int32_t rtc_close(uint32_t fd);

/* Read function of rtc */
int32_t rtc_read(uint32_t fd, void* buf, int32_t nbytes);

/* Write function of rtc, note not currently using spinlock */
int32_t rtc_write(uint32_t fd, const void* buf, int32_t nbytes);

/* Helper function of rtc for write */
int ret_rate(int);

/* Helper function for counting */
int counter();

/* Helper function for counting */
void count_init(int);

/* Helper function to turn on print_flag */
void print_on();

/* Helper function to turn off print_flag */
void print_off();

#endif // RTC_H
