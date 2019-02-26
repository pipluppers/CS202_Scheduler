# xv6 Scheduling

Thanks to Professor Nael Abu-Ghazaleh for overseeing this project!

xv6 is an operating system provided by MIT and is great for practicing implementations of important operating system components. The original ReadMe is listed below. In this specific project, three separate system calls were added.

## Info

Info is a system call that takes in one integer as input. This integer is allowed to only be three values (1, 2, or 3):

- If the input is 1, then the system call will print the number of processes in the system. 

- If the input is 2, then it will print the number of system calls that the current process has done so far. 

- If the input is 3, then it will print the number of memory pages that
the process is currently using.

## Lottery Scheduling

Lottery Scheduling is a type of scheduling algorithm that utilizes a ticket system to determine which process should run next. This type
of scheduling is probabilistically fair due to the fact that the more tickets a process has, the higher the chance that it will be
chosen to run. It also prevents starvation in that a process always has a non-zero chance of being run.

In this implementation, upon being allocated, a process is initialized with a set number of tickets (2 in case). Another system call 
known as set_tickets is also implemented in order to allow a process to have a customizable amount of tickets. When choosing the next 
process to run, the algorithm grabs a random ticket from the ticket pool (which contains all of the tickets of single process). 
Then it loops through the available processes in order to find the winning ticket holder and schedules that process
to run next.

## Stride Scheduling

Stride scheduling is another type of scheduling algorithm that is very similar to lottery scheduling. It also uses the ticket system, 
but is more deterministic instead of probabilistic. Based on the number of tickets that process currently has, the algorithm assigns it 
a stride value, which is inversely proportional to the number of tickets held. The process with the lowest stride will be chosen to run,
and after it has run, its stride will be increased according to its ticket number. This prevents starvation as well and allows progress.

# Original ReadMe ---------------------------

xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern x86-based multiprocessor using ANSI C.

ACKNOWLEDGMENTS

xv6 is inspired by John Lions's Commentary on UNIX 6th Edition (Peer
to Peer Communications; ISBN: 1-57398-013-7; 1st edition (June 14,
2000)). See also https://pdos.csail.mit.edu/6.828/, which
provides pointers to on-line resources for v6.

xv6 borrows code from the following sources:
    JOS (asm.h, elf.h, mmu.h, bootasm.S, ide.c, console.c, and others)
    Plan 9 (entryother.S, mp.h, mp.c, lapic.c)
    FreeBSD (ioapic.c)
    NetBSD (console.c)

The following people have made contributions: Russ Cox (context switching,
locking), Cliff Frey (MP), Xiao Yu (MP), Nickolai Zeldovich, and Austin
Clements.

We are also grateful for the bug reports and patches contributed by Silas
Boyd-Wickizer, Anton Burtsev, Cody Cutler, Mike CAT, Tej Chajed, eyalz800,
Nelson Elhage, Saar Ettinger, Alice Ferrazzi, Nathaniel Filardo, Peter
Froehlich, Yakir Goaron,Shivam Handa, Bryan Henry, Jim Huang, Alexander
Kapshuk, Anders Kaseorg, kehao95, Wolfgang Keller, Eddie Kohler, Austin
Liew, Imbar Marinescu, Yandong Mao, Matan Shabtay, Hitoshi Mitake, Carmi
Merimovich, Mark Morrissey, mtasm, Joel Nider, Greg Price, Ayan Shafqat,
Eldar Sehayek, Yongming Shen, Cam Tenny, tyfkda, Rafael Ubal, Warren
Toomey, Stephen Tu, Pablo Ventura, Xi Wang, Keiichi Watanabe, Nicolas
Wolovick, wxdao, Grant Wu, Jindong Zhang, Icenowy Zheng, and Zou Chang Wei.

The code in the files that constitute xv6 is
Copyright 2006-2018 Frans Kaashoek, Robert Morris, and Russ Cox.

ERROR REPORTS

Please send errors and suggestions to Frans Kaashoek and Robert Morris
(kaashoek,rtm@mit.edu). The main purpose of xv6 is as a teaching
operating system for MIT's 6.828, so we are more interested in
simplifications and clarifications than new features.

BUILDING AND RUNNING XV6

To build xv6 on an x86 ELF machine (like Linux or FreeBSD), run
"make". On non-x86 or non-ELF machines (like OS X, even on x86), you
will need to install a cross-compiler gcc suite capable of producing
x86 ELF binaries (see https://pdos.csail.mit.edu/6.828/).
Then run "make TOOLPREFIX=i386-jos-elf-". Now install the QEMU PC
simulator and run "make qemu".
