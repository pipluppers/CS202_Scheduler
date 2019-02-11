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
	printf(1, "Starting usertests1\n");
	
	//	This is doing weird things right now and it's not because of the first parameter of printf
	//	Trap 14 Page Fault
	// info(2);
	int x = info(1);
	printf(1, "x = %d\n", x);
	

	printf(1,"Exiting\n");
	exit();
	return 0;
}
