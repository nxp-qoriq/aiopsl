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

#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "general.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "fsl_dprc.h"
#include "fsl_dpci.h"
#include "fsl_mc_init.h"
#include "ls2085_aiop/fsl_platform.h"
#include "cmdif_srv.h"

extern struct aiop_init_info g_init_data;

#define MC_DPCI_NUM 1
#define DPCI_DYNAMIC_MAX 32

int mc_obj_init();
void mc_obj_free();

extern int dpci_amq_bdi_init(uint32_t dpci_id);
extern int dpci_rx_ctx_init(uint32_t dpci_id, uint32_t id);

__COLD_CODE static int aiop_container_init()
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
	if ((err = dprc_get_container_id(&dprc->io, &container_id)) != 0) {
		pr_err("Failed to get AIOP root container ID.\n");
		return err;
	}
	if ((err = dprc_open(&dprc->io, container_id, &dprc->token)) != 0) {
		pr_err("Failed to open AIOP root container DP-RC%d.\n",
		container_id);
		return err;
	}

	err = sys_add_handle(dprc, FSL_OS_MOD_AIOP_RC, 1, 0);
	return err;
}

__COLD_CODE static void aiop_container_free()
{
	void *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	sys_remove_handle(FSL_OS_MOD_AIOP_RC, 0);

	if (dprc != NULL)
		fsl_free(dprc);
}

__COLD_CODE static int dpci_tbl_create(struct mc_dpci_tbl **_dpci_tbl, int dpci_count)
{
	uint32_t size = 0;
	struct   mc_dpci_tbl *dpci_tbl = NULL;
	int      err = 0;

	size = sizeof(struct mc_dpci_tbl);
	dpci_tbl = fsl_malloc(size, 1);
	*_dpci_tbl = dpci_tbl;
	if (dpci_tbl == NULL) {
		pr_err("No memory for %d DPCIs\n", dpci_count);
		return -ENOMEM;
	}
	memset(dpci_tbl, 0, size);

	size = sizeof(uint32_t) * dpci_count;
	dpci_tbl->dpci_id = fsl_malloc(size,1);
	if (dpci_tbl->dpci_id == NULL) {
		pr_err("No memory for %d DPCIs\n", dpci_count);
		return -ENOMEM;
	}
	memset(dpci_tbl->dpci_id, 0xff, size);

	size = sizeof(uint32_t) * dpci_count;
	dpci_tbl->ic = fsl_malloc(size,1);
	if (dpci_tbl->ic == NULL) {
		pr_err("No memory for %d DPCIs\n", dpci_count);
		return -ENOMEM;
	}
	memset(dpci_tbl->ic, 0xff, size);

	dpci_tbl->count = 0;
	dpci_tbl->lock = 0;
	dpci_tbl->max = dpci_count;
	
	err = sys_add_handle(dpci_tbl,
			FSL_OS_MOD_DPCI_TBL,
			1,
			0);
	if (err != 0) {
		pr_err("FSL_OS_MOD_DPCI_TBL sys_add_handle failed\n");
		return err;
	}

	return err;
}

__COLD_CODE static int dpci_tbl_add(struct dprc_obj_desc *dev_desc)
{
	int      err = 0;

	if (dev_desc == NULL)
		return -EINVAL;

	pr_debug(" Found DPCI device\n");
	pr_debug("***********\n");
	pr_debug("vendor - %x\n", dev_desc->vendor);
	pr_debug("type - %s\n", dev_desc->type);
	pr_debug("id - %d\n", dev_desc->id);
	pr_debug("region_count - %d\n", dev_desc->region_count);
	pr_debug("state - %d\n", dev_desc->state);
	pr_debug("ver_major - %d\n", dev_desc->ver_major);
	pr_debug("ver_minor - %d\n", dev_desc->ver_minor);
	pr_debug("irq_count - %d\n\n", dev_desc->irq_count);

	err = dpci_amq_bdi_init((uint32_t)dev_desc->id);
	if (err >= 0) {
		err = dpci_rx_ctx_init((uint32_t)dev_desc->id, (uint32_t)err);
	}

	return err;
}

