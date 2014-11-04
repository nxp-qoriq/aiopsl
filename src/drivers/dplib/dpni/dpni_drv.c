/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
#include "fsl_dbg.h"
#include "common/fsl_string.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_malloc.h"
#include "fsl_io_ccsr.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "platform.h"
#include "inc/fsl_sys.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpbp.h"
#include "ls2085_aiop/fsl_platform.h"
#include "dplib/fsl_dpni_drv.h"
#include "aiop_common.h"
#include "system.h"
#include "fsl_mc_init.h"

#define __ERR_MODULE__  MODULE_DPNI
#define ETH_BROADCAST_ADDR		((uint8_t []){0xff,0xff,0xff,0xff,0xff,0xff})
int dpni_drv_init(void);
void dpni_drv_free(void);

/* TODO - get rid */
struct dpni_drv nis_first __attribute__((aligned(8)));
struct dpni_drv *nis = &nis_first;
int num_of_nis;

void discard_rx_cb()
{

	pr_debug("Packet discarded by discard_rx_cb.\n");
	/*Discard frame and terminate task*/
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}

static void discard_rx_app_cb(dpni_drv_app_arg_t arg)
{
	UNUSED(arg);
	pr_debug("Packet discarded by discard_rx_app_cb.\n");
	/*Discard frame and terminate task*/
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}

int dpni_drv_register_rx_cb (uint16_t		ni_id,
                             rx_cb_t      	*cb,
                             dpni_drv_app_arg_t arg)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	lock_spinlock(&dpni_drv->dpni_lock); /*Lock dpni table entry*/
	dpni_drv->arg = arg;
	dpni_drv->rx_cbs = cb;
	unlock_spinlock(&dpni_drv->dpni_lock); /*Unlock dpni table entry*/
	return 0;
}

int dpni_drv_unregister_rx_cb (uint16_t		ni_id)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	lock_spinlock(&dpni_drv->dpni_lock); /*Lock dpni table entry*/
	dpni_drv->rx_cbs = &discard_rx_app_cb;
	unlock_spinlock(&dpni_drv->dpni_lock); /*Unlock dpni table entry*/
	return 0;
}

int dpni_drv_enable (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	int		err;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;

	if ((err = dpni_enable(&dprc->io, dpni_drv->dpni_drv_params_var.dpni))
									!= 0)
		return err;
	return 0;
}

int dpni_drv_disable (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	return dpni_disable(&dprc->io, dpni_drv->dpni_drv_params_var.dpni);
}


int dpni_drv_probe(struct mc_dprc *dprc,
		   uint16_t mc_niid,
		   uint16_t aiop_niid,
                   struct dpni_pools_cfg *pools_params)
{
	uintptr_t wrks_addr;
	int i;
	uint32_t j;
	int err = 0;
	uint16_t dpni = 0;
	uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	uint16_t qdid;
	uint16_t spid;
	struct dpni_attr attributes;

	/* TODO: replace wrks_addr with global struct */
	wrks_addr = (sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW, 0, E_MAPPED_MEM_TYPE_GEN_REGS) +
		     SOC_PERIPH_OFF_AIOP_WRKS);

	/* TODO: replace 1024 w/ #define from Yulia */
	/* Search for NIID (mc_niid) in EPID table and prepare the NI for usage. */
	for (i = AIOP_EPID_DPNI_START; i < 1024; i++) {
		/* Prepare to read from entry i in EPID table - EPAS reg */
		iowrite32_ccsr((uint32_t)i, UINT_TO_PTR(wrks_addr + 0x0f8)); // TODO: change to LE, replace address with #define

		/* Read Entry Point Param (EP_PM) which contains the MC NI ID */
		j = ioread32_ccsr(UINT_TO_PTR(wrks_addr + 0x104)); // TODO: change to LE, replace address with #define

		pr_debug("EPID[%d].EP_PM = %d\n", i, j);

		if (j == mc_niid) {

			/* Replace MC NI ID with AIOP NI ID */
			iowrite32_ccsr(aiop_niid, UINT_TO_PTR(wrks_addr + 0x104)); // TODO: change to LE, replace address with #define

#if 0
			/* TODO: the mc_niid field will be necessary if we decide to close the DPNI at the end of Probe. */
			/* Register MC NI ID in AIOP NI table */
			nis[aiop_niid].mc_niid = mc_niid;
#endif


			if ((err = dpni_open(&dprc->io, mc_niid, &dpni)) != 0) {
				pr_err("Failed to open DP-NI%d\n.", mc_niid);
				return err;
			}

			/* Save dpni regs and authentication ID in internal AIOP NI table */
			nis[aiop_niid].dpni_drv_params_var.dpni = dpni;

			/* Register MAC address in internal AIOP NI table */
			if ((err = dpni_get_primary_mac_addr(&dprc->io, dpni, mac_addr)) != 0) {
				pr_err("Failed to get MAC address for DP-NI%d\n", mc_niid);
				return err;
			}
			memcpy(nis[aiop_niid].mac_addr, mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);


			if ((err = dpni_get_attributes(&dprc->io, dpni, &attributes)) != 0) {
				pr_err("Failed to get attributes of DP-NI%d.\n", mc_niid);
				return err;
			}

			/* TODO: set nis[aiop_niid].starting_hxs according to the DPNI attributes.
			 * Not yet implemented on MC. Currently always set to zero, which means ETH. */
			if ((err = dpni_set_pools(&dprc->io, dpni, pools_params)) != 0) {
				pr_err("Failed to set the pools to DP-NI%d.\n", mc_niid);
				return err;
			}

			/* Enable DPNI before updating the entry point function (EP_PC)
			 * in order to allow DPNI's attributes to be initialized.
			 * Frames arriving before the entry point function is updated will be dropped. */
			if ((err = dpni_enable(&dprc->io, dpni)) != 0) {
				pr_err("Failed to enable DP-NI%d\n", mc_niid);
				return -ENODEV;
			}

			/* Now a Storage Profile exists and is associated with the NI */

			/* Register QDID in internal AIOP NI table */
			if ((err = dpni_get_qdid(&dprc->io, dpni, &qdid)) != 0) {
				pr_err("Failed to get QDID for DP-NI%d\n", mc_niid);
				return -ENODEV;
			}
			nis[aiop_niid].dpni_drv_tx_params_var.qdid = qdid;

			/* Register SPID in internal AIOP NI table */
			if ((err = dpni_get_spid(&dprc->io, dpni, &spid)) != 0) {
				pr_err("Failed to get SPID for DP-NI%d\n", mc_niid);
				return -ENODEV;
			}
			nis[aiop_niid].dpni_drv_params_var.spid = (uint8_t)spid; /*TODO: change to uint16_t in nis table for the next release*/


#if 0
			/* TODO: need to decide if we should close DPNI at this stage.
			 * If so, then the MC NI ID must be saved in dpni_drv.mc_niid.
			 */
			dpni_close(&dpni);
#endif

			/* TODO: need to initialize additional NI table fields according to DPNI attributes */

			/* Replace discard callback with receive callback */
			iowrite32_ccsr(PTR_TO_UINT(receive_cb), UINT_TO_PTR(wrks_addr + 0x100)); // TODO: change to LE, replace address with #define
			num_of_nis ++;
			return 0;
		}
	}

	pr_err("DP-NI%d not found in EPID table.\n", mc_niid);
	return(-ENODEV);
}


