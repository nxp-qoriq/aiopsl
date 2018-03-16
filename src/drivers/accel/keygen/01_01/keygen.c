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
@File		keygen.c

@Description	This file contains the AIOP SW Key Generation API implementation

*//***************************************************************************/

#include "fsl_keygen.h"
#include "fsl_cdma.h"

#include "keygen.h"
#include "system.h"
#include "fsl_id_pool.h"

extern uint64_t ext_keyid_pool_address;

int keygen_kcr_builder_add_constant_fec(uint8_t constant, uint8_t num,
					       struct kcr_builder *kb)
{

	uint8_t curr_byte = kb->kcr_length;

	if ((curr_byte + KEYGEN_KCR_CONST_FEC_SIZE) > KEYGEN_KCR_MAX_KCR_SIZE)
		return -EINVAL;

	/* Update kcr_builder struct */
	/* User-defined FECID, no mask extension */
	kb->kcr[curr_byte] = KEYGEN_KCR_UDC_FECID << 1;
	/* The number of times the user defined constant is repeated */
	kb->kcr[curr_byte+1] = num-1;
	/* User defined constant */
	kb->kcr[curr_byte+2] = constant;

	kb->kcr[KEYGEN_KCR_NFEC] += 1;
	kb->kcr_length += KEYGEN_KCR_CONST_FEC_SIZE;

	return 0;
}


