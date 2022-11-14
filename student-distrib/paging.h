#ifndef PAGING_H
#define PAGING_H

#define MAX_PROCESSES 6
#define PD_ADDR_OFFSET 22
#define PT_ADDR_OFFSET 12
#define VIRT_PT_OFFSET 10
#define BIG_PAGE_MASK 0x3FFFFF
#define SMALL_PAGE_MASK 0xFFF
#define TABLE_SIZE 1024
#define BIG_PAGE 0x80
#define WRITE_ENABLE 0x2
#define USER_SPACE 0x4
#define PRESENT 0x1
#define KERNAL_PAGE_ADDR 0x01
#define KERNAL_PAGE_ADDR_END 0x02
#define USER_ADDR 32
#define INVALID_ADDR 0xFFFFFFFF

// Initialize paging
void paging_init();

// Virtual to physical translation
uint32_t virtual_to_physical(uint32_t virtual_addr);

// map new virtual address to physical
int alloc_new_process();

// unmap virtual address from physical
int dealloc_process(int pid);

// switch between processes
int context_switch_paging(int pid);

// zeros out process_in_use[0] for base case
void zero_base(void);

#endif // PAGING_H
