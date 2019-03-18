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
	// xchg provides atomic lock acquiring
	while (xchg(&lock->locked, 1) != 0);
}

void lock_release(struct lock_t *lock) {

	xchg(&lock->locked, 0);
}

//	Array Lock	-------------------------------------------------




//	Seq Lock	-------------------------------------------------
//	Writers do an atomic increment of counter after acquiring lock and before releasing lock
//	Readers don't acquire lock but read the counter
//		If the counter is an odd number, then a writer has a lock.
//		Therefore, need to check counter before and after reading shared data. If counter has changed,
//			then a writer changed the data just now.
//	Reader checks will happen in that actual function that called it (i.e. test program)


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



//	MCS Lock	---------------------------------------------------------

//	If null, lock is acquired
struct mcs_node* fetch_and_store(struct mcs_node *q, struct mcs_node *n) {
	struct mcs_node *q1 = q;
	// TODO

	return q1;	
}

//	TODO Wtf is fetch and store
void mcs_lock_lock(struct mcs_lock *lock, struct mcs_node *n) {
	n->next = 0;
	
	struct mcs_node *prev = fetch_and_store(lock->queue, n);
	if (prev == 0) return;
	else {
		n->has_lock = 1;
		prev->next = n;
		while (n->has_lock == 1);
	}
}

//	If the first parameter has the same value as the second parameter, then change it to the third parameter
uint compare_and_swap(struct mcs_node *q, struct mcs_node *old_node, struct mcs_node *new_node) {
	if (q->has_lock != old_node->has_lock || q->next != old_node->next) {
		return 0;
	}
	*q = *new_node;
	return 1;
}

void mcs_lock_unlock(struct mcs_lock *lock, struct mcs_node *n) {
	
	if (n->next == 0) {
		if (compare_and_swap(lock, n, 0) == 1) return;
	}
	else {
		while(n->next == 0);
	}

	n->next->has_lock == 0;
}



//	Thread_create 	----------------------------------------------------------

int thread_create(void *(*start_routine)(void *), void *arg) {

	int tid;	// thread ID
	void *stack;

	

	if ((tid = clone(stack, PGSIZE)) == 0) {
		return -1;
	}	
	start_routine(arg);


	return tid;
}
