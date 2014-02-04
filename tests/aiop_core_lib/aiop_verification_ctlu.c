/**************************************************************************//**
@File		aiop_verification_ctlu.c

@Description	This file contains the AIOP CTLU SRs SW Verification

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "aiop_verification.h"
#include "aiop_verification_ctlu.h"
#include "system.h"


uint16_t aiop_verification_ctlu(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint32_t flags;

	opcode  = *((uint32_t *) asa_seg_addr);
	flags = 0x0;


	switch (opcode) {
		/* Table Create Command Verification */
		case CTLU_TABLE_CREATE_CMD_STR:
		{
			struct ctlu_table_create_command *str =
			(struct ctlu_table_create_command *) asa_seg_addr;
		
			str->status = ctlu_table_create 
					((struct ctlu_table_create_params *)str->table_create_params_ptr,
					&(str->table_id));
			str_size = sizeof(struct ctlu_table_create_command);
			break;
		}
		
		/* Update Miss Rule Command Verification */
		case CTLU_UPDATE_MISS_RULE_CMD_STR:
		{
			struct ctlu_update_miss_rule_command *str =
			(struct ctlu_update_miss_rule_command *) asa_seg_addr;
		
			str->status = ctlu_table_replace_miss_result
				(str->table_id, &(str->miss_rule),
				 &str->old_miss_result);
			str_size = sizeof(struct ctlu_update_miss_rule_command);
			break;
		}
		
		/* Table Parameters Query Command Verification */
		case CTLU_TABLE_GET_PARAMS_CMD_STR:
		{
			struct ctlu_table_get_params_command *str =
			(struct ctlu_table_get_params_command *) asa_seg_addr;
			
			str->status = ctlu_table_get_params(str->table_id,
			&str->table_get_params_out);

			str_size = sizeof(struct ctlu_table_get_params_command);
			break;
		}

		/* Get Miss Rule Command Verification */
		case CTLU_GET_MISS_RULE_CMD_STR:
		{
			struct ctlu_get_miss_rule_command *str =
			(struct ctlu_get_miss_rule_command *) asa_seg_addr;
			
			str->status = ctlu_table_get_miss_result
					(str->table_id,
					&(str->miss_rule));

			str_size = sizeof(struct ctlu_get_miss_rule_command);
			break;
		}
		
		/* Table Delete Command Verification */
		case CTLU_TABLE_DELETE_CMD_STR:
		{
			struct ctlu_table_delete_command *str =
			(struct ctlu_table_delete_command *) asa_seg_addr;
			
			str->status = ctlu_table_delete(str->table_id);

			str_size = sizeof(struct ctlu_table_delete_command);
			break;
		}
	
		/* Table Rule Create Command Verification */
		case CTLU_RULE_CREATE_CMD_STR:
		{
			struct ctlu_table_rule_create_command *str =
			(struct ctlu_table_rule_create_command *) asa_seg_addr;
			
			str->status = ctlu_table_rule_create(str->table_id,
				    (struct ctlu_table_rule*)str->rule_ptr,
				    str->key_size);

			str_size =
				sizeof(struct ctlu_table_rule_create_command);
			break;
		}
		
		/* Table Rule Create or Replace Command Verification */
		case CTLU_RULE_CREATE_OR_REPLACE_CMD_STR:
		{
			struct ctlu_table_rule_create_replace_command *str =
			  (struct ctlu_table_rule_create_replace_command *)
			  asa_seg_addr;
			
			str->status = ctlu_table_rule_create_or_replace
				(str->table_id, 
				(struct ctlu_table_rule*)str->rule_ptr,
				str->key_size,
				&str->old_res);

			str_size =
				sizeof(struct ctlu_table_rule_create_command);
			break;
		}

		/* Table Rule Replace Command Verification */
		case CTLU_RULE_REPLACE_CMD_STR:
		{
			struct ctlu_table_rule_create_replace_command *str =
			  (struct ctlu_table_rule_create_replace_command *)
			  asa_seg_addr;

			str->status = ctlu_table_rule_replace
				(str->table_id, 
				(struct ctlu_table_rule*)str->rule_ptr,
				str->key_size,
				&str->old_res);

			str_size =
				sizeof(struct ctlu_table_rule_create_command);
			break;
		}

		/* Table Rule Delete Command Verification */
		case CTLU_RULE_DELETE_CMD_STR:
		{
			struct ctlu_table_rule_delete_command *str =
			(struct ctlu_table_rule_delete_command *) asa_seg_addr;
			
			str->status = ctlu_table_rule_delete
				(str->table_id,
				 (union ctlu_key *)str->key_ptr,
				 str->key_size,
				 &str->old_res);

			str_size =
				sizeof(struct ctlu_table_rule_delete_command);
			break;
		}

		/* Table Lookup by KeyID Command Verification */
		case CTLU_LOOKUP_BY_KEYID_CMD_STR:
		{
			struct ctlu_table_lookup_by_keyid_command *str =
			(struct ctlu_table_lookup_by_keyid_command *) asa_seg_addr;

			str->status = ctlu_table_lookup_by_keyid(str->table_id, 
				str->key_id, &(str->lookup_result));

			str_size =
				sizeof(struct ctlu_table_lookup_by_keyid_command);
			break;
		}
		
		/* Table Lookup with explicit Key Command Verification */
		case CTLU_LOOKUP_BY_KEY_CMD_STR:
		{
			struct ctlu_table_lookup_by_key_command *str =
			(struct ctlu_table_lookup_by_key_command *) asa_seg_addr;

			str->status = ctlu_table_lookup_by_key(str->table_id,
				(union ctlu_key *)str->key_ptr, str->key_size,
				&(str->lookup_result));

			str_size =
				sizeof(struct ctlu_table_lookup_by_key_command);
			break;
		}
		
		/* Key Composition Rule Init Command Verification */
		case CTLU_KCR_BUILDER_INIT_CMD_STR:
		{
			struct ctlu_kcr_builder_init_command *str =
			(struct ctlu_kcr_builder_init_command *) asa_seg_addr;

			str->status = ctlu_kcr_builder_init((struct ctlu_kcr_builder *)str->kb_ptr);

			str_size =
				sizeof(struct ctlu_kcr_builder_init_command);
			break;
		}
		
		/* Key Composition Rule Add constant FEC Command Verification */
		case CTLU_KCR_BUILDER_ADD_CONSTANT_FEC_CMD_STR:
		{
			struct ctlu_kcr_builder_add_constant_fec_command *str =
			(struct ctlu_kcr_builder_add_constant_fec_command *) 
								asa_seg_addr;

			str->status = ctlu_kcr_builder_add_constant_fec
				(str->constant, str->num,
				 (struct ctlu_kcr_builder *)str->kb_ptr);

			str_size =
			sizeof(struct ctlu_kcr_builder_add_constant_fec_command);
			break;
		}

		/* Key Composition Rule Add Protocol specific FEC Command 
		 * Verification */
		case CTLU_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FIELD_FEC_CMD_STR:
		{
		
			struct ctlu_kcr_builder_fec_mask *mask_ptr;
			
			struct ctlu_kcr_builder_add_protocol_specific_field_fec_command *str =
			(struct ctlu_kcr_builder_add_protocol_specific_field_fec_command *) 
								asa_seg_addr;
			
			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = 
				ctlu_kcr_builder_add_protocol_specific_field
				((enum kcr_builder_protocol_fecid)str->fecid, 
				mask_ptr, 
				(struct ctlu_kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct ctlu_kcr_builder_add_protocol_specific_field_fec_command);
			break;
		}


		/* Key Composition Rule Add Protocol based generic FEC Command 
		 * Verification */
		case CTLU_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_EXTRACT_FEC_CMD_STR:
		{
		
			struct ctlu_kcr_builder_fec_mask *mask_ptr;
			
			struct ctlu_kcr_builder_add_protocol_based_generic_extract_fec_command *str =
			(struct ctlu_kcr_builder_add_protocol_based_generic_extract_fec_command *) 
								asa_seg_addr;
			
			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = 
			ctlu_kcr_builder_add_protocol_based_generic_fec(
				 (enum kcr_builder_parse_result_offset) 
				 str->parse_result_offset, str->extract_offset,
				 str->extract_size, mask_ptr,
				 (struct ctlu_kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct ctlu_kcr_builder_add_protocol_based_generic_extract_fec_command);
			break;
		}

		/* Key Composition Rule Add generic Extract FEC Command 
		 * Verification */
		case CTLU_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC_CMD_STR:
		{
		
			struct ctlu_kcr_builder_fec_mask *mask_ptr;
			
			struct ctlu_kcr_builder_add_generic_extract_fec_command *str =
			(struct ctlu_kcr_builder_add_generic_extract_fec_command *) 
								asa_seg_addr;
			
			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = 
			ctlu_kcr_builder_add_generic_extract_fec(str->offset,
			str->extract_size, str->flags, mask_ptr, (struct ctlu_kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct ctlu_kcr_builder_add_generic_extract_fec_command);
			break;
		}

		/* Key Composition Rule Add Lookup Result Field Extract FEC 
		 * Command Verification */
		case CTLU_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC_CMD_STR:
		{
		
			struct ctlu_kcr_builder_fec_mask *mask_ptr;
			
			struct ctlu_kcr_builder_add_lookup_result_field_fec_command *str =
			(struct ctlu_kcr_builder_add_lookup_result_field_fec_command *) 
								asa_seg_addr;
			
			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = 
			ctlu_kcr_builder_add_lookup_result_field_fec(
				str->extract_field, str->offset_in_opaque, 
				str->extract_size_in_opaque, mask_ptr, (struct ctlu_kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct ctlu_kcr_builder_add_lookup_result_field_fec_command);
			break;
		}

		/* Key Composition Rule Add Valid Field FEC Command 
		 * Verification */
		case CTLU_KCR_BUILDER_ADD_VALID_FIELD_FEC_CMD_STR:
		{
					
			struct ctlu_kcr_builder_add_valid_field_fec_command *str =
			(struct ctlu_kcr_builder_add_valid_field_fec_command *) 
								asa_seg_addr;
		
			str->status = 
			ctlu_kcr_builder_add_valid_field_fec(str->mask,
							     (struct ctlu_kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct ctlu_kcr_builder_add_valid_field_fec_command);
			break;
		}

		
		/* Key Composition Rule Create Command Verification */
		case CTLU_KCR_CREATE_CMD_STR:
		{
					
			struct ctlu_kcr_create_or_replace_command *str =
			(struct ctlu_kcr_create_or_replace_command *)
								asa_seg_addr;
		
			str->status = ctlu_kcr_create((uint8_t *)str->kcr_ptr,
						      &str->key_id);
			str_size =
			sizeof (struct ctlu_kcr_create_or_replace_command);
			break;
		}

		/* Key Composition Rule Replace Command Verification */
		case CTLU_KCR_REPLACE_CMD_STR:
		{
					
			struct ctlu_kcr_create_or_replace_command *str =
			(struct ctlu_kcr_create_or_replace_command *) 
								asa_seg_addr;
		
			str->status = ctlu_kcr_replace
				((uint8_t *)str->kcr_ptr, str->key_id);
			str_size =
			sizeof (struct ctlu_kcr_create_or_replace_command);
			break;
		}

		/* Key Composition Rule Delete Command Verification */
		case CTLU_KCR_DELETE_CMD_STR:
		{
					
			struct ctlu_kcr_delete_command *str =
			(struct ctlu_kcr_delete_command *) asa_seg_addr;
		
			str->status = 
				ctlu_kcr_delete(str->key_id);
			str_size =
			sizeof (struct ctlu_kcr_delete_command);
			break;
		}


		/* Key Composition Rule Query Command Verification */
		case CTLU_KCR_QUERY_CMD_STR:
		{
					
			struct ctlu_kcr_query_command *str =
			(struct ctlu_kcr_query_command *) 
								asa_seg_addr;
		
			str->status = 
				ctlu_kcr_query 
				(str->key_id, (uint8_t*)(str->kcr_ptr), 
					(uint8_t*)&(str->size));
			
			str_size =
			sizeof (struct ctlu_kcr_query_command);
			break;
		}

		/* Key Generation Command Verification */
		case CTLU_GEN_KEY_CMD_STR:
		{
			struct ctlu_gen_key_command *str =
			(struct ctlu_gen_key_command *) asa_seg_addr;

			str->status = ctlu_gen_key(str->key_id, (union
						   ctlu_key *)str->key_ptr,
						   &str->key_size);
			str_size =
				sizeof(struct ctlu_gen_key_command);
			break;
		}

		/* Hash Generation Command Verification */
		case CTLU_GEN_HASH_CMD_STR:
		{
			struct ctlu_gen_hash_command *str =
			(struct ctlu_gen_hash_command *) asa_seg_addr;
			str->status = ctlu_gen_hash((union ctlu_key *)str->key_ptr, str->key_size,
						    &(str->hash));
			
			str_size =
				sizeof(struct ctlu_gen_hash_command);
			break;
		}

		/* Hash Generation Command Verification */
		case CTLU_TABLE_QUERY_DEBUG_CMD_STR:
		{
			struct ctlu_table_query_debug_command *str =
			(struct ctlu_table_query_debug_command *) asa_seg_addr;
			str->status = ctlu_table_query_debug(str->table_id,
				(struct ctlu_table_params_query_output_message *)(str->output_ptr));
			
			str_size =
				sizeof(struct ctlu_table_query_debug_command);
			break;
		}
		case CTLU_KEY_ID_POOL_CREATE_CMD_STR:
		{
			struct ctlu_key_id_pool_create_verif_command *str =
			(struct ctlu_key_id_pool_create_verif_command *) 
			asa_seg_addr;
			str->status = sys_ctlu_keyid_pool_create();
			str_size = sizeof(struct ctlu_key_id_pool_create_verif_command);
			break;
		}
		default:
		{
			return STR_SIZE_ERR;
		}
	}

	return str_size;
}
