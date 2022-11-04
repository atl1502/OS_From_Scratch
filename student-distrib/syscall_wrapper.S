open:
pushl %ebx # Callee saved register

movl $0x02,%eax
movl 8(%esp),%ebx # Get pointer to filename

int $0x80 # do the system call

cmpl $0xFFFFF001, %eax # -1 to -4095 are errors
jb done

done:
popl %ebx # restore EBX from stack
ret
