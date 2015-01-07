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

/***************************************************************************//**

@File          cmgw.c

@Description   Command Gateway
 *//***************************************************************************/

#include "common/types.h"
#include "aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_spinlock.h"
#include "fsl_core.h"
#include "cmgw.h"

static struct aiop_cmgw_regs * cmgw_regs;

uint8_t abcr_lock = 0;

/******************************************************************************/
void cmgw_init(void * cmgw_regs_base) 
{
    ASSERT_COND(cmgw_regs_base);
	
    cmgw_regs = cmgw_regs_base;
    abcr_lock = 0;
}

/******************************************************************************/
void cmgw_report_boot_failure()
{   
    ASSERT_COND(cmgw_regs);

	iowrite32_ccsr(0x1, &(cmgw_regs->acgpr[CMGW_ACGPR_BOOT_FAIL]));
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

