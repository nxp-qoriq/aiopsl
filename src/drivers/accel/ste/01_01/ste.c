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

/**************************************************************************//**
@File		ste.c

@Description	This file contains the AIOP SW Statistics API implementation.
				(common)
*//***************************************************************************/

#include "general.h"
#include "fsl_ste.h"

void ste_barrier()
{
	uint32_t cmd_type = (uint32_t)STE_CMDTYPE_SYNC;
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;

	/* A command to the TST to set the scheduling inhibit for the task */
	__e_osmcmd(STE_OSM_REQ_TYPE, 0);
	/* call STE sync command. */
	__stqw(cmd_type, 0, 0, 0, 0, (uint32_t *)mem_ptr);
	/* YIELD. */
	__e_hwacceli(YIELD_ACCEL_ID);
}

void ste_inc_counter_wrp(uint64_t counter_addr, uint32_t inc_value,
			 uint32_t flags)
{
	ste_inc_counter(counter_addr, inc_value, flags);
}

void ste_dec_counter_wrp(uint64_t counter_addr, uint32_t inc_value,
			 uint32_t flags)
{
	ste_dec_counter(counter_addr, inc_value, flags);
}
