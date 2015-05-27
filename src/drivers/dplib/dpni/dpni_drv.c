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
#include "fsl_dprc_drv.h"
#include "slab.h"
#include "evm.h"
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
/* TODO - get rid */
struct dpni_drv nis_first __attribute__((aligned(8)));
struct dpni_drv *nis = &nis_first;
int num_of_nis;

struct dpni_early_init_request g_dpni_early_init_data = {0};

static int dpni_drv_ev_cb(uint8_t generator_id, uint8_t event_id, uint64_t size, void *event_data);

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

int dpni_drv_register_rx_cb (uint16_t		ni_id,
                             rx_cb_t      	*cb)
{
	struct dpni_drv *dpni_drv;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
				sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table entry*/
	iowrite32_ccsr((uint32_t)(dpni_drv->dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	iowrite32_ccsr(PTR_TO_UINT(cb), &wrks_addr->ep_pc);
	cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock); /*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
	return 0;
}

int dpni_drv_unregister_rx_cb (uint16_t		ni_id)
{
	struct dpni_drv *dpni_drv;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
				sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table entry*/
	iowrite32_ccsr((uint32_t)(dpni_drv->dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock); /*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
	return 0;
}

int dpni_drv_enable (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	return dpni_enable(&dprc->io, dpni_drv->dpni_drv_params_var.dpni);
}

int dpni_drv_disable (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	return dpni_disable(&dprc->io, dpni_drv->dpni_drv_params_var.dpni);
}

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
		/*send event: "DPNI_ADDED_EVENT" to EVM with
		 * AIOP NI ID */
		err = evmng_sl_raise_event(
			EVM_GENERATOR_AIOPSL,
			DPNI_EVENT_ADDED,
			&aiop_niid);
		if(err){
			sl_pr_err("Failed to raise event for "
				"NI-%d.\n", aiop_niid);
			return err;
		}
		index = (int)aiop_niid;
	}

	/*update that this index scanned*/
	nis[index].dpni_drv_params_var.flags |= DPNI_DRV_FLG_SCANNED;
	cdma_mutex_lock_release((uint64_t)nis);
	return 0;
}

int dpni_drv_sync(struct mc_dprc *dprc)
{
	uint16_t aiop_niid;
	int err;
	cdma_mutex_lock_take((uint64_t)nis, CDMA_MUTEX_WRITE_LOCK);
	for(aiop_niid = 0; aiop_niid < SOC_MAX_NUM_OF_DPNI; aiop_niid++)
	{
		if(nis[aiop_niid].dpni_drv_params_var.flags &
			DPNI_DRV_FLG_SCANNED){
			nis[aiop_niid].dpni_drv_params_var.flags &= 0xFE;
		}
		else if(nis[aiop_niid].dpni_id != DPNI_NOT_IN_USE){
			dpni_drv_unprobe(dprc, aiop_niid);
			/*send event: "DPNI_REMOVED_EVENT" to EVM with
			 * AIOP NI ID */
			err = evmng_sl_raise_event(
				EVM_GENERATOR_AIOPSL,
				DPNI_EVENT_REMOVED,
				&aiop_niid);
			if(err){
				sl_pr_err("Failed to raise event for "
					"NI-%d.\n", aiop_niid);
				return err;
			}
		}
	}
	cdma_mutex_lock_release((uint64_t)nis);
	return 0;
}


int dpni_drv_unprobe(struct mc_dprc *dprc,
                               uint16_t aiop_niid)
{
	int err;
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

	err = dpni_close(&dprc->io, nis[aiop_niid].dpni_drv_params_var.dpni);
	if(err){
		sl_pr_err("Error %d, "
			"failed to close DP-NI%d\n.", err,
			nis[aiop_niid].dpni_id);
	}

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
	nis[aiop_niid].dpni_lock                   = 0;
	nis[aiop_niid].dpni_id = DPNI_NOT_IN_USE;
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
	uint16_t dpni = 0;
	uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	uint16_t qdid;
	struct dpni_sp_info sp_info = { 0 };
	struct dpni_attr attributes;
	struct dpni_buffer_layout layout = {0};
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

			err = dpni_open(&dprc->io, mc_niid, &dpni);
			if(err){
				sl_pr_err("Failed to open DP-NI%d\n.", mc_niid);
				break;
			}

			/* Save dpni regs and authentication ID in internal
			 * AIOP NI table */
			nis[aiop_niid].dpni_drv_params_var.dpni = dpni;

			/* Register MAC address in internal AIOP NI table */
			err = dpni_get_primary_mac_addr(&dprc->io,
			                                     dpni,
			                                     mac_addr);
			if(err){
				sl_pr_err("Failed to get MAC address for DP-NI%d\n",
				       mc_niid);
				break;
			}
			memcpy(nis[aiop_niid].mac_addr,
			       mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);


			err = dpni_get_attributes(&dprc->io,
			                               dpni,
			                               &attributes);
			if(err){
				sl_pr_err("Failed to get attributes of DP-NI%d.\n",
				       mc_niid);
				break;
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
				sl_pr_err("Failed to set the pools to DP-NI%d.\n",
				       mc_niid);
				break;
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
				sl_pr_err("Failed to set rx buffer layout for DP-NI%d\n",
				       mc_niid);
				break;
			}

			/*
			 * Disable TX confirmation for DPNI's in AIOP in case
			 * the option: 'DPNI_OPT_TX_CONF_DISABLED' was not
			 * selected at DPNI creation.
			 * */
			err = dpni_set_tx_conf_revoke(&dprc->io, dpni, 1);
			if(err){
				sl_pr_err("Failed to set tx_conf_revoke for DP-NI%d\n",
				          mc_niid);
				break;
			}

			/* Enable DPNI before updating the entry point
			 * function (EP_PC) in order to allow DPNI's attributes
			 * to be initialized.
			 * Frames arriving before the entry point function is
			 * updated will be dropped. */
			err = dpni_enable(&dprc->io, dpni);
			if(err){
				sl_pr_err("Failed to enable DP-NI%d\n", mc_niid);
				break;
			}

			/* Now a Storage Profile exists and is associated
			 * with the NI */

			/* Register QDID in internal AIOP NI table */
			err = dpni_get_qdid(&dprc->io,
			                         dpni, &qdid);
			if(err){
				sl_pr_err("Failed to get QDID for DP-NI%d\n",
				       mc_niid);
				break;
			}
			nis[aiop_niid].dpni_drv_tx_params_var.qdid = qdid;

			/* Register SPID in internal AIOP NI table */
			if ((err = dpni_get_sp_info(&dprc->io,
			                         dpni, &sp_info)) != 0) {
				sl_pr_err("Failed to get SPID for DP-NI%d\n",
				       mc_niid);
				break;
			}
			/*TODO: change to uint16_t in nis table
			 * for the next release*/
			nis[aiop_niid].dpni_drv_params_var.spid =
				(uint8_t)sp_info.spids[0];
			/* Store epid index in AIOP NI's array*/
			nis[aiop_niid].dpni_drv_params_var.epid_idx =
				(uint16_t)i;

#if 0
			/* TODO: need to decide if we should close DPNI at this stage.
			 * If so, then the MC NI ID must be saved in dpni_drv.mc_niid.
			 */
			dpni_close(&dpni);
#endif

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

			err = dpni_set_irq(&dprc->io, dpni,
				DPNI_IRQ_INDEX, DPNI_EVENT,
				(uint32_t)mc_niid, 0);
			if(err){
				sl_pr_err("Failed to set irq for DP-NI%d\n",
				          mc_niid);
				break;
			}

			err = dpni_set_irq_mask(&dprc->io, dpni,
			                   DPNI_IRQ_INDEX,
			                   DPNI_IRQ_EVENT_LINK_CHANGED);
			if(err){
				sl_pr_err("Failed to set irq mask for DP-NI%d\n",
				          mc_niid);
				break;
			}

			err = dpni_set_irq_enable(&dprc->io, dpni,
			                        DPNI_IRQ_INDEX, 1);
			if(err){
				sl_pr_err("Failed to set irq enable for DP-NI%d\n",
				          mc_niid);
				break;
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
		cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
		sl_pr_err("DP-NI %d not found in EPID table.\n", mc_niid);
		err = -ENODEV;
	}
	return err;
}


int dpni_drv_get_spid(uint16_t ni_id, uint16_t *spid)
{
	struct dpni_drv *dpni_drv;

	dpni_drv = nis + ni_id;
	*spid = dpni_drv->dpni_drv_params_var.spid;

	return 0;
}

int dpni_drv_get_spid_ddr(uint16_t ni_id, uint16_t *spid_ddr)
{
	struct dpni_drv *dpni_drv;

	dpni_drv = nis + ni_id;
	*spid_ddr = dpni_drv->dpni_drv_params_var.spid_ddr;
	return 0;
}

int dpni_drv_get_num_of_nis (void)
{
	return num_of_nis;
}


/* TODO: replace with macro/inline */
int dpni_drv_get_primary_mac_addr(uint16_t niid,
                                  uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct dpni_drv *dpni_drv;
	/* calculate pointer to the NI structure */
	dpni_drv = nis + niid;
	/*Lock dpni table entry*/
	cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock,
	                     CDMA_MUTEX_WRITE_LOCK);
	memcpy(mac_addr, dpni_drv->mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);
	/*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock);

	return 0;
}

int dpni_drv_set_primary_mac_addr(uint16_t niid,
                                  uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err;
	/* calculate pointer to the NI structure */
	dpni_drv = nis + niid;
	/*Lock dpni table entry*/
	cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock,
	                     CDMA_MUTEX_WRITE_LOCK);
	err = dpni_set_primary_mac_addr(&dprc->io,
	                                dpni_drv->dpni_drv_params_var.dpni,
	                                mac_addr);
	if(err){
		/*Unlock dpni table entry*/
		cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock);
		return err;
	}
	memcpy(dpni_drv->mac_addr, mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);
	/*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock);
	return 0;
}

int dpni_drv_add_mac_addr(uint16_t ni_id,
                          const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_add_mac_addr(&dprc->io, dpni_drv->dpni_drv_params_var.dpni,
	                         mac_addr);
}

int dpni_drv_remove_mac_addr(uint16_t ni_id,
                             const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_remove_mac_addr(&dprc->io,
	                            dpni_drv->dpni_drv_params_var.dpni,
	                            mac_addr);
}

int dpni_drv_set_max_frame_length(uint16_t ni_id,
                                  const uint16_t mfl)
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_set_max_frame_length(&dprc->io,
	                                 dpni_drv->dpni_drv_params_var.dpni,
	                                 mfl);
}

int dpni_drv_get_max_frame_length(uint16_t ni_id,
                                  uint16_t *mfl)
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_get_max_frame_length(&dprc->io,
	                                 dpni_drv->dpni_drv_params_var.dpni,
	                                 mfl);
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


__COLD_CODE int dpni_drv_init(void)
{
	int i;
	uint8_t prpid = 0;
	int err = 0;
	int dev_count;
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
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

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
		dpni_drv->dpni_lock                        = 0;
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
//	for (i = 0; i < dev_count; i++) {
//		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
//		if (strcmp(dev_desc.type, "dpni") == 0) {
//			/* TODO: print conditionally based on log level */
//			print_dev_desc(&dev_desc);
//
//
//			err = dpni_drv_probe(dprc, (uint16_t)dev_desc.id, &aiop_niid);
//			if(err){
//				pr_err("Error: %d, failed to probe DPNI-%d.\n",
//				       err, dev_desc.id);
//				return err;
//			}
//			/*send event: "DPNI_ADDED_EVENT" to EVM with
//			 * AIOP NI ID */
//			err = evmng_sl_raise_event(DPNI_EVENT_ADDED,
//			                      &aiop_niid);
//			if(err){
//				sl_pr_err("Failed to raise event for "
//					"NI-%d.\n", aiop_niid);
//				return err;
//			}
//		}
//	}

	err = evmng_irq_register(EVM_GENERATOR_AIOPSL,
	                         DPNI_EVENT,
	                         0,
	                         0,
	                         dpni_drv_ev_cb);
	if(err){
		pr_err("EVM registration for DPNI events failed %d\n",err);
		return -ENAVAIL;
	}
	else{
		pr_info("Registered to: dpni_drv_ev_cb\n");
	}

	return err;
}

