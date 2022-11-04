# Assembly linkage for the interupts and syscalls

.data

# Interupt functions
.globl handler_keyboard, handler_rtc, handler_interrupt

# System call functions
.globl system_call

# Allows for calling of syscalls in C
.globl sys_halt
.globl sys_execute
.globl sys_read
.globl sys_write
.globl sys_open
.globl sys_close
.globl sys_getarts
.globl sys_vidmap
.globl sys_set_handler
.globl sigreturn

.align 4
jump_table:
.long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sigreturn

.text

# keyboard interupt vector is called
handler_keyboard:
pushl $0xFFFFFFFE
jmp handler_interrupt

# rtc interupt vector is called
handler_rtc:
pushl $0xFFFFFFF7
jmp handler_interrupt

# common interupt (all interupts end up here)
handler_interrupt:
pusha
cld
call do_IRQ
jmp ret_from_intr

# end of interupt
ret_from_intr:
popa

# context switching and soft interrupts would happen here

# Pop negative IRQ #
addl $4, %esp
iret

# Common syscall handler

system_call:
pusha
cld

# EAX can call one of 10 syscalls
cmp %eax, 0xA
ja invalid_syscall

# Jump table of syscall functions
call *jump_table(,%eax,4)

popa
iret

invalid_syscall:
movl $-1, %eax
ret
