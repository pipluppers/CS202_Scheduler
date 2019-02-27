#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

int numProcesses = 0;
int numThreads = 0;

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  	struct cpu *c;
  	struct proc *p;
  
	//	Disable interrupts
	pushcli();
  	
	c = mycpu();
  	p = c->proc;
  	
	//	Renable interrupts
	popcli();
  	
	return p;
}


/*
static struct proc*
allocthread(void)
{
	struct proc *p;
	char *sp;

	// cprintf("Calling allocthread in proc.c\n");
	
	acquire(&ptable.lock);

	//	Look for an unused process
	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
		if (p->state == UNUSED) goto found;
	}

	//	If no unused process, release lock and stop
	release(&ptable.lock);
	return 0;

	found:
		p->state = EMBRYO;
		struct proc *curproc = myproc();	// Used to get the parent address space
		p->pid = nextpid++;
		release(&ptable.lock);

		// Should not allocate new kernel stack
		// Use the same one as the parent
		p->kstack = curproc->kstack;
		sp = p->kstack + KSTACKSIZE;

		// Leave room for trap frame
		sp -= sizeof *p->tf;
		p->tf = (struct trapframe *) sp;

		// Set up new context to start executing at forkret
		// which returns to trapret
		sp -= 4;
		*(uint *)sp = (uint) trapret;

		sp -= sizeof *p->context;
		p->context = (struct context*) sp;
		memset(p->context,0,sizeof *p->context);
		p->context->eip = (uint) forkret;

		// Lab 1 Additions
		p->numSysCalls = 0;
		p->numMemPg = 1;
		++numProcesses;	// necessary? Is a thread a process?
		p->tickets = 2;
		p->original_stride = 10000/p->tickets;
		p->stride = p->original_stride;
		p->numRan = 0;

		return p;
}
*/		

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO
// and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

//	cprintf("Calling proc::allocproc\n");

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;

  release(&ptable.lock);



	// Allocate kernel stack.
	// kalloc defined in kalloc.c
	// 	It allocated one 4096-byte page of phys memory
	// 	and returns a pointer that the kernel can use
	// 	If it failed, return 0
  	if((p->kstack = kalloc()) == 0){
    		p->state = UNUSED;
	    	return 0;
  	}
	// KSTACKSIZE is the size of a per-process kernel stack (4096)
	// kstack is the bottom of the kernel stack for that process
  	sp = p->kstack + KSTACKSIZE;



  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;


	//	Lab 1
	//	Initializing numSysCalls to 0
	p->numSysCalls = 0;
	p->numMemPg = 1;
	++numProcesses;

	acquire(&ptable.lock);
	p->tickets = 2;
	p->original_stride = 10000/(p->tickets);
	p->stride = p->original_stride;
	p->numRan = 0;
	p->numThreads = 1;
	release(&ptable.lock);

//	cprintf("Leaving proc::allocproc()\n");
	
  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

//	cprintf("Calling proc::userinit\n");

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

//	cprintf("About to release the lock\n");
	release(&ptable.lock);
//	cprintf("Leaving userinit\n");
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();


//	cprintf("Calling proc::growproc\n");


  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);

//	Take size and divide by PGSIZE

	++curproc->numMemPg;

  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

//	cprintf("Calling proc::fork\n");



//	Allocation of address space is somewhere here???
//	------------------------------------------------------

  	// Allocate process.
  	if((np = allocproc()) == 0){
    		return -1;
  	}

  	// Copy process state from proc.
  	// Create a copy of the parent's page table for the child
  	if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    		kfree(np->kstack);
    		np->kstack = 0;
    		np->state = UNUSED;
    		return -1;
  	}	

