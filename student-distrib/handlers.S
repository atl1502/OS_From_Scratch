# Assembly linkage for the interupts and syscalls

.data

# Interupt functions
.globl handler_keyboard, handler_rtc, handler_interrupt

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
