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
@File		aiop_verification_ipsec.c

@Description	This file contains the AIOP IPsec SW Verification Commands

*//***************************************************************************/

//#define IPSEC_OVERRIDE_RTA

#include "aiop_verification.h"
#include "aiop_verification_ipsec.h"
#include "ipsec.h"

#include "rta.h"

#ifndef IPSEC_OVERRIDE_RTA
//#include "protoshared.h"
//#include "desc/ipsec.h"

//#endif
//#include "sec_app.h"
//#include "ipsec_test_vector.h"
#endif

__VERIF_GLOBAL uint64_t sa_desc_handle[32]; /* Global in Shared RAM */
__VERIF_GLOBAL uint64_t verif_instance_handle[32]; /* Global in Shared RAM */

extern __TASK struct aiop_default_task_params default_task_params;

uint16_t  aiop_verification_ipsec(uint32_t data_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint64_t ws_sa_desc_handle; /* Temporary Workspace place holder*/
	uint64_t ws_verif_instance_handle; /* Temporary Workspace place holder*/

	opcode  = *((uint32_t *) data_addr);

	switch (opcode) {
//	case IPSEC_INIT_CMD:
//	{
//		struct ipsec_init_command *str =
//			(struct ipsec_init_command *)data_addr;
//		
//		str->status = ipsec_init(
//				str->max_sa_no);
//		*((int32_t *)(str->status_addr)) = str->status;
//		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
//		str_size = (uint16_t)sizeof(struct ipsec_init_command);
//		break;
//	}
	
	case IPSEC_EARLY_INIT_CMD:
	{
		struct ipsec_early_init_command *str =
			(struct ipsec_early_init_command *)data_addr;
		
		str->status = ipsec_early_init(
				str->total_instance_num,
				str->total_committed_sa_num,
				str->total_max_sa_num,
				str->flags
			);
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_early_init_command);
		break;
	}

	case IPSEC_CREATE_INSTANCE_CMD:
	{
		struct ipsec_create_instance_command *str =
			(struct ipsec_create_instance_command *)data_addr;
		
		str->status = ipsec_create_instance(
				str->committed_sa_num,
				str->max_sa_num,
				str->instance_flags,
				str->tmi_id,
				&ws_verif_instance_handle
			);
		
		verif_instance_handle[str->instance_id] = ws_verif_instance_handle;
		str->instance_addr = ws_verif_instance_handle;
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_create_instance_command);
		break;
	}

	case IPSEC_DELETE_INSTANCE_CMD:
	{
		struct ipsec_delete_instance_command *str =
			(struct ipsec_delete_instance_command *)data_addr;
		
		str->status = ipsec_delete_instance(
				verif_instance_handle[str->instance_id]
				);
				
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_delete_instance_command);
		break;
	}

	case IPSEC_ADD_SA_DESCRIPTOR_CMD:
	{
		struct ipsec_add_sa_descriptor_command *str =
			(struct ipsec_add_sa_descriptor_command *)data_addr;
		
		/* Pointer to outer IP header within the command string */
		if (str->params.direction == IPSEC_DIRECTION_OUTBOUND) {
			str->params.encparams.outer_hdr = (uint32_t *)str->outer_ip_header;
		}
		
		
		str->status = ipsec_add_sa_descriptor(
				&(str->params),
				verif_instance_handle[str->instance_id],
				&ws_sa_desc_handle
				);

		sa_desc_handle[str->sa_desc_id] = ws_sa_desc_handle;
		
		str->descriptor_addr = ws_sa_desc_handle;
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_add_sa_descriptor_command);
		
		break;
	}
	
	case IPSEC_DEL_SA_DESCRIPTOR_CMD:
	{
		struct ipsec_del_sa_descriptor_command *str =
			(struct ipsec_del_sa_descriptor_command *)data_addr;
		
		str->status = ipsec_del_sa_descriptor(
				sa_desc_handle[str->sa_desc_id]
				);
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_del_sa_descriptor_command);
		
		break;
	}
	
	case IPSEC_GET_LIFETIME_STATS_CMD:
	{
		struct ipsec_get_lifetime_stats_command *str =
			(struct ipsec_get_lifetime_stats_command *)data_addr;
		
		str->status = ipsec_get_lifetime_stats(
				//*((uint64_t *)(str->ipsec_handle_ptr)),
				sa_desc_handle[str->sa_desc_id],
				&(str-> kilobytes),
				&(str-> packets),
				&(str-> sec)
				);
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_get_lifetime_stats_command);
		
		break;
	}
	
	case IPSEC_DECR_LIFETIME_COUNTERS_CMD:
	{
		struct ipsec_decr_lifetime_counters_command *str =
			(struct ipsec_decr_lifetime_counters_command *)data_addr;
		
		str->status = ipsec_decr_lifetime_counters(
				sa_desc_handle[str->sa_desc_id],
				str->kilobytes_decr_val,
				str->packets_decr_val
				);
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_decr_lifetime_counters_command);
		
		break;
	}
	
	case IPSEC_GET_SEQ_NUM_CMD:
	{
		struct ipsec_get_seq_num_command *str =
			(struct ipsec_get_seq_num_command *)data_addr;
		
		str->status = ipsec_get_seq_num(
				//*((uint64_t *)(str->ipsec_handle_ptr)),
				sa_desc_handle[str->sa_desc_id],
				&(str->sequence_number),
				&(str->extended_sequence_number),
				&(str->anti_replay_bitmap[4])
				);
		
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_get_seq_num_command);
		
		break;
	}
	
	
	case IPSEC_FRAME_DECRYPT_CMD:
	{
		struct ipsec_frame_decrypt_command *str =
			(struct ipsec_frame_decrypt_command *)data_addr;
		
		default_task_params.parser_starting_hxs = str->starting_hxs;
		/* Run parser, in case the starting HXS changed */ 
		parse_result_generate_default (PARSER_NO_FLAGS);
		
		str->status = ipsec_frame_decrypt(
				//*((uint64_t *)(str->ipsec_handle_ptr)),
				sa_desc_handle[str->sa_desc_id],
				&str->dec_status		
				);
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_frame_decrypt_command);
		
		break;
	}
	case IPSEC_FRAME_ENCRYPT_CMD:
	{
		struct ipsec_frame_encrypt_command *str =
			(struct ipsec_frame_encrypt_command *)data_addr;
		
		default_task_params.parser_starting_hxs = str->starting_hxs;
		/* Run parser, in case the starting HXS changed */ 
		parse_result_generate_default (PARSER_NO_FLAGS);
		
		str->status = ipsec_frame_encrypt(
				//*((uint64_t *)(str->ipsec_handle_ptr)),
				sa_desc_handle[str->sa_desc_id],
				&str->enc_status		
				);
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_frame_encrypt_command);
		
		break;
	}
	
	case IPSEC_FRAME_ENCR_DECR_CMD:
	{
		struct ipsec_frame_encr_decr_command *str =
			(struct ipsec_frame_encr_decr_command *)data_addr;
		
		default_task_params.parser_starting_hxs = str->starting_hxs;
		/* Run parser, in case the starting HXS changed */ 
		parse_result_generate_default (PARSER_NO_FLAGS);
		
		/* Encryption */
		str->fm_encr_status = ipsec_frame_encrypt(
				//*((uint64_t *)(str->ipsec_encr_handle_ptr)),
				sa_desc_handle[str->encr_sa_desc_id],
				&str->enc_status		
				);
		*((int32_t *)(str->fm_encr_status_addr)) = str->fm_encr_status;
		
		/* if encryption failed, don't do decryption */
		if (str->fm_encr_status)
			break;
		
		/* Decryption */
		str->fm_decr_status = ipsec_frame_decrypt(
				//*((uint64_t *)(str->ipsec_decr_handle_ptr)),
				sa_desc_handle[str->decr_sa_desc_id],
				&str->dec_status		
		);
		*((int32_t *)(str->fm_decr_status_addr)) = str->fm_decr_status;
		
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_frame_encr_decr_command);
		
		break;
	}
	
	
	#ifndef IPSEC_OVERRIDE_RTA
	/* RTA descriptor debug */
	/*
	case IPSEC_RUN_DESC_DEBUG:
	{
		struct ipsec_run_desc_debug_command *str =
				(struct ipsec_run_desc_debug_command *)data_addr;
		
			
		str->status = sec_run_desc(&(str->descriptor_addr));
			
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_run_desc_debug_command);
			
		break;
	}
	*/
	#endif
	
	case IPSEC_CREATE_DEBUG_BUFFER_CMD:
	{
		struct ipsec_create_debug_buffer_command *str =
			(struct ipsec_create_debug_buffer_command *)data_addr;
		
		str->status = 0;
		*((int32_t *)(str->status_addr)) = str->status;
		str->prc = *((struct presentation_context *) HWC_PRC_ADDRESS);
		str_size = (uint16_t)sizeof(struct ipsec_create_debug_buffer_command);
		break;
	}
	
	
	default:
	{
		return STR_SIZE_ERR;
	}
	}
	
	return str_size;
}






