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
     * 11-9 Avail, 8 G, 7 BIG_PAGE, 6 '0', 5 Accessed, 4 Cache Disabled, 3 PWT, 2 U/S, 1 R/W, 0 P
     *     000      1     0     0          0              0          0      0       1    1
     * which is 0x103 for last 12 bits
     * First 24 bits is page table address
    */
    page_directory[0] = ((uint32_t) page_table) | WRITE_ENABLE | PRESENT;

    /*
     * Fill in PDE for 4-8MB Kernel Big Page
     * For this paging Entry:
     * 21-13 '0' 12 PAT 11-9 Avail, 8 G, 7 BIG_PAGE, 6 '0', 5 Accessed, 4 Cache Disabled, 3 PWT, 2 U/S, 1 R/W, 0 P
     * 000000000   0        000      1     1     0          0              0          0      0       1    1
     * Which is 0x183 for the last 12 bits
     * First 10 bits is page address, which is 1, since it is the second 4MB block (0 indexed)
    */
    page_directory[KERNAL_PAGE_ADDR] = (KERNAL_PAGE_ADDR << PD_ADDR_OFFSET) | BIG_PAGE | WRITE_ENABLE | PRESENT;

    /*
     * Fill in entry for address 0xB8, which is the page index for video mem
     * For this paging entry
     * 11-9 Avail, 8 G, 7 BIG_PAGE, 6 '0', 5 Accessed, 4 Cache Disabled, 3 PWT, 2 U/S, 1 R/W, 0 P
     *     000      1     0     0          0              0          0      0       1    1
     * which is 0x103 for the last 12 bits
     * B8 for next eight bits to represent VGA 4KB aligned address
     */
    page_table[0xB8] = 0xB8103;

    /* Writing to registers to enable paging */
    uint32_t cr0, cr4;

    /* Write PDE Addr to CR3 */
    asm volatile("movl %0, %%cr3":: "r"(page_directory));

    /* Enable paging for large page */
    asm volatile("mov %%cr4, %0": "=r"(cr4));
    cr4 |= 0x10;
    asm volatile("mov %0, %%cr4":: "r"(cr4));

    /* Enable Paging */
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "r"(cr0));
}

/*
 * virtual_to_physical
 * DESCRIPTION: allocates a new page and sets up a PD
 * INPUTS: virtual_addr: the address that is to be translated to physical
 * SIDE EFFECTS: None
 * RETURN VALUE: the physical address or 0: for invalid address
 */
uint32_t virtual_to_physical(uint32_t virtual_addr){
    uint32_t pd_addr, page_directory_entry, page_table_entry, ret_addr;
    uint32_t* cur_pd;
    uint32_t* cur_pt;
    uint32_t big_page_mask = BIG_PAGE_MASK;
    uint32_t small_page_mask = SMALL_PAGE_MASK;
    pd_addr = virtual_addr >> PD_ADDR_OFFSET;

    // get the current page directory
    asm volatile("mov %%cr3, %0": "=r"(cur_pd));
    // get the data at current entry
    page_directory_entry = cur_pd[pd_addr];
    // check entry is present
    if((page_directory_entry & PRESENT) == 0){
        return INVALID_ADDR;
    }

    // check if its a big page or small page
    if((page_directory_entry & BIG_PAGE) == 0){
        // 4 KB pages
        cur_pt = (uint32_t*)(page_directory_entry & (~small_page_mask));
        // get entry in page table (middle 10 bits of virual address)
        page_table_entry = *(cur_pt+((virtual_addr << VIRT_PT_OFFSET)>>PD_ADDR_OFFSET));
        // check if the entry is present
        if((page_table_entry & PRESENT) == 0){
            return INVALID_ADDR;
        }

        // calculate physical address
        ret_addr = (page_table_entry & ~small_page_mask) | (virtual_addr & small_page_mask);
    } else {
        // 4 MB pages
        // top 10 bits are from PD, bottom 22 are from given addr
        ret_addr = (page_directory_entry & ~big_page_mask) | (virtual_addr & big_page_mask);
    }
    return ret_addr;
}

/*
 * alloc_new_process
 * DESCRIPTION: allocates a new page and sets up a PD
 * INPUTS: None
 * SIDE EFFECTS: Finds an open PD and sets it up
 * RETURN VALUE: the PID or -1 if no open PDs
 */
int alloc_new_process(){
    int open_process, i;
    int* cur_pd;
    int mask = WRITE_ENABLE;
    // find an open PD to use
    for (open_process = 0; open_process < MAX_PROCESSES; open_process++){
        if (process_in_use[open_process] == 0){
            break;
        }
    }
    // check if there isn't any open processes
    if(open_process == MAX_PROCESSES){
        return -1;
    }
    // mark process as in use
    process_in_use[open_process] = 1;
    cur_pd = (int*)process_pds[open_process];
    // the page directory is set up as specified in Appendix C
    // Copy kernel page into current directory
    for(i = 0; i < TABLE_SIZE; i++){
      // disable write for user space
      cur_pd[i] = page_directory[i] & (~mask);
    }
    // 128 MB location program, set up big page (kernal page + PID)
    cur_pd[USER_ADDR] = (((KERNAL_PAGE_ADDR_END)+open_process) << PD_ADDR_OFFSET) | BIG_PAGE | USER_SPACE | WRITE_ENABLE | PRESENT;
    return open_process;
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
  * DESCRIPTION: Switches PD to a given pid (7 is kernal PD)
  * CALL BEFORE CONTEXT DEALLOC
  * INPUTS:
  * int pid: the PID of the process you want to point to
  * SIDE EFFECTS: Changes value in cr3
  * RETURN VALUE: -1: process not occuring   0: on success
  */
 int context_switch_paging(int pid){
     if(pid == 6){
         asm volatile("movl %0, %%cr3":: "r"(page_directory));
         return 0;
     }
     if(process_in_use[pid] == 0){
         return -1;
     }
     asm volatile("movl %0, %%cr3":: "r"((int*)process_pds[pid]));
     return 0;
 }
