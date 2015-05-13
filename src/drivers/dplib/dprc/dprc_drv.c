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
extern int evm_raise_event_cb(void *dev, uint16_t cmd, uint32_t size, void *data);

static int dprc_drv_ev_cb(uint8_t generator_id, uint8_t event_id, uint32_t size, void *data)
{
	/*Container was updated*/
	int err;
	UNUSED(generator_id);
	UNUSED(size);
	UNUSED(data);
	if(event_id == DPRC_EVENT_OBJ_ADDED){
		sl_pr_debug("DPRC objects added event\n");
		err = dprc_drv_add_obj();
		if(err){
			sl_pr_err("Failed to add dp object, %d.\n", err);
			return err;
		}
	}
	else if(event_id == DPRC_EVENT_OBJ_REMOVED){
		sl_pr_debug("DPRC object removed event\n");
		err = dprc_drv_remove_obj();
		if(err){
			sl_pr_err("Failed to remove dp object, %d.\n", err);
			return err;
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



int dprc_drv_add_obj(void)
{
	int i, aiop_niid, err, dev_count = 0;
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	err = dprc_get_obj_count(&dprc->io, dprc->token, &dev_count);
	if (err) {
		sl_pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}

	for(i = 0, aiop_niid = 0; i < dev_count; i++){
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpni") == 0) {

			if(dpni_drv_is_dpni_exist((uint16_t)dev_desc.id) == -1){
			/*object not found in the table and should be added*/
				err = dpni_drv_probe(dprc,
				                     (uint16_t)dev_desc.id,
				                     (uint16_t)aiop_niid);
				if (err) {
					sl_pr_err("Failed to probe DPNI-%d.\n",
					          dev_desc.id);
					return err;
				}
				/*send event: "DPNI_ADDED_EVENT" to EVM */
				err = evm_raise_event_cb((void *)DPNI,
				                         DPNI_EVENT_ADDED,
				                         sizeof(dev_desc.id),
				                         &dev_desc.id);
				if(err){
					sl_pr_err("Failed to raise event for "
						"DPNI-%d.\n", dev_desc.id);
					return err;
				}
			}
			aiop_niid++;
		}
		else if(strcmp(dev_desc.type, "dpci") == 0) {

			if(dpci_mng_find((uint32_t)dev_desc.id) < 0){
				/*object not found in the
						table and should be added*/
				err = dpci_event_assign((uint32_t)dev_desc.id);
				if(err){
					sl_pr_err("Failed to add DPCI-%d.\n", i);
					return err;
				}
				/*send event: "DPNI_ADDED_EVENT" to EVM */
				err = evm_raise_event_cb((void *)DPCI,
				                         DPCI_EVENT_ADDED,
				                         sizeof(dev_desc.id),
				                         &dev_desc.id);
				if(err){
					sl_pr_err("Failed to raise event for "
						"DPCI-%d.\n", dev_desc.id);
					return err;
				}
			}
		}
	}
	return 0;
}

int dprc_drv_remove_obj(void)
{
	int i, index, err, dev_count = 0;
	struct dprc_obj_desc dev_desc;
	/*each bit represent the object index in internal table which is in use*/
	uint64_t valid_dpnis = 0;
	uint64_t valid_dpcis = 0;

	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	dpni_drv_valid_dpnis(&valid_dpnis);
/*	dpni_drv_valid_dpcis(&valid_dpcis);*/

	err = dprc_get_obj_count(&dprc->io, dprc->token, &dev_count);
	if (err){
		sl_pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}

	for(i = 0; i < dev_count; i++){
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpni") == 0){
			index = dpni_drv_is_dpni_exist((uint16_t)dev_desc.id);
			/* check if index valid and if this dpni was found
			 * before exist*/
			if((index >= 0) &&
				(valid_dpnis & (uint64_t)(1 << index))){
				valid_dpnis ^= (uint64_t)(1 << index);
			}
		}
		else if(strcmp(dev_desc.type, "dpci") == 0){
			index = dpci_mng_find((uint32_t)dev_desc.id);
			/* check if index valid and if this dpni was found
			 * before exist*/
			if((index >= 0) &&
				(valid_dpcis & (uint64_t)(1 << index))){
				valid_dpcis ^= (uint64_t)(1 << index);
			}
		}
	}
	for(i = 0; i < 64; i++, valid_dpcis = valid_dpcis >> 1,
				valid_dpnis = valid_dpnis >> 1){
		if(valid_dpnis & 0x01){
			/*dpni_drv_unprobe (should return the mc_niid)*/
			/*send event: "DPNI_REMOVED_EVENT" to EVM with mc_niid */
		}
		if(valid_dpcis & 0x01){
			/*dpci_drv_unprobe (should return the dpci id)*/
			/*send event: "DPCI_REMOVED_EVENT" to EVM with dpci id */
		}
	}
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

	err = dprc_set_irq(&dprc->io, dprc->token, DPRC_IRQ_EVENT_OBJ_ADDED,
	                   DPRC, DPRC_EVENT_OBJ_ADDED, 0);
	if(err){
		pr_err("Set irq for DPRC object added failed\n");
		return -ENAVAIL;
	}

	err = evm_sl_register(DPRC, DPRC_EVENT_OBJ_ADDED, 0, dprc_drv_ev_cb);
	if(err){
		pr_err("EVM registration for DPRC object added failed\n");
		return -ENAVAIL;
	}

	err = dprc_set_irq(&dprc->io, dprc->token, DPRC_IRQ_EVENT_OBJ_REMOVED,
	                   DPRC, DPRC_EVENT_OBJ_REMOVED, 0);
	if(err){
		pr_err("Set irq for DPRC object removed failed\n");
		return -ENAVAIL;
	}

	err = evm_sl_register(DPRC, DPRC_EVENT_OBJ_REMOVED, 0, dprc_drv_ev_cb);
	if(err){
		pr_err("EVM registration for DPRC object removed failed\n");
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


