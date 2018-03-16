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

/***************************************************************************//**

@File          cmgw.c

@Description   Command Gateway
 *//***************************************************************************/

#include "fsl_types.h"
#include "fsl_aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_spinlock.h"
#include "fsl_core.h"
#include "fsl_dbg.h"
#include "fsl_cmgw.h"

#define TIME_BASE_IS_UPDATING (0xffffffff)

static struct aiop_cmgw_regs * cmgw_regs;

/* The lock must be aligned to a double word boundary. */
uint64_t abcr_lock __attribute__((aligned(8))) = 0;

/******************************************************************************/
void cmgw_init(void)
{
	cmgw_regs = (struct aiop_cmgw_regs *)(AIOP_PERIPHERALS_OFF +
				SOC_PERIPH_OFF_AIOP_TILE +
				offsetof(struct aiop_tile_regs, cmgw_regs));

    abcr_lock = 0;
}

/******************************************************************************/
void cmgw_report_boot_status(uint32_t st)
{
    ASSERT_COND(cmgw_regs);

	iowrite32_ccsr(st, &(cmgw_regs->acgpr[CMGW_ACGPR_BOOT_STATUS]));
}

/******************************************************************************/
uint64_t cmgw_get_time_base()
{
	uint32_t tmr_base_h, tmr_base_l;
	uint32_t tmr_base_h_2;
	uint64_t tmr_base = 0;

    ASSERT_COND(cmgw_regs);

    tmr_base_h = ioread32_ccsr(&(cmgw_regs->mgpr[CMGW_ACGPR_TIME_BASE_HIGH]));
    tmr_base_l = ioread32_ccsr(&(cmgw_regs->mgpr[CMGW_ACGPR_TIME_BASE_LOW]));
    tmr_base_h_2 = ioread32_ccsr(&(cmgw_regs->mgpr[CMGW_ACGPR_TIME_BASE_HIGH]));

    if(tmr_base_h == TIME_BASE_IS_UPDATING)
	    return CMGW_TIME_BASE_NOT_VALID;
    if(tmr_base_h != tmr_base_h_2)
	    return CMGW_TIME_BASE_NOT_VALID;

    tmr_base = (uint64_t)(((uint64_t)tmr_base_h << 32) | (uint64_t)tmr_base_l);

	return tmr_base;
}

/******************************************************************************/
void cmgw_report_boot_failure(int err)
{
    ASSERT_COND(cmgw_regs);

	iowrite32_ccsr((uint32_t)err, &cmgw_regs->acgpr[CMGW_ACGPR_BOOT_FAIL]);
}

/******************************************************************************/
void cmgw_update_core_boot_completion()
{
    uint32_t abcr_val;

    uint32_t* abcr = &(cmgw_regs->abcr);

    /* Write AIOP boot status (ABCR) */
    lock_spinlock(&abcr_lock);
    abcr_val = ioread32_ccsr(abcr);
    abcr_val |= (uint32_t)(1 << core_get_id());
    iowrite32_ccsr(abcr_val, abcr);
    unlock_spinlock(&abcr_lock);
}

uint32_t cmgw_get_ntasks()
{
	return (ioread32_ccsr(&cmgw_regs->wscr) & CMGW_WSCR_NTASKS_MASK);
}

