/**************************************************************************//**
@File		aiop_verification_ipf.h

@Description	This file contains the AIOP IPF SW Verification Structures.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_IPF_H_
#define __AIOP_VERIFICATION_IPF_H_

#include "dplib/fsl_ldpaa.h"
#include "common/gen.h"
#include "general.h"
#include "ipf.h"
#include "dplib/fsl_parser.h"


/* IPF Command IDs */
	/** IPF Context Init command code */
#define IPF_CONTEXT_INIT_CMD			0x00000001
	/** IPF Generate Fragment command code */
#define IPF_GENERATE_FRAG_CMD			0x00000002
	/** IPF Discard Remaining Frame command code */
#define IPF_DISCARD_REMAINING_FRAME_CMD		0x00000003


/* IPF Commands Structure identifiers */
	/** IPF Context Init command Structure identifier */
#define IPF_CONTEXT_INIT_CMD_STR (IPF_MODULE_STATUS_ID | IPF_CONTEXT_INIT_CMD)
	/** IPF Generate Fragment Command Structure identifier */
#define IPF_GENERATE_FRAG_CMD_STR	(IPF_MODULE_STATUS_ID | \
						IPF_GENERATE_FRAG_CMD)
	/** IPF Discard Remaining Frame command Structure identifier */
#define IPF_DISCARD_REMAINING_FRAME_CMD_STR  (IPF_MODULE_STATUS_ID | \
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
		/** Maximum Transmission Unit. */
	uint16_t mtu;
		/** Padding. */
	int8_t  pad[6];
};

/**************************************************************************//**
@Description	IPF Generate Fragment Command structure.

		Includes information needed for IPF Generate Fragment command.

*//***************************************************************************/
struct ipf_generate_frag_command {
		/** IPF Generate Fragment command structure identifier. */
	uint32_t opcode;
		/** Returned Value: 
		 * Iteration return status. */
	int32_t status;
		/** Returned Internal Value:
		 * IPF context. */
	struct ipf_context ipf_ctx;
		/** Returned Value:
		 * presentation context. */
	struct presentation_context prc;
		/** Returned Value:
		 * parse results. */
	struct parse_result pr;
		/** Returned Value:
		 * task defaults. */
	struct aiop_default_task_params default_task_params;
		
};

/**************************************************************************//**
@Description	IPF Discard Frame Remainder Command structure.

		Includes information needed for IPF Frame Remainder command.

*//***************************************************************************/
struct ipf_discard_remainder_frame_command {
		/** IPF discard remainder frame command structure 
		 * identifier. */
	uint32_t opcode;
		/** Returned Value: 
		 * Command return status. */
	int32_t status;
		/** Returned Internal Value:
		 * IPF context. */
	struct ipf_context ipf_ctx;
};


uint16_t  aiop_verification_ipf(
		ipf_ctx_t ipf_context_addr, 
		uint32_t data_addr);


/** @} */ /* end of AIOP_GSO_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */


#endif /* __AIOP_VERIFICATION_GSO_H_ */