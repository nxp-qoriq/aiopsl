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
@File		aiop_verification_cwapf.h

@Description	This file contains the AIOP CWAPF SW Verification Structures.

*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_CWAPF_H_
#define __AIOP_VERIFICATION_CWAPF_H_

#include "fsl_ldpaa.h"
#include "fsl_gen.h"
#include "general.h"
#include "cwapf.h"
#include "fsl_parser.h"


/* CWAPF Command IDs */
	/** CWAPF Context Init command code */
#define CWAPF_CONTEXT_INIT_CMD			0x00000001
	/** CWAPF Generate Fragment command code */
#define CWAPF_GENERATE_FRAG_CMD			0x00000002
	/** CWAPF Discard Remaining Frame command code */
#define CWAPF_DISCARD_REMAINING_FRAME_CMD		0x00000003


/* CWAPF Commands Structure identifiers */
	/** CWAPF Context Init command Structure identifier */
#define CWAPF_CONTEXT_INIT_CMD_STR ((CWAPF_MODULE << 16) | CWAPF_CONTEXT_INIT_CMD)
	/** CWAPF Generate Fragment Command Structure identifier */
#define CWAPF_GENERATE_FRAG_CMD_STR ((CWAPF_MODULE << 16) | \
				CWAPF_GENERATE_FRAG_CMD)
	/** CWAPF Discard Remaining Frame command Structure identifier */
#define CWAPF_DISCARD_REMAINING_FRAME_CMD_STR  ((CWAPF_MODULE << 16) | \
				CWAPF_DISCARD_REMAINING_FRAME_CMD)

/** \addtogroup AIOP_FMs_Verification
 *  @{
 */


/**************************************************************************//**
 @Group		AIOP_CWAPF_Verification

 @Description	AIOP CWAPF Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	CWAPF Init Context Command structure.

		Includes information needed for CWAPF init command.

*//***************************************************************************/
struct cwapf_init_command {
		/** CWAPF Init Context command structure identifier. */
	uint32_t opcode;
		/** Workspace address of the CWAPF internal context.
		 * Should be defined in the TLS area. */
	uint32_t cwapf_ctx_addr;
		/** Maximum Transmission Unit. */
	uint16_t mtu;
		/** Padding. */
/*	int8_t  pad[2];*/
};

/**************************************************************************//**
@Description	CWAPF Generate Fragment Command structure.

		Includes information needed for CWAPF Generate Fragment command.

*//***************************************************************************/
struct cwapf_generate_frag_command {
		/** CWAPF Generate Fragment command structure identifier. */
	uint32_t opcode;
		/** Workspace address of the CWAPF internal context.
		 * Should be defined in the TLS area. */
	uint32_t cwapf_ctx_addr;
		/** Returned Value:
		 * Iteration return status. */
	int32_t status;
		/** Workspace address of the CWAPF last returned status.
		 * Should be defined in the TLS area. */
	uint32_t cwapf_status_addr;
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
	int8_t  pad[4];

};

/**************************************************************************//**
@Description	CWAPF Discard Frame Remainder Command structure.

		Includes information needed for CWAPF Frame Remainder command.

*//***************************************************************************/
struct cwapf_discard_remainder_frame_command {
		/** CWAPF discard remainder frame command structure
		 * identifier. */
	uint32_t opcode;
		/** Workspace address of the CWAPF internal context.
		 * Should be defined in the TLS area. */
	uint32_t cwapf_ctx_addr;
		/** Returned Value:
		 * Command return status. */
	int32_t status;
		/** Workspace address of the last CWAPF returned status.
		 * Should be defined in the TLS area. */
	uint32_t cwapf_status_addr;
};


uint16_t  aiop_verification_cwapf(
		uint32_t data_addr);


/** @} */ /* end of AIOP_CWAPF_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */


#endif /* __AIOP_VERIFICATION_GSO_H_ */