int dpni_drv_get_spid(uint16_t ni_id, uint16_t *spid)
{
	struct dpni_drv *dpni_drv;

	dpni_drv = nis + ni_id;
	*spid = dpni_drv->dpni_drv_params_var.spid;

	return 0;
}

int dpni_get_num_of_ni (void)
{
	return num_of_nis;
}


/* TODO: replace with macro/inline */
int dpni_drv_get_primary_mac_addr(uint16_t niid, uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	memcpy(mac_addr, nis[niid].mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);
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
			dpni_drv->dpni_drv_params_var.dpni, mac_addr);
}

int dpni_drv_set_max_frame_length(uint16_t ni_id,
                          const uint16_t mfl)
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_set_max_frame_length(&dprc->io, dpni_drv->dpni_drv_params_var.dpni, mfl);
}

int dpni_drv_get_max_frame_length(uint16_t ni_id,
                          uint16_t *mfl)
{
	struct dpni_drv *dpni_drv;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_get_max_frame_length(&dprc->io, dpni_drv->dpni_drv_params_var.dpni, mfl);
}



static int parser_profile_init(uint8_t *prpid)
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
    parse_profile1.parse_profile.ipv6_hxs_config = 0x0;
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
    for(i=0; i<16; i++)
        parse_profile1.parse_profile.soft_examination_param_array[i] = 0x0;

    return parser_profile_create(&(parse_profile1), prpid);
}


int dpni_drv_init(void)
{
	int		    i;
	int         error = 0;
	uint8_t prpid = 0;


	num_of_nis = 0;
	/* Allocate initernal AIOP NI table */
	nis =fsl_os_xmalloc(sizeof(struct dpni_drv)*SOC_MAX_NUM_OF_DPNI, MEM_PART_SH_RAM, 64);
	if (!nis) {
	    return -ENOMEM;
	}

	error = parser_profile_init(&prpid);
	if(error){
		pr_err("parser profile initialization failed %d\n", error);
		return error;
	}
	/* Initialize internal AIOP NI table */
	for (i = 0; i < SOC_MAX_NUM_OF_DPNI; i++) {
		struct dpni_drv * dpni_drv = nis + i;

		dpni_drv->dpni_drv_tx_params_var.aiop_niid    = (uint16_t)i;
#if 0
		/* TODO: the mc_niid field will be necessary if we decide to close the DPNI at the end of Probe. */
		dpni_drv->mc_niid      = 0;
#endif
		dpni_drv->dpni_drv_params_var.spid         = 0;
		dpni_drv->dpni_drv_params_var.prpid        = prpid; /*parser profile id from parser_profile_init()*/
		dpni_drv->dpni_drv_params_var.starting_hxs = 0; //ETH HXS
		dpni_drv->dpni_drv_tx_params_var.qdid         = 0;
		dpni_drv->dpni_drv_params_var.flags        = DPNI_DRV_FLG_PARSE | DPNI_DRV_FLG_PARSER_DIS;
		dpni_drv->dpni_drv_tx_params_var.mtu          = 0xffff;

		/* put a default RX callback - dropping the frame */
		dpni_drv->rx_cbs = discard_rx_app_cb;
	}
	return error;
}

void dpni_drv_free(void)
{
	if (nis)
		fsl_os_xfree(nis);
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


