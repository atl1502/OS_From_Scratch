/* pit.h - Defines used in interactions with the PIT
 * vim:ts=4 noexpandtab
 */
 #ifndef PIT_H
 #define PIT_H

#include "../types.h"
#include "../fd.h"

#define MAX_PIT_FREQ 1193182
#define PIT_IRQ 0
#define CMD_PORT 0x43
#define PIT_PORT 0x40
#define CMD_NUM 0x36

/* Initialize the PIT */
void pit_init(uint32_t freq);

/* Handle the PIT interrupt */
void pit_handle_interrupt(void);

#endif // PIT_H
