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
@File		keygen_inline.h

@Description	This file contains the AIOP SW KeyGen Inline API implementation.

*//***************************************************************************/

#ifndef __FSL_KEYGEN_INLINE_H
#define __FSL_KEYGEN_INLINE_H

#include "fsl_keygen.h"
#include "keygen.h"

extern uint64_t ext_keyid_pool_address;

inline void keygen_kcr_builder_init(struct kcr_builder *kb)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN((uint32_t)kb, ALIGNMENT_16B);
#endif

	/* clear the KCR array */
	cdma_ws_memory_init(kb->kcr, KEYGEN_KCR_LENGTH, 0x0);

	/* Initialize KCR length to 1 */
	kb->kcr_length = 1;
}

inline int keygen_gen_key(enum keygen_hw_accel_id acc_id,
		     uint8_t keyid,
		     uint64_t user_metadata,
		     void *key,
		     uint8_t *key_size)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN((uint32_t)key, ALIGNMENT_16B);
	DEBUG_ALIGN((uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	int32_t status;
	struct keygen_input_message_params input_struct
					__attribute__((aligned(16)));
	uint32_t arg1;

	if (user_metadata) {
/*
		__stdw(0, 0, 0, &input_struct);
		__stdw(0, 0, 8, &input_struct);
*/
		input_struct.opaquein = user_metadata;

		/* Prepare HW context for TLU accelerator call */
		arg1 = ((((uint32_t)(&input_struct)) << 16) | (uint32_t)key);
		__stqw((KEYGEN_KEY_GENERATE_EPRS_MTYPE | KEYGEN_OPAQUEIN_VALID),
			arg1, ((uint32_t)keyid) << 16, 0,
				HWC_ACC_IN_ADDRESS, 0);
	} else {
	/* Prepare HW context for TLU accelerator call */
	__stqw(KEYGEN_KEY_GENERATE_EPRS_MTYPE, ((uint32_t)key),
	       ((uint32_t)keyid) << 16, 0, HWC_ACC_IN_ADDRESS, 0);
	}

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	*key_size = *(((uint8_t *)HWC_ACC_OUT_ADDRESS) + 5);

	/* Return status */
	status = *((int32_t *)HWC_ACC_OUT_ADDRESS);
	if (status == KEYGEN_HW_STATUS_SUCCESS) {
		return 0;
	} else if (status == KEYGEN_HW_STATUS_EOFH) {
		return -EIO;
	} else {
		keygen_exception_handler(KEYGEN_GEN_KEY,
			__LINE__, 
			status);
		return (-1);	
	}
}

inline int keygen_kcr_builder_add_protocol_specific_field(enum
	kcr_builder_protocol_fecid protocol_fecid,
	struct kcr_builder_fec_mask *mask, struct kcr_builder *kb)
{
	uint8_t curr_byte = kb->kcr_length;
	uint8_t	mask_bytes = 0;
	uint8_t	fec_bytes_num = KEYGEN_KCR_PROTOCOL_SPECIFIC_FEC_SIZE;
	uint8_t fecid;

	/* Build the FEC */
	/* Protocol Specific FECID, mask extension indication */
	fecid = (uint8_t)protocol_fecid << 1;

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
			kb->kcr[curr_byte+7] =
				mask->single_mask[3].mask;
			kb->kcr[curr_byte+6] =
				mask->single_mask[3].mask_offset;
		case(3):
			kb->kcr[curr_byte+5] =
				mask->single_mask[2].mask;
			kb->kcr[curr_byte+3] =
				mask->single_mask[2].mask_offset;
		case(2):
			kb->kcr[curr_byte+4] =
				mask->single_mask[1].mask;
			kb->kcr[curr_byte+3] |=
				mask->single_mask[1].mask_offset << 4;
		case(1):
			kb->kcr[curr_byte+2] =
				mask->single_mask[0].mask;
			kb->kcr[curr_byte+1] =
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
	kb->kcr[KEYGEN_KCR_NFEC] += 1;
	kb->kcr_length += fec_bytes_num;

	return 0;
}

inline int keygen_kcr_create(enum keygen_hw_accel_id acc_id,
			uint8_t *kcr,
			uint8_t *keyid)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN((uint32_t)kcr, ALIGNMENT_16B);
#endif
	
	int32_t status;

	status = get_id(ext_keyid_pool_address, keyid);

	if (status != 0)
		return status;

	/* Prepare HW context for TLU accelerator call */
	__stqw(KEYGEN_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE,
	       ((uint32_t)kcr) << 16, ((uint32_t)(*keyid)) << 16, 0,
	       HWC_ACC_IN_ADDRESS, 0);

	/* Call CTLU accelerator */
	__e_hwaccel(acc_id);

	/* Return status */
	return 0;
}

#endif /* __FSL_KEYGEN_INLINE_H */
