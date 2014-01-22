/**************************************************************************//**
@File		ipf.c

@Description	This file contains the AIOP SW IP Fragmentation implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "general.h"
#include "net/fsl_net.h"
#include "dplib/fsl_parser.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_ldpaa.h"
#include "dplib/fsl_ipv4_checksum.h"
#include "fsl_spinlock.h"
#include "ipf.h"
#include "fdma.h"

extern __TASK struct aiop_default_task_params default_task_params;

/*uint64_t lfsr64;
uint64_t lfsr_array64[16];
uint32_t lfsr32;
uint32_t lfsr_array32[16];
uint8_t id_lock_byte;*/

/* Remove these !!! */

/*int32_t get_id_mutex(uint64_t *id); 
uint64_t get_id_per_core(void);
uint32_t get_ipv6_id(void);
uint32_t aiop_get_32bit_id();
uint64_t aiop_get_64bit_id();*/


int32_t ipf_move_remaining_frame(struct ipf_context *ipf_ctx)
{
	int32_t	status;
	struct fdma_present_frame_params present_rem_frame_params;

	status = fdma_store_default_frame_data();
	if (status) 
		return status; /* TODO */
	/* Copy default FD to remaining_FD in IPF ctx */
	ipf_ctx->rem_fd = *((struct ldpaa_fd*)HWC_FD_ADDRESS);
	
	/* Present the remaining FD */
	present_rem_frame_params.flags = FDMA_INIT_NDS_BIT;
	present_rem_frame_params.asa_size = 0;
	present_rem_frame_params.fd_src = &(ipf_ctx->rem_fd);
	present_rem_frame_params.pta_dst = (void *)PRC_PTA_NOT_LOADED_ADDRESS;
	status = fdma_present_frame(&present_rem_frame_params);
	ipf_ctx->rem_frame_handle = present_rem_frame_params.frame_handle;
	
	return status;
}

int32_t ipf_insert_ipv6_frag_header(struct ipf_context *ipf_ctx,
						uint16_t frag_hdr_offset){
	
	int32_t	status;
	uint16_t next_header_offset;
	uint8_t *next_header;
	uint8_t orig_next_header;
	struct ipv6_fragment_header frag_ext_header;

	/* Keep the last "next header" of the unfragmentable part */
	next_header_offset = PARSER_GET_SHIM1_OFFSET_DEFAULT(); /* TODO */
	next_header = (uint8_t*)(next_header_offset + 
			PRC_GET_SEGMENT_ADDRESS()); /* TODO soft parser? */
	orig_next_header = *next_header;
	/* Replace the last "next header" of the unfragmentable part with 44 */
	*(next_header) = 44;
	status = fdma_modify_default_segment_data(next_header_offset,1);
	if (status) /* TODO */
		return status;
	if(ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS){ 
		/* TODO Get IPv6 fragment header from parameters buffer */
	} else {
		/* Build IPv6 fragment header */
		frag_ext_header.next_header = orig_next_header;
		frag_ext_header.reserved = 0;
		frag_ext_header.fragment_offset_flags = IPV6_HDR_M_FLAG_MASK;
	//	frag_ext_header.id = aiop_get_64bit_id(); /*TODO */

		/* Insert to header */
		status = fdma_insert_default_segment_data(frag_hdr_offset,
						&frag_ext_header,
						IPV6_FRAGMENT_HEADER_LENGTH,
						FDMA_REPLACE_SA_REPRESENT_BIT);
		if (status) /* TODO */
			return status;
	}
	/* Run parser */
	parse_result_generate_default(PARSER_NO_FLAGS);
	ipf_ctx->first_frag = 0;
	return status;
}

