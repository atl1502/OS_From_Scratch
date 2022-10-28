#ifndef PCB
#define PCB

#include "filesystem.h"
#include "fd.h"
#include "types.h"

typedef struct pcb {
    int pid;
    int parent_id;
    fd_t fd;
    uint32_t esp;
    uint32_t eip;
    
    int active; // 1 if active/started
} pcb_t;

pcb_t* get_pcb();

#endif
