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
#include "fsl_types.h"
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

static inline void booke_exception_machine_check_isr()
{
	uint32_t mcsrr0 = booke_get_spr_MCSRR0(); /* Last executed instruction(best effort) */
	uint32_t mcsrr1 = booke_get_spr_MCSRR1(); /* MSR at the time of the interrupt */
	uint32_t mcar = booke_get_spr_MCAR();     /* Address register */
	uint32_t mcsr = booke_get_spr_MCSR();     /* Syndrome register */
	uint32_t core_id = core_get_id();

	fsl_print("core %d int: MACHINE_CHECK\n", core_id);
	if(mcsr & 0x0400 /* STACK_ERR */) {
		uint32_t dac1, dac2; /* Data Address Compare Registers */

		fsl_print("core #%d: Stack Overflow Exception...\n", core_id);

		dac1 = booke_get_spr_DAC1();
		dac2 = booke_get_spr_DAC2();
		fsl_print("core #%d: Stack size is: 0x%x Bytes\n", core_id, (dac2 - dac1));
	}

	if(mcsr & 0x80000 /* MAV */) {
		if(mcsr & 0x40000 /* MEA */) {
			fsl_print("core #%d: Violation effective address: 0x%x\n", core_id, mcar);
		}
		else {
			fsl_print("core #%d: Violation real address: 0x%x\n", core_id, mcar);
		}
	}

	if(mcsr & 0x8000 /* LD */) {
		fsl_print("core #%d: An error occurred during the attempt to execute the load type instruction located at 0x%x.\n", core_id, mcsrr0);
	}

	if(mcsr & 0x4000 /* ST */) {
		fsl_print("core #%d: An error occurred during the attempt to execute the store type instruction located at 0x%x.\n", core_id, mcsrr0);
	}
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
	case(CRITICAL_INTR):
		fsl_print("core %d int: CRITICAL\n", core_get_id());
		break;
	case(MACHINE_CHECK_INTR): /* MACHINE_CHECK */
		booke_exception_machine_check_isr();
		break;
	case(DATA_STORAGE_INTR):
		fsl_print("core %d int: DATA_STORAGE\n", core_get_id());
		break;
	case(INSTRUCTION_STORAGE_INTR):
		fsl_print("core %d int: INSTRUCTION_STORAGE\n", core_get_id());
		break;
	case(EXTERNAL_INTR):
		fsl_print("core %d int: EXTERNAL\n", core_get_id());
		break;
	case(ALIGNMENT_INTR):
		fsl_print("core %d int: ALIGNMENT\n", core_get_id());
		break;
	case(PROGRAM_INTR):
		fsl_print("core %d int: PROGRAM\n", core_get_id());
		break;
	case(PERF_MONITOR_INTR):
		fsl_print("core %d int: performance monitor\n", core_get_id());
		break;
	case(SYSTEM_CALL_INTR):
		fsl_print("core %d int: SYSTEM CALL\n", core_get_id());
		break;
	case(DEBUG_INTR):
		fsl_print("core %d int: debug\n", core_get_id());
		break;
	case(EFPU_DATA_INTR):
		fsl_print("core %d int: floating point data\n", core_get_id());
		break;
	case(EFPU_ROUND_INTR):
		fsl_print("core %d int: floating point round\n", core_get_id());
		break;
	case(EFPU_NA_INTR):
		fsl_print("core %d int: floating point unavailable\n", core_get_id());
		break;
	case(CTS_WD_INTR):
		fsl_print("core %d int: CTS Task Watchdog interrupt\n", core_get_id());
		break;
	default:
		fsl_print("undefined interrupt #%x\n", intr_entry);
		break;
	}

	while(1){}
}

asm static void branch_table(void) {
    nofralloc

    /* Critical Input Interrupt (Offset 0x00) */
    li  r3, CRITICAL_INTR
    b  generic_irq

    /* Machine Check Interrupt (Offset 0x10) */
    .align 0x10
    li  r3, MACHINE_CHECK_INTR
    b  machine_irq

    /* Data Storage Interrupt (Offset 0x20) */
    .align 0x10
    li  r3, DATA_STORAGE_INTR
    b  generic_irq

    /* Instruction Storage Interrupt (Offset 0x30) */
    .align 0x10
    li  r3, INSTRUCTION_STORAGE_INTR
    b  generic_irq

    /* External Input Interrupt (Offset 0x40) */
    .align 0x10
    li  r3, EXTERNAL_INTR
    b  generic_irq

    /* Alignment Interrupt (Offset 0x50) */
    .align 0x10
    li  r3, ALIGNMENT_INTR
    b  generic_irq

    /* Program Interrupt (Offset 0x60) */
    .align 0x10
    li  r3, PROGRAM_INTR
    b  generic_irq

    /* Performance Monitor Interrupt (Offset 0x70) */
    .align 0x10
    li  r3, PERF_MONITOR_INTR
    b  generic_irq

    /* System Call Interrupt (Offset 0x80) */
    .align 0x10
    li  r3, SYSTEM_CALL_INTR
    b  generic_irq

    /* Debug Interrupt (Offset 0x90) */
    .align 0x10
    li  r3, DEBUG_INTR
    b  generic_irq

    /* Embedded Floating-point Data Interrupt (Offset 0xA0) */
    .align 0x10
    li  r3, EFPU_DATA_INTR
    b generic_irq

    /* Embedded Floating-point Round Interrupt (Offset 0xB0) */
    .align 0x10
    li  r3, EFPU_ROUND_INTR
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
    li  r3, CTS_WD_INTR
    b generic_irq

    /***************************************************/
    /*** machine check *********************************/
    /***************************************************/
    .align 0x100
machine_irq:
	mfspr    r4, MCSR
	se_btsti r4,21 /* test bit 0x00000400 - STACK_ERR */
	beq      generic_irq /* branch to generic irq if machine_check is not stack error */
	mfdcr    r4, CTSCSR0 /* CTSCSR0 */
	andis.   r4, r4, CTSCSR_TASKS_MASK@h
	/* case: 16 tasks */
	lis      r0, CTSCSR_16_TASKS@h
	cmpw     r4, r0
	li       rsp, 0x7f0  /* 0x800 - 0xf = 0x7f0 */
	beq      generic_irq
	/* case: 8 tasks */
	lis      r0, CTSCSR_8_TASKS@h
	cmpw     r4, r0
	li       rsp, 0xff0  /* 0x1000 - 0xf = 0xff0 */
	beq      generic_irq
	/* case: 4 tasks */
	lis      r0, CTSCSR_4_TASKS@h
	cmpw     r4, r0
	li       rsp, 0x1ff0 /* 0x2000 - 0xf = 0x1ff0 */
	beq      generic_irq
	/* case: 2 tasks */
	lis      r0, CTSCSR_2_TASKS@h
	cmpw     r4, r0
	li       rsp, 0x3ff0 /* 0x4000 - 0xf = 0x3ff0 */
	beq      generic_irq
	/* case: 1 task */
	lis      r0, CTSCSR_1_TASKS@h
	cmpw     r4, r0
	li       rsp, 0x7ff0 /* 0x8000 - 0xf = 0x7ff0 */
	beq      generic_irq
	/* case: else (default) */
	se_illegal
	se_dnh

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