inline int32_t ipf_after_split_ipv4_fragment(struct ipf_context *ipf_ctx){

	int32_t	status;
	struct fdma_present_segment_params present_segment_params;
	struct fdma_insert_segment_data_params insert_segment_data_params;
	struct ipv4hdr *ipv4_hdr;
	uint16_t header_length, frag_offset, ipv4_offset;
	uint8_t first_frag;
	
	first_frag = ipf_ctx->first_frag;
	if (first_frag) {
		ipv4_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ipv4_hdr = (struct ipv4hdr *)
				(ipv4_offset + PRC_GET_SEGMENT_ADDRESS());
		frag_offset = (ipv4_hdr->flags_and_offset &
						IPV4_HDR_FRAG_OFFSET_MASK);	
		ipv4_hdr->total_length = 
		(uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) - ipv4_offset;
	} else {
	/* Not first fragment */
		/* Run parser */
		parse_result_generate_default(PARSER_NO_FLAGS);
		ipv4_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		ipv4_hdr = (struct ipv4hdr *)
				(ipv4_offset + PRC_GET_SEGMENT_ADDRESS());
		frag_offset = ipf_ctx->ipf_params.mtu_params.prev_frag_offset +
			ipf_ctx->ipf_params.mtu_params.mtu_payload_length>>3;
	}
	/* Update frag offset, M flag=1, checksum, length */
	ipv4_hdr->flags_and_offset = frag_offset | IPV4_HDR_M_FLAG_MASK; 
	status = ipv4_cksum_calculate(ipv4_hdr);
	if (status)
		return status; /* TODO*/
	/* Modify 12 first header fields in FDMA */
	status = fdma_modify_default_segment_data(ipv4_offset,12);
	if (status)
		return status; /* TODO*/
	ipf_ctx->ipf_params.mtu_params.prev_frag_offset = frag_offset;
	
	present_segment_params.flags = FDMA_PRES_NO_FLAGS;
	present_segment_params.frame_handle = ipf_ctx->rem_frame_handle;
	present_segment_params.offset = 0;
	present_segment_params.present_size = 0;
	/* present empty segment of the remaining frame */
	status = fdma_present_frame_segment(&present_segment_params);
	if (status)
		return status; /* TODO*/

	if (first_frag) {
		/* TODO Handle options */
		ipf_ctx->first_frag = 0;
	}
	header_length = 
		(uint16_t)(ipv4_hdr->vsn_and_ihl & IPV4_HDR_IHL_MASK) << 2;
	
	insert_segment_data_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
	insert_segment_data_params.frame_handle = ipf_ctx->rem_frame_handle;
	insert_segment_data_params.insert_size = header_length;
	insert_segment_data_params.to_offset = 0;
	insert_segment_data_params.from_ws_src = ipv4_hdr;
	insert_segment_data_params.seg_handle = 
		present_segment_params.seg_handle;
	/* Insert the header to the remaining frame,
	 * close segment */
	status = fdma_insert_segment_data(&insert_segment_data_params);
	return status;
}

