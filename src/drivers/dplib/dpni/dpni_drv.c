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
#include "fsl_mem_mng.h"
#include "sparser_drv.h"
#include "fsl_dpni_cmd.h"

#define __ERR_MODULE__  MODULE_DPNI
#define ETH_BROADCAST_ADDR		((uint8_t []){0xff,0xff,0xff,0xff,0xff,0xff})
int dpni_drv_init(void);
void dpni_drv_free(void);

#define DPNI_DEFAULT_DIST_SIZE 1
int g_dpcon_id = -1;
uint8_t g_dpcon_priorities = 1;

extern struct platform_app_params g_app_params;

/*
 * struct for pool params used for storage profile
 * This struct is set during dpni_drv_init and can not change in runtime
 */
struct dpni_pools_cfg pools_params;

/*buffer used for dpni_drv_set_order_scope*/
uint8_t order_scope_buf[PARAMS_IOVA_BUFF_SIZE];

struct dpni_drv *nis;
int num_of_nis;

struct dpni_drv_stats {
	uint8_t page;
	uint8_t offset;
};

uint8_t egress_parse_profile_id;

/* WARNING - Update this structure, size and content if the enumeration
 * dpni_drv_counter changes */
struct dpni_drv_stats dpni_statistics[DPNI_DRV_CNT_EGR_CONF_FRAME + 1] = {
	{ 0, 0 }, { 0, 1 }, { 2, 0 }, { 2, 1 }, { 0, 2 }, { 0, 3 }, { 0, 4 },
	{ 0, 5 }, { 1, 0 }, { 1, 1 }, { 2, 3 }, { 1, 2 }, { 1, 3 }, { 1, 4 },
	{ 1, 5 }, { 2, 2 }, { 2, 4 }
};

/* WARNING - Update this structure, size and content if the enumeration
 * dpni_drv_counter changes */
struct dpni_drv_stats
dpni_qos_statistics[DPNI_DRV_QOS_CNT_EGR_TC_REJECT_FRAME + 1] = {
	{3, 0}, {3, 1}, {3, 2}, {3, 3}
};

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

static inline void clear_fs_table(uint16_t ni_id)
{
	uint8_t i;
	struct dpni_drv *dpni_drv = nis + ni_id;

	dpni_drv->fs.size = 0;
	for (i = 0; i < DPNI_FS_TABLE_SIZE; i++)
		dpni_drv->fs.table[i].pos = DPNI_FS_TABLE_SIZE;
}

static inline uint8_t get_fs_entry(uint16_t ni_id, uint16_t etype)
{
	uint8_t i;
	struct dpni_drv *dpni_drv = nis + ni_id;

	if (dpni_drv->fs.size == 0)
		return DPNI_FS_TABLE_SIZE;

	for (i = 0; i < DPNI_FS_TABLE_SIZE; i++)
		if ((dpni_drv->fs.table[i].pos != DPNI_FS_TABLE_SIZE) &&
		    (dpni_drv->fs.table[i].etype == etype))
			return dpni_drv->fs.table[i].pos;

	return DPNI_FS_TABLE_SIZE;
}

static inline uint8_t add_fs_entry(uint16_t ni_id, uint16_t etype)
{
	uint8_t i;
	struct dpni_drv *dpni_drv = nis + ni_id;

	for (i = 0; i < DPNI_FS_TABLE_SIZE; i++)
		if (dpni_drv->fs.table[i].pos == DPNI_FS_TABLE_SIZE) {
			dpni_drv->fs.table[i].pos = i;
			dpni_drv->fs.table[i].etype = etype;
			dpni_drv->fs.size++;
			return i;
		}
	return DPNI_FS_TABLE_SIZE;
}

static inline void remove_fs_entry(uint16_t ni_id, uint16_t etype)
{
	uint8_t i;
	struct dpni_drv *dpni_drv = nis + ni_id;

	if (dpni_drv->fs.size == 0)
		return;

	for (i = 0; i < DPNI_FS_TABLE_SIZE; i++)
		if ((dpni_drv->fs.table[i].pos != DPNI_FS_TABLE_SIZE) &&
		    (dpni_drv->fs.table[i].etype == etype)) {
			dpni_drv->fs.table[i].pos = DPNI_FS_TABLE_SIZE;
			dpni_drv->fs.size--;
		}
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

int dpni_drv_register_rx_cb_etype(uint16_t ni_id, rx_cb_t *cb, uint16_t etype)
{
	int ret = 0;
	uint8_t pos;
	uint32_t epid;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);

	pos = get_fs_entry(ni_id, etype);
	if (pos != DPNI_FS_TABLE_SIZE) {
		epid = nis[ni_id].dpni_drv_params_var.epid_idx;
		epid += (pos + 1) * (uint32_t)SOC_MAX_NUM_OF_DPNI;
		iowrite32_ccsr(epid, &wrks_addr->epas);
		iowrite32_ccsr(PTR_TO_UINT(cb), &wrks_addr->ep_pc);
	} else {
		ret = -ENOENT;
	}

	/*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)nis);
	EP_MNG_MUTEX_RELEASE;
	return ret;
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

int dpni_drv_unregister_rx_cb_etype(uint16_t ni_id, uint16_t etype)
{
	int ret = 0;
	uint8_t pos;
	uint32_t epid;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);

	pos = get_fs_entry(ni_id, etype);
	if (pos != DPNI_FS_TABLE_SIZE) {
		epid = nis[ni_id].dpni_drv_params_var.epid_idx;
		epid += (pos + 1) * (uint32_t)SOC_MAX_NUM_OF_DPNI;
		iowrite32_ccsr(epid, &wrks_addr->epas);
		iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	} else {
		ret = -ENOENT;
	}

	/*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)nis);
	EP_MNG_MUTEX_RELEASE;
	return ret;
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

int dpni_drv_update_obj(struct mc_dprc *dprc, uint16_t mc_niid, char *label)
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

		err = dpni_drv_probe(dprc, mc_niid, &aiop_niid, label);
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
	uint8_t i;
	uint32_t epid;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
						sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;


	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	iowrite32_ccsr((uint32_t)nis[aiop_niid].dpni_drv_params_var.epid_idx,
	               &wrks_addr->epas);
	/*clear ep_pc*/
	iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);

	/* FS entries */
	epid = nis[aiop_niid].dpni_drv_params_var.epid_idx;
	for (i = 0; i < DPNI_FS_TABLE_SIZE; i++) {
		epid += SOC_MAX_NUM_OF_DPNI;
		iowrite32_ccsr(epid, &wrks_addr->epas);
		iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	}

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

	clear_fs_table(aiop_niid);

}

/* Configure probed dpni's parameters (attributes, pools, layout,
 * TX confirmation, SPID info */
