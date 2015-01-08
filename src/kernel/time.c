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

#include "time.h"
#include "aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_sys.h"
#include "fsl_dbg.h"
#include "fsl_spinlock.h"


uint64_t time_epoch_to_midnight_ms __attribute__((aligned(8))) = 0; /*microseconds since epoch till midnight
* This global variable should be double word aligned to support load as atomic command.
*/
uint8_t time_to_midnight_lock = 0;
_time_get_t *time_get_func_ptr;   /*initialized in time_init*/
struct aiop_cmgw_regs *time_cmgw_regs;

extern struct aiop_init_info g_init_data;

int time_init(void);
void time_free(void);

int _get_time_fast(uint64_t *time)
{
	uint32_t TEMP, TSCRU, TSCRL;
	uint64_t temp_val;
	register struct aiop_cmgw_regs *time_regs_cmgw = time_cmgw_regs;
#ifdef DEBUG
	if(time == NULL)
		return -EACCES;
	if(time_regs_cmgw == NULL)
		return -ENAVAIL;
#endif

	TEMP = ioread32_ccsr(&time_regs_cmgw->tscru);
	TSCRL = ioread32_ccsr(&time_regs_cmgw->tscrl);
	if ((TSCRU = ioread32_ccsr(&time_regs_cmgw->tscru)) > TEMP )
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

	/*TODO: if time period set to different value then 1000 in MC, devision should be made*/
	*time = temp_val;
	return 0;
}

/*****************************************************************************/
int fsl_get_time_ms(uint32_t *time)
{
	uint64_t time_ms;
	uint64_t local_epoch_to_midnight;
	int err;

	err = (time_get_func_ptr)(&time_ms);
	if(err < 0)
		return err;

	local_epoch_to_midnight = LLLDW(0, &time_epoch_to_midnight_ms);
	time_ms -= local_epoch_to_midnight;
	if(time_ms < 86400000) { /*86400000000 = 24 Hours in microseconds (24*60*60*1000*1000) */
		*time =(uint32_t) (time_ms);
		return 0;
	}
	else {
		time_ms += local_epoch_to_midnight;
		local_epoch_to_midnight = time_ms - (time_ms % 86400000);
		*time = (uint32_t) (time_ms - local_epoch_to_midnight);
		lock_spinlock(&time_to_midnight_lock);
		LLSTDW(local_epoch_to_midnight,0 , &time_epoch_to_midnight_ms);
		unlock_spinlock(&time_to_midnight_lock);
		return 0;
	}
}

/*****************************************************************************/
int fsl_get_time_since_epoch_ms(uint64_t *time)
{
	return (time_get_func_ptr)(time);
}

/*****************************************************************************/
uint32_t fsl_os_current_time(void)
{
	return 0;
}

/*****************************************************************************/
__COLD_CODE int time_init(void)
{
	if(g_init_data.sl_info.clock_period == 1000)
	{
		struct aiop_tile_regs *aiop_regs = (struct aiop_tile_regs *)
				       		       sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
		time_cmgw_regs = (struct aiop_cmgw_regs*) &aiop_regs->cmgw_regs;

		time_get_func_ptr = _get_time_fast;
		return 0;
	}
	else
	{
		time_get_func_ptr = NULL;
		pr_err("Only fast path supported for time period 1000, time period: %d\n",g_init_data.sl_info.clock_period);
		return -ENOTSUP;
	}
}

/*****************************************************************************/
void time_free(void)
{

}



