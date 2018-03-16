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
@File		parser_inline.h

@Description	This file contains the AIOP SW Parser Inline API implementation.

*//***************************************************************************/

#ifndef __PARSER_INLINE_H
#define __PARSER_INLINE_H


#include "parser.h"
#include "general.h"
#include "fsl_types.h"
#include "fsl_fdma.h"
#include "fsl_parser.h"
#include "net.h"

#include "system.h"
#include "fsl_id_pool.h"


inline int parse_result_generate(uint16_t starting_hxs, uint8_t starting_offset,
				 uint8_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser_inline.h",(uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	uint32_t arg1, arg2;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	/* 8 Byte aligned for stqw optimization */
	struct parser_input_message_params input_struct
				__attribute__((aligned(16)));
	

	arg1 = (uint32_t)default_task_params.parser_profile_id;
	arg1 = __e_rlwimi(arg1, (uint32_t)flags, 8, 16, 23);
	arg1 = __e_rlwimi(arg1, (uint32_t)starting_hxs, 13, 8, 18);
	arg1 = __e_rlwimi(arg1, (uint32_t)starting_offset, 24, 0, 7);

	/* If Gross Running Sum != 0 then it is valid */
	if (pr->gross_running_sum) {
		input_struct.gross_running_sum = pr->gross_running_sum;
		
		arg2 = ((uint32_t)(&input_struct) << 16);
		
		__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
			arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
	} else {
		/* If L4 checksum validation is required, calculate it first */
		if (flags & PARSER_VALIDATE_L4_CHECKSUM) {
			fdma_calculate_default_frame_checksum(0, 0xFFFF,
					&(input_struct.gross_running_sum));
						
			arg2 = ((uint32_t)(&input_struct) << 16);
		
			__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
		} else {
			/* Gross Running Sum == 0 and validation is not
			 * required */
			__stqw(PARSER_GEN_PARSE_RES_MTYPE, 0, 0, arg1,
			HWC_ACC_IN_ADDRESS, 0);
		}
	}

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;

	/* We assume that PARSER_HW_STATUS_PARSING_ERROR bit is always set
	 * for any kind of parsing error */
	 if ((status & ~(PARSER_HW_STATUS_FAIL_L3_CHECKSUM_VALIDATION_ERROR |
					 PARSER_HW_STATUS_FAIL_L4_CHECKSUM_VALIDATION_ERROR))
		 == PARSER_HW_STATUS_SUCCESS) {
		return 0;
	} else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
		return (-1);
	} else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE,
			__LINE__, 
			(status & 
			PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION));
		return (-1);
	} else if (status & PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED) {
		return -ENOSPC;
	} else {
		return -EIO;
	}
}

inline int parse_result_generate_default(uint8_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser_inline.h",(uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	uint32_t arg1, arg2;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct parser_input_message_params input_struct
					__attribute__((aligned(16)));

	
	arg1 = (uint32_t)default_task_params.parser_profile_id;
	arg1 = __e_rlwimi(arg1, (uint32_t)flags, 8, 16, 23);
	arg1 = __e_rlwimi(arg1,
			(uint32_t)default_task_params.parser_starting_hxs,
			13, 8, 18);

	/* If Gross Running Sum != 0 then it is valid */
	if (pr->gross_running_sum) {
		input_struct.gross_running_sum = pr->gross_running_sum;

		arg2 = ((uint32_t)(&input_struct) << 16);

		__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
	} else {
		/* If L4 checksum validation is required, calculate it first */
		if (flags & PARSER_VALIDATE_L4_CHECKSUM) {

			fdma_calculate_default_frame_checksum(0, 0xFFFF,
					&(input_struct.gross_running_sum));

			arg2 = ((uint32_t)(&input_struct) << 16);
			
			__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
		} else {
			/* Gross Running Sum == 0 and validation is not
			 * required */
			__stqw(PARSER_GEN_PARSE_RES_MTYPE, 0, 0, arg1,
			HWC_ACC_IN_ADDRESS, 0);
		}
	}
	
	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
	/* implementation of errors is priority based (if-else) since multiple
	 * error indications may appear at the same time */
	/* We assume that PARSER_HW_STATUS_PARSING_ERROR bit is always set
	 * for any kind of parsing error */
	 if ((status & ~(PARSER_HW_STATUS_FAIL_L3_CHECKSUM_VALIDATION_ERROR |
					 PARSER_HW_STATUS_FAIL_L4_CHECKSUM_VALIDATION_ERROR))
		 == PARSER_HW_STATUS_SUCCESS) {
		return 0;
	} else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE_DEFAULT,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
		return (-1);
	} else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE_DEFAULT,
			__LINE__, 
			(status & 
			PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION));
		return (-1);
	} else if (status & PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED) {
		return -ENOSPC;
	} else {
		return -EIO;
	}
}