static int configure_dpni_params(struct mc_dprc *dprc, uint16_t aiop_niid,
				 uint16_t dpni)
{
	struct dpni_buffer_layout layout = {0};
	struct dpni_sp_info sp_info = { 0 };
	struct dpni_attr attributes = { 0 };
	int err;

	err = dpni_get_attributes(&dprc->io, 0, dpni, &attributes);
	if (err) {
		sl_pr_err("Failed to get attributes\n");
		return err;
	}
	/* TODO: set nis[aiop_niid].starting_hxs according to the DPNI
	 * attributes. Not yet implemented on MC. Currently always set to zero,
	 * which means ETH. */
	err = dpni_set_pools(&dprc->io, 0, dpni, &pools_params);
	if (err) {
		sl_pr_err("Failed to set the pools\n");
		return err;
	}

	layout.options = DPNI_BUF_LAYOUT_OPT_DATA_HEAD_ROOM |
		DPNI_BUF_LAYOUT_OPT_DATA_TAIL_ROOM;
	if (g_dpni_early_init_data.count > 0) {
		layout.data_head_room =
			g_dpni_early_init_data.head_room_sum;
		layout.data_tail_room =
			g_dpni_early_init_data.tail_room_sum;
		if (g_dpni_early_init_data.private_data_size_sum) {
			layout.private_data_size =
				g_dpni_early_init_data.private_data_size_sum;
			layout.options |= DPNI_BUF_LAYOUT_OPT_PRIVATE_DATA_SIZE;
		}
		if (g_dpni_early_init_data.frame_anno &
		    DPNI_DRV_FA_STATUS_AND_TS) {
			layout.pass_frame_status = 1;
			layout.pass_timestamp = 1;
			layout.options |= DPNI_BUF_LAYOUT_OPT_FRAME_STATUS |
					DPNI_BUF_LAYOUT_OPT_TIMESTAMP;
		}
		if (g_dpni_early_init_data.frame_anno &
		    DPNI_DRV_FA_PARSER_RESULT) {
			layout.pass_parser_result = 1;
			layout.options |= DPNI_BUF_LAYOUT_OPT_PARSER_RESULT;
		}
		if (g_dpni_early_init_data.frame_anno &
		    DPNI_DRV_FA_SW_OPAQUE) {
			layout.pass_sw_opaque = 1;
			layout.options |= DPNI_BUF_LAYOUT_OPT_SW_OPAQUE;
		}
	} else {
		layout.data_head_room = DPNI_DRV_DHR_DEF;
		layout.data_tail_room = DPNI_DRV_DTR_DEF;
		layout.private_data_size = DPNI_DRV_PTA_DEF;
	}

#ifdef ERR009354
	layout.options |= DPNI_BUF_LAYOUT_OPT_DATA_ALIGN;
	layout.data_align = DPNI_DRV_DATA_ALIGN_DEF;
	sl_pr_debug("ERR009354: data-offset must be aligned to 256\n");
#endif  /* ERR009354 */
	err = dpni_set_buffer_layout(&dprc->io, 0, dpni, DPNI_QUEUE_RX,
				     &layout);
	if (err) {
		sl_pr_err("Failed to set rx buffer layout\n");
		return err;
	}
	/* Disable TX confirmation for DPNI's in AIOP in case the option:
	 * 'DPNI_OPT_TX_CONF_DISABLED' was not selected at DPNI creation. */
	err = dpni_set_tx_confirmation_mode(&dprc->io, 0, dpni,
					    DPNI_CONF_DISABLE);
	if (err) {
		sl_pr_err("Failed to set tx_conf_revoke\n");
		return err;
	}
	/* Now a Storage Profile exists and is associated with the NI.
	 * Register SPID in internal AIOP NI table */
	err = dpni_get_sp_info(&dprc->io, 0, dpni, &sp_info);
	if (err) {
		sl_pr_err("Failed to get SPID\n");
		return err;
	}
	/* TODO: change to uint16_t in nis table for the next release*/
	nis[aiop_niid].dpni_drv_params_var.spid = (uint8_t)sp_info.spids[0];
	/* TODO: need to initialize additional NI table fields according to
	 * DPNI attributes */
	/* bpid exist to use for ddr pool*/
	if (pools_params.num_dpbp == 2) {
		nis[aiop_niid].dpni_drv_params_var.spid_ddr =
			(uint8_t)sp_info.spids[1];
	} else {
		sl_pr_err("DDR spid is not available \n");
		nis[aiop_niid].dpni_drv_params_var.spid_ddr = 0;
	}
	return 0;
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

int dpni_drv_probe(struct mc_dprc *dprc, uint16_t mc_niid, uint16_t *niid,
		   char *label)
{
	int i;
	uint32_t j;
	uint32_t ep_osc;
	uint16_t aiop_niid;
	int err = 0;
	uint32_t epid;
	uint8_t k;

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

			/* FS entries */
			epid = (uint32_t)i;
			for (k = 0; k < DPNI_FS_TABLE_SIZE; k++) {
				epid += SOC_MAX_NUM_OF_DPNI;
				iowrite32_ccsr(epid, &wrks_addr->epas);
				iowrite32_ccsr(aiop_niid, &wrks_addr->ep_pm);
				ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
				ep_osc &= ORDER_MODE_CLEAR_BIT;
				/*Set concurrent mode for NI in epid table*/
				iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);
			}

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
			/* Save DPNI label*/
			k = sizeof(nis[aiop_niid].dpni_label);
			strncpy(nis[aiop_niid].dpni_label, label, k);
			nis[aiop_niid].dpni_label[k - 1] = 0;
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

/******************************************************************************/
__COLD_CODE static int parser_profile_init(uint8_t *prpid)
{
	struct parse_profile_input parse_profile1 __attribute__((aligned(16)));
	int ret;

	/* Init basic parse profile */
	memset(&parse_profile1.parse_profile, 0,
	       sizeof(struct parse_profile_record));
	/* Frame Parsing advances to MPLS Default Next Parse (IP HXS) */
	parse_profile1.parse_profile.mpls_hxs_config.lie_dnp =
			PARSER_PRP_MPLS_HXS_CONFIG_LIE;
	/* Routing header is ignored and the destination address from
	 * main header is used instead */
	parse_profile1.parse_profile.ipv6_hxs_config =
			PARSER_PRP_IPV6_HXS_CONFIG_RHE;
	/* In short Packet, padding is removed from Checksum calculation */
	parse_profile1.parse_profile.tcp_hxs_config =
			PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	/* In short Packet, padding is removed from Checksum calculation */
	parse_profile1.parse_profile.udp_hxs_config =
			PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	ret = parser_profile_create(&(parse_profile1), prpid);
	if (ret)
		pr_err("Can't create Parse Profile\n");
	else
		pr_info("Parse Profile %d created\n", *prpid);
	return ret;
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
		if (mem_pid[i] == MEM_PART_SYSTEM_DDR ||
		    mem_pid[i] == MEM_PART_DP_DDR) {
			pools_params.pools[i].backup_pool = 1;
			pools_params.pools[i].priority_mask =
				(uint8_t)(g_app_params.dpni_pool_pri_mask >> 8);
		} else {
			pools_params.pools[i].priority_mask =
				(uint8_t)g_app_params.dpni_pool_pri_mask;
		}
	}
	return 0;
}

#pragma warning_errors on
ASSERT_EQUAL(sizeof(struct dpni_tx_priorities_cfg), \
	sizeof(struct dpni_drv_tx_selection));

ASSERT_EQUAL(DPNI_MAX_TC, DPNI_DRV_MAX_TC);

ASSERT_EQUAL(sizeof(struct dpni_tx_schedule_cfg), \
	sizeof(struct dpni_drv_tx_schedule));

ASSERT_EQUAL((DPNI_TX_SCHED_WEIGHTED_B - DPNI_TX_SCHED_STRICT_PRIORITY), \
	(DPNI_DRV_TX_SCHED_WEIGHTED_B - DPNI_DRV_TX_SCHED_STRICT_PRIORITY));
#pragma warning_errors off

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

static __COLD_CODE inline void dpni_drv_init_ni_table(uint8_t prpid,
						      uint8_t prpid_egr)
{
	int i;

	for (i = 0; i < SOC_MAX_NUM_OF_DPNI; i++) {
		struct dpni_drv * dpni_drv = nis + i;
		dpni_drv->dpni_id                          = DPNI_NOT_IN_USE;
		dpni_drv->dpni_drv_params_var.spid         = 0;
		dpni_drv->dpni_drv_params_var.spid_ddr     = 0;
		dpni_drv->dpni_drv_params_var.epid_idx     = 0;
		/* Parser profile IDs from parser_profile_init()*/
		dpni_drv->dpni_drv_params_var.prpid        = prpid;
		dpni_drv->dpni_drv_params_var.prpid_egress = prpid_egr;
		/*ETH HXS */
		dpni_drv->dpni_drv_params_var.starting_hxs = 0;
		dpni_drv->dpni_drv_tx_params_var.qdid      = 0;
		dpni_drv->dpni_drv_params_var.flags        =
			DPNI_DRV_FLG_PARSE | DPNI_DRV_FLG_PARSER_DIS;
		/* Flow Steering */
		clear_fs_table((uint16_t)i);
	}
}