int keygen_kcr_builder_add_input_value_fec(uint8_t offset,
					uint8_t extract_size,
					struct kcr_builder_fec_mask *mask,
					struct kcr_builder *kb){


	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1, op2;
	uint8_t	fec_bytes_num = KEYGEN_KCR_LOOKUP_RES_FEC_SIZE;
	uint8_t	mask_bytes = 0;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = KEYGEN_KCR_GEC_FECID << 1;

	op0 = KEYGEN_KCR_OP0_HET_GEC | KEYGEN_KCR_EXT_OPAQUE_IN_EOM;
	op1 = KEYGEN_KCR_EXT_OPAQUE_IN_BASIC_EO + offset;
	op2 = extract_size - 1;


	if (mask) {
		/*
		if (mask->single_mask[0].mask_offset > 0xF ||
			mask->single_mask[1].mask_offset > 0xF ||
			mask->single_mask[2].mask_offset > 0xF ||
			mask->single_mask[3].mask_offset > 0xF)
			return KEYGEN_KCR_MASK_OFFSET_ERR;
		 */

		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;

		fecid = fecid | KEYGEN_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[KEYGEN_KCR_NFEC] += 1;
	kb->kcr_length += KEYGEN_KCR_LOOKUP_RES_FEC_SIZE + mask_bytes;

	return 0;
}


int keygen_kcr_builder_add_protocol_based_generic_fec(
	enum kcr_builder_parse_result_offset pr_offset,
	uint8_t extract_offset, uint8_t extract_size,
	struct kcr_builder_fec_mask *mask, struct kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	mask_bytes = 0;
	uint8_t	fec_bytes_num = KEYGEN_KCR_PROTOCOL_BASED_GENERIC_FEC_SIZE;
	uint8_t fecid, op0, op1, op2;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = KEYGEN_KCR_GEC_FECID << 1;

	/* OP0 for Protocol based extraction */
	op0 = KEYGEN_KCR_OP0_HET_PROTOCOL |
		  KEYGEN_KCR_PROTOCOL_HVT |
		  (uint8_t)pr_offset;

	/* OP1 = Extract Offset */
	op1 = extract_offset;

	/* OP2 = Extract Size*/
	op2 = extract_size - 1;

	if (mask) {
		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;

		fecid = fecid | KEYGEN_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[KEYGEN_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return 0;
}


int keygen_kcr_builder_add_generic_extract_fec(uint8_t offset,
	uint8_t extract_size, enum kcr_builder_gec_source gec_source,
	struct kcr_builder_fec_mask *mask, struct kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1, op2;
	uint8_t aligned_offset, extract_offset;
	uint8_t	fec_bytes_num = KEYGEN_KCR_GENERIC_FEC_SIZE;
	uint8_t	mask_bytes = 0;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = KEYGEN_KCR_GEC_FECID << 1;

	/* OP0 for General extraction */
	op0 = 0;
	aligned_offset = offset & KEYGEN_KCR_16_BYTES_ALIGNMENT;
	if (gec_source == KEYGEN_KCR_GEC_FRAME) {
		/* Generic extraction from start of frame */
		switch (aligned_offset) {

		case (0x00):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x00;
			break;

		case (0x10):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x10;
			break;

		case (0x20):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x20;
			break;

		case (0x30):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x30;
			break;

		case (0x40):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			     KEYGEN_KCR_EOM_FRAME_OFFSET_0x40;
			break;

		case (0x50):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x50;
			break;

		case (0x60):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x60;
			break;

		case (0x70):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x70;
			break;

		case (0x80):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x80;
			break;

		case (0x90):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0x90;
			break;

		case (0xA0):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0xA0;
			break;

		case (0xB0):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0xB0;
			break;

		case (0xC0):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0xC0;
			break;

		case (0xD0):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0xD0;
			break;

		case (0xE0):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0xE0;
			break;

		case (0xF0):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_FRAME_OFFSET_0xF0;
			break;

		default:
			break;

		} /* switch */
	} /* if */
	else {
		/* Generic extraction from parse result */
		switch (aligned_offset) {

		case (0x00):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_PARSE_RES_OFFSET_0x00;
			break;

		case (0x10):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_PARSE_RES_OFFSET_0x10;
			break;

		case (0x20):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_PARSE_RES_OFFSET_0x20;
			break;

		case (0x30):
			op0 = KEYGEN_KCR_OP0_HET_GEC |
			      KEYGEN_KCR_EOM_PARSE_RES_OFFSET_0x30;
			break;

		default:
			break;
		}
	}


	/* OP1 = Extract Offset */
	extract_offset = offset & KEYGEN_KCR_OFFSET_WITHIN_16_BYTES;
	op1 = extract_offset;

	/* OP2 = Extract Size*/
	op2 = extract_size - 1;

	if (mask) {
		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;

		fecid = fecid | KEYGEN_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[KEYGEN_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return 0;
}

#if 0
int keygen_kcr_builder_add_lookup_result_field_fec(
	enum kcr_builder_ext_lookup_res_field extract_field,
	uint8_t offset_in_opaque, uint8_t extract_size_in_opaque,
	struct kcr_builder_fec_mask *mask, struct kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t fecid, op0, op1, op2;
	uint8_t	fec_bytes_num = KEYGEN_KCR_LOOKUP_RES_FEC_SIZE;
	uint8_t	mask_bytes = 0;

	/* Build the FEC */
	/* General extraction FECID, mask extension indication */
	fecid = KEYGEN_KCR_GEC_FECID << 1;

	switch (extract_field) {

	case (KEYGEN_KCR_EXT_OPAQUE0):
		op0 = KEYGEN_KCR_OP0_HET_GEC | KEYGEN_KCR_EXT_OPAQUE0_EOM;
		op1 = KEYGEN_KCR_EXT_OPAQUE0_BASIC_EO + offset_in_opaque;
		op2 = extract_size_in_opaque - 1;
		break;
	case (KEYGEN_KCR_EXT_OPAQUE1):
		op0 = KEYGEN_KCR_OP0_HET_GEC | KEYGEN_KCR_EXT_OPAQUE1_EOM;
		op1 = KEYGEN_KCR_EXT_OPAQUE1_BASIC_EO + offset_in_opaque;
		op2 = extract_size_in_opaque - 1;
		break;
	case (KEYGEN_KCR_EXT_OPAQUE2):
		op0 = KEYGEN_KCR_OP0_HET_GEC | KEYGEN_KCR_EXT_OPAQUE2_EOM;
		op1 = KEYGEN_KCR_EXT_OPAQUE2_BASIC_EO;
		op2 = KEYGEN_KCR_EXT_OPAQUE2_SIZE;
		break;
	case (KEYGEN_KCR_EXT_UNIQUE_ID):
		op0 = KEYGEN_KCR_OP0_HET_GEC | KEYGEN_KCR_EXT_UNIQUE_ID_EOM;
		op1 = KEYGEN_KCR_EXT_UNIQUE_ID_BASIC_EO;
		op2 = KEYGEN_KCR_EXT_UNIQUE_ID_SIZE;
		break;
	case (KEYGEN_KCR_EXT_TIMESTAMP):
		op0 = KEYGEN_KCR_OP0_HET_GEC | KEYGEN_KCR_EXT_TIMESTAMP_EOM;
		op1 = KEYGEN_KCR_EXT_TIMESTAMP_BASIC_EO;
		op2 = KEYGEN_KCR_EXT_TIMESTAMP_SIZE;
		break;
	default:
		break;
	}


	if (mask) {
		/* build fec_mask */
		mask_bytes = ((mask->num_of_masks == 1) ? 2 :
				(mask->num_of_masks == 2) ? 4 :
				(mask->num_of_masks == 3) ? 5 : 7);
		fec_bytes_num = fec_bytes_num + mask_bytes;
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;

		fecid = fecid | KEYGEN_KCR_MASK_EXT;

		switch (mask->num_of_masks) {
		case(4):
			kb->kcr[curr_byte+10] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+9] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+8] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+7] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+6] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+5] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+4] =
				((mask->num_of_masks - 1) << 4) |
					mask->single_mask[0].mask_offset;
			break;
		default:
			break;
		}
	} else {
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;
	}

	/* Update kcr_builder struct */
	kb->kcr[curr_byte] = fecid;
	kb->kcr[curr_byte+1] = op0;
	kb->kcr[curr_byte+2] = op1;
	kb->kcr[curr_byte+3] = op2;
	kb->kcr[KEYGEN_KCR_NFEC] += 1;
	kb->kcr_length += 4 + mask_bytes;

	return 0;
}
#endif


