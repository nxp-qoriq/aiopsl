#include "common/types.h"
#include "fsl_dbg.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "fsl_io.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "platform.h"
#include "inc/fsl_sys.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpbp.h"
#include "ls2085_aiop/fsl_platform.h"
#include "drv.h"
#include "system.h"

#define __ERR_MODULE__  MODULE_DPNI
#define ETH_BROADCAST_ADDR		((uint8_t []){0xff,0xff,0xff,0xff,0xff,0xff})
int dpni_drv_init(void);
void dpni_drv_free(void);

/* TODO - get rid */
__SHRAM struct dpni_drv *nis;
__SHRAM int num_of_nis;

static void discard_rx_cb()
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
                             uint16_t		flow_id,
                             rx_cb_t      	*cb,
                             dpni_drv_app_arg_t arg)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	dpni_drv->args[flow_id] = arg;
	dpni_drv->rx_cbs[flow_id] = cb;

	return 0;
}

int dpni_drv_unregister_rx_cb (uint16_t		ni_id,
                             uint16_t		flow_id)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	dpni_drv->rx_cbs[flow_id] = &discard_rx_app_cb;

	return 0;
}

int dpni_drv_enable (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	int		err;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;

	if ((err = dpni_enable(&dpni_drv->dpni)) != 0)
		return err;
	return 0;
}

int dpni_drv_disable (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	return dpni_disable(&dpni_drv->dpni);
}


int dpni_drv_probe(struct dprc	*dprc,
		   uint16_t	mc_niid,
		   uint16_t	aiop_niid,
                   struct dpni_pools_cfg *pools_params)
{
	uintptr_t wrks_addr;
	int i;
	uint32_t j;
	int err = 0, tmp = 0;
	struct dpbp dpbp = { 0 };
	struct dpni dpni = { 0 };
	uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	uint16_t qdid;
	struct dpni_attr attributes;
	struct dpni_attach_cfg attach_params;

	/* TODO: replace wrks_addr with global struct */
	wrks_addr = (sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW, 0, E_MAPPED_MEM_TYPE_GEN_REGS) +
		     SOC_PERIPH_OFF_AIOP_WRKS);

	/* TODO: replace 1024 w/ #define from Yulia */
	/* Search for NIID (mc_niid) in EPID table and prepare the NI for usage. */
	for (i = DPNI_EPID_START; i < 1024; i++) {
		/* Prepare to read from entry i in EPID table - EPAS reg */
		iowrite32((uint32_t)i, UINT_TO_PTR(wrks_addr + 0x0f8)); // TODO: change to LE, replace address with #define

		/* Read Entry Point Param (EP_PM) which contains the MC NI ID */
		j = ioread32(UINT_TO_PTR(wrks_addr + 0x104)); // TODO: change to LE, replace address with #define

		pr_debug("Found NI: EPID[%d].EP_PM = %d\n", i, j);

		if (j == mc_niid) {
			num_of_nis ++;
			/* Replace MC NI ID with AIOP NI ID */
			iowrite32(aiop_niid, UINT_TO_PTR(wrks_addr + 0x104)); // TODO: change to LE, replace address with #define

#if 0
			/* TODO: the mc_niid field will be necessary if we decide to close the DPNI at the end of Probe. */
			/* Register MC NI ID in AIOP NI table */
			nis[aiop_niid].mc_niid = mc_niid;
#endif

			dpni.regs = dprc->regs;

			if ((err = dpni_open(&dpni, mc_niid)) != 0) {
				pr_err("Failed to open DP-NI%d\n.", mc_niid);
				return err;
			}

			/* Save dpni regs and authentication ID in internal AIOP NI table */
			nis[aiop_niid].dpni = dpni;

			/* Register MAC address in internal AIOP NI table */
			if ((err = dpni_get_primary_mac_addr(&dpni, mac_addr)) != 0) {
				pr_err("Failed to get MAC address for DP-NI%d\n", mc_niid);
				return err;
			}
			memcpy(nis[aiop_niid].mac_addr, mac_addr, NET_HDR_FLD_ETH_ADDR_SIZE);

			/* Register Broadcast MAC address*/
			if ((err = dpni_add_mac_addr(&dpni, ETH_BROADCAST_ADDR)) != 0) {
				pr_err("Failed to add MAC address for DP-NI%d\n", mc_niid);
				return err;
			}


			if ((err = dpni_get_attributes(&dpni, &attributes)) != 0) {
				pr_err("Failed to get attributes of DP-NI%d.\n", mc_niid);
				return err;
			}

			/* TODO: set nis[aiop_niid].starting_hxs according to the DPNI attributes.
			 * Not yet implemented on MC. Currently always set to zero, which means ETH. */
			memset (&attach_params, 0, sizeof(attach_params));
			if ((err = dpni_attach(&dpni, &attach_params)) != 0) {
				pr_err("Failed to attach parameters to DP-NI%d.\n", mc_niid);
				return err;
			}

			if ((err = dpni_set_pools(&dpni, pools_params)) != 0) {
				pr_err("Failed to set the pools to DP-NI%d.\n", mc_niid);
				return err;
			}

			/* Enable DPNI before updating the entry point function (EP_PC)
			 * in order to allow DPNI's attributes to be initialized.
			 * Frames arriving before the entry point function is updated will be dropped. */
			if ((err = dpni_enable(&dpni)) != 0) {
				pr_err("Failed to enable DP-NI%d\n", mc_niid);
				return -ENODEV;
			}

			/* Now a Storage Profile exists and is associated with the NI */

			/* Register QDID in internal AIOP NI table */
			if ((err = dpni_get_qdid(&dpni, &qdid)) != 0) {
				pr_err("Failed to get QDID for DP-NI%d\n", mc_niid);
				return -ENODEV;
			}
			nis[aiop_niid].qdid = qdid;

#ifdef NEW_MC_API
			/* Register SPID in internal AIOP NI table */
			if ((err = dpni_get_spid(&dpni, &spid)) != 0) {
				pr_err("Failed to get SPID for DP-NI%d\n", mc_niid);
				return -ENODEV;
			}
			nis[aiop_niid].spid = spid;
#endif

#if 0
			/* TODO: need to decide if we should close DPNI at this stage.
			 * If so, then the MC NI ID must be saved in dpni_drv.mc_niid.
			 */
			dpni_close(&dpni);
#endif

			/* TODO: need to initialize additional NI table fields according to DPNI attributes */

			/* Replace discard callback with receive callback */
			iowrite32(PTR_TO_UINT(receive_cb), UINT_TO_PTR(wrks_addr + 0x100)); // TODO: change to LE, replace address with #define

			return 0;
		}
	}

	pr_err("DP-NI%d not found in EPID table.\n", mc_niid);
	return(-ENODEV);
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

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_add_mac_addr(&(dpni_drv->dpni), mac_addr);
}

