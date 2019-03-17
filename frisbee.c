#include "param.h"
#include "types.h"
#include "stat.h"

//#include "defs.h"

#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"
//#include "thread.h"

void test_routine(void* arg) {
	printf(1,"I have no idea what this function (start_routine) is supposed to do");
	return;
}

int main() {

	printf(1,"Enter number of Players: ");
	// Get input from user numThreads
	int numThreads = 4;
	printf(1,"\nEnter number of Passes: ");
	// Get input from user numPasses
	int numPasses = 6;
	printf(1, "\n");

	int i = 0;
	int tid = -1;
	for (; i < numThreads; ++i) {
		// Undefined reference to thread_create
		// Not sure why yet
		/*
		tid = thread_create(test_routine, (void*)&numPasses );
		if (tid == -1) {
			printf(1,"An error occurred in creating the thread");
			return 1;
		}
		*/

		printf(1, "tid: %d\n",tid);
	}

	

	return 0;
}
