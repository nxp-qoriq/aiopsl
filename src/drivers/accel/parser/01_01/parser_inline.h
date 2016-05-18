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
#include "fsl_net.h"

#include "system.h"
#include "fsl_id_pool.h"

inline int parse_result_generate(enum parser_starting_hxs_code starting_hxs,
	uint8_t starting_offset, uint8_t flags)
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
	if (status == PARSER_HW_STATUS_SUCCESS) {
		return 0;
	} else if ((status ==
			PARSER_HW_STATUS_L3_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L4_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L3_L4_CHECKSUM_VALIDATION_SUCCEEDED)) {
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
	if (status == PARSER_HW_STATUS_SUCCESS) {
		return 0;
	} else if ((status ==
			PARSER_HW_STATUS_L3_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L4_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L3_L4_CHECKSUM_VALIDATION_SUCCEEDED)) {
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

inline int parse_result_generate_basic(void)
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
	
//	__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
//		arg2, 0, 0, HWC_ACC_IN_ADDRESS, 0);
	
	__stdw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
		arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(0,0, HWC_ACC_IN_ADDRESS3, 0);

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);


	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
       if (status == PARSER_HW_STATUS_SUCCESS) {
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

inline void parser_pop_vlan_update()
{
	uint8_t temp_8b;
	uint32_t temp_32b;
	struct parse_result *pr =(struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct presentation_context *prc= (struct presentation_context *)HWC_PRC_ADDRESS;
	uint16_t seg_address = prc->seg_address;

	temp_8b = pr->last_etype_offset - sizeof(struct vlanhdr);
	pr->last_etype_offset = temp_8b;
	uint16_t l3_protocol = *(uint16_t *)((uint16_t)temp_8b + seg_address);
	void *next_header = (void *)((uint16_t)pr->ip1_or_arp_offset - sizeof(struct vlanhdr) + seg_address);
	
	/* Common Stack: ETHERNET-IPV4/IPV6-TCP/UDP */
	/* IPV4 */
	
	if ((l3_protocol == NET_ETH_ETYPE_IPV4) && 
		((((struct ipv4hdr *)next_header)->protocol == TCP_PROTOCOL) || 
		(((struct ipv4hdr *)next_header)->protocol == UDP_PROTOCOL))) {
			pr->ip_pid_offset = pr->ip_pid_offset - sizeof(struct vlanhdr);
			pr->ip1_or_arp_offset = pr->ip1_or_arp_offset - sizeof(struct vlanhdr);
			pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
			pr->l4_offset = pr->l4_offset - sizeof(struct vlanhdr);
			temp_8b = pr->gtp_esp_ipsec_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->gtp_esp_ipsec_offset = temp_8b - sizeof(struct vlanhdr);
			pr->nxt_hdr_offset = pr->nxt_hdr_offset - sizeof(struct vlanhdr);
			pr->gross_running_sum = 0;
	}
	/* IPV6 */
	else {
		if ((l3_protocol == NET_ETH_ETYPE_IPV6) && 
			((((struct ipv6hdr *)next_header)->next_header == TCP_PROTOCOL) || 
			(((struct ipv6hdr *)next_header)->next_header == UDP_PROTOCOL))) {
				pr->ip_pid_offset = pr->ip_pid_offset - sizeof(struct vlanhdr);
				pr->ip1_or_arp_offset = pr->ip1_or_arp_offset - sizeof(struct vlanhdr);
				pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
				pr->l4_offset = pr->l4_offset - sizeof(struct vlanhdr);
				temp_8b = pr->gtp_esp_ipsec_offset;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->gtp_esp_ipsec_offset = temp_8b - sizeof(struct vlanhdr);
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
			temp_8b = pr->ip_pid_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ip_pid_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->ip1_or_arp_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ip1_or_arp_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->ipn_or_minencapO_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ipn_or_minencapO_offset = temp_8b - sizeof(struct vlanhdr);
			else
				pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
			temp_8b = pr->gre_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->gre_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->l4_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l4_offset = temp_8b - sizeof(struct vlanhdr);
			temp_8b = pr->gtp_esp_ipsec_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->gtp_esp_ipsec_offset = temp_8b - sizeof(struct vlanhdr);
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
		}
	}

	/* check if this is the only VLAN (no VLAN left in the frame)
	 * assume that PARSER_IS_ONE_VLAN_DEFAULT() is true since user applied pop_vlan earlier */
	temp_32b = pr->frame_attribute_flags_1;
	if (!(temp_32b & PARSER_ATT_VLAN_N_MASK)) {
		temp_32b &= ~(PARSER_ATT_CFI_IN_VLAN_MASK | PARSER_ATT_VLAN_1_MASK);
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
}

inline void parser_push_vlan_update()
{
	uint8_t temp_8b;
	uint32_t temp_32b;
	struct parse_result *pr =(struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct presentation_context *prc= (struct presentation_context *)HWC_PRC_ADDRESS;
	uint16_t seg_address = prc->seg_address;
	
	temp_8b = pr->last_etype_offset + sizeof(struct vlanhdr);
	pr->last_etype_offset = temp_8b;
	uint16_t l3_protocol = *(uint16_t *)((uint16_t)temp_8b + seg_address);
	void *next_header = (void *)((uint16_t)pr->ip1_or_arp_offset + sizeof(struct vlanhdr) + seg_address);

	/* Common Stack: ETHERNET-IPV4/IPV6-TCP/UDP */
	/* IPV4 */
	if ((l3_protocol == NET_ETH_ETYPE_IPV4) && 
		((((struct ipv4hdr *)next_header)->protocol == TCP_PROTOCOL) || 
		(((struct ipv4hdr *)next_header)->protocol == UDP_PROTOCOL))) {
			pr->ip_pid_offset = pr->ip_pid_offset + sizeof(struct vlanhdr);
			pr->ip1_or_arp_offset = pr->ip1_or_arp_offset + sizeof(struct vlanhdr);
			pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
			pr->l4_offset = pr->l4_offset + sizeof(struct vlanhdr);
			temp_8b = pr->gtp_esp_ipsec_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->gtp_esp_ipsec_offset = temp_8b + sizeof(struct vlanhdr);
			pr->nxt_hdr_offset = pr->nxt_hdr_offset + sizeof(struct vlanhdr);
			pr->gross_running_sum = 0;
	}
	else {
		if ((l3_protocol == NET_ETH_ETYPE_IPV6) && 
			((((struct ipv6hdr *)next_header)->next_header == TCP_PROTOCOL) || 
			(((struct ipv6hdr *)next_header)->next_header == UDP_PROTOCOL))) {
				pr->ip_pid_offset = pr->ip_pid_offset + sizeof(struct vlanhdr);
				pr->ip1_or_arp_offset = pr->ip1_or_arp_offset + sizeof(struct vlanhdr);
				pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
				pr->l4_offset = pr->l4_offset + sizeof(struct vlanhdr);
				temp_8b = pr->gtp_esp_ipsec_offset;
				if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
					pr->gtp_esp_ipsec_offset = temp_8b + sizeof(struct vlanhdr);
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
			temp_8b = pr->ip_pid_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ip_pid_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->ip1_or_arp_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ip1_or_arp_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->ipn_or_minencapO_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->ipn_or_minencapO_offset = temp_8b + sizeof(struct vlanhdr);
			else
				pr->ipn_or_minencapO_offset = pr->ip1_or_arp_offset;
			temp_8b = pr->gre_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->gre_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->l4_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->l4_offset = temp_8b + sizeof(struct vlanhdr);
			temp_8b = pr->gtp_esp_ipsec_offset;
			if (temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE)
				pr->gtp_esp_ipsec_offset = temp_8b + sizeof(struct vlanhdr);
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

	/* check for truncation error:
	 * Frame Parsing reached the limit of the minimum between 
	 * presentation_length and 256 bytes before completing all parsing */
	
	/* pointing to the payload of the last parsed header. Can also be 
	 * defined as the offset to the last header that has not been parsed.
	 * nextHeaderOffset is pointing to the start of the data, therefore
	 * this is the size of all the headers */
	temp_8b = pr->nxt_hdr_offset;
	if ((temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (temp_8b > prc->seg_length)) {
		pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_PARSING_ERROR_MASK;
		fsl_print("BLE CASE\n");
//		fsl_print("ip_pid_offset = %x\n", pr->ip_pid_offset);	
//		fsl_print("eth_offset = %x\n", pr->eth_offset);	
//		fsl_print("llc_snap_offset = %x\n", pr->llc_snap_offset);	
//		fsl_print("vlan_tci1_offset = %x\n", pr->vlan_tci1_offset);	
//		fsl_print("vlan_tcin_offset = %x\n", pr->vlan_tcin_offset);	
//		fsl_print("last_etype_offset = %x\n", pr->last_etype_offset);	
//		fsl_print("pppoe_offset = %x\n", pr->pppoe_offset);	
//		fsl_print("mpls_offset_1 = %x\n", pr->mpls_offset_1);	
//		fsl_print("mpls_offset_n = %x\n", pr->mpls_offset_n);
//		fsl_print("ip1_or_arp_offset = %x\n", pr->ip1_or_arp_offset);
//		fsl_print("ipn_or_minencapO_offset = %x\n", pr->ipn_or_minencapO_offset);
//		fsl_print("gre_offset = %x\n", pr->gre_offset);
//		fsl_print("l4_offset = %x\n", pr->l4_offset);
//		fsl_print("gtp_esp_ipsec_offset = %x\n", pr->gtp_esp_ipsec_offset);
//		fsl_print("routing_hdr_offset1 = %x\n", pr->routing_hdr_offset1);
//		fsl_print("routing_hdr_offset2 = %x\n", pr->routing_hdr_offset2);
//		fsl_print("ipv6_frag_offset = %x\n", pr->ipv6_frag_offset);
		
		/* Next Header offset update */
		volatile uint8_t *j = 0;
		uint8_t max_offset = pr->ip_pid_offset;
		for (volatile uint8_t *i = &(pr->vlan_tci1_offset); i < &(pr->nxt_hdr_offset); i++)
			if ((*i != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (*i > max_offset)) {
				max_offset = *i;
				j = i;
			}
		temp_8b = pr->ipv6_frag_offset;
		if ((temp_8b != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (temp_8b > max_offset)) {
			max_offset = temp_8b;
			j = &(pr->ipv6_frag_offset);
		}
		pr->nxt_hdr_offset = max_offset;
		*j = PARSER_UNINITILIZED_FIELD_OF_BYTE;	

		/* Next Header update and FAF */
		if (j == &(pr->llc_snap_offset)) {
			pr->nxt_hdr = 0x05dc;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_LLC_SNAP_MASK;
		}
		if (j == &(pr->vlan_tci1_offset)) {
			pr->nxt_hdr = seg_address + (uint16_t)pr->vlan_tci1_offset; /* Next Header = TPID */
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_VLAN_1_MASK;
		}
		if (j == &(pr->vlan_tcin_offset)) {
			/* next header should be previous vlan tpid, not the first tpid necessarily */
			pr->nxt_hdr = seg_address + (uint16_t)pr->vlan_tci1_offset; /* Next Header = TPID */
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_VLAN_N_MASK;
		}
		if (j == &(pr->last_etype_offset)) {
			/* TODO:what is the next header?? */
		}
		if (j == &(pr->pppoe_offset)) {
			pr->nxt_hdr = 0x8864;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_PPPOE_PPP_MASK;
		}
		if (j == &(pr->mpls_offset_1)) {
			pr->nxt_hdr = 0x8847;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_MPLS_1_MASK;
		}
		if (j == &(pr->mpls_offset_n)) {
			pr->nxt_hdr = 0x8848;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_MPLS_N_MASK;
		}
		if (j == &(pr->ip1_or_arp_offset)) { /* l3 offset */
			if (pr->frame_attribute_flags_1 & PARSER_ATT_ARP_MASK) {
				pr->nxt_hdr = 0x0806;
				pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_ARP_MASK;
			}
			else {
				if (pr->frame_attribute_flags_2 & PARSER_ATT_IPV4_1_MASK) {
					pr->nxt_hdr = 0x0800;
					pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_IPV4_1_MASK;
					/* TODO: do I need to set off more flags? */
				}
				else {
					if (pr->frame_attribute_flags_2 & PARSER_ATT_IPV6_1_MASK) {
						pr->nxt_hdr = 0x086dd;
						pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_IPV6_1_MASK;
						/* TODO: do I need to set off more flags? */
					}
				}
			pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_IP_1_OPTIONS_MASK;
			/* TODO: do I need to set off more flags? */
			}
		}
		if (j == &(pr->ipn_or_minencapO_offset)) {
			if (pr->frame_attribute_flags_2 & PARSER_ATT_MIN_ENCAP_MASK) {
				pr->nxt_hdr = 0x55;
				pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_MIN_ENCAP_MASK;
			}
			if (pr->frame_attribute_flags_2 & PARSER_ATT_IP_N_OPTIONS_MASK) {
				/*TODO:pr->nxt_hdr = ???;*/
				pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_IP_N_OPTIONS_MASK;
			}
		}
		if (j == &(pr->gre_offset)) {
			pr->nxt_hdr = 0x47;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_GRE_MASK;
		}
		if (j == &(pr->l4_offset)) {
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_L4_UNKOWN_PROTOCOL_MASK;
			if (pr->frame_attribute_flags_3 & PARSER_ATT_TCP_MASK) {
				pr->nxt_hdr = 0x6;
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_TCP_MASK;
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_TCP_OPTIONS_MASK;
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_TCP_CONTROLS_6_11_SET_MASK;
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_TCP_CONTROLS_3_5_SET_MASK;
			}
			if (pr->frame_attribute_flags_3 & PARSER_ATT_UDP_MASK) {
				pr->nxt_hdr = 0x17;
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_UDP_MASK;
			}
			if (pr->frame_attribute_flags_3 & PARSER_ATT_DCCP_MASK) {
				pr->nxt_hdr = 0x33;
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_DCCP_MASK;
			}
			if (pr->frame_attribute_flags_3 & PARSER_ATT_SCTP_MASK) {
				pr->nxt_hdr = 0x132;
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_SCTP_MASK;
			}
		}
		if (j == &(pr->gtp_esp_ipsec_offset)) { /* l5 offset */
			if (pr->frame_attribute_flags_3 & PARSER_ATT_IPSEC_MASK) {
				pr->nxt_hdr = 0x50; /* TODO: 50 or 51??*/
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_IPSEC_MASK;
			}
			if (pr->frame_attribute_flags_3 & PARSER_ATT_GTP_MASK) {
				pr->nxt_hdr = 0x2123; /* TODO: 2123 or 2152 or 3386??*/
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_GTP_MASK;
			}
			if (pr->frame_attribute_flags_3 & PARSER_ATT_ESP_OR_IKE_OVER_UDP_MASK) {
				pr->nxt_hdr = 0x4500;
				pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_ESP_OR_IKE_OVER_UDP_MASK;
			}
		}
		if (j == &(pr->routing_hdr_offset1)) {
			/*TODO:pr->nxt_hdr = ???;*/
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_IPV6_ROUTING_HDR_1;
		}
	}
#if 0
	/* check for truncation error:
	 * Frame Parsing reached the limit of the minimum between 
	 * presentation_length and 256 bytes before completing all parsing */
	seg_length = prc->seg_length;
	min = MIN(PARSER_MAX_VALID_OFFSET, (int)seg_length);
	
	if ((pr->nxt_hdr_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->nxt_hdr_offset > min)) {
		fsl_print("TRUNCATION CASE\n");
		pr->parse_error_code = pr->parse_error_code | PARSER_FRAME_TRUNCATION;
		pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_PARSING_ERROR_MASK;
		
		/* fixing offsets backwards, every exceeding offset is set to 0xFF */
		if ((pr->ipv6_frag_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->ipv6_frag_offset > min))
			pr->ipv6_frag_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
		if ((pr->routing_hdr_offset2 != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->routing_hdr_offset2 > min))
			pr->routing_hdr_offset2 = PARSER_UNINITILIZED_FIELD_OF_BYTE;
		if ((pr->routing_hdr_offset1 != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->routing_hdr_offset1 > min)) {
			pr->routing_hdr_offset1 = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_IPV6_ROUTING_HDR_1;
		}
		if ((pr->gtp_esp_ipsec_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->gtp_esp_ipsec_offset > min)) {
			pr->gtp_esp_ipsec_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_GTP_MASK;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 | PARSER_ATT_GTP_PARSING_ERROR_MASK;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_ESP_OR_IKE_OVER_UDP_MASK;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 | PARSER_ATT_ESP_OR_IKE_OVER_UDP_PARSING_ERROR_MASK;
		}
		if ((pr->l4_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->l4_offset > min)) {
			fsl_print("hhhhhhhhhhh\n");
			pr->l4_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_L4_UNKOWN_PROTOCOL_MASK;
			if (l3_protocol == NET_ETH_ETYPE_IPV4) {
				pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_IPV4_1_MASK;
				if (((struct ipv4hdr *)next_header)->protocol == TCP_PROTOCOL) {
					pr->parse_error_code = pr->parse_error_code | PARSER_TCP_PACKET_TRUNCATION;
					pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_TCP_MASK;
					pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 | PARSER_ATT_TCP_PARSING_ERROR_MASK;
				}
				else if (((struct ipv4hdr *)next_header)->protocol == UDP_PROTOCOL) {
					pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_UDP_MASK;
					pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 | PARSER_ATT_UDP_PARSING_ERROR_MASK;
				}
			}
			else if (l3_protocol == NET_ETH_ETYPE_IPV6) {
				pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_IPV6_1_MASK;
				if (((struct ipv6hdr *)next_header)->next_header == TCP_PROTOCOL) {
					pr->parse_error_code = pr->parse_error_code | PARSER_IPV6_PACKET_TRUNCATION;
					pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_TCP_MASK;
					pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 | PARSER_ATT_TCP_PARSING_ERROR_MASK;
				}
				else if (((struct ipv6hdr *)next_header)->next_header == UDP_PROTOCOL) {
					pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_UDP_MASK;
					pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 | PARSER_ATT_UDP_PARSING_ERROR_MASK;
				}
			}
		}
		if ((pr->gre_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->gre_offset > min)) {
			pr->gre_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 & ~PARSER_ATT_GRE_MASK;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 | PARSER_ATT_GRE_PARSING_ERROR_MASK;
		}
		if ((pr->ipn_or_minencapO_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->ipn_or_minencapO_offset > min)) {
			pr->ipn_or_minencapO_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_MIN_ENCAP_MASK;
			pr->frame_attribute_flags_3 = pr->frame_attribute_flags_3 | PARSER_ATT_MIN_ENCAP_PARSING_ERROR_MASK;
			pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_IP_N_OPTIONS_MASK;
			pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 | PARSER_ATT_IP_N_PARSING_ERROR_MASK;
		}
		if ((pr->ip1_or_arp_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->ip1_or_arp_offset > min)) {
			pr->ip1_or_arp_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 & ~PARSER_ATT_IP_1_OPTIONS_MASK;
			pr->frame_attribute_flags_2 = pr->frame_attribute_flags_2 | PARSER_ATT_IP_1_PARSING_ERROR_MASK;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_ARP_MASK;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_ARP_PARSING_ERROR_MASK;
		}
		if ((pr->mpls_offset_n != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->mpls_offset_n > min)) {
			pr->mpls_offset_n = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_MPLS_N_MASK;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_MPLS_PARSING_ERROR_MASK;
		}
		if ((pr->mpls_offset_1 != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->mpls_offset_1 > min)) {
			pr->mpls_offset_1 = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_MPLS_1_MASK;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_MPLS_PARSING_ERROR_MASK;
		}
		if ((pr->pppoe_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->pppoe_offset > min)) {
			pr->pppoe_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->parse_error_code = pr->parse_error_code | PARSER_PPPOE_TRUNCATION;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_PPPOE_PPP_MASK;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_PPPOE_PPP_PARSING_ERROR_MASK;
		}
		if ((pr->vlan_tcin_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->vlan_tcin_offset > min)) {
			pr->vlan_tcin_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_VLAN_N_MASK;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_VLAN_PARSING_ERROR_MASK;
		}
		if ((pr->vlan_tci1_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->vlan_tci1_offset > min)) {
			pr->vlan_tci1_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_VLAN_1_MASK;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_VLAN_PARSING_ERROR_MASK;
		}
		if ((pr->llc_snap_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->llc_snap_offset > min)) {
			pr->llc_snap_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->parse_error_code = pr->parse_error_code | PARSER_ETH_802_3_TRUNCATION;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_LLC_SNAP_MASK;
			//fsl_print("frame_attribute_flags_1:%x\n", pr->frame_attribute_flags_1);
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_LLC_SNAP_PARSING_ERROR_MASK;
			//fsl_print("frame_attribute_flags_1:%x\n", pr->frame_attribute_flags_1);
		}
		if ((pr->eth_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->eth_offset > min)) {
			pr->eth_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 & ~PARSER_ATT_ETH_MAC_MASK;
			pr->frame_attribute_flags_1 = pr->frame_attribute_flags_1 | PARSER_ATT_ETH_PARSING_ERROR_MASK;
		}
		if ((pr->ip_pid_offset != PARSER_UNINITILIZED_FIELD_OF_BYTE) && (pr->ip_pid_offset > min)) {
			pr->ip_pid_offset = PARSER_UNINITILIZED_FIELD_OF_BYTE;
			if (l3_protocol == NET_ETH_ETYPE_IPV4)
				pr->parse_error_code = pr->parse_error_code | PARSER_IPV4_PACKET_TRUNCATION;
			if (l3_protocol == NET_ETH_ETYPE_IPV6)
				pr->parse_error_code = pr->parse_error_code | PARSER_IPV6_PACKET_TRUNCATION;
		}
	}
#endif
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
	if (status == PARSER_HW_STATUS_SUCCESS) {
		*l3_checksum = *((uint16_t *)HWC_ACC_OUT_ADDRESS2);
		*l4_checksum = *((uint16_t *)(HWC_ACC_OUT_ADDRESS2+2));
		return 0;
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