static inline int parse_result_generate_basic_common(uint32_t arg1)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser_inline.h",(uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	uint32_t arg2;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct parser_input_message_params input_struct
					__attribute__((aligned(16)));

	
	input_struct.gross_running_sum = pr->gross_running_sum;

	arg2 = ((uint32_t)(&input_struct) << 16);
	
	__stqw(arg1,
		arg2, 0, 0, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);


	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
      
	if ((status & ~PARSER_HW_STATUS_L3_L4_CHECKSUM_GENERATION_SUCCEEDED)
			== PARSER_HW_STATUS_SUCCESS) {
             return 0;
       } else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE_BASIC,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
             return (-1);
       } else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE_BASIC,
			__LINE__, 
			(status & 
			PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION));
             return (-1);
       } else if (status & PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED) {
             return -ENOSPC;
       } else {
             return -EIO;
       }
}

inline int parse_result_generate_basic(void)
{
	return parse_result_generate_basic_common(
			PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE);
}

inline int parse_result_generate_basic_with_ref_take(void)
{
	return parse_result_generate_basic_common(PARSER_EPHS_MASK
			| PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE);
}

inline void parser_pop_vlan_update()
{
	uint8_t temp_8b, i;
	uint32_t temp_32b;
	struct parse_result *pr =(struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct presentation_context *prc= (struct presentation_context *)HWC_PRC_ADDRESS;
	uint16_t seg_address = prc->seg_address;

	temp_8b = pr->last_etype_offset - sizeof(struct vlanhdr);
	pr->last_etype_offset = temp_8b;
	uint16_t l3_protocol = *(uint16_t *)((uint16_t)temp_8b + seg_address);
	void *next_header = (void *)((uint16_t)pr->l3_offset - sizeof(struct vlanhdr) + seg_address);
	
	/* Common Stack: ETHERNET-IPV4/IPV6-TCP/UDP */
	/* IPV4 */
	
	if ((l3_protocol == NET_ETH_ETYPE_IPV4) && 
		((((struct ipv4hdr *)next_header)->protocol == TCP_PROTOCOL) || 
		(((struct ipv4hdr *)next_header)->protocol == UDP_PROTOCOL))) {
			pr->ip_1_pid_offset = pr->ip_1_pid_offset - sizeof(struct vlanhdr);
			pr->l3_offset = pr->l3_offset - sizeof(struct vlanhdr);
			pr->ipn_or_minencapO_offset = pr->l3_offset;
			pr->l4_offset = pr->l4_offset - sizeof(struct vlanhdr);
			temp_8b = pr->l5_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l5_offset = temp_8b - sizeof(struct vlanhdr);
			pr->nxt_hdr_offset = pr->nxt_hdr_offset - sizeof(struct vlanhdr);
			pr->gross_running_sum = 0;
	}
	/* IPV6 */
	else {
		if ((l3_protocol == NET_ETH_ETYPE_IPV6) && 
			((((struct ipv6hdr *)next_header)->next_header == TCP_PROTOCOL) || 
			(((struct ipv6hdr *)next_header)->next_header == UDP_PROTOCOL))) {
				pr->ip_1_pid_offset = pr->ip_1_pid_offset - sizeof(struct vlanhdr);
				pr->l3_offset = pr->l3_offset - sizeof(struct vlanhdr);
				pr->ipn_or_minencapO_offset = pr->l3_offset;
				pr->l4_offset = pr->l4_offset - sizeof(struct vlanhdr);
				temp_8b = pr->l5_offset;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->l5_offset = temp_8b - sizeof(struct vlanhdr);
				pr->nxt_hdr_offset = pr->nxt_hdr_offset - sizeof(struct vlanhdr);
				pr->gross_running_sum = 0;
				temp_8b = pr->routing_hdr_offset1;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE) {
					pr->routing_hdr_offset1 = temp_8b - sizeof(struct vlanhdr);
					temp_8b = pr->routing_hdr_offset2;
					if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
						pr->routing_hdr_offset2 = temp_8b - sizeof(struct vlanhdr);
				}
				temp_8b = pr->ipv6_frag_offset;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->ipv6_frag_offset = temp_8b - sizeof(struct vlanhdr);
		}
		/* not a common stack */
		else {
			temp_8b = pr->llc_snap_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->llc_snap_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->pppoe_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->pppoe_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->mpls_offset_1;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE) {
				pr->mpls_offset_1 = temp_8b - sizeof(struct vlanhdr);
				temp_8b = pr->mpls_offset_n;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->mpls_offset_n = temp_8b - sizeof(struct vlanhdr);
			}
			temp_8b = pr->ip_1_pid_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ip_1_pid_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->l3_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l3_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->ipn_or_minencapO_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ipn_or_minencapO_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->gre_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->gre_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->l4_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l4_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->l5_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l5_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->routing_hdr_offset1;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE) {
				pr->routing_hdr_offset1 = temp_8b - sizeof(struct vlanhdr);
				temp_8b = pr->routing_hdr_offset2;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->routing_hdr_offset2 = temp_8b - sizeof(struct vlanhdr);
			}
		
			pr->nxt_hdr_offset = pr->nxt_hdr_offset - sizeof(struct vlanhdr);
			temp_8b = pr->ipv6_frag_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ipv6_frag_offset = temp_8b - sizeof(struct vlanhdr);
			pr->gross_running_sum = 0;	//reset gross_running_sum
			temp_8b = pr->nxt_hdr_before_ipv6_frag_ext;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->nxt_hdr_before_ipv6_frag_ext = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->ip_n_pid_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ip_n_pid_offset = temp_8b - sizeof(struct vlanhdr);
		}
	}

	/* check if this is the only VLAN (no VLAN left in the frame)
	 * assume that PARSER_IS_ONE_VLAN_DEFAULT() is true since user applied pop_vlan earlier */
	temp_32b = pr->frame_attribute_flags_1;
	if (!(temp_32b & PARSER_ATT_VLAN_N_MASK)) {
		temp_32b &= ~(PARSER_ATT_CFI_IN_VLAN_MASK | PARSER_ATT_VLAN_1_MASK | PARSER_ATT_VLAN_PRIORITY_MASK);
		pr->frame_attribute_flags_1 = temp_32b;
		/* reset VLANTCIOffset_1, VLANTCIOffset_n */
		*((uint16_t *) &(pr->vlan_tci1_offset)) = 
				PARSER_UNINITILIZED_FIELD_OF_HALF_WORD; 
	}

	/* check if there is only 1 VLAN left in the frame */
	else {
		temp_8b = pr->vlan_tci1_offset;
		if (PARSER_GET_LAST_VLAN_TCI_OFFSET_DEFAULT() - temp_8b == sizeof(struct vlanhdr)) {
			pr->frame_attribute_flags_1 = temp_32b & ~PARSER_ATT_VLAN_N_MASK; 		//reset VLAN n Present (bit 22)
			pr->vlan_tcin_offset = temp_8b;				//VLANTCIOffset_n = VLANTCIOffset_1
		}
		/* there is more than 1 VLAN left in the frame */
		else
			pr->vlan_tcin_offset = pr->vlan_tcin_offset - sizeof(struct vlanhdr);
	}
	
	/* check if VID=0 for the removed VLAN
	 * check if CFI should be reset */
	int found_vid, found_cfi;
	temp_32b = pr->frame_attribute_flags_1;
	if ((temp_32b & PARSER_ATT_CFI_IN_VLAN_MASK) || 
			(temp_32b & PARSER_ATT_VLAN_PRIORITY_MASK)) {
		found_vid = FALSE, found_cfi = FALSE;
		temp_8b = pr->vlan_tcin_offset;
		for (i = pr->vlan_tci1_offset; i <= temp_8b; i = i+sizeof(struct vlanhdr) /* next TCI offset */) {
			if (!(*(uint8_t *)i & VLAN_VID_MASK))
				found_vid = TRUE;
			if ((*(uint8_t *)i & VLAN_TPID_MASK == 0x8100) &&
				(*(uint8_t *)i & VLAN_CFI_MASK))
				found_cfi = TRUE;
		}
		if (!found_vid)
			temp_32b &= ~PARSER_ATT_VLAN_PRIORITY_MASK;	//reset VLAN Prio frame
		if (!found_cfi)
			temp_32b &= ~PARSER_ATT_CFI_IN_VLAN_MASK;	//reset CFI
		pr->frame_attribute_flags_1 = temp_32b;
	}
}

