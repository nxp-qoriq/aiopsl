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
#include "fsl_fdma.h"
#include "fsl_spinlock.h"
#include "fsl_icontext.h"

int app_early_init(void);
int app_init(void);
void app_free(void);

static int32_t doorbell_cb_count = 0;
static int32_t doorbell_clear_count = 0;

static uint16_t epid[] = {AIOP_EPID_TABLE_SIZE - 1, /* G Priority 0*/
                          AIOP_EPID_TABLE_SIZE - 2, /* G Priority 1*/
                          AIOP_EPID_TABLE_SIZE - 3, /* M Priority 0*/
                          AIOP_EPID_TABLE_SIZE - 4};/* M Priority 1*/

__HOT_CODE ENTRY_POINT static void doorbell_cb(void)
{
	uint32_t mask;
	struct fdma_amq amq;
	struct icontext ic;
	uint8_t src = ((((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fdsrc_va_fca_bdi) & 0xf);
	int i;
	int pr;

	get_default_amq_attributes(&amq);
	icontext_aiop_get(&ic);
	ASSERT_COND(ic.icid == amq.icid);

	for (i = 0; i < DOORBELL_SRC_LAST; i++) {
		for (pr = 0; pr < 2; pr++) {
			doorbell_status(pr, (enum doorbell_reg)i, &mask);
			pr_debug("Read status pr %d g_m %d mask 0x%x\n", pr, i, mask);
			mask = (0x1 << pr); /* WA for sim bug */
			//if (mask != 0) {
				doorbell_clear(pr, (enum doorbell_reg)i, mask);
				pr_debug("Cleared pr %d g_m %d mask 0x%x\n", pr, i, mask);
				atomic_incr32(&doorbell_clear_count, 1);
				doorbell_status(pr, (enum doorbell_reg)i, &mask);
				ASSERT_COND(mask == 0);
			//}
		}
	}
	
	atomic_incr32(&doorbell_cb_count, 1);
	pr_debug(" doorbell_cb %d src 0x%x\n", doorbell_cb_count, src);
	
	if ((doorbell_cb_count == 4) && (doorbell_clear_count >= 4)) {
		fsl_print("Test Finished SUCCESSFULLY\n");
	} /*else {
		fsl_print("Check if  can ring after clear \n");
		doorbell_ring(0, DOORBELL_SRC_GENERAL, 0x1);
		doorbell_ring(1, DOORBELL_SRC_GENERAL, 0x2);
	}*/

	fdma_terminate_task();
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
			               epid[i*2 + pr],
			               doorbell_cb,
			               (uint32_t)(pr + 1));
			doorbell_ring(pr, 
			              (enum doorbell_reg)i,
			              (uint32_t)(0x1 << pr));
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
