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

#include "fsl_parser.h"
#include "fsl_net.h"
#include "fsl_sys.h"
#include "fsl_sl_evmng.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_dprc.h"
#include "fsl_sl_dbg.h"

int dprc_drv_test_init(void);

static int dprc_drv_test_evmng_cb(uint8_t generator_id, uint8_t event_id, uint64_t app_ctx, void *event_data)
{
	/*Container was updated*/
	int err;
	uint32_t status;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	UNUSED(app_ctx);
	UNUSED(event_data);

	if(event_id == DPRC_EVENT && generator_id == EVMNG_GENERATOR_AIOPSL){
		sl_pr_debug("DPRC objects changed event\n");

		err = dprc_get_irq_status(&dprc->io, 0, dprc->token,
		                          DPRC_IRQ_INDEX,
		                          &status);
		if(err){
			sl_pr_err("Get irq status for DPRC object change "
				"failed\n");
			return -ENAVAIL;
		}

		if(status & (DPRC_IRQ_EVENT_OBJ_ADDED |
			DPRC_IRQ_EVENT_OBJ_REMOVED |
			DPRC_IRQ_EVENT_OBJ_CREATED |
			DPRC_IRQ_EVENT_OBJ_DESTROYED)){
			err = dprc_clear_irq_status(&dprc->io, 0, dprc->token,
			                            DPRC_IRQ_INDEX,
			                            DPRC_IRQ_EVENT_OBJ_ADDED |
			                            DPRC_IRQ_EVENT_OBJ_REMOVED |
			                            DPRC_IRQ_EVENT_OBJ_CREATED |
			                            DPRC_IRQ_EVENT_OBJ_DESTROYED);
			if(err){
				sl_pr_err("Clear status for DPRC object "
					"change failed\n");
				return err;
			}
			err = dprc_drv_scan();
			if(err){
				sl_pr_err("Failed to scan dp object, %d.\n", err);
				return err;
			}
		}
	}
	else{
		sl_pr_debug("Event %d is not supported\n",event_id);
		return -EINVAL;
	}

	return 0;
}

int dprc_drv_test_init(void)
{
	int err;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	fsl_print("Running AIOP dprc_drv_test_init()\n");
	fsl_print("Update IRQ mask to support OBJ CREATED & DESTROYED to simulate Add/Remove event\n");

	err = dprc_set_irq_mask(&dprc->io, 0, dprc->token, 0,
	                        DPRC_IRQ_EVENT_OBJ_ADDED |
	                        DPRC_IRQ_EVENT_OBJ_REMOVED |
	                        DPRC_IRQ_EVENT_OBJ_CREATED |
	                        DPRC_IRQ_EVENT_OBJ_DESTROYED);
	if(err){
		pr_err("Set irq mask for DPRC object change failed\n");
		return -ENAVAIL;
	}
	err = dprc_clear_irq_status(&dprc->io, 0, dprc->token,
	                            DPRC_IRQ_INDEX,
	                            DPRC_IRQ_EVENT_OBJ_ADDED |
	                            DPRC_IRQ_EVENT_OBJ_REMOVED |
	                            DPRC_IRQ_EVENT_OBJ_CREATED |
	                            DPRC_IRQ_EVENT_OBJ_DESTROYED);
	if(err){
		pr_err("Set irq mask for DPRC object change failed\n");
		return -ENAVAIL;
	}
	err = evmng_irq_register(EVMNG_GENERATOR_AIOPSL,
	                         DPRC_EVENT, 0, 0, dprc_drv_test_evmng_cb);
	if(err){
			pr_err("EVM registration for DPRC object change failed\n");
			return -ENAVAIL;
	}
	return 0;
}



