/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
@File          cwapf.h

@Description   This file contains CAPWAP Fragmentation internal functions and
		definitions

*//***************************************************************************/

#ifndef __AIOP_CWAPF_H
#define __AIOP_CWAPF_H

#include "fsl_cwapf.h"

/**************************************************************************//**
@Group		AIOP_CWAPF_INTERNAL  AIOP CAPWAP Fragmentation Internal

@Description	AIOP CAPWAP Fragmentation Internal

@{
*//***************************************************************************/


/**************************************************************************//**
@Group		CWAPF_INTERNAL_STRUCTS CAPWAP Fragmentation Internal Structures

@Description	AIOP CAPWAP Fragmentation Internal Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	CWAPF Context Internally used by CAPWAP Fragmentation functions.
*//***************************************************************************/
struct cwapf_context {
	 /** Remaining frame's FD  */
	struct ldpaa_fd rem_fd
			__attribute__((aligned(sizeof(struct ldpaa_fd))));
	/** Original Starting HXS for Parser from Task default */
	uint16_t parser_starting_hxs;
	/** Original segment address from PRC */
	uint16_t prc_seg_address;
	/** Original segment length from PRC */
	uint16_t prc_seg_length;
	/** default segment offset for split command parameters */
	uint16_t prc_seg_offset;
	/** Remaining payload length*/
	uint16_t remaining_payload_length;
	/** MTU payload length*/
	uint16_t mtu_payload_length;
	/** Split size*/
	uint16_t split_size;
	/** Previous Fragment Offset */
	uint16_t prev_frag_offset;
	/** Original Parser Profile ID from Task default */
	uint8_t parser_profile_id;
	/** Remaining frame handle */
	uint8_t rem_frame_handle;
	/** First fragment indication */
	uint8_t	first_frag;
	/** Padding */
	uint8_t	pad[2];
};

/** @} */ /* end of CWAPF_INTERNAL_STRUCTS */


/**************************************************************************//**
@Group	CWAPF_INTERNAL_MACROS CAPWAP Fragmentation Internal Macros

@Description	CWAPF Internal Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	CWAPF_GENERAL_INT_DEFINITIONS CAPWAP Fragmentation General Internal
		Definitions

 @Description CAPWAP Fragmentation General Internal Definitions.

 @{
*//***************************************************************************/

/** Size of CWAPF Context. */
#define SIZEOF_CWAPF_CONTEXT	(sizeof(struct cwapf_context))

#pragma warning_errors on
/** CWAPF internal struct size assertion check. */
ASSERT_STRUCT_SIZE(SIZEOF_CWAPF_CONTEXT, CWAPF_CONTEXT_SIZE);
#pragma warning_errors off

/** @} */ /* end of CWAPF_GENERAL_INT_DEFINITIONS */

/** @} */ /* end of CWAPF_INTERNAL_MACROS */

int cwapf_restore_orig_fragment(struct cwapf_context *cwapf_ctx);
void cwapf_after_split_fragment(struct cwapf_context *cwapf_ctx);
int cwapf_split_fragment(struct cwapf_context *cwapf_ctx);

int cwapf_move_remaining_frame(struct cwapf_context *cwapf_ctx);
int cwapf_last_frag(struct cwapf_context *cwapf_ctx);
int cwapf_last_frag(struct cwapf_context *cwapf_ctx);

/** @} */ /* end of AIOP_CWAPF_INTERNAL */

#endif /* __AIOP_CWAPF_H */
