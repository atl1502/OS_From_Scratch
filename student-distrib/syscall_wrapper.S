.data

.globl halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

.text

halt:
pushl %ebx
xorl %eax, %eax # Open is syscall 0
movl 8(%esp),%ebx # Get status from halt
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret

execute:
pushl %ebx
movl $0x01,%eax # Execute is syscall 1
movl 8(%esp),%ebx # Get pointer to execute instruction
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret

read:
pushl %ebx
movl $0x02,%eax # Read is syscall 2
movl 8(%esp),%ebx # Get nbytes
movl 12(%esp),%ecx # Get pointer to to buffer
movl 16(%esp),%edx # Get file descriptor
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret

write:
pushl %ebx
movl $0x03,%eax # Write is syscall 3
movl 8(%esp),%ebx # Get nbytes
movl 12(%esp),%ecx # Get pointer to to buffer
movl 16(%esp),%edx # Get file descriptor
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret

open:
pushl %ebx
movl $0x04,%eax # Open is syscall 4
movl 8(%esp),%ebx # Get pointer to filename
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret

close:
pushl %ebx # Save ebx 
movl $0x05,%eax # Close is syscall 5
movl 8(%esp),%ebx # Get pointer to file descriptor index
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret


getargs:
pushl %ebx
movl $0x06,%eax # Getargs is syscall 6
movl 8(%esp),%ebx # Get nbytes
movl 12(%esp),%ecx # Get pointer to buffer
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret

vidmap:
pushl %ebx
movl $0x07,%eax # Vidmap is syscall 7
movl 8(%esp),%ebx # Get double pointer vidmap
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret

set_handler:
pushl %ebx
movl $0x08,%eax # Sethandler is syscall 8
movl 8(%esp),%ebx # Get signum
movl 12(%esp),%ecx # Get handler address
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret

sigreturn:
pushl %ebx
movl $0x09,%eax # Sigreturn is syscall 9
int $0x80 # do the system call
popl %ebx # restore EBX from stack
ret
