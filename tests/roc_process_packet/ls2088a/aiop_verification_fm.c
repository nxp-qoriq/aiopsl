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

/**************************************************************************//**
@File		aiop_verification_fm.c

@Description	This file contains the AIOP SW Verification Engine

*//***************************************************************************/

#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_gso.h"
#include "fsl_ipf.h"
#include "fsl_cwapf.h"
#include "aiop_verification.h"
#include "system.h"

#include "fsl_cmgw.h"


/*
__TASK tcp_gso_ctx_t tcp_gso_context_addr;
__TASK ipf_ctx_t ipf_context_addr;
__TASK int32_t status_gso;
__TASK int32_t status_ipf;
*/

extern __VERIF_GLOBAL uint8_t verif_prpid;
extern __VERIF_TLS uint64_t initial_ext_address;
extern __VERIF_TLS uint8_t slab_parser_error;
extern __VERIF_TLS uint8_t slab_keygen_error;
extern __VERIF_TLS uint8_t slab_general_error;

extern __PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];

__declspec(entry_point) void aiop_verification_fm()
{
	/* Data Address in workspace*/
	uint8_t data_addr[DATA_SIZE] __attribute__((aligned(64)));
	struct fdma_present_segment_params present_params;
	uint64_t ext_address;	/* External Data Address */
	uint16_t str_size = 0;	/* Command struct Size */
	uint32_t opcode;
	uint16_t flags = 0;
	uint16_t segment_offset;
	uint8_t spid;
	uint32_t *pData = (uint32_t *)HWC_FD_ADDRESS;

	fsl_print("\n**************************\n");
	fsl_print("*** Start Verification ***\n");
	fsl_print("**************************\n");
	/* Read 8 bytes from frame PTA/ last 8 bytes of payload
	 * This is the external buffer address */
	if (LDPAA_FD_GET_PTA(HWC_FD_ADDRESS)) {
		/* fix CR:ENGR00364084 */
		segment_offset = *((uint16_t *)(HWC_PRC_ADDRESS + 0x8));
		/* load PTA */
		if (fdma_read_default_frame_pta((void *)data_addr) !=
				FDMA_SUCCESS)
			return;
		ext_address = *((uint64_t *)data_addr);
		slab_parser_error = *((uint8_t *)data_addr + 8);
		slab_keygen_error = *((uint8_t *)data_addr + 9);
		/* fix CR:ENGR00364084 */
		*((uint16_t *)(HWC_PRC_ADDRESS + 0x8)) = segment_offset;
		fsl_print("Commands in PTA : ext_address = 0x%x-%08x\n",
			  (uint32_t)(ext_address >> 32), (uint32_t)ext_address);
	} else {
		present_params.flags = FDMA_PRES_SR_BIT;
		present_params.frame_handle = PRC_GET_FRAME_HANDLE();
		present_params.offset = 10;
		present_params.present_size = 10;
		present_params.ws_dst = (void *)data_addr;
		if (fdma_present_frame_segment(&present_params) != FDMA_SUCCESS)
			return;
		slab_keygen_error = data_addr[0];
		slab_parser_error = data_addr[1];
		ext_address = *((uint64_t *)(&(data_addr[2])));
		fsl_print("Commands in ASA : ext_address = 0x%x-%08x\n",
			  (uint32_t)(ext_address >> 32), (uint32_t)ext_address);
	}
	initial_ext_address = ext_address;
	slab_general_error = 0;

	init_verif();
	sl_prolog();
#ifndef LS2085A_REV1
	/* Set CDMA AxCache attributes as they are expected by the ROC MC test
	 * application. */
	cdma_set_data_write_attributes(NON_COHERENT_NO_CACHE_LKUP, ALLOC_NONE);
	cdma_set_data_read_attributes(NON_COHERENT_NO_CACHE_LKUP, ALLOC_NONE);
#endif
	spid = *((uint8_t *)HWC_SPID_ADDRESS);

	if (LDPAA_FD_GET_ASAL(HWC_FD_ADDRESS) > 
	    (storage_profile[spid].mode_bits1 & 0x0F))
	{
		/*set storage profile ASAR */
		storage_profile[spid].mode_bits1 &= 0xF0;
		storage_profile[spid].mode_bits1 |= LDPAA_FD_GET_ASAL(HWC_FD_ADDRESS);    
	}
	/*set storage profile PTAR to 1 */
	if (LDPAA_FD_GET_PTA(HWC_FD_ADDRESS))
		storage_profile[spid].mode_bits1 |= 0x80;
	/*set storage profile PTAR to 0 */
	else
		storage_profile[spid].mode_bits1 &= 0x7F;

	cdma_read((void *)data_addr, ext_address, (uint16_t)DATA_SIZE);

	/* The Terminate command will finish the verification */
	do {
		/* Read a new buffer from DDR with DATA_SIZE */
		cdma_read((void *)data_addr, ext_address, (uint16_t)DATA_SIZE);

		opcode  = *((uint32_t *) data_addr);
		fsl_print("opcode received: 0x%x\n", opcode);
		/*
		 * Is this the 0x55AA55AA pseudo-command (see AIOPSL-1177) ?
		 *
		 * To avoid AIOP to ROC MC cache coherence issue, the ROC MC
		 * should append the 0x55AA55AA pseudo-command and wait on the
		 * pseudo-command offset till the value becomes 0xAA55AA55.
		 *
		 * Here the pseudo-command is detected and replaced with the
		 * expected "response" 0xAA55AA55.
		 *
		 */
		if (opcode == 0x55AA55AA) {
			uint32_t	*mark_test_end;

			/* End of commands list detected */
			/*fsl_print(" End of test detected !\n");*/
			mark_test_end = (uint32_t *)data_addr;
			*mark_test_end = 0xAA55AA55;
			str_size = sizeof(uint32_t);
			cdma_write(ext_address, (void *)data_addr, str_size);
			fdma_terminate_task();
		}
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
		case CWAPF_MODULE:
		{
			str_size = aiop_verification_cwapf((uint32_t)data_addr);
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
		case IPSEC_MODULE:
		{
			str_size = aiop_verification_ipsec(
					(uint32_t)data_addr);
			break;
		}
		case OSM_MODULE:
		{
			str_size = aiop_verification_osm(
					(uint32_t)data_addr);
			break;
		}
		case WRITE_DATA_TO_WS_MODULE:
		{

			struct write_data_to_workspace_command *str =
				(struct write_data_to_workspace_command *)
					data_addr;
			uint16_t i;
			uint8_t *address = (uint8_t *)(str->ws_dst_rs);
			for (i = 0; i < str->size; i++)
				*address++ = str->data[i%32];

			str->status = 0;
			str_size = (uint16_t)
				sizeof(struct write_data_to_workspace_command);
			break;
		}
		case UPDATE_EXT_VARIABLE:
		{

			struct update_ext_cmd_var_command *str =
				(struct update_ext_cmd_var_command *)
					data_addr;
			uint32_t value;
			cdma_read_with_mutex(initial_ext_address + str->offset,
						CDMA_PREDMA_MUTEX_WRITE_LOCK,
						&value,
						4);
			switch (str->operation)
			{
			case INCREMENT_OPER: value = value + str->value; break;
			case DECREMENT_OPER: value = value - str->value; break;
			case SET_OPER: 	value = str->value; break;	
			default:
				value = str->value;
			}
			
			cdma_write_with_mutex(initial_ext_address + str->offset,
						CDMA_POSTDMA_MUTEX_RM_BIT,
						&value,
						4);
			
			str_size = (uint16_t)
				sizeof(struct update_ext_cmd_var_command);
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
					str->compared_size,
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
					str->compared_size,
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
		case EXCEPTION_MODULE:
		{
			str_size = (uint16_t)
			  sizeof(struct fatal_error_command);
			break;
		}
		case UPDATE_DEFAULT_SP_ASAR:
		{
			struct update_default_sp_asar_command *str =
				(struct update_default_sp_asar_command *)
							((uint32_t)data_addr);

	        /*set storage profile ASAR */
	        storage_profile[spid].mode_bits1 &= 0xF0;
	        storage_profile[spid].mode_bits1 |= str->asar_val;

			str_size = (uint16_t)
			  sizeof(struct update_default_sp_asar_command);
			break;
		}
		case UPDATE_DEFAULT_SP_PTAR:
		{
			struct update_default_sp_ptar_command *str =
				(struct update_default_sp_ptar_command *)
							((uint32_t)data_addr);

	        /*set storage profile PTAR to 1 */
	        if (str->ptar_val)
	        	storage_profile[spid].mode_bits1 |= 0x80;
	        /*set storage profile PTAR to 0 */
	        else
	        	storage_profile[spid].mode_bits1 &= 0x7F;
			
			str_size = (uint16_t)
			  sizeof(struct update_default_sp_ptar_command);
			break;
		}
		case CWAPR_MODULE:
		{
			str_size = aiop_verification_cwapr((uint32_t)data_addr);
			break;
		}
		case INFINATE_LOOP_MODULE:
			while (1) {
				/* Let other tasks to be scheduled on
				 * this core */
				__e_hwacceli(YIELD_ACCEL_ID);
				/* Do some dummy processing */
				fsl_print("");
			}
			break;
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
	struct parse_profile_input verif_parse_profile __attribute__((aligned(16)));

	verif_parse_profile.parse_profile.eth_hxs_config = 0x0;
	verif_parse_profile.parse_profile.llc_snap_hxs_config = 0x0;
	verif_parse_profile.parse_profile.vlan_hxs_config.en_erm_soft_seq_start = 0x0;
	verif_parse_profile.parse_profile.vlan_hxs_config.configured_tpid_1 = 0x0;
	verif_parse_profile.parse_profile.vlan_hxs_config.configured_tpid_2 = 0x0;
	/* No MTU checking */
	verif_parse_profile.parse_profile.pppoe_ppp_hxs_config = 0x0;
	verif_parse_profile.parse_profile.mpls_hxs_config.en_erm_soft_seq_start = 0x0;
	/* Frame Parsing advances to MPLS Default Next Parse (IP HXS) */
	verif_parse_profile.parse_profile.mpls_hxs_config.lie_dnp =
			PARSER_PRP_MPLS_HXS_CONFIG_LIE;
	verif_parse_profile.parse_profile.arp_hxs_config = 0x0;
	verif_parse_profile.parse_profile.ip_hxs_config = 0x0;
	verif_parse_profile.parse_profile.ipv4_hxs_config = 0x0;
	/* Final destination address is used */
	verif_parse_profile.parse_profile.ipv6_hxs_config = PARSER_PRP_IPV6_HXS_CONFIG_RHE;
	verif_parse_profile.parse_profile.gre_hxs_config = 0x0;
	verif_parse_profile.parse_profile.minenc_hxs_config = 0x0;
	verif_parse_profile.parse_profile.other_l3_shell_hxs_config = 0x0;
	/* In short Packet, padding is removed from Checksum calculation */
	verif_parse_profile.parse_profile.tcp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	/* In short Packet, padding is removed from Checksum calculation */
	verif_parse_profile.parse_profile.udp_hxs_config = PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR;
	verif_parse_profile.parse_profile.ipsec_hxs_config = 0x0;
	verif_parse_profile.parse_profile.sctp_hxs_config = 0x0;
	verif_parse_profile.parse_profile.dccp_hxs_config = 0x0;
	verif_parse_profile.parse_profile.other_l4_shell_hxs_config = 0x0;
	verif_parse_profile.parse_profile.gtp_hxs_config = 0x0;
	verif_parse_profile.parse_profile.esp_hxs_config = 0x0;
	verif_parse_profile.parse_profile.l5_shell_hxs_config = 0x0;
	verif_parse_profile.parse_profile.final_shell_hxs_config = 0x0;
	/* Assuming no soft examination parameters */
	for (i = 0; i < 16; i++)
		verif_parse_profile.parse_profile.soft_examination_param_array[i] = 0x0;
	//sys_prpid_pool_create();
	/* Create the parse_profile and get an id */
	parser_profile_create(&verif_parse_profile, &prpid);
	verif_prpid = prpid;
}

uint32_t if_statement_result(
		uint16_t compared_variable_addr,
		uint32_t size,
		int64_t compared_value,
		uint8_t cond)
{
	uint8_t if_result;
	int64_t compared_variable;

	switch (size) {
	case COMPARE_1BYTE:
	{
		compared_variable =
			(int64_t)(*((int8_t *)compared_variable_addr));
		break;
	}
	case COMPARE_2BYTE:
	{
		compared_variable =
			(int64_t)(*((int16_t *)compared_variable_addr));
		break;
	}
	case COMPARE_4BYTE:
	{
		compared_variable =
			(int64_t)(*((int32_t *)compared_variable_addr));
		break;
	}
	case COMPARE_8BYTE:
	{
		compared_variable = *((int64_t *)compared_variable_addr);
		break;
	}
	default:
	{
		compared_variable =
			(int64_t)(*((int32_t *)compared_variable_addr));
		break;
	}
	}

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

void timeout_cb_verif(uint64_t arg)
{
	struct fdma_enqueue_wf_command str;
	struct fdma_queueing_destination_params qdp;
	//int32_t status;
	uint32_t flags = 0;

	if (arg == 0)
		return;
	cdma_read((void *)&str, arg,
			(uint16_t)sizeof(struct fdma_enqueue_wf_command));

	*(uint8_t *) HWC_SPID_ADDRESS = str.spid;
	flags |= ((str.TC == 1) ? (FDMA_EN_TC_TERM_BITS) : 0x0);
	flags |= ((str.PS) ? FDMA_ENWF_PS_BIT : 0x0);

	/* W/A to set NI Storage Profile PTAR the same as FD PTA.
	 * This is needed since generic code uses NI spid while ROC works
	 * without NI (results verification environment to use a fault spid). */
	if (LDPAA_FD_GET_PTA(HWC_FD_ADDRESS)) /* set SP PTAR to 1*/
		storage_profile[GET_DEFAULT_SPID()].mode_bits1 |= 0x80;
	else /*set SP PTAR to 0 */
		storage_profile[GET_DEFAULT_SPID()].mode_bits1 &= 0x7F;
	
	if (str.EIS) {
		str.status = (int8_t)
			fdma_store_and_enqueue_default_frame_fqid(
				str.qd_fqid, flags);
	} else{
		qdp.qd = (uint16_t)(str.qd_fqid);
		qdp.qdbin = str.qdbin;
		qdp.qd_priority = str.qd_priority;
		str.status = (int8_t)
			fdma_store_and_enqueue_default_frame_qd(
					&qdp, flags);
	}
	fdma_terminate_task();
}
