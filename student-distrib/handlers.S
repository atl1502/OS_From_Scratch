
# Assembly linkage for the interupts

.data

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
pushl %EAX
pushl %EBP
pushl %EDI
pushl %ESI
pushl %EDX
pushl %ECX
pushl %EBX

call do_IRQ

jmp ret_from_intr

# end of interupt
ret_from_intr:
# context switching would happen here
popl %EBX
popl %ECX
popl %EDX
popl %ESI
popl %EDI
popl %EBP
popl %EAX
# unsure josh help
addl $4, %ESP
IRET