//	-----------------------------------------------------



	// Copy the parent's size of process memory
	//	and trapframe for current syscall
  	np->sz = curproc->sz;
  	np->parent = curproc;
  	*np->tf = *curproc->tf;

  	// Clear %eax so that fork returns 0 in the child.
	//	%eax is a general purpose register of type unsigned int
	np->tf->eax = 0;

	// NOFILE is 16
	// If the file is open, filedup increments the reference count of that file
	// 	and returns that same file
	// 	i.e. let the system know that another process is using said file
  	for(i = 0; i < NOFILE; i++) {
    		if(curproc->ofile[i]) {
      			np->ofile[i] = filedup(curproc->ofile[i]);
		}
	}
  	// Increments the reference counter of the current working directory
	np->cwd = idup(curproc->cwd);

	// Just like strncpy but guaranteed to null-terminate
  	safestrcpy(np->name, curproc->name, sizeof(curproc->name));

	// Technically don't need this line. Could just return np->pid
  	pid = np->pid;

	// Make the child process runnable
  	acquire(&ptable.lock);
  	np->state = RUNNABLE;
  	release(&ptable.lock);

  return pid;
}


//int clone(int size, void *stack) {
//	kstack is a char*. Maybe make stack a char pointer?????
//
//
//	Use the parent's address space
//	Don't duplicate the file descriptors of the parents. Should use the same file descriptor
//	Run on the child user's stack, NOT the parent's stack
//
//	Return the pid of the child to the parent (or 0 to a newly created child thread)
int clone(void *stack, int size) {
	int i, pid;
	struct proc *np;			// New process or thread in this case
	struct proc *curproc = myproc();	// Current process


//	--------------------------------------------------------

	// Don't create a new process??
	// Allocate thread
	//if ((np = allocthread()) == 0) return -1;	
	if ((np = allocproc()) == 0) return -1;

	np->pgdir = curproc->pgdir;	// Set child's page table to parent's page table????


//	-------------------------------------------------------


	np->sz = curproc->sz;
	np->parent = curproc;
	*np->tf = *curproc->tf;
	
	// Clear %eax so that clone returns 0 in the child
	np->tf->eax = 0;



	// Ensure that the file descriptor is not a duplicate but is using the same one
	// TODO Wat the fack
	for (i = 0; i < NOFILE; ++i) {
		if (curproc->ofile[i])
			np->ofile[i] = filedup(curproc->ofile[i]);
	}
	np->cwd = idup(curproc->cwd);


	// np->name = curproc->name
	safestrcpy(np->name, curproc->name, sizeof(curproc->name));
	
	pid = np->pid;
	
	acquire(&ptable.lock);
	np->state = RUNNABLE;
	release(&ptable.lock);

	return pid;
}


// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;


//	cprintf("Calling proc::exit\n");


  if(curproc == initproc)
    panic("init exiting");


	//	Lab 2
	//	Only close all file descriptors when the last thread exits

	// New part -------------------------------------

	--curproc->parent->numThreads;
	if (curproc->parent->numThreads > 0) {
		return;
	}

	// ----------------------------------------------

  	// Close all open files.
  	for(fd = 0; fd < NOFILE; fd++){
    		if(curproc->ofile[fd]){
      			fileclose(curproc->ofile[fd]);
      			curproc->ofile[fd] = 0;
    		}
  	}




  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  

//	cprintf("Calling proc::wait\n");


  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }
    // No point waiting if we don't have any children.

    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;


//	cprintf("Calling proc::scheduler\n");
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
}


#define RAND_MAX_32 ((1U << 31) - 1)
int rseed = 0;
int rand() {
	return (rseed = (rseed * 214013 + 2531011) & RAND_MAX_32) >> 16; 
}




