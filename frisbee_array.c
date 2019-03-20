
//#include "param.h"
//#include "types.h"
//#include "stat.h"

//#include "defs.h"

//#include "user.h"
//#include "fs.h"
//#include "fcntl.h"
#include "syscall.h"
//#include "traps.h"
//#include "memlayout.h"
#include "thread.c"

//	Only include thread.c and all the redefinitions go away


void test_routine(void* arg) {
	printf(1,"I have no idea what this function (start_routine) is supposed to do");
	return;
}

int main(int argc, char *argv[]) {

	if (argc != 3) {
		printf(1, "Need 3 arguments: executable, numPlayers, numPasses\n");
		exit();
	}
	int numThreads = atoi(argv[1]);
	int numPasses = atoi(argv[2]);
	printf(1, "numPlayers = %d, numPasses = %d\n",numThreads,numPasses);

	int i = 0;
	int tid = -1;
	for (; i < numThreads; ++i) {
		tid = thread_create(test_routine, (void*)&numPasses );
		/*if (tid == -1) {
			printf(1,"An error occurred in creating the thread");
			return 1;
		}
		*/
		printf(1, "tid: %d\n",tid);
	}
	
	for (i = 0; i < numPasses; ++i) {
		printf(1, "Pass number no: %d, Thread %d is passing the token to thread %d\n", i+1, tid, tid);
	}


	printf(1,"End of frisbee_spin\n");
	exit();
	return 0;
}
