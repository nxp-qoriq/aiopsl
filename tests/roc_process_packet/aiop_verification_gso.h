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
@File		aiop_verification_gso.h

@Description	This file contains the AIOP GSO SW Verification Structures.

*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_GSO_H_
#define __AIOP_VERIFICATION_GSO_H_

#include "fsl_ldpaa.h"
#include "fsl_gen.h"
#include "general.h"
#include "gso.h"
#include "fsl_parser.h"


/* TCP_GSO Command IDs */
	/** TCP GSO Init command code */
#define TCP_GSO_CONTEXT_INIT_CMD		0x00000001
	/** TCP GSO Generate Segment command code */
#define TCP_GSO_GENERATE_SEG_CMD		0x00000002
	/** TCP GSO Discard Remaining Frame command code */
#define TCP_GSO_DISCARD_REMAINING_FRAME_CMD	0x00000003


/* TCP_GSO Commands Structure identifiers */
	/** TCP GSO Init command Structure identifier */
#define TCP_GSO_CONTEXT_INIT_CMD_STR	((GSO_MODULE << 16) | 	\
			TCP_GSO_CONTEXT_INIT_CMD)
	/** TCP GSO Generate Segment Command Structure identifier */
#define TCP_GSO_GENERATE_SEG_CMD_STR	((GSO_MODULE << 16) | 	\
			TCP_GSO_GENERATE_SEG_CMD)
	/** TCP GSO Discard Remaining Frame command Structure identifier */
#define TCP_GSO_DISCARD_REMAINING_CMD_STR ((GSO_MODULE << 16) |	\
			TCP_GSO_DISCARD_REMAINING_FRAME_CMD)


/** \addtogroup AIOP_FMs_Verification
 *  @{
 */


/**************************************************************************//**
 @Group		AIOP_GSO_Verification

 @Description	AIOP GSO Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GSO Init Context Command structure.

		Includes information needed for GSO init command.

*//***************************************************************************/
struct tcp_gso_init_command {
		/** TCP GSO Init Context command structure identifier. */
	uint32_t opcode;
		/** Please refer to \ref TCP_GSO_FLAGS. */
	uint32_t flags;
		/** Workspace address of the GSO internal context.
		 * Should be defined in the TLS area. */
	uint32_t gso_ctx_addr;
		/** Maximum Segment Size. */
	uint16_t mss;
		/** Padding. */
	int8_t  pad[2];
};

/**************************************************************************//**
@Description	TCP GSO Generate Segment Command structure.

		Includes information needed for GSO Generate Segment command.

*//***************************************************************************/
struct tcp_gso_generate_seg_command {
		/** TCP GSO Generate Segment command structure identifier. */
	uint32_t opcode;
		/** Workspace address of the GSO internal context.
		 * Should be defined in the TLS area. */
	uint32_t gso_ctx_addr;
		/** Returned Value:
		 * Iteration return status. */
	int32_t status;
		/** Workspace address of the GSO last returned status.
		 * Should be defined in the TLS area. */
	uint32_t gso_status_addr;
		/** Returned Value:
		 * presentation context. */
	struct presentation_context prc;
		/** Returned Value:
		 * parse results. */
	struct parse_result pr;
		/** Returned Value:
		 * task defaults. */
	struct aiop_default_task_params default_task_params;
		/** Padding. */
	int8_t  pad[8];
};

/**************************************************************************//**
@Description	TCP GSO Discard Remainder Frame Command structure.

		Includes information needed for GSO Remainder Frame command.

*//***************************************************************************/
struct tcp_gso_discard_remainder_frame_command {
		/** TCP GSO discard remainder frame command structure
		 * identifier. */
	uint32_t opcode;
		/** Workspace address of the GSO internal context.
		 * Should be defined in the TLS area. */
	uint32_t gso_ctx_addr;
		/** Returned Value:
		 * Iteration return status. */
	int32_t  status;
		/** Workspace address of the last GSO returned status.
		 * Should be defined in the TLS area. */
	uint32_t gso_status_addr;
};


uint16_t  aiop_verification_gso(
		uint32_t data_addr);


/** @} */ /* end of AIOP_GSO_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */


#endif /* __AIOP_VERIFICATION_GSO_H_ */
