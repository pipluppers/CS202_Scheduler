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
	//printf(1, "Starting usertests1\n");
	
	//	This is doing weird things right now and it's not because of the first parameter of printf
	//	Trap 14 Page Fault

	int x = info(2);
	printf(1, "Running processes in  the system = %d\n", info(1));
	printf(1, "Number of System Calls for this process = %d\n", info(2));
	printf(1, "Number of Memory Tables for the process = %d\n", info(3));
	

	//printf(1,"Exiting\n");
	exit();
	return 0;
}