__COLD_CODE static int dpci_for_mc_add(struct mc_dprc *dprc)
{
	struct dpci_cfg dpci_cfg;
	uint16_t dpci = 0;
	struct dpci_rx_queue_cfg queue_cfg;
	struct dprc_endpoint endpoint1 ;
	struct dprc_endpoint endpoint2;
	struct dpci_attr attr;
	uint8_t p = 0;
	int     err = 0;
	int     link_up = 0;

	memset(&queue_cfg, 0, sizeof(struct dpci_rx_queue_cfg));
	memset(&attr, 0, sizeof(attr));

	dpci_cfg.num_of_priorities = 2;

	err |= dpci_create(&dprc->io, &dpci_cfg, &dpci);

	/* Get attributes just for dpci id fqids are not there yet */
	err |= dpci_get_attributes(&dprc->io, dpci, &attr);
	
	/* Set priorities 0 and 1
	 * 0 is high priority
	 * 1 is low priority
	 * Making sure that low priority is at index 0*/
	queue_cfg.options = CMDIF_Q_OPTIONS;
	queue_cfg.dest_cfg.dest_type = DPCI_DEST_NONE;
	err = dpci_amq_bdi_init((uint32_t)attr.id);
	if (err >= 0) {
		/* Set index to DPCI table */
		queue_cfg.user_ctx = 0;
		CMDIF_DPCI_FQID(USER_CTX_SET, err, DPCI_FQID_NOT_VALID);
	}
	for (p = 0; p < dpci_cfg.num_of_priorities; p++) {
		queue_cfg.dest_cfg.priority = DPCI_LOW_PR - p;
		err |= dpci_set_rx_queue(&dprc->io, dpci, p, &queue_cfg);
	}
	ASSERT_COND(!err);
	
	/* Connect to dpci that belongs to MC */
	pr_debug("MC dpci ID[%d] \n", g_init_data.sl_info.mc_dpci_id);

	memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
	memset(&endpoint2, 0, sizeof(struct dprc_endpoint));
	endpoint1.id = (int)g_init_data.sl_info.mc_dpci_id;
	endpoint1.interface_id = 0;
	strcpy(endpoint1.type, "dpci");

	endpoint2.id = attr.id;
	endpoint2.interface_id = 0;
	strcpy(endpoint2.type, "dpci");

	err = dprc_connect(&dprc->io, dprc->token, &endpoint1, &endpoint2);
	if (err) {
		pr_err("dprc_connect failed\n");
	}

	err = dpci_enable(&dprc->io, dpci);
	if (err) {
		pr_err("dpci_enable failed\n");
	}

	err = dpci_get_link_state(&dprc->io, dpci, &link_up);
	if (!link_up) {
		pr_debug("MC DPCI[%d]<->AIOP DPCI[%d] link is down \n",
		endpoint1.id,
		endpoint2.id);
		/* Don't return error maybe it will be linked in the future */
	} else {
		pr_debug("MC DPCI[%d]<->AIOP DPCI[%d] link is up \n",
		endpoint1.id,
		endpoint2.id);
	}

	err = dpci_close(&dprc->io, dpci);
	return err;
}

__COLD_CODE static int dpci_tbl_fill(struct mc_dprc *dprc,
				int dpci_count, int dev_count)
{
	int ind = 0;
	int i   = 0;
	int err = 0;
	struct dprc_obj_desc dev_desc;


	while ((i < dev_count) && (ind < dpci_count)) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {
			err = dpci_tbl_add(&dev_desc);
			if (err) {
				pr_err("Failed dpci_tbl_add \n");
				return -ENODEV;
			}
			ind++;
		}
		i++;
	}

	err = dpci_for_mc_add(dprc);
	if (err) {
		pr_err("Failed to create and link AIOP<->MC DPCI \n");
	}
	
	return err;
}

__COLD_CODE static int dpci_discovery()
{
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct mc_dpci_tbl *dpci_tbl = NULL;
	int dev_count  = 0;
	int dpci_count = 0;
	int err        = 0;
	int i          = 0;

	if (dprc == NULL) {
		pr_err("No AIOP root container \n");
		return -ENODEV;
	}

	if ((err = dprc_get_obj_count(&dprc->io, dprc->token, &dev_count)) != 0) {
		pr_err("Failed to get device count for RC auth_d = %d\n",
		       dprc->token);
		return err;
	}

	/* First count how many DPCI objects we have */
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {
			dpci_count++;
		}
	}

	err = dpci_tbl_create(&dpci_tbl, 
	                      dpci_count + MC_DPCI_NUM + DPCI_DYNAMIC_MAX);
	if (err != 0) {
		pr_err("Failed dpci_tbl_create() \n");
		return err;
	}

	err = dpci_tbl_fill(dprc, dpci_count, dev_count);
	return err;
}

__COLD_CODE static void dpci_discovery_free()
{
	void *dpci_tbl = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	sys_remove_handle(FSL_OS_MOD_DPCI_TBL, 0);

	if (dpci_tbl != NULL)
		fsl_free(dpci_tbl);
}

__COLD_CODE int mc_obj_init()
{
	int err = 0;

	err |= aiop_container_init();
	err |= dpci_discovery(); /* must be after aiop_container_init */
	return err;

}

__COLD_CODE void mc_obj_free()
{
	aiop_container_free();
	dpci_discovery_free();
	/* TODO DPCI close ???
	 * TODO DPRC close */
}


int mc_dpci_find(uint32_t dpci_id, uint32_t *ic)
{
	struct mc_dpci_tbl *dpci_tbl = (struct mc_dpci_tbl *)\
		sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);
	int i;
	
	for (i = 0; i < dpci_tbl->count; i++) {
		if ((dpci_tbl->dpci_id[i] == dpci_id) || 
			dpci_tbl->dpci_id_peer[i] == dpci_id) {
			if (ic != NULL)
				*ic = dpci_tbl->ic[i];
			return i;
		}
	}
	
	return -ENOENT;
}
