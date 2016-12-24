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
 + add system calls for environment creation
* Copy-on-Write Fork
   + on _fork()_ the kernel only copy the address space __mappings__ not its contents and mark writable and COW pages as COW pages.
* Clock interrupt and IPC
 + implement clock interrupts. In JOS, external device interrupts are always _disabled_ when in the kernel and enabled when in user space. controlled by FL_IF flag bit of %eflags register.
 + implement Inter-Process communication
   + two sys function sys_ipc_recv and sys_ipc_try_send. To receive a msg, an process calls sys_ipc_recv, the current process will be de-scheduled and doesn't run again until a msg has been received. Now any other process can send it a msg by calling sys_ipc_try_send. a 32-bit int and an optional page mapping can be transferred between two processes.


### lab5 File system, Spawn and Shell
* File system
Most UNIX file systems divide disk space into _inode regions_ and _data regions_. In JOS, we don't have the concept of _inode_ which means we also won't support hard links, symbolic links. JOS will simply store all of a file's meta-data within the (one and only) directory entry describing that file.
 + enable disk access for fs env
 + set up a 3GB block cache for fs env in a form of _demand paging_.
 + add block operations
 + link block number with virtual memory address
 + implement file system interface
 ```
	  Regular env           FS env
       +---------------+   +---------------+
       |      read     |   |   file_read   |
       |   (lib/fd.c)  |   |   (fs/fs.c)   |
    ...|.......|.......|...|.......^.......|...............
       |       v       |   |       |       | RPC mechanism
       |  devfile_read |   |  serve_read   |
       |  (lib/file.c) |   |  (fs/serv.c)  |
       |       |       |   |       ^       |
       |       v       |   |       |       |
       |     fsipc     |   |     serve     |
       |  (lib/file.c) |   |  (fs/serv.c)  |
       |       |       |   |       ^       |
       |       v       |   |       |       |
       |   ipc_send    |   |   ipc_recv    |
       |       |       |   |       ^       |
       +-------|-------+   +-------|-------+
	       |                   |
	       +-------------------+

```
 + implement spawn processes and sharing ability across fork and spawn
 + implement shell redirection