static int dpni_drv_ev_cb(uint8_t generator_id, uint8_t event_id, uint64_t app_ctx, void *event_data)
{
	/*Container was updated*/
	int err;
	uint16_t ni_id;
	uint32_t status;
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_link_state link_state;

	UNUSED(app_ctx);
	/*TODO SIZE should be cooperated with Ehud*/

	if(event_id == DPNI_EVENT && generator_id == EVM_GENERATOR_AIOPSL){
		ni_id = *((uint16_t*)event_data);
		sl_pr_debug("DPNI event\n");
		/* calculate pointer to the NI structure */
		dpni_drv = nis + ni_id;

		err = dpni_get_irq_status(&dprc->io,
		                          dpni_drv->dpni_drv_params_var.dpni,
		                          DPNI_IRQ_INDEX,
		                          &status);
		if(err){
			sl_pr_err("Get irq status for NI %d "
				"failed\n", ni_id);
			return -ENAVAIL;
		}

		if(status & DPNI_IRQ_EVENT_LINK_CHANGED){
			err = dpni_get_link_state(&dprc->io,
			                          dpni_drv->dpni_drv_params_var.dpni,
			                          &link_state);
			if(err){
				sl_pr_err("Failed to get dpni link state, %d.\n", err);
				return err;
			}

			if(link_state.up){
				err = evmng_sl_raise_event(
					EVM_GENERATOR_AIOPSL,
					DPNI_EVENT_LINK_UP,
					&ni_id);
			}
			else{
				err = evmng_sl_raise_event(
					EVM_GENERATOR_AIOPSL,
					DPNI_EVENT_LINK_DOWN,
					&ni_id);
			}
			if(err){
				sl_pr_err("Failed to raise event for "
					"NI-%d.\n", ni_id);
				return err;
			}

			err = dpni_clear_irq_status(&dprc->io, dprc->token,
			                            DPNI_IRQ_INDEX,
			                            DPNI_IRQ_EVENT_LINK_CHANGED);
			if(err){
				sl_pr_err("Clear status for DPNI link "
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

__COLD_CODE void dpni_drv_free(void)
{
	if (nis)
		fsl_free(nis);
	nis = NULL;
}



int dpni_drv_set_multicast_promisc(uint16_t ni_id, int en){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;

	return dpni_set_multicast_promisc(&dprc->io,
	                                  dpni_drv->dpni_drv_params_var.dpni,
	                                  en);
}


int dpni_drv_get_multicast_promisc(uint16_t ni_id, int *en){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;

	return dpni_get_multicast_promisc(&dprc->io,
	                                  dpni_drv->dpni_drv_params_var.dpni,
	                                  en);
}


int dpni_drv_set_unicast_promisc(uint16_t ni_id, int en){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;

	return dpni_set_unicast_promisc(&dprc->io,
	                                dpni_drv->dpni_drv_params_var.dpni,
	                                en);
}


int dpni_drv_get_unicast_promisc(uint16_t ni_id, int *en){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;

	return dpni_get_unicast_promisc(&dprc->io,
	                                dpni_drv->dpni_drv_params_var.dpni,
	                                en);
}

int dpni_drv_get_ordering_mode(uint16_t ni_id){
	uint32_t ep_osc;
	struct dpni_drv *dpni_drv;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
				sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	/* write epid index to epas register */
	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table entry*/
	iowrite32_ccsr((uint32_t)(dpni_drv->dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	/* read ep_osc - to get the order scope (concurrent / exclusive) */
	ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
	cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock); /*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);

	return (int)(ep_osc & ORDER_MODE_BIT_MASK) >> 24;
}

static int dpni_drv_set_ordering_mode(uint16_t ni_id, int ep_mode){
	uint32_t ep_osc;
	struct dpni_drv *dpni_drv;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
				sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
	cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table entry*/
	/* write epid index to epas register */
	iowrite32_ccsr((uint32_t)(dpni_drv->dpni_drv_params_var.epid_idx), &wrks_addr->epas);
	/* read ep_osc - to get the order scope (concurrent / exclusive) */
	ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
	ep_osc &= ORDER_MODE_CLEAR_BIT;
	ep_osc |= (ep_mode & 0x01) << 24;
	/*Set concurrent mode for NI in epid table*/
	iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);
	cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock); /*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
	return 0;
}

int dpni_drv_set_concurrent(uint16_t ni_id){
	return dpni_drv_set_ordering_mode(ni_id, DPNI_DRV_CONCURRENT_MODE);
}

int dpni_drv_set_exclusive(uint16_t ni_id){
	return dpni_drv_set_ordering_mode(ni_id, DPNI_DRV_EXCLUSIVE_MODE);
}

__COLD_CODE int dpni_drv_set_order_scope(uint16_t ni_id, struct dpkg_profile_cfg *key_cfg){
	uint32_t ep_osc;
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_rx_tc_dist_cfg cfg = {0};
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
					sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;
	int err;
	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;

	memset(order_scope_buffer, 0, PARAMS_IOVA_BUFF_SIZE);
	cfg.dist_size = 0;

	if(key_cfg == DPNI_DRV_NO_ORDER_SCOPE)
	{
		/*Mutex lock to avoid race condition while writing to EPID table*/
		cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);
		cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock, CDMA_MUTEX_WRITE_LOCK); /*Lock dpni table entry*/
		/* write epid index to epas register */
		iowrite32_ccsr((uint32_t)(dpni_drv->dpni_drv_params_var.epid_idx), &wrks_addr->epas);
		/* Read ep_osc from EPID table */
		ep_osc = ioread32_ccsr(&wrks_addr->ep_osc);
		/* src = 0 - No order scope specified. Task does not enter a scope,
		 * ep = 0 - executing concurrently,
		 * sel = 0,
		 * osrm = 0 - all frames enter scope 0 */
		ep_osc &= ORDER_MODE_NO_ORDER_SCOPE;
		/*Write ep_osc to EPID table*/
		iowrite32_ccsr(ep_osc, &wrks_addr->ep_osc);
		cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock); /*Unlock dpni table entry*/
		cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
		cfg.dist_mode = DPNI_DIST_MODE_NONE;
	}
	else
	{
		dpni_prepare_key_cfg(key_cfg, order_scope_buffer);
		cfg.dist_mode = DPNI_DIST_MODE_HASH;
	}

	cfg.key_cfg_iova = (uint64_t)order_scope_buffer;
	err = dpni_set_rx_tc_dist(&dprc->io,
	                          dpni_drv->dpni_drv_params_var.dpni,
	                          0,
	                          &cfg);
	return err;
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
	if(err)
		return err;

	for(i = 0; i <  dpni_drv_get_num_of_nis(); i++){
		if(endpoint2.id == nis[i].dpni_id){
			*aiop_niid = i;
			break;
		}
	}
	if(i == dpni_drv_get_num_of_nis())
		return -ENAVAIL;

	return 0;
}

