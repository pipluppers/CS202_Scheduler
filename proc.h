//	Lab 1
//	Give the ticket the number of tickets and the owner of the ticket
struct ticket {
	int owner;
	struct proc *p;		// points at the owner of the ticket
	//struct ticket(int pid): owner(pid);	// simply assign the owner to the owner's pid
};

// Per-CPU state
struct cpu {
  uchar apicid;                // Local APIC ID
  struct context *scheduler;   // swtch() here to enter scheduler
  struct taskstate ts;         // Used by x86 to find stack for interrupt
  struct segdesc gdt[NSEGS];   // x86 global descriptor table
  volatile uint started;       // Has the CPU started?
  int ncli;                    // Depth of pushcli nesting.
  int intena;                  // Were interrupts enabled before pushcli?
  struct proc *proc;           // The process running on this cpu or null

	struct context *lottery_scheduler;
	struct context *stride_scheduler;

	//	Lab 1
	//int sysNumTickets;
	//struct ticket *sysTickets;		// List of all the tickets in the system

};




extern struct cpu cpus[NCPU];
extern int ncpu;

//PAGEBREAK: 17
// Saved registers for kernel context switches.
// Don't need to save all the segment registers (%cs, etc),
// because they are constant across kernel contexts.
// Don't need to save %eax, %ecx, %edx, because the
// x86 convention is that the caller has saved them.
// Contexts are stored at the bottom of the stack they
// describe; the stack pointer is the address of the context.
// The layout of the context matches the layout of the stack in swtch.S
// at the "Switch stacks" comment. Switch doesn't save eip explicitly,
// but it is on the stack and allocproc() manipulates it.
struct context {
  uint edi;
  uint esi;
  uint ebx;
  uint ebp;
  uint eip;
};

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };




// Per-process state
struct proc {
 	uint sz;                     	// Size of process memory (bytes)
	pde_t* pgdir;                	// Page table
  	char *kstack;                	// Bottom of kernel stack for this process
  	enum procstate state;        	// Process state
  	int pid;                     	// Process ID
  	struct proc *parent;         	// Parent process
  	struct trapframe *tf;        	// Trap frame for current syscall
  	struct context *context;     	// swtch() here to run process
  	void *chan;                  	// If non-zero, sleeping on chan
  	int killed;                  	// If non-zero, have been killed
  	struct file *ofile[NOFILE];  	// Open files
  	struct inode *cwd;           	// Current directory
  	char name[16];               	// Process name (debugging)
	
	int numSysCalls;		// Keeps track of the number of system calls performed by this process
	int numMemPg;			// Keeps track of the number of memory pages the current process is using

	int tickets;			// Keeps track of the number of tickets the process has
	int original_stride;
	int stride;
	int numRan;

	// Lab 2
	
	//	Keeps track of the total number of threads that share the addr space as this one
	//	Should be initialized to 1
	//	Every one should be increased by 1 with each call to clone...HMMMM?????
	struct proc * child;	// List of children
	int numFriends;
};

// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap
