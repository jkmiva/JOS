# JOS
simple OS kernel
### lab2 virtual memory
* physical page allocator
* operations on 2-level page table
* kernel part of page directory, UPAGES, kernel stack, kernel base


### lab3 user environment
In JOS, we use _environment_ denote the concept of _process_<br>
In JOS, individual environments do not have their own kernel stacks, there can be only _one_ JOS environment active in the kernel at a time, so JOS needs only a single kernel stack.<br>
* create user environment, load ELF binary and run it
