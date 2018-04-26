/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2018 NXP
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

	pr_info("int: MACHINE_CHECK\n");
	if(mcsr & 0x0400 /* STACK_ERR */) {
		uint32_t dac1, dac2; /* Data Address Compare Registers */

		pr_err("Stack Overflow Exception...\n");

		dac1 = booke_get_spr_DAC1();
		dac2 = booke_get_spr_DAC2();
		pr_err("Stack size is: 0x%x Bytes\n", (dac2 - dac1));
	}

	if(mcsr & 0x80000 /* MAV */) {
		if(mcsr & 0x40000 /* MEA */) {
			pr_err("Violation effective address: 0x%x\n", mcar);
		}
		else {
			pr_err("Violation real address: 0x%x\n", mcar);
		}
	}

	if(mcsr & 0x8000 /* LD */) {
		pr_err("Error on load type instruction at address 0x%x.\n", mcsrr0);
	}

	if(mcsr & 0x4000 /* ST */) {
		pr_err("Error on store type instruction at address 0x%x.\n", mcsrr0);
	}
}

#ifdef AIOPSL_KERNEL_EXCEPTION_HOOK
__declspec(weak) void booke_debug_hook(uint32_t intr_type);
__COLD_CODE void booke_debug_hook(uint32_t intr_type)
#else
__COLD_CODE static void booke_debug_hook(uint32_t intr_type)
#endif
{
	UNUSED(intr_type);
	/* Use tmp variable in debugger to get out of the loop */
	volatile int tmp = 1;
	while (tmp == 1)
		asm("nop");
}

__COLD_CODE static void booke_non_critical_isr(void)
{
	/* Obtain ID of task which generated this exception */
	register uint32_t task,tmp;

	asm
	{
		mfdcr	tmp,dcr476	/* TASKCSR0 */
		e_clrlwi task,tmp,24	/* clear top 24 bits */
	}

	/* Some registers dump which could be useful to understand root cause
	 * For details & explanations see document
	 * AIOP_Z490_CPU_Spec_v1.3 sections 5.7.x
	 */
	pr_info("Core ID: 0x%x\n", core_get_id());
	pr_info("TASK ID: 0x%x\n", task);
	pr_info("SRR0: 0x%08x, SRR1: 0x%08x\n",
		booke_get_spr_SRR0(),
		booke_get_spr_SRR1());
	pr_info("ESR: 0x%08x, DEAR: 0x%08x, MCSR: 0x%08x\n",
		booke_get_spr_ESR(),
		booke_get_spr_DEAR(),
		booke_get_spr_MCSR());
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
	/*
	 * According to table 5-1 AIOP_Z490_CPU_Spec_v1.3
	 * these interrupts are considered non-critical
	 * All interrupts provide info in same registers
	 */
	case(DATA_STORAGE_INTR):
	case(INSTRUCTION_STORAGE_INTR):
	case(EXTERNAL_INTR):
	case(ALIGNMENT_INTR):
	case(PROGRAM_INTR):
	case(PERF_MONITOR_INTR):
		/* Table 5-2. Exceptions and Conditions - to
		 * determine ID <-> name mapping */
		pr_info("Non-critical interrupt: 0x%02x\n", intr_entry);
		booke_non_critical_isr();
		break;
	case(CRITICAL_INTR):
		/* CSRR0: effective address of the instruction that the processor
		 * would have attempted to execute next if no exception conditions
		 * were present.
		 * CSRR1: contents of the MSR at the time of the interrupt
		 */
		pr_info("int: CRITICAL: CSSRR0=0x%08x, CSRR1=0x%08x\n",
				booke_get_spr_CSRR0(), booke_get_spr_CSRR1());
		break;
	case(MACHINE_CHECK_INTR): /* MACHINE_CHECK */
		booke_exception_machine_check_isr();
		break;
	case(SYSTEM_CALL_INTR):
		pr_info("int: SYSTEM CALL\n");
		break;
	case(DEBUG_INTR):
		pr_info("int: debug\n");
		break;
	case(EFPU_DATA_INTR):
		pr_info("int: floating point data\n");
		break;
	case(EFPU_ROUND_INTR):
		pr_info("int: floating point round\n");
		break;
	case(EFPU_NA_INTR):
		pr_info("int: floating point unavailable\n");
		break;
	case(CTS_WD_INTR):
		pr_info("int: CTS Task Watchdog interrupt\n");
		break;
	default:
		pr_err("undefined interrupt #%x\n", intr_entry);
		break;
	}

	booke_debug_hook(intr_entry);
}

