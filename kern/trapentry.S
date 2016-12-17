/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
 /* trap handler with error code */
 
TRAPHANDLER(vector_8,  T_DBLFLT)
TRAPHANDLER(vector_10, T_TSS)
TRAPHANDLER(vector_11, T_SEGNP)
TRAPHANDLER(vector_12, T_STACK)
TRAPHANDLER(vector_13, T_GPFLT)
TRAPHANDLER(vector_14, T_PGFLT)
TRAPHANDLER(vector_17, T_ALIGN)

/* trap handler without error code */
TRAPHANDLER_NOEC(vector_0, T_DIVIDE);
TRAPHANDLER_NOEC(vector_1, T_DEBUG)
TRAPHANDLER_NOEC(vector_2, T_NMI)
TRAPHANDLER_NOEC(vector_3, T_BRKPT)
TRAPHANDLER_NOEC(vector_4, T_OFLOW)
TRAPHANDLER_NOEC(vector_5, T_BOUND)
TRAPHANDLER_NOEC(vector_6, T_ILLOP)
TRAPHANDLER_NOEC(vector_7, T_DEVICE)

TRAPHANDLER_NOEC(vector_16, T_FPERR)
TRAPHANDLER_NOEC(vector_18, T_MCHK)
TRAPHANDLER_NOEC(vector_19, T_SIMDERR)

TRAPHANDLER_NOEC(vector_32, IRQ_OFFSET+IRQ_TIMER)	// offset 0
TRAPHANDLER_NOEC(vector_33, IRQ_OFFSET+IRQ_KBD)
TRAPHANDLER_NOEC(vector_36, IRQ_OFFSET+IRQ_SERIAL)
TRAPHANDLER_NOEC(vector_39, IRQ_OFFSET+IRQ_SPURIOUS)
TRAPHANDLER_NOEC(vector_46, IRQ_OFFSET+IRQ_IDE)
TRAPHANDLER_NOEC(vector_51, IRQ_OFFSET+IRQ_ERROR)

TRAPHANDLER_NOEC(vector_48, T_SYSCALL)	// int 0x30 syscall





/*
 * Lab 3: Your code here for _alltraps
 */
 
 _alltraps:
# push values to make the stack look like a struct Trapframe
# pushal pushes struct PushRegs, then %es , %ds  and pointer to trapFrame
	pushl %ds
	pushl %es
	pushal
# load GD_KD into %ds and %es
	movl $GD_KD, %eax
	movw %ax, %ds
	movw %ax, %es
# pushl %esp to pass a pointer to the Trapframe as an argument to trap()
	pushl %esp
	call trap

