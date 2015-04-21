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
#include "fsl_dpci_drv.h"
#include "fsl_spinlock.h"

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
	dpci_tbl->dpci_id_peer = fsl_malloc(size,1);
	if (dpci_tbl->dpci_id_peer == NULL) {
		pr_err("No memory for %d DPCIs\n", dpci_count);
		return -ENOMEM;
	}
	memset(dpci_tbl->dpci_id_peer, 0xff, size);

	size = sizeof(uint32_t) * dpci_count;
	dpci_tbl->ic = fsl_malloc(size,1);
	if (dpci_tbl->ic == NULL) {
		pr_err("No memory for %d DPCIs\n", dpci_count);
		return -ENOMEM;
	}
	memset(dpci_tbl->ic, 0xff, size);

	dpci_tbl->count = 0;
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

#ifdef  ARENA_LEGACY_CODE
	err = dpci_amq_bdi_init((uint32_t)dev_desc->id);
	if (err >= 0) {
		err = dpci_rx_ctx_init((uint32_t)dev_desc->id, (uint32_t)err);
	}
#endif
	err = dpci_drv_added((uint32_t)dev_desc->id);

	return err;
}

__COLD_CODE void mc_dpci_tbl_dump()
{
	int i;
	struct mc_dpci_tbl *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	ASSERT_COND(dt);

	fsl_os_print("----------DPCI table----------\n");
	for (i = 0; i < dt->count; i++) {
		fsl_os_print("ID = 0x%x\t PEER ID = 0x%x\t IC = 0x%x\t\n",
		             dt->dpci_id[i], dt->dpci_id_peer[i], dt->ic[i]);
	}
}

int mc_dpci_find(uint32_t dpci_id, uint32_t *ic)
{
	int i;
	struct mc_dpci_tbl *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	ASSERT_COND(dt);
	ASSERT_COND(dpci_id != DPCI_FQID_NOT_VALID);
	
	for (i = 0; i < dt->count; i++) {
		if (dt->dpci_id[i] == dpci_id) {
			if (ic != NULL)
				*ic = dt->ic[i];
			return i;
		}
	}

	return -ENOENT;
}

int mc_dpci_peer_find(uint32_t dpci_id, uint32_t *ic)
{
	int i;
	struct mc_dpci_tbl *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	ASSERT_COND(dt);
	ASSERT_COND(dpci_id != DPCI_FQID_NOT_VALID);
	
	for (i = 0; i < dt->count; i++) {
		if (dt->dpci_id_peer[i] == dpci_id) {
			if (ic != NULL)
				*ic = dt->ic[i];
			return i;
		}
	}

	return -ENOENT;
}

int mc_dpci_entry_get()
{
	int i;
	struct mc_dpci_tbl *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	ASSERT_COND(dt);

	for (i = 0; i < dt->count; i++)
		if (dt->dpci_id[i] == DPCI_FQID_NOT_VALID)
			return i;

	if (dt->count < dt->max) {
		i = dt->count;
		atomic_incr32(&dt->count, 1);
		return i;
	}
	
	return -ENOENT;
}

void mc_dpci_entry_delete(int ind)
{
	struct mc_dpci_tbl *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	ASSERT_COND(dt);

	dt->ic[ind] = DPCI_FQID_NOT_VALID;
	dt->dpci_id[ind] = DPCI_FQID_NOT_VALID;
	dt->dpci_id_peer[ind] = DPCI_FQID_NOT_VALID;
	atomic_decr32(&dt->count, 1);
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
	err = dpci_close(&dprc->io, dpci);
	ASSERT_COND(!err);

#ifdef  ARENA_LEGACY_CODE
	err = dpci_amq_bdi_init((uint32_t)attr.id);
	if (err >= 0) {
		err = dpci_rx_ctx_init((uint32_t)attr.id, (uint32_t)err);
	}
#endif
	err = dpci_drv_added((uint32_t)attr.id);

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

	mc_dpci_tbl_dump();

	return err;
}

__COLD_CODE static void dpci_discovery_free()
{
	void *dpci_tbl = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	sys_remove_handle(FSL_OS_MOD_DPCI_TBL, 0);

	if (dpci_tbl != NULL)
		fsl_free(dpci_tbl);
	/*
	 * TODO free all entries inside dpci_tbl
	 */
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
