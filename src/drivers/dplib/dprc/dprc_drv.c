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
#include "evmng.h"
#include "fsl_sys.h"
#include "fsl_malloc.h"
#include "dpni_drv.h"
#include "drv.h"
#include "aiop_common.h"
#include "fsl_dpci_mng.h"
#include "fsl_dpci_event.h"

extern struct aiop_init_info g_init_data;

int dprc_drv_init(void);
void dprc_drv_free(void);
static int aiop_container_init(void);
static void aiop_container_free(void);

static int dprc_drv_evmng_cb(uint8_t generator_id, uint8_t event_id, uint64_t app_ctx, void *event_data)
{
	/*Container was updated*/
	int err;
	uint32_t status;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	UNUSED(app_ctx);
	UNUSED(event_data);

	if(event_id == DPRC_EVENT && generator_id == EVMNG_GENERATOR_AIOPSL){
		sl_pr_debug("DPRC objects changed event\n");

		err = dprc_get_irq_status(&dprc->io, dprc->token,
		                          DPRC_NUM_OF_IRQS,
		                          &status);
		if(err){
			sl_pr_err("Get irq status for DPRC object change "
				"failed\n");
			return -ENAVAIL;
		}

		if(status & (DPRC_IRQ_EVENT_OBJ_ADDED |
			DPRC_IRQ_EVENT_OBJ_REMOVED)){
			err = dprc_drv_scan();
			if(err){
				sl_pr_err("Failed to scan dp object, %d.\n", err);
				return err;
			}
			err = dprc_clear_irq_status(&dprc->io, dprc->token,
			                            DPRC_NUM_OF_IRQS,
			                            DPRC_IRQ_EVENT_OBJ_ADDED |
			                            DPRC_IRQ_EVENT_OBJ_REMOVED);
			if(err){
				sl_pr_err("Clear status for DPRC object "
					"change failed\n");
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

int dprc_drv_init(void)
{
	int err;
	err = aiop_container_init();
	if(err){
		pr_err("Failed to initialize AIOP container, %d.\n", err);
		return err;
	}

	return 0;
}


void dprc_drv_free(void)
{
	aiop_container_free();
	/*TODO what else should be freed*/
}

int dprc_drv_scan(void)
{
	int i, err, dev_count = 0;
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	err = dprc_get_obj_count(&dprc->io, dprc->token, &dev_count);
	if (err) {
		sl_pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		          dprc->token);
		return err;
	}
	sl_pr_debug("Num of objects found: %d\n",dev_count);
	for(i = 0; i < dev_count; i++){
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpni") == 0) {
			sl_pr_debug("DPNI %d found in the container\n",dev_desc.id);

			err = dpni_drv_update_obj(dprc,
			                          (uint16_t)dev_desc.id);
			if (err) {
				sl_pr_err("Failed to update DPNI %d.\n",
				          dev_desc.id);
				return err;
			}

		}
		else if(strcmp(dev_desc.type, "dpci") == 0) {

		}
	}
	dpni_drv_sync(dprc);
	return 0;

}

__COLD_CODE static int aiop_container_init(void)
{
	void *p_vaddr;
	int err = 0;
	int container_id;
	/*struct mc_dprc *dprc = fsl_os_xmalloc(sizeof(struct mc_dprc),
					MEM_PART_SH_RAM,
					1);*/
	struct mc_dprc *dprc = fsl_malloc(sizeof(struct mc_dprc),
	                                  1);
	if (dprc == NULL) {
		pr_err("No memory for AIOP Root Container \n");
		return -ENOMEM;
	}
	memset(dprc, 0, sizeof(struct mc_dprc));

	/* TODO : in this call, can 3rd argument be zero? */
	/* Get virtual address of MC portal */
	p_vaddr = UINT_TO_PTR(((uintptr_t)sys_get_handle(FSL_OS_MOD_MC_PORTAL, 0))
	                      + SOC_PERIPH_OFF_PORTALS_MC(g_init_data.sl_info.mc_portal_id));

	pr_debug("MC portal ID[%d] addr = 0x%x\n", g_init_data.sl_info.mc_portal_id, (uint32_t)p_vaddr);

	/* Open root container in order to create and query for devices */
	dprc->io.regs = p_vaddr;
	err = dprc_get_container_id(&dprc->io, &container_id);
	if(err){
		pr_err("Failed to get AIOP root container ID.\n");
		return err;
	}
	err = dprc_open(&dprc->io, container_id, &dprc->token);
	if(err){
		pr_err("Failed to open AIOP root container DP-RC%d.\n",
		       container_id);
		return err;
	}

	err = dprc_set_irq(&dprc->io, dprc->token, 0,
	                   DPRC_EVENT,(uint32_t) container_id, 0);
	if(err){
		pr_err("Set irq for DPRC object change failed\n");
		return -ENAVAIL;
	}
	err = dprc_set_irq_mask(&dprc->io, dprc->token, 0,
	                        DPRC_IRQ_EVENT_OBJ_ADDED |
	                        DPRC_IRQ_EVENT_OBJ_REMOVED);
	if(err){
		pr_err("Set irq mask for DPRC object change failed\n");
		return -ENAVAIL;
	}

	err = evmng_irq_register(EVMNG_GENERATOR_AIOPSL,
	                         DPRC_EVENT, 0, 0, dprc_drv_evmng_cb);
	if(err){
		pr_err("EVM registration for DPRC object change failed\n");
		return -ENAVAIL;
	}

	err = dprc_set_irq_enable(&dprc->io, dprc->token, 0, 1);
	if(err){
		pr_err("Set irq enable for DPRC object change failed\n");
		return -ENAVAIL;
	}

	err = sys_add_handle(dprc, FSL_OS_MOD_AIOP_RC, 1, 0);
	return err;
}

__COLD_CODE static void aiop_container_free(void)
{
	void *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	sys_remove_handle(FSL_OS_MOD_AIOP_RC, 0);

	if (dprc != NULL)
		fsl_free(dprc);
}


