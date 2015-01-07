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
#include "aiop_verification_keygen.h"
#include "system.h"


uint16_t aiop_verification_keygen(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;

	opcode  = *((uint32_t *) asa_seg_addr);

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
			*((int32_t *)(str->keygen_status_addr)) = str->status;

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
			*((int32_t *)(str->keygen_status_addr)) = str->status;

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
			*((int32_t *)(str->keygen_status_addr)) = str->status;

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
			*((int32_t *)(str->keygen_status_addr)) = str->status;

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
			str->extract_size, str->gec_source, mask_ptr, (struct kcr_builder *)str->kb_ptr);
			*((int32_t *)(str->keygen_status_addr)) = str->status;

			str_size =
			sizeof (struct keygen_kcr_builder_add_generic_extract_fec_command);
			break;
		}

#ifdef REV2
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
			*((int32_t *)(str->keygen_status_addr)) = str->status;

			str_size =
			sizeof (struct keygen_kcr_builder_add_lookup_result_field_fec_command);
			break;
		}
#endif /*REV2*/

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
			*((int32_t *)(str->keygen_status_addr)) = str->status;

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
			*((int32_t *)(str->keygen_status_addr)) = str->status;
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
			*((int32_t *)(str->keygen_status_addr)) = str->status;
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
						   (union table_key_desc *)str->key_ptr,						   &str->key_size);
			*((int32_t *)(str->keygen_status_addr)) = str->status;
			str_size =
				sizeof(struct keygen_gen_key_command);
			break;
		}

		/* Hash Generation Command Verification */
		case KEYGEN_GEN_HASH_CMD_STR:
		{
			struct keygen_gen_hash_command *str =
			(struct keygen_gen_hash_command *) asa_seg_addr;
			str->status = keygen_gen_hash((union table_key_desc *)str->key_ptr, str->key_size,
						    &(str->hash));
			
			*((int32_t *)(str->keygen_status_addr)) = str->status;
			str_size =
				sizeof(struct keygen_gen_hash_command);
			break;
		}

		case KEYGEN_KEY_ID_POOL_CREATE_CMD_STR:
		{
			struct keygen_key_id_pool_create_verif_command *str =
			(struct keygen_key_id_pool_create_verif_command *) 
			asa_seg_addr;
			sys_keyid_pool_create();
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
