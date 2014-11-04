/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
@File		aiop_verification_gro.h

@Description	This file contains the AIOP GRO SW Verification Structures.

*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_GRO_H_
#define __AIOP_VERIFICATION_GRO_H_

#include "fsl_ldpaa.h"
#include "fsl_gen.h"
#include "general.h"
#include "gro.h"
#include "fsl_parser.h"


/* TCP_GRO Command IDs */
	/** TCP GRO Aggregate Segment command code */
#define TCP_GRO_CONTEXT_AGG_SEG_CMD		0x00000001
	/** TCP GRO Flush Aggregation command code */
#define TCP_GRO_CONTEXT_FLUSH_AGG_CMD		0x00000002


/* TCP_GRO Commands Structure identifiers */
	/** TCP GRO Aggregate Segment command Structure identifier */
#define TCP_GRO_CONTEXT_AGG_SEG_CMD_STR	((GRO_MODULE << 16) | 		\
			TCP_GRO_CONTEXT_AGG_SEG_CMD)
	/** TCP GRO Flush Aggregation Command Structure identifier */
#define TCP_GRO_CONTEXT_FLUSH_AGG_CMD_STR ((GRO_MODULE << 16) | 	\
			TCP_GRO_CONTEXT_FLUSH_AGG_CMD)


/** \addtogroup AIOP_FMs_Verification
 *  @{
 */


/**************************************************************************//**
 @Group		AIOP_GRO_Verification

 @Description	AIOP GRO Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GRO Aggregate Segment Command structure.

		Includes information needed for GRO Aggregate Segment command.

*//***************************************************************************/
struct tcp_gro_agg_seg_command {
		/** TCP GRO Aggregate Segment command structure identifier. */
	uint32_t opcode;
		/** Please refer to \ref TCP_GRO_AGG_FLAGS. */
	uint32_t flags;
		/** Address (in HW buffers) of the TCP GRO internal context.
		The user should allocate \ref tcp_gro_ctx_t in this address.
		The user should zero the \ref tcp_gro_ctx_t allocated space once
		a new session begins. */
	uint64_t tcp_gro_context_addr;
		/** Pointer to the TCP GRO aggregation parameters. */
	struct tcp_gro_context_params params;
		/** Returned Value:
		 * presentation context. */
	struct presentation_context prc;
		/** Returned Value:
		 * Iteration return status. */
	int status;
		/** Workspace address of the GRO last returned status.
		 * Should be defined in the TLS area. */
	uint32_t gro_status_addr;
		/** Shared RAM address of the tmi_id. */
	uint32_t tmi_id_addr;
		/** Padding. */
	int8_t  pad[4];
};

/**************************************************************************//**
@Description	TCP GRO Flush Aggregation Command structure.

		Includes information needed for GRO Flush Aggregation command.

*//***************************************************************************/
struct tcp_gro_flush_agg_command {
		/** TCP GRO Flush Aggregation command structure identifier. */
	uint32_t opcode;
		/** Returned Value:
		 * Iteration return status. */
	int status;
		/** Address (in HW buffers) of the TCP GRO internal context.
		The user should allocate \ref tcp_gro_ctx_t in this address.
		The user should zero the \ref tcp_gro_ctx_t allocated space once
		a new session begins. */
	uint64_t tcp_gro_context_addr;
		/** Returned Value:
		 * presentation context. */
	struct presentation_context prc;
		/** Returned Value:
		 * parse results. */
	struct parse_result pr;
		/** Workspace address of the GSO last returned status.
		 * Should be defined in the TLS area. */
	uint32_t gro_status_addr;
		/** Padding. */
	int8_t  pad[4];
};



uint16_t  aiop_verification_gro(uint32_t data_addr);
void gro_verif_create_next_frame(uint8_t gro_iteration);

/** @} */ /* end of AIOP_GRO_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */


#endif /* __AIOP_VERIFICATION_GRO_H_ */