int keygen_kcr_builder_add_valid_field_fec(uint8_t mask,
					 struct kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	nmsk_moff0 = 0x00; /* indicates 1 bytes mask from offset 0x00 */
	uint8_t	fec_bytes_num = KEYGEN_KCR_VALID_FIELD_FEC_SIZE;

	if (mask) {
		fec_bytes_num = fec_bytes_num + 2;
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;

		kb->kcr[curr_byte] = (KEYGEN_KCR_VF_FECID << 1) |
				     KEYGEN_KCR_MASK_EXT;
		kb->kcr[curr_byte+1] = nmsk_moff0;
		kb->kcr[curr_byte+2] = mask;

	} else {
		if ((curr_byte + fec_bytes_num) > KEYGEN_KCR_MAX_KCR_SIZE)
			return -EINVAL;

		kb->kcr[curr_byte] = KEYGEN_KCR_VF_FECID << 1;
	}
	/* Build the FEC */
	/* Valid field FECID, mask extension indication*/

	/* Update kcr_builder struct */
	kb->kcr[0] += 1;
	kb->kcr_length += fec_bytes_num;

	return 0;
}


void keygen_kcr_replace(enum keygen_hw_accel_id acc_id,
			 uint8_t *kcr,
			 uint8_t keyid)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("keygen.c", (uint32_t)kcr, ALIGNMENT_16B);
#endif

	/* Prepare HW context for TLU accelerator call */
	__stqw(KEYGEN_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)kcr) << 16, ((uint32_t)keyid) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	return;
}


int keygen_kcr_delete(enum keygen_hw_accel_id acc_id,
			uint8_t keyid)
{
	int32_t status;
	volatile uint8_t fake_kcr __attribute__((aligned(16))) = 0;

	/* Prepare HW context for TLU accelerator call */
	__stqw(KEYGEN_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)&fake_kcr) << 16, ((uint32_t)keyid) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	status = release_id(keyid, ext_keyid_pool_address);

	return status;
}


void keygen_kcr_query(enum keygen_hw_accel_id acc_id,
		       uint8_t keyid, uint8_t *kcr)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("keygen.c", (uint32_t)kcr, ALIGNMENT_16B);
#endif
	
	/* Prepare HW context for TLU accelerator call */
	__stqw(KEYGEN_KEY_COMPOSITION_RULE_QUERY_MTYPE, (uint32_t)kcr,
	       ((uint32_t)keyid) << 16, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	return;
}


int keygen_gen_hash(void *key, uint8_t key_size, uint32_t *hash)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("keygen.c", (uint32_t)key, ALIGNMENT_16B);
#endif

	int32_t status;

	/* Prepare HW context for TLU accelerator call */
	__stqw(KEYGEN_HASH_GEN_KEY_MTYPE, ((uint32_t)key) << 16,
	       ((uint32_t)key_size) << 16, 0, HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwacceli(KEYGEN_ACCEL_ID_CTLU);

	*hash = *((uint32_t *)HWC_ACC_OUT_ADDRESS2);

	/* Return status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == KEYGEN_HW_STATUS_SUCCESS) {
		return 0;
	} else {
		keygen_exception_handler(KEYGEN_GEN_HASH,
			__LINE__, 
			status);
		return (-1);
	}
}

int keygen_gen_key_wrp(enum keygen_hw_accel_id acc_id,
		     uint8_t keyid,
		     uint64_t user_metadata,
		     void *key,
		     uint8_t *key_size)
{
	return keygen_gen_key(acc_id,
			      keyid,
			      user_metadata,
			      key,
			      key_size);
}

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

void keygen_exception_handler(enum keygen_function_identifier func_id,
		     uint32_t line,
		     int32_t status)
{
	char *func_name;
	char *err_msg;
	
	/* Translate function ID to function name string */
	switch(func_id) {
	case KEYGEN_GEN_KEY:
		func_name = "keygen_gen_key";
		break;
	case KEYGEN_GEN_HASH:
		func_name = "keygen_gen_hash";
		break;
	default:
		/* create own exception */
		func_name = "Unknown Function";
	}
	
	/* Translate error ID to error name string */
	if (status & KEYGEN_HW_STATUS_KSE) {
		err_msg = "Key composition error: Invalid KeyID or KeyID with"
				"0 FECs or Key size error (key>124 bytes).\n";
	} else {
		err_msg = "Unknown or Invalid status.\n";
	}
	
	exception_handler(__FILE__, func_name, line, err_msg);
}

#pragma pop




