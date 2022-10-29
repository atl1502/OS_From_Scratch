#ifndef PCB
#define PCB

#include "drivers/filesystem.h"
#include "fd.h"
#include "types.h"

#define MAX_FILES 8 // Each task can have up to 8 open files.

typedef struct pcb {
	int pid;
	int parent_id;
	fd_t fd[MAX_FILES];
	uint32_t esp;
	uint32_t eip;
	
	int active; // 1 if active/started
} pcb_t;

pcb_t* get_pcb();

#endif