int dpni_drv_remove_mac_addr(uint16_t ni_id,
                             const uint8_t mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE])
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the NI structure */
	dpni_drv = nis + ni_id;
	return dpni_remove_mac_addr(&(dpni_drv->dpni), mac_addr);
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
#ifndef AIOP_STANDALONE
	uintptr_t	wrks_addr;
#endif
	int		    i;
	int         error = 0;
	uint8_t prpid;


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
		int	   j;

		dpni_drv->aiop_niid    = (uint16_t)i;
#if 0
		/* TODO: the mc_niid field will be necessary if we decide to close the DPNI at the end of Probe. */
		dpni_drv->mc_niid      = 0;
#endif
		dpni_drv->spid         = 0;
		dpni_drv->prpid        = prpid; /*parser profile id from parser_profile_init()*/
		dpni_drv->starting_hxs = 0; //ETH HXS
		dpni_drv->qdid         = 0;
		dpni_drv->flags        = DPNI_DRV_FLG_PARSE | DPNI_DRV_FLG_PARSER_DIS | DPNI_DRV_FLG_MTU_ENABLE;
		dpni_drv->mtu          = 0xffff;

		/* put a default RX callback - dropping the frame */
		for (j = 0; j < DPNI_DRV_MAX_NUM_FLOWS; j++)
			dpni_drv->rx_cbs[j] = discard_rx_app_cb;
	}

#ifndef AIOP_STANDALONE
	/* Initialize EPID-table with discard_rx_cb for all entries (EP_PC field) */
#if 0
	/* TODO: following code can not currently compile on AIOP, need to port over  MC definitions */
	aiop_tile_regs = (struct aiop_tile_regs *)sys_get_memory_mapped_module_base(FSL_OS_MOD_AIOP,
	                                                     0,
	                                                     E_MAPPED_MEM_TYPE_GEN_REGS);
	ws_regs = &aiop_tile_regs->ws_regs;
	/* TODO: replace 1024 w/ constant */
	for (i = DPNI_EPID_START; i < 1024; i++) {
		/* Prepare to write to entry i in EPID table */
		iowrite32((uint32_t)i, ws_regs->epas; 					// TODO: change to LE
		iowrite32(PTR_TO_UINT(discard_rx_cb), ws_regs->ep_pc); 	// TODO: change to LE
	}
#else
	/* TODO: temporary code. should be removed. */
	wrks_addr = (sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW, 0, E_MAPPED_MEM_TYPE_GEN_REGS) +
	             SOC_PERIPH_OFF_AIOP_WRKS);


	/* TODO: replace 1024 w/ constant */
	for (i = DPNI_EPID_START; i < 1024; i++) {
		/* Prepare to write to entry i in EPID table - EPAS reg */
		iowrite32((uint32_t)i, UINT_TO_PTR(wrks_addr + 0x0f8)); // TODO: change to LE, replace address with #define

		iowrite32(PTR_TO_UINT(discard_rx_cb), UINT_TO_PTR(wrks_addr + 0x100)); // TODO: change to LE, replace address with #define

#if 0
		/* TODO : this is a temporary assignment for testing purposes, until MC initialization of EPID table will be operational. */
		iowrite32((uint32_t)i, UINT_TO_PTR(wrks_addr + 0x104));
#endif
	}
#endif
#endif



	return error;
}

void dpni_drv_free(void)
{
	if (nis)
		fsl_os_xfree(nis);
	nis = NULL;
}
