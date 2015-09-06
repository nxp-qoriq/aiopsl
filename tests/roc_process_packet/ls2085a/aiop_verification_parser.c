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

/**************************************************************************//**
@File		aiop_verification_parser.c

@Description	This file contains the AIOP Parser SRs SW Verification

*//***************************************************************************/

#include "fsl_parser.h"
#include "system.h"

#include "aiop_verification.h"
#include "aiop_verification_parser.h"

extern __TASK struct aiop_default_task_params default_task_params;

void aiop_init_parser(uint8_t *prpid)
{
	uint8_t i;
	struct parse_profile_input verif_parse_profile1 __attribute__((aligned(16)));

	/* Init basic parse profile */
	verif_parse_profile1.parse_profile.eth_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.llc_snap_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.vlan_hxs_config.en_erm_soft_seq_start = 0x0;
	verif_parse_profile1.parse_profile.vlan_hxs_config.configured_tpid_1 = 0x0;
	verif_parse_profile1.parse_profile.vlan_hxs_config.configured_tpid_2 = 0x0;
	/* Enable MTU checking */
	verif_parse_profile1.parse_profile.pppoe_ppp_hxs_config = PARSER_PRP_PPP_HXS_CONFIG_EMC;
	verif_parse_profile1.parse_profile.mpls_hxs_config.en_erm_soft_seq_start= 0x0;
	/* Frame Parsing advances to MPLS Default Next Parse (IP HXS) */
	verif_parse_profile1.parse_profile.mpls_hxs_config.lie_dnp =
					PARSER_PRP_MPLS_HXS_CONFIG_LIE;
	verif_parse_profile1.parse_profile.arp_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.ip_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.ipv4_hxs_config = 0x0;
	/* Routing header is ignored and the destination address from
	 * main header is used instead */
	verif_parse_profile1.parse_profile.ipv6_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.gre_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.minenc_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.other_l3_shell_hxs_config= 0x0;
	/* In short Packet, padding is removed from Checksum calculation */
	verif_parse_profile1.parse_profile.tcp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	/* In short Packet, padding is removed from Checksum calculation */
	verif_parse_profile1.parse_profile.udp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	verif_parse_profile1.parse_profile.ipsec_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.sctp_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.dccp_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.other_l4_shell_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.gtp_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.esp_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.l5_shell_hxs_config = 0x0;
	verif_parse_profile1.parse_profile.final_shell_hxs_config = 0x0;
	/* Assuming no soft examination parameters */
	for(i=0; i<16; i++)
		verif_parse_profile1.parse_profile.soft_examination_param_array[i] = 0x0;
	sys_prpid_pool_create();
	/* Create the parse_profile and get an id */
	parser_profile_create(&verif_parse_profile1, prpid);
	/* Update prpid in task defaults */
	default_task_params.parser_profile_id = *prpid;
}


