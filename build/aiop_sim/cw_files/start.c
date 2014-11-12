/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define USE_FAR_ADDRESSING_TO_TEXT_SECTION

#include <__mem.h>
#include <__ppc_eabi_linker.h>		/* linker-generated symbol declarations */
#include "types.h"
//#include <__ppc_eabi_init.h>		/* board- and user-level initialization */

/***************************************************************************/
/*
 *    Function declarations
 */
/***************************************************************************/
void __sys_start(register int argc, register char **argv, register char **envp);
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

int  _master = 0xffffffff;


/*****************************************************************************/
__COLD_CODE static void __init_bss(void)
{
    __bss_init_info *bii = _bss_init_info;

    while(bii->size) {
        memset(bii->addr, 0, bii->size);
        bii++;
    }
}


/*****************************************************************************/
__COLD_CODE extern void abort(void)
{
	_ExitProcess();
}

/*****************************************************************************/
__COLD_CODE extern void exit(int status)
{
#pragma unused(status)
	_ExitProcess();
}

/*****************************************************************************/
asm __COLD_CODE void _ExitProcess(void)
{
	nofralloc
	
	/* 
	 * Debug Notify Halt 
	 * Acts as 'se_illegal' if EDBCR0[DNH_EN] is set
	 */
	se_dnh
}


/*****************************************************************************/
asm __COLD_CODE void __sys_start(register int argc, register char **argv, register char **envp)
{
	nofralloc

    /* Store core ID */
    mfpir  r17

    /* Initialize small data area pointers */
    lis    r2, _SDA2_BASE_@ha
    addi   r2, r2, _SDA2_BASE_@l
    lis    r13, _SDA_BASE_@ha
    addi   r13, r13, _SDA_BASE_@l

    /* Initialize stack pointer (based on core ID) */
    lis     r1,    _stack_addr@ha
    addi    r1, r1, _stack_addr@l

    /* Memory access is safe now */
    
    /* Set MSR */
    mfmsr  r6
    ori    r6, r6, 0x0200 /* DE */
    mtmsr  r6
    isync

    /* Prepare a terminating stack record */
    stwu   r1, -16(r1)       /* LinuxABI required SP to always be 16-byte aligned */
    li     r0, 0x00000000   /* Load up r0 with 0x00000000 */
    stw    r0,  0(r1)        /* SysVr4 Supp indicated that initial back chain word should be null */
    li     r0, 0xffffffff   /* Load up r0 with 0xffffffff */
    stw    r0, 4(r1)         /* Make an illegal return address of 0xffffffff */
   
    /* master-core clears bss sections while others wait */
	lis     r19, _master@ha
	addi    r19, r19, _master@l
    cmpwi   r17, 0
    bne     halt
    bl      __init_bss   /* Initialize bss section (master core only) */
    stw     r17, 0(r19)
halt:
    lwz     r18, 0(r19)
    cmpwi   r18, 0
    bne     halt

    /* Branch to main program */
    lis    r6, main@ha
    addi   r6, r6, main@l
    mtlr   r6
    mr     r3, argc /* Init command line arguments */
    mr     r4, argv
    mr     r5, envp
    blrl

    /* Exit program */
    lis    r6, exit@ha
    addi   r6, r6, exit@l
    mtlr   r6
    blrl
}

