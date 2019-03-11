#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "thread.h"

void lock_init(struct lock_t *lock) {
	lock->locked = 0;		// Not holding the lock
}

void lock_acquire(struct lock_t *lock) {

	pushcli();
	if (*lock == 1) panic("acquire");	

}

void lock_release(struct lock_t *lock) {

}
