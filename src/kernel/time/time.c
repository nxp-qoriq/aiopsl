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
#include "fsl_aiop_common.h"
#include "fsl_sys.h"
#include "fsl_dbg.h"
#include "fsl_spinlock.h"
#include "fsl_cmgw.h"
#include "tman_inline.h"

uint64_t time_epoch_to_midnight_ms __attribute__((aligned(8))) = 0; /*microseconds since epoch till midnight
* This global variable should be double word aligned to support load as atomic command.
*/
uint8_t time_to_midnight_lock = 0;

int time_init(void);
void time_free(void);

/*****************************************************************************/
int _get_time_tman(uint64_t *time)
{
	uint64_t time_base = 0;

	time_base = cmgw_get_time_base(); /* Timebase is in mSec */

	if(time_base == CMGW_TIME_BASE_NOT_VALID)
		return -EACCES; /* Something wrong while reading */

	if(time_base == 0)
		return -ENODEV; /* Time-base is not yet configured by MC */

	tman_get_timestamp(time);
	*time = ulldiv1000(*time); /* Convert Tman value to mSec */

	*time += time_base;

	return 0;
}

/*****************************************************************************/
int fsl_get_time_ms(uint32_t *time)
{
	uint64_t time_ms;
	uint64_t local_epoch_to_midnight;
	int err;

	err = _get_time_tman(&time_ms);
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
	return _get_time_tman(time);
}

/*****************************************************************************/
__COLD_CODE int time_init(void)
{
	return 0;
}

/*****************************************************************************/
void time_free(void)
{

}



