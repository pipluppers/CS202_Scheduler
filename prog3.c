#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
	set_tickets(10);
	int i,k;
	const int loop = 43000;

	for (i = 0; i < loop; i++)
		for (k = 0; k < loop; k++);
	asm("nop");

	printf(1, "Number of tickets for prog3: %d\n", print_tickets());
	exit();
	return 0;
}
