#ifndef PCB
#define PCB

#include "drivers/filesystem.h"
#include "fd.h"
#include "types.h"

// Each task can have up to 8 open files
#define MAX_FILES 8
#define K_PAGE_ADDR 0x8000000
#define EIGHT_KB 0x2000


typedef struct pcb {
	int pid;
	int parent_id;
	fd_t fd_array[MAX_FILES];
	uint32_t k_esp;
	uint32_t k_ebp;
	uint32_t u_esp;
	uint32_t u_ebp;

	int active; // 1 if active/started
} pcb_t;

pcb_t* get_pcb(int pid);

typedef struct task_stack {
	int8_t kernel_stack[8192 - sizeof(pcb_t)];
	pcb_t task_pcb;
} task_stack_t;

#endif
