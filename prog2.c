#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"



int main(int argc, char *argv[])
{
	set_tickets(20);

	int i,k;
	const int loop = 43000;

	for (i = 0; i < loop; i++)
		for (k = 0; k < loop; k++);
	asm("nop");
	
	printf(1, "Number of tickets for prog2: %d\n", print_tickets());

	printf(1, "Number of strides taken for prog2: %d\n", stride_runs());
	exit();
	return 0;
}
