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

#include "fsl_types.h"
#include "fsl_stdio.h"
#include "fsl_general.h"
#include "fsl_dbg.h"
#include "fsl_doorbell.h"
#include "fsl_aiop_common.h"

int app_early_init(void);
int app_init(void);
void app_free(void);

__HOT_CODE ENTRY_POINT static void doorbell_cb(void)
{
	pr_debug(" doorbell_cb \n");
	
	doorbell_clear(0, DOORBELL_SRC_GENERAL, 0xffffffff);
	doorbell_clear(0, DOORBELL_SRC_MANAGEMENT, 0xffffffff);
	doorbell_clear(1, DOORBELL_SRC_GENERAL, 0xffffffff);
	doorbell_clear(1, DOORBELL_SRC_MANAGEMENT, 0xffffffff);
}

int app_init(void)
{
	int i;
	int pr;
	
	pr_info("Running app_init()\n");

	for (i = 0; i < DOORBELL_SRC_LAST; i++) {
		for (pr = 0; pr < 2; pr++) {
			doorbell_setup(pr,
			               (enum doorbell_reg)i, 
			               (uint16_t)(AIOP_EPID_TABLE_SIZE - 1 - pr),
			               doorbell_cb,
			               (uint32_t)(pr + 1));
			doorbell_ring(pr, 
			              (enum doorbell_reg)i,
			              (uint32_t)(pr + 1));
		}
	}
	
	return 0;
}

int app_early_init(void)
{
	/* Early initialization */
	return 0;
}

void app_free(void)
{
	/* free application resources*/
}