int dpni_drv_get_connected_dpni_id(const uint16_t aiop_niid, uint16_t *dpni_id, int *state){
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dprc_endpoint endpoint1 = {0};
	struct dprc_endpoint endpoint2 = {0};
	int err;

	if(dprc == NULL)
		return -EINVAL;

	endpoint1.id = nis[aiop_niid].dpni_id;
	endpoint1.interface_id = 0;
	strcpy(&endpoint1.type[0], "dpni");

	err = dprc_get_connection(&dprc->io, dprc->token, &endpoint1, &endpoint2,
	                          state);
	if(err)
		return err;

	*dpni_id = (uint16_t)endpoint2.id;
	return 0;
}

int dpni_drv_set_rx_buffer_layout(uint16_t ni_id, const struct dpni_drv_buf_layout *layout){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_buffer_layout dpni_layout;
	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	dpni_layout.options = layout->options;
	dpni_layout.pass_timestamp = layout->pass_timestamp;
	dpni_layout.pass_parser_result = layout->pass_parser_result;
	dpni_layout.pass_frame_status = layout->pass_frame_status;
	dpni_layout.private_data_size = layout->private_data_size;
	dpni_layout.data_align = layout->data_align;
	dpni_layout.data_head_room = layout->data_head_room;
	dpni_layout.data_tail_room = layout->data_tail_room;

	return dpni_set_rx_buffer_layout(&dprc->io,
	                                 dpni_drv->dpni_drv_params_var.dpni,
	                                 &dpni_layout);
}

