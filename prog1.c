#include "types.h"
#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

int main() {
	
	set_tickets(30);

	int i,k;
	const int loop = 200;

	printf(1,"Beginning for loop\n");
/*
	for (i = 0; i < loop; ++i)
		for (k = 0; k < loop; ++k);
	printf(1,"Done with for loop\n");
	//asm("nop");
	printf(1, "Done with asm nop\n");	
*/		
	exit();
	return 0;
}
