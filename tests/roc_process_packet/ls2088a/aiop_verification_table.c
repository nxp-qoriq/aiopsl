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
@File		aiop_verification_ctlu.c

@Description	This file contains the AIOP CTLU SRs SW Verification

*//***************************************************************************/
#include "aiop_verification.h"
#include "aiop_verification_table.h"
#include "system.h"

__VERIF_GLOBAL t_rule_id rule_id_array[TABLE_VERIF_RULE_ID_ARRAY_SIZE];

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
	
		str->status = table_create(str->acc_id,
					   (struct table_create_params *)
						   str->table_create_params_ptr,
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
						str->key_size,
						rule_id_array +
						    str->rule_id_index);
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

		struct table_result *result_ptr = NULL;
		uint8_t             *options_ptr = NULL;
		uint32_t            *timestamp_ptr = NULL;

		if (!(str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL))
			result_ptr = &str->replaced_result;
		if (!(str->flags & TABLE_VERIF_FLAG_OLD_OPTIONS_NULL))
			options_ptr = &str->replaced_options;
		if (!(str->flags & TABLE_VERIF_FLAG_OLD_TIMESTAMP_NULL))
			timestamp_ptr = &str->timestamp;

		struct table_rule rule_ptr __attribute__((aligned(16)));

		rule_ptr = *(struct table_rule*)str->rule_ptr;
		str->status = table_rule_create_or_replace(str->acc_id,
							   str->table_id,
							   &rule_ptr,
							   str->key_size,
							   rule_id_array +
							    str->rule_id_index,
							    result_ptr,
							    options_ptr,
							    timestamp_ptr);
		str_size =
			sizeof(struct table_rule_create_replace_command);
		break;
	}


	/* Table Rule Replace Command Verification */
	case TABLE_RULE_REPLACE_CMD_STR:
	{
		struct table_rule_replace_command *str =
		  (struct table_rule_replace_command *)
		  asa_seg_addr;

		struct table_result *result_ptr = NULL;
		uint8_t             *options_ptr = NULL;
		uint32_t            *timestamp_ptr = NULL;
		uint64_t            rule_id = TABLE_VERIF_RULE_ID_ALL_ONE;

		if (!(str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL))
			result_ptr = &str->replaced_result;
		if (!(str->flags & TABLE_VERIF_FLAG_OLD_OPTIONS_NULL))
			options_ptr = &str->replaced_options;
		if (!(str->flags & TABLE_VERIF_FLAG_OLD_TIMESTAMP_NULL))
			timestamp_ptr = &str->timestamp;
		if(str->flags & TABLE_VERIF_FLAG_RULE_ID_ALL_ONE)
			rule_id = rule_id_array[str->rule_id_index];

		str->status = table_rule_replace(str->acc_id,
						 str->table_id,
						 rule_id,
						 &str->new_result,
						 str->new_options,
						 result_ptr,
						 options_ptr,
						 timestamp_ptr);
		str_size =
			sizeof(struct table_rule_replace_command);
		break;
	}


	/* Table Rule Query Get Result Command Verification */
	case TABLE_RULE_QUERY_GET_RESULT_CMD_STR:
	{
		struct table_rule_query_get_result_command *str =
		(struct table_rule_query_get_result_command *) asa_seg_addr;

		struct table_result *result_ptr = NULL;
		uint8_t             *options_ptr = NULL;
		uint32_t            *timestamp_ptr = NULL;
		uint64_t            rule_id = TABLE_VERIF_RULE_ID_ALL_ONE;

		if (!(str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL))
			result_ptr = &str->queried_result;
		if (!(str->flags & TABLE_VERIF_FLAG_OLD_OPTIONS_NULL))
			options_ptr = &str->queried_options;
		if (!(str->flags & TABLE_VERIF_FLAG_OLD_TIMESTAMP_NULL))
			timestamp_ptr = &str->timestamp;
		if(str->flags & TABLE_VERIF_FLAG_RULE_ID_ALL_ONE)
			rule_id = rule_id_array[str->rule_id_index];

		str->status = table_rule_query_get_result
			(str->acc_id,
			 str->table_id,
			 rule_id,
			 result_ptr,
			 options_ptr,
			 timestamp_ptr);

		str_size =
			sizeof(struct table_rule_query_get_result_command);
		break;
	}


	/* Table Rule Query Get Result Command Verification */
	case TABLE_RULE_QUERY_GET_KEY_DESC_CMD_STR:
	{
		struct table_rule_query_get_key_desc_command *str =
		(struct table_rule_query_get_key_desc_command *) asa_seg_addr;

		union table_key_desc desc __attribute__((aligned(16)));
		uint64_t            rule_id = TABLE_VERIF_RULE_ID_ALL_ONE;

		if(str->flags & TABLE_VERIF_FLAG_RULE_ID_ALL_ONE)
			rule_id = rule_id_array[str->rule_id_index];

		str->status = table_rule_query_get_key_desc(str->acc_id,
							    str->table_id,
							    rule_id,
							    &desc);

		str->key_desc = desc;

		str_size =
			sizeof(struct table_rule_query_get_key_desc_command);
		break;
	}

	/* Table Rule Query Get Next ID Command Verification */
	case TABLE_RULE_QUERY_GET_NEXT_ID_CMD_STR:
	{
		struct table_rule_query_get_next_id_desc_command *str =
		(struct table_rule_query_get_next_id_desc_command *)
			asa_seg_addr;
		uint64_t next_rule_id, rule_id =
					rule_id_array[str->rule_id_index];

		str->status = table_get_next_ruleid(str->acc_id,
						    str->table_id,
						    str->rule_id_index,
						    &next_rule_id);

		str->next_rule_id_index = rule_id_array[next_rule_id];
		str_size =
		sizeof(struct table_rule_query_get_next_id_desc_command);
		break;
	}

	/* Table Rule Delete Command Verification */
	case TABLE_RULE_DELETE_CMD_STR:
	{
		struct table_rule_delete_command *str =
		(struct table_rule_delete_command *) asa_seg_addr;

		struct table_result *result_ptr = NULL;
		uint8_t             *options_ptr = NULL;
		uint32_t            *timestamp_ptr = NULL;
		uint64_t            rule_id = TABLE_VERIF_RULE_ID_ALL_ONE;

		if (!(str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL))
			result_ptr = &str->deleted_result;
		if (!(str->flags & TABLE_VERIF_FLAG_OLD_OPTIONS_NULL))
			options_ptr = &str->deleted_options;
		if (!(str->flags & TABLE_VERIF_FLAG_OLD_TIMESTAMP_NULL))
			timestamp_ptr = &str->timestamp;
		if(str->flags & TABLE_VERIF_FLAG_RULE_ID_ALL_ONE)
			rule_id = rule_id_array[str->rule_id_index];


		str->status = table_rule_delete(str->acc_id,
						str->table_id,
						rule_id,
						result_ptr,
						options_ptr,
						timestamp_ptr);

		str_size =
			sizeof(struct table_rule_delete_command);
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


	/* Table Rule Replace by key descriptor Command Verification */
	case TABLE_RULE_REPLACE_BY_KEY_DESC_CMD_STR:
	{
		struct table_rule_create_replace_command *str =
		  (struct table_rule_create_replace_command *)
		  asa_seg_addr;

		if (str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL) {
			str->status = table_rule_replace_by_key_desc
				(str->acc_id,
				str->table_id,
				(struct table_rule*)str->rule_ptr,
				str->key_size,
				(void *)0);
		} else {
			str->status = table_rule_replace_by_key_desc
				(str->acc_id,
				str->table_id,
				(struct table_rule*)str->rule_ptr,
				str->key_size,
				&str->replaced_result);
		}

		str_size =
			sizeof(struct table_rule_create_replace_command);
		break;
	}

	/* Table Rule Query by key descriptor Command Verification */
	case TABLE_RULE_QUERY_BY_KEY_DESC_CMD_STR:
	{
		struct table_rule_query_by_key_desc_command *str =
		(struct table_rule_query_by_key_desc_command *) asa_seg_addr;
		/* struct table_result result __attribute__((aligned(16)));
		cdma_ws_memory_init((void *)&result, sizeof(result), 0); */

		str->status = table_rule_query_by_key_desc(
			str->acc_id,
			str->table_id,
			(union table_key_desc *)str->key_desc_ptr,
			str->key_size,
			/*&result*/&str->result,
			&str->timestamp,
			&str->priority,
			rule_id_array + str->rule_id_index);
		/*str->result = result;*/
		str_size =
			sizeof(struct table_rule_query_by_key_desc_command);
		break;
	}

	/* Table Rule Delete by key descriptor Command Verification */
	case TABLE_RULE_DELETE_BY_KEY_DESC_CMD_STR:
	{
		struct table_rule_delete_by_key_desc_command *str =
		(struct table_rule_delete_by_key_desc_command *) asa_seg_addr;

		if (str->flags & TABLE_VERIF_FLAG_OLD_RESULT_NULL) {
			str->status = table_rule_delete_by_key_desc
				(str->acc_id,
				 str->table_id,
				 (union table_key_desc *)str->key_desc_ptr,
				 str->key_size,
				 (void *) 0);
		} else {
			str->status = table_rule_delete_by_key_desc
				(str->acc_id,
				 str->table_id,
				 (union table_key_desc *)str->key_desc_ptr,
				 str->key_size,
				 &str->old_res);
		}

		str_size =
			sizeof(struct table_rule_delete_by_key_desc_command);
		break;
	}


	/* Table Query Debug Command Verification */
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

	default:
	{
		return STR_SIZE_ERR;
	}
	}

	return str_size;
}
