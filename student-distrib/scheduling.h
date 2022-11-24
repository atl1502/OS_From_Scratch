#ifndef SCHEDULE
#define SCHEDULE

#define BASE_PROC 3

extern uint8_t schedule[BASE_PROC];
extern uint8_t running_proc;
int context_switch();

#endif