inline int32_t ipf_after_split_ipv6_fragment(struct ipf_context *ipf_ctx){
	
	int32_t	status;
	struct fdma_present_segment_params present_segment_params;
	struct fdma_insert_segment_data_params insert_segment_data_params;
	struct ipv6hdr *ipv6_hdr;
	struct ipv6_fragment_header *ipv6_frag_hdr; 
	uint16_t header_length, frag_offset;
	uint16_t ipv6_offset, frag_hdr_offset;
	uint8_t first_frag;
	
	first_frag = ipf_ctx->first_frag;
	
	if (first_frag) {
		ipv6_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		frag_hdr_offset =
			(uint16_t)PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
		ipv6_hdr = (struct ipv6hdr *)
				(ipv6_offset + PRC_GET_SEGMENT_ADDRESS());
		ipv6_hdr->payload_length =
		(uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) - ipv6_offset +
				IPV6_FRAGMENT_HEADER_LENGTH;
		/* Modify payload length field in FDMA */
		status = fdma_modify_default_segment_data(ipv6_offset+4,2);
		if (status)
			return status; /* TODO */
		status = ipf_insert_ipv6_frag_header(ipf_ctx, frag_hdr_offset);
		if (status)
			return status; /* TODO */
		ipf_ctx->ipf_params.mtu_params.split_size +=
				IPV6_FRAGMENT_HEADER_LENGTH;
	} else {
	/* Not first fragment */
		/* Run parser */
		parse_result_generate_default(PARSER_NO_FLAGS);
		ipv6_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		frag_hdr_offset =
			(uint16_t)PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
		ipv6_hdr = (struct ipv6hdr *)
				(ipv6_offset + PRC_GET_SEGMENT_ADDRESS());
		frag_offset = ipf_ctx->ipf_params.mtu_params.prev_frag_offset +
			ipf_ctx->ipf_params.mtu_params.mtu_payload_length>>3;
		ipv6_frag_hdr = (struct ipv6_fragment_header *)
				(frag_hdr_offset + PRC_GET_SEGMENT_ADDRESS());
		ipv6_frag_hdr->fragment_offset_flags =
				frag_offset <<3 | IPV6_HDR_M_FLAG_MASK; 
		/* Modify fragment header fields in FDMA */
		status = fdma_modify_default_segment_data(frag_hdr_offset+2,2);	
		ipf_ctx->ipf_params.mtu_params.prev_frag_offset = frag_offset;
		
		present_segment_params.flags = FDMA_PRES_NO_FLAGS;
		present_segment_params.frame_handle = ipf_ctx->rem_frame_handle;
		present_segment_params.offset = 0;
		present_segment_params.present_size = 0;
		/* present empty segment of the remaining frame */
		status = fdma_present_frame_segment(&present_segment_params);
		if (status)
			return status; /* TODO*/
		header_length = frag_hdr_offset - ipv6_offset +
				IPV6_FRAGMENT_HEADER_LENGTH;
		
		insert_segment_data_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
		insert_segment_data_params.frame_handle =
				ipf_ctx->rem_frame_handle;
		insert_segment_data_params.insert_size = header_length;
		insert_segment_data_params.to_offset = 0;
		insert_segment_data_params.from_ws_src = ipv6_hdr;
		insert_segment_data_params.seg_handle = 
			present_segment_params.seg_handle;
		/* Insert the header to the remaining frame, close segment */
		status = fdma_insert_segment_data(&insert_segment_data_params);
	}
	return status;
}


