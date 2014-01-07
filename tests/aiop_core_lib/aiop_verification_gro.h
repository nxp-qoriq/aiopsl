/**************************************************************************//**
@File		aiop_verification_gro.h

@Description	This file contains the AIOP GRO SW Verification Structures.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_GRO_H_
#define __AIOP_VERIFICATION_GRO_H_

#include "dplib/fsl_ldpaa.h"
#include "common/gen.h"
#include "general.h"
#include "gro.h"
#include "dplib/fsl_parser.h"


/* TCP_GRO Command IDs */
	/** TCP GRO Aggregate Segment command code */
#define TCP_GRO_CONTEXT_AGG_SEG_CMD		0x00000001
	/** TCP GRO Flush Aggregation command code */
#define TCP_GRO_CONTEXT_FLUSH_AGG_CMD		0x00000002


/* TCP_GRO Commands Structure identifiers */
	/** TCP GRO Aggregate Segment command Structure identifier */
#define TCP_GRO_CONTEXT_AGG_SEG_CMD_STR	(TCP_GRO_MODULE_STATUS_ID | 	\
			TCP_GRO_CONTEXT_AGG_SEG_CMD)
	/** TCP GRO Flush Aggregation Command Structure identifier */
#define TCP_GRO_CONTEXT_FLUSH_AGG_CMD_STR (TCP_GRO_MODULE_STATUS_ID | 	\
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
	/* Todo - should we add:
	 * 	1. parse results
	 * 	2. presentation context
	 * 	3. global task defaults
	 * 	4. gro internal context - it is in external memory already.. */
		/** Returned Value: 
		 * Iteration return status. */
	int32_t status;
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
	int32_t status;	
		/** Address (in HW buffers) of the TCP GRO internal context. 
		The user should allocate \ref tcp_gro_ctx_t in this address.
		The user should zero the \ref tcp_gro_ctx_t allocated space once
		a new session begins. */
	uint64_t tcp_gro_context_addr;
	/* Todo - should we add:
	 * 	1. parse results
	 * 	2. presentation context
	 * 	3. global task defaults
	 * 	4. gro internal context - it is in external memory already.. */	
};



uint16_t  aiop_verification_gro(uint32_t data_addr);


/** @} */ /* end of AIOP_GRO_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */


#endif /* __AIOP_VERIFICATION_GRO_H_ */