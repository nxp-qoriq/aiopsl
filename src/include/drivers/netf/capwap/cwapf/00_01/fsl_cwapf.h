/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
@File          fsl_cwapf.h

@Description   This file contains the AIOP SW CAPWAP Fragmentation API

*//***************************************************************************/

#ifndef __FSL_CWAPF_H
#define __FSL_CWAPF_H

#include "fsl_types.h"

/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		FSL_CWAPF CAPWAP Fragmentation

@Description	Freescale AIOP CAPWAP Fragmentation

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	CWAPF_MACROS CAPWAP Fragmentation Macros
@{
*//***************************************************************************/
/**************************************************************************//**
 @Group	CWAPF_GENERAL_DEFINITIONS CAPWAP Fragmentation General Definitions
 @{
*//***************************************************************************/

	/** CWAPF context size definition. */
#define CWAPF_CONTEXT_SIZE	64
	/** CWAPF context definition. Must be aligned to 32 Bytes.*/
typedef uint8_t cwapf_ctx_t[CWAPF_CONTEXT_SIZE]
			__attribute__((aligned(sizeof(struct ldpaa_fd))));


/** @} */ /* end of CWAPF_GENERAL_DEFINITIONS */


/**************************************************************************//**
 @Group	CWAPF_PARSER_HELPERS CAPWAP Fragmentation parser helper macros

 @Description	These macros extend parser capabilities to parse CAPWAP frames

 @{
*//***************************************************************************/

#define CWAPF_GET_CAPWAP_HDR_OFFSET() \
	 ((PARSER_IS_UDP_DEFAULT()) ? \
		PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() : \
		0)

#define CWAPF_GET_CAPWAP_HDR_LENGTH(capwap_hdr) \
		(((capwap_hdr->hlen_rid_wbid_t & \
					NET_HDR_FLD_CAPWAP_HLEN_MASK) >> \
					NET_HDR_FLD_CAPWAP_HLEN_OFFSET) << 2)

/** @} */ /* end of CWAPF_PARSER_HELPERS */

/**************************************************************************//**
@Group	CWAPF_GENERATE_FRAG_STATUS  CAPWAP Fragmentation Return Status

@Description cwapf_generate_frag() return values

@{
*//***************************************************************************/
/** Fragmentation process complete. The last fragment was generated */
#define	CWAPF_GEN_FRAG_STATUS_DONE		SUCCESS
/** Fragmentation process didn't complete.
 * Fragment was generated and the user SHOULD call generate_frag()
 * again to generate another fragment*/
#define	CWAPF_GEN_FRAG_STATUS_IN_PROCESS	(CWAPF_MODULE_STATUS_ID | 0x1)
/** Fragmentation was attempted on a non-CAPWAP frame */
#define CWAPF_GEN_FRAG_STATUS_NOT_CAPWAP		(CWAPF_MODULE_STATUS_ID | 0x2)

/** @} */ /* end of CWAPF_GENERATE_FRAG_STATUS */

/** @} */ /* end of CWAPF_MACROS */


/**************************************************************************//**
@Group		CWAPF_Functions CAPWAP Fragmentation Functions

@Description	AIOP CAPWAP Fragmentation Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	cwapf_generate_frag

@Description	This function generates a single CAPWAP fragment and locates it
		in the default frame location in the workspace.

		Pre-condition - In the first iteration this function is called
		for a source packet, the source packet should be located
		at the default frame location in workspace.

		The remaining source frame is kept in the internal CWAPF
		structure, and remains open until fragmentation process is
		complete (\ref CWAPF_GEN_FRAG_STATUS_DONE).

		This function should be called repeatedly
		until the returned status indicates fragmentation is complete
		(\ref CWAPF_GEN_FRAG_STATUS_DONE).

		Ordering:
		For best performance it is recommended to work concurrently,
		and move to exclusive mode only before enqueuing the last
		fragment. From this point transition to concurrent is not
		allowed. This way fragments of different frames will be
		interleaved but ordering will be kept between the last
		fragments.
		Alternately, user can move to exclusive mode before calling
		CWAPF init or before enqueuing the first fragment. From this
		point transition to concurrent is not allowed. This way the
		whole fragmentation process will be done exclusively and there
		will be no interleaving between fragments of different frames
		However, in case there is IPSec later in the flow, the ordering
		scope must be Exclusive before first fragment enters IPSec.

@Param[in]	cwapf_context_addr - Address to the CWAPF internal context. Must
		be initialized by cwapf_context_init() prior to the first call.

@Return		Status. (\ref CWAPF_GENERATE_FRAG_STATUS or negative value on
		error.)

@Retval		ENOMEM - Received packet cannot be stored due to buffer pool
		depletion. Recommendation is to discard the frame.
		The packet was not fragmented.
@Retval		EIO - Received packet FD contain errors (FD.err != 0).
		Recommendation is to either force discard of the default frame
		(by calling \ref fdma_force_discard_fd) or enqueue the default
		frame.
		The packet was not fragmented.

@Cautions	In the output fragment, ASA & PTA are not presented.\n
		It is assumed that the address of the presented segment is
		aligned to 16 bytes.\n
		As part of a workaround to ticket TKT260685 in REV1 this
		function requires one of the four nested scope levels.

*//***************************************************************************/
int cwapf_generate_frag(cwapf_ctx_t cwapf_context_addr);

/**************************************************************************//**
@Function	cwapf_discard_frame_remainder

@Description	This function discards the remainder of the frame being
		fragmented in case the user decides to stop the fragmentation
		process before its completion (before
		\ref CWAPF_GEN_FRAG_STATUS_DONE status is returned).

@Param[in]	cwapf_context_addr - Address to the CWAPF internal context.

@Return		Status of the operation (0 - Success).

@Cautions	Following this function no packet resides in the default frame
		location in the task defaults.
		This function should only be called after \ref
		CWAPF_GEN_FRAG_STATUS_IN_PROCESS status is returned from \ref
		cwapf_generate_frag() function call.
*//***************************************************************************/
int cwapf_discard_frame_remainder(cwapf_ctx_t cwapf_context_addr);

/**************************************************************************//**
@Function	cwapf_context_init

@Description	This function initializes the CWAPF context structure that is
		used for the CAPWAP fragmentation process.

@Param[in]	mtu - Maximum Transmit Unit.

@Param[out]	cwapf_context_addr - Address to the CWAPF internal context
		structure located in the workspace by the user.
		Internally used by CAPWAP Fragmentation functions.

@Return		None.

*//***************************************************************************/
void cwapf_context_init(uint16_t mtu, cwapf_ctx_t cwapf_context_addr);

/** @} */ /* end of CWAPF_Functions */
/** @} */ /* end of FSL_CWAPF */
/** @} */ /* end of NETF */

#endif /* __FSL_CWAPF_H */