int32_t ipf_split_fragment(struct ipf_context *ipf_ctx)
{
	int32_t	status;
	struct fdma_split_frame_params split_frame_params;
	struct fdma_isolation_attributes isolation_attributes; 
	struct ipv4hdr *ipv4_hdr;
	struct ipv6hdr *ipv6_hdr;
	struct ipv6_fragment_header *ipv6_frag_hdr; 
	uint16_t remaining_payload_length, mtu_payload_length;
	uint16_t ipv6_offset, frag_hdr_offset;
	uint16_t frag_offset, ipv4_offset;
	uint8_t spid, ipv4;
	
	ipv4 = ipf_ctx->ipv4;
	remaining_payload_length = ipf_ctx->remaining_payload_length;
	mtu_payload_length = ipf_ctx->ipf_params.mtu_params.mtu_payload_length;
	if(remaining_payload_length > mtu_payload_length) {
	/* Not last fragment, need to split */
		ipf_ctx->remaining_payload_length = remaining_payload_length -
						mtu_payload_length;
		
		split_frame_params.flags = FDMA_CFA_COPY_BIT | 
					FDMA_SPLIT_PSA_PRESENT_BIT;
		split_frame_params.fd_dst = (void *)HWC_FD_ADDRESS;
		split_frame_params.seg_dst = (void *)PRC_GET_SEGMENT_ADDRESS();
		split_frame_params.seg_offset = PRC_GET_SEGMENT_OFFSET();
		split_frame_params.present_size = PRC_GET_SEGMENT_LENGTH();
		split_frame_params.split_size_sf = ipf_ctx->ipf_params.
							mtu_params.split_size;
		split_frame_params.source_frame_handle =
						ipf_ctx->rem_frame_handle;
		split_frame_params.spid = *((uint8_t *)HWC_SPID_ADDRESS);

		/* Split remaining frame, put split frame in default FD 
		 * location*/
		status = fdma_split_frame(&split_frame_params);
		if (status)
			return status; /* TODO*/ 

		if (ipv4) {
			status = ipf_after_split_ipv4_fragment(ipf_ctx);
			if (status)
				return status; /* TODO*/
		} else { 
			status = ipf_after_split_ipv6_fragment(ipf_ctx);
			if (status)
				return status; /* TODO*/
		}
	} else {
		/* Last Fragment */
		spid = *((uint8_t *)HWC_SPID_ADDRESS);
		status = fdma_store_frame_data(ipf_ctx->rem_frame_handle, spid,
			 &isolation_attributes);
		/* Copy remaining_FD to default FD */
		*((struct ldpaa_fd*)HWC_FD_ADDRESS) = ipf_ctx->rem_fd;
		/* present fragment + header segment */
		status = fdma_present_default_frame();
		/* Run parser */
		parse_result_generate_default(PARSER_NO_FLAGS);
	
		frag_offset = ipf_ctx->ipf_params.mtu_params.
				prev_frag_offset +
				(ipf_ctx->ipf_params.mtu_params.
						mtu_payload_length >>3);
		if (ipv4) {
			ipv4_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
			ipv4_hdr = (struct ipv4hdr *)
				(ipv4_offset + PRC_GET_SEGMENT_ADDRESS());
			/* Updating frag offset, M flag=0, checksum, length */
			ipv4_hdr->flags_and_offset = frag_offset;
			ipv4_hdr->total_length = (uint16_t)
					LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) - 
					ipv4_offset;
			status = ipv4_cksum_calculate(ipv4_hdr);
			if (status)
				return status; /* TODO */
			/* Modify 12 first header fields in FDMA */
			status = fdma_modify_default_segment_data(
					(uint16_t)ipv4_offset,12);	
			return status;	
		} else {
			/* IPv6 */
			ipv6_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
			ipv6_hdr = (struct ipv6hdr *)
				(ipv6_offset + PRC_GET_SEGMENT_ADDRESS());
			/* Update frag offset, M flag=0 */
			frag_hdr_offset = (uint16_t)
			PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
			ipv6_frag_hdr = (struct ipv6_fragment_header *)
				(frag_hdr_offset + PRC_GET_SEGMENT_ADDRESS());
			ipv6_frag_hdr->fragment_offset_flags =
						frag_offset <<3;
			/* Modify fragment header fields in FDMA */
			status = fdma_modify_default_segment_data
					(frag_hdr_offset+2,2);	
			/* Update payload length in ipv6 header */
			ipv6_hdr->payload_length = (uint16_t)
					LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) - 
					ipv6_offset;
			/* Modify header fields in FDMA */
			status = fdma_modify_default_segment_data(
					(uint16_t)ipv6_offset+4,2);	
		} 
	}
	return status;
}
	
int32_t ipf_restore_orig_fragment(struct ipf_context *ipf_ctx){
	return (int32_t)ipf_ctx->pad;
}

