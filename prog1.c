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
//#include <time.h>
#include "date.h"
//#include "defs.h"

int main() {
	set_tickets(30);

	//int t0 = time(NULL);
	//int t1 = 0;

	struct rtcdate *r;
/*	if (date(&r)) {
		printf(2, "date failed\n");
		exit();
	}
*/
//	cmostime(r);


	int i,k;
	const int loop = 43000;

	for (i = 0; i < loop; ++i)
		for (k = 0; k < loop; ++k);
	asm("nop");

	//t1 = time(NULL);
	//printf(1, "Runtime of prog1: %d\n", t1-t0);	

	printf(1, "Number of tickets for prog1: %d\n", print_tickets());
	exit();
	return 0;
}
