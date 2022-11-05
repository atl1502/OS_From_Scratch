# Assembly linkage for the interupts
.data

# Handler functions
.globl handler_keyboard, handler_rtc, handler_interrupt

# Syscall functions
.globl system_call

.globl sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn

#
.align 4
jump_table:
.long sys_halt, sys_execute, sys_read, sys_write, sys_open, sys_close, sys_getargs, sys_vidmap, sys_set_handler, sys_sigreturn

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
pushal
call do_IRQ

jmp ret_from_intr

# end of interupt
ret_from_intr:
popal

# context switching would happen here
# unsure josh help

addl $4, %ESP
iret

system_call:

cmpl $10, %eax
ja system_call_error

# set IF = 1
sti

# Pop callee saved registers
pushl %ebx
pushl %edi
pushl %esi

# Push all parameters no matter what, function will ignore parameters
pushl %edx
pushl %ecx
pushl %ebx

call *jump_table(,%eax,4)

# Pop parameters off of stack
addl $12, %esp

# Pop callee saved registers
popl %esi
popl %edi
popl %ebx

iret

system_call_error:
movl $-1, %eax
iret
