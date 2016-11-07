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
#include "fsl_sl_dbg.h"
#include "common/fsl_string.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_malloc.h"
#include "fsl_io_ccsr.h"
#include "fsl_dpni.h"
#include "fsl_fdma.h"
#include "fsl_parser.h"
#include "fsl_platform.h"
#include "fsl_sys.h"
#include "fsl_dprc.h"
#include "fsl_dpbp.h"
#include "fsl_bman.h"
#include "fsl_malloc.h"
#include "dpni_drv.h"
#include "fsl_aiop_common.h"
#include "system.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_sl_slab.h"
#include "fsl_sl_evmng.h"
#include "fsl_ep_mng.h"

#define __ERR_MODULE__  MODULE_DPNI
#define ETH_BROADCAST_ADDR		((uint8_t []){0xff,0xff,0xff,0xff,0xff,0xff})
int dpni_drv_init(void);
void dpni_drv_free(void);

extern struct platform_app_params g_app_params;

/*
 * struct for pool params used for storage profile
 * This struct is set during dpni_drv_init and can not change in runtime
 */
struct dpni_pools_cfg pools_params;

/*buffer used for dpni_drv_set_order_scope*/
uint8_t order_scope_buffer[PARAMS_IOVA_BUFF_SIZE];

struct dpni_drv *nis;
int num_of_nis;

struct dpni_drv_stats {
	uint8_t page;
	uint8_t offset;
};

struct dpni_drv_stats dpni_statistics[17] = {
		{ 0, 0 }, { 0, 1 }, { 2, 0 }, { 2, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 },
		{ 0, 5 }, { 1, 0 }, { 1, 1 }, { 2, 3 }, { 1, 2 }, { 1, 3 }, { 1, 4 },
		{ 1, 5 }, { 2, 2 }, { 2, 4 } };

struct dpni_early_init_request g_dpni_early_init_data = {0};

static int dpni_drv_evmng_cb(uint8_t generator_id, uint8_t event_id, uint64_t size, void *event_data);

__COLD_CODE static void print_dev_desc(struct dprc_obj_desc* dev_desc)
{
	pr_debug(" device %d\n", dev_desc->id);
	pr_debug("***********\n");
	pr_debug("vendor - %x\n", dev_desc->vendor);

	if (strcmp(dev_desc->type, "dpni") == 0)
	{
		pr_debug("type - DP_DEV_DPNI\n");
	}
	else if (strcmp(dev_desc->type, "dprc") == 0)
	{
		pr_debug("type - DP_DEV_DPRC\n");
	}
	else if (strcmp(dev_desc->type, "dpio") == 0)
	{
		pr_debug("type - DP_DEV_DPIO\n");
	}
	pr_debug("id - %d\n", dev_desc->id);
	pr_debug("region_count - %d\n", dev_desc->region_count);
	pr_debug("ver_major - %d\n", dev_desc->ver_major);
	pr_debug("ver_minor - %d\n", dev_desc->ver_minor);
	pr_debug("irq_count - %d\n\n", dev_desc->irq_count);

}

void discard_rx_cb(void)
{

	pr_debug("Packet discarded by discard_rx_cb.\n");
	/*Discard frame and terminate task*/
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}

static void discard_rx_app_cb(void)
{
	pr_debug("Packet discarded by discard_rx_app_cb.\n");
	/*Discard frame and terminate task*/
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}

int dpni_drv_register_rx_cb (uint16_t ni_id, rx_cb_t *cb)
{
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	iowrite32_ccsr((uint32_t)(nis[ni_id].dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	iowrite32_ccsr(PTR_TO_UINT(cb), &wrks_addr->ep_pc);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	EP_MNG_MUTEX_RELEASE;
	return 0;
}

int dpni_drv_unregister_rx_cb (uint16_t ni_id)
{
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	iowrite32_ccsr((uint32_t)(nis[ni_id].dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	EP_MNG_MUTEX_RELEASE;
	return 0;
}

int dpni_drv_enable (uint16_t ni_id)
{
	uint16_t dpni;
	int err;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_enable(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("dpni_enable failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_disable (uint16_t ni_id)
{
	uint16_t dpni;
	int err;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_disable(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("dpni_disable failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}
/* This function called from dpni_drv_update_obj and protected by mutex write */
int dpni_drv_is_dpni_exist(uint16_t mc_niid)
{
	int i;
	for(i = 0; i < SOC_MAX_NUM_OF_DPNI; i++){
		if(nis[i].dpni_id == mc_niid){
			break;
		}
	}
	if(i == SOC_MAX_NUM_OF_DPNI)
		return -1;
	else
		return i;
}

int dpni_drv_update_obj(struct mc_dprc *dprc, uint16_t mc_niid)
{
	int err;
	uint16_t aiop_niid;
	int index;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK);
	index = dpni_drv_is_dpni_exist(mc_niid);
	if(index == -1){
		sl_pr_debug("DPNI %d not exist nis table\n",mc_niid);
		err = dpni_open(&dprc->io, 0, mc_niid, &dpni);
		if(err){
			cdma_mutex_lock_release((uint64_t)nis);
			sl_pr_err("Failed to open DP-NI before reset%d\n.", mc_niid);
			/* if open failed, no need to close*/
			return err;
		}

		err = dpni_reset(&dprc->io, 0, dpni);
		if(err){
			cdma_mutex_lock_release((uint64_t)nis);
			sl_pr_err("Failed to reset DP-NI%d\n.", mc_niid);
			dpni_close(&dprc->io, 0, dpni);
			return err;
		}

		err = dpni_close(&dprc->io, 0, dpni);
		if(err){
			cdma_mutex_lock_release((uint64_t)nis);
			sl_pr_err("Failed to close DP-NI after reset%d\n.", mc_niid);
			/* if open failed, no need to close*/
			return err;
		}

		err = dpni_drv_probe(dprc, mc_niid, &aiop_niid);
		if(err){
			cdma_mutex_lock_release((uint64_t)nis);
			sl_pr_err("DP-NI %d was not probed, err: %d.\n",
			          mc_niid,
			          aiop_niid);
			return err;
		}
		nis[aiop_niid].dpni_drv_params_var.flags |= DPNI_DRV_FLG_SCANNED;
		cdma_mutex_lock_release((uint64_t)nis);
		/*send event: "DPNI_ADDED_EVENT" to EVM with
		 * AIOP NI ID */
		evmng_sl_raise_event(
			EVMNG_GENERATOR_AIOPSL,
			DPNI_EVENT_ADDED,
			(void *)aiop_niid);
	}
	else{
		/*update that this index scanned*/
		nis[index].dpni_drv_params_var.flags |= DPNI_DRV_FLG_SCANNED;
		cdma_mutex_lock_release((uint64_t)nis);
	}
	return 0;
}

void dpni_drv_handle_removed_objects(void)
{
	uint16_t aiop_niid;

	for(aiop_niid = 0; aiop_niid < SOC_MAX_NUM_OF_DPNI; aiop_niid++)
	{
		cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK);
		if(nis[aiop_niid].dpni_drv_params_var.flags &
			DPNI_DRV_FLG_SCANNED){
			nis[aiop_niid].dpni_drv_params_var.flags &= ~DPNI_DRV_FLG_SCANNED;
			cdma_mutex_lock_release((uint64_t)nis);
		}
		else if(nis[aiop_niid].dpni_id != DPNI_NOT_IN_USE){
			dpni_drv_unprobe(aiop_niid);
			/*send event: "DPNI_REMOVED_EVENT" to EVM with
			 * AIOP NI ID */
			cdma_mutex_lock_release((uint64_t)nis);
			sl_pr_debug("DPNI with NI %d removed from nis table\n",aiop_niid);
			evmng_sl_raise_event(
				EVMNG_GENERATOR_AIOPSL,
				DPNI_EVENT_REMOVED,
				(void *)aiop_niid);
		}
		else{
			cdma_mutex_lock_release((uint64_t)nis);
		}
	}
	return;
}


void dpni_drv_unprobe(uint16_t aiop_niid)
{
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
						sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;


	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	iowrite32_ccsr((uint32_t)nis[aiop_niid].dpni_drv_params_var.epid_idx,
	               &wrks_addr->epas);
	/*clear ep_pc*/
	iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	/*Mutex unlock EPID table*/
	EP_MNG_MUTEX_RELEASE;
	nis[aiop_niid].dpni_id = DPNI_NOT_IN_USE;

	nis[aiop_niid].dpni_drv_params_var.spid         = 0;
	nis[aiop_niid].dpni_drv_params_var.spid_ddr     = 0;
	nis[aiop_niid].dpni_drv_params_var.epid_idx     = 0;
	/*parser profile id from parser_profile_init()*/
	/*
	TODO: prpid should be updated dynamically in dpni_drv_probe
	nis[aiop_niid].dpni_drv_params_var.prpid        = prpid;
	 */
	/*ETH HXS */
	nis[aiop_niid].dpni_drv_params_var.starting_hxs = 0;
	nis[aiop_niid].dpni_drv_tx_params_var.qdid      = 0;
	nis[aiop_niid].dpni_drv_params_var.flags        =
		DPNI_DRV_FLG_PARSE | DPNI_DRV_FLG_PARSER_DIS;

}

/* configure probed dpni's parameters (attributes, pools, layout, TX confirmation, SPID info*/
static int configure_dpni_params(struct mc_dprc *dprc, uint16_t aiop_niid, uint16_t dpni)
{
	struct dpni_buffer_layout layout = {0};
	struct dpni_sp_info sp_info = { 0 };
	struct dpni_attr attributes = { 0 };
	int err;

	err = dpni_get_attributes(&dprc->io,
	                          0,
	                          dpni,
	                          &attributes);
	if(err){
		sl_pr_err("Failed to get attributes\n");
		return err;
	}

	/* TODO: set nis[aiop_niid].starting_hxs according to
	 * the DPNI attributes.
	 * Not yet implemented on MC.
	 * Currently always set to zero, which means ETH. */
	err = dpni_set_pools(
		&dprc->io,
		0,
		dpni,
		&pools_params);
	if(err){
		sl_pr_err("Failed to set the pools\n");
		return err;
	}

	layout.options = DPNI_BUF_LAYOUT_OPT_DATA_HEAD_ROOM
		| DPNI_BUF_LAYOUT_OPT_DATA_TAIL_ROOM;

	if(g_dpni_early_init_data.count > 0) {
		layout.data_head_room =
			g_dpni_early_init_data.head_room_sum;
		layout.data_tail_room =
			g_dpni_early_init_data.tail_room_sum;
		layout.private_data_size =
			g_dpni_early_init_data.private_data_size_sum;
	}else {
		layout.data_head_room = DPNI_DRV_DHR_DEF;
		layout.data_tail_room = DPNI_DRV_DTR_DEF;
		layout.private_data_size = DPNI_DRV_PTA_DEF;
	}

#ifdef ERR009354
	layout.options |= DPNI_BUF_LAYOUT_OPT_DATA_ALIGN;
	layout.data_align = DPNI_DRV_DATA_ALIGN_DEF;
	sl_pr_debug("ERR009354: data-offset must be aligned to 256\n");
#endif  /* ERR009354 */

	err = dpni_set_buffer_layout(&dprc->io, 0, dpni, DPNI_QUEUE_RX, &layout);
	if(err){
		sl_pr_err("Failed to set rx buffer layout\n");
		return err;
	}

	/*
	 * Disable TX confirmation for DPNI's in AIOP in case
	 * the option: 'DPNI_OPT_TX_CONF_DISABLED' was not
	 * selected at DPNI creation.
	 * */
	err = dpni_set_tx_confirmation_mode(&dprc->io, 0, dpni, DPNI_CONF_DISABLE);
	if(err){
		sl_pr_err("Failed to set tx_conf_revoke\n");
		return err;
	}

	/* Now a Storage Profile exists and is associated
	 * with the NI */


	/* Register SPID in internal AIOP NI table */
	if ((err = dpni_get_sp_info(&dprc->io, 0,
	                            dpni, &sp_info)) != 0) {
		sl_pr_err("Failed to get SPID\n");
		return err;
	}

	/*TODO: change to uint16_t in nis table
	 * for the next release*/
	nis[aiop_niid].dpni_drv_params_var.spid =
		(uint8_t)sp_info.spids[0];

	/* TODO: need to initialize additional NI table fields according to DPNI attributes */

	/*bpid exist to use for ddr pool*/
	if(pools_params.num_dpbp == 2){
		nis[aiop_niid].dpni_drv_params_var.spid_ddr =
			(uint8_t)sp_info.spids[1];
	}
	else{
		sl_pr_err("DDR spid is not available \n");
		nis[aiop_niid].dpni_drv_params_var.spid_ddr = 0;
	}
	return err;
}

/* configure probed dpni's irq */
static int configure_dpni_irq(struct mc_dprc *dprc, uint16_t mc_niid, uint16_t dpni)
{
	struct dpni_irq_cfg irq_cfg = { 0 };
	int err;

	irq_cfg.addr = (uint64_t)DPNI_EVENT;
	irq_cfg.val = (uint32_t)mc_niid;
	irq_cfg.irq_num = 0;

	sl_pr_debug("Register for irq with addr %d and val %d\n", (int)irq_cfg.addr, (int)irq_cfg.val);


	err = dpni_set_irq(&dprc->io, 0, dpni,
	                   DPNI_IRQ_INDEX, &irq_cfg);
	if(err){
		sl_pr_err("Failed to set irq\n");
		return err;
	}

	err = dpni_set_irq_mask(&dprc->io, 0, dpni,
	                        DPNI_IRQ_INDEX,
	                        DPNI_IRQ_EVENT_LINK_CHANGED);
	if(err){
		sl_pr_err("Failed to set irq mask\n");
		return err;
	}

	err = dpni_clear_irq_status(&dprc->io,0, dpni,
	                            DPNI_IRQ_INDEX,
	                            DPNI_IRQ_EVENT_LINK_CHANGED);
	if(err){
		sl_pr_err("Failed to clear IRQ status\n");
		return err;
	}

	err = dpni_set_irq_enable(&dprc->io, 0, dpni,
	                          DPNI_IRQ_INDEX, 1);
	if(err){
		sl_pr_err("Failed to set irq enable\n");
		return err;
	}
	return err;
}

/* initialize probed dpni */
static int initialize_dpni(struct mc_dprc *dprc, uint16_t mc_niid, uint16_t aiop_niid)
{
	int err;
	uint16_t dpni;
	uint16_t qdid;
	uint16_t flow_id = DPNI_NEW_FLOW_ID;
	uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	struct dpni_queue queue = {0};


	err = dpni_open(&dprc->io, 0, mc_niid, &dpni);
	if(err){
		sl_pr_err("Failed to open DP-NI%d\n.", mc_niid);
		/* if open failed, no need to close*/
		return err;
	}

	/* Register MAC address in internal AIOP NI table */
	err = dpni_get_primary_mac_addr(&dprc->io, 0,
	                                dpni,
	                                mac_addr);
	if(err){
		sl_pr_err("Failed to get MAC address for DP-NI%d\n",
		          mc_niid);
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	memcpy(nis[aiop_niid].mac_addr,
	       mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);


	/* Register QDID in internal AIOP NI table */
	err = dpni_get_qdid(&dprc->io, 0,
	                    dpni,DPNI_QUEUE_TX,&qdid);
	if(err){
		sl_pr_err("Failed to get QDID for DP-NI%d\n",
		          mc_niid);
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	nis[aiop_niid].dpni_drv_tx_params_var.qdid = qdid;

	err = dpni_set_queue(&dprc->io, 0, dpni,DPNI_QUEUE_TX,0,0,0, &queue);
	if(err){
		sl_pr_err("dpni_set_tx_flow failed for DP-NI%d\n",
		          mc_niid);
	}

	err = configure_dpni_params(dprc, aiop_niid, dpni);
	if(err){
		sl_pr_err("configure_dpni_params failed for DP-NI%d\n",
		          mc_niid);
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	if(configure_dpni_irq(dprc, mc_niid, dpni)){
		sl_pr_err("configure_dpni_irq failed for DP-NI%d\n",
		          mc_niid);
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Failed to close dpni for DP-NI%d\n",
		          mc_niid);
		/* No need to close twice*/
		return err;
	}
	return 0;
}

int dpni_drv_probe(struct mc_dprc *dprc,
                   uint16_t mc_niid,
                   uint16_t *niid)
{
	int i;
	uint32_t j;
	uint32_t ep_osc;
	uint16_t aiop_niid;
	int err = 0;

	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/* Check if dpni with same ID already exists and find first entry to use*/
	for (i = 0, aiop_niid = SOC_MAX_NUM_OF_DPNI;
		i < SOC_MAX_NUM_OF_DPNI; i++)
	{
		if(nis[i].dpni_id == DPNI_NOT_IN_USE &&
			aiop_niid == SOC_MAX_NUM_OF_DPNI){
			aiop_niid = (uint16_t)i;
		}
		if(nis[i].dpni_id == mc_niid)
		{
			sl_pr_err("DPNI %d already exist in nis[]\n", mc_niid);
			return -EEXIST;
		}
	}
	if(aiop_niid == SOC_MAX_NUM_OF_DPNI){
		sl_pr_err("NI's table is full\n");
		return -ENOSPC;
	}

	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	/* Search for NIID (mc_niid) in EPID table and prepare the NI for usage. */
	for (i = AIOP_EPID_DPNI_START; i < AIOP_EPID_TABLE_SIZE; i++) {
		/* Prepare to read from entry i in EPID table - EPAS reg */
		iowrite32_ccsr((uint32_t)i, &wrks_addr->epas);

		/* Read Entry Point Param (EP_PM) which contains the MC NI ID */
		j = ioread32_ccsr(&wrks_addr->ep_pm);

		sl_pr_debug("EPID[%d].EP_PM = 0x%x\n", i, j);

		/*MC dpni id found in EPID table*/
		if (j == (mc_niid | AIOP_EPID_SET_BY_DPNI)) {
			/* Replace MC NI ID with AIOP NI ID */
			sl_pr_debug("Found EPID[%d].EP_PM = %d\n", i, (j & ~(AIOP_EPID_SET_BY_DPNI)));
			iowrite32_ccsr(aiop_niid, &wrks_addr->ep_pm);

			ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
			ep_osc &= ORDER_MODE_CLEAR_BIT;
			/*Set concurrent mode for NI in epid table*/
			iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);

			/*Mutex unlock EPID table*/
			EP_MNG_MUTEX_RELEASE;
			/* Store epid index in AIOP NI's array*/
			nis[aiop_niid].dpni_drv_params_var.epid_idx =
				(uint16_t)i;
			err = initialize_dpni(dprc, mc_niid, aiop_niid);
			if(err){
				sl_pr_err("initialize dpni %d failed with error %d\n", (int)mc_niid, err);
				return err;
			}

			num_of_nis ++;

			nis[aiop_niid].dpni_id = mc_niid;
			/* Unlock nis table*/
			*niid = aiop_niid;
			return 0;
		}
	}


	if(i == AIOP_EPID_TABLE_SIZE){ /*MC dpni id not found in EPID table*/
		/*Unlock EPID table*/
		EP_MNG_MUTEX_RELEASE;
		sl_pr_err("DP-NI %d not found in EPID table.\n", mc_niid);
		err = -ENODEV;
	}

	return err;
}


int dpni_drv_get_spid(uint16_t ni_id, uint16_t *spid)
{
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	*spid = nis[ni_id].dpni_drv_params_var.spid;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/

	return 0;
}

int dpni_drv_get_spid_ddr(uint16_t ni_id, uint16_t *spid_ddr)
{
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	*spid_ddr = nis[ni_id].dpni_drv_params_var.spid_ddr;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	return 0;
}

int dpni_drv_get_num_of_nis (void)
{
	int nis_num;
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	nis_num = num_of_nis;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	return nis_num;
}


/* TODO: replace with macro/inline */
int dpni_drv_get_primary_mac_addr(uint16_t ni_id,
                                  uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	memcpy(mac_addr, nis[ni_id].mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	return 0;
}

int dpni_drv_set_primary_mac_addr(uint16_t ni_id,
                                  uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_set_primary_mac_addr(&dprc->io,0,  dpni, mac_addr);
	if(err){
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		sl_pr_err("dpni_set_primary_mac_addr failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	memcpy(nis[ni_id].mac_addr, mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);
	/*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_add_mac_addr(uint16_t ni_id,
                          const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_add_mac_addr(&dprc->io, 0, dpni, mac_addr);
	if(err){
		sl_pr_err("dpni_add_mac_addr failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_remove_mac_addr(uint16_t ni_id,
                             const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_remove_mac_addr(&dprc->io, 0, dpni, mac_addr);
	if(err){
		sl_pr_err("dpni_remove_mac_addr failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_max_frame_length(uint16_t ni_id,
                                  const uint16_t mfl)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_max_frame_length(&dprc->io,0,  dpni, mfl);
	if(err){
		sl_pr_err("dpni_set_max_frame_length failed\n");
		dpni_close(&dprc->io,0,  dpni);
		return err;
	}
	err = dpni_close(&dprc->io,0,  dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_max_frame_length(uint16_t ni_id,
                                  uint16_t *mfl)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_max_frame_length(&dprc->io, 0, dpni, mfl);
	if(err){
		sl_pr_err("dpni_get_max_frame_length failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

__COLD_CODE static int parser_profile_init(uint8_t *prpid)
{
	struct parse_profile_input parse_profile1 __attribute__((aligned(16)));

	/* Init basic parse profile */
	memset(&(parse_profile1.parse_profile), 0, sizeof(struct parse_profile_record));

	/* Frame Parsing advances to MPLS Default Next Parse (IP HXS) */
	parse_profile1.parse_profile.mpls_hxs_config.lie_dnp = PARSER_PRP_MPLS_HXS_CONFIG_LIE;

	/* Routing header is ignored and the destination address from
	 * main header is used instead */
	parse_profile1.parse_profile.ipv6_hxs_config = PARSER_PRP_IPV6_HXS_CONFIG_RHE;

	/* In short Packet, padding is removed from Checksum calculation */
	parse_profile1.parse_profile.tcp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	/* In short Packet, padding is removed from Checksum calculation */
	parse_profile1.parse_profile.udp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;

	return parser_profile_create(&(parse_profile1), prpid);
}

static int get_existing_ddr_memory(void)
{
	if(fsl_mem_exists(MEM_PART_DP_DDR)){
		return MEM_PART_DP_DDR;
	}
	else if(fsl_mem_exists(MEM_PART_SYSTEM_DDR)){
		return MEM_PART_SYSTEM_DDR;
	}
	else{
		return 0;
	}
}

static int get_valid_alignment(uint16_t *alignment, uint16_t buffer_size)
{
	if(IS_POWER_VALID_ALLIGN(g_app_params.dpni_drv_alignment,buffer_size))
	{
		*alignment = (uint16_t)g_app_params.dpni_drv_alignment;
		return 0;
	}
	else
	{
		pr_err("Given alignment is not valid (not power of 2 or <= buffer size)\n");
		return -EINVAL;
	}
}

/* Used to configure DPBP's for dpni's during dpni driver initialization */
static int configure_bpids_for_dpni(void)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int dev_count;
	int i, err;
	int num_bpids = 0;
	/* TODO: replace with memset */
	uint16_t dpbp = 0;
	struct dprc_obj_desc dev_desc;
	int dpbp_id[DPNI_DRV_NUM_USED_BPIDS];
	struct dpbp_attr attr;
	uint16_t buffer_size = (uint16_t)g_app_params.dpni_buff_size;
	uint16_t num_buffs = (uint16_t)g_app_params.dpni_num_buffs;
	uint8_t bkp_pool_disable = g_app_params.app_config_flags &
			DPNI_BACKUP_POOL_DISABLE ? 1 : 0;
	uint16_t alignment;
	uint8_t mem_pid[] = {DPNI_DRV_FAST_MEMORY, (uint8_t)get_existing_ddr_memory()};

	ASSERT_COND(ARRAY_SIZE(mem_pid) == DPNI_DRV_NUM_USED_BPIDS);

#ifdef ERR009354
	if(buffer_size & (DPNI_DRV_DATA_ALIGN_DEF - 1)){
		pr_err("ERR009354: data-offset must be aligned to 256\n");
		return -EINVAL;
	}
#endif /* ERR009354 */

	err = get_valid_alignment(&alignment, buffer_size);
	if(err){
		return err;
	}

	if (dprc == NULL)
	{
		return -ENODEV;
	}
	/* TODO: replace the following dpbp_open&init with dpbp_create when available */


	if ((err = dprc_get_obj_count(&dprc->io, 0, dprc->token,
	                              &dev_count)) != 0) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}

	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, 0, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			/* TODO: print conditionally based on log level */
			pr_info("Found DPBP ID: %d, will be used for frame buffers\n",dev_desc.id);
			dpbp_id[num_bpids]= dev_desc.id;
			num_bpids ++;

			if(num_bpids == DPNI_DRV_NUM_USED_BPIDS || bkp_pool_disable)
				break;
		}
	}

	if ((num_bpids != 1 && bkp_pool_disable) || (num_bpids != DPNI_DRV_NUM_USED_BPIDS && !bkp_pool_disable)){
		pr_err("Not enough DPBPs found in the container.\n");
		return -ENAVAIL;
	}

	for(i = 0; i < num_bpids; i++)
	{
		if ((err = dpbp_open(&dprc->io, 0, dpbp_id[i], &dpbp)) != 0) {
			pr_err("Failed to open DPBP-%d.\n", dpbp_id[i]);
			return err;
		}

		if ((err = dpbp_enable(&dprc->io, 0, dpbp)) != 0) {
			pr_err("Failed to enable DPBP-%d.\n", dpbp_id[i]);
			return err;
		}

		if ((err = dpbp_get_attributes(&dprc->io, 0, dpbp, &attr)) != 0) {
			pr_err("Failed to get attributes from DPBP-%d.\n", dpbp_id[i]);
			return err;
		}

		if ((err = bman_fill_bpid(num_buffs,
		                          buffer_size,
		                          alignment,
		                          (enum memory_partition_id) mem_pid[i],
		                          attr.bpid,
		                          0)) != 0) {
			pr_err("Failed to fill DPBP-%d (BPID=%d) with buffer size %d.\n",
			       dpbp_id[i], attr.bpid, buffer_size);
			return err;
		}

		/* Prepare parameters to attach to DPNI object */
		/* for AIOP, can be up to 2 */
		pools_params.num_dpbp ++;
		/*!< DPBPs object id */
		pools_params.pools[i].dpbp_id = (uint16_t)dpbp_id[i];
		pools_params.pools[i].buffer_size = buffer_size;
		if(mem_pid[i] == MEM_PART_SYSTEM_DDR || mem_pid[i] == MEM_PART_DP_DDR){
			pools_params.pools[i].backup_pool = 1;
		}
	}
	return 0;
}

static int check_if_drv_and_flib_structs_identical(void)
{
	struct dpni_drv_tx_selection dpni_drv_tx_sel;
	struct dpni_tx_priorities_cfg dpni_tx_sel;

	if((sizeof(dpni_tx_sel) !=
		sizeof(dpni_drv_tx_sel))
	|| ( sizeof(dpni_tx_sel.tc_sched[0].delta_bandwidth) !=
		sizeof(dpni_drv_tx_sel.tc_sched[0].delta_bandwidth))
	/* The enum values cant't change */
	|| ( sizeof(dpni_tx_sel.tc_sched[0].mode) !=
		sizeof(dpni_drv_tx_sel.tc_sched[0].mode))
	|| (DPNI_MAX_TC != DPNI_DRV_MAX_TC))
	{
		sl_pr_err("Structs for driver and flib are not identical\n");
		return -EINVAL;
	}
	return 0;
}

__COLD_CODE static inline void dpni_drv_init_ni_table(uint8_t prpid)
{
	int i;

	for (i = 0; i < SOC_MAX_NUM_OF_DPNI; i++) {
		struct dpni_drv * dpni_drv = nis + i;
		dpni_drv->dpni_id                          = DPNI_NOT_IN_USE;
		dpni_drv->dpni_drv_params_var.spid         = 0;
		dpni_drv->dpni_drv_params_var.spid_ddr     = 0;
		dpni_drv->dpni_drv_params_var.epid_idx     = 0;
		/*parser profile id from parser_profile_init()*/
		dpni_drv->dpni_drv_params_var.prpid        = prpid;
		/*ETH HXS */
		dpni_drv->dpni_drv_params_var.starting_hxs = 0;
		dpni_drv->dpni_drv_tx_params_var.qdid      = 0;
		dpni_drv->dpni_drv_params_var.flags        =
			DPNI_DRV_FLG_PARSE | DPNI_DRV_FLG_PARSER_DIS;
	}
}

__COLD_CODE int dpni_drv_init(void)
{
	uint8_t prpid = 0;
	int err = 0;

	memset(&pools_params, 0, sizeof(struct dpni_pools_cfg));
	num_of_nis = 0;
	/* Allocate internal AIOP NI table */
	nis =fsl_malloc(sizeof(struct dpni_drv)*SOC_MAX_NUM_OF_DPNI,64);
	if (!nis) {
		return -ENOMEM;
	}

	err = parser_profile_init(&prpid);
	if(err){
		pr_err("parser profile initialization failed %d\n", err);
		return err;
	}
	/* Initialize internal AIOP NI table */
	dpni_drv_init_ni_table(prpid);

	/* TODO - add initialization of global default DP-IO
	 * (i.e. call 'dpio_open', 'dpio_init');
	 * This should be mapped to ALL cores of AIOP and to ALL the tasks */
	/* TODO - add initialization of global default DP-SP
	 * (i.e. call 'dpsp_open', 'dpsp_init');
	 * This should be mapped to 3 buff-pools with sizes: 128B, 512B, 2KB;
	 * all should be placed in PEB. */
	/* TODO - need to scan the bus in order to retrieve the AIOP
	 * "Device list" */
	/* TODO - iterate through the device-list:
	 * call 'dpni_drv_probe(ni_id, mc_portal_id, dpio, dp-sp)' */

	err = configure_bpids_for_dpni();
	if(err){
		pr_err("configure_bpids_for_dpni failed %d\n",err);
		return err;
	}



	err = evmng_irq_register(EVMNG_GENERATOR_AIOPSL,
	                         DPNI_EVENT,
	                         0,
	                         0,
	                         dpni_drv_evmng_cb);
	if(err){
		pr_err("EVM registration for DPNI events failed %d\n",err);
		return -ENAVAIL;
	}
	else{
		pr_info("Registered to: dpni_drv_evmng_cb\n");
	}

#ifdef DEBUG
	err = check_if_drv_and_flib_structs_identical();
	if(err){
		pr_err("check_if_drv_and_flib_structs_identical %d\n",err);
		return -ENAVAIL;
	}
	else{
		pr_info("flib and driver structs are identical.\n");
	}
#endif
	return err;
}

/* Check irq status for received event from MC*/
static int dpni_drv_check_irq_status(struct mc_dprc *dprc, uint16_t dpni)
{
	int err;
	uint32_t status = 0;

	err = dpni_get_irq_status(&dprc->io,
	                          0,
	                          dpni,
	                          DPNI_IRQ_INDEX,
	                          &status);
	if(err){
		sl_pr_err("Get irq status failed\n");
		return err;
	}

	if(status & DPNI_IRQ_EVENT_LINK_CHANGED){
		err = dpni_clear_irq_status(&dprc->io, 0, dpni,
		                            DPNI_IRQ_INDEX,
		                            DPNI_IRQ_EVENT_LINK_CHANGED);
		if(err){
			sl_pr_err("Clear status for DPNI link "
				"change failed\n");
			return err;
		}
	}
	else{
		return -ENOTSUP;
	}
	return 0;
}

/* Raise internal event due to link status */
static void dpni_drv_raise_linkchange_event(int up, int ni_id)
{
	if(up){
		evmng_sl_raise_event(
			EVMNG_GENERATOR_AIOPSL,
			DPNI_EVENT_LINK_UP,
			(void *)ni_id);
	}
	else{
		evmng_sl_raise_event(
			EVMNG_GENERATOR_AIOPSL,
			DPNI_EVENT_LINK_DOWN,
			(void *)ni_id);
	}

}
/* Callback called by EVMNG when DPNI event received from MC*/
static int dpni_drv_evmng_cb(uint8_t generator_id, uint8_t event_id, uint64_t app_ctx, void *event_data)
{
	/*Container was updated*/
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_link_state link_state;
	int err;
	int ni_id;
	uint16_t dpni;

	UNUSED(app_ctx);
	/*TODO SIZE should be cooperated with Ehud*/

	if(event_id == DPNI_EVENT && generator_id == EVMNG_GENERATOR_AIOPSL){
		sl_pr_debug("DPNI event\n");
		/* calculate pointer to the NI structure */

		cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
		/* Check if the received dpni id exists in nis table*/
		ni_id = dpni_drv_is_dpni_exist((uint16_t)((uint32_t)event_data));
		sl_pr_debug("NI id %d, DPNI id %d\n", ni_id, (int)nis[ni_id].dpni_id);
		if(ni_id == -1){
			sl_pr_debug("DPNI %d not exist in nis table\n", (uint16_t)((uint32_t)event_data));
			cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
			return -EEXIST;
		}
		err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		if(err){
			sl_pr_err("Open DPNI failed\n");
			return err;
		}

		err = dpni_drv_check_irq_status(dprc, dpni);
		if(err){
			dpni_close(&dprc->io, 0, dpni);
			return err;
		}

		err = dpni_get_link_state(&dprc->io,
		                          0,
		                          dpni,
		                          &link_state);
		if(err){
			sl_pr_err("Failed to get dpni link state\n");
			dpni_close(&dprc->io, 0, dpni);
			return err;
		}

		err = dpni_close(&dprc->io, 0, dpni);
		if(err){
			sl_pr_err("Close DPNI failed\n");
			return err;
		}

		dpni_drv_raise_linkchange_event(link_state.up, ni_id);

	}
	else{
		sl_pr_debug("Event %d is not supported\n",event_id);
		return -ENOTSUP;
	}

	return 0;
}

__COLD_CODE void dpni_drv_free(void)
{
	if (nis)
		fsl_free(nis);
	nis = NULL;
}



int dpni_drv_set_multicast_promisc(uint16_t ni_id, int en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_multicast_promisc(&dprc->io, 0, dpni, en);
	if(err){
		sl_pr_err("dpni_set_multicast_promisc failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}


int dpni_drv_get_multicast_promisc(uint16_t ni_id, int *en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_multicast_promisc(&dprc->io, 0, dpni, en);
	if(err){
		sl_pr_err("dpni_get_multicast_promisc failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}


int dpni_drv_set_unicast_promisc(uint16_t ni_id, int en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_unicast_promisc(&dprc->io, 0, dpni, en);
	if(err){
		sl_pr_err("dpni_set_unicast_promisc failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}


int dpni_drv_get_unicast_promisc(uint16_t ni_id, int *en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_unicast_promisc(&dprc->io, 0, dpni, en);
	if(err){
		sl_pr_err("dpni_get_unicast_promisc failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_ordering_mode(uint16_t ni_id){
	uint32_t ep_osc;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;


	/* write epid index to epas register */
	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	iowrite32_ccsr((uint32_t)(nis[ni_id].dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	/* read ep_osc - to get the order scope (concurrent / exclusive) */
	ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	EP_MNG_MUTEX_RELEASE;

	return (int)(ep_osc & ORDER_MODE_BIT_MASK) >> 24;
}

static int dpni_drv_set_ordering_mode(uint16_t ni_id, int ep_mode){
	uint32_t ep_osc;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	/* write epid index to epas register */
	iowrite32_ccsr((uint32_t)(nis[ni_id].dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	/* read ep_osc - to get the order scope (concurrent / exclusive) */
	ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
	ep_osc &= ORDER_MODE_CLEAR_BIT;
	ep_osc |= (ep_mode & 0x01) << 24;
	/*Set concurrent mode for NI in epid table*/
	iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);
	EP_MNG_MUTEX_RELEASE;
	return 0;
}

int dpni_drv_set_concurrent(uint16_t ni_id){
	return dpni_drv_set_ordering_mode(ni_id, DPNI_DRV_CONCURRENT_MODE);
}

int dpni_drv_set_exclusive(uint16_t ni_id){
	return dpni_drv_set_ordering_mode(ni_id, DPNI_DRV_EXCLUSIVE_MODE);
}

__COLD_CODE int dpni_drv_set_order_scope(uint16_t ni_id, struct dpkg_profile_cfg *key_cfg)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_rx_tc_dist_cfg cfg = {0};
	struct dpni_attr attr = {0};
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
						sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;
	int err;
	uint32_t ep_osc;
	uint16_t dpni;
	uint8_t i;

	/*Mutex will be needed if the function will be supported in run time*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table*/
	memset(order_scope_buffer, 0, PARAMS_IOVA_BUFF_SIZE);
	cfg.dist_size = 1;
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	if(err){
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	if(key_cfg == DPNI_DRV_NO_ORDER_SCOPE)
	{
		/*Mutex lock to avoid race condition while writing to EPID table*/
		/*Mutex will be needed if the function will be supported in run time*/
		EP_MNG_MUTEX_W_TAKE;
		/* write epid index to epas register */
		iowrite32_ccsr((uint32_t)(nis[ni_id].dpni_drv_params_var.epid_idx), &wrks_addr->epas);
		/* Read ep_osc from EPID table */
		ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
		/* src = 0 - No order scope specified. Task does not enter a scope,
		 * ep = 0 - executing concurrently,
		 * sel = 0,
		 * osrm = 0 - all frames enter scope 0 */
		ep_osc &= ORDER_MODE_NO_ORDER_SCOPE;
		/*Write ep_osc to EPID table*/
		iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);
		EP_MNG_MUTEX_RELEASE;
		cfg.dist_mode = DPNI_DIST_MODE_NONE;
	}
	else
	{
		dpni_prepare_key_cfg(key_cfg, order_scope_buffer);
		cfg.dist_mode = DPNI_DIST_MODE_HASH;
	}

	cfg.key_cfg_iova = (uint64_t)order_scope_buffer;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/

	err = dpni_get_attributes(&dprc->io, 0, dpni, &attr);
	if(err){
		sl_pr_err("dpni_get_attributes failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	for(i = 0; i < attr.num_tcs; i++)
	{
		err = dpni_set_rx_tc_dist(&dprc->io, 0, dpni, i,  &cfg);
		if(err){
			sl_pr_err("dpni_set_rx_tc_dist failed\n");
			dpni_close(&dprc->io, 0, dpni);
			return err;
		}
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_connected_ni(const int id, const char type[16], uint16_t *aiop_niid, int *state)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dprc_endpoint endpoint1 = {0};
	struct dprc_endpoint endpoint2 = {0};
	int err;
	uint16_t i;

	if(dprc == NULL)
		return -EINVAL;

	endpoint1.id = id;
	endpoint1.if_id = 0;
	strcpy(&endpoint1.type[0], type);

	err = dprc_get_connection(&dprc->io, 0, dprc->token, &endpoint1, &endpoint2,
	                          state);
	if(err){
		sl_pr_err("dprc_get_connection failed\n");
		return err;
	}

	if(strcmp(endpoint2.type, "dpni")){
		return -ENAVAIL;
	}

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	for(i = 0; i < (uint16_t)num_of_nis; i++){
		if(endpoint2.id == nis[i].dpni_id){
			*aiop_niid = i;
			break;
		}
	}
	if(i == num_of_nis){
		sl_pr_err("connected AIOP NI to DPNI %d not found\n", endpoint2.id);
		err = -ENAVAIL;
	}
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	return err;
}

int dpni_drv_get_connected_obj(const uint16_t aiop_niid, int *id, char type[16], int *state)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dprc_endpoint endpoint1 = {0};
	struct dprc_endpoint endpoint2 = {0};
	int err;

	if(dprc == NULL)
		return -EINVAL;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	endpoint1.id = nis[aiop_niid].dpni_id;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	endpoint1.if_id = 0;
	strcpy(&endpoint1.type[0], "dpni");

	err = dprc_get_connection(&dprc->io, 0, dprc->token, &endpoint1, &endpoint2,
	                          state);
	if(err){
		sl_pr_err("dprc_get_connection failed\n");
		return err;
	}
	*id = endpoint2.id;
	strncpy(&type[0], &endpoint2.type[0], sizeof(char) * 16);
	return 0;
}

int dpni_drv_set_rx_buffer_layout(uint16_t ni_id, const struct dpni_drv_buf_layout *layout)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_buffer_layout dpni_layout;
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	dpni_layout.options = layout->options;
	dpni_layout.pass_timestamp = layout->pass_timestamp;
	dpni_layout.pass_parser_result = layout->pass_parser_result;
	dpni_layout.pass_frame_status = layout->pass_frame_status;
	dpni_layout.private_data_size = layout->private_data_size;
	dpni_layout.data_align = layout->data_align;
	dpni_layout.data_head_room = layout->data_head_room;
	dpni_layout.data_tail_room = layout->data_tail_room;

#ifdef ERR009354
	dpni_layout.options |= DPNI_BUF_LAYOUT_OPT_DATA_ALIGN;
	dpni_layout.data_align = DPNI_DRV_DATA_ALIGN_DEF;
	sl_pr_debug("ERR009354: data-offset must be aligned to 256\n");
#endif  /* ERR009354 */

	err = dpni_set_buffer_layout(&dprc->io, 0, dpni, DPNI_QUEUE_RX,
			&dpni_layout);
	if(err){
		sl_pr_err("dpni_set_rx_buffer_layout failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_rx_buffer_layout(uint16_t ni_id, struct dpni_drv_buf_layout *layout)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_buffer_layout dpni_layout = {0};
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_buffer_layout(&dprc->io, 0, dpni, DPNI_QUEUE_RX,
			&dpni_layout);
	if(err){
		sl_pr_err("dpni_get_rx_buffer_layout failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	layout->options = dpni_layout.options;
	layout->pass_timestamp = dpni_layout.pass_timestamp;
	layout->pass_parser_result = dpni_layout.pass_parser_result;
	layout->pass_frame_status = dpni_layout.pass_frame_status;
	layout->private_data_size = dpni_layout.private_data_size;
	layout->data_align = dpni_layout.data_align;
	layout->data_head_room = dpni_layout.data_head_room;
	layout->data_tail_room = dpni_layout.data_tail_room;
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_register_rx_buffer_layout_requirements(uint16_t head_room, uint16_t tail_room, uint16_t private_data_size)
{
	g_dpni_early_init_data.count++;

	g_dpni_early_init_data.head_room_sum += head_room;
	g_dpni_early_init_data.tail_room_sum += tail_room;

	if(private_data_size) {
		g_dpni_early_init_data.private_data_size_sum = DPNI_DRV_PTA_SIZE;
	}

	return 0;
}

int dpni_drv_get_counter(uint16_t ni_id, enum dpni_drv_counter counter, uint64_t *value)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	union dpni_statistics stats;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_get_statistics(&dprc->io, 0, dpni,dpni_statistics[counter].page,&stats);
	if(err){
		sl_pr_err("dpni_get_counter failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	memcpy(value,&stats + dpni_statistics[counter].offset, sizeof(uint64_t));

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_reset_statistics(uint16_t ni_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	dpni_reset_statistics(&dprc->io,0, dpni);
	if(err){
		sl_pr_err("dpni_reset_statistics failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_dpni_id(uint16_t ni_id, uint16_t *dpni_id)
{
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	if(ni_id >= (uint16_t)num_of_nis ||
		nis[ni_id].dpni_id == DPNI_NOT_IN_USE)
	{
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		sl_pr_err("AIOP NI ID %d not exist\n", ni_id);
		return -ENAVAIL;
	}

	*dpni_id = nis[ni_id].dpni_id;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	return 0;
}

int dpni_drv_get_ni_id(uint16_t dpni_id, uint16_t *ni_id)
{
	uint16_t i;
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	for(i = 0; i < (uint16_t)num_of_nis; i++)
	{
		if(nis[i].dpni_id == dpni_id)
		{
			*ni_id = i;
			break;
		}
	}
	if(i == (uint16_t)num_of_nis){
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		sl_pr_err("DPNI ID %d not exist\n", dpni_id);
		return -ENAVAIL;
	}
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	return 0;
}

int dpni_drv_get_link_state(uint16_t ni_id, struct dpni_drv_link_state *state)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_link_state link_state;
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_link_state(&dprc->io, 0, dpni, &link_state);
	if(err){
		sl_pr_err("dpni_get_link_state failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	state->options = link_state.options;
	state->rate = link_state.rate;
	state->up = link_state.up;
	err = dpni_close(&dprc->io,0,  dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_clear_mac_filters(uint16_t ni_id, uint8_t unicast, uint8_t multicast)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_clear_mac_filters(&dprc->io, 0, dpni,
	                             (int)unicast,
	                             (int) multicast);
	if(err){
		sl_pr_err("dpni_clear_mac_filters failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_clear_vlan_filters(uint16_t ni_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_clear_vlan_filters(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("dpni_clear_vlan_filters failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_vlan_filters(uint16_t ni_id, int en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_enable_vlan_filter(&dprc->io, 0, dpni, en);
	if(err){
		sl_pr_err("dpni_set_vlan_filters failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_add_vlan_id(uint16_t ni_id, uint16_t vlan_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_add_vlan_id(&dprc->io, 0, dpni, vlan_id);
	if(err){
		sl_pr_err("dpni_add_vlan_id failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_remove_vlan_id(uint16_t ni_id, uint16_t vlan_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_remove_vlan_id(&dprc->io,0,  dpni, vlan_id);
	if(err){
		sl_pr_err("dpni_remove_vlan_id failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_initial_presentation(
	uint16_t ni_id,
	struct ep_init_presentation* const init_presentation){
	uint16_t epid;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	epid = nis[ni_id].dpni_drv_params_var.epid_idx;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/

	return ep_mng_get_initial_presentation(epid, init_presentation);
}

int dpni_drv_set_initial_presentation(
	uint16_t ni_id,
	const struct ep_init_presentation* const init_presentation){
	uint16_t epid;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	epid = nis[ni_id].dpni_drv_params_var.epid_idx;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/

	return ep_mng_set_initial_presentation(epid, init_presentation);
}

/* This function is not exposed to users */
int dpni_drv_set_irq_enable(uint16_t ni_id, uint8_t en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t mc_niid;
	uint16_t dpni;
	struct dpni_irq_cfg irq_cfg = { 0 };

	ASSERT_COND_LIGHT(en == 0 || en == 1);
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	mc_niid = nis[ni_id].dpni_id;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	err = dpni_open(&dprc->io, 0, (int)mc_niid, &dpni);
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	if(en){
		irq_cfg.addr = (uint64_t)DPNI_EVENT;
		irq_cfg.val = (uint32_t)mc_niid;
		irq_cfg.irq_num = 0;

		sl_pr_debug("Register for irq with addr %d and val %d\n", (int)irq_cfg.addr, (int)irq_cfg.val);


		err = dpni_set_irq(&dprc->io, 0, dpni,
		                   DPNI_IRQ_INDEX, &irq_cfg);
		if(err){
			sl_pr_err("Failed to set irq for DP-NI%d\n", mc_niid);
			dpni_close(&dprc->io, 0, dpni);
			return err;
		}

		err = dpni_set_irq_mask(&dprc->io, 0, dpni,
		                        DPNI_IRQ_INDEX,
		                        DPNI_IRQ_EVENT_LINK_CHANGED);
		if(err){
			sl_pr_err("Failed to set irq mask for DP-NI%d\n", mc_niid);
			dpni_close(&dprc->io, 0, dpni);
			return err;
		}

		err = dpni_clear_irq_status(&dprc->io, 0, dpni,
		                            DPNI_IRQ_INDEX,
		                            DPNI_IRQ_EVENT_LINK_CHANGED);
		if(err){
			sl_pr_err("Failed to clear IRQ status for DP-NI%d\n", mc_niid);
			dpni_close(&dprc->io, 0, dpni);
			return err;
		}
	}
	err = dpni_set_irq_enable(&dprc->io, 0, dpni,
	                          DPNI_IRQ_INDEX, en);
	if(err){
		sl_pr_err("dpni_set_irq_enable failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_tx_checksum(uint16_t ni_id,
                             const struct dpni_drv_tx_checksum * const tx_checksum)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	uint16_t flow_id = DPNI_DRV_AIOP_TX_FLOW_ID;
	struct dpni_queue queue = {0};

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	
	if(tx_checksum->l3_checksum_gen == 1) 
		dpni_set_offload(&dprc->io, 0, dpni,DPNI_OFF_TX_L3_CSUM, 1);
	 else 
		dpni_set_offload(&dprc->io, 0, dpni,DPNI_OFF_TX_L3_CSUM, 0);
	
	if(tx_checksum->l4_checksum_gen == 1) 
		dpni_set_offload(&dprc->io, 0, dpni,DPNI_OFF_TX_L4_CSUM, 1);
	else 
		dpni_set_offload(&dprc->io, 0, dpni,DPNI_OFF_TX_L4_CSUM, 0);
	
	err = dpni_set_queue(&dprc->io, 0, dpni,DPNI_QUEUE_TX,0,0,0, &queue);
	if(err){
		sl_pr_err("dpni_set_tx_flow failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_tx_checksum(uint16_t ni_id,
                            struct dpni_drv_tx_checksum * const tx_checksum)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	uint32_t offload_config;
	struct dpni_queue queue = {0};
	struct dpni_queue_id queue_id;
	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_get_queue(&dprc->io, 0, dpni, DPNI_QUEUE_TX, 0, 0, &queue, &queue_id);
	if(err){
		sl_pr_err("dpni_get_tx_flow failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
		dpni_get_offload(&dprc->io, 0, dpni,DPNI_OFF_TX_L3_CSUM, &offload_config);
		tx_checksum->l3_checksum_gen = (uint16_t)offload_config;
		dpni_get_offload(&dprc->io, 0, dpni,DPNI_OFF_TX_L4_CSUM, &offload_config);
		tx_checksum->l4_checksum_gen = (uint16_t)offload_config;
	
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_rx_tc_policing(uint16_t ni_id, uint8_t tc_id,
                                const struct dpni_drv_rx_tc_policing_cfg *cfg)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	struct dpni_rx_tc_policing_cfg tc_policing = {0};

	if(cfg->options & DPNI_DRV_POLICER_OPT_COLOR_AWARE){
		tc_policing.options |= DPNI_POLICER_OPT_COLOR_AWARE;
	}
	if(cfg->options & DPNI_DRV_POLICER_OPT_DISCARD_RED){
		tc_policing.options |= DPNI_POLICER_OPT_DISCARD_RED;
	}

	tc_policing.mode = (enum dpni_policer_mode)(cfg->mode);
	tc_policing.units = (enum dpni_policer_unit)(cfg->unit);
	tc_policing.default_color = (enum dpni_policer_color)(cfg->default_color);
	tc_policing.cir = cfg->cir;
	tc_policing.cbs = cfg->cbs;
	tc_policing.eir = cfg->eir;
	tc_policing.ebs = cfg->ebs;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed err %d\n", err);
		return err;
	}

	err = dpni_set_rx_tc_policing(&dprc->io, 0, dpni, tc_id, &tc_policing);
	if(err){
		sl_pr_err("dpni_set_rx_tc_policing failed err %d\n", err);
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed err %d\n", err);
		return err;
	}
	return 0;
}

int dpni_drv_get_rx_tc_policing(uint16_t ni_id, uint8_t tc_id,
                                struct dpni_drv_rx_tc_policing_cfg * const cfg)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	struct dpni_rx_tc_policing_cfg tc_policing = {0};

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_get_rx_tc_policing(&dprc->io, 0, dpni, tc_id, &tc_policing);
	if(err){
		sl_pr_err("dpni_get_rx_tc_policing failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}

	cfg->options = 0;
	if(tc_policing.options & DPNI_POLICER_OPT_COLOR_AWARE){
		cfg->options |= DPNI_DRV_POLICER_OPT_COLOR_AWARE;
	}
	if(tc_policing.options & DPNI_POLICER_OPT_DISCARD_RED){
		cfg->options |= DPNI_DRV_POLICER_OPT_DISCARD_RED;
	}

	cfg->mode = (enum dpni_drv_policer_mode)(tc_policing.mode);
	cfg->unit = (enum dpni_drv_policer_unit)(tc_policing.units);
	cfg->default_color = (enum dpni_drv_policer_color)(tc_policing.default_color);
	cfg->cir = tc_policing.cir;
	cfg->cbs = tc_policing.cbs;
	cfg->eir = tc_policing.eir;
	cfg->ebs = tc_policing.ebs;
	return 0;
}

int dpni_drv_set_tx_selection(uint16_t ni_id,
                          const struct dpni_drv_tx_selection *cfg)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_set_tx_priorities(&dprc->io, 0, dpni,
	                            (struct dpni_tx_priorities_cfg *) &cfg);
	if(err){
		sl_pr_err("dpni_set_tx_selection failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_tx_shaping(uint16_t ni_id,
                          const struct dpni_drv_tx_shaping *cfg)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	struct dpni_tx_shaping_cfg tx_shaper = {0};

	tx_shaper.rate_limit = cfg->rate_limit;
	tx_shaper.max_burst_size = cfg->max_burst_size;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_set_tx_shaping(&dprc->io, 0, dpni, &tx_shaper);
	if(err){
		sl_pr_err("dpni_set_tx_shaping failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_qos_table(uint16_t ni_id,
                          const struct dpni_drv_qos_tbl *cfg)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	struct dpni_qos_tbl_cfg  qos_tbl = {0};

	qos_tbl.key_cfg_iova = cfg->key_cfg_iova;
	qos_tbl.discard_on_miss = (int)cfg->discard_on_miss;
	qos_tbl.default_tc = cfg->default_tc;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_set_qos_table(&dprc->io, 0, dpni, &qos_tbl);
	if(err){
		sl_pr_err("dpni_set_qos_table failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_add_qos_entry(uint16_t ni_id,
                           const struct dpni_drv_qos_rule *cfg,
                           uint8_t tc_id, 
                           uint16_t index)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	struct dpni_rule_cfg  rule_cfg = {0};

	rule_cfg.key_iova = cfg->key_iova;
	rule_cfg.key_size = cfg->key_size;
	rule_cfg.mask_iova = cfg->mask_iova;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_add_qos_entry(&dprc->io, 0, dpni, &rule_cfg, tc_id, index);
	if(err){
		sl_pr_err("dpni_add_qos_entry failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_remove_qos_entry(uint16_t ni_id,
                           const struct dpni_drv_qos_rule *cfg)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	struct dpni_rule_cfg  rule_cfg = {0};

	rule_cfg.key_iova = cfg->key_iova;
	rule_cfg.key_size = cfg->key_size;
	rule_cfg.mask_iova = cfg->mask_iova;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_remove_qos_entry(&dprc->io, 0, dpni, &rule_cfg);
	if(err){
		sl_pr_err("dpni_remove_qos_entry failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_clear_qos_table(uint16_t ni_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_clear_qos_table(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("dpni_clear_qos_table failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

void dpni_drv_prepare_rx_tc_early_drop(
	const struct dpni_drv_early_drop_cfg *cfg,
	uint8_t *early_drop_buf)
{
	struct dpni_early_drop_cfg early_drop_cfg;
	memset(&early_drop_cfg, 0, sizeof(struct dpni_early_drop_cfg));

	early_drop_cfg.mode = (enum dpni_early_drop_mode) cfg->mode;
	early_drop_cfg.units = (enum dpni_congestion_unit) cfg->units;
	early_drop_cfg.green.max_threshold = cfg->green.max_threshold;
	early_drop_cfg.green.min_threshold = cfg->green.min_threshold;
	early_drop_cfg.green.drop_probability = cfg->green.drop_probability;
	early_drop_cfg.yellow.max_threshold = cfg->yellow.max_threshold;
	early_drop_cfg.yellow.min_threshold = cfg->yellow.min_threshold;
	early_drop_cfg.yellow.drop_probability = cfg->yellow.drop_probability;
	early_drop_cfg.red.max_threshold = cfg->red.max_threshold;
	early_drop_cfg.red.min_threshold = cfg->red.min_threshold;
	early_drop_cfg.red.drop_probability = cfg->red.drop_probability;
	early_drop_cfg.tail_drop_threshold = cfg->tail_drop_threshold;

	dpni_prepare_early_drop(&early_drop_cfg, early_drop_buf);
}

int dpni_drv_set_rx_tc_early_drop(uint16_t ni_id,
                                  uint8_t tc_id,
                                  uint64_t early_drop_iova)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_set_early_drop(&dprc->io, 0, dpni, DPNI_QUEUE_RX, tc_id,
			early_drop_iova);
	if(err){
		sl_pr_err("dpni_set_rx_tc_early_drop failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}


int dpni_drv_prepare_key_cfg(struct dpkg_profile_cfg *cfg,
                             uint8_t *key_cfg_buf)
{
	int err;
	err = dpni_prepare_key_cfg(cfg, key_cfg_buf);
	if(err){
		sl_pr_err("dpni_prepare_key_cfg failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_num_free_bufs(uint32_t flags,
			       struct dpni_drv_free_bufs *free_bufs)
{
	struct mc_dprc		*dprc;
	int			i, err, dp_bpid;
	uint16_t		dpbp;
	uint32_t		num_free_bufs;

	/* Only backup counter requested but backup pool is disabled */
	if ((flags & DPNI_DRV_BACKUP_FREE_BUFS) == DPNI_DRV_BACKUP_FREE_BUFS &&
	    (g_app_params.app_config_flags & DPNI_BACKUP_POOL_DISABLE)) {
			pr_warn("Backup pool not enabled\n");
			return -EINVAL;
	}
	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	free_bufs->peb_bp_free_bufs = 0;
	free_bufs->backup_bp_free_bufs = 0;
	for (i = 0; i < pools_params.num_dpbp; i++) {
		/* Counter not requested case */
		if ((pools_params.pools[i].backup_pool &&
		     !(flags & DPNI_DRV_BACKUP_FREE_BUFS)) ||
		    (!pools_params.pools[i].backup_pool &&
		     !(flags & DPNI_DRV_PEB_FREE_BUFS)))
			continue;
		dp_bpid = pools_params.pools[i].dpbp_id;
		/* Open DPBP control session */
		err = dpbp_open(&dprc->io, 0, dp_bpid, &dpbp);
		if (err) {
			pr_err("Open DPBP@%d\n", dp_bpid);
			return err;
		}
		num_free_bufs = 0;
		/* Get number of free buffers */
		err = dpbp_get_num_free_bufs(&dprc->io, 0, dpbp,
					     &num_free_bufs);
		if (err) {
			pr_err("Get number of free buffers for DPBP@%d.\n",
			       dp_bpid);
			/* Close DPBP control session */
			if (dpbp_close(&dprc->io, 0, dpbp))
				pr_err("Close DPBP@%d.\n", dp_bpid);
			return err;
		}
		/* Close DPBP control session */
		err = dpbp_close(&dprc->io, 0, dpbp);
		if (err) {
			pr_err("Close DPBP@%d.\n", dp_bpid);
			return err;
		}
		if (pools_params.pools[i].backup_pool)
			free_bufs->backup_bp_free_bufs += num_free_bufs;
		else
			free_bufs->peb_bp_free_bufs += num_free_bufs;
	}
	return 0;
}
