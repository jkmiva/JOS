# JOS
simple OS kernel
### lab2 Virtual Memory
* physical page allocator
* operations on 2-level page table
* kernel part of page directory, UPAGES, kernel stack, kernel base


### lab3 User Environment
In JOS, we use _environment_ denote the concept of _process_<br>
In JOS, individual environments do not have their own kernel stacks, there can be only _one_ JOS environment active in the kernel at a time, so JOS needs only a single kernel stack.<br>
* create user environment, load ELF binary and run it
* set idt with trap handler, the trap handler function take control after cs:eip get new value.
* set up page fault(right now just destroy the environment that cause the fault)
* set up syscall with four sys function
* set up breakpoint exception(triggered by panic() and run monitor() func)


### lab4 Preemptive Multitasking
* multiprocessor support
 + set pgdir for Memory-Mapped I/O
 + set up MP entry code, kernel stack and TSS for different CPUs
 + use a big kernel lock that only one environment can enter kernel mode
 + implement Round-Robin scheduling
