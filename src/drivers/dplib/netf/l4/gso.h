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
 @File          gso.h

 @Description   This file contains the AIOP SW TCP GSO Internal API
*//***************************************************************************/
#ifndef __GSO_H
#define __GSO_H

#include "dplib/fsl_gso.h"
#include "fsl_gen.h"


/** \addtogroup FSL_AIOP_GSO
 *  @{
 */

/**************************************************************************//**
@Group		FSL_AIOP_TCP_GSO_INTERNAL FSL AIOP TCP GSO Internal

@Description	FSL AIOP TCP GSO Internal

@{
*//***************************************************************************/


/**************************************************************************//**
@Group		TCP_GSO_INTERNAL_STRUCTS TCP GSO Internal Structures

@Description	AIOP TCP GSO Internal Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GSO Context Internally used by GSO functions.
*//***************************************************************************/
struct tcp_gso_context {
	/** Remaining packet FD  */
	struct ldpaa_fd rem_fd
		__attribute__((aligned(sizeof(struct ldpaa_fd))));
	/** Flags - Please refer to \ref TCP_GSO_FLAGS */
	uint32_t flags;
	/** Internal TCP GSO flags - Please refer to
	 * \ref TCP_GSO_INTERNAL_FLAGS */
	uint32_t internal_flags;
	/** MSS */
	uint16_t mss;
	/** Split Size. */
	uint16_t split_size;
	/** Headers Size. */
	uint16_t headers_size;
	/** Urgent Pointer offset. */
	uint16_t urgent_pointer;
	/** Task default Starting HXS for Parser */
	uint16_t parser_starting_hxs;
	/** default segment workspace address for split command parameters */
	uint16_t seg_address;
	/** default segment length for split command parameters */
	uint16_t seg_length;
	/** default segment offset for split command parameters */
	uint16_t seg_offset;
	/** Task default Parser Profile ID */
	uint8_t parser_profile_id;
	/** Remaining frame handle. */
	uint8_t	rem_frame_handle;
	/** First Segment indication */
	uint8_t	first_seg;
	/** IP offset */
	uint8_t	ip_offset;
	/** TCP offset */
	uint8_t	tcp_offset;
	/** IPv4 */
	uint8_t	ipv4;
	/** Padding */
	uint8_t	pad[2];
};

/** @} */ /* end of TCP_GSO_INTERNAL_STRUCTS */


/**************************************************************************//**
@Group	TCP_GSO_INTERNAL_MACROS TCP GSO Internal Macros

@Description	TCP GSO Internal Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GSO_GENERAL_INT_DEFINITIONS TCP GSO General Internal Definitions

 @Description TCP GSO General Internal Definitions.

 @{
*//***************************************************************************/

	/** Size of GSO Context. */
#define SIZEOF_GSO_CONTEXT	(sizeof(struct tcp_gso_context))
	/* GSO internal struct size assertion check */
#pragma warning_errors on
ASSERT_STRUCT_SIZE(SIZEOF_GSO_CONTEXT, TCP_GSO_CONTEXT_SIZE);
#pragma warning_errors off

/** @} */ /* end of TCP_GSO_GENERAL_INT_DEFINITIONS */

/**************************************************************************//**
 @Group	TCP_GSO_INTERNAL_FLAGS TCP GSO Internal Flags

 @Description TCP GSO Internal Flags.

 @{
*//***************************************************************************/

	/** If set, the FIN flag in the TCP header of the GSO aggregation is
	 * set. */
#define TCP_GSO_FIN_BIT		0x00000001
	/** If set, the PSH flag in the TCP header of the GSO aggregation is
	 * set. */
#define TCP_GSO_PSH_BIT		0x00000008

/** @} */ /* end of TCP_GSO_INTERNAL_FLAGS */

/**************************************************************************//**
 @Group	TCP_GSO_INTERNAL_DEFINITIONS TCP GSO Internal Definitions

 @Description TCP GSO Internal Definitions.

 @{
*//***************************************************************************/

	/** Size of modification in IP header of the source frame in the first
	 * iteration. */
#define TCP_GSO_IP_MODIFICATION_SIZE	12


/** @} */ /* end of TCP_GSO_INTERNAL_DEFINITIONS */

/** @} */ /* end of TCP_GSO_INTERNAL_MACROS */

/**************************************************************************//**
 @Group	TCP_GSO_INTERNAL_FUNCTIONS TCP GSO Internal Functions

 @Description TCP GSO Internal Functions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Function	tcp_gso_return_frame_remainder_as_default_frame

@Description	This function relocate the remainder packet being segmented to
		the default frame location.
		This function should be called internally for debug purposes
		only.

@Param[in]	tcp_gso_context_addr - Address to the TCP GSO internal context.

@Return		Status of the operation (\ref FDMA_DISCARD_FRAME_ERRORS).

@Cautions	None.
*//***************************************************************************/
int32_t tcp_gso_return_frame_remainder_as_default_frame(
		tcp_gso_ctx_t tcp_gso_context_addr);

/**************************************************************************//**
@Function	tcp_gso_split_segment

@Description	This function generates a single TCP segment and locates it in
		the default frame location in the workspace.

		The remaining source frame is kept in the internal GSO
		structure.

@Param[in]	tcp_gso_context_addr - Address to the TCP GSO internal context.

@Return		Status of the operation (\ref FDMA_DISCARD_FRAME_ERRORS).

@Cautions	None.
*//***************************************************************************/
int32_t tcp_gso_split_segment(
		struct tcp_gso_context *gso_ctx);

/** @} */ /* end of TCP_GSO_INTERNAL_FUNCTIONS */

/** @} */ /* end of FSL_AIOP_TCP_GSO_INTERNAL */

/** @} */ /* end of FSL_AIOP_GSO */

#endif /* __GSO_H */
