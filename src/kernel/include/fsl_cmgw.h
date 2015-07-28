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

/**************************************************************************//**
@File          cmgw.h

@Description   Command Gateway
*//***************************************************************************/
#ifndef __CMGW_H
#define __CMGW_H

#include "fsl_types.h"
#include "fsl_errors.h"

#define CMGW_ACGPR_BOOT_FAIL 0
#define CMGW_ACGPR_BOOT_STATUS 1
#define CMGW_ACGPR_AIOP_COMPLETION_STATUS 2

#define CMGW_ACGPR_TIME_BASE_LOW  0
#define CMGW_ACGPR_TIME_BASE_HIGH 1
#define CMGW_TIME_BASE_NOT_VALID (0xffffffffffffffffULL)

#define CMGW_WSCR_NTASKS_MASK 0x0000000F

void cmgw_init(void * cmgw_regs_base);
void cmgw_report_boot_status(uint32_t st);

void cmgw_report_boot_failure();
void cmgw_update_core_boot_completion();
uint32_t cmgw_get_ntasks();
uint64_t cmgw_get_time_base();


/** @} */ /* end of group */
#endif /* __CMGW_H */

