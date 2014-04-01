/**************************************************************************//**
@File		aiop_verification_fm.c

@Description	This file contains the AIOP SW Verification Engine

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_fdma.h"
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_gso.h"
#include "dplib/fsl_ipf.h"
#include "aiop_verification.h"
#include "system.h"

/*
__TASK tcp_gso_ctx_t tcp_gso_context_addr;
__TASK ipf_ctx_t ipf_context_addr;
__TASK int32_t status_gso;
__TASK int32_t status_ipf;
*/

extern __VERIF_GLOBAL uint8_t verif_prpid;
extern struct parse_profile_record verif_parse_profile;

void aiop_verification_fm()
{
	uint8_t data_addr[DATA_SIZE];	/* Data Address in workspace*/
	struct fdma_present_segment_params present_params;
	uint64_t ext_address;	/* External Data Address */
	uint64_t initial_ext_address;	/* Initial External Data Address */
	uint16_t str_size;	/* Command struct Size */
	uint32_t opcode;

	/* Read last 8 bytes from frame PTA/ last 8 bytes of payload */
	if (LDPAA_FD_GET_PTA(HWC_FD_ADDRESS)) {
			/* PTA was already loaded */
		if (PRC_GET_PTA_ADDRESS() != PRC_PTA_NOT_LOADED_ADDRESS) {
			ext_address = *((uint64_t *)PRC_GET_PTA_ADDRESS());
		} else { /* PTA was not loaded */
			if (fdma_read_default_frame_pta((void *)data_addr) !=
					FDMA_SUCCESS)
				return;
			ext_address = *((uint64_t *)data_addr);
			PRC_SET_PTA_ADDRESS(PRC_PTA_NOT_LOADED_ADDRESS);
		}
	} else{
		present_params.flags = FDMA_PRES_SR_BIT;
		present_params.frame_handle = PRC_GET_FRAME_HANDLE();
		present_params.offset = 8;
		present_params.present_size = 8;
		present_params.ws_dst = (void *)&ext_address;
		if (fdma_present_frame_segment(&present_params) != FDMA_SUCCESS)
			return;
	}
	initial_ext_address = ext_address;

	/* spid=0. This is a temporary spid setter and has to be removed when
				* the ni function will be run.
				* (According to Ilan request) */
	*((uint8_t *)HWC_SPID_ADDRESS) = 0;

	init_verif();

	/* The Terminate command will finish the verification */
	do {
		/* Read a new buffer from DDR with DATA_SIZE */
		cdma_read((void *)data_addr, ext_address, (uint16_t)DATA_SIZE);

		opcode  = *((uint32_t *) data_addr);
		opcode = (opcode & ACCEL_ID_CMD_MASK) >> 16;

		switch (opcode) {

		case GRO_MODULE:
		{
			str_size = aiop_verification_gro((uint32_t)data_addr);
			break;
		}
		case IPF_MODULE:
		{
			str_size = aiop_verification_ipf((uint32_t)data_addr);
			break;
		}
		case GSO_MODULE:
		{
			str_size = aiop_verification_gso((uint32_t)data_addr);
			break;
		}
		case IPR_MODULE:
		{
			str_size = aiop_verification_ipr((uint32_t)data_addr);
			break;
		}
		case FDMA_MODULE:
		{
			str_size = aiop_verification_fdma((uint32_t)data_addr);
			break;
		}
		case TMAN_MODULE:
		{
			str_size = aiop_verification_tman((uint32_t)data_addr);
			break;
		}
		case STE_MODULE:
		{
			str_size = aiop_verification_ste((uint32_t)data_addr);
			break;
		}
		case CDMA_MODULE:
		{
			str_size = aiop_verification_cdma((uint32_t)data_addr);
			break;
		}
		case TABLE_MODULE:
		{
			str_size = aiop_verification_table((uint32_t)data_addr);
			break;
		}
		case KEYGEN_MODULE:
		{
			str_size = aiop_verification_keygen((uint32_t)data_addr);
			break;
		}
		case PARSE_MODULE:
		{
			str_size = aiop_verification_parser(
					(uint32_t)data_addr);
			break;
		}
		case HM_MODULE:
		{
			str_size = aiop_verification_hm((uint32_t)data_addr);
			break;
		}
		case VPOOL_MODULE:
		{
			str_size = verification_virtual_pools(
					(uint32_t)data_addr);
			break;
		}
		case WRITE_DATA_TO_WS_MODULE:
		{

			struct write_data_to_workspace_command *str =
				(struct write_data_to_workspace_command *)
					data_addr;
			uint8_t i;
			uint8_t *address = (uint8_t *)(str->ws_dst_rs);
			for (i = 0; i < str->size; i++)
				*address++ = str->data[i%32];

			str->status = 0;
			str_size = (uint16_t)
				sizeof(struct write_data_to_workspace_command);
			break;
		}
		case IF_MODULE:
		{
			struct aiop_if_verif_command *str =
				(struct aiop_if_verif_command *)
					((uint32_t)data_addr);
			uint32_t if_result;

			if_result = if_statement_result(
					str->compared_variable_addr,
					str->compared_value, str->cond);

			if (if_result == TERMINATE_FLOW_MODULE) {
				fdma_terminate_task();
				return;
			} else if (if_result) {
				ext_address = initial_ext_address +
					str->true_cmd_offset;
			} else {/* jump to the next sequential command */
				ext_address = ext_address +
					sizeof(struct aiop_if_verif_command);
			}

			break;
		}
		case IF_ELSE_MODULE:
		{
			struct aiop_if_else_verif_command *str =
				(struct aiop_if_else_verif_command *)
							((uint32_t)data_addr);
			uint32_t if_result;

			if_result = if_statement_result(
					str->compared_variable_addr,
					str->compared_value, str->cond);

			if (if_result == TERMINATE_FLOW_MODULE) {
				fdma_terminate_task();
				return;
			} else if (if_result) {
				ext_address = initial_ext_address +
					str->true_cmd_offset;
			} else {
				ext_address = initial_ext_address +
					str->false_cmd_offset;
			}

			break;
		}
		case TERMINATE_FLOW_MODULE:
		default:
		{
			fdma_terminate_task();
			return;
		}
		}


		if (str_size == STR_SIZE_ERR) {
			fdma_terminate_task();
			return;
		} else if ((opcode != IF_MODULE) &&
			 (opcode != IF_ELSE_MODULE)) {
			/* write command results back to DDR */
			cdma_write(ext_address, (void *)data_addr, str_size);
			/* Set next address to read from DDR */
			ext_address += str_size;
		}


	} while (1);

	fdma_terminate_task();

	return;
}