//	Lottery Scheduler
//	Implement only the basic operation (no ticket transfers, compensation tickets, etc.)
//	Freedom to decide how many tickets to assign to each process
//	Can use a system call to assign tickets to processors
void
lottery_scheduler(void)
{
	struct proc *p;
	struct cpu *c = mycpu();
	c->proc = 0;
	int chosenTicket;

//	cprintf("Calling proc::lottery_scheduler\n");

	int tot_tickets = 0;
	int counter = 0;

	for(;;) {
		//	Enables interrupts on this processor
		sti();
		
		//	Get the lock
		acquire(&ptable.lock);
		
		//	Find the total number of tickets in the system
		tot_tickets = 0;
		for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
			if (p->state != RUNNABLE) {
				continue;
			}
			tot_tickets += p->tickets;
		}

		//	Grab a random ticket from the ticket list		
		chosenTicket = rand() % tot_tickets;

		counter = 0;
		for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
			if (p->state != RUNNABLE) continue;
			++counter;
			if (counter != chosenTicket) continue;
			
			//	Schedule this process
			c->proc = p;
			switchuvm(p);
			p->state = RUNNING;
			swtch(&(c->lottery_scheduler),p->context);		
	
			switchkvm();
			c->proc = 0;
			break;
		}
		release(&ptable.lock);
	}
	cprintf("This should never print. In proc::lottery_schduler\n");
}



//	Stride Scheduler
void 
stride_scheduler(void)
{
	struct proc *p;
	struct cpu *c = mycpu();
	c->proc = 0;
	float min_stride;
	struct proc *min_proc;

	cprintf("Calling proc::stride_scheduler\n");

	//	Run 5ever
	for (;;) {

		// Enables interrupts on this processor
		sti();

		// Get the lock
		acquire(&ptable.lock);

		min_stride = 1000000000;
		for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
			if (p->state != RUNNABLE) continue;

			if (p->stride < min_stride) {
			
				min_stride = p->stride;

				min_proc = p;
			}
		}
		min_proc->stride += min_proc->original_stride;	
		++min_proc->numRan;	
		//cprintf("Process %d's stride: %d\n", min_proc->pid, min_proc->stride);
		c->proc = min_proc;
		switchuvm(min_proc);
		min_proc->state = RUNNING;
		
		swtch(&(c->stride_scheduler), min_proc->context);
		switchkvm();

		//	Process is done running now
		//	It should have changed its p->state before coming back
		c->proc = 0;
		release(&ptable.lock);
	}
	cprintf("This should never print. In proc::stride_scheduler\n");
}





// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{


//	cprintf("Calling sched() in proc.c\n");


  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  


	//	Lab 1
	//	Calls the scheduler
	//	Also change main.c
	//	Will change this to the lottery and stride schedulers later
	swtch(&p->context, mycpu()->scheduler);
//	swtch(&p->context, mycpu()->lottery_scheduler); 
//	swtch(&p->context, mycpu()->stride_scheduler);


	mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{


//	cprintf("Calling proc::yield\n");


  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???  hehe what";	// The question marks were there already, I promise ...
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}





//	For use in lottery and stride schedulers
//	Initializes the number of tickets to for a process
void
set_tickets(int numTickets)
{
	struct proc *curproc = myproc();

	acquire(&ptable.lock);

	curproc->tickets = numTickets;

	release(&ptable.lock);

}


// Take in one integer parameter with value 1, 2, or 3
// 1: Return count of the processes in system
// 2: Return count of the total number of system calls that a process has done so far
// 3: Return number of memory pages the current process is using
int
info(int param)
{
//	cprintf("------------\nCalling the info function\n");
	
	struct proc *curproc = myproc();

	if (param == 1) {
		return numProcesses;
	}
	else if (param == 2)
		return curproc->numSysCalls;
	else if (param == 3)
		return curproc->numMemPg;
	else {
		exit();
	}

	//cprintf("End of the info function\n--------------\n");

	return param;

}



int
print_tickets() {

	acquire(&ptable.lock);
	int t = 0;
	struct proc *p;
	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
		t += p->tickets;
	}
	release(&ptable.lock);
	return t;
}



int
stride_runs()
{

	struct proc *p = myproc();
	return p->numRan;
}


