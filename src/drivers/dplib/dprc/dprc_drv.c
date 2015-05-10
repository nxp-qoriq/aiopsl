/* Copyright 2013-2015 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
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

#include "dprc_drv.h"
#include "fsl_dprc.h"
#include "fsl_dbg.h"
#include "evm.h"
#include "fsl_sys.h"
#include "fsl_mc_init.h"


int dprc_drv_init(void);
void dprc_drv_free(void);

static int dprc_drv_ev_cb(uint8_t id, uint16_t cmd, uint32_t size, void *data)
{
	/*Container was updated*/

	return 0;
}

int dprc_drv_init(void)
{
	int err;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	err = dprc_drv_scan();
	if(err){
		pr_err("Failed to scan resource container, %d.\n", err);
		return err;
	}

	err = dprc_set_irq(&dprc->io, dprc->token, DPRC_IRQ_EVENT_OBJ_ADDED,
	                   AIOP_DPRC, AIOP_DPRC_ASSIGN, 0);
	if(err){
		pr_err("Set irq for new object added failed\n");
		return -ENAVAIL;
	}

	err = evm_sl_register(AIOP_DPRC, AIOP_DPRC_ASSIGN, 1, dprc_drv_ev_cb);
	if(err){
		pr_err("EVM registration for DPRC assign failed\n");
		return -ENAVAIL;
	}

	err = dprc_set_irq(&dprc->io, dprc->token, DPRC_IRQ_EVENT_OBJ_REMOVED,
	                   AIOP_DPRC, AIOP_DPRC_UNASSIGN, 0);
	if(err){
		pr_err("Set irq for new object added failed\n");
		return -ENAVAIL;
	}

	err = evm_sl_register(AIOP_DPRC, AIOP_DPRC_UNASSIGN, 1, dprc_drv_ev_cb);
	if(err){
		pr_err("EVM registration for DPRC un-assign failed\n");
		return -ENAVAIL;
	}
	return 0;
}


void dprc_drv_free(void)
{

}



int dprc_drv_scan(void)
{
	int i, err, dev_count = 0;
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);


	if ((err = dprc_get_obj_count(&dprc->io, dprc->token,
	                              &dev_count)) != 0) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}

	for(i = 0; i < dev_count; i++){
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
	}



	return 0;
}



