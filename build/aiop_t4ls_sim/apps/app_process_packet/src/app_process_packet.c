#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "fsl_dpni.h"
#include "dplib/dpni_drv.h"
#include "dpni/drv.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_ip.h"
#include "fsl_parser.h"

int app_init(void);
void app_free(void);


static void app_process_packet (dpni_drv_app_arg_t arg)
{
	// Change the first 4 bytes of the frame
	*((uint32_t *) PRC_GET_SEGMENT_ADDRESS()) = 0xdeadbeef;
	fdma_modify_default_segment_data(0,4);
#if 0
	/*
	ret = ip_set_nw_src(src_addr);
	if (!ret)
			fsl_os_print("AIOP test: Error while replacing header src address\n");

	 */
#endif
	dpni_drv_send((uint16_t)arg);
	fdma_terminate_task();
}

#if 0
static void aiop_replace_parser()
{
    struct parse_profile_record verif_parse_profile1;
    uint8_t prpid = 0;
    int status = 0;
    
    /* Init basic parse profile */
    verif_parse_profile1.eth_hxs_config = 0x0;
    verif_parse_profile1.llc_snap_hxs_config = 0x0;
    verif_parse_profile1.vlan_hxs_config.en_erm_soft_seq_start = 0x0;
    verif_parse_profile1.vlan_hxs_config.configured_tpid_1 = 0x0;
    verif_parse_profile1.vlan_hxs_config.configured_tpid_2 = 0x0;
    /* No MTU checking */
    verif_parse_profile1.pppoe_ppp_hxs_config = 0x0;
    verif_parse_profile1.mpls_hxs_config.en_erm_soft_seq_start= 0x0;
    /* Frame Parsing advances to MPLS Default Next Parse (IP HXS) */
    verif_parse_profile1.mpls_hxs_config.lie_dnp = PARSER_IP_STARTING_HXS;
    verif_parse_profile1.arp_hxs_config = 0x0;
    verif_parse_profile1.ip_hxs_config = 0x0;
    verif_parse_profile1.ipv4_hxs_config = 0x0;
    /* Routing header is ignored and the destination address from
     * main header is used instead */
    verif_parse_profile1.ipv6_hxs_config = 0x0;
    verif_parse_profile1.gre_hxs_config = 0x0;
    verif_parse_profile1.minenc_hxs_config = 0x0;
    verif_parse_profile1.other_l3_shell_hxs_config= 0x0;
    /* In short Packet, padding is removed from Checksum calculation */
    verif_parse_profile1.tcp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
    /* In short Packet, padding is removed from Checksum calculation */
    verif_parse_profile1.udp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
    verif_parse_profile1.ipsec_hxs_config = 0x0;
    verif_parse_profile1.sctp_hxs_config = 0x0;
    verif_parse_profile1.dccp_hxs_config = 0x0;
    verif_parse_profile1.other_l4_shell_hxs_config = 0x0;
    verif_parse_profile1.gtp_hxs_config = 0x0;
    verif_parse_profile1.esp_hxs_config = 0x0;
    verif_parse_profile1.l5_shell_hxs_config = 0x0;
    verif_parse_profile1.final_shell_hxs_config = 0x0;

    parser_profile_replace(&verif_parse_profile1, prpid);
}
#endif

static  void  app_receive_cb(void) {
#if 0
    //aiop_replace_parser();
#endif
    receive_cb();
}

int app_init(void)
{
    int err = 0;
    
    // This code is supposed to tell linker to include function receive_cb() in elf file.
    volatile int always_zero = 0;
    if(always_zero)
        app_receive_cb();

    err = dpni_drv_register_rx_cb(0/*ni_id*/, 
                                  0/*flow_id*/, 
                                  NULL/*dpio*/, 
                                  NULL /*dpsp*/, 
                                  app_process_packet, /* callback for flow_id*/
                                  0/*arg, nic number*/);
    
    return err;
}

void app_free(void)
{
    /* TODO - complete!*/
}
