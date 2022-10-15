// spinlock.c - Spin lock functions

#include "spinlock.h"

// aquire the given spin lock, ensure it is atomic
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

// release the given spin lock, 1: locked 0: unlocked
void spin_unlock(spinlock_t* lock){
    asm volatile ("                   \n\
            movl $0, %0               \n\
            "
            :
            : "a"(lock)
            : "memory"
    );
}

// check if the given spin lock is locked
int spin_is_locked(spinlock_t* lock){
    return lock->lock;
}

// try to aquire spin lock once
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

// aquire the lock and clear interupt flags
void spin_lock_irq(spinlock_t* lock){
    cli();
    spin_lock(lock);
}

// release the lock and set IF = 1
void spin_unlock_irq(spinlock_t* lock){
    spin_unlock(lock);
    sti();
}

// aquire the lock and put IF = 0 and save it in flags
void spin_lock_irqsave(spinlock_t* lock, unsigned long flags){
    cli_and_save(flags);
    spin_lock(lock);
}

// release the lock and set flags to flags
void spin_unlock_irqrestore(spinlock_t* lock, unsigned long flags){
    spin_lock(lock);
    restore_flags(flags);
}
