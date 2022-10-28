#ifndef PAGING_H
#define PAGING_H

#define MAX_PROCESSES 6
#define ADDR_OFFSET 22
#define TABLE_SIZE 1024
#define GLOBAL 0x100
#define PS 0x80
#define RW 0x2
#define P 0x1
#define KERNAL_PAGE_ADDR 0x01
#define USER_ADDR 32

// Initialize paging
void paging_init();

// map new virtual address to physical
int alloc_new_process();

// unmap virtual address from physical
int dealloc_process(int pid);

// switch between processes
int context_switch_paging(int pid);

#endif // PAGING_H
