/**************************************************************************//**
@File		aiop_verification_parser.c

@Description	This file contains the AIOP Parser SRs SW Verification

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_parser.h"
#include "system.h"

#include "aiop_verification.h"
#include "aiop_verification_parser.h"

extern __TASK struct aiop_default_task_params default_task_params;

void aiop_init_parser(uint8_t *prpid)
{
	uint8_t i;
	struct parse_profile_record verif_parse_profile1;
	
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
	verif_parse_profile1.mpls_hxs_config.lie_dnp =
					PARSER_PRP_MPLS_HXS_CONFIG_LIE;
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
	/* Assuming no soft examination parameters */
	for(i=0; i<16; i++)
		verif_parse_profile1.soft_examination_param_array[i] = 0x0;
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
	
	opcode  = *((uint32_t *) asa_seg_addr);

	switch (opcode) {
	case PARSER_PRP_CREATE_STR:
	{
		struct parser_prp_create_verif_command *pc =
				(struct parser_prp_create_verif_command *)
				asa_seg_addr;

		pc->status = 
			parser_profile_create(
			(struct parse_profile_record *)pc->parse_profile,
			&pc->prpid);
		str_size = sizeof(struct parser_prp_create_verif_command);
		break;
	}
	case PARSER_PRP_DELETE_STR:
	{
		struct parser_prp_delete_verif_command *pd =
				(struct parser_prp_delete_verif_command *)
				asa_seg_addr;
		pd->status = parser_profile_delete(pd->prpid);
		str_size = sizeof(struct parser_prp_delete_verif_command);
		break;
	}
	case PARSER_PRP_QUERY_STR:
	{
		struct parser_prp_query_verif_command *pq =
				(struct parser_prp_query_verif_command *)
				asa_seg_addr;

/*		pq->status = */
		   parser_profile_query(pq->prpid,
			(struct parse_profile_record *)pq->parse_profile);

		str_size = sizeof(struct parser_prp_query_verif_command);
		break;
	}
	case PARSER_PRP_REPLACE_STR:
	{
		struct parser_prp_replace_verif_command *pq =
				(struct parser_prp_replace_verif_command *)
				asa_seg_addr;

		pq->status = parser_profile_replace(
			(struct parse_profile_record *)pq->parse_profile,
			pq->prpid);

		str_size = sizeof(struct parser_prp_replace_verif_command);
		break;
	}

	case PARSER_GEN_PARSE_RES_STR:
	{
		struct parser_gen_parser_res_verif_command *gpr =
			(struct parser_gen_parser_res_verif_command *)
			asa_seg_addr;
		default_task_params.parser_profile_id = gpr->prpid;
		gpr->status = parse_result_generate_default(gpr->flags);
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
		str_size =
			sizeof(struct parser_gen_parser_res_exp_verif_command);
		break;
	}
	case PARSER_PRP_ID_POOL_CREATE_STR:
	{
		struct parser_prp_id_pool_create_verif_command *pipc =
			(struct parser_prp_id_pool_create_verif_command*)
			asa_seg_addr;
		pipc->status = sys_prpid_pool_create();
		str_size =
			sizeof(struct parser_prp_id_pool_create_verif_command);
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
		
		((struct parse_result *)str->macros_struct)->nxt_hdr = PARSER_GET_NEXT_HEADER_DEFAULT();
		((struct parse_result *)str->macros_struct)->shim_offset_1 = PARSER_GET_SHIM1_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->shim_offset_2 = PARSER_GET_SHIM2_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->ip_pid_offset = PARSER_GET_IP_PID_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->eth_offset = PARSER_GET_ETH_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->llc_snap_offset = PARSER_GET_LLC_SNAP_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->vlan_tci1_offset = PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->vlan_tcin_offset = PARSER_GET_LAST_VLAN_TCI_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->last_etype_offset = PARSER_GET_LAST_ETYPE_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->pppoe_offset = PARSER_GET_PPPOE_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->mpls_offset_1 = PARSER_GET_FIRST_MPLS_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->mpls_offset_n = PARSER_GET_LAST_MPLS_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->ip1_or_arp_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->ip1_or_arp_offset = PARSER_GET_ARP_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->ipn_or_minencapO_offset = PARSER_GET_INNER_IP_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->ipn_or_minencapO_offset = PARSER_GET_MINENCAP_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->gre_offset = PARSER_GET_GRE_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->l4_offset = PARSER_GET_L4_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->gtp_esp_ipsec_offset = PARSER_GET_L5_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->routing_hdr_offset1 = PARSER_GET_1ST_IPV6_ROUTING_HDR_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->routing_hdr_offset2 = PARSER_GET_2ND_IPV6_ROUTING_HDR_OFFSET_DEFAULT(); 
		((struct parse_result *)str->macros_struct)->nxt_hdr_offset = PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->ipv6_frag_offset = PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
		((struct parse_result *)str->macros_struct)->parse_error_code = PARSER_GET_PARSE_ERROR_CODE_DEFAULT(); 
		
		// TODO : Add attributes macros
		
		str_size = sizeof(struct parser_macros_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}

	return str_size;
}