__COLD_CODE int dpni_drv_init(void)
{
	uint8_t		prpid = 0, prpid_egr = 0;
	int		err;

	memset(&pools_params, 0, sizeof(struct dpni_pools_cfg));
	num_of_nis = 0;
	/* Allocate internal AIOP NI table */
	nis = fsl_malloc(sizeof(struct dpni_drv) * SOC_MAX_NUM_OF_DPNI, 64);
	if (!nis) {
		return -ENOMEM;
	}
	/* Create and initialize the "ingress" parse profile */
	err = parser_profile_init(&prpid);
	if (err) {
		pr_err("[%d] : Ingress parser profile initialization\n", err);
		return err;
	}
	pr_info("AIOP Ingress parse profile ID : %d\n", prpid);
	/* Create and initialize the "egress" parse profile */
	err = parser_profile_init(&prpid_egr);
	if (err) {
		pr_err("[%d] : Egress parser profile initialization\n", err);
		return err;
	}
	egress_parse_profile_id = prpid_egr;
	pr_info("AIOP Egress parse profile ID : %d\n", egress_parse_profile_id);
	/* Initialize internal AIOP NI table */
	dpni_drv_init_ni_table(prpid, prpid_egr);
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

int dpni_drv_get_ordering_mode_etype(uint16_t ni_id, uint16_t etype)
{
	uint32_t ep_osc = (uint32_t)(-ENOENT);
	uint8_t pos;
	uint32_t epid;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/* write epid index to epas register */
	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);

	pos = get_fs_entry(ni_id, etype);
	if (pos != DPNI_FS_TABLE_SIZE) {
		epid = nis[ni_id].dpni_drv_params_var.epid_idx;
		epid += (pos + 1) * (uint32_t)SOC_MAX_NUM_OF_DPNI;
		iowrite32_ccsr(epid, &wrks_addr->epas);
		/* read ep_osc - to get the order scope */
		ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
		ep_osc = (ep_osc & ORDER_MODE_BIT_MASK) >> 24;
	}

	/*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)nis);
	EP_MNG_MUTEX_RELEASE;

	return (int)ep_osc;
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

static int dpni_drv_set_ordering_mode_etype(uint16_t ni_id, int ep_mode,
					    uint16_t etype) {
	int ret = 0;
	uint8_t pos;
	uint32_t epid;
	uint32_t ep_osc;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/*Mutex lock to avoid race condition while writing to EPID table*/
	EP_MNG_MUTEX_W_TAKE;
	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);

	pos = get_fs_entry(ni_id, etype);
	if (pos != DPNI_FS_TABLE_SIZE) {
		epid = nis[ni_id].dpni_drv_params_var.epid_idx;
		epid += (pos + 1) * (uint32_t)SOC_MAX_NUM_OF_DPNI;
		/* write epid index to epas register */
		iowrite32_ccsr(epid, &wrks_addr->epas);
		/* read ep_osc - to get the order scope */
		ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
		ep_osc &= ORDER_MODE_CLEAR_BIT;
		ep_osc |= (ep_mode & 0x01) << 24;
		/*Set concurrent mode for NI in epid table*/
		iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);
	} else {
		ret = -ENOENT;
	}

	/*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)nis);
	EP_MNG_MUTEX_RELEASE;
	return ret;
}

/******************************************************************************/
int dpni_drv_set_concurrent(uint16_t ni_id){
	return dpni_drv_set_ordering_mode(ni_id, DPNI_DRV_CONCURRENT_MODE);
}

int dpni_drv_set_exclusive(uint16_t ni_id){
	return dpni_drv_set_ordering_mode(ni_id, DPNI_DRV_EXCLUSIVE_MODE);
}

int dpni_drv_set_concurrent_etype(uint16_t ni, uint16_t etype)
{
	return dpni_drv_set_ordering_mode_etype(ni, DPNI_DRV_CONCURRENT_MODE,
						etype);
}

