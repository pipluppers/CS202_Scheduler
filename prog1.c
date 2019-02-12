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

int main(int argc, char *argv[]) {
	//set_tickets(30);

	int i,k;
	const int loop = 200;
	for (i = 0; i < loop; ++i)
		for (k = 0; k < loop; ++k);
	asm("nop");

	
	exit();
}
