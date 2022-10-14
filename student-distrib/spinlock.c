// spinlock.c - Spin lock functions

#include "spinlock.h"

void spin_lock(spinlock_t* lock){
    asm volatile ("                   \n\
            loop:                     \n\
                movl $1, %%ecx        \n\
                xchgl %%ecx, (%0)     \n\
                cmpl $1, %%ecx        \n\
                je      loop          \n\
                "
            :
            : "a"(lock)
            : "memory"
    );
}

void spin_unlock(spinlock_t* lock){
    asm volatile ("                   \n\
            movl $0, %0               \n\
            "
            :
            : "a"(lock)
            : "memory"
    );
}

int spin_is_locked(spinlock_t* lock){
    return lock->lock;
}

int spin_trylock(spinlock_t* lock){
    asm volatile ("                   \n\
            movl $1, %%ecx            \n\
            xchgl %%ecx, (%0)         \n\
            cmpl $1, %%ecx            \n\
            "
            :
            : "a"(lock)
            : "memory"
    );
    return lock->lock;
}
void spin_lock_irq(spinlock_t* lock){
    cli();
    spin_lock(lock);
}

void spin_unlock_irq(spinlock_t* lock){
    spin_unlock(lock);
    sti();
}

void spin_lock_irqsave(spinlock_t* lock, unsigned long flags){
    cli_and_save(flags);
    spin_lock(lock);
}

void spin_unlock_irqrestore(spinlock_t* lock, unsigned long flags){
    spin_lock(lock);
    restore_flags(flags);
}
