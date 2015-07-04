/* See COPYRIGHT for copyright information. */
#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>

 /* In Exercise4, it says,"push values to make the stack look like a struct Trapframe" , go and check the definition of Trapframe,then you will understand what the "look like" means and every pierce of instruction  */

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
/* The nessarity  to distinguish TRAPHANDLER and TRAPHANDLER_NOEC is also to "make the stack look like a struct trapframe"*/
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
TRAPHANDLER_NOEC(routine_divide,T_DIVIDE)
TRAPHANDLER_NOEC(routine_debug,T_DEBUG)
TRAPHANDLER_NOEC(routine_nmi,T_NMI)
TRAPHANDLER_NOEC(routine_brkpt,T_BRKPT)
TRAPHANDLER_NOEC(routine_oflow,T_OFLOW)
TRAPHANDLER_NOEC(routine_bound,T_BOUND)
TRAPHANDLER_NOEC(routine_illop,T_ILLOP)
TRAPHANDLER_NOEC(routine_device,T_DEVICE)
TRAPHANDLER(routine_dblflt,T_DBLFLT)
TRAPHANDLER(routine_tss,T_TSS)
TRAPHANDLER(routine_segnp,T_SEGNP)
TRAPHANDLER(routine_stack,T_STACK)
TRAPHANDLER(routine_gpflt,T_GPFLT)
TRAPHANDLER(routine_pgflt,T_PGFLT)
TRAPHANDLER(routine_fperr,T_FPERR)
TRAPHANDLER(routine_align,T_ALIGN)
TRAPHANDLER(routine_mchk,T_MCHK)
TRAPHANDLER(routine_simderr,T_SIMDERR)

TRAPHANDLER_NOEC(routine_syscall,T_SYSCALL)



TRAPHANDLER_NOEC(routine_timer,IRQ_TIMER+IRQ_OFFSET)
TRAPHANDLER_NOEC(routine_kbd,IRQ_KBD+IRQ_OFFSET)
TRAPHANDLER_NOEC(routine_serial,IRQ_SERIAL+IRQ_OFFSET)
TRAPHANDLER_NOEC(routine_spurious,IRQ_SPURIOUS+IRQ_OFFSET)
TRAPHANDLER_NOEC(routine_ide,IRQ_IDE+IRQ_OFFSET)
TRAPHANDLER_NOEC(routine_error,IRQ_ERROR+IRQ_OFFSET)
/*
 * Lab 3: Your code here for _alltraps
 */
_alltraps:
pushw $0x0          /* pushw pushs a 16-bit word*/
pushw %ds
pushw $0x0
pushw %es

pushal
movl $GD_KD, %eax
movw %ax, %ds
movw %ax, %es
pushl %esp
call trap