uint16_t aiop_verification_parser(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint16_t tmp_gross;

	opcode  = *((uint32_t *) asa_seg_addr);

	switch (opcode) {
	case PARSER_GEN_INIT_GROSS_STR:
	{
		struct parser_init_gross_verif_command *pc =
						(struct parser_init_gross_verif_command *)
						asa_seg_addr;
		struct parse_result *pr =
						(struct parse_result *)HWC_PARSE_RES_ADDRESS;
		fdma_calculate_default_frame_checksum(0, 0xFFFF, &tmp_gross);
		pr->gross_running_sum = tmp_gross;
		
		str_size = sizeof(struct parser_init_gross_verif_command);
		break;
	}
	case PARSER_PRP_CREATE_STR:
	{
		struct parser_prp_create_verif_command *pc =
				(struct parser_prp_create_verif_command *)
				asa_seg_addr;
	
		/* Enforced alignment */
		struct parse_profile_input parse_profile __attribute__((aligned(16)));
		
		fdma_copy_data(sizeof(struct parse_profile_input),0,(void *)(pc->parse_profile), (void *)(&parse_profile));

		pc->status =
			parser_profile_create(
			(struct parse_profile_input *)(&parse_profile),
			&pc->prpid);
		
		//Copy back modified fields
		fdma_copy_data(8, 0, (void *)(&parse_profile), (void *)(pc->parse_profile));
		
		*((int32_t *)(pc->parser_status_addr)) = pc->status;
		str_size = sizeof(struct parser_prp_create_verif_command);
		break;
	}
	case PARSER_PRP_DELETE_STR:
	{
		struct parser_prp_delete_verif_command *pd =
				(struct parser_prp_delete_verif_command *)
				asa_seg_addr;
		pd->status = parser_profile_delete(pd->prpid);
		*((int32_t *)(pd->parser_status_addr)) = pd->status;
		str_size = sizeof(struct parser_prp_delete_verif_command);
		break;
	}
	case PARSER_PRP_QUERY_STR:
	{
		struct parser_prp_query_verif_command *pq =
				(struct parser_prp_query_verif_command *)
				asa_seg_addr;
		/* Enforced alignment */
		struct parse_profile_input parse_profile __attribute__((aligned(16)));

		parser_profile_query(pq->prpid, &parse_profile);
	
		fdma_copy_data(sizeof(struct parse_profile_input),0,(void *)(&parse_profile), (void *)(&(pq->parse_profile)));

		str_size = sizeof(struct parser_prp_query_verif_command);
		break;
	}
	case PARSER_PRP_REPLACE_STR:
	{
		struct parser_prp_replace_verif_command *pq =
				(struct parser_prp_replace_verif_command *)
				asa_seg_addr;
		/* Enforced alignment */
		struct parse_profile_input parse_profile __attribute__((aligned(16)));
		
		fdma_copy_data(sizeof(struct parse_profile_input),0,(void *)(pq->parse_profile), (void *)(&parse_profile));

		parser_profile_replace(
			(struct parse_profile_input *)(&parse_profile),
			pq->prpid);

		//Copy back modified fields
		fdma_copy_data(8, 0, (void *)(&parse_profile), (void *)(pq->parse_profile));

		str_size = sizeof(struct parser_prp_replace_verif_command);
		break;
	}
	case PARSER_GEN_PARSE_RES_CHECKSUM_STR:
	{
		struct parser_gen_parser_res_checksum_verif_command *gpr =
			(struct parser_gen_parser_res_checksum_verif_command *)
			asa_seg_addr;
		gpr->status = parse_result_generate_checksum(
						(enum parser_starting_hxs_code)gpr->hxs,
							gpr->offset,
							&gpr->l3_checksum,
							&gpr->l4_checksum);
		*((int32_t *)(gpr->parser_status_addr)) = gpr->status;
		str_size = sizeof(struct parser_gen_parser_res_checksum_verif_command);
		break;
	}
	
	case PARSER_SET_PRPID_HXS_STR:
	{
		struct parser_set_prpid_hxs_command *gpr =
			(struct parser_set_prpid_hxs_command *)
			asa_seg_addr;
		
		PARSER_SET_PRPID(gpr->prpid);					
		PARSER_SET_STARTING_HXS(gpr->starting_hxs);
		
		str_size = sizeof(struct parser_set_prpid_hxs_command);
		break;
	}
	
	case PARSER_GEN_PARSE_RES_STR:
	{
		struct parser_gen_parser_res_verif_command *gpr =
			(struct parser_gen_parser_res_verif_command *)
			asa_seg_addr;
		default_task_params.parser_profile_id = gpr->prpid;
		gpr->status = parse_result_generate_default(gpr->flags);
		*((int32_t *)(gpr->parser_status_addr)) = gpr->status;
		str_size = sizeof(struct parser_gen_parser_res_verif_command);
		break;
	}
	case PARSER_GEN_PARSE_RES_EXP_STR:
	{
		struct parser_gen_parser_res_exp_verif_command *gpre =
			(struct parser_gen_parser_res_exp_verif_command *)
			asa_seg_addr;
		gpre->status = parse_result_generate(
				(enum parser_starting_hxs_code)gpre->hxs,
							 gpre->offset,
							 gpre->flags);
		*((int32_t *)(gpre->parser_status_addr)) = gpre->status;
		str_size =
			sizeof(struct parser_gen_parser_res_exp_verif_command);
		break;
	}
	case PARSER_GEN_PARSE_RES_BASIC_STR:
	{
		struct parser_gen_parser_res_basic_verif_command *gpr =
			(struct parser_gen_parser_res_basic_verif_command *)
			asa_seg_addr;
		gpr->status = parse_result_generate_basic();
		*((int32_t *)(gpr->parser_status_addr)) = gpr->status;
		str_size = 
			sizeof(struct parser_gen_parser_res_basic_verif_command);
		break;
	}
	case PARSER_PRP_ID_POOL_CREATE_STR:
	{
		struct parser_prp_id_pool_create_verif_command *pipc =
			(struct parser_prp_id_pool_create_verif_command*)
			asa_seg_addr;
		sys_prpid_pool_create();
		str_size =
			sizeof(struct parser_prp_id_pool_create_verif_command);
		break;
	}
	case PARSER_SET_FRAME_LENGTH_STR:
	{
		struct parser_set_frame_length_command *str =
		(struct parser_set_frame_length_command *) asa_seg_addr;
		
		LDPAA_FD_SET_LENGTH(HWC_FD_ADDRESS, str->frame_length);

		str_size = sizeof(struct parser_set_frame_length_command);
		break;
	}
	case PARSER_INIT_FOR_VERIF_STR:
	{
		struct parser_init_verif_command *str =
		(struct parser_init_verif_command *) asa_seg_addr;
		default_task_params.parser_starting_hxs =
				str->parser_starting_hxs;
		aiop_init_parser(&(str->prpid));
		str_size = sizeof(struct parser_init_verif_command);
		break;
	}
	case PARSER_MACROS_STR:
	{
		struct parser_macros_command *str =
		(struct parser_macros_command *) asa_seg_addr;
		
		/* SW parse results */
		if (PARSER_IS_VXLAN_DEFAULT())
			str->sw_parse_res.vxlan = (uint8_t)PARSER_IS_VXLAN_DEFAULT();
		if (PARSER_IS_IKE_OVER_UDP_DEFAULT())
			str->sw_parse_res.ike= PARSER_IS_IKE_OVER_UDP_DEFAULT() ;
		if(PARSER_IS_ESP_OVER_UDP_DEFAULT())
			str->sw_parse_res.esp = PARSER_IS_ESP_OVER_UDP_DEFAULT() ;
		
		/* Next header offset */
		str->macros_struct.nxt_hdr = PARSER_GET_NEXT_HEADER_DEFAULT();

		/* Frame Attribute Flags Extension */
		if (PARSER_IS_ROUTING_HDR_IN_2ND_IPV6_HDR_DEFAULT())
			str->macros_struct.frame_attribute_flags_extension = PARSER_IS_ROUTING_HDR_IN_2ND_IPV6_HDR_DEFAULT();
		
		/* Frame Attribute Flags 1 */ 

		if (PARSER_IS_SHIM_SOFT_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_SHIM_SOFT_PARSING_ERROR_DEFAULT();
		if (PARSER_IS_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_PARSING_ERROR_DEFAULT();
		if (PARSER_IS_ETH_MAC_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ETH_MAC_DEFAULT();
		if (PARSER_IS_ETH_MAC_UNICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ETH_MAC_UNICAST_DEFAULT() ;
		if (PARSER_IS_ETH_MAC_MULTICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ETH_MAC_MULTICAST_DEFAULT() ;
		if (PARSER_IS_ETH_MAC_BROADCAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ETH_MAC_BROADCAST_DEFAULT() ;
		if (PARSER_IS_BPDU_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_BPDU_DEFAULT();
		if (PARSER_IS_FCOE_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_FCOE_DEFAULT() ;
		if (PARSER_IS_FCOE_INIT_PROTOCOL_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_FCOE_INIT_PROTOCOL_DEFAULT() ;
		if (PARSER_IS_ETH_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ETH_PARSING_ERROR_DEFAULT();
		if (PARSER_IS_LLC_SNAP_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_LLC_SNAP_DEFAULT();
		if (PARSER_IS_UNKNOWN_LLC_OUI_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_UNKNOWN_LLC_OUI_DEFAULT() ;
		if (PARSER_IS_LLC_SNAP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_LLC_SNAP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_ONE_VLAN_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ONE_VLAN_DEFAULT() ;
		if (PARSER_IS_MORE_THAN_ONE_VLAN_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_MORE_THAN_ONE_VLAN_DEFAULT() ;
		if (PARSER_IS_CFI_IN_VLAN_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_CFI_IN_VLAN_DEFAULT() ;
		if (PARSER_IS_VLAN_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_VLAN_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_PPPOE_PPP_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_PPPOE_PPP_DEFAULT();
		if (PARSER_IS_PPPOE_PPP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_PPPOE_PPP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_ONE_MPLS_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ONE_MPLS_DEFAULT() ;
		if (PARSER_IS_MORE_THAN_ONE_MPLS_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_MORE_THAN_ONE_MPLS_DEFAULT() ;
		if (PARSER_IS_MPLS_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_MPLS_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_ARP_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ARP_DEFAULT() ;
		if (PARSER_IS_ARP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_1 |= PARSER_IS_ARP_PARSING_ERROR_DEFAULT();

		/* Frame Attribute Flags 2 */
		if (PARSER_IS_L2_UNKNOWN_PROTOCOL_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_L2_UNKNOWN_PROTOCOL_DEFAULT() ;
		if (PARSER_IS_L2_SOFT_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_L2_SOFT_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_OUTER_IPV4_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IPV4_DEFAULT() ;
		if (PARSER_IS_OUTER_IPV4_UNICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IPV4_UNICAST_DEFAULT() ;
		if (PARSER_IS_OUTER_IPV4_MULTICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IPV4_MULTICAST_DEFAULT() ;
		if (PARSER_IS_OUTER_IPV4_BROADCAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IPV4_BROADCAST_DEFAULT() ;
		if (PARSER_IS_INNER_IPV4_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IPV4_DEFAULT() ;
		if (PARSER_IS_INNER_IPV4_UNICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IPV4_UNICAST_DEFAULT() ;
		if (PARSER_IS_INNER_IPV4_MULTICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IPV4_MULTICAST_DEFAULT() ;
		if (PARSER_IS_INNER_IPV4_BROADCAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IPV4_BROADCAST_DEFAULT() ;
		if (PARSER_IS_OUTER_IPV6_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IPV6_DEFAULT() ;
		if (PARSER_IS_OUTER_IPV6_UNICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IPV6_UNICAST_DEFAULT() ;
		if (PARSER_IS_OUTER_IPV6_MULTICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IPV6_MULTICAST_DEFAULT();
		if (PARSER_IS_INNER_IPV6_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IPV6_DEFAULT() ;
		if (PARSER_IS_INNER_IPV6_UNICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IPV6_UNICAST_DEFAULT() ;
		if (PARSER_IS_INNER_IPV6_MULTICAST_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IPV6_MULTICAST_DEFAULT() ;
		if (PARSER_IS_OUTER_IP_OPTIONS_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IP_OPTIONS_DEFAULT() ;
		if (PARSER_IS_OUTER_IP_UNKNOWN_PROTOCOL_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IP_UNKNOWN_PROTOCOL_DEFAULT() ;
		if (PARSER_IS_OUTER_IP_FRAGMENT_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IP_FRAGMENT_DEFAULT() ;
		if (PARSER_IS_OUTER_IP_INIT_FRAGMENT_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IP_INIT_FRAGMENT_DEFAULT() ;
		if (PARSER_IS_OUTER_IP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_OUTER_IP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_INNER_IP_OPTIONS_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IP_OPTIONS_DEFAULT() ;
		if (PARSER_IS_INNER_IP_UNKNOWN_PROTOCOL_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IP_UNKNOWN_PROTOCOL_DEFAULT();
		if (PARSER_IS_INNER_IP_FRAGMENT_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IP_FRAGMENT_DEFAULT();
		if (PARSER_IS_INNER_IP_INIT_FRAGMENT_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IP_INIT_FRAGMENT_DEFAULT() ;
		if (PARSER_IS_ICMP_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_ICMP_DEFAULT();
		if (PARSER_IS_IGMP_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_IGMP_DEFAULT() ;
		if (PARSER_IS_ICMPV6_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_ICMPV6_DEFAULT();
		if (PARSER_IS_UDP_LITE_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_UDP_LITE_DEFAULT() ;
		if (PARSER_IS_INNER_IP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_INNER_IP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_MIN_ENCAP_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_MIN_ENCAP_DEFAULT();
		if (PARSER_IS_MIN_ENCAP_S_FLAG_DEFAULT())
			str->macros_struct.frame_attribute_flags_2 |= PARSER_IS_MIN_ENCAP_S_FLAG_DEFAULT();

		/* Frame Attribute Flags 3 */
		if (PARSER_IS_MIN_ENCAP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_MIN_ENCAP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_GRE_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_GRE_DEFAULT() ;
		if (PARSER_IS_GRE_R_BIT_SET_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_GRE_R_BIT_SET_DEFAULT() ;
		if (PARSER_IS_GRE_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_GRE_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_L3_UNKOWN_PROTOCOL_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_L3_UNKOWN_PROTOCOL_DEFAULT() ;
		if (PARSER_IS_L3_SOFT_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_L3_SOFT_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_UDP_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_UDP_DEFAULT() ;
		if (PARSER_IS_UDP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_UDP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_TCP_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_TCP_DEFAULT() ;
		if (PARSER_IS_TCP_OPTIONS_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_TCP_OPTIONS_DEFAULT() ;
		if (PARSER_IS_TCP_CONTROLS_6_11_SET_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_TCP_CONTROLS_6_11_SET_DEFAULT() ;
		if (PARSER_IS_TCP_CONTROLS_3_5_SET_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_TCP_CONTROLS_3_5_SET_DEFAULT() ;
		if (PARSER_IS_TCP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_TCP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_IPSEC_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_IPSEC_DEFAULT() ;
		if (PARSER_IS_IPSEC_ESP_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_IPSEC_ESP_DEFAULT() ;
		if (PARSER_IS_IPSEC_AH_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_IPSEC_AH_DEFAULT() ;
		if (PARSER_IS_IPSEC_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_IPSEC_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_SCTP_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_SCTP_DEFAULT() ;
		if (PARSER_IS_SCTP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_SCTP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_DCCP_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_DCCP_DEFAULT() ;
		if (PARSER_IS_DCCP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_DCCP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_L4_UNKOWN_PROTOCOL_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_L4_UNKOWN_PROTOCOL_DEFAULT() ;
		if (PARSER_IS_L4_SOFT_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_L4_SOFT_PARSING_ERROR_DEFAULT();
		if (PARSER_IS_GTP_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_GTP_DEFAULT() ;
		if (PARSER_IS_GTP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_GTP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_ESP_OR_IKE_OVER_UDP_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_ESP_OR_IKE_OVER_UDP_DEFAULT() ;
		if (PARSER_IS_ESP_OR_IKE_OVER_UDP_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_ESP_OR_IKE_OVER_UDP_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_ISCSI_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_ISCSI_DEFAULT() ;
		if (PARSER_IS_CAPWAP_CONTROL_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_CAPWAP_CONTROL_DEFAULT() ;
		if (PARSER_IS_CAPWAP_DATA_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_CAPWAP_DATA_DEFAULT() ;
		if (PARSER_IS_L5_SOFT_PARSING_ERROR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_L5_SOFT_PARSING_ERROR_DEFAULT() ;
		if (PARSER_IS_ROUTING_HDR_IN_1ST_IPV6_HDR_DEFAULT())
			str->macros_struct.frame_attribute_flags_3 |= PARSER_IS_ROUTING_HDR_IN_1ST_IPV6_HDR_DEFAULT();

		/* Offsets */			
		str->macros_struct.shim_offset_1 = PARSER_GET_SHIM1_OFFSET_DEFAULT();
		str->macros_struct.shim_offset_2 = PARSER_GET_SHIM2_OFFSET_DEFAULT();
		str->macros_struct.ip_pid_offset = PARSER_GET_IP_PID_OFFSET_DEFAULT();
		str->macros_struct.eth_offset = PARSER_GET_ETH_OFFSET_DEFAULT();
		str->macros_struct.llc_snap_offset = PARSER_GET_LLC_SNAP_OFFSET_DEFAULT();
		str->macros_struct.vlan_tci1_offset = PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT();
		str->macros_struct.vlan_tcin_offset = PARSER_GET_LAST_VLAN_TCI_OFFSET_DEFAULT();
		str->macros_struct.last_etype_offset = PARSER_GET_LAST_ETYPE_OFFSET_DEFAULT();
		str->macros_struct.pppoe_offset = PARSER_GET_PPPOE_OFFSET_DEFAULT();
		str->macros_struct.mpls_offset_1 = PARSER_GET_FIRST_MPLS_OFFSET_DEFAULT();
		str->macros_struct.mpls_offset_n = PARSER_GET_LAST_MPLS_OFFSET_DEFAULT();
		str->macros_struct.ip1_or_arp_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		str->macros_struct.ip1_or_arp_offset = PARSER_GET_ARP_OFFSET_DEFAULT();
		str->macros_struct.ipn_or_minencapO_offset = PARSER_GET_INNER_IP_OFFSET_DEFAULT();
		str->macros_struct.ipn_or_minencapO_offset = PARSER_GET_MINENCAP_OFFSET_DEFAULT();
		str->macros_struct.gre_offset = PARSER_GET_GRE_OFFSET_DEFAULT();
		str->macros_struct.l4_offset = PARSER_GET_L4_OFFSET_DEFAULT();
		str->macros_struct.gtp_esp_ipsec_offset = PARSER_GET_L5_OFFSET_DEFAULT();
		str->macros_struct.routing_hdr_offset1 = PARSER_GET_1ST_IPV6_ROUTING_HDR_OFFSET_DEFAULT();
		str->macros_struct.routing_hdr_offset2 = PARSER_GET_2ND_IPV6_ROUTING_HDR_OFFSET_DEFAULT();
		str->macros_struct.nxt_hdr_offset = PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT();
		str->macros_struct.ipv6_frag_offset = PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
		str->macros_struct.gross_running_sum = PARSER_GET_GROSS_RUNNING_SUM_CODE_DEFAULT();
		str->macros_struct.running_sum = PARSER_GET_RUNNING_SUM_DEFAULT();

		
		/* FSL_PARSER_ERROR_CODES */
		switch (PARSER_GET_PARSE_ERROR_CODE_DEFAULT()){	
		case PARSER_EXCEED_BLOCK_LIMIT: 
			str->macros_struct.parse_error_code = PARSER_EXCEED_BLOCK_LIMIT;
			break;
		case PARSER_FRAME_TRUNCATION: 
			str->macros_struct.parse_error_code = PARSER_FRAME_TRUNCATION;
			break;
		case PARSER_ETH_802_3_TRUNCATION: 
			str->macros_struct.parse_error_code = PARSER_ETH_802_3_TRUNCATION;
			break;
		case PARSER_PPPOE_TRUNCATION: 
			str->macros_struct.parse_error_code = PARSER_PPPOE_TRUNCATION;
			break;
		case PARSER_PPPOE_MTU_VIOLATED: 
			str->macros_struct.parse_error_code = PARSER_PPPOE_MTU_VIOLATED;
			break;
		case PARSER_PPPOE_VERSION_INVALID: 
			str->macros_struct.parse_error_code = PARSER_PPPOE_VERSION_INVALID;
			break;
		case PARSER_PPPOE_TYPE_INVALID: 
			str->macros_struct.parse_error_code = PARSER_PPPOE_TYPE_INVALID;
			break;
		case PARSER_PPPOE_CODE_INVALID: 
			str->macros_struct.parse_error_code = PARSER_PPPOE_CODE_INVALID;
			break;
		case PARSER_PPPOE_SESSION_ID_INVALID: 
			str->macros_struct.parse_error_code = PARSER_PPPOE_SESSION_ID_INVALID;
			break;
		case PARSER_IPV4_PACKET_TRUNCATION: 
			str->macros_struct.parse_error_code = PARSER_IPV4_PACKET_TRUNCATION;
			break;
		case PARSER_IPV4_CHECKSUM_ERROR: 
			str->macros_struct.parse_error_code = PARSER_IPV4_CHECKSUM_ERROR;
			break;
		case PARSER_IPV4_VERSION_ERROR: 
			str->macros_struct.parse_error_code = PARSER_IPV4_VERSION_ERROR;
			break;
		case PARSER_IPV4_MIN_FRAG_SIZE_ERROR: 
			str->macros_struct.parse_error_code = PARSER_IPV4_MIN_FRAG_SIZE_ERROR;
			break;
		case PARSER_IPV4_HEADER_LENGTH_ERROR: 
			str->macros_struct.parse_error_code = PARSER_IPV4_HEADER_LENGTH_ERROR;
			break;
		case PARSER_IPV6_PACKET_TRUNCATION: 
			str->macros_struct.parse_error_code = PARSER_IPV6_PACKET_TRUNCATION;
			break;
		case PARSER_IPV6_EXTENSION_HEADER_VIOLATION: 
			str->macros_struct.parse_error_code = PARSER_IPV6_EXTENSION_HEADER_VIOLATION;
			break;
		case PARSER_IPV6_VERSION_ERROR: 
			str->macros_struct.parse_error_code = PARSER_IPV6_VERSION_ERROR;
			break;
		case PARSER_IPV6_ROUTING_HEADER_ERROR: 
			str->macros_struct.parse_error_code = PARSER_IPV6_ROUTING_HEADER_ERROR;
			break;
		case PARSER_GRE_VERSION_ERROR: 
			str->macros_struct.parse_error_code = PARSER_GRE_VERSION_ERROR;
			break;
		case PARSER_MINENC_CHECKSUM_ERROR: 
			str->macros_struct.parse_error_code = PARSER_MINENC_CHECKSUM_ERROR;
			break;
		case PARSER_TCP_INVALID_OFFSET: 
			str->macros_struct.parse_error_code = PARSER_TCP_INVALID_OFFSET;
			break;
		case PARSER_TCP_PACKET_TRUNCATION: 
			str->macros_struct.parse_error_code = PARSER_TCP_PACKET_TRUNCATION;
			break;
		case PARSER_TCP_CHECKSUM_ERROR: 
			str->macros_struct.parse_error_code = PARSER_TCP_CHECKSUM_ERROR;
			break;
		case PARSER_TCP_BAD_FLAGS: 
			str->macros_struct.parse_error_code = PARSER_TCP_BAD_FLAGS;
			break;
		case PARSER_UDP_LENGTH_ERROR: 
			str->macros_struct.parse_error_code = PARSER_UDP_LENGTH_ERROR;
			break;
		case PARSER_UDP_CHECKSUM_ZERO: 
			str->macros_struct.parse_error_code = PARSER_UDP_CHECKSUM_ZERO;
			break;
		case PARSER_UDP_CHECKSUM_ERROR: 
			str->macros_struct.parse_error_code = PARSER_UDP_CHECKSUM_ERROR;
			break;
		case PARSER_SCTP_PORT_0_DETECTED: 
			str->macros_struct.parse_error_code = PARSER_SCTP_PORT_0_DETECTED;
			break;
		case PARSER_GTP_UNSUPPORTED_VERSION: 
			str->macros_struct.parse_error_code = PARSER_GTP_UNSUPPORTED_VERSION;
			break;
		case PARSER_GTP_INVALID_PROTOCOL_TYPE: 
			str->macros_struct.parse_error_code = PARSER_GTP_INVALID_PROTOCOL_TYPE;
			break;
		case PARSER_GTP_INVALID_L_BIT_ERROR: 
			str->macros_struct.parse_error_code = PARSER_GTP_INVALID_L_BIT_ERROR;
			break;
		default:
			str->macros_struct.parse_error_code = PARSER_GET_PARSE_ERROR_CODE_DEFAULT();
			break;
		}

		str_size = sizeof(struct parser_macros_command);
		break;
	}
	case PARSER_PARSE_AFTER_POP_VLAN_STR: 
	{
		uint16_t vlan_offset;
		uint32_t fdma_flags;
		struct parser_parse_after_pop_vlan_verif_command *pv =
			(struct parser_parse_after_pop_vlan_verif_command *)
			asa_seg_addr;

		if (PARSER_IS_ONE_VLAN_DEFAULT()) {
			vlan_offset = (uint16_t)(PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT()) - 
					PARSER_TCI_DIST_FROM_START_OF_VLAN; //-2 for TCI offset
			fdma_flags = FDMA_REPLACE_SA_REPRESENT_BIT;

			/* Remove first VLAN header */
			fdma_delete_default_segment_data(vlan_offset,
							sizeof(struct vlanhdr),
							fdma_flags);
			/* Update parse result table */
			parser_pop_vlan_update();
		}
		str_size =
			sizeof(struct parser_parse_after_pop_vlan_verif_command);
		break;
	}
	case PARSER_PARSE_AFTER_PUSH_VLAN_STR:
	{
		uint16_t to_offset;
		uint32_t fdma_flags;
		struct parser_parse_after_push_vlan_verif_command *pv =
			(struct parser_parse_after_push_vlan_verif_command *)
			asa_seg_addr;
		struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
		
		/* Add a VLAN header */
		to_offset = (uint16_t)pr->eth_offset + 2*NET_HDR_FLD_ETH_ADDR_SIZE;
		fdma_flags = FDMA_REPLACE_SA_REPRESENT_BIT;
		
		fdma_insert_default_segment_data(to_offset, 
				&(pv->vlan),
				sizeof(struct vlanhdr), 
				fdma_flags);
		
		/* Update parse result table */
		parser_push_vlan_update();
		
		str_size =
			sizeof(struct parser_parse_after_push_vlan_verif_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}

	return str_size;
}

