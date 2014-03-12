/**************************************************************************//**
@File		aiop_verification_ctlu.c

@Description	This file contains the AIOP CTLU SRs SW Verification

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "aiop_verification.h"
#include "aiop_verification_keygen.h"
#include "system.h"


uint16_t aiop_verification_keygen(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint32_t flags;

	opcode  = *((uint32_t *) asa_seg_addr);
	flags = 0x0;


	switch (opcode) {
		/* Key Composition Rule Init Command Verification */
		case KEYGEN_KCR_BUILDER_INIT_CMD_STR:
		{
			struct keygen_kcr_builder_init_command *str =
			(struct keygen_kcr_builder_init_command *) asa_seg_addr;

			keygen_kcr_builder_init((struct kcr_builder *)str->kb_ptr);

			str_size =
				sizeof(struct keygen_kcr_builder_init_command);
			break;
		}
		
		/* Key Composition Rule Add constant FEC Command Verification */
		case KEYGEN_KCR_BUILDER_ADD_CONSTANT_FEC_CMD_STR:
		{
			struct keygen_kcr_builder_add_constant_fec_command *str =
			(struct keygen_kcr_builder_add_constant_fec_command *) 
								asa_seg_addr;

			str->status = keygen_kcr_builder_add_constant_fec
				(str->constant, str->num,
				 (struct kcr_builder *)str->kb_ptr);

			str_size =
			sizeof(struct keygen_kcr_builder_add_constant_fec_command);
			break;
		}

		
		/* Key Composition Rule Add input value FEC Command Verification */
		case KEYGEN_KCR_BUILDER_ADD_INPUT_VALUE_FEC_CMD_STR:
		{
			struct kcr_builder_fec_mask *mask_ptr;

			struct keygen_kcr_builder_add_input_value_fec_command *str =
			(struct keygen_kcr_builder_add_input_value_fec_command *) 
								asa_seg_addr;

			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = keygen_kcr_builder_add_input_value_fec
				(str->offset, str->extract_size, mask_ptr,
				 (struct kcr_builder *)str->kb_ptr);

			str_size =
			sizeof(struct keygen_kcr_builder_add_input_value_fec_command);
			break;
		}

		
		/* Key Composition Rule Add Protocol specific FEC Command 
		 * Verification */
		case KEYGEN_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FIELD_FEC_CMD_STR:
		{
		
			struct kcr_builder_fec_mask *mask_ptr;
			
			struct keygen_kcr_builder_add_protocol_specific_field_fec_command *str =
			(struct keygen_kcr_builder_add_protocol_specific_field_fec_command *) 
								asa_seg_addr;
			
			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = 
				keygen_kcr_builder_add_protocol_specific_field
				((enum kcr_builder_protocol_fecid)str->fecid, 
				mask_ptr, 
				(struct kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct keygen_kcr_builder_add_protocol_specific_field_fec_command);
			break;
		}


		/* Key Composition Rule Add Protocol based generic FEC Command 
		 * Verification */
		case KEYGEN_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_EXTRACT_FEC_CMD_STR:
		{
		
			struct kcr_builder_fec_mask *mask_ptr;
			
			struct keygen_kcr_builder_add_protocol_based_generic_extract_fec_command *str =
			(struct keygen_kcr_builder_add_protocol_based_generic_extract_fec_command *) 
								asa_seg_addr;
			
			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = 
			keygen_kcr_builder_add_protocol_based_generic_fec(
				 (enum kcr_builder_parse_result_offset) 
				 str->parse_result_offset, str->extract_offset,
				 str->extract_size, mask_ptr,
				 (struct kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct keygen_kcr_builder_add_protocol_based_generic_extract_fec_command);
			break;
		}

		/* Key Composition Rule Add generic Extract FEC Command 
		 * Verification */
		case KEYGEN_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC_CMD_STR:
		{
		
			struct kcr_builder_fec_mask *mask_ptr;
			
			struct keygen_kcr_builder_add_generic_extract_fec_command *str =
			(struct keygen_kcr_builder_add_generic_extract_fec_command *) 
								asa_seg_addr;
			
			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = 
			keygen_kcr_builder_add_generic_extract_fec(str->offset,
			str->extract_size, str->flags, mask_ptr, (struct kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct keygen_kcr_builder_add_generic_extract_fec_command);
			break;
		}

		/* Key Composition Rule Add Lookup Result Field Extract FEC 
		 * Command Verification */
		case KEYGEN_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC_CMD_STR:
		{
		
			struct kcr_builder_fec_mask *mask_ptr;
			
			struct keygen_kcr_builder_add_lookup_result_field_fec_command *str =
			(struct keygen_kcr_builder_add_lookup_result_field_fec_command *) 
								asa_seg_addr;
			
			mask_ptr = (str->mask.num_of_masks == 0) ? NULL :
							 &(str->mask);
		
			str->status = 
				keygen_kcr_builder_add_lookup_result_field_fec(
				str->extract_field, str->offset_in_opaque, 
				str->extract_size_in_opaque, mask_ptr, (struct kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct keygen_kcr_builder_add_lookup_result_field_fec_command);
			break;
		}

		/* Key Composition Rule Add Valid Field FEC Command 
		 * Verification */
		case KEYGEN_KCR_BUILDER_ADD_VALID_FIELD_FEC_CMD_STR:
		{
					
			struct keygen_kcr_builder_add_valid_field_fec_command *str =
			(struct keygen_kcr_builder_add_valid_field_fec_command *) 
								asa_seg_addr;
		
			str->status = 
					keygen_kcr_builder_add_valid_field_fec(str->mask,
							     (struct kcr_builder *)str->kb_ptr);

			str_size =
			sizeof (struct keygen_kcr_builder_add_valid_field_fec_command);
			break;
		}

		
		/* Key Composition Rule Create Command Verification */
		case KEYGEN_KCR_CREATE_CMD_STR:
		{
					
			struct keygen_kcr_create_or_replace_command *str =
			(struct keygen_kcr_create_or_replace_command *)
								asa_seg_addr;
		
			str->status = keygen_kcr_create(str->acc_id,
						      (uint8_t *)str->kcr_ptr,
						      &str->key_id);
			str_size =
			sizeof (struct keygen_kcr_create_or_replace_command);
			break;
		}

		/* Key Composition Rule Replace Command Verification */
		case KEYGEN_KCR_REPLACE_CMD_STR:
		{
					
			struct keygen_kcr_create_or_replace_command *str =
			(struct keygen_kcr_create_or_replace_command *) 
								asa_seg_addr;
		
			keygen_kcr_replace
				(str->acc_id, (uint8_t *)str->kcr_ptr, str->key_id);
			str_size =
			sizeof (struct keygen_kcr_create_or_replace_command);
			break;
		}

		/* Key Composition Rule Delete Command Verification */
		case KEYGEN_KCR_DELETE_CMD_STR:
		{
			struct keygen_kcr_delete_command *str =
			(struct keygen_kcr_delete_command *) asa_seg_addr;

			str->status = 
					keygen_kcr_delete(str->acc_id, str->key_id);
			str_size =
			sizeof (struct keygen_kcr_delete_command);
			break;
		}


		/* Key Composition Rule Query Command Verification */
		case KEYGEN_KCR_QUERY_CMD_STR:
		{
					
			struct keygen_kcr_query_command *str =
			(struct keygen_kcr_query_command *) asa_seg_addr;
		
			keygen_kcr_query 
				(str->acc_id, str->key_id, (uint8_t*)(str->kcr_ptr));
			
			str_size =
			sizeof (struct keygen_kcr_query_command);
			break;
		}

		/* Key Generation Command Verification */
		case KEYGEN_GEN_KEY_CMD_STR:
		{
			struct keygen_gen_key_command *str =
			(struct keygen_gen_key_command *) asa_seg_addr;

			str->status = keygen_gen_key(str->acc_id,
						   str->key_id, 
						   str->opaquein,
						   (union table_key *)str->key_ptr,						   &str->key_size);
			str_size =
				sizeof(struct keygen_gen_key_command);
			break;
		}

		/* Hash Generation Command Verification */
		case KEYGEN_GEN_HASH_CMD_STR:
		{
			struct keygen_gen_hash_command *str =
			(struct keygen_gen_hash_command *) asa_seg_addr;
			str->status = keygen_gen_hash((union table_key *)str->key_ptr, str->key_size,
						    &(str->hash));
			
			str_size =
				sizeof(struct keygen_gen_hash_command);
			break;
		}

		case KEYGEN_KEY_ID_POOL_CREATE_CMD_STR:
		{
			struct keygen_key_id_pool_create_verif_command *str =
			(struct keygen_key_id_pool_create_verif_command *) 
			asa_seg_addr;
			str->status = sys_ctlu_keyid_pool_create();
			str_size = sizeof(struct keygen_key_id_pool_create_verif_command);
			break;
		}
		default:
		{
			return STR_SIZE_ERR;
		}
	}

	return str_size;
}
