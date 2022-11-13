# x86_desc.S - Set up x86 segment descriptors, descriptor tables
# vim:ts=4 noexpandtab

#define ASM     1
#include "x86_desc.h"

.text

.globl ldt_size, tss_size, pid
.globl gdt_desc, ldt_desc, tss_desc
.globl tss, tss_desc_ptr, ldt, ldt_desc_ptr
.globl gdt_ptr
.globl idt_desc_ptr, idt
.globl page_directory,pd_p0,pd_p1,pd_p2,pd_p3,pd_p4,pd_p5
.globl page_table

.align 4

pid:
    .long 0
tss_size:
    .long tss_bottom - tss - 1

ldt_size:
    .long ldt_bottom - ldt - 1

    .word 0 # Padding
ldt_desc:
    .word KERNEL_LDT
    .long ldt

    .align 4
tss:
_tss:
    .rept 104
    .byte 0
    .endr
tss_bottom:

.align 4
    .word 0 # Padding
gdt_desc:
    .word gdt_bottom - gdt - 1
    .long gdt
    .align  16
gdt:
_gdt:

    # First GDT entry cannot be used
    .quad 0

    # NULL entry
    .quad 0

    # Segmentation will not be used
    # CS and DS both are 0-4GB r/w segments
    #
    # The layout is (from Intel IA-32 reference manual):
    #  31        24 23  22  21  20  19   16 15  14 13 12  11   8 7          0
    # |----------------------------------------------------------------------|
    # |            |   | D |   | A |  Seg  |   |  D  |   |      |            |
    # | Base 31:24 | G | / | 0 | V | Limit | P |  P  | S | Type | Base 23:16 |
    # |            |   | B |   | L | 19:16 |   |  L  |   |      |            |
    # |----------------------------------------------------------------------|
    #
    # |----------------------------------------------------------------------|
    # |                                    |                                 |
    # | Base 15:0                          | Segment Limit 15:0              |
    # |                                    |                                 |
    # |----------------------------------------------------------------------|

gdt_ptr:
    # Set up an entry for kernel CS
    .quad 0x00CF9A000000FFFF

    # Set up an entry for kernel DS
    .quad 0x00CF92000000FFFF

    # Set up an entry for user CS
    .quad 0x00CFFA000000FFFF

    # Set up an entry for user DS
    .quad 0x00CFF2000000FFFF

    # Set up an entry for TSS
tss_desc_ptr:
    .quad 0

    # Set up one LDT
ldt_desc_ptr:
    .quad 0

gdt_bottom:

ldt:
    .rept 4
    .quad 0
    .endr
ldt_bottom:

.align 4
    .word 0 # Padding
idt_desc_ptr:
    .word idt_bottom - idt - 1
    .long idt


    .align  16
idt:
_idt:
    .rept NUM_VEC
    .quad 0
    .endr

idt_bottom:

.align 4096

page_directory:
_page_directory:
    .rept 1024
    .long 0
	.endr
page_directory_bottom:

.align 4096

pd_p0:
_pd_p0:
    .rept 1024
    .long 0
	.endr
pd_bottom_p0:

.align 4096

pd_p1:
_pd_p1:
    .rept 1024
    .long 0
	.endr
pd_bottom_p1:

.align 4096

pd_p2:
_pd_p2:
    .rept 1024
    .long 0
	.endr
pd_bottom_p2:

.align 4096

pd_p3:
_pd_p3:
    .rept 1024
    .long 0
	.endr
pd_bottom_p3:

.align 4096

pd_p4:
_pd_p4:
    .rept 1024
    .long 0
	.endr
pd_bottom_p4:

.align 4096

pd_p5:
_pd_p5:
    .rept 1024
    .long 0
	.endr
pd_bottom_p5:

.align 4096

page_table:
_page_table:
    .rept 1024
    .long 0
	.endr
pd_bottom:

page_table_vid:
_page_table_vid:
    .rept 1024
    .long 0
	.endr
pd_bottom_vid:
