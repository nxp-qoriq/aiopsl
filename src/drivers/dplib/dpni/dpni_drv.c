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
#include "fsl_sl_dbg.h"
#include "common/fsl_string.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_malloc.h"
#include "fsl_io_ccsr.h"
#include "fsl_dpni.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "platform.h"
#include "inc/fsl_sys.h"
#include "fsl_dprc.h"
#include "fsl_dpbp.h"
#include "fsl_bman.h"
#include "ls2085_aiop/fsl_platform.h"
#include "dpni_drv.h"
#include "aiop_common.h"
#include "system.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_sl_slab.h"
#include "fsl_sl_evmng.h"
#include "fsl_ep_mng.h"

#define __ERR_MODULE__  MODULE_DPNI
#define ETH_BROADCAST_ADDR		((uint8_t []){0xff,0xff,0xff,0xff,0xff,0xff})
int dpni_drv_init(void);
void dpni_drv_free(void);

extern struct aiop_init_info g_init_data;
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
					sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	iowrite32_ccsr((uint32_t)(nis[ni_id].dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	iowrite32_ccsr(PTR_TO_UINT(cb), &wrks_addr->ep_pc);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
	return 0;
}

int dpni_drv_unregister_rx_cb (uint16_t ni_id)
{
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	iowrite32_ccsr((uint32_t)(nis[ni_id].dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
	return 0;
}

int dpni_drv_enable (uint16_t ni_id)
{
	uint16_t dpni;
	int err;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_enable(&dprc->io, dpni);
	if(err){
		sl_pr_err("dpni_enable failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, dpni);
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_disable(&dprc->io, dpni);
	if(err){
		sl_pr_err("dpni_disable failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
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
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK);
	index = dpni_drv_is_dpni_exist(mc_niid);
	if(index == -1){
		sl_pr_debug("DPNI %d not exist nis table\n",mc_niid);
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
						sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;


	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	iowrite32_ccsr((uint32_t)nis[aiop_niid].dpni_drv_params_var.epid_idx,
	               &wrks_addr->epas);
	/*clear ep_pc*/
	iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	/*Mutex unlock EPID table*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
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
	struct dpni_attr attributes;
	int err;

	err = dpni_get_attributes(&dprc->io,
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

	err = dpni_set_rx_buffer_layout(&dprc->io,
	                                dpni,
	                                &layout);
	if(err){
		sl_pr_err("Failed to set rx buffer layout\n");
		return err;
	}

	/*
	 * Disable TX confirmation for DPNI's in AIOP in case
	 * the option: 'DPNI_OPT_TX_CONF_DISABLED' was not
	 * selected at DPNI creation.
	 * */
	err = dpni_set_tx_conf_revoke(&dprc->io, dpni, 1);
	if(err){
		sl_pr_err("Failed to set tx_conf_revoke\n");
		return err;
	}

	/* Now a Storage Profile exists and is associated
	 * with the NI */


	/* Register SPID in internal AIOP NI table */
	if ((err = dpni_get_sp_info(&dprc->io,
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
	irq_cfg.user_irq_id = 0;

	sl_pr_debug("Register for irq with addr %d and val %d\n", (int)irq_cfg.addr, (int)irq_cfg.val);


	err = dpni_set_irq(&dprc->io, dpni,
	                   DPNI_IRQ_INDEX, &irq_cfg);
	if(err){
		sl_pr_err("Failed to set irq\n");
		return err;
	}

	err = dpni_set_irq_mask(&dprc->io, dpni,
	                        DPNI_IRQ_INDEX,
	                        DPNI_IRQ_EVENT_LINK_CHANGED);
	if(err){
		sl_pr_err("Failed to set irq mask\n");
		return err;
	}

	err = dpni_clear_irq_status(&dprc->io, dpni,
	                            DPNI_IRQ_INDEX,
	                            DPNI_IRQ_EVENT_LINK_CHANGED);
	if(err){
		sl_pr_err("Failed to clear IRQ status\n");
		return err;
	}

	err = dpni_set_irq_enable(&dprc->io, dpni,
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
	uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];


	err = dpni_open(&dprc->io, mc_niid, &dpni);
	if(err){
		sl_pr_err("Failed to open DP-NI%d\n.", mc_niid);
		/* if open failed, no need to close*/
		return err;
	}

	/* Register MAC address in internal AIOP NI table */
	err = dpni_get_primary_mac_addr(&dprc->io,
	                                dpni,
	                                mac_addr);
	if(err){
		sl_pr_err("Failed to get MAC address for DP-NI%d\n",
		          mc_niid);
		dpni_close(&dprc->io, dpni);
		return err;
	}
	memcpy(nis[aiop_niid].mac_addr,
	       mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);


	/* Register QDID in internal AIOP NI table */
	err = dpni_get_qdid(&dprc->io,
	                    dpni, &qdid);
	if(err){
		sl_pr_err("Failed to get QDID for DP-NI%d\n",
		          mc_niid);
		dpni_close(&dprc->io, dpni);
		return err;
	}
	nis[aiop_niid].dpni_drv_tx_params_var.qdid = qdid;

	if(configure_dpni_params(dprc, aiop_niid, dpni)){
		sl_pr_err("configure_dpni_params failed for DP-NI%d\n",
		          mc_niid);
		dpni_close(&dprc->io, dpni);
		return err;
	}

	if(configure_dpni_irq(dprc, mc_niid, dpni)){
		sl_pr_err("configure_dpni_irq failed for DP-NI%d\n",
		          mc_niid);
		dpni_close(&dprc->io, dpni);
		return err;
	}

	err = dpni_close(&dprc->io, dpni);
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
					sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
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
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	/* Search for NIID (mc_niid) in EPID table and prepare the NI for usage. */
	for (i = AIOP_EPID_DPNI_START; i < AIOP_EPID_TABLE_SIZE; i++) {
		/* Prepare to read from entry i in EPID table - EPAS reg */
		iowrite32_ccsr((uint32_t)i, &wrks_addr->epas);

		/* Read Entry Point Param (EP_PM) which contains the MC NI ID */
		j = ioread32_ccsr(&wrks_addr->ep_pm);

		sl_pr_debug("EPID[%d].EP_PM = %d\n", i, j);

		/*MC dpni id found in EPID table*/
		if (j == mc_niid) {
			/* Replace MC NI ID with AIOP NI ID */
			iowrite32_ccsr(aiop_niid, &wrks_addr->ep_pm);

			ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
			ep_osc &= ORDER_MODE_CLEAR_BIT;
			/*Set concurrent mode for NI in epid table*/
			iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);

			/*Mutex unlock EPID table*/
			cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
			/* Store epid index in AIOP NI's array*/
			nis[aiop_niid].dpni_drv_params_var.epid_idx =
				(uint16_t)i;
			err = initialize_dpni(dprc, mc_niid, aiop_niid);
			if(err)
				return err;
			num_of_nis ++;

			nis[aiop_niid].dpni_id = mc_niid;
			/* Unlock nis table*/
			*niid = aiop_niid;
			return 0;
		}
	}


	if(i == AIOP_EPID_TABLE_SIZE){ /*MC dpni id not found in EPID table*/
		/*Unlock EPID table*/
		cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_set_primary_mac_addr(&dprc->io, dpni, mac_addr);
	if(err){
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		sl_pr_err("dpni_set_primary_mac_addr failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}

	memcpy(nis[ni_id].mac_addr, mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);
	/*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_add_mac_addr(uint16_t ni_id,
                          const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_add_mac_addr(&dprc->io, dpni, mac_addr);
	if(err){
		sl_pr_err("dpni_add_mac_addr failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_remove_mac_addr(uint16_t ni_id,
                             const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_remove_mac_addr(&dprc->io, dpni, mac_addr);
	if(err){
		sl_pr_err("dpni_remove_mac_addr failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_max_frame_length(uint16_t ni_id,
                                  const uint16_t mfl)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_max_frame_length(&dprc->io, dpni, mfl);
	if(err){
		sl_pr_err("dpni_set_max_frame_length failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_max_frame_length(uint16_t ni_id,
                                  uint16_t *mfl)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_max_frame_length(&dprc->io, dpni, mfl);
	if(err){
		sl_pr_err("dpni_get_max_frame_length failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

__COLD_CODE static int parser_profile_init(uint8_t *prpid)
{
	struct parse_profile_input parse_profile1 __attribute__((aligned(16)));
	int i;

	/* Init basic parse profile */
	parse_profile1.parse_profile.eth_hxs_config = 0x0;
	parse_profile1.parse_profile.llc_snap_hxs_config = 0x0;
	parse_profile1.parse_profile.vlan_hxs_config.en_erm_soft_seq_start = 0x0;
	parse_profile1.parse_profile.vlan_hxs_config.configured_tpid_1 = 0x0;
	parse_profile1.parse_profile.vlan_hxs_config.configured_tpid_2 = 0x0;
	/* No MTU checking */
	parse_profile1.parse_profile.pppoe_ppp_hxs_config = 0x0;
	parse_profile1.parse_profile.mpls_hxs_config.en_erm_soft_seq_start= 0x0;
	/* Frame Parsing advances to MPLS Default Next Parse (IP HXS) */
	parse_profile1.parse_profile.mpls_hxs_config.lie_dnp = PARSER_PRP_MPLS_HXS_CONFIG_LIE;
	parse_profile1.parse_profile.arp_hxs_config = 0x0;
	parse_profile1.parse_profile.ip_hxs_config = 0x0;
	parse_profile1.parse_profile.ipv4_hxs_config = 0x0;
	/* Routing header is ignored and the destination address from
	 * main header is used instead */
	parse_profile1.parse_profile.ipv6_hxs_config = PARSER_PRP_IPV6_HXS_CONFIG_RHE;
	parse_profile1.parse_profile.gre_hxs_config = 0x0;
	parse_profile1.parse_profile.minenc_hxs_config = 0x0;
	parse_profile1.parse_profile.other_l3_shell_hxs_config= 0x0;
	/* In short Packet, padding is removed from Checksum calculation */
	parse_profile1.parse_profile.tcp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	/* In short Packet, padding is removed from Checksum calculation */
	parse_profile1.parse_profile.udp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	parse_profile1.parse_profile.ipsec_hxs_config = 0x0;
	parse_profile1.parse_profile.sctp_hxs_config = 0x0;
	parse_profile1.parse_profile.dccp_hxs_config = 0x0;
	parse_profile1.parse_profile.other_l4_shell_hxs_config = 0x0;
	parse_profile1.parse_profile.gtp_hxs_config = 0x0;
	parse_profile1.parse_profile.esp_hxs_config = 0x0;
	parse_profile1.parse_profile.l5_shell_hxs_config = 0x0;
	parse_profile1.parse_profile.final_shell_hxs_config = 0x0;
	/* Assuming no soft examination parameters */
	for(i=0; i < 16; i++)
		parse_profile1.parse_profile.soft_examination_param_array[i] = 0x0;

	return parser_profile_create(&(parse_profile1), prpid);
}

/* Used to configure DPBP's for dpni's during dpni driver initialization */
static int configure_bpids_for_dpni(void)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
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
	uint16_t alignment;
	uint8_t mem_pid[] = {DPNI_DRV_FAST_MEMORY, DPNI_DRV_DDR_MEMORY};




	if(IS_POWER_VALID_ALLIGN(g_app_params.dpni_drv_alignment,buffer_size))
		alignment = (uint16_t)g_app_params.dpni_drv_alignment;
	else
	{
		pr_err("Given alignment is not valid (not power of 2 or <= buffer size)\n");
	}
	if (dprc == NULL)
	{
		pr_err("Don't find AIOP root container \n");
		return -ENODEV;
	}
	/* TODO: replace the following dpbp_open&init with dpbp_create when available */


	if ((err = dprc_get_obj_count(&dprc->io, dprc->token,
	                              &dev_count)) != 0) {
		pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		       dprc->token);
		return err;
	}

	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpbp") == 0) {
			/* TODO: print conditionally based on log level */
			pr_info("Found DPBP ID: %d, will be used for frame buffers\n",dev_desc.id);
			dpbp_id[num_bpids]= dev_desc.id;
			num_bpids ++;

			if(num_bpids == DPNI_DRV_NUM_USED_BPIDS)
				break;
		}
	}

	if(num_bpids < DPNI_DRV_NUM_USED_BPIDS){
		pr_err("Not enough DPBPs found in the container.\n");
		return -ENAVAIL;
	}

	for(i = 0; i < DPNI_DRV_NUM_USED_BPIDS; i++)
	{
		if ((err = dpbp_open(&dprc->io, dpbp_id[i], &dpbp)) != 0) {
			pr_err("Failed to open DPBP-%d.\n", dpbp_id[i]);
			return err;
		}

		if ((err = dpbp_enable(&dprc->io, dpbp)) != 0) {
			pr_err("Failed to enable DPBP-%d.\n", dpbp_id[i]);
			return err;
		}

		if ((err = dpbp_get_attributes(&dprc->io, dpbp, &attr)) != 0) {
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
		if(mem_pid[i] == DPNI_DRV_DDR_MEMORY){
			pools_params.pools[i].backup_pool = 1;
		}

		/* Enable all DPNI devices */
	}
	return 0;
}

__COLD_CODE int dpni_drv_init(void)
{
	int i;
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

	return err;
}

/* Check irq status for received event from MC*/
static int dpni_drv_check_irq_status(struct mc_dprc *dprc, uint16_t dpni)
{
	int err;
	uint32_t status;

	err = dpni_get_irq_status(&dprc->io,
	                          dpni,
	                          DPNI_IRQ_INDEX,
	                          &status);
	if(err){
		sl_pr_err("Get irq status failed\n");
		return err;
	}

	if(status & DPNI_IRQ_EVENT_LINK_CHANGED){
		err = dpni_clear_irq_status(&dprc->io, dpni,
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
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
		err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		if(err){
			sl_pr_err("Open DPNI failed\n");
			return err;
		}

		err = dpni_drv_check_irq_status(dprc, dpni);
		if(err){
			dpni_close(&dprc->io, dpni);
			return err;
		}

		err = dpni_get_link_state(&dprc->io,
		                          dpni,
		                          &link_state);
		if(err){
			sl_pr_err("Failed to get dpni link state\n");
			dpni_close(&dprc->io, dpni);
			return err;
		}

		err = dpni_close(&dprc->io, dpni);
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_multicast_promisc(&dprc->io, dpni, en);
	if(err){
		sl_pr_err("dpni_set_multicast_promisc failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}


int dpni_drv_get_multicast_promisc(uint16_t ni_id, int *en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_multicast_promisc(&dprc->io, dpni, en);
	if(err){
		sl_pr_err("dpni_get_multicast_promisc failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}


int dpni_drv_set_unicast_promisc(uint16_t ni_id, int en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_unicast_promisc(&dprc->io, dpni, en);
	if(err){
		sl_pr_err("dpni_set_unicast_promisc failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}


int dpni_drv_get_unicast_promisc(uint16_t ni_id, int *en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_unicast_promisc(&dprc->io, dpni, en);
	if(err){
		sl_pr_err("dpni_get_unicast_promisc failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_ordering_mode(uint16_t ni_id){
	uint32_t ep_osc;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;


	/* write epid index to epas register */
	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	iowrite32_ccsr((uint32_t)(nis[ni_id].dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	/* read ep_osc - to get the order scope (concurrent / exclusive) */
	ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);

	return (int)(ep_osc & ORDER_MODE_BIT_MASK) >> 24;
}

static int dpni_drv_set_ordering_mode(uint16_t ni_id, int ep_mode){
	uint32_t ep_osc;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
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
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_rx_tc_dist_cfg cfg = {0};
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
						sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;
	int err;
	uint32_t ep_osc;
	uint16_t dpni;

	/*Mutex will be needed if the function will be supported in run time*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table*/
	memset(order_scope_buffer, 0, PARAMS_IOVA_BUFF_SIZE);
	cfg.dist_size = 0;
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	if(err){
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	if(key_cfg == DPNI_DRV_NO_ORDER_SCOPE)
	{
		/*Mutex lock to avoid race condition while writing to EPID table*/
		/*Mutex will be needed if the function will be supported in run time*/
		cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
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
		cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
		cfg.dist_mode = DPNI_DIST_MODE_NONE;
	}
	else
	{
		dpni_prepare_key_cfg(key_cfg, order_scope_buffer);
		cfg.dist_mode = DPNI_DIST_MODE_HASH;
	}

	cfg.key_cfg_iova = (uint64_t)order_scope_buffer;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	err = dpni_set_rx_tc_dist(&dprc->io, dpni, 0,  &cfg);
	if(err){
		sl_pr_err("dpni_set_rx_tc_dist failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_connected_aiop_ni_id(const uint16_t dpni_id, uint16_t *aiop_niid, int *state){
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dprc_endpoint endpoint1 = {0};
	struct dprc_endpoint endpoint2 = {0};
	int err;
	uint16_t i;

	if(dprc == NULL)
		return -EINVAL;

	endpoint1.id = dpni_id;
	endpoint1.interface_id = 0;
	strcpy(&endpoint1.type[0], "dpni");

	err = dprc_get_connection(&dprc->io, dprc->token, &endpoint1, &endpoint2,
	                          state);
	if(err){
		sl_pr_err("dprc_get_connection failed\n");
		return err;
	}

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	for(i = 0; i <  dpni_drv_get_num_of_nis(); i++){
		if(endpoint2.id == nis[i].dpni_id){
			*aiop_niid = i;
			break;
		}
	}
	if(i == dpni_drv_get_num_of_nis()){
		sl_pr_err("connected AIOP NI to DPNI %d not found\n", endpoint2.id);
		err = -ENAVAIL;
	}
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	return err;
}

int dpni_drv_get_connected_dpni_id(const uint16_t aiop_niid, uint16_t *dpni_id, int *state){
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dprc_endpoint endpoint1 = {0};
	struct dprc_endpoint endpoint2 = {0};
	int err;

	if(dprc == NULL)
		return -EINVAL;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	endpoint1.id = nis[aiop_niid].dpni_id;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	endpoint1.interface_id = 0;
	strcpy(&endpoint1.type[0], "dpni");

	err = dprc_get_connection(&dprc->io, dprc->token, &endpoint1, &endpoint2,
	                          state);
	if(err){
		sl_pr_err("dprc_get_connection failed\n");
		return err;
	}
	*dpni_id = (uint16_t)endpoint2.id;
	return 0;
}

int dpni_drv_set_rx_buffer_layout(uint16_t ni_id, const struct dpni_drv_buf_layout *layout)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_buffer_layout dpni_layout;
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
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

	err = dpni_set_rx_buffer_layout(&dprc->io, dpni, &dpni_layout);
	if(err){
		sl_pr_err("dpni_set_rx_buffer_layout failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_rx_buffer_layout(uint16_t ni_id, struct dpni_drv_buf_layout *layout)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_buffer_layout dpni_layout = {0};
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_rx_buffer_layout(&dprc->io, dpni, &dpni_layout);
	if(err){
		sl_pr_err("dpni_get_rx_buffer_layout failed\n");
		dpni_close(&dprc->io, dpni);
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
	err = dpni_close(&dprc->io, dpni);
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_counter(&dprc->io, dpni, (enum dpni_counter)counter,
	                       value);
	if(err){
		sl_pr_err("dpni_get_counter failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_reset_counter(uint16_t ni_id, enum dpni_drv_counter counter)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_counter(&dprc->io,
	                       dpni,
	                       (enum dpni_counter)counter,
	                       0);
	if(err){
		sl_pr_err("dpni_set_counter failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_dpni_id(uint16_t ni_id, uint16_t *dpni_id)
{
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	if(ni_id >= dpni_drv_get_num_of_nis() ||
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
	for(i = 0; i < dpni_drv_get_num_of_nis(); i++)
	{
		if(nis[i].dpni_id == dpni_id)
		{
			*ni_id = i;
			break;
		}
	}
	if(i == dpni_drv_get_num_of_nis()){
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		sl_pr_err("DPNI ID %d not exist\n", dpni_id);
		return -ENAVAIL;
	}
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	return 0;
}

int dpni_drv_get_link_state(uint16_t ni_id, struct dpni_drv_link_state *state)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_link_state link_state;
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_link_state(&dprc->io, dpni, &link_state);
	if(err){
		sl_pr_err("dpni_get_link_state failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	state->options = link_state.options;
	state->rate = link_state.rate;
	state->up = link_state.up;
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_clear_mac_filters(uint16_t ni_id, uint8_t unicast, uint8_t multicast)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_clear_mac_filters(&dprc->io, dpni,
	                             (int)unicast,
	                             (int) multicast);
	if(err){
		sl_pr_err("dpni_clear_mac_filters failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_clear_vlan_filters(uint16_t ni_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_clear_vlan_filters(&dprc->io, dpni);
	if(err){
		sl_pr_err("dpni_clear_vlan_filters failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_vlan_filters(uint16_t ni_id, int en)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_vlan_filters(&dprc->io, dpni, en);
	if(err){
		sl_pr_err("dpni_set_vlan_filters failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_add_vlan_id(uint16_t ni_id, uint16_t vlan_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_add_vlan_id(&dprc->io, dpni, vlan_id);
	if(err){
		sl_pr_err("dpni_add_vlan_id failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_remove_vlan_id(uint16_t ni_id, uint16_t vlan_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	err = dpni_open(&dprc->io, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_remove_vlan_id(&dprc->io, dpni, vlan_id);
	if(err){
		sl_pr_err("dpni_remove_vlan_id failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	uint16_t mc_niid;
	uint16_t dpni;
	struct dpni_irq_cfg irq_cfg = { 0 };

	ASSERT_COND_LIGHT(en == 0 || en == 1);
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK); /*Lock dpni table*/
	mc_niid = nis[ni_id].dpni_id;
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	err = dpni_open(&dprc->io, (int)mc_niid, &dpni);
	if(err){
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	if(en){
		irq_cfg.addr = (uint64_t)DPNI_EVENT;
		irq_cfg.val = (uint32_t)mc_niid;
		irq_cfg.user_irq_id = 0;

		sl_pr_debug("Register for irq with addr %d and val %d\n", (int)irq_cfg.addr, (int)irq_cfg.val);


		err = dpni_set_irq(&dprc->io, dpni,
		                   DPNI_IRQ_INDEX, &irq_cfg);
		if(err){
			sl_pr_err("Failed to set irq for DP-NI%d\n", mc_niid);
			dpni_close(&dprc->io, dpni);
			return err;
		}

		err = dpni_set_irq_mask(&dprc->io, dpni,
		                        DPNI_IRQ_INDEX,
		                        DPNI_IRQ_EVENT_LINK_CHANGED);
		if(err){
			sl_pr_err("Failed to set irq mask for DP-NI%d\n", mc_niid);
			dpni_close(&dprc->io, dpni);
			return err;
		}

		err = dpni_clear_irq_status(&dprc->io, dpni,
		                            DPNI_IRQ_INDEX,
		                            DPNI_IRQ_EVENT_LINK_CHANGED);
		if(err){
			sl_pr_err("Failed to clear IRQ status for DP-NI%d\n", mc_niid);
			dpni_close(&dprc->io, dpni);
			return err;
		}
	}
	err = dpni_set_irq_enable(&dprc->io, dpni,
	                          DPNI_IRQ_INDEX, en);
	if(err){
		sl_pr_err("dpni_set_irq_enable failed\n");
		dpni_close(&dprc->io, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}
