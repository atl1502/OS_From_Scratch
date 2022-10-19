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

/* Externally-visible functions */

/* Initialize the RTC */
void rtc_init(void);

/* Handle the RTC interrupt */
void rtc_handle_interrupt(void);

#endif // RTC_H