int dpni_drv_get_rx_buffer_layout(uint16_t ni_id, struct dpni_drv_buf_layout *layout){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_buffer_layout dpni_layout = {0};
	int err;
	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	err = dpni_get_rx_buffer_layout(&dprc->io,
	                                dpni_drv->dpni_drv_params_var.dpni,
	                                &dpni_layout);
	layout->options = dpni_layout.options;
	layout->pass_timestamp = dpni_layout.pass_timestamp;
	layout->pass_parser_result = dpni_layout.pass_parser_result;
	layout->pass_frame_status = dpni_layout.pass_frame_status;
	layout->private_data_size = dpni_layout.private_data_size;
	layout->data_align = dpni_layout.data_align;
	layout->data_head_room = dpni_layout.data_head_room;
	layout->data_tail_room = dpni_layout.data_tail_room;
	return err;
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

int dpni_drv_get_counter(uint16_t ni_id, enum dpni_drv_counter counter, uint64_t *value){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_get_counter(&dprc->io,
	                        dpni_drv->dpni_drv_params_var.dpni,
	                        (enum dpni_counter)counter,
	                        value);
}

int dpni_drv_reset_counter(uint16_t ni_id, enum dpni_drv_counter counter){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_set_counter(&dprc->io,
	                        dpni_drv->dpni_drv_params_var.dpni,
	                        (enum dpni_counter)counter,
	                        0);
}
/*TODO: Implement a real search when dynamic dpni will be available*/
int dpni_drv_get_dpni_id(uint16_t ni_id, uint16_t *dpni_id){
	if(ni_id >= dpni_drv_get_num_of_nis())
	{
		return -ENAVAIL;
	}

	*dpni_id = nis[ni_id].dpni_id;
	return 0;
}
/*TODO: Implement a real search when dynamic dpni will be available*/
int dpni_drv_get_ni_id(uint16_t dpni_id, uint16_t *ni_id){
	uint16_t i;

	for(i = 0; i < dpni_drv_get_num_of_nis(); i++)
	{
		if(nis[i].dpni_id == dpni_id)
		{
			*ni_id = i;
			break;
		}
	}
	if(i == dpni_drv_get_num_of_nis()){
		return -ENAVAIL;
	}
	return 0;
}

int dpni_drv_get_link_state(uint16_t ni_id, struct dpni_drv_link_state *state){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpni_link_state link_state;
	int err;

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	err = dpni_get_link_state(&dprc->io,
	                          dpni_drv->dpni_drv_params_var.dpni,
	                          &link_state);
	state->options = link_state.options;
	state->rate = link_state.rate;
	state->up = link_state.up;
	return err;
}

int dpni_drv_clear_mac_filters(uint16_t ni_id, uint8_t unicast, uint8_t multicast){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_clear_mac_filters(&dprc->io,
	                              dpni_drv->dpni_drv_params_var.dpni,
	                              (int)unicast, (int) multicast);
}

int dpni_drv_clear_vlan_filters(uint16_t ni_id){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_clear_vlan_filters(&dprc->io,
	                               dpni_drv->dpni_drv_params_var.dpni);
}

int dpni_drv_set_vlan_filters(uint16_t ni_id, int en){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_set_vlan_filters(&dprc->io,
	                             dpni_drv->dpni_drv_params_var.dpni,
	                             en);
}

int dpni_drv_add_vlan_id(uint16_t ni_id, uint16_t vlan_id){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_add_vlan_id(&dprc->io,
	                        dpni_drv->dpni_drv_params_var.dpni,
	                        vlan_id);
}

int dpni_drv_remove_vlan_id(uint16_t ni_id, uint16_t vlan_id){
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_remove_vlan_id(&dprc->io,
	                           dpni_drv->dpni_drv_params_var.dpni,
	                           vlan_id);
}

int dpni_drv_get_initial_presentation(
	uint16_t ni_id,
	struct ep_init_presentation* const init_presentation){
	struct dpni_drv *dpni_drv;
	int err;

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;

	/*Lock dpni table entry*/
	cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock, CDMA_MUTEX_WRITE_LOCK);

	err = ep_mng_get_initial_presentation(
			dpni_drv->dpni_drv_params_var.epid_idx, init_presentation);
	/*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock);

	return err;
}

int dpni_drv_set_initial_presentation(
	uint16_t ni_id,
	const struct ep_init_presentation* const init_presentation){
	struct dpni_drv *dpni_drv;
	int err;
	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;

	/*Lock dpni table entry*/
	cdma_mutex_lock_take((uint64_t)&dpni_drv->dpni_lock, CDMA_MUTEX_WRITE_LOCK);

	err = ep_mng_set_initial_presentation(
			dpni_drv->dpni_drv_params_var.epid_idx, init_presentation);

	/*Unlock dpni table entry*/
	cdma_mutex_lock_release((uint64_t)&dpni_drv->dpni_lock);
	return err;
}
