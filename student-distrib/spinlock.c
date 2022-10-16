// spinlock.c - Spin lock functions

#include "spinlock.h"

/* 
 * spin_lock
 * DESCRIPTION: aquire the given spin lock, ensure it is atomic
 * INPUTS: lock
 * SIDE EFFECTS: lock becomes locked
 * RETURN VALUE: none
 */
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

/* 
 * spin_unlock
 * DESCRIPTION: release the given spin lock, 1: locked 0: unlocked
 * INPUTS: lock
 * SIDE EFFECTS: lock becomes unlocked
 * RETURN VALUE: never returns
 */
void spin_unlock(spinlock_t* lock){
    asm volatile ("                   \n\
            movl $0, %0               \n\
            "
            :
            : "a"(lock)
            : "memory"
    );
}

/* 
 * spin_is_locked
 * DESCRIPTION: check if the given spin lock is locked
 * INPUTS: lock
 * SIDE EFFECTS: none
 * RETURN VALUE: 1 if locked, 0 if unlocked
 */
int spin_is_locked(spinlock_t* lock){
    return lock->lock;
}

/* 
 * spin_trylock
 * DESCRIPTION: try to aquire spin lock once
 * INPUTS: lock
 * SIDE EFFECTS:
 * RETURN VALUE: 1 if locked, 0 if unlocked
 */
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

/* 
 * spin_lock_irq
 * DESCRIPTION: aquire the given spin lock, ensure it is atomic
 * INPUTS: lock
 * SIDE EFFECTS: lock is locked, interrupt flags are cleared via CLI
 * RETURN VALUE: none
 */
void spin_lock_irq(spinlock_t* lock){
    cli();
    spin_lock(lock);
}

/* 
 * spin_unlock_irq
 * DESCRIPTION: release the given spin lock, 1: locked 0: unlocked
 * INPUTS: lock
 * SIDE EFFECTS: interrupt flags set to 1 via STI
 * RETURN VALUE: none
 */
void spin_unlock_irq(spinlock_t* lock){
    spin_unlock(lock);
    sti();
}

/* 
 * spin_lock_irqsave
 * DESCRIPTION: aquire the given spin lock, ensure it is atomic
 * INPUTS: lock, flags
 * SIDE EFFECTS: IF = 0 and save it in flags
 * RETURN VALUE: none
 */
void spin_lock_irqsave(spinlock_t* lock, unsigned long* flags){
    cli_and_save(*flags);
    spin_lock(lock);
}

/* 
 * spin_unlock_irqrestore
 * DESCRIPTION: release the given spin lock, 1: locked 0: unlocked
 * INPUTS: lock, flags
 * SIDE EFFECTS: set flags to flags
 * RETURN VALUE: none
 */
void spin_unlock_irqrestore(spinlock_t* lock, unsigned long* flags){
    spin_unlock(lock);
    restore_flags(*flags);
}
