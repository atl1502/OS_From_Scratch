#ifndef PCB
#define PCB

#include "drivers/filesystem.h"
#include "fd.h"
#include "types.h"

// Each task can have up to 8 open files
#define MAX_FILES 8
#define K_PAGE_ADDR 0x800000
#define EIGHT_KB 0x2000
#define BUF_LEN 128

typedef struct pcb {
	int pid;
	int parent_id;
	fd_t fd_array[MAX_FILES];
	uint32_t par_esp;
	uint32_t par_ebp;
	uint32_t curr_esp;
	uint32_t curr_ebp;
	uint8_t cmd[BUF_LEN];
	uint8_t arg[BUF_LEN];
	int active; // 1 if active/started
	int vid_flag;
} pcb_t;

pcb_t* get_pcb(int pid);

typedef struct task_stack {
	pcb_t task_pcb;
	int8_t kernel_stack[EIGHT_KB - sizeof(pcb_t)];
} task_stack_t;

#endif