int32_t ipf_generate_frag(ipf_ctx_t ipf_context_addr)
{
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;

	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	int32_t	status;
	uint16_t ip_header_length, mtu_payload_length, split_size;
	struct ipv4hdr *ipv4_hdr;
	struct ipv6hdr *ipv6_hdr;	
	uint8_t first_frag = ipf_ctx->first_frag; /* first fragment flag */
//	struct params_for_restoration restore_params;

		
	if (first_frag){
		/* First Fragment */
		/* Keep parser's parameters from task defaults */
		ipf_ctx->parser_profile_id =
				default_task_params.parser_profile_id;
		ipf_ctx->parser_starting_hxs =
				default_task_params.parser_starting_hxs;
		/* Keep PRC parameters */
		ipf_ctx->prc_seg_address = PRC_GET_SEGMENT_ADDRESS ();
		ipf_ctx->prc_seg_length = PRC_GET_SEGMENT_LENGTH ();
		
		if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS){
			/* Restore original fragments */
			if (IPF_SFV_QUERY()){
				status = ipf_move_remaining_frame(ipf_ctx);
				if (status)
					return status;
				/* Clear gross running sum in parse results */
				pr->gross_running_sum = 0;
				status = ipf_restore_orig_fragment(ipf_ctx);
				return status; /* TODO */
			} else {
				/* SFV bit is clear */
				return IPF_GEN_FRAG_STATUS_SFV_CLEAR;
			}
		} else {
			/* First Fragment, Split according to MTU */
			if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
				/* IPv4 */
				ipf_ctx->ipv4 = 1;
				ipv4_hdr = (struct ipv4hdr *)
					PARSER_GET_OUTER_IP_POINTER_DEFAULT();
				if (ipv4_hdr->flags_and_offset &
						IPV4_HDR_D_FLAG_MASK)
					return IPF_GEN_FRAG_STATUS_DF_SET;
				ip_header_length = (uint16_t)
				(ipv4_hdr->vsn_and_ihl & IPV4_HDR_IHL_MASK) <<2;
				mtu_payload_length = 
					(ipf_ctx->ipf_params.mtu_params.mtu -
						ip_header_length) & ~0x7;
				ipf_ctx->ipf_params.mtu_params.
				mtu_payload_length = mtu_payload_length;
				split_size = mtu_payload_length +
						ip_header_length +
						(uint16_t)
					PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
				ipf_ctx->ipf_params.mtu_params.split_size = 
								split_size;
				ipf_ctx->remaining_payload_length =
						ipv4_hdr->total_length -
						ip_header_length;
				status = ipf_move_remaining_frame(ipf_ctx);
				if (status)
					return status; /* TODO */
				/* Clear gross running sum in parse results */
				pr->gross_running_sum = 0;
				status = ipf_split_fragment(ipf_ctx);
				return status; /* TODO */
			} else {
				/* IPv6 */
				//ipf_ctx->ipv4 = 0;
				ipv6_hdr = (struct ipv6hdr *)
					PARSER_GET_OUTER_IP_POINTER_DEFAULT();
				ip_header_length = (uint16_t)
				(PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT() -
				PARSER_GET_OUTER_IP_OFFSET_DEFAULT() +
				IPV6_FRAGMENT_HEADER_LENGTH);
				mtu_payload_length =
					(ipf_ctx->ipf_params.mtu_params.mtu -
					ip_header_length) & ~0x7;
				ipf_ctx->ipf_params.mtu_params.
				mtu_payload_length = mtu_payload_length;
				split_size = mtu_payload_length +
						(uint16_t)
				PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
				ipf_ctx->ipf_params.mtu_params.split_size = 
								split_size;
				ipf_ctx->remaining_payload_length =
						ipv6_hdr->payload_length -
						ip_header_length -
						IPV6_FRAGMENT_HEADER_LENGTH;
				status = ipf_move_remaining_frame(ipf_ctx);
				if (status)
					return status; /* TODO */
				/* Clear gross running sum in parse results */
				pr->gross_running_sum = 0;
				status = ipf_split_fragment(ipf_ctx);
				return status; /* TODO */
			}
		}
	} else {
		/* Not first Fragment */
		/* Restore original parser's parameters in task default */
		default_task_params.parser_profile_id = 
					ipf_ctx->parser_profile_id;
		default_task_params.parser_starting_hxs =
					ipf_ctx->parser_starting_hxs;
		/* Restore original PRC parameters */
		PRC_SET_SEGMENT_ADDRESS(ipf_ctx->prc_seg_address);
		PRC_SET_SEGMENT_LENGTH(ipf_ctx->prc_seg_length);

		/* Clear gross running sum in parse results */
		pr->gross_running_sum = 0;

		if (ipf_ctx->flags & IPF_RESTORE_ORIGINAL_FRAGMENTS)
			status = ipf_restore_orig_fragment(ipf_ctx);
		else
			status = ipf_split_fragment(ipf_ctx);
		return status;
	} 
}
	
