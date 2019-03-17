#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "thread.h"

//	Spin Lock	--------------------------------------------------

void lock_init(struct lock_t *lock) {
	lock->locked = 0;		// Not holding the lock
}

void lock_acquire(struct lock_t *lock) {

	// Disable interrupts to avoid deadlock
	// Might have to define pushcli first
	pushcli();

	//Not considering CPU. This might be unnecessary
	//if (*lock == 1) panic("acquire");	
	
	// xchg provides atomic lock acquiring
	//
	while (xchg(&lock->locked, 1) != 0);

}

void lock_release(struct lock_t *lock) {

	// Want to set lock->locked to 0 atomically
	xchg(&lock->locked, 0);

	// Might have to define this function
	popcli();
}

//	Array Lock	-------------------------------------------------




//	Seq Lock	-------------------------------------------------
//	Writers do an atomic increment of counter after acquiring lock and before releasing lock
//	Readers don't acquire lock but read the counter
//		If the counter is an odd number, then a writer has a lock.
//		Therefore, need to check counter before and after reading shared data. If counter has changed,
//			then a writer changed the data just now.

void seq_lock_init(struct seq_lock_t *lock) {
	lock->locked = 0;	// might be unnecessary
	lock->seq_num = 0;	// initialize counter to 0
}

//	Writers do an atomic increment of a counter
void seq_lock_acquire(struct seq_lock_t *lock) {
	// idk if this will work
	//while(xchg(&lock->seq_num, ++lock->seq_num) != &lock->seq_num);

	// Is this atomic too??
	while(xchg(&lock->locked, 1) != 0);
	++lock->seq_num;
}

//	Writers do an atomic increment of a counter
void seq_lock_release(struct seq_lock_t *lock) {
	//while(xchg(&lock->seq_num, ++lock->seq_num) != &lock->seq_num);

	++lock->seq_num;	
	while(xchg(&lock->locked,1)!=0);
}

//	Readers use to read the counter/sequence number
uint seq_lock_read(struct seq_lock_t *lock) {
	return lock->seq_num;
}





//	Thead_create 	----------------------------------------------------------

int thread_create(void *(*start_routine)(void *), void *arg) {

	int tid;	// thread ID
	void *stack;

	

	if ((tid = clone(stack, PGSIZE)) == 0) {
		return -1;
	}	
	start_routine(arg);


	return tid;
}
