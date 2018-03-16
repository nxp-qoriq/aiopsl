/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define USE_FAR_ADDRESSING_TO_TEXT_SECTION

#include <__mem.h>
#include <__ppc_eabi_linker.h>		/* linker-generated symbol declarations */
#include "fsl_types.h"
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

#pragma push
#pragma force_active on
#pragma function_align 256 /* IVPR must be aligned to 256 bytes */

asm static __COLD_CODE void tmp_branch_table(void) {
    nofralloc
    
    /* Critical Input Interrupt (Offset 0x00) */
    li  r3, 0x00
    b  exception_irq
    
    /* Machine Check Interrupt (Offset 0x10) */
    .align 0x10
    li  r3, 0x10
    b  exception_irq
    
    /* Data Storage Interrupt (Offset 0x20) */
    .align 0x10
    li  r3, 0x20
    b  exception_irq

    /* Instruction Storage Interrupt (Offset 0x30) */
    .align 0x10
    li  r3, 0x30
    b  exception_irq

    /* External Input Interrupt (Offset 0x40) */
    .align 0x10
    li  r3, 0x40
    b  exception_irq

    /* Alignment Interrupt (Offset 0x50) */
    .align 0x10
    li  r3, 0x50
    b  exception_irq
    
    /* Program Interrupt (Offset 0x60) */
    .align 0x10
    li  r3, 0x60
    b  exception_irq
    
    /* Performance Monitor Interrupt (Offset 0x70) */
    .align 0x10
    li  r3, 0x70
    b  exception_irq 
    
    /* System Call Interrupt (Offset 0x80) */
    .align 0x10
    li  r3, 0x80
    b  exception_irq 
    
    /* Debug Interrupt (Offset 0x90) */
    .align 0x10
    li  r3, 0x90
    b  exception_irq
    
    /* Embedded Floating-point Data Interrupt (Offset 0xA0) */
    .align 0x10
    li  r3, 0xA0
    b exception_irq
    
    /* Embedded Floating-point Round Interrupt (Offset 0xB0) */
    .align 0x10
    li  r3, 0xB0
    b exception_irq

    /* place holder (Offset 0xC0) */
    .align 0x10
    nop 

    /* place holder (Offset 0xD0) */
    .align 0x10
    nop 

    /* place holder (Offset 0xE0) */
    .align 0x10
    nop 

    /* CTS Task Watchdog Timer Interrupt (Offset 0xF0) */
    .align 0x10
    li  r3, 0xF0
    b exception_irq
    
    /***************************************************/
    /*** generic exception *****************************/
    /***************************************************/
    .align 0x100
exception_irq:
	/* Exit program */
	lis    r6, exit@ha
	addi   r6, r6, exit@l
	mtlr   r6
	blrl
}

#pragma pop

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
asm static __COLD_CODE void __clear_ws(void)
{
	nofralloc

    /* MC clear WS for each core to reset ECC */
    /* The WS is a per core 32K RAM. */
    /* Loops to cover WS, stmw allows 64 bytes (16 GPRS x 4 bytes) writes */
    
    /* clear top 16 registers */
    li r16, 0x0
    mr r17, r16
    mr r18, r16
    mr r19, r16
    mr r20, r16
    mr r21, r16
    mr r22, r16
    mr r23, r16
    mr r24, r16
    mr r25, r16
    mr r26, r16
    mr r27, r16
    mr r28, r16
    mr r29, r16
    mr r30, r16
    mr r31, r16

    mfspr r14, 694 			/* Extract address of DMEM from DMEMCFG0 (SPR 694) */
    lis r13, 0xffff      	/* Low order bits of DMEM size are driven to 0s */
    ori r13, r13, 0xf000
    and r14, r14, r13
    e_li r12, 0x200 		/* set counter to 512 (32K = 512 * 64) */
    mtctr r12
init_ws_loop:
    stmw r16, 0(r14)        /* Write 16 GPRs to WS */
    addi r14, r14, 64       /* Inc the ram ptr; 16 GPRs * 4 bytes = 64 bytes */
    bdnz init_ws_loop       /* Loop until CTR is non-zero */
	
	blr
}

/*****************************************************************************/
asm __COLD_CODE void __sys_start(register int argc, register char **argv, register char **envp)
{
	nofralloc
	
    /* Initialize PPC interrupts vector */
    lis    r31,tmp_branch_table@h
    ori    r31,r31,tmp_branch_table@l
    mtspr  IVPR,r31
    
    bl     __clear_ws
    
    /* Store core ID */
    mfpir  r17
    
    /* Initialize small data area pointers */
    lis    r2, _SDA2_BASE_@ha
    addi   r2, r2, _SDA2_BASE_@l
    lis    r13, _SDA_BASE_@ha
    addi   r13, r13, _SDA_BASE_@l

    /* Initialize stack pointer (based on core ID) */
    lis    r1,    _stack_addr@ha
    addi   r1, r1, _stack_addr@l

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
	lis    r19, _master@ha
	addi   r19, r19, _master@l
    cmpwi  r17, 0
    bne    halt
    bl     __init_bss   /* Initialize bss section (master core only) */
    stw    r17, 0(r19)
halt:
    lwz    r18, 0(r19)
    cmpwi  r18, 0
    bne    halt
    
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