int dpni_drv_set_exclusive_etype(uint16_t ni, uint16_t etype)
{
	return dpni_drv_set_ordering_mode_etype(ni, DPNI_DRV_EXCLUSIVE_MODE,
						etype);
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
	uint32_t ep_osc, epid;
	uint16_t dpni;
	uint8_t i;

	/*Mutex will be needed if the function will be supported in run time*/
	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK);
	epid = nis[ni_id].dpni_drv_params_var.epid_idx;
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_get_attributes(&dprc->io, 0, dpni, &attr);
	if (err) {
		sl_pr_err("dpni_get_attributes failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	cdma_mutex_lock_take((uint64_t)order_scope_buf, CDMA_MUTEX_WRITE_LOCK);
	memset(order_scope_buf, 0, PARAMS_IOVA_BUFF_SIZE);

	if (key_cfg == DPNI_DRV_NO_ORDER_SCOPE)
	{
		/*Mutex lock to avoid race condition while writing to EPID table*/
		/*Mutex will be needed if the function will be supported in run time*/
		EP_MNG_MUTEX_W_TAKE;
		/* write epid index to epas register */
		iowrite32_ccsr(epid, &wrks_addr->epas);
		/* Read ep_osc from EPID table */
		ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
		/* src = 0 - No order scope specified. Task does not enter a scope,
		 * ep = 0 - executing concurrently,
		 * sel = 0,
		 * osrm = 0 - all frames enter scope 0 */
		ep_osc &= ORDER_MODE_NO_ORDER_SCOPE;
		/*Write ep_osc to EPID table*/
		iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);

		/* FS entries */
		for (i = 0; i < DPNI_FS_TABLE_SIZE; i++) {
			epid += SOC_MAX_NUM_OF_DPNI;
			iowrite32_ccsr(epid, &wrks_addr->epas);
			ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
			ep_osc &= ORDER_MODE_NO_ORDER_SCOPE;
			iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);
		}
		EP_MNG_MUTEX_RELEASE;
		cfg.dist_mode = DPNI_DIST_MODE_NONE;
	}
	else
	{
		dpni_prepare_key_cfg(key_cfg, order_scope_buf);
		cfg.dist_mode = DPNI_DIST_MODE_HASH;
	}

	cfg.dist_size = DPNI_DEFAULT_DIST_SIZE;
	cfg.key_cfg_iova = (uint64_t)order_scope_buf;
	for (i = 0; i < attr.num_rx_tcs; i++) {
		err = dpni_set_rx_tc_dist(&dprc->io, 0, dpni, i, &cfg);
		if (err) {
			sl_pr_err("dpni_set_rx_tc_dist failed\n");
			dpni_close(&dprc->io, 0, dpni);
			cdma_mutex_lock_release((uint64_t)order_scope_buf);
			return err;
		}
	}

	cdma_mutex_lock_release((uint64_t)order_scope_buf);
	err = dpni_close(&dprc->io, 0, dpni);
	if(err){
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

__COLD_CODE int dpni_drv_enable_etype_fs(uint16_t ni_id, uint16_t etype)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_attr attr = {0};
	int err;
	uint16_t dpni;
	uint8_t i;
	struct dpni_rule_cfg cfg;
	struct dpni_fs_action_cfg act;
	uint16_t epid;
	struct dpkg_profile_cfg key_cfg;
	struct dpni_rx_tc_dist_cfg dist_cfg = {0};
	uint8_t pos;

	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK);
	epid = nis[ni_id].dpni_drv_params_var.epid_idx;

	pos = get_fs_entry(ni_id, etype);
	if (pos != DPNI_FS_TABLE_SIZE) {
		cdma_mutex_lock_release((uint64_t)nis);
		return -EEXIST;
	}
	pos = add_fs_entry(ni_id, etype);
	if (pos == DPNI_FS_TABLE_SIZE) {
		cdma_mutex_lock_release((uint64_t)nis);
		return -ENOSPC;
	}
	epid += (pos + 1) * (uint16_t)SOC_MAX_NUM_OF_DPNI;

	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_get_attributes(&dprc->io, 0, dpni, &attr);
	if (err) {
		sl_pr_err("dpni_get_attributes failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	cdma_mutex_lock_take((uint64_t)order_scope_buf, CDMA_MUTEX_WRITE_LOCK);
	memset(order_scope_buf, 0, PARAMS_IOVA_BUFF_SIZE);

	/* Create the key generation profile */
	memset(&key_cfg, 0x0, sizeof(struct dpkg_profile_cfg));
	key_cfg.num_extracts = 1;
	key_cfg.extracts[0].type = DPKG_EXTRACT_FROM_HDR;
	key_cfg.extracts[0].extract.from_hdr.prot = NET_PROT_ETH;
	key_cfg.extracts[0].extract.from_hdr.type = DPKG_FULL_FIELD;
	key_cfg.extracts[0].extract.from_hdr.field = NET_HDR_FLD_ETH_TYPE;
	dpni_prepare_key_cfg(&key_cfg, order_scope_buf);

	dist_cfg.dist_size = DPNI_DEFAULT_DIST_SIZE;
	dist_cfg.dist_mode = DPNI_DIST_MODE_FS;
	dist_cfg.fs_cfg.keep_hash_key = 1;
	dist_cfg.fs_cfg.miss_action = DPNI_FS_MISS_HASH;
	dist_cfg.key_cfg_iova = (uint64_t)order_scope_buf;

	for (i = 0; i < attr.num_rx_tcs; i++) {
		/* set the key only once */
		if (pos != 0)
			break;
		err = dpni_set_rx_tc_dist(&dprc->io, 0, dpni, i, &dist_cfg);
		if (err) {
			sl_pr_err("dpni_set_rx_tc_dist failed\n");
			dpni_close(&dprc->io, 0, dpni);
			cdma_mutex_lock_release((uint64_t)order_scope_buf);
			return err;
		}
	}

	cfg.key_size = 2;
	cfg.key_iova = (uint64_t)order_scope_buf;
	cfg.mask_iova = NULL;

	/* EtherType */
	order_scope_buf[0] = (uint8_t)(etype >> 8);
	order_scope_buf[1] = (uint8_t)(etype & 0xFF);

	act.flow_id = 0;
	act.options = DPNI_FS_OPT_SET_FLC;
	act.flc = epid;
	for (i = 0; i < attr.num_rx_tcs; i++) {
		err = dpni_add_fs_entry(&dprc->io, 0, dpni, i, 0, &cfg, &act);
		if (err) {
			sl_pr_err("dpni_add_fs_entry failed\n");
			dpni_close(&dprc->io, 0, dpni);
			cdma_mutex_lock_release((uint64_t)order_scope_buf);
			return err;
		}
	}
	cdma_mutex_lock_release((uint64_t)order_scope_buf);

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
	dpni_layout.pass_sw_opaque = layout->pass_sw_opaque;
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
	layout->pass_sw_opaque = dpni_layout.pass_sw_opaque;
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

int dpni_drv_register_rx_buffer_layout_requirements(uint16_t head_room,
						    uint16_t tail_room,
						    uint16_t private_data_size,
						    uint32_t frame_anno)
{
	g_dpni_early_init_data.count++;

	g_dpni_early_init_data.head_room_sum += head_room;
	g_dpni_early_init_data.tail_room_sum += tail_room;
	if (frame_anno)
		g_dpni_early_init_data.frame_anno = frame_anno;

	if (private_data_size) {
		pr_warn("Private data size is set to %d bytes\n",
			DPNI_DRV_PTA_SIZE);
		g_dpni_early_init_data.private_data_size_sum =
			DPNI_DRV_PTA_SIZE;
	}
	return 0;
}

int dpni_drv_get_counter(uint16_t ni_id, enum dpni_drv_counter counter,
			 uint64_t *value)
{
	struct mc_dprc		*dprc;
	int			err;
	uint16_t		dpni;
	union dpni_statistics	stats;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		sl_pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	 /* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_statistics(&dprc->io, 0, dpni,
				  dpni_statistics[counter].page, 0, &stats);
	if (err) {
		sl_pr_err("dpni_get_counter failed\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("Close DPNI failed\n");
		return err;
	}
	*value = stats.raw.counter[dpni_statistics[counter].offset];
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
int dpni_drv_get_qos_counter(uint16_t ni_id, uint8_t tc,
			     enum dpni_drv_qos_counter counter,
			     uint64_t *value)
{
	struct mc_dprc		*dprc;
	int			err;
	uint16_t		dpni;
	union dpni_statistics	stats;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		sl_pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	 /* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_statistics(&dprc->io, 0, dpni,
				  dpni_qos_statistics[counter].page,
				  tc, &stats);
	if (err) {
		sl_pr_err("dpni_get_counter failed\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("Close DPNI failed\n");
		return err;
	}
	*value = stats.raw.counter[dpni_qos_statistics[counter].offset];
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
int dpni_drv_get_statistics(uint16_t ni_id, uint8_t page, uint8_t param,
				dpni_drv_statistics *stat)
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

	dpni_get_statistics(&dprc->io, 0, dpni, page, param, stat);
	if(err){
		sl_pr_err("dpni_get_statistics failed\n");
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

/******************************************************************************/
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

int dpni_drv_get_dpni_label(uint16_t ni_id, char **dpni_label)
{
	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	if (ni_id >= (uint16_t)num_of_nis ||
	    nis[ni_id].dpni_id == DPNI_NOT_IN_USE) {
		cdma_mutex_lock_release((uint64_t)nis); /*Unlock dpni table*/
		sl_pr_err("AIOP NI ID %d not exist\n", ni_id);
		return -ENAVAIL;
	}

	*dpni_label = &nis[ni_id].dpni_label[0];
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

/******************************************************************************/
int dpni_drv_set_link_cfg(uint16_t ni_id, struct dpni_drv_link_cfg *cfg)
{
	struct mc_dprc		*dprc;
	struct dpni_link_cfg	link_cfg;
	int			err;
	uint16_t		dpni;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	 /* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	 /* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	link_cfg.options = cfg->options;
	link_cfg.rate = cfg->rate;
	err = dpni_set_link_cfg(&dprc->io, 0, dpni, &link_cfg);
	if (err) {
		sl_pr_err("dpni_set_link_cfg failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
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

int dpni_drv_get_initial_presentation_etype(
	uint16_t ni_id,
	struct ep_init_presentation * const init_presentation,
	uint16_t etype) {
	uint16_t epid;
	uint8_t pos;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	epid = nis[ni_id].dpni_drv_params_var.epid_idx;
	pos = get_fs_entry(ni_id, etype);
	/*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)nis);

	if (pos == DPNI_FS_TABLE_SIZE)
		return -ENOENT;

	epid += (pos + 1) * (uint32_t)SOC_MAX_NUM_OF_DPNI;
	return ep_mng_get_initial_presentation(epid, init_presentation);
}

int dpni_drv_set_initial_presentation_etype(
	uint16_t ni_id,
	const struct ep_init_presentation * const init_presentation,
	uint16_t etype) {
	uint16_t epid;
	uint8_t pos;

	/*Lock dpni table*/
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	epid = nis[ni_id].dpni_drv_params_var.epid_idx;
	pos = get_fs_entry(ni_id, etype);
	/*Unlock dpni table*/
	cdma_mutex_lock_release((uint64_t)nis);

	if (pos == DPNI_FS_TABLE_SIZE)
		return -ENOENT;

	epid += (pos + 1) * (uint32_t)SOC_MAX_NUM_OF_DPNI;
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

/******************************************************************************/
__COLD_CODE int dpni_drv_set_tx_shaping(uint16_t ni_id,
					struct dpni_drv_tx_shaping *cr_cfg,
					struct dpni_drv_tx_shaping *er_cfg,
					uint8_t coupled)
{
	struct mc_dprc	*dprc;
	int		err;
	uint16_t	dpni;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_tx_shaping(&dprc->io, 0, dpni,
				  (const struct dpni_tx_shaping_cfg *)cr_cfg,
				  (const struct dpni_tx_shaping_cfg *)er_cfg,
				  (int)coupled);
	if (err) {
		sl_pr_err("dpni_set_tx_shaping failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
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

	early_drop_cfg.enable =
		(cfg->mode == DPNI_DRV_EARLY_DROP_MODE_NONE) ? 0 : 1;
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

int dpni_drv_set_errors_behavior(uint16_t ni_id,
				 const struct dpni_drv_error_cfg *cfg)
{
	struct mc_dprc		*dprc;
	int			err;
	uint16_t		dpni;
	struct dpni_error_cfg	err_cfg;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	err_cfg.error_action = (enum dpni_error_action)cfg->error_action;
	err_cfg.errors = cfg->errors;
	/* Set to '1' to mark the errors in frame annotation status (FAS).
	 * Relevant only for the non-discard action */
	err_cfg.set_frame_annotation =
		cfg->errors != DPNI_DRV_ERR_ACTION_DISCARD ? 1 : 0;
	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	 /* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_errors_behavior(&dprc->io, 0, dpni, &err_cfg);
	if (err) {
		sl_pr_err("dpni_set_errors_behavior failed\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("Close DPNI failed\n");
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
__COLD_CODE void dpni_drv_get_ingress_parse_profile_id(uint16_t ni_id,
						       uint8_t *prpid)
{
	struct dpni_drv		*dpni_drv;

	dpni_drv = nis + ni_id;
	*prpid = dpni_drv->dpni_drv_params_var.prpid;
}

/******************************************************************************/
__COLD_CODE void dpni_drv_get_egress_parse_profile_id(uint16_t ni_id,
						      uint8_t *prpid)
{
	struct dpni_drv		*dpni_drv;

	dpni_drv = nis + ni_id;
	*prpid = dpni_drv->dpni_drv_params_var.prpid_egress;
}

/******************************************************************************/
static __COLD_CODE
int dpni_drv_enable_soft_parser(uint8_t prpid,
				const struct dpni_drv_sparser_param *param)
{
	uint8_t				*pb;
	struct parse_profile_input	pp __attribute__((aligned(16)));

	#define SOFT_PARSER_EN		0x8000
	#define SOFT_PARSER_SSS_MASK	0x7FF

	ASSERT_COND(param);
	if (param->param_array && param->param_size &&
	    param->param_offset + param->param_size > 64) {
		pr_err("Invalid Parameters Array : off = %d, size = %d\n",
		       param->param_offset, param->param_size);
		return -EINVAL;
	}
#ifdef LS2085A_REV1
	if (param->custom_header_first) {
		pr_err("First header as custom header not supported\n");
		return -EINVAL;
	}
#endif
	/* The first header is not linked to a hard HXS. If it has no parameters
	 * there is no need to update the parse profile. */
	if (param->custom_header_first && !param->param_size)
		return 0;
	memset(&pp, 0, sizeof(struct parse_profile_input));
	parser_profile_query(prpid, &pp);
	if (!param->custom_header_first) {
		uint16_t	*pval;

		switch (param->link_to_hard_hxs) {
		case PARSER_ETH_STARTING_HXS:
			pval = &pp.parse_profile.eth_hxs_config;
			break;
		case PARSER_LLC_SNAP_STARTING_HXS:
			 pval = &pp.parse_profile.llc_snap_hxs_config;
			 break;
		case PARSER_VLAN_STARTING_HXS:
			pval = &pp.parse_profile.vlan_hxs_config.
					en_erm_soft_seq_start;
			break;
		case PARSER_PPPOE_PPP_STARTING_HXS:
			pval = &pp.parse_profile.pppoe_ppp_hxs_config;
			break;
		case PARSER_MPLS_STARTING_HXS:
			pval = &pp.parse_profile.mpls_hxs_config.
					en_erm_soft_seq_start;
			break;
		case PARSER_ARP_STARTING_HXS:
			pval = &pp.parse_profile.arp_hxs_config;
			break;
		case PARSER_IP_STARTING_HXS:
			pval = &pp.parse_profile.ip_hxs_config;
			break;
#ifndef LS2085A_REV1
		case PARSER_IPV4_STARTING_HXS:
			pval = &pp.parse_profile.ipv4_hxs_config;
			break;
		case PARSER_IPV6_STARTING_HXS:
			pval = &pp.parse_profile.ipv6_hxs_config;
			break;
#endif
		case PARSER_GRE_STARTING_HXS:
			pval = &pp.parse_profile.gre_hxs_config;
			break;
		case PARSER_MINENCAP_STARTING_HXS:
			pval = &pp.parse_profile.minenc_hxs_config;
			break;
		case PARSER_OTHER_L3_SHELL_STARTING_HXS:
			pval = &pp.parse_profile.other_l3_shell_hxs_config;
			break;
		case PARSER_TCP_STARTING_HXS:
			pval = &pp.parse_profile.tcp_hxs_config;
			break;
		case PARSER_UDP_STARTING_HXS:
			pval = &pp.parse_profile.udp_hxs_config;
			break;
		case PARSER_IPSEC_STARTING_HXS:
			pval = &pp.parse_profile.ipsec_hxs_config;
			break;
		case PARSER_SCTP_STARTING_HXS:
			pval = &pp.parse_profile.sctp_hxs_config;
			break;
		case PARSER_DCCP_STARTING_HXS:
			pval = &pp.parse_profile.dccp_hxs_config;
			break;
		case PARSER_OTHER_L4_SHELL_STARTING_HXS:
			pval = &pp.parse_profile.other_l4_shell_hxs_config;
			break;
		case PARSER_GTP_STARTING_HXS:
			pval = &pp.parse_profile.gtp_hxs_config;
			break;
		case PARSER_ESP_STARTING_HXS:
			pval = &pp.parse_profile.esp_hxs_config;
			break;
#ifndef LS2085A_REV1
		case PARSER_VXLAN_STARTING_HXS:
			pval = &pp.parse_profile.vxlan_hxs_config;
			break;
#endif
		case PARSER_L5_SHELL_STARTING_HXS:
			pval = &pp.parse_profile.l5_shell_hxs_config;
			break;
		default:
		case PARSER_FINAL_SHELL_STARTING_HXS:
			pval = &pp.parse_profile.final_shell_hxs_config;
			break;
		}
		/* Enable SP and set the starting PC */
		*pval &= ~SOFT_PARSER_SSS_MASK;
		*pval |= SOFT_PARSER_EN | (param->start_pc &
					   SOFT_PARSER_SSS_MASK);
	}
	/* Copy SP parameters (if exist) */
	if (param->param_array && param->param_size) {
		pb = (uint8_t *)
			&pp.parse_profile.soft_examination_param_array[0];
		pb += param->param_offset;
		memcpy(pb, param->param_array, param->param_size);
	}
	parser_profile_replace(&pp, prpid);
	pr_info("Parse Profile %d updated\n", prpid);
	return 0;
}

/******************************************************************************/
__COLD_CODE int dpni_drv_enable_ingress_soft_parser
			(const struct dpni_drv_sparser_param *param)
{
	uint8_t		prpid;
	int		ret;

	pr_info("Enable soft parser at PC = 0x%x\n", param->start_pc);
	dpni_drv_get_ingress_parse_profile_id(0, &prpid);
	ret = sparser_drv_check_params(param->start_pc, param->param_size,
				       param->param_offset, prpid, 1);
	if (ret)
		return ret;
	return dpni_drv_enable_soft_parser(prpid, param);
}

/******************************************************************************/
__COLD_CODE int dpni_drv_enable_egress_soft_parser
			(const struct dpni_drv_sparser_param *param)
{
	uint8_t		prpid;
	int		ret;

	pr_info("Enable soft parser at PC = 0x%x\n", param->start_pc);
	dpni_drv_get_egress_parse_profile_id(0, &prpid);
	ret = sparser_drv_check_params(param->start_pc, param->param_size,
				       param->param_offset, prpid, 0);
	if (ret)
		return ret;
	return dpni_drv_enable_soft_parser(prpid, param);
}

/******************************************************************************/
static __COLD_CODE int load_wriop_soft_parser(struct dpni_load_ss_cfg *cfg)
{
	struct mc_dprc			*dprc;
	int				err;
	uint16_t			dpni, ni_id;

	/* Use AIOP NI 0 to send the information */
	ni_id = 0;
	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_load_sw_sequence(&dprc->io, 0, dpni, cfg);
	if (err) {
		sl_pr_err("dpni_load_sw_sequence failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
static __COLD_CODE int enable_wriop_soft_parser(uint16_t ni_id,
						struct dpni_enable_ss_cfg *cfg)
{
	struct mc_dprc			*dprc;
	int				err;
	uint16_t			dpni;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_enable_sw_sequence(&dprc->io, 0, dpni, cfg);
	if (err) {
		sl_pr_err("dpni_enable_sw_sequence failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
__COLD_CODE int dpni_drv_load_wriop_ingress_soft_parser
			(const struct dpni_drv_sparser_param *param)
{
	struct dpni_load_ss_cfg		cfg;

	pr_info("Load soft parser at PC = 0x%x\n", param->start_pc);
	cfg.dest = DPNI_SS_INGRESS;
	cfg.ss_offset = param->start_pc;
	cfg.ss_size = param->size;
	cfg.ss_iova = (uint64_t)param->byte_code;

	return load_wriop_soft_parser(&cfg);
}

/******************************************************************************/
__COLD_CODE int dpni_drv_load_wriop_egress_soft_parser
			(const struct dpni_drv_sparser_param *param)
{
	struct dpni_load_ss_cfg		cfg;

	pr_info("Load soft parser at PC = 0x%x\n", param->start_pc);
	cfg.dest = DPNI_SS_EGRESS;
	cfg.ss_offset = param->start_pc;
	cfg.ss_size = param->size;
	cfg.ss_iova = (uint64_t)param->byte_code;

	return load_wriop_soft_parser(&cfg);
}

/******************************************************************************/
__COLD_CODE int dpni_drv_enable_wriop_ingress_soft_parser
		(uint16_t ni_id, const struct dpni_drv_sparser_param *param)
{
	struct dpni_enable_ss_cfg	cfg;
	int ret;

	pr_info("Enable soft parser at PC = 0x%x on DPNI_%d\n", param->start_pc,
		ni_id);
	cfg.dest = DPNI_SS_INGRESS;
	cfg.ss_offset = param->start_pc;
	cfg.set_start = param->custom_header_first;
	cfg.hxs = (uint16_t)param->link_to_hard_hxs;
	cfg.param_offset = param->param_offset;
	cfg.param_size = param->param_size;
	cdma_mutex_lock_take((uint64_t)order_scope_buf, CDMA_MUTEX_WRITE_LOCK);
	if (cfg.param_size) {
		memset(order_scope_buf, 0, PARAMS_IOVA_BUFF_SIZE);
		memcpy(&order_scope_buf, param->param_array, cfg.param_size);
		cfg.param_iova = (uint64_t)&order_scope_buf[0];
	} else {
		cfg.param_iova = 0;
	}
	ret = enable_wriop_soft_parser(ni_id, &cfg);
	cdma_mutex_lock_release((uint64_t)order_scope_buf);
	return ret;
}

/******************************************************************************/
__COLD_CODE int dpni_drv_enable_wriop_egress_soft_parser
		(uint16_t ni_id, const struct dpni_drv_sparser_param *param)
{
	struct dpni_enable_ss_cfg	cfg;
	int ret;

	pr_info("Enable soft parser at PC = 0x%x on DPNI_%d\n", param->start_pc,
		ni_id);
	cfg.dest = DPNI_SS_EGRESS;
	cfg.ss_offset = param->start_pc;
	cfg.set_start = param->custom_header_first;
	cfg.hxs = (uint16_t)param->link_to_hard_hxs;
	cfg.param_offset = param->param_offset;
	cfg.param_size = param->param_size;
	cdma_mutex_lock_take((uint64_t)order_scope_buf, CDMA_MUTEX_WRITE_LOCK);
	if (cfg.param_size) {
		memset(order_scope_buf, 0, PARAMS_IOVA_BUFF_SIZE);
		memcpy(&order_scope_buf, param->param_array, cfg.param_size);
		cfg.param_iova = (uint64_t)&order_scope_buf[0];
	} else {
		cfg.param_iova = 0;
	}
	ret = enable_wriop_soft_parser(ni_id, &cfg);
	cdma_mutex_lock_release((uint64_t)order_scope_buf);
	return ret;
}

/******************************************************************************/
__COLD_CODE static int dpni_drv_set_taildrop(uint16_t ni_id,
					enum dpni_queue_type qtype, uint8_t tc,
					struct dpni_drv_taildrop *taildrop)
{
	struct mc_dprc			*dprc;
	int				err;
	uint16_t			dpni;
	struct dpni_taildrop		mc_td;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	/* Only DPNI_CP_GROUP is supported for Tx queues. Queues index value
	 * is ignored */
	mc_td.enable = (char)taildrop->enable;
	mc_td.units = (enum dpni_congestion_unit)taildrop->units;
	mc_td.threshold = taildrop->threshold;
	/* Bring the OAL values into the allowed range */
	if (taildrop->oal < -2048)
		taildrop->oal = -2048;
	else if (taildrop->oal > 2047)
		taildrop->oal = 2047;
	mc_td.oal = taildrop->oal;
	err = dpni_set_taildrop(&dprc->io, 0, dpni, DPNI_CP_GROUP,
				qtype, tc, 0, &mc_td);
	if (err) {
		sl_pr_err("dpni_set_taildrop failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
__COLD_CODE static int dpni_drv_get_taildrop(uint16_t ni_id,
					enum dpni_queue_type qtype, uint8_t tc,
					struct dpni_drv_taildrop *taildrop)
{
	struct mc_dprc			*dprc;
	int				err;
	uint16_t			dpni;
	struct dpni_taildrop		mc_td;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	/* Only DPNI_CP_GROUP is supported for Tx queues. Queues index value
	 * is ignored */
	err = dpni_get_taildrop(&dprc->io, 0, dpni, DPNI_CP_GROUP,
				qtype, tc, 0, &mc_td);
	if (err) {
		sl_pr_err("dpni_get_taildrop failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	taildrop->enable = (uint8_t)mc_td.enable;
	taildrop->units = (enum dpni_drv_congestion_unit)mc_td.units;
	taildrop->threshold = mc_td.threshold;
	/* OAL should be converted into a integer value from 12-bit unsigned
	 * value. */
	#define SIGN_BIT_12_BIT_VAL	0x800
	if (mc_td.oal & SIGN_BIT_12_BIT_VAL)
		taildrop->oal = (int16_t)(mc_td.oal | 0xF800);
	else
		taildrop->oal = (int16_t)mc_td.oal;
	return 0;
}

/******************************************************************************/
int dpni_drv_set_tx_taildrop(uint16_t ni_id, uint8_t tc,
				struct dpni_drv_taildrop *taildrop)
{
	return dpni_drv_set_taildrop(ni_id, DPNI_QUEUE_TX, tc, taildrop);
}

/******************************************************************************/
int dpni_drv_get_tx_taildrop(uint16_t ni_id, uint8_t tc,
				struct dpni_drv_taildrop *taildrop)
{
	return dpni_drv_get_taildrop(ni_id, DPNI_QUEUE_TX, tc, taildrop);
}

/******************************************************************************/
__COLD_CODE static int dpni_drv_set_early_drop(uint16_t ni_id,
					enum dpni_queue_type qtype, uint8_t tc,
					struct dpni_drv_early_drop *cfg)
{
	struct mc_dprc			*dprc;
	int				err;
	uint16_t			dpni;
	struct dpni_early_drop_cfg	mc_cfg;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	/* Prepare Extension structure in IOVA */
	cdma_mutex_lock_take((uint64_t)order_scope_buf, CDMA_MUTEX_WRITE_LOCK);
	memset(order_scope_buf, 0, PARAMS_IOVA_BUFF_SIZE);

	mc_cfg.enable = cfg->enable;
	mc_cfg.units = (enum dpni_congestion_unit)cfg->units;
	mc_cfg.green.max_threshold = cfg->green.max_threshold;
	mc_cfg.green.min_threshold = cfg->green.min_threshold;
	mc_cfg.green.drop_probability = cfg->green.drop_probability;
	mc_cfg.yellow.max_threshold = cfg->yellow.max_threshold;
	mc_cfg.yellow.min_threshold = cfg->yellow.min_threshold;
	mc_cfg.yellow.drop_probability = cfg->yellow.drop_probability;
	mc_cfg.red.max_threshold = cfg->red.max_threshold;
	mc_cfg.red.min_threshold = cfg->red.min_threshold;
	mc_cfg.red.drop_probability = cfg->red.drop_probability;

	dpni_prepare_early_drop(&mc_cfg, order_scope_buf);
	err = dpni_set_early_drop(&dprc->io, 0, dpni, qtype, tc,
				  (uint64_t)&order_scope_buf[0]);
	cdma_mutex_lock_release((uint64_t)order_scope_buf);
	if (err) {
		sl_pr_err("dpni_set_early_drop failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
__COLD_CODE static int dpni_drv_get_early_drop(uint16_t ni_id,
					enum dpni_queue_type qtype, uint8_t tc,
					struct dpni_drv_early_drop *cfg)
{
	struct mc_dprc			*dprc;
	int				err;
	uint16_t			dpni;
	struct dpni_early_drop_cfg	mc_cfg;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	cdma_mutex_lock_take((uint64_t)order_scope_buf, CDMA_MUTEX_WRITE_LOCK);
	err = dpni_get_early_drop(&dprc->io, 0, dpni, qtype, tc,
				  (uint64_t)&order_scope_buf);
	/* Get Extension structure from IOVA */
	dpni_extract_early_drop(&mc_cfg, order_scope_buf);
	cdma_mutex_lock_release((uint64_t)order_scope_buf);

	if (err) {
		sl_pr_err("dpni_get_early_drop failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}

	cfg->enable = mc_cfg.enable;
	cfg->units = (enum dpni_drv_congestion_unit)mc_cfg.units;
	cfg->green.max_threshold = mc_cfg.green.max_threshold;
	cfg->green.min_threshold = mc_cfg.green.min_threshold;
	cfg->green.drop_probability = mc_cfg.green.drop_probability;
	cfg->yellow.max_threshold = mc_cfg.yellow.max_threshold;
	cfg->yellow.min_threshold = mc_cfg.yellow.min_threshold;
	cfg->yellow.drop_probability = mc_cfg.yellow.drop_probability;
	cfg->red.max_threshold = mc_cfg.red.max_threshold;
	cfg->red.min_threshold = mc_cfg.red.min_threshold;
	cfg->red.drop_probability = mc_cfg.red.drop_probability;
	return 0;
}

/******************************************************************************/
int dpni_drv_set_tx_early_drop(uint16_t ni_id, uint8_t tc,
				struct dpni_drv_early_drop *cfg)
{
	return dpni_drv_set_early_drop(ni_id, DPNI_QUEUE_TX, tc, cfg);
}

/******************************************************************************/
int dpni_drv_get_tx_early_drop(uint16_t ni_id, uint8_t tc,
				struct dpni_drv_early_drop *cfg)
{
	return dpni_drv_get_early_drop(ni_id, DPNI_QUEUE_TX, tc, cfg);
}

/******************************************************************************/
int dpni_drv_set_rx_taildrop(uint16_t ni_id, uint8_t tc,
				struct dpni_drv_taildrop *taildrop)
{
	return dpni_drv_set_taildrop(ni_id, DPNI_QUEUE_RX, tc, taildrop);
}

/******************************************************************************/
int dpni_drv_get_rx_taildrop(uint16_t ni_id, uint8_t tc,
				struct dpni_drv_taildrop *taildrop)
{
	return dpni_drv_get_taildrop(ni_id, DPNI_QUEUE_RX, tc, taildrop);
}

/******************************************************************************/
int dpni_drv_set_rx_early_drop(uint16_t ni_id, uint8_t tc,
				struct dpni_drv_early_drop *cfg)
{
	return dpni_drv_set_early_drop(ni_id, DPNI_QUEUE_RX, tc, cfg);
}

/******************************************************************************/
int dpni_drv_get_rx_early_drop(uint16_t ni_id, uint8_t tc,
				struct dpni_drv_early_drop *cfg)
{
	return dpni_drv_get_early_drop(ni_id, DPNI_QUEUE_RX, tc, cfg);
}

/******************************************************************************/
int dpni_drv_set_pools(uint16_t ni_id, dpni_drv_pools_cfg *cfg)
{
	struct mc_dprc		*dprc;
	int			err;
	uint16_t		dpni;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	 /* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	 /* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_set_pools(&dprc->io, 0, dpni, cfg);
	if (err) {
		sl_pr_err("dpni_set_pools failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
int dpni_drv_set_congestion_notification(uint16_t ni_id, uint8_t tc,
			dpni_drv_queue_type qtype,
			struct dpni_drv_congestion_notification_cfg *cfg)
{
	struct mc_dprc		*dprc;
	int			err;
	uint16_t		dpni;
	struct dpni_congestion_notification_cfg cfg_mc;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	 /* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	 /* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	if (g_dpcon_id == -1) {
		sl_pr_err("dpni_set_congestion_notification: No AIOP DPCON\n");
		dpni_close(&dprc->io, 0, dpni);
		return -EINVAL;
	}
	cfg_mc.units = (enum dpni_congestion_unit)cfg->units;
	cfg_mc.threshold_entry = cfg->threshold_entry;
	cfg_mc.threshold_exit = cfg->threshold_exit;
	cfg_mc.message_ctx = 0;
	cfg_mc.message_iova = 0;
	cfg_mc.dest_cfg.dest_type = DPNI_DEST_DPCON;
	cfg_mc.dest_cfg.dest_id = g_dpcon_id;
	cfg_mc.dest_cfg.priority = MIN(tc, g_dpcon_priorities - 1);
	cfg_mc.notification_mode = cfg->notification_mode;
	err = dpni_set_congestion_notification(&dprc->io, 0, dpni, qtype, tc,
						&cfg_mc);
	if (err) {
		sl_pr_err("dpni_set_congestion_notification failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
int dpni_drv_get_congestion_notification(uint16_t ni_id, uint8_t tc,
			dpni_drv_queue_type qtype,
			struct dpni_drv_congestion_notification_cfg *cfg)
{
	struct mc_dprc		*dprc;
	int			err;
	uint16_t		dpni;
	struct dpni_congestion_notification_cfg cfg_mc;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}
	 /* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	 /* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_congestion_notification(&dprc->io, 0, dpni, qtype, tc,
						&cfg_mc);
	if (err) {
		sl_pr_err("dpni_get_congestion_notification failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	cfg->units = (enum dpni_drv_congestion_unit)cfg_mc.units;
	cfg->threshold_entry = cfg_mc.threshold_entry;
	cfg->threshold_exit = cfg_mc.threshold_exit;
	cfg->notification_mode = cfg_mc.notification_mode;
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
int dpni_drv_set_rx_priorities(uint16_t ni_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;
	uint8_t tc;
	uint8_t fq_idx;
	struct dpni_queue queue = {0};
	struct dpni_queue_id queue_id;
	struct dpni_attr attr = {0};

	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /* Unlock dpni table */

	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	err = dpni_get_attributes(&dprc->io, 0, dpni, &attr);
	if (err) {
		sl_pr_err("dpni_get_attributes failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}

	for (tc = 0; tc < attr.num_rx_tcs; tc++) {
		for (fq_idx = 0; fq_idx < DPNI_DEFAULT_DIST_SIZE; fq_idx++) {
			err = dpni_get_queue(&dprc->io, 0, dpni, DPNI_QUEUE_RX,
					tc, fq_idx, &queue, &queue_id);
			if (err) {
				sl_pr_err("dpni_get_queue failed\n");
				dpni_close(&dprc->io, 0, dpni);
				return err;
			}
			if (g_dpcon_id == -1) {
				queue.destination.priority = tc;
				queue.destination.type = DPNI_DEST_NONE;
				queue.destination.id = 0;
			} else {
				queue.destination.priority =
					MIN(tc, g_dpcon_priorities - 1);
				queue.destination.type = DPNI_DEST_DPCON;
				queue.destination.id = (uint16_t)g_dpcon_id;
			}
			err = dpni_set_queue(&dprc->io, 0, dpni, DPNI_QUEUE_RX,
				tc, fq_idx, DPNI_QUEUE_OPT_DEST, &queue);
			if (err) {
				sl_pr_err("dpni_set_queue failed\n");
				dpni_close(&dprc->io, 0, dpni);
				return err;
			}
		}
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return err;
}

/******************************************************************************/
int dpni_drv_get_attributes(uint16_t ni_id, dpni_drv_attr *attr) {
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	int err;
	uint16_t dpni;

	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	cdma_mutex_lock_release((uint64_t)nis); /* Unlock dpni table */

	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}
	err = dpni_get_attributes(&dprc->io, 0, dpni, attr);
	if (err) {
		sl_pr_err("dpni_get_attributes failed\n");
		dpni_close(&dprc->io, 0, dpni);
		return err;
	}
	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return err;
}

/******************************************************************************/
int dpni_drv_set_enable_tx_confirmation(uint16_t ni_id, int enable)
{
	struct mc_dprc *dprc;
	int err;
	uint16_t dpni;

	dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	if (!dprc) {
		pr_err("No AIOP container found\n");
		return -ENODEV;
	}

	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	 /* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);

	if (err) {
		sl_pr_err("Open DPNI failed\n");
		return err;
	}

	if (enable)
		err = dpni_set_tx_confirmation_mode(&dprc->io, 0, dpni,
						    DPNI_CONF_SINGLE);
	else
		err = dpni_set_tx_confirmation_mode(&dprc->io, 0, dpni,
						    DPNI_CONF_DISABLE);
	if (err) {
		sl_pr_err("set_tx_confirmation_mode failed\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("Close DPNI failed\n");
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("Close DPNI failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_set_tx_hw_annotation(uint16_t ni_id, uint32_t hw_anno)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_buffer_layout layout = {0};
	char dpni_ep_type[16];
	int dpni_ep_id, err, link_state;
	uint16_t dpni;

	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("dpni_open failed\n");
		return err;
	}
	/* Check if the given DPNI is on a recycle path */
	err = dpni_drv_get_connected_obj(ni_id, &dpni_ep_id,
					 dpni_ep_type, &link_state);
	if (err) {
		sl_pr_err("get_connected_obj failed\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("dpni_close failed\n");
		return err;
	}
	if (strcmp(dpni_ep_type, "dpni")) {
		sl_pr_err("Given NI is not connected to another DPNI\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("dpni_close failed\n");
		return -EINVAL;
	}
	/* Due to the way MC handles these settings, the layout.pass_timestamp
	 * field is unnecessary for this case, as both the status and timestamp
	 * passing are validated by the layout.pass_frame_status field. */
	if (hw_anno & DPNI_DRV_TX_HW_ANNOTATION_PASS_TS) {
		layout.pass_frame_status = 1;
		layout.pass_timestamp = 1;
	}
	if (hw_anno & DPNI_DRV_TX_HW_ANNOTATION_PASS_SW_OPAQUE)
		layout.pass_sw_opaque = 1;
	layout.options |= DPNI_DRV_BUF_LAYOUT_OPT_FRAME_STATUS |
			  DPNI_DRV_BUF_LAYOUT_OPT_TIMESTAMP |
			  DPNI_DRV_BUF_LAYOUT_OPT_SW_OPAQUE;
	err = dpni_set_buffer_layout(&dprc->io, 0, dpni,
				     DPNI_QUEUE_TX, &layout);
	if (err) {
		sl_pr_err("dpni_set_buffer_layout failed\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("dpni_close failed\n");
		return err;
	}

	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("dpni_close failed\n");
		return err;
	}
	return 0;
}

int dpni_drv_get_tx_hw_annotation(uint16_t ni_id, uint32_t *hw_anno)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);
	struct dpni_buffer_layout layout = {0};
	char dpni_ep_type[16];
	int dpni_ep_id, err, link_state;
	uint16_t dpni;

	/* Lock dpni table */
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_READ_LOCK);
	err = dpni_open(&dprc->io, 0, (int)nis[ni_id].dpni_id, &dpni);
	/* Unlock dpni table */
	cdma_mutex_lock_release((uint64_t)nis);
	if (err) {
		sl_pr_err("dpni_open failed\n");
		return err;
	}
	/* Check if the given DPNI is on a recycle path */
	err = dpni_drv_get_connected_obj(ni_id, &dpni_ep_id,
					 dpni_ep_type, &link_state);
	if (err) {
		sl_pr_err("get_connected_obj failed\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("dpni_close failed\n");
		return err;
	}
	if (strcmp(dpni_ep_type, "dpni")) {
		sl_pr_err("given NI is not connected to another DPNI\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("dpni_close failed\n");
		return -EINVAL;
	}
	/* Read the settings */
	err = dpni_get_buffer_layout(&dprc->io, 0, dpni,
				     DPNI_QUEUE_TX, &layout);
	if (err) {
		sl_pr_err("dpni_get_buffer_layout failed\n");
		if (dpni_close(&dprc->io, 0, dpni))
			sl_pr_err("dpni_close failed\n");
		return err;
	}
	*hw_anno = 0;
	if (layout.pass_frame_status || layout.pass_timestamp)
		*hw_anno |= DPNI_DRV_TX_HW_ANNOTATION_PASS_TS;
	if (layout.pass_sw_opaque)
		*hw_anno |= DPNI_DRV_TX_HW_ANNOTATION_PASS_SW_OPAQUE;

	err = dpni_close(&dprc->io, 0, dpni);
	if (err) {
		sl_pr_err("dpni_close failed\n");
		return err;
	}
	return 0;
}

#ifdef SL_DEBUG
/******************************************************************************/
__COLD_CODE void dpni_drv_dump_tx_taildrop(uint16_t ni_id, uint8_t tc_id)
{
	struct dpni_drv_taildrop		cfg;
	int					err;

	fsl_print("\n Tx Taildrop Configuration on DPNI %d, TC = %d\n",
		  ni_id, tc_id);
	err = dpni_drv_get_tx_taildrop(ni_id, tc_id, &cfg);
	if (err) {
		pr_err("\t [%d] : Cannot get taildrop configuration\n", err);
		return;
	}
	if (!cfg.enable) {
		fsl_print("\t Taildrop is disabled\n");
	} else {
		fsl_print("\t Congestion unit            : %s\n",
			  cfg.units == DPNI_DRV_CONGESTION_UNIT_BYTES ?
			  "Bytes" : "Frames");
		fsl_print("\t Taildrop threshold         : 0x%08x\n",
			  cfg.threshold);
		fsl_print("\t Overhead Accounting Length : %d\n",
			  cfg.oal);
	}
}

/******************************************************************************/
__COLD_CODE void dpni_drv_dump_tx_early_drop(uint16_t ni_id, uint8_t tc_id)
{
	struct dpni_drv_early_drop		cfg;
	int					err;

	fsl_print("\n Tx Early Drop Configuration on DPNI %d, TC = %d\n",
		  ni_id, tc_id);
	err = dpni_drv_get_tx_early_drop(ni_id, tc_id, &cfg);
	if (err) {
		pr_err("\t [%d] : Cannot get early drop configuration\n", err);
		return;
	}
	if (!cfg.enable) {
		fsl_print("\t Early drop is disabled\n");
	} else {
		fsl_print("\t Early drop mode            : WRED\n");
		fsl_print("\t Congestion unit            : %s\n",
			  cfg.units == DPNI_DRV_CONGESTION_UNIT_BYTES ?
			  "Bytes" : "Frames");
		fsl_print("\t Green maximum threshold    : 0x%x-%08x\n",
			  (uint32_t)(cfg.green.max_threshold >> 32),
			  (uint32_t)(cfg.green.max_threshold));
		fsl_print("\t Green minimum threshold    : 0x%x-%08x\n",
			  (uint32_t)(cfg.green.min_threshold >> 32),
			  (uint32_t)(cfg.green.min_threshold));
		fsl_print("\t Green drop probability     : %d\n",
			  cfg.green.drop_probability);
		fsl_print("\t Yellow maximum threshold   : 0x%x-%08x\n",
			  (uint32_t)(cfg.yellow.max_threshold >> 32),
			  (uint32_t)(cfg.yellow.max_threshold));
		fsl_print("\t Yellow minimum threshold   : 0x%x-%08x\n",
			  (uint32_t)(cfg.yellow.min_threshold >> 32),
			  (uint32_t)(cfg.yellow.min_threshold));
		fsl_print("\t Yellow drop probability    : %d\n",
			  cfg.yellow.drop_probability);
		fsl_print("\t Red maximum threshold      : 0x%x-%08x\n",
			  (uint32_t)(cfg.red.max_threshold >> 32),
			  (uint32_t)(cfg.red.max_threshold));
		fsl_print("\t Red minimum threshold      : 0x%x-%08x\n",
			  (uint32_t)(cfg.red.min_threshold >> 32),
			  (uint32_t)(cfg.red.min_threshold));
		fsl_print("\t Red drop probability       : %d\n",
			  cfg.red.drop_probability);
	}
}

void dpni_drv_dump_rx_buffer_layout(uint16_t ni_id)
{
	struct dpni_drv_buf_layout	*layout = NULL;
	int err;

	fsl_print("\nBuffer layout configuration on DPNI %d:\n", ni_id);

	err = dpni_drv_get_rx_buffer_layout(ni_id, layout);
	if (err) {
		pr_err("cannot get buffer layout\n");
		return;
	}

	fsl_print("\tPASS_TIMESTAMP     = %d\n", layout->pass_timestamp);
	fsl_print("\tPASS_PARSER_RESULT = %d\n", layout->pass_parser_result);
	fsl_print("\tPASS_FRAME_STATUS  = %d\n", layout->pass_frame_status);
	fsl_print("\tPASS_SW_OPAQUE     = %d\n", layout->pass_sw_opaque);
	fsl_print("\tPRIVATE_DATA_SIZE  = %d\n", layout->private_data_size);
	fsl_print("\tDATA_ALIGN         = %d\n", layout->data_align);
	fsl_print("\tDATA_HEAD_ROOM     = %d\n", layout->data_head_room);
	fsl_print("\tDATA_TAIL_ROOM     = %d\n", layout->data_tail_room);
}

void dpni_drv_dump_tx_hw_annotation(uint16_t ni_id)
{
	uint32_t hw_anno;
	int err;

	err = dpni_drv_get_tx_hw_annotation(ni_id, &hw_anno);
	if (err) {
		pr_err("get_tx_hw_annotation failed\n");
		return;
	}

	fsl_print("\nHW annotation tx pass-through fields for NI %d:\n", ni_id);
	fsl_print("\tSTATUS_AND_TS\t= %d\n",
		  !!(hw_anno & DPNI_DRV_TX_HW_ANNOTATION_PASS_TS));
	fsl_print("\tSW_OPAQUE\t= %d\n",
		  !!(hw_anno & DPNI_DRV_TX_HW_ANNOTATION_PASS_SW_OPAQUE));
}

#endif	/* SL_DEBUG */
