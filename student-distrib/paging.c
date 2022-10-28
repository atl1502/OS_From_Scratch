#include "x86_desc.h"
#include "paging.h"

// keep track of next process page directory to be allocated
static int process_in_use[MAX_PROCESSES] = {0};
static void* process_pds[MAX_PROCESSES] = {pd_p0,pd_p1,pd_p2,pd_p3,pd_p4,pd_p5};
/*
 * paging_init
 * DESCRIPTION: Initialize the paging
 * INPUTS: none
 * SIDE EFFECTS: sets cr3 and changes bits in cr0, cr4
 * RETURN VALUE: none
 */
void paging_init(){
    /*
     * Fill in PDE for 0-4MB Page table
     * For this paging Entry:
     * 11-9 Avail, 8 G, 7 PS, 6 '0', 5 Accessed, 4 Cache Disabled, 3 PWT, 2 U/S, 1 R/W, 0 P
     *     000      1     0     0          0              0          0      0       1    1
     * which is 0x103 for last 12 bits
     * First 24 bits is page table address
    */
    page_directory[0] = ((uint32_t) page_table) | GLOBAL | RW | P;

    /*
     * Fill in PDE for 4-8MB Kernel Big Page
     * For this paging Entry:
     * 21-13 '0' 12 PAT 11-9 Avail, 8 G, 7 PS, 6 '0', 5 Accessed, 4 Cache Disabled, 3 PWT, 2 U/S, 1 R/W, 0 P
     * 000000000   0        000      1     1     0          0              0          0      0       1    1
     * Which is 0x183 for the last 12 bits
     * First 10 bits is page address, which is 1, since it is the second 4MB block (0 indexed)
    */
    page_directory[KERNAL_PAGE_ADDR] = (KERNAL_PAGE_ADDR << ADDR_OFFSET) | GLOBAL | PS | RW | P;

    /*
     * Fill in entry for address 0xB8, which is the page index for video mem
     * For this paging entry
     * 11-9 Avail, 8 G, 7 PS, 6 '0', 5 Accessed, 4 Cache Disabled, 3 PWT, 2 U/S, 1 R/W, 0 P
     *     000      1     0     0          0              0          0      0       1    1
     * which is 0x103 for the last 12 bits
     * B8 for next eight bits to represent VGA 4KB aligned address
     */
    page_table[0xB8] = 0xB8103;

    /* Writing to registers to enable paging */
    uint32_t cr0, cr4;

    /* Write PDE Addr to CR3 */
    asm volatile("movl %0, %%cr3":: "r"(page_directory));

    /* Enable PSE for large page */
    asm volatile("mov %%cr4, %0": "=r"(cr4));
    cr4 |= 0x10;
    asm volatile("mov %0, %%cr4":: "r"(cr4));

    /* Enable Paging */
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

/*
 * alloc_new_process
 * DESCRIPTION: allocates a new page and sets up a PD
 * INPUTS: None
 * SIDE EFFECTS: Finds an open PD and sets it up
 * RETURN VALUE: the PID or -1 if no open PDs
 */
int alloc_new_process(){
    int i;
    int* cur_pd;
    // find an open PD to use
    for (i = 0; i < MAX_PROCESSES; i++){
        if (process_in_use[i] == 0){
            break;
        }
    }
    // check if there isn't any open processes
    if(i == MAX_PROCESSES){
        return -1;
    }
    // mark process as in use
    process_in_use[i] = 1;
    cur_pd = (int*)process_pds[i];
    // the page directory is set up as specified in Appendix C
    // First 8 MB were set to global so no need to change
    // 128 MB location program, set up big page (kernal page + PID)
    cur_pd[USER_ADDR] = ((KERNAL_PAGE_ADDR+i) << ADDR_OFFSET) | PS | RW | P;
    return i;
}

/*
 * dealloc_process
 * DESCRIPTION: Removes a given processes page from the PD
 * CALL AFTER CONTEXT SWITCH TO PARENT
 * INPUTS:
 * int pid: the PID of the process you want to stop
 * SIDE EFFECTS: Resets a given PDE 0s.
 * RETURN VALUE: -1: virtual_addr is already empty/invalid param   0: on success
 */
 int dealloc_process(int pid){
     int i;
     int* cur_pd;
     // make sure current process is actually in use
     if(process_in_use[pid] == 0){
         return -1;
     }
     // set inuse to 0
     process_in_use[pid] = 0;
     // clear the PD for the given process
     cur_pd = (int*)process_pds[pid];
     for(i = 0; i < TABLE_SIZE; i++){
         cur_pd[i] = 0;
     }
     return 0;
 }

 /*
  * context_switch_paging
  * DESCRIPTION: Switches PD to a given pid
  * CALL BEFORE CONTEXT DEALLOC
  * INPUTS:
  * int pid: the PID of the process you want to stop
  * SIDE EFFECTS: Changes value in cr3
  * RETURN VALUE: -1: process not occuring   0: on success
  */
 int context_switch_paging(int pid){
     if(process_in_use[pid] == 0){
         return -1;
     }
     asm volatile("movl %0, %%cr3":: "r"(process_pds[pid]));
     return 0;
 }
