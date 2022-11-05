.data

.globl halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

.text

halt:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx # Callee saved
xorl %eax, %eax # Open is syscall 0
movl 8(%ebp),%ebx # Get status from halt
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret

execute:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx
movl $0x01,%eax # Execute is syscall 1
movl 8(%ebp),%ebx # Get pointer to execute instruction
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret

read:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx
movl $0x02,%eax # Read is syscall 2
movl 8(%ebp),%ebx # Get nbytes
movl 12(%ebp),%ecx # Get pointer to to buffer
movl 16(%ebp),%edx # Get file descriptor
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret

write:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx
movl $0x03,%eax # Write is syscall 3
movl 8(%ebp),%ebx # Get nbytes
movl 12(%ebp),%ecx # Get pointer to to buffer
movl 16(%ebp),%edx # Get file descriptor
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret

open:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx
movl $0x04,%eax # Open is syscall 4
movl 8(%ebp),%ebx # Get pointer to filename
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret

close:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx # Save ebx 
movl $0x05,%eax # Close is syscall 5
movl 8(%ebp),%ebx # Get pointer to file descriptor index
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret


getargs:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx
movl $0x06,%eax # Getargs is syscall 6
movl 8(%ebp),%ebx # Get nbytes
movl 12(%ebp),%ecx # Get pointer to buffer
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret

vidmap:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx
movl $0x07,%eax # Vidmap is syscall 7
movl 8(%ebp),%ebx # Get double pointer vidmap
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret

set_handler:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx
movl $0x08,%eax # Sethandler is syscall 8
movl 8(%ebp),%ebx # Get signum
movl 12(%ebp),%ecx # Get handler address
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret

sigreturn:
pushl %ebp  # Setup function frame
movl %esp, %ebp
pushl %ebx
movl $0x09,%eax # Sigreturn is syscall 9
int $0x80 # do the system call
popl %ebx # restore EBX from stack
leave
ret