asm static void branch_table(void) {
	nofralloc

	/* Critical Input Interrupt (Offset 0x00) */
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, CRITICAL_INTR
	b  critical_irq

	/* Machine Check Interrupt (Offset 0x10) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, MACHINE_CHECK_INTR
	b  machine_irq

	/* Data Storage Interrupt (Offset 0x20) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, DATA_STORAGE_INTR
	b generic_rfi

	/* Instruction Storage Interrupt (Offset 0x30) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, INSTRUCTION_STORAGE_INTR
	b generic_rfi

	/* External Input Interrupt (Offset 0x40) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, EXTERNAL_INTR
	b generic_rfi

	/* Alignment Interrupt (Offset 0x50) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, ALIGNMENT_INTR
	b generic_rfi

	/* Program Interrupt (Offset 0x60) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, PROGRAM_INTR
	b generic_rfi

	/* Performance Monitor Interrupt (Offset 0x70) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, PERF_MONITOR_INTR
	b generic_rfi

	/* System Call Interrupt (Offset 0x80) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, SYSTEM_CALL_INTR
	b generic_rfi

	/* Debug Interrupt (Offset 0x90) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, DEBUG_INTR
	b  dbg_irq

	/* Embedded Floating-point Data Interrupt (Offset 0xA0) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, EFPU_DATA_INTR
	b generic_rfi

	/* Embedded Floating-point Round Interrupt (Offset 0xB0) */
	.align 0x10
	stwu     rsp,-80(rsp)
	stw      r3,40(rsp)
	li  r3, EFPU_ROUND_INTR
	b generic_rfi

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
	/*** Critical interrupt *********************************/
	/***************************************************/
    .align 0x100
critical_irq:
	stw      r0,8(rsp)
	mfctr    r0
	stw      r0,12(rsp)
	mfxer    r0
	stw      r0,16(rsp)
	mfcr     r0
	stw      r0,20(rsp)
	mflr     r0
	stw      r0,24(rsp)
	stw      r3,40(rsp)
	stw      r4,44(rsp)
	stw      r5,48(rsp)
	stw      r6,52(rsp)
	stw      r7,56(rsp)
	stw      r8,60(rsp)
	stw      r9,64(rsp)
	stw      r10,68(rsp)
	stw      r11,72(rsp)
	stw      r12,76(rsp)
	mfspr    r0,CSRR0 /* Critical save/restore register 0 */
	stw      r0,28(rsp)
	mfspr    r0,CSRR1 /* Critical save/restore register 1 */
	stw      r0,32(rsp)

	lis r4,booke_generic_exception_isr@h
	ori r4,r4,booke_generic_exception_isr@l
	mtlr    r4
	blrl

	lwz      r0,32(rsp)
	mtspr    CSRR1,r0
	lwz      r0,28(rsp)
	mtspr    CSRR0,r0
	lwz      r3,40(rsp)
	lwz      r4,44(rsp)
	lwz      r5,48(rsp)
	lwz      r6,52(rsp)
	lwz      r7,56(rsp)
	lwz      r8,60(rsp)
	lwz      r9,64(rsp)
	lwz      r10,68(rsp)
	lwz      r11,72(rsp)
	lwz      r12,76(rsp)
	lwz      r0,24(rsp)
	mtlr     r0
	lwz      r0,20(rsp)
	mtcrf    0xff,r0
	lwz      r0,16(rsp)
	mtxer    r0
	lwz      r0,12(rsp)
	mtctr    r0
	lwz      r0,8(rsp)
	addi     rsp,rsp,80
	rfci

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
	/*** Debug interrupt *********************************/
	/***************************************************/
	.align 0x100
dbg_irq:
	stw      r0,8(rsp)
	mfctr    r0
	stw      r0,12(rsp)
	mfxer    r0
	stw      r0,16(rsp)
	mfcr     r0
	stw      r0,20(rsp)
	mflr     r0
	stw      r0,24(rsp)
	stw      r3,40(rsp)
	stw      r4,44(rsp)
	stw      r5,48(rsp)
	stw      r6,52(rsp)
	stw      r7,56(rsp)
	stw      r8,60(rsp)
	stw      r9,64(rsp)
	stw      r10,68(rsp)
	stw      r11,72(rsp)
	stw      r12,76(rsp)
	mfspr    r0,DSRR0
	stw      r0,28(rsp)
	mfspr    r0,DSRR1
	stw      r0,32(rsp)

	lis r4,booke_generic_exception_isr@h
	ori r4,r4,booke_generic_exception_isr@l
	mtlr    r4
	blrl

	lwz      r0,32(rsp)
	mtspr    DSRR1,r0
	lwz      r0,28(rsp)
	mtspr    DSRR0,r0
	lwz      r3,40(rsp)
	lwz      r4,44(rsp)
	lwz      r5,48(rsp)
	lwz      r6,52(rsp)
	lwz      r7,56(rsp)
	lwz      r8,60(rsp)
	lwz      r9,64(rsp)
	lwz      r10,68(rsp)
	lwz      r11,72(rsp)
	lwz      r12,76(rsp)
	lwz      r0,24(rsp)
	mtlr     r0
	lwz      r0,20(rsp)
	mtcrf    0xff,r0
	lwz      r0,16(rsp)
	mtxer    r0
	lwz      r0,12(rsp)
	mtctr    r0
	lwz      r0,8(rsp)
	addi     rsp,rsp,80
	rfdi

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

	.align 0x100
generic_rfi:
	stw      r0,8(rsp)
	mfctr    r0
	stw      r0,12(rsp)
	mfxer    r0
	stw      r0,16(rsp)
	mfcr     r0
	stw      r0,20(rsp)
	mflr     r0
	stw      r0,24(rsp)
	stw      r4,44(rsp)
	stw      r5,48(rsp)
	stw      r6,52(rsp)
	stw      r7,56(rsp)
	stw      r8,60(rsp)
	stw      r9,64(rsp)
	stw      r10,68(rsp)
	stw      r11,72(rsp)
	stw      r12,76(rsp)
	mfsrr0   r0
	stw      r0,28(rsp)
	mfsrr1   r0
	stw      r0,32(rsp)

	lis r4,booke_generic_exception_isr@h
	ori r4,r4,booke_generic_exception_isr@l
	mtlr    r4
	blrl

	lwz      r0,32(rsp)
	mtsrr1   r0
	lwz      r0,28(rsp)
	mtsrr0   r0
	lwz      r3,40(rsp)
	lwz      r4,44(rsp)
	lwz      r5,48(rsp)
	lwz      r6,52(rsp)
	lwz      r7,56(rsp)
	lwz      r8,60(rsp)
	lwz      r9,64(rsp)
	lwz      r10,68(rsp)
	lwz      r11,72(rsp)
	lwz      r12,76(rsp)
	lwz      r0,24(rsp)
	mtlr     r0
	lwz      r0,20(rsp)
	mtcrf    0xff,r0
	lwz      r0,16(rsp)
	mtxer    r0
	lwz      r0,12(rsp)
	mtctr    r0
	lwz      r0,8(rsp)
	addi     rsp,rsp,80
	rfi
}

__COLD_CODE asm void booke_init_interrupt_vector(void)
{
	lis     r3,branch_table@h
	ori     r3,r3,branch_table@l
	mtspr   IVPR,r3
}

#pragma pop