int32_t ipf_discard_frame_remainder(ipf_ctx_t ipf_context_addr)
{
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;
	return fdma_discard_frame(
			ipf_ctx->rem_frame_handle, FDMA_DIS_NO_FLAGS);
}

void ipf_context_init(uint32_t flags, uint16_t mtu, ipf_ctx_t ipf_context_addr)
{
	struct ipf_context *ipf_ctx = (struct ipf_context *)ipf_context_addr;
	ipf_ctx->first_frag = 1;
	ipf_ctx->flags = flags;
	ipf_ctx->ipf_params.mtu_params.mtu = mtu;
	ipf_ctx->ipv4 = 0;
	ipf_ctx->frag_index = 0;
	ipf_ctx->ipf_params.mtu_params.prev_frag_offset = 0;
}


/*
int32_t get_id_mutex(uint64_t *id){
	int32_t status;
	status = cdma_mutex_lock_take((uint64_t)&lfsr64,CDMA_MUTEX_WRITE_LOCK);
	if (status)
		return status;
	//lfsr32 = (lfsr32 >> 1) ^ (-(lfsr32 & 1u) & 0xD0000001u);
	 lfsr64=(lfsr64>>1) ^ (-(lfsr64 & 1LL) & 0x9d206b8e1036b29fLL);
        *id = lfsr64;
        status = cdma_mutex_lock_release((uint64_t)&lfsr64);
	return status;
}


uint64_t get_id_per_core(void){
	uint8_t core_id;
	uint64_t my_id;
	
	core_id = 5;
	my_id = (uint64_t)lfsr_array64[core_id];
	//lfsr32 = (lfsr32 >> 1) ^ (-(lfsr32 & 1u) & 0xD0000001u);
	lfsr64=(lfsr64>>1) ^ (-(lfsr64 & 1LL) & 0x9d206b8e1036b29fLL);
        lfsr_array64[core_id] = my_id;
	return my_id;
}


inline void my_lock_spinlock(register uint8_t *spinlock)
{
	register uint8_t temp1;
	register uint8_t temp2;
	asm{
 Address is in r3, new value (non zero) in r4 and old in r5 
	li	temp1, 1		 
spinlock_loop :
	lbarx	temp2, 0, spinlock	  
	cmpwi	temp2, 0		  
	bne -	spinlock_loop	  
	stbcx.	temp1, 0, spinlock	 
	bne -	spinlock_loop		
	}
}

inline void my_unlock_spinlock(uint8_t *spinlock) {
		*spinlock = 0;
	}


uint64_t aiop_get_64bit_id()
{	
	spinlock_loop:
	my_lock_spinlock(&id_lock_byte);
	lfsr64=(lfsr64>>1) ^ (-(lfsr64 & 1LL) & 0x9d206b8e1036b29fLL);
	my_unlock_spinlock(&id_lock_byte);
	 asm
	{
		bne-	spinlock_loop
	}
	 return lfsr64;
	}
	

uint32_t aiop_get_32bit_id()
{
	uint32_t temp1, temp2;
	
	spinlock_loop:
	__lwarx(temp1 , 0, &lfsr32)
	temp2 = (temp1 >> 1) ^ (-(temp1 & 1u) & 0xD0000001u);
	__stwcx(temp2, 0, &lfsr32)
	 asm
	{
		bne-	spinlock_loop
	}
	 return lfsr32;
	}


uint32_t get_ipv6_id(void){
	 lfsr32=(lfsr32>>1) ^ (-(lfsr32 & 1LL) & 0xD0000001u);
	return (uint32_t)lfsr32;
}
*/

