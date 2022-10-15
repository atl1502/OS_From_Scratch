/*
 *  Our implementation of spinlock
 *  Interface is identical to given one
 */
#include "lib.h"

#define SPIN_LOCK_UNLOCKED {0}

typedef struct {
	volatile unsigned int lock;
} spinlock_t;

// aquire the spin lock (hold if held)
void spin_lock(spinlock_t* lock);
// give up the spin lock, unlock the lock
void spin_unlock(spinlock_t* lock);
// return 1 if held 0 if not
int spin_is_locked(spinlock_t* lock);
// aquire the spin lock (one try)
int spin_trylock(spinlock_t* lock);
// aquire the spin lock, clear interrupts (hold if held)
void spin_lock_irq(spinlock_t* lock);
// give up the spin lock, unlock the lock, set interrupt flag to 1
void spin_unlock_irq(spinlock_t* lock);
// aquire the spin lock, clear interrupts and save flags (hold if held)
void spin_lock_irqsave(spinlock_t* lock, unsigned long flags);
// give up the spin lock, unlock the lock, set interrupt flag to flags
void spin_unlock_irqrestore(spinlock_t* lock, unsigned long flags);
