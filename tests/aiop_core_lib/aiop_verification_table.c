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
@File		aiop_verification_ctlu.c

@Description	This file contains the AIOP CTLU SRs SW Verification

*//***************************************************************************/
#include "aiop_verification.h"
#include "aiop_verification_table.h"
#include "system.h"


uint16_t aiop_verification_table(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint32_t flags;

	opcode  = *((uint32_t *) asa_seg_addr);
	flags = 0x0;


	switch (opcode) {
	/* Table Create Command Verification */
	case TABLE_CREATE_CMD_STR:
	{
		struct table_create_command *str =
		(struct table_create_command *) asa_seg_addr;
	
		str->status = table_create 
				(str->acc_id,
				(struct table_create_params *)str->table_create_params_ptr,
				&(str->table_id));
		str_size = sizeof(struct table_create_command);
		break;
	}

	/* Update Miss Rule Command Verification */
	case TABLE_REPLACE_MISS_RESULT_CMD_STR:
	{
		struct table_replace_miss_result_command *str =
		(struct table_replace_miss_result_command *) asa_seg_addr;

		if (str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL) {
			table_replace_miss_result(str->acc_id,
						  str->table_id,
						  &(str->miss_rule),
						  (void *)0);
		} else {
			table_replace_miss_result(str->acc_id,
						  str->table_id,
						  &(str->miss_rule),
						  &str->old_miss_result);
		}

		str_size = sizeof(struct table_replace_miss_result_command);
		break;
	}

	/* Table Parameters Query Command Verification */
	case TABLE_GET_PARAMS_CMD_STR:
	{
		struct ctlu_table_get_params_command *str =
		(struct ctlu_table_get_params_command *) asa_seg_addr;

		table_get_params(str->acc_id, str->table_id,
				 &str->table_get_params_out);

		str_size = sizeof(struct ctlu_table_get_params_command);
		break;
	}

	/* Get Miss Rule Command Verification */
	case TABLE_GET_MISS_RESULT_CMD_STR:
	{
		struct table_get_miss_result_command *str =
		(struct table_get_miss_result_command *) asa_seg_addr;
		
		table_get_miss_result(str->acc_id, str->table_id,
				      &(str->miss_rule));

		str_size = sizeof(struct table_get_miss_result_command);
		break;
	}

	/* Table Delete Command Verification */
	case TABLE_DELETE_CMD_STR:
	{
		struct table_delete_command *str =
		(struct table_delete_command *) asa_seg_addr;
		
		table_delete(str->acc_id, str->table_id);

		str_size = sizeof(struct table_delete_command);
		break;
	}

	/* Table Rule Create Command Verification */
	case TABLE_RULE_CREATE_CMD_STR:
	{
		struct table_rule_create_command *str =
		(struct table_rule_create_command *) asa_seg_addr;
		struct table_rule rule __attribute__((aligned(16)));
		
		rule = *(struct table_rule*)(str->rule_ptr);
		
		str->status = table_rule_create(str->acc_id,
			    str->table_id,
			    (struct table_rule*)&rule,
			    str->key_size);

		str_size =
			sizeof(struct table_rule_create_command);
		break;
	}

	/* Table Rule Create or Replace Command Verification */
	case TABLE_RULE_CREATE_OR_REPLACE_CMD_STR:
	{
		struct table_rule_create_replace_command *str =
		  (struct table_rule_create_replace_command *)
		  asa_seg_addr;
		struct table_rule rule_ptr __attribute__((aligned(16)));

		rule_ptr = *(struct table_rule*)str->rule_ptr;
		if (str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL) {
			str->status = table_rule_create_or_replace
				(str->acc_id,
				 str->table_id,
				 &rule_ptr,
				 str->key_size,
				 (void *)0);
		} else {
			str->status = table_rule_create_or_replace
				(str->acc_id,
				 str->table_id,
				 &rule_ptr,
				 str->key_size,
				 &str->old_res);
		}
		str_size =
			sizeof(struct table_rule_create_replace_command);
		break;
	}

	/* Table Rule Replace Command Verification */
	case TABLE_RULE_REPLACE_CMD_STR:
	{
		struct table_rule_create_replace_command *str =
		  (struct table_rule_create_replace_command *)
		  asa_seg_addr;

		if (str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL) {
			str->status = table_rule_replace
				(str->acc_id,
				str->table_id,
				(struct table_rule*)str->rule_ptr,
				str->key_size,
				(void *)0);
		} else {
			str->status = table_rule_replace
				(str->acc_id,
				str->table_id,
				(struct table_rule*)str->rule_ptr,
				str->key_size,
				&str->old_res);
		}

		str_size =
			sizeof(struct table_rule_create_replace_command);
		break;
	}

	/* Table Rule Delete Command Verification */
	case TABLE_RULE_DELETE_CMD_STR:
	{
		struct table_rule_delete_command *str =
		(struct table_rule_delete_command *) asa_seg_addr;

		if (str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL) {
			str->status = table_rule_delete
				(str->acc_id,
				 str->table_id,
				 (union table_key_desc *)str->key_desc_ptr,
				 str->key_size,
				 (void *) 0);
		} else {
			str->status = table_rule_delete
				(str->acc_id,
				 str->table_id,
				 (union table_key_desc *)str->key_desc_ptr,
				 str->key_size,
				 &str->old_res);
		}

		str_size =
			sizeof(struct table_rule_delete_command);
		break;
	}

	/* Table Rule Delete Command Verification */
	case TABLE_RULE_QUERY_CMD_STR:
	{
		struct table_rule_query_command *str =
		(struct table_rule_query_command *) asa_seg_addr;
		struct table_result result __attribute__((aligned(16)));

		str->status = table_rule_query
			(str->acc_id,
			 str->table_id,
			 (union table_key_desc *)str->key_desc_ptr,
			 str->key_size,
			 &result,
			 &str->timestamp);

		str->result = result;
		str_size =
			sizeof(struct table_rule_query_command);
		break;
	}
	/* Table Lookup with explicit Key Command Verification */
	case TABLE_LOOKUP_BY_KEY_CMD_STR:
	{
		struct table_lookup_by_key_command *str =
		(struct table_lookup_by_key_command *) asa_seg_addr;
		struct table_lookup_result lookup_result __attribute__((aligned(16)));
		union table_lookup_key_desc key_desc __attribute__((aligned(16)));
		
		key_desc = str->key_desc;
		str->status = table_lookup_by_key(str->acc_id,
						  str->table_id,
						  key_desc,
						  str->key_size,
						  &lookup_result);	
		str->lookup_result = lookup_result;
		
		str_size =
			sizeof(struct table_lookup_by_key_command);
		break;
	}
	/* Table Lookup by KeyID Default Frame Command Verification */
	case TABLE_LOOKUP_BY_KEYID_DEFAULT_FRAME_CMD_STR:
	{
		struct table_lookup_by_keyid_default_frame_command *str =
		(struct table_lookup_by_keyid_default_frame_command *) asa_seg_addr;
		struct table_lookup_result lookup_result __attribute__((aligned(16)));
	
		/* WA for HW segment length issue */
		uint16_t old_seg_length = PRC_GET_SEGMENT_LENGTH();
		PRC_SET_SEGMENT_LENGTH(DEFAULT_SEGMENT_SIZE);

		str->status = table_lookup_by_keyid_default_frame(str->acc_id,
			str->table_id,
			str->key_id, 
			&(lookup_result));
		
		str->lookup_result = lookup_result;
		
		/* WA for HW segment length issue */
		PRC_SET_SEGMENT_LENGTH(old_seg_length);
		str_size =
		  sizeof(struct table_lookup_by_keyid_default_frame_command);
		break;
	}
	/* Table Lookup by KeyID Command Verification */
	case TABLE_LOOKUP_BY_KEYID_CMD_STR:
	{
		struct table_lookup_non_default_params ndf_params 
												__attribute__((aligned(16)));
		struct table_lookup_result lookup_result 
												__attribute__((aligned(16)));
		struct table_lookup_by_keyid_command *str =
		(struct table_lookup_by_keyid_command *) asa_seg_addr;
		ndf_params = str->ndf_params;

		/* WA for HW segment length issue */
		uint16_t old_seg_length = PRC_GET_SEGMENT_LENGTH();
		PRC_SET_SEGMENT_LENGTH(DEFAULT_SEGMENT_SIZE);

		str->status = table_lookup_by_keyid(str->acc_id,
						    str->table_id,
						    str->key_id,
						    str->flags,
						    &(ndf_params),
						    &(lookup_result));
		str->lookup_result = lookup_result;

		/* WA for HW segment length issue */
		PRC_SET_SEGMENT_LENGTH(old_seg_length);

		str_size =
			sizeof(struct table_lookup_by_keyid_command);
		break;
	}
	/* Table Query Command Verification */
	case TABLE_QUERY_DEBUG_CMD_STR:
	{
		struct table_query_debug_command *str =
		(struct table_query_debug_command *) asa_seg_addr;
		struct table_params_query_output_message output_ptr 
											__attribute__((aligned(16)));

		str->status = table_query_debug(str->acc_id,
						str->table_id,
						&output_ptr);
		
		/* Disable reserved fields */
		cdma_ws_memory_init((void *)output_ptr.reserved,
							TABLE_QUERY_OUTPUT_MESSAGE_RESERVED_SPACE, 0);

		*(struct table_params_query_output_message *)str->output_ptr = 
																output_ptr;
		str_size =
			sizeof(struct table_query_debug_command);
		break;
	}

	/* Table HW ACCEL Acquire Lock Command Verification */
	case TABLE_HW_ACCEL_ACQUIRE_LOCK_CMD_STR:
	{
		struct table_hw_accel_acquire_lock_command *str =
		(struct table_hw_accel_acquire_lock_command *) asa_seg_addr;
		str->status = table_hw_accel_acquire_lock(str->acc_id);
		str_size =
			sizeof(struct table_hw_accel_acquire_lock_command);
		break;
	}

	/* Table HW ACCEL Release Lock Command Verification */
	case TABLE_HW_ACCEL_RELEASE_LOCK_CMD_STR:
	{
		struct table_hw_accel_release_lock_command *str =
		(struct table_hw_accel_release_lock_command *) asa_seg_addr;
		table_hw_accel_release_lock(str->acc_id);
		str_size =
			sizeof(struct table_hw_accel_release_lock_command);
		break;
	}

	default:
	{
		return STR_SIZE_ERR;
	}
	}

	return str_size;
}
