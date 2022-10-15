
.data

.globl handler_keyboard, handler_rtc, handler_interrupt

.text

handler_keyboard:
pushl $0xFFFFFFFE
jmp handler_interrupt

handler_rtc:
pushl $0xFFFFFFFD
jmp handler_interrupt

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

ret_from_intr:
