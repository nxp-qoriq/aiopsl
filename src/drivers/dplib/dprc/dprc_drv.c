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

#include "fsl_sl_dprc_drv.h"
#include "fsl_dprc.h"
#include "fsl_dbg.h"
#include "fsl_sl_evmng.h"
#include "fsl_sys.h"
#include "fsl_malloc.h"
#include "fsl_sl_dpni_drv.h"
#include "drv.h"
#include "fsl_aiop_common.h"
#include "fsl_dpci_mng.h"
#include "fsl_dpci_event.h"
#include "fsl_dpcon.h"
#include "fsl_dpbp.h"

extern struct aiop_init_info g_init_data;

extern int g_dpcon_id;
extern uint8_t g_dpcon_priorities;

int dprc_drv_init(void);
void dprc_drv_free(void);
static int aiop_container_init(void);
static void aiop_container_free(void);
static int aiop_container_empty_dpbp(void);

static int dprc_drv_evmng_cb(uint8_t generator_id, uint8_t event_id, uint64_t app_ctx, void *event_data)
{
	/*Container was updated*/
	int err;
	uint32_t status = 0;
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
			DPRC_IRQ_EVENT_OBJ_REMOVED)){
			err = dprc_clear_irq_status(&dprc->io, 0, dprc->token,
			                            DPRC_IRQ_INDEX,
			                            DPRC_IRQ_EVENT_OBJ_ADDED |
			                            DPRC_IRQ_EVENT_OBJ_REMOVED);
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

int dprc_drv_init(void)
{
	int err;

	err = aiop_container_init();
	if (err) {
		pr_err("Failed to initialize AIOP container, %d.\n", err);
		return err;
	}
	err = aiop_container_empty_dpbp();
	if (err)
		return err;
	return 0;
}


void dprc_drv_free(void)
{
	aiop_container_free();
	/*TODO what else should be freed*/
}

__COLD_CODE static uint8_t get_dpcon_priorities(struct mc_dprc *dprc, int id)
{
	int err;
	uint16_t dpcon;
	struct dpcon_attr attr;

	err = dpcon_open(&dprc->io, 0, id, &dpcon);
	if (err) {
		sl_pr_err("Open DPCON failed\n");
		return 0;
	}
	err = dpcon_get_attributes(&dprc->io, 0, dpcon, &attr);
	if (err) {
		sl_pr_err("dpcon_get_attributes failed\n");
		dpni_close(&dprc->io, 0, dpcon);
		return 0;
	}
	err = dpcon_close(&dprc->io, 0, dpcon);
	if (err) {
		sl_pr_err("Close DPCON failed\n");
		return 0;
	}
	return attr.num_priorities;
}

int dprc_drv_scan(void)
{
	int i, err, dev_count = 0;
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);

	err = dprc_get_obj_count(&dprc->io, 0, dprc->token, &dev_count);
	if (err) {
		sl_pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		          dprc->token);
		return err;
	}
	sl_pr_debug("Num of objects found: %d\n",dev_count);

	/* Search first the dpcon object if any since we need to know
	  the dpcon id and the number of priorities before configuring dpni queues*/
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, 0, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpcon") == 0) {
			g_dpcon_id = dev_desc.id;
			g_dpcon_priorities =
				get_dpcon_priorities(dprc, g_dpcon_id);
			break;
		}
	}
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, 0, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpni") == 0) {
			sl_pr_debug("DPNI %d found in the container\n",dev_desc.id);

			err = dpni_drv_update_obj(dprc,
			                          (uint16_t)dev_desc.id);
			if (err) {
				sl_pr_err("Failed to update DPNI %d.\n",
				          dev_desc.id);
				return err;
			}

		} else if (strcmp(dev_desc.type, "dpci") == 0) {
			err = dpci_event_update_obj((uint32_t)dev_desc.id);
			if (err) {
				sl_pr_err("Failed to update DPCI %d.\n",
				          dev_desc.id);
				return err;
			}

		}
	}
	dpni_drv_handle_removed_objects();
	dpci_event_handle_removed_objects();
	return 0;

}

__COLD_CODE static int aiop_container_init(void)
{
	void *p_vaddr;
	int err = 0;
	int container_id = -1;
	struct dprc_irq_cfg irq_cfg;
	/*struct mc_dprc *dprc = fsl_xmalloc(sizeof(struct mc_dprc),
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
	p_vaddr = UINT_TO_PTR(((uintptr_t)sys_get_handle(FSL_MOD_MC_PORTAL, 0))
	                      + SOC_PERIPH_OFF_PORTALS_MC(g_init_data.sl_info.mc_portal_id));

	pr_debug("MC portal ID[%d] addr = 0x%x\n", g_init_data.sl_info.mc_portal_id, (uint32_t)p_vaddr);

	/* Open root container in order to create and query for devices */
	dprc->io.regs = p_vaddr;
	err = dprc_get_container_id(&dprc->io, 0, &container_id);
	if (err || (container_id == -1)) {
		pr_err("Failed to get AIOP container ID err = %d\n", err);
		return err;
	}
	err = dprc_open(&dprc->io, 0, container_id, &dprc->token);
	if(err){
		pr_err("Failed to open AIOP root container DP-RC%d.\n",
		       container_id);
		return err;
	}

	irq_cfg.addr = DPRC_EVENT;
	irq_cfg.val = (uint32_t) container_id;
	irq_cfg.irq_num = 0;

	err = dprc_set_irq(&dprc->io, 0, dprc->token, DPRC_IRQ_INDEX, &irq_cfg);
	if(err){
		pr_err("Set irq for DPRC object change failed\n");
		return -ENAVAIL;
	}
	err = dprc_set_irq_mask(&dprc->io, 0, dprc->token, DPRC_IRQ_INDEX,
	                        DPRC_IRQ_EVENT_OBJ_ADDED |
	                        DPRC_IRQ_EVENT_OBJ_REMOVED);
	if(err){
		pr_err("Set irq mask for DPRC object change failed\n");
		return -ENAVAIL;
	}

	err = dprc_clear_irq_status(&dprc->io, 0, dprc->token,
	                            DPRC_IRQ_INDEX,
	                            DPRC_IRQ_EVENT_OBJ_ADDED |
	                            DPRC_IRQ_EVENT_OBJ_REMOVED);
	if(err){
		pr_err("Set irq mask for DPRC object change failed\n");
		return -ENAVAIL;
	}

	err = evmng_irq_register(EVMNG_GENERATOR_AIOPSL,
	                         DPRC_EVENT, 1, 0, dprc_drv_evmng_cb);
	if(err){
		pr_err("EVM registration for DPRC object change failed\n");
		return -ENAVAIL;
	}

	err = dprc_set_irq_enable(&dprc->io, 0, dprc->token, 0, 1);
	if(err){
		pr_err("Set irq enable for DPRC object change failed\n");
		return -ENAVAIL;
	}

	err = sys_add_handle(dprc, FSL_MOD_AIOP_RC, 1, 0);
	return err;
}

__COLD_CODE static void aiop_container_free(void)
{
	void *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);

	sys_remove_handle(FSL_MOD_AIOP_RC, 0);

	if (dprc != NULL)
		fsl_free(dprc);
}

static __COLD_CODE int aiop_container_empty_dpbp(void)
{
	struct mc_dprc		*dprc;
	int			i, dev_count, err;
	uint16_t		dpbp;
	struct dprc_obj_desc	dev_desc;
	struct dpbp_attr	attr;
	uint64_t		dst;
#ifdef SL_DEBUG
	uint32_t		num_free_bufs, buffer_cnt;
#endif

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc)
		return -ENODEV;
	err = dprc_get_obj_count(&dprc->io, 0, dprc->token, &dev_count);
	if (err) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}
	dpbp = 0;
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, 0, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp"))
			continue;
		pr_info("Found dpbp@%d\n", dev_desc.id);
		err = dpbp_open(&dprc->io, 0, dev_desc.id, &dpbp);
		if (err) {
			pr_err("Failed to open dpbp@%d\n", dev_desc.id);
			return err;
		}
		err = dpbp_get_attributes(&dprc->io, 0, dpbp, &attr);
		if (err) {
			pr_err("Failed to get attributes of dpbp@%d\n",
			       dev_desc.id);
			/* Close DPBP control session */
			if (dpbp_close(&dprc->io, 0, dpbp))
				pr_err("Close dpbp@%d\n", dev_desc.id);
			return err;
		}
#ifdef SL_DEBUG
		buffer_cnt = 0;
		/* Get number of buffers in pool */
		err = dpbp_get_num_free_bufs(&dprc->io, 0, dpbp,
					     &num_free_bufs);
		if (err) {
			pr_err("Get number of free buffers for DPBP@%d.\n",
			       dev_desc.id);
			/* Close DPBP control session */
			if (dpbp_close(&dprc->io, 0, dpbp))
				pr_err("Close dpbp@%d\n", dev_desc.id);
			return err;
		}
		pr_info("\t BPID #%d has %d buffers\n", attr.bpid,
			num_free_bufs);
#endif
		do {
			err = fdma_acquire_buffer
			(icontext_aiop.icid,
			 icontext_aiop.bdi_flags & FDMA_ENF_BDI_BIT,
			 attr.bpid, &dst);
#ifdef SL_DEBUG
			if (!err) {
				/* Uncomment the line bellow to print the
				   acquired buffers */
				/*pr_info("\t\t addr = 0x%x-0x%08x\n",
					(uint32_t)(dst >> 32), (uint32_t)dst);*/
				buffer_cnt++;
			}
#endif
		} while (!err);
		/* Close DPBP control session */
		err = dpbp_close(&dprc->io, 0, dpbp);
		if (err) {
			pr_err("Close DPBP@%d.\n", dev_desc.id);
			return err;
		}
#ifdef SL_DEBUG
		pr_info("\t BPID #%d Acquired %d buffers\n", attr.bpid,
			buffer_cnt);
#endif
	}
	return 0;
}


