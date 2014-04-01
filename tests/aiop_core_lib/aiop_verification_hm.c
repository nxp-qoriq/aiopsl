/**************************************************************************//**
@File          aiop_verification_hm.c

@Description   This file contains the AIOP Header Manipulation SW Verification
*//***************************************************************************/

#include "general.h"
#include "net/fsl_net.h"
#include "dplib/fsl_ip.h"
#include "dplib/fsl_l2.h"
#include "dplib/fsl_l4.h"
#include "dplib/fsl_nat.h"
#include "dplib/fsl_parser.h"

/* TODO - get rid of this! */
#include "../arch/accel/system.h"

#include "aiop_verification.h"
#include "aiop_verification_hm.h"


struct parse_profile_record verif_parse_profile;

extern __TASK struct aiop_default_task_params default_task_params;

void aiop_hm_init_parser()
{
	uint8_t i, prpid;

	verif_parse_profile.eth_hxs_config = 0x0;
	verif_parse_profile.llc_snap_hxs_config = 0x0;
	verif_parse_profile.vlan_hxs_config.en_erm_soft_seq_start = 0x0;
	verif_parse_profile.vlan_hxs_config.configured_tpid_1 = 0x0;
	verif_parse_profile.vlan_hxs_config.configured_tpid_2 = 0x0;
	/* No MTU checking */
	verif_parse_profile.pppoe_ppp_hxs_config = 0x0;
	verif_parse_profile.mpls_hxs_config.en_erm_soft_seq_start= 0x0;
	/* Frame Parsing advances to MPLS Default Next Parse (IP HXS) */
	verif_parse_profile.mpls_hxs_config.lie_dnp = 
			PARSER_PRP_MPLS_HXS_CONFIG_LIE;
	verif_parse_profile.arp_hxs_config = 0x0;
	verif_parse_profile.ip_hxs_config = 0x0;
	verif_parse_profile.ipv4_hxs_config = 0x0;
	/* Routing header is ignored and the destination address from
	 * main header is used instead */
	verif_parse_profile.ipv6_hxs_config = 0x0;
	verif_parse_profile.gre_hxs_config = 0x0;
	verif_parse_profile.minenc_hxs_config = 0x0;
	verif_parse_profile.other_l3_shell_hxs_config= 0x0;
	/* In short Packet, padding is removed from Checksum calculation */
	verif_parse_profile.tcp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	/* In short Packet, padding is removed from Checksum calculation */
	verif_parse_profile.udp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	verif_parse_profile.ipsec_hxs_config = 0x0;
	verif_parse_profile.sctp_hxs_config = 0x0;
	verif_parse_profile.dccp_hxs_config = 0x0;
	verif_parse_profile.other_l4_shell_hxs_config = 0x0;
	verif_parse_profile.gtp_hxs_config = 0x0;
	verif_parse_profile.esp_hxs_config = 0x0;
	verif_parse_profile.l5_shell_hxs_config = 0x0;
	verif_parse_profile.final_shell_hxs_config = 0x0;
	/* Assuming no soft examination parameters */
	for(i=0; i<16; i++)
		verif_parse_profile.soft_examination_param_array[i] = 0x0;
	sys_prpid_pool_create();
	/* Create the parse_profile and get an id */
	parser_profile_create(&verif_parse_profile, &prpid);
	default_task_params.parser_profile_id = prpid;
	parse_result_generate_default(0);
}
uint16_t aiop_verification_hm(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;


	opcode = *((uint32_t *) asa_seg_addr);

	switch (opcode) {
		/* HM Verification Initialization */
		case HM_VERIF_INIT_CMD_STR:
		{
			struct hm_init_verif_command *str =
			(struct hm_init_verif_command *) asa_seg_addr;
			default_task_params.parser_starting_hxs =
					str->parser_starting_hxs;
			aiop_hm_init_parser();
			str_size = sizeof(struct hm_init_verif_command);
			break;
		}

		/* HM L2 header remove Command Verification */
		case HM_L2_HEADER_REMOVE_CMD_STR:
		{
			l2_header_remove();
			str_size = sizeof(struct hm_l2_header_remove_command);
			break;
		}

		/* HM VLAN header remove Command Verification */
		case HM_VLAN_HEADER_REMOVE_CMD_STR:
		{
			struct hm_vlan_remove_command *str =
			(struct hm_vlan_remove_command *) asa_seg_addr;
			str->status = l2_vlan_header_remove();
			str_size = sizeof(struct hm_vlan_remove_command);
			break;
		}

		/* HM IPv4 Header Modification Command Verification */
		case HM_IPV4_MODIFICATION_CMD_STR:
		{
			struct hm_ipv4_modification_command *str =
			(struct hm_ipv4_modification_command *) asa_seg_addr;
			str->status = ipv4_header_modification(str->flags,
					str->tos,
					str->id,
					str->ip_src_addr,
					str->ip_dst_addr);
			str_size = sizeof(struct hm_ipv4_modification_command);
			break;
		}

		/* HM IPv6 Header Modification Command Verification */
		case HM_IPV6_MODIFICATION_CMD_STR:
		{
			struct hm_ipv6_modification_command *str =
			(struct hm_ipv6_modification_command *) asa_seg_addr;
			str->status = ipv6_header_modification(str->flags,
					str->tc,
					str->flow_label,
					str->ip_src_addr,
					str->ip_dst_addr);
			str_size = sizeof(struct hm_ipv6_modification_command);
			break;
		}

		/* HM IPv4 Header Encapsulation Command Verification */
		case HM_IPV4_ENCAPSULATION_CMD_STR:
		{
			struct hm_ipv4_encapsulation_command *str =
			(struct hm_ipv4_encapsulation_command *) asa_seg_addr;
			str->status = ipv4_header_encapsulation(str->flags,
					&str->ipv4_header_ptr,
					str->ipv4_header_size);
			str_size = sizeof(struct hm_ipv4_encapsulation_command);
			break;
		}

		/* HM IPv6 Header Encapsulation Command Verification */
		case HM_IPV6_ENCAPSULATION_CMD_STR:
		{
			struct hm_ipv6_encapsulation_command *str =
			(struct hm_ipv6_encapsulation_command *) asa_seg_addr;
			str->status = ipv6_header_encapsulation(str->flags,
					&str->ipv6_header_ptr,
					str->ipv6_header_size);
			str_size = sizeof(struct hm_ipv6_encapsulation_command);
			break;
		}
/*Todo need to add Ext encapsulation functions */

		/* HM IP Header Decapsulation Command Verification */
		case HM_IP_DECAPSULATION_CMD_STR:
		{
			struct hm_ip_decapsulation_command *str =
			(struct hm_ip_decapsulation_command *) asa_seg_addr;
			str->status = ip_header_decapsulation(str->flags);
			str_size = sizeof(struct hm_ip_decapsulation_command);
			break;
		}

		/* HM UDP Header Modification Command Verification */
		case HM_UDP_MODIFICATION_CMD_STR:
		{
			struct hm_udp_modification_command *str =
			(struct hm_udp_modification_command *) asa_seg_addr;
			str->status = l4_udp_header_modification(str->flags,
					str->udp_src_port,
					str->udp_dst_port);
			str_size = sizeof(struct hm_udp_modification_command);
			break;
		}

		/* HM TCP Header Modification Command Verification */
		case HM_TCP_MODIFICATION_CMD_STR:
		{
			struct hm_tcp_modification_command *str =
			(struct hm_tcp_modification_command *) asa_seg_addr;
			str->status = l4_tcp_header_modification(str->flags,
					str->tcp_src_port,
					str->tcp_dst_port,
					str->tcp_seq_num_delta,
					str->tcp_ack_num_delta,
					str->tcp_mss);
			str_size = sizeof(struct hm_tcp_modification_command);
			break;
		}

		/* HM IPv4 NAT Command Verification */
		case HM_NAT_IPV4_CMD_STR:
		{
			struct hm_ipv4_nat_command *str =
			(struct hm_ipv4_nat_command *) asa_seg_addr;
			str->status = nat_ipv4(str->flags,
					str->ip_src_addr,
					str->ip_dst_addr,
					str->l4_src_port,
					str->l4_dst_port,
					str->tcp_seq_num_delta,
					str->tcp_ack_num_delta);
			str_size = sizeof(struct hm_ipv4_nat_command);
			break;
		}

		/* HM IPv6 NAT Command Verification */
		case HM_NAT_IPV6_CMD_STR:
		{
			struct hm_ipv6_nat_command *str =
			(struct hm_ipv6_nat_command *) asa_seg_addr;
			str->status = nat_ipv6(str->flags,
					str->ip_src_addr,
					str->ip_dst_addr,
					str->l4_src_port,
					str->l4_dst_port,
					str->tcp_seq_num_delta,
					str->tcp_ack_num_delta);
			str_size = sizeof(struct hm_ipv6_nat_command);
			break;
		}

		/* HM VLAN VID Command Verification */
		case HM_SET_VLAN_VID_CMD_STR:
		{
			struct hm_vlan_vid_command *str =
			(struct hm_vlan_vid_command *) asa_seg_addr;
			str->status = l2_set_vlan_vid(str->vlan_vid);
			str_size = sizeof(struct hm_vlan_vid_command);
			break;
		}

		/* HM VLAN PCP Command Verification */
		case HM_SET_VLAN_PCP_CMD_STR:
		{
			struct hm_vlan_pcp_command *str =
			(struct hm_vlan_pcp_command *) asa_seg_addr;
			str->status = l2_set_vlan_pcp(str->vlan_pcp);
			str_size = sizeof(struct hm_vlan_pcp_command);
			break;
		}

		/* HM set MAC SRC Command Verification */
		case HM_SET_DL_SRC_CMD_STR:
		{
			struct hm_set_dl_command *str =
			(struct hm_set_dl_command *) asa_seg_addr;
			l2_set_dl_src(str->mac_addr);
			str_size = sizeof(struct hm_set_dl_command);
			break;
		}

		/* HM set MAC DST Command Verification */
		case HM_SET_DL_DST_CMD_STR:
		{
			struct hm_set_dl_command *str =
			(struct hm_set_dl_command *) asa_seg_addr;
			l2_set_dl_dst(str->mac_addr);
			str_size = sizeof(struct hm_set_dl_command);
			break;
		}

		/* HM set NW SRC Command Verification */
		case HM_SET_NW_SRC_CMD_STR:
		{
			struct hm_set_nw_command *str =
			(struct hm_set_nw_command *) asa_seg_addr;
			str->status = ip_set_nw_src(str->ipv4_addr);
			str_size = sizeof(struct hm_set_nw_command);
			break;
		}

		/* HM set NW DST Command Verification */
		case HM_SET_NW_DST_CMD_STR:
		{
			struct hm_set_nw_command *str =
			(struct hm_set_nw_command *) asa_seg_addr;
			str->status = ip_set_nw_dst(str->ipv4_addr);
			str_size = sizeof(struct hm_set_nw_command);
			break;
		}

		/* HM set TCP/UDP SRC Port Command Verification */
		case HM_SET_TP_SRC_CMD_STR:
		{
			struct hm_set_tp_command *str =
			(struct hm_set_tp_command *) asa_seg_addr;
			str->status = l4_set_tp_src(str->port);
			str_size = sizeof(struct hm_set_tp_command);
			break;
		}

		/* HM set TCP/UDP SRC Port Command Verification */
		case HM_SET_TP_DST_CMD_STR:
		{
			struct hm_set_tp_command *str =
			(struct hm_set_tp_command *) asa_seg_addr;
			str->status = l4_set_tp_dst(str->port);
			str_size = sizeof(struct hm_set_tp_command);
			break;
		}

		/* HM push VLAN Command Verification */
		case HM_PUSH_VLAN_CMD_STR:
		{
			struct hm_push_vlan_command *str =
			(struct hm_push_vlan_command *) asa_seg_addr;
			l2_push_vlan(str->ethertype);
			str_size = sizeof(struct hm_push_vlan_command);
			break;
		}

		/* HM pop VLAN Command Verification */
		case HM_POP_VLAN_CMD_STR:
		{
			struct hm_pop_vlan_command *str =
			(struct hm_pop_vlan_command *) asa_seg_addr;
			str->status = l2_pop_vlan();
			str_size = sizeof(struct hm_pop_vlan_command);
			break;
		}
		case HM_IP_CKSUM_CALCULATE_CMD_STR:
		{
			struct hm_ip_cksum_calculation_command *cmd =
				(struct hm_ip_cksum_calculation_command *)
				asa_seg_addr;
			cmd->status = ip_cksum_calculate(
					(struct ipv4hdr *)cmd->ipv4header,
					cmd->flags);
			str_size =
			   sizeof(struct hm_ip_cksum_calculation_command);
			break;
		}
		case HM_L4_UDP_TCP_CKSUM_CALC_CMD_STR:
		{
			struct hm_l4_udp_tcp_cksum_calc_command *cmd =
					(struct hm_l4_udp_tcp_cksum_calc_command *)
					asa_seg_addr;
			cmd->status = l4_udp_tcp_cksum_calc(cmd->flags);
			str_size = sizeof(struct hm_l4_udp_tcp_cksum_calc_command);
			break;
		}
		default:
		{
			return STR_SIZE_ERR;
		}
	}
	return str_size;
}
