#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
#include "spinlock.h"

int main() {
	set_tickets(30);

	int i,k;
	const int loop = 43000;

	for (i = 0; i < loop; ++i)
		for (k = 0; k < loop; ++k);
	asm("nop");

	
	printf(1, "Number of tickets for prog1: %d\n", print_tickets());
	exit();
	return 0;
}
