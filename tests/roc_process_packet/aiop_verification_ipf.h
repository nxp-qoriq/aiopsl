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
@File		aiop_verification_ipf.h

@Description	This file contains the AIOP IPF SW Verification Structures.

*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_IPF_H_
#define __AIOP_VERIFICATION_IPF_H_

#include "fsl_ldpaa.h"
#include "fsl_gen.h"
#include "general.h"
#include "ipf.h"
#include "fsl_parser.h"


/* IPF Command IDs */
	/** IPF Context Init command code */
#define IPF_CONTEXT_INIT_CMD			0x00000001
	/** IPF Generate Fragment command code */
#define IPF_GENERATE_FRAG_CMD			0x00000002
	/** IPF Discard Remaining Frame command code */
#define IPF_DISCARD_REMAINING_FRAME_CMD		0x00000003


/* IPF Commands Structure identifiers */
	/** IPF Context Init command Structure identifier */
#define IPF_CONTEXT_INIT_CMD_STR ((IPF_MODULE << 16) | IPF_CONTEXT_INIT_CMD)
	/** IPF Generate Fragment Command Structure identifier */
#define IPF_GENERATE_FRAG_CMD_STR ((IPF_MODULE << 16) | \
				IPF_GENERATE_FRAG_CMD)
	/** IPF Discard Remaining Frame command Structure identifier */
#define IPF_DISCARD_REMAINING_FRAME_CMD_STR  ((IPF_MODULE << 16) | \
				IPF_DISCARD_REMAINING_FRAME_CMD)

/** \addtogroup AIOP_FMs_Verification
 *  @{
 */


/**************************************************************************//**
 @Group		AIOP_IPF_Verification

 @Description	AIOP IPF Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPF Init Context Command structure.

		Includes information needed for IPF init command.

*//***************************************************************************/
struct ipf_init_command {
		/** IPF Init Context command structure identifier. */
	uint32_t opcode;
		/** Please refer to \ref IPF_Flags. */
	uint32_t flags;
		/** Workspace address of the IPF internal context.
		 * Should be defined in the TLS area. */
	uint32_t ipf_ctx_addr;
		/** Maximum Transmission Unit. */
	uint16_t mtu;
		/** Padding. */
/*	int8_t  pad[2];*/
};

/**************************************************************************//**
@Description	IPF Generate Fragment Command structure.

		Includes information needed for IPF Generate Fragment command.

*//***************************************************************************/
struct ipf_generate_frag_command {
		/** IPF Generate Fragment command structure identifier. */
	uint32_t opcode;
		/** Workspace address of the IPF internal context.
		 * Should be defined in the TLS area. */
	uint32_t ipf_ctx_addr;
		/** Returned Value:
		 * Iteration return status. */
	int32_t status;
		/** Workspace address of the IPF last returned status.
		 * Should be defined in the TLS area. */
	uint32_t ipf_status_addr;
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
@Description	IPF Discard Frame Remainder Command structure.

		Includes information needed for IPF Frame Remainder command.

*//***************************************************************************/
struct ipf_discard_remainder_frame_command {
		/** IPF discard remainder frame command structure
		 * identifier. */
	uint32_t opcode;
		/** Workspace address of the IPF internal context.
		 * Should be defined in the TLS area. */
	uint32_t ipf_ctx_addr;
		/** Returned Value:
		 * Command return status. */
	int32_t status;
		/** Workspace address of the last IPF returned status.
		 * Should be defined in the TLS area. */
	uint32_t ipf_status_addr;
};


uint16_t  aiop_verification_ipf(
		uint32_t data_addr);


/** @} */ /* end of AIOP_IPF_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */


#endif /* __AIOP_VERIFICATION_GSO_H_ */
