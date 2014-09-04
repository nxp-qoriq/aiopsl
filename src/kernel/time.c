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



__SHRAM uint64_t time_epoch_to_midnight = 0; /*microseconds since epoch till midnight*/

__HOT_CODE int _gettime(uint64_t *time)
{
	uint32_t TEMP, TSCRU, TSCRL;
	uint64_t temp_val;
	struct aiop_tile_regs *aiop_regs = (struct aiop_tile_regs *)
		       		       sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
#ifdef DEBUG
	if(time == NULL)
		return -EACCES;
	if(aiop_regs == NULL)
		return -ENAVAIL;
#endif

	TEMP = ioread32_ccsr(&aiop_regs->cmgw_regs.tscru);
	TSCRL = ioread32_ccsr(&aiop_regs->cmgw_regs.tscrl);
	if ((TSCRU = ioread32_ccsr(&aiop_regs->cmgw_regs.tscru)) > TEMP )
		TSCRL = 0;
	else if(TSCRU < TEMP) /*something wrong while reading*/
		return -EACCES;

	TEMP = udiv1000(TSCRU);
	TSCRU = TSCRU-(1000*TEMP); // remainder
	temp_val = TEMP;
	temp_val <<= 32;
	TSCRU = (TSCRU << 16) | (TSCRL >> 16);
	TEMP = udiv1000(TSCRU);
	TSCRU = TSCRU - (1000 * TEMP); // remainder
	temp_val |= ((uint64_t) TEMP) << 16;
	TSCRU = (TSCRU << 16) | (TSCRL & 0xffff);
	temp_val |= udiv1000(TSCRU);

	*time = temp_val;
	return 0;
}

/*****************************************************************************/
__HOT_CODE int fsl_get_time_ms(uint32_t *time)
{
	uint64_t time_us;
	uint64_t local_epoch_to_midnight;
	int err;

	err = _gettime(&time_us);
	if(err < 0)
		return err;

	local_epoch_to_midnight = LLLDW(0, &time_epoch_to_midnight);
	time_us -= local_epoch_to_midnight;
	if(time_us < 86400000000) { /*86400000000 = 24 Hours in microseconds (24*60*60*1000*1000) */
		*time =(uint32_t) (ulldiv1000(time_us));
		return 0;
	}
	else {
		time_us += local_epoch_to_midnight;
		local_epoch_to_midnight = time_us - (time_us % 86400000000);
		*time = (uint32_t) (ulldiv1000(time_us - local_epoch_to_midnight));
		LLSTDW(local_epoch_to_midnight,0 , &time_epoch_to_midnight);
		return 0;
	}
}

/*****************************************************************************/
int fsl_get_time_since_epoch_ms(uint64_t *time)
{
	int err = _gettime(time);
	if(err < 0)
		return err;
	*time = ulldiv1000(*time);
	return 0;
}

/*****************************************************************************/
__HOT_CODE uint32_t fsl_os_current_time(void)
{
	return 0;
}


