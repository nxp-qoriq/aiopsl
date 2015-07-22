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
@File			fsl_ipf.h

@Description	This file contains the AIOP SW IP fragmentation API
*//***************************************************************************/
#ifndef __FSL_IPF_H
#define __FSL_IPF_H

#include "types.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		FSL_IPF IPF

@Description	Freescale AIOP IP Fragmentation

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	IPF_MACROS IP Fragmentation Macros
@{
*//***************************************************************************/
/**************************************************************************//**
 @Group	IPF_GENERAL_DEFINITIONS IPF General Definitions
 @{
*//***************************************************************************/

	/** IPF context size definition. */
#define IPF_CONTEXT_SIZE	64
	/** IPF context definition. Must be aligned to 32 Bytes.*/
typedef uint8_t ipf_ctx_t[IPF_CONTEXT_SIZE]
			__attribute__((aligned(sizeof(struct ldpaa_fd))));


/** @} */ /* end of TCP_IPF_GENERAL_DEFINITIONS */


/**************************************************************************//**
 @Group	IPF_FLAGS IPF Flags

 @Description	Flags for ipf_generate_frag() function

 @{
*//***************************************************************************/
/** No flags indication. */
#define IPF_NO_FLAGS			0x00000000
/** Restore Original Fragments.
 * When set, fragmentation restores the original fragments of a reassembled
 * frame and ignores MTU. */
#define IPF_RESTORE_ORIGINAL_FRAGMENTS	0x00000001

/** @} */ /* end of IPF_FLAGS */

/**************************************************************************//**
@Group	IPF_GENERATE_FRAG_STATUS  IPF Return Status

@Description ipf_generate_frag() return values

@{
*//***************************************************************************/
/** Fragmentation process complete. The last fragment was generated */
#define	IPF_GEN_FRAG_STATUS_DONE		SUCCESS
/** Fragmentation process didn't complete.
 * Fragment was generated and the user SHOULD call generate_frag()
 * again to generate another fragment*/
#define	IPF_GEN_FRAG_STATUS_IN_PROCESS	(IPF_MODULE_STATUS_ID | 0x1)
/** Fragmentation not done due to Length > MTU but DF=1 */
#define	IPF_GEN_FRAG_STATUS_DF_SET	(IPF_MODULE_STATUS_ID | 0x2)

/** @} */ /* end of IPF_GENERATE_FRAG_STATUS */

/** @} */ /* end of IPF_MACROS */


/**************************************************************************//**
@Group		IPF_Functions IPF Functions

@Description	AIOP IPF Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	ipf_generate_frag

@Description	This function generates a single IP fragment and locates it in
		the default frame location in the workspace.

		Pre-condition - In the first iteration this function is called
		for a source packet, the source packet should be located
		at the default frame location in workspace.

		The remaining source frame is kept in the internal IPF
		structure, and remains open until fragmentation process is
		complete (\ref IPF_GEN_FRAG_STATUS_DONE).

		This function should be called repeatedly
		until the returned status indicates fragmentation is complete
		(\ref IPF_GEN_FRAG_STATUS_DONE).
		
		Ordering:
		For best performance it is recommended to work concurrently,
		and move to exclusive mode only before enqueuing the last
		fragment. From this point transition to concurrent is not
		allowed. This way fragments of different frames will be
		interleaved but ordering will be kept between the last
		fragments.
		Alternately, user can move to exclusive mode before calling IPF
		init or before enqueuing the first fragment. From this point
		transition to concurrent is not allowed. This way the whole
		fragmentation process will be done exclusively and there will be
		no interleaving between fragments of different frames. 
		However, in case there is IPSec later in the flow, the ordering
		scope must be Exclusive before first fragment enters IPSec.

@Param[in]	ipf_context_addr - Address to the IPF internal context. Must
		be initialized by ipf_context_init() prior to the first call.

@Return		Status. (\ref IPF_GENERATE_FRAG_STATUS or negative value on
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
		No support in IPv6 jumbograms.\n
		It is assumed that the address of the presented segment is
		aligned to 16 bytes.\n
		Since during fragmentation process of an IPv6 frame, fragment
		extension (8 bytes) is added to the header, 8 bytes will be
		removed from the tail of the presented segment.

*//***************************************************************************/
int ipf_generate_frag(ipf_ctx_t ipf_context_addr);

/**************************************************************************//**
@Function	ipf_discard_frame_remainder

@Description	This function discards the remainder of the frame being
		fragmented in case the user decides to stop the fragmentation
		process before its completion (before
		\ref IPF_GEN_FRAG_STATUS_DONE status is returned).

@Param[in]	ipf_context_addr - Address to the IPF internal context.

@Return		Status of the operation (0 - Success).

@Cautions	Following this function no packet resides in the default frame
		location in the task defaults.
		This function should only be called after \ref
		IPF_GEN_FRAG_STATUS_IN_PROCESS status is returned from \ref
		ipf_generate_frag() function call.
*//***************************************************************************/
int ipf_discard_frame_remainder(ipf_ctx_t ipf_context_addr);

/**************************************************************************//**
@Function	ipf_context_init

@Description	This function initializes the IPF context structure that is
		used for the IP fragmentation process.

@Param[in]	flags - Please refer to \ref IPF_FLAGS.
@Param[in]	mtu - Maximum Transmit Unit.
		In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
		parameter is ignored.
@Param[out]	ipf_context_addr - Address to the IPF internal context
		structure located in the workspace by the user.
		Internally used by IP Fragmentation functions.

@Return		None.

@Cautions	No support in IPv6 jumbograms.
*//***************************************************************************/
void ipf_context_init(uint32_t flags, uint16_t mtu, ipf_ctx_t ipf_context_addr);

/** @} */ /* end of IPF_Functions */
/** @} */ /* end of FSL_IPF */
/** @} */ /* end of NETF */


#endif /* __FSL_IPF_H */
