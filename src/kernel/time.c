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

#include "time.h"
#include "aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_sys.h"

__HOT_CODE int _gettime(uint64_t *time)
{
	uint32_t TSCRU1, TSCRU2, TSCRL;
	uint64_t temp_val;
	struct aiop_tile_regs *aiop_regs = (struct aiop_tile_regs *)
		       		       sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
#ifdef DEBUG
	if(time == NULL)
		return -EACCES;
	if(aiop_regs == NULL)
		return -ENAVAIL;
#endif

	TSCRU1 = ioread32_ccsr(&aiop_regs->cmgw_regs.tscru);
	TSCRL = ioread32_ccsr(&aiop_regs->cmgw_regs.tscrl);
	if ((TSCRU2=ioread32_ccsr(&aiop_regs->cmgw_regs.tscru)) > TSCRU1 )
		TSCRL = 0;
	else if(TSCRU2 < TSCRU1) /*something wrong while reading*/
		return -EACCES;

	temp_val = (uint64_t)(TSCRU2) << 32;
	temp_val |= (TSCRL); /*temp_val holds time in nanoseconds*/
	*time = (temp_val / 1000); /*convert to microseconds*/
	 return 0;
}
