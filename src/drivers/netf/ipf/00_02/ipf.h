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
 @File          ipf.h

 @Description   This file contains the AIOP SW IP Fragmentation Internal API
*//***************************************************************************/
#ifndef __IPF_H
#define __IPF_H

#include "fsl_ipf.h"

/**************************************************************************//**
@Group		AIOP_IPF_INTERNAL  AIOP IPF Internal

@Description	AIOP IPF Internal

@{
*//***************************************************************************/


/**************************************************************************//**
@Group		IPF_INTERNAL_STRUCTS IPF Internal Structures

@Description	AIOP IPF Internal Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPF Context Internally used by IPF functions.
*//***************************************************************************/
struct ipf_context {
	 /** Remaining frame's FD  */
	struct ldpaa_fd rem_fd;
	/** Frame ID. Used for IPv6 fragmentation extension in case of
	 * fragmentation according to MTU.
	 * In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
	 * parameter is ignored and the ID is inherited from the original
	 * fragments. */
	uint32_t frame_id;
	/** Flags - Please refer to \ref IPF_FLAGS */
	uint32_t flags;
	/** Original Starting HXS for Parser from Task default */
	uint16_t parser_starting_hxs;
	/** Original segment address from PRC */
	uint16_t prc_seg_address;
	/** Original segment length from PRC */
	uint16_t prc_seg_length;
	/** default segment offset for split command parameters */
	uint16_t prc_seg_offset;
	/** Remaining payload length (for split by MTU) */
	uint16_t remaining_payload_length;
	/** MTU payload length (for split by MTU) */
	uint16_t mtu_payload_length;
	/** Split size (for split by MTU) */
	uint16_t split_size;
	/** Previous Fragment Offset */
	uint16_t prev_frag_offset;
	/** Original Parser Profile ID from Task default */
	uint8_t parser_profile_id;
	/** Remaining frame handle */
	uint8_t rem_frame_handle;
	/** First fragment indication */
	uint8_t	first_frag;
	/** IPv4 indication */
	uint8_t ipv4;
	/** IP offset */
	uint8_t ip_offset;
	/** IPv6 Fragment header offset.
	 * Cannot exceed 256 bytes due to parser limitation. */
	uint8_t ipv6_frag_hdr_offset;
	/** Padding */
	uint8_t	pad[2];
};

/** @} */ /* end of IPF_INTERNAL_STRUCTS */


/**************************************************************************//**
@Group	IPF_INTERNAL_MACROS IPF Internal Macros

@Description	IPF Internal Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	IPF_GENERAL_INT_DEFINITIONS IPF General Internal Definitions

 @Description IPF General Internal Definitions.

 @{
*//***************************************************************************/

/** Size of IPF Context. */
#define SIZEOF_IPF_CONTEXT	(sizeof(struct ipf_context))

/** Fragmentation of fragment indication. */
#define	FRAGMENTATION_OF_FRAG	0x80000000

#pragma warning_errors on
/** IPF internal struct size assertion check. */
ASSERT_STRUCT_SIZE(SIZEOF_IPF_CONTEXT, IPF_CONTEXT_SIZE);
#pragma warning_errors off

/** @} */ /* end of IPF_GENERAL_INT_DEFINITIONS */

/** @} */ /* end of IPF_INTERNAL_MACROS */


/*inline*/ int ipf_restore_orig_fragment(struct ipf_context *ipf_ctx);
/*inline */void ipf_after_split_ipv6_fragment(struct ipf_context *ipf_ctx,
						uint32_t last_ext_hdr_size);
/*inline*/ void ipf_after_split_ipv4_fragment(struct ipf_context *ipf_ctx);
/*inline*/ int ipf_split_ipv4_fragment(struct ipf_context *ipf_ctx);
/*inline*/ int ipf_split_ipv6_fragment(struct ipf_context *ipf_ctx,
						uint32_t last_ext_hdr_size);
/*inline*/ int ipf_move_remaining_frame(struct ipf_context *ipf_ctx);
/*inline*/int ipf_ipv4_last_frag(struct ipf_context *ipf_ctx);
/*inline*/int ipf_ipv6_last_frag(struct ipf_context *ipf_ctx);


/** @} */ /* end of AIOP_IPF_INTERNAL */

#endif /* __IPF_H */
