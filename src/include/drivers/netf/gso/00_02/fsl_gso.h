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
 @File          fsl_gso.h

 @Description   This file contains the AIOP SW TCP GSO API
*//***************************************************************************/
#ifndef __FSL_GSO_H
#define __FSL_GSO_H

#include "fsl_types.h"
#include "fsl_ldpaa.h"

/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_L4 L4

 @Description	AIOP L4 related API

 @{
 *//***************************************************************************/

/**************************************************************************//**
@Group		FSL_AIOP_GSO GSO

@Description	FSL_AIOP_GSO

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	GSO_MACROS GSO Macros

@Description	GSO Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GSO_GENERAL_DEFINITIONS TCP GSO General Definitions

 @Description TCP GSO General Definitions.

 @{
*//***************************************************************************/

	/** GSO code placement. Must be one of __COLD_CODE (default),
	 *  __WARM_CODE or __HOT_CODE. */
#ifndef GSO_CODE_PLACEMENT
#define GSO_CODE_PLACEMENT __COLD_CODE
#endif
	/** TCP GSO context size definition. */
#define TCP_GSO_CONTEXT_SIZE	64
	/** TCP GSO context definition.
	 * Must be aligned to 32 bytes. */
typedef uint8_t tcp_gso_ctx_t[TCP_GSO_CONTEXT_SIZE];


/** @} */ /* end of TCP_GSO_GENERAL_DEFINITIONS */


/**************************************************************************//**
 @Group	TCP_GSO_FLAGS TCP GSO Flags

 @Description Flags for tcp_gso_context_init() function.

 @{
*//***************************************************************************/

	/** GSO no flags indication. */
#define TCP_GSO_NO_FLAGS		0x00000000

/** @} */ /* end of TCP_GSO_FLAGS */

/**************************************************************************//**
@Group	TCP_GSO_GENERATE_SEG_STATUS  TCP GSO Statuses

@Description tcp_gso_generate_seg() return values

@{
*//***************************************************************************/

	/** Segmentation process complete. The last segment was generated. */
#define	TCP_GSO_GEN_SEG_STATUS_DONE					\
						SUCCESS
	/** Segmentation process did not complete.
	 * Segment was generated and the user should call
	 * gso_generate_tcp_seg() again to generate another segment */
#define	TCP_GSO_GEN_SEG_STATUS_IN_PROCESS				\
						(TCP_GSO_MODULE_STATUS_ID | 0x1)
	/** Segmentation process cannot start since the syn/rst flags are set.*/
#define	TCP_GSO_GEN_SEG_STATUS_SYN_RST_SET				\
						(TCP_GSO_MODULE_STATUS_ID | 0x2)

/** @} */ /* end of TCP_GSO_GENERATE_SEG_STATUS */

/** @} */ /* end of GSO_MACROS */


/**************************************************************************//**
@Group		GSO_Functions GSO Functions

@Description	GSO Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	tcp_gso_generate_seg

@Description	This function generates a single TCP segment and locates it in
		the default frame location in the workspace.

		Pre-condition - In the first iteration this function is called
		for a source packet, the source packet should be located
		at the default frame location in workspace.

		The remaining source frame is kept in the internal GSO
		structure.

		This function should be called repeatedly
		until the returned status indicates segmentation is completed
		(\ref TCP_GSO_GEN_SEG_STATUS_DONE).

		Ordering:
		To keep order between frames, user should move to exclusive mode
		before calling GSO init. From this point transition to
		concurrent is not allowed, thus the whole segmentation process
		will be done exclusively.

@Param[in]	tcp_gso_context_addr - Address to the TCP GSO internal context.
		Must be initialized by gso_context_init() prior to the first
		call. Must be aligned to Frame Descriptor size.

@Return		GSO Status (\ref TCP_GSO_GENERATE_SEG_STATUS), or
		negative value on error.

@Retval		ENOMEM - Received packet cannot be stored due to buffer pool
		depletion. Recommendation is to discard the frame 
		(call fdma_discard_default_frame).
		The packet was not segmented.
@Retval		EIO - Received packet FD contain errors (FD.err != 0).
		Recommendation is to either force discard of the default frame
		(by calling \ref fdma_force_discard_fd) or enqueue the default
		frame.
		The packet was not segmented.

@Cautions	None.
*//***************************************************************************/
int tcp_gso_generate_seg(
		tcp_gso_ctx_t tcp_gso_context_addr);

/**************************************************************************//**
@Function	tcp_gso_discard_frame_remainder

@Description	This function discard the remainder packet being segmented in
		case the user decides to stop the segmentation process before
		its completion (before a \ref TCP_GSO_GEN_SEG_STATUS_DONE status
		is returned).

@Param[in]	tcp_gso_context_addr - Address to the TCP GSO internal context.
		Must be aligned to Frame Descriptor size.

@Return	0 - Success

@Cautions	Following this function no packet resides in the default frame
		location in the task defaults.
		This function should only be called after \ref
		TCP_GSO_GEN_SEG_STATUS_IN_PROCESS status is returned from
		gso_generate_tcp_seg() function call.
*//***************************************************************************/
int tcp_gso_discard_frame_remainder(
		tcp_gso_ctx_t tcp_gso_context_addr);

/**************************************************************************//**
@Function	tcp_gso_context_init

@Description	This function initializes the GSO context structure that is
		used for the TCP GSO process of the packet.

		This function must be called once before each new packet
		segmentation process.

@Param[in]	flags - Please refer to \ref TCP_GSO_FLAGS.
@Param[in]	mss - Maximum Segment Size.
@Param[out]	tcp_gso_context_addr - Address to the TCP GSO internal context
		structure located in the workspace by the user. Internally used
		by TCP GSO functions.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void tcp_gso_context_init(
		uint32_t flags,
		uint16_t mss,
		tcp_gso_ctx_t tcp_gso_context_addr);

/** @} */ /* end of GSO_Functions */
/** @} */ /* end of FSL_AIOP_GSO */
/** @} */ /* end of group AIOP_L4 */
/** @} */ /* end of NETF */


#endif /* __FSL_GSO_H */
