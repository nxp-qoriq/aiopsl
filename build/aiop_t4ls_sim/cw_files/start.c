#define USE_FAR_ADDRESSING_TO_TEXT_SECTION

#include <__mem.h>
#include <__ppc_eabi_linker.h>		/* linker-generated symbol declarations */
//#include <__ppc_eabi_init.h>		/* board- and user-level initialization */

/***************************************************************************/
/*
 *    Function declarations
 */
/***************************************************************************/
void __sys_start(register int argc, register char **argv, register char **envp);
void __sys_start_guest(register int argc, register char **argv, register char **envp);
void _ExitProcess(void);
__declspec(weak) extern void abort(void);
__declspec(weak) extern void exit(int status);


/***************************************************************************/
/*
 *    External declarations
 */
/***************************************************************************/

void * memset(void * dst, int val, size_t n);

extern void main();

extern char         _stack_addr[];     /* Starting address for stack */
extern char         _stack_end[];      /* Address after end byte of stack */

extern char         _SDA_BASE_[];       /* Small Data Area (<=8 bytes) base address
                                           used for .sdata, .sbss */

extern char         _spin_table[];

int  _master = 0xffffffff;


/*****************************************************************************/
static void __init_bss(void)
{
	__bss_init_info *bii = _bss_init_info;

    if (bii->size)
        memset(bii->addr, 0, bii->size);
}


/*****************************************************************************/
extern void abort(void)
{
	_ExitProcess();
}

/*****************************************************************************/
extern void exit(int status)
{
#pragma unused(status)
	_ExitProcess();
}

/*****************************************************************************/
asm void _ExitProcess(void)
{
	nofralloc

    se_illegal
}


/*****************************************************************************/
asm void __sys_start(register int argc, register char **argv, register char **envp)
{
	nofralloc

    /* Init command line arguments */
    mr     r14, argc
    mr     r15, argv
    mr     r16, envp

    /* Store core ID */
    mfpir  r17
    /*"srwi   17, 17, 5 \n"*/

    /* Initialize small data area pointers.
       No _SDA2_BASE_ because r2 is reserved for thread pointer under LinuxABI */
    lis    r13,     _SDA_BASE_@ha
    addi   r13, r13, _SDA_BASE_@l

    /* Initialize stack pointer (based on core ID) */
    cmpwi   r17, 0
    bne     1f
    lis     r1,    _stack_addr@ha
    addi    r1, r1, _stack_addr@l
    b       done_sp

1:
done_sp:
    /* Memory access is safe now */

    /* Set MSR */
    mfmsr  r6
    ori    r6, r6, 0x2000
    mtmsr  r6
    isync

    /* Prepare a terminating stack record */
    stwu   r1, -16(r1)       /* LinuxABI required SP to always be 16-byte aligned */
    li     r0, 0x00000000   /* Load up r0 with 0x00000000 */
    stw    r0,  0(r1)        /* SysVr4 Supp indicated that initial back chain word should be null */
    li     r0, 0xffffffff   /* Load up r0 with 0xffffffff */
    stw    r0, 4(r1)         /* Make an illegal return address of 0xffffffff */

    /* Identify master core (first to arrive) */
    lis     r19, _master@ha
    addi    r19, r19, _master@l
    lwz     r18, 0(r19)
    cmpwi   r18, 0xffffffff
    bne     1f
    stw     r17, 0(r19)   /* Store master core in _master and in r18 */
    mr      r18, r17
    bl      __init_bss   /* Initialize bss section (master core only) */

    /* Branch to main program */
1:
    lis    r6, main@ha
    addi   r6, r6, main@l
    mtlr   r6
    mr     r3, r14
    mr     r4, r15
    mr     r5, r16
    blrl

    /* Exit program */
    lis    r6, exit@ha
    addi   r6, r6, exit@l
    mtlr   r6
    blrl
}

void __sys_start_guest(register int argc, register char **argv, register char **envp)
{
#pragma unused(argc)
#pragma unused(argv)
#pragma unused(envp)
    /* Jump to spin table code at 0x3ffff800 */
    __asm__ (
    "lis    6, _spin_table@ha    \n"
    "addi   6, 6, _spin_table@l  \n"
    "addi   6, 6, 0x800  \n"
    "mtlr   6             \n"
    "blrl                 \n"
    );
}