void aiop_verif_init_parser()
{
	uint8_t i, prpid;

	verif_parse_profile.eth_hxs_config = 0x0;
	verif_parse_profile.llc_snap_hxs_config = 0x0;
	verif_parse_profile.vlan_hxs_config.en_erm_soft_seq_start = 0x0;
	verif_parse_profile.vlan_hxs_config.configured_tpid_1 = 0x0;
	verif_parse_profile.vlan_hxs_config.configured_tpid_2 = 0x0;
	/* No MTU checking */
	verif_parse_profile.pppoe_ppp_hxs_config = 0x0;
	verif_parse_profile.mpls_hxs_config.en_erm_soft_seq_start = 0x0;
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
	verif_parse_profile.other_l3_shell_hxs_config = 0x0;
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
	for (i = 0; i < 16; i++)
		verif_parse_profile.soft_examination_param_array[i] = 0x0;
	sys_prpid_pool_create();
	/* Create the parse_profile and get an id */
	parser_profile_create(&verif_parse_profile, &prpid);
	verif_prpid = prpid;
}

uint32_t if_statement_result(
		uint32_t compared_variable_addr,
		int32_t compared_value,
		uint8_t cond)
{
	uint8_t if_result;
	int32_t compared_variable;

	compared_variable = *((int32_t *)compared_variable_addr);
	/*switch (compared_variable_id){
	case COMPARE_GRO_STATUS:
	{
		compared_variable = status_gro;
		break;
	}
	case COMPARE_GSO_STATUS:
	{
		compared_variable = status_gso;
		break;
	}
	case COMPARE_IPR_STATUS:
	{
		compared_variable = status_ipr;
		break;
	}
	case COMPARE_IPF_STATUS:
	{
		compared_variable = status_ipf;
		break;
	}
	case COMPARE_LAST_STATUS:
	{
		compared_variable = status;
		break;
	}
	default:
	{
		return AIOP_TERMINATE_FLOW_CMD;
	}
	}*/

	switch (cond) {
	case COND_EQUAL:
	{
		if_result = (compared_variable == compared_value) ? 1 : 0;
		break;
	}
	case COND_NON_EQUAL:
	{
		if_result = (compared_variable != compared_value) ? 1 : 0;
		break;
	}
	default:
	{
		return TERMINATE_FLOW_MODULE;
	}
	}

	return if_result;
}