inline void parser_push_vlan_update()
{
	uint8_t temp_8b;
	uint16_t temp_16b;
	uint32_t temp_32b;
	struct parse_result *pr =(struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct presentation_context *prc= (struct presentation_context *)HWC_PRC_ADDRESS;
	uint16_t seg_address = prc->seg_address;
	
	temp_8b = pr->last_etype_offset + sizeof(struct vlanhdr);
	pr->last_etype_offset = temp_8b;
	uint16_t l3_protocol = *(uint16_t *)((uint16_t)temp_8b + seg_address);
	void *next_header = (void *)((uint16_t)pr->l3_offset + sizeof(struct vlanhdr) + seg_address);

	/* Common Stack: ETHERNET-IPV4/IPV6-TCP/UDP */
	/* IPV4 */
	if ((l3_protocol == NET_ETH_ETYPE_IPV4) && 
		((((struct ipv4hdr *)next_header)->protocol == TCP_PROTOCOL) || 
		(((struct ipv4hdr *)next_header)->protocol == UDP_PROTOCOL))) {
			pr->ip_1_pid_offset = pr->ip_1_pid_offset + sizeof(struct vlanhdr);
			pr->l3_offset = pr->l3_offset + sizeof(struct vlanhdr);
			pr->ipn_or_minencapO_offset = pr->l3_offset;
			pr->l4_offset = pr->l4_offset + sizeof(struct vlanhdr);
			temp_8b = pr->l5_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l5_offset = temp_8b + sizeof(struct vlanhdr);
			pr->nxt_hdr_offset = pr->nxt_hdr_offset + sizeof(struct vlanhdr);
			pr->gross_running_sum = 0;
	}
	else {
		if ((l3_protocol == NET_ETH_ETYPE_IPV6) && 
			((((struct ipv6hdr *)next_header)->next_header == TCP_PROTOCOL) || 
			(((struct ipv6hdr *)next_header)->next_header == UDP_PROTOCOL))) {
				pr->ip_1_pid_offset = pr->ip_1_pid_offset + sizeof(struct vlanhdr);
				pr->l3_offset = pr->l3_offset + sizeof(struct vlanhdr);
				pr->ipn_or_minencapO_offset = pr->l3_offset;
				pr->l4_offset = pr->l4_offset + sizeof(struct vlanhdr);
				temp_8b = pr->l5_offset;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->l5_offset = temp_8b + sizeof(struct vlanhdr);
				pr->nxt_hdr_offset = pr->nxt_hdr_offset + sizeof(struct vlanhdr);
				pr->gross_running_sum = 0;
				temp_8b = pr->routing_hdr_offset1;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE) {
					pr->routing_hdr_offset1 = temp_8b + sizeof(struct vlanhdr);
					temp_8b = pr->routing_hdr_offset2;
					if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
						pr->routing_hdr_offset2 = temp_8b + sizeof(struct vlanhdr);
				}
				temp_8b = pr->ipv6_frag_offset;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->ipv6_frag_offset = temp_8b + sizeof(struct vlanhdr);
		}
		/* not a common stack */
		else {
			temp_8b = pr->llc_snap_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->llc_snap_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->pppoe_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->pppoe_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->mpls_offset_1;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE) {
				pr->mpls_offset_1 = temp_8b + sizeof(struct vlanhdr);
				temp_8b = pr->mpls_offset_n;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->mpls_offset_n = temp_8b + sizeof(struct vlanhdr);
			}
			temp_8b = pr->ip_1_pid_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ip_1_pid_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->l3_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l3_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->ipn_or_minencapO_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ipn_or_minencapO_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->gre_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->gre_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->l4_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l4_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->l5_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l5_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->routing_hdr_offset1;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE) {
				pr->routing_hdr_offset1 = temp_8b + sizeof(struct vlanhdr);
				temp_8b = pr->routing_hdr_offset2;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->routing_hdr_offset2 = temp_8b + sizeof(struct vlanhdr);
			}
			pr->nxt_hdr_offset = pr->nxt_hdr_offset + sizeof(struct vlanhdr);
			temp_8b = pr->ipv6_frag_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ipv6_frag_offset = temp_8b + sizeof(struct vlanhdr);
			pr->gross_running_sum = 0;	//reset gross_running_sum
			temp_8b = pr->nxt_hdr_before_ipv6_frag_ext;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->nxt_hdr_before_ipv6_frag_ext = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->ip_n_pid_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ip_n_pid_offset = temp_8b + sizeof(struct vlanhdr);
		}
	}

	/* check if there is no VLAN in the frame */
	temp_32b = pr->frame_attribute_flags_1;
	if (!(temp_32b & PARSER_ATT_VLAN_1_MASK)) {
		pr->vlan_tci1_offset = pr->eth_offset + 2*NET_HDR_FLD_ETH_ADDR_SIZE + offsetof(struct vlanhdr, tci);
		pr->vlan_tcin_offset = pr->vlan_tci1_offset;
		pr->frame_attribute_flags_1 = temp_32b | PARSER_ATT_VLAN_1_MASK;
	}

	/* check if there is only 1 VLAN in the frame */
	else {
		if (!(temp_32b & PARSER_ATT_VLAN_N_MASK)) {
			pr->vlan_tcin_offset = pr->vlan_tci1_offset + sizeof(struct vlanhdr);
			pr->frame_attribute_flags_1 = temp_32b | PARSER_ATT_VLAN_N_MASK;
		}
		else
			pr->vlan_tcin_offset = pr->vlan_tcin_offset + sizeof(struct vlanhdr);
	}
	
	temp_32b = pr->frame_attribute_flags_1;
	temp_16b = (uint16_t)pr->vlan_tci1_offset -offsetof(struct vlanhdr, tci) + seg_address;
	
	/* check if VID=0 for the added VLAN */
	if (!(temp_32b & PARSER_ATT_VLAN_PRIORITY_MASK))
		if (!(*((uint32_t *)temp_16b) & VLAN_VID_MASK))
			temp_32b |= PARSER_ATT_VLAN_PRIORITY_MASK;
	
	/* check if CFI should be set on */
	if ((*(uint32_t *)temp_16b & VLAN_TPID_MASK == 0x8100) &&
			(*(uint32_t *)temp_16b & VLAN_CFI_MASK))
		temp_32b |= PARSER_ATT_CFI_IN_VLAN_MASK;
	
	pr->frame_attribute_flags_1 = temp_32b;
}

