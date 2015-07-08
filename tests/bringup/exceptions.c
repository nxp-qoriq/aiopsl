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

#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_malloc.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"
#include "fsl_dbg.h"
#include "inline_asm.h"
#include "fsl_io_ccsr.h"
#include "cmgw.h"
#include "aiop_common.h"
#include "fsl_core_booke_regs.h"


#if 0
static struct aiop_cmgw_regs *cmgw_regs;

static void init()
{
	cmgw_regs = (struct aiop_cmgw_regs *)\
		(SOC_PERIPH_OFF_AIOP_TILE + SOC_PERIPH_OFF_AIOP_CMGW);
}

static uint32_t _cmgw_get_ntasks()
{
	return (ioread32_ccsr(&cmgw_regs->wscr) & CMGW_WSCR_NTASKS_MASK);
}

static inline void config_runtime_stack_overflow_detection()
{
	switch(_cmgw_get_ntasks())
	{
	case 0: /* 1 Task */
		booke_set_spr_DAC2(0x8000);
		break;
	case 1: /* 2 Tasks */
		booke_set_spr_DAC2(0x4000);
		break;
	case 2: /* 4 Tasks */
		booke_set_spr_DAC2(0x2000);
		break;
	case 3: /* 8 Tasks */
		booke_set_spr_DAC2(0x1000);
		break;
	case 4: /* 16 Tasks */
		booke_set_spr_DAC2(0x800);
		break;
	default:
		//TODO complete
		break;
	}
}
#endif

static inline void _configure_stack_overflow_detection(void)
{
    /* DBCR2 */
    booke_set_spr_DBCR2(booke_get_spr_DBCR2() | 0x00c00000);

    /* DBCR4 */
    asm {
        mfspr   r6,DBCR4
        ori r6, r6, 0x0080 /* DAC1CFG */
        mtspr   DBCR4,r6
        isync
    }
//  booke_set_spr_DBCR4(booke_get_spr_DBCR4() | 0x00000080);
//  booke_instruction_sync();

    /* DBCR0 */
    booke_set_spr_DBCR0(booke_get_spr_DBCR0() | 0x400f0000);
    booke_instruction_sync();

    /* initiate DAC registers */
    booke_set_spr_DAC1(0x400);
    booke_set_spr_DAC2(0x8000);
}

static void check_exceptions()
{
	uint32_t esr, mcsr;

	esr = booke_get_spr_ESR();
	mcsr = booke_get_spr_MCSR();

	if (!esr && !mcsr) while(1) {}; /* FAILED : EXCEPTION_TEST */
}

#pragma push
#pragma section code_type ".interrupt_vector"
#pragma force_active on
#pragma function_align 256 /* IVPR must be aligned to 256 bytes */

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
void _booke_generic_exception_isr(uint32_t intr_entry);
void _booke_generic_exception_isr(uint32_t intr_entry)
{
	switch(intr_entry)
	{
	case(0x00):
		break;
	case(0x10):
		break;
	case(0x20):
		break;
	case(0x30):
		break;
	case(0x40):
		break;
	case(0x50):
		break;
	case(0x60):
		break;
	case(0x70):
		break;
	case(0x80):
		break;
	case(0x90):
		break;
	case(0xA0):
		break;
	case(0xB0):
		break;
	case(0xF0):
		break;
	default:
		break;
	}
	check_exceptions();
	while(1) {} /* PASSED : EXCEPTION_TEST */
}

asm static void _branch_table(void) {
    nofralloc

    /* Critical Input Interrupt (Offset 0x00) */
    li  r3, 0x00
    b  exception_irq

    /* Machine Check Interrupt (Offset 0x10) */
    .align 0x10
    li  r3, 0x10
    b  machine_irq

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
    li       r0, 0x00000000
    /* disable exceptions and interrupts */
    mtspr    DBSR, r0
    mtspr    DBCR0, r0
    mtspr    DBCR2, r0
    /* disable debug and interrupts in MSR */
    mtmsr    r0
    isync
    /* update stack overflow detection to 1-task (0x8000) */
    se_bgeni r4,16
    mtspr    DAC2,r4
    /* clear stack pointer */
    li       rsp, 0x7ff0
    /* branch to isr */
    lis      r4,_booke_generic_exception_isr@h
    ori      r4,r4,_booke_generic_exception_isr@l
    mtlr     r4
    blrl
    se_illegal

    /***************************************************/
    /*** machine check *********************************/
    /***************************************************/
    .align 0x100
machine_irq:
    se_dnh  /* PASSED : STACK_OVERFLOW_TEST */
}

asm static void _booke_init_interrupt_vector(void)
{
    lis     r3,_branch_table@h
    ori     r3,r3,_branch_table@l
    mtspr   IVPR,r3
}

#pragma pop

__HOT_CODE static int func_in_iram()
{
	int err = 0;

	err = -1;

	return err;
}

static int recursion_func(int i)
{
	int arr[0x1000];

	arr[i] = i + 1;

	if (i == 1000)
		return 0;
	else
		recursion_func(arr[i]);
	return 0;
}

int stack_ovf_test();
int stack_ovf_test()
{
	_booke_init_interrupt_vector();

	/* Stack overflow */
	_configure_stack_overflow_detection();

	recursion_func(1);
	return -EINVAL;
}

int exceptions_test();
int exceptions_test()
{
	int err = 0;
	uint8_t *iram_ptr = (uint8_t *)((void *)func_in_iram);

	_booke_init_interrupt_vector();

	/* Write to IRAM */
	iram_ptr[0] = 0xff;
	iram_ptr[1] = 0xff;
	if ((iram_ptr[1] != 0xff) || (iram_ptr[1] != 0xff))
		return -EINVAL;

	/* Trigger exception */
	err = func_in_iram();

	return -EINVAL;
}
