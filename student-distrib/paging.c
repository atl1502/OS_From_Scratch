#include "x86_desc.h"

void paging_init(){
    /*
     * Fill in PDE for 0-4MB Page table
     * For this paging Entry:
     * 11-9 Avail, 8 G, 7 PS, 6 '0', 5 Accessed, 4 Cache Disabled, 3 PWT, 2 U/S, 1 R/W, 0 P
     *     000      1     0     0          0              0          0      0       1    1
     * which is 0x103 for last 12 bits
     * First 24 bits is page table address
    */
    page_directory[0] = ((uint32_t) page_table) | 0x103;

    /*
     * Fill in PDE for 4-8MB Kernel Big Page
     * For this paging Entry:
     * 21-13 '0' 12 PAT 11-9 Avail, 8 G, 7 PS, 6 '0', 5 Accessed, 4 Cache Disabled, 3 PWT, 2 U/S, 1 R/W, 0 P
     * 000000000   0        000      1     1     0          0              0          0      0       1    1
     * Which is 0x183 for the last 12 bits
     * First 10 bits is page address, which is 1, since it is the second 4MB block (0 indexed)
    */
    page_directory[1] = (0x01 << 22) | 0x183;

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