inline int parse_result_generate_checksum(
		enum parser_starting_hxs_code starting_hxs,
		uint8_t starting_offset, uint16_t *l3_checksum,
		uint16_t *l4_checksum)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("parser.c", (uint32_t *)PRC_GET_SEGMENT_ADDRESS(), ALIGNMENT_16B);
#endif
	
	uint32_t arg1, arg2;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct parser_input_message_params input_struct
					__attribute__((aligned(16)));

	
	/* Check if Gross Running Sum calculation is needed */
	if (!pr->gross_running_sum) {
		fdma_calculate_default_frame_checksum(0, 0xFFFF,
					      &input_struct.gross_running_sum);
	} else {
		input_struct.gross_running_sum = pr->gross_running_sum;
	}

	arg1 = (uint32_t)default_task_params.parser_profile_id;
	arg1 = __e_rlwimi(arg1, (uint32_t)starting_hxs, 13, 8, 18);
	arg1 = __e_rlwimi(arg1, (uint32_t)starting_offset, 24, 0, 7);

	arg2 = ((uint32_t)(&input_struct) << 16);

	__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE), arg2, 0, arg1,
						HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
	if (status == PARSER_HW_STATUS_L3_L4_CHECKSUM_GENERATION_SUCCEEDED) {
		*l3_checksum = *((uint16_t *)HWC_ACC_OUT_ADDRESS2);
		*l4_checksum = *((uint16_t *)(HWC_ACC_OUT_ADDRESS2+2));
		return PARSER_STATUS_L3_L4_CHECKSUM_GENERATION_SUCCEEDED;
	} else if (status == PARSER_HW_STATUS_L4_CHECKSUM_GENERATION_SUCCEEDED){
		*l4_checksum = *((uint16_t *)(HWC_ACC_OUT_ADDRESS2+2));
		return PARSER_STATUS_L4_CHECKSUM_GENERATION_SUCCEEDED;
	} else if (status == PARSER_HW_STATUS_L3_CHECKSUM_GENERATION_SUCCEEDED){
		*l3_checksum = *((uint16_t *)HWC_ACC_OUT_ADDRESS2);
		return PARSER_STATUS_L3_CHECKSUM_GENERATION_SUCCEEDED;
/*
	} else if (status == PARSER_HW_STATUS_CHECKSUM_GENERATION_FAILED){
		return -EIO;
*/
	} else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE_CHECKSUM,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
		return (-1);
	} else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE_CHECKSUM,
			__LINE__, 
			(status & 
			PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION));
		return (-1);
	} else if (status & PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED) {
		return -ENOSPC;
	} else {
		return -EIO;
	}
}

#endif /* __PARSER_INLINE_H */
