/* rtc.h - Defines used in interactions with the RTC
 * vim:ts=4 noexpandtab
 */



#include "../types.h"


/* Externally-visible functions */

/* Initialize the RTC */
void rtc_init(void);

/* Handle the RTC interrupt */
void rtc_handle_interrupt(void);