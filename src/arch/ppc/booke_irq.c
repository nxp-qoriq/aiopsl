/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

/*
 *
 * @File          booke_irq.c
 * 
 * @Description   This is an E200 z490 specific implementation. 
 *                This file contain the PPC general exception 
 *                interrupt routines.
 *
 * @Cautions
 *
 *
 * Author: Danny Shultz.
 *
 */

#include "fsl_core_booke_regs.h"
#include "types.h"
#include "fsl_dbg.h"

#pragma push
#pragma section code_type ".interrupt_vector"
#pragma force_active on
#pragma function_align 256 /* IVPR must be aligned to 256 bytes */

void booke_init_interrupt_vector(void);
void booke_generic_exception_isr(uint32_t intr_entry);

/*****************************************************************/
/* routine:       booke_generic_irq_init                         */
/*                                                               */
/* description:                                                  */
/*        Initialization of pointers to the exception handlers.  */
/*                                                               */
/* arguments:                                                    */
/*    None.                                                      */
/*                                                               */
/*****************************************************************/
__COLD_CODE void booke_generic_irq_init(void)
{
    booke_init_interrupt_vector();
}

/*****************************************************************/
/* routine:       booke_generic_exception_isr                    */
/*                                                               */
/* description:                                                  */
/*    Internal routine, called by the main interrupt handler     */
/*    to indicate the occurrence of an INT.                      */
/*                                                               */
/* arguments:                                                    */
/*    intrEntry (In) - The interrupt handler original offset     */
/*                     from IVPR register.                       */
/*                                                               */
/*****************************************************************/
void booke_generic_exception_isr(uint32_t intr_entry)
{	
	switch(intr_entry)
	{
	case(0x00):
		pr_debug("core %d int: CRITICAL\n", core_get_id());
		break;
	case(0x10):
		{
			uint32_t mcsr = booke_get_spr_MCSR();
			uint32_t core_id = core_get_id();
			pr_debug("core %d int: MACHINE_CHECK\n", core_id);
			if(mcsr & 0x0400 /* STACK_ERR */) {
				pr_debug("Stack overflow Exception: MCSR = 0x%x\n", mcsr);
			}
			break;
		}
	case(0x20):
		pr_debug("core %d int: DATA_STORAGE\n", core_get_id());
		break;
	case(0x30):
		pr_debug("core %d int: INSTRUCTION_STORAGE\n", core_get_id());
		break;
	case(0x40):
		pr_debug("core %d int: EXTERNAL\n", core_get_id());
		break;
	case(0x50):
		pr_debug("core %d int: ALIGNMENT\n", core_get_id());
		break;
	case(0x60):
		pr_debug("core %d int: PROGRAM\n", core_get_id());
		break;
	case(0x70):
		pr_debug("core %d int: SYSTEM CALL\n", core_get_id());
		break;
	case(0x80):
		pr_debug("core %d int: debug\n", core_get_id());
		break;
	case(0x90):
		pr_debug("core %d int: SPE-floating point data\n", core_get_id());
		break;
	case(0xA0):
		pr_debug("core %d int: SPE-floating point round\n", core_get_id());
		break;
	case(0xB0):
		pr_debug("core %d int: performance monitor\n", core_get_id());
		break;
	case(0xF0): 
		pr_debug("core %d int: CTS interrupt\n", core_get_id());
		break;
	default:
		pr_warn("undefined interrupt #%x\n", intr_entry);
		break;
	}
	
	while(1){}
}

asm static void branch_table(void) {
    nofralloc
    
    /* Critical Input Interrupt (Offset 0x00) */
    li  r3, 0x00
    b  generic_irq
    
    /* Machine Check Interrupt (Offset 0x10) */
    .align 0x10
    li  r3, 0x10
    b  machine_irq
    
    /* Data Storage Interrupt (Offset 0x20) */
    .align 0x10
    li  r3, 0x20
    b  generic_irq

    /* Instruction Storage Interrupt (Offset 0x30) */
    .align 0x10
    li  r3, 0x30
    b  generic_irq

    /* External Input Interrupt (Offset 0x40) */
    .align 0x10
    li  r3, 0x40
    b  generic_irq

    /* Alignment Interrupt (Offset 0x50) */
    .align 0x10
    li  r3, 0x50
    b  generic_irq
    
    /* Program Interrupt (Offset 0x60) */
    .align 0x10
    li  r3, 0x60
    b  generic_irq
    
    /* Performance Monitor Interrupt (Offset 0x70) */
    .align 0x10
    li  r3, 0x70
    b  generic_irq 
    
    /* System Call Interrupt (Offset 0x80) */
    .align 0x10
    li  r3, 0x80
    b  generic_irq 
    
    /* Debug Interrupt (Offset 0x90) */
    .align 0x10
    li  r3, 0x90
    b  generic_irq
    
    /* Embedded Floating-point Data Interrupt (Offset 0xA0) */
    .align 0x10
    li  r3, 0xA0
    b generic_irq
    
    /* Embedded Floating-point Round Interrupt (Offset 0xB0) */
    .align 0x10
    li  r3, 0xB0
    b generic_irq

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
    b generic_irq
    
    /***************************************************/
    /*** machine check *********************************/
    /***************************************************/
    .align 0x100
machine_irq:
	mfspr    r4, MCSR
	se_btsti r4,21 /* test bit 0x00000400 - STACK_ERR */
	beq      generic_irq
    li       rsp,  0x7f0 /* clear stack pointer - for 2k WS (smallest) */
    b        generic_irq
	
    /***************************************************/
    /*** generic exception handler *********************/
    /***************************************************/
    .align 0x100
generic_irq:
	/* branch to isr */
	lis      r4,booke_generic_exception_isr@h
	ori      r4,r4,booke_generic_exception_isr@l
	mtlr     r4
	blr
	
}

__COLD_CODE asm void booke_init_interrupt_vector(void)
{
    lis     r3,branch_table@h
    ori     r3,r3,branch_table@l
    mtspr   IVPR,r3
}

#pragma pop
