#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

extern int numSysCalls;

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}



//	New stuff for CS 202
//	info system call takes in 1, 2, or 3
//		1. return count of the processes in the system
//		2. return count of the total number of system calls that a process has done so far
//		3. return the number of memory pages the current process is using
int
sys_info(void)
{
//	cprintf("Calling sys_info function in sysproc.c\n");

	int pid;
	
	//	Call argint to grab the argument value in the info call
	if (argint(0, &pid) < 0)
		return -1;
	//cprintf("Value of the parameter: %d\n", pid);
	return info(pid);
}


int
sys_set_tickets(void)
{
//	cprintf("Calling sys_set_tickets function in sysproc.c\n");

	int pid;
	
	if (argint(0, &pid) < 0)
		return -1;
	set_tickets(pid);
	return 0;
}

int
sys_print_tickets(void)
{

//	cprintf("Calling sys_print_tickets in sysproc.c\n");
	
	return print_tickets();
}


int 
sys_stride_runs(void)
{
	return stride_runs();
}

int
sys_clone(void)
{

	int pid;
	if (argint(0, &pid) < 0)
		return -1;

	// huh
	return clone(pid);


}


