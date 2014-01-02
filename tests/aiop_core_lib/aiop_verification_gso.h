/**************************************************************************//**
@File		aiop_verification_gso.h

@Description	This file contains the AIOP GSO SW Verification Structures.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_GSO_H_
#define __AIOP_VERIFICATION_GSO_H_

#include "dplib/fsl_ldpaa.h"
#include "common/gen.h"
#include "general.h"
#include "gso.h"


/* TCP_GSO Command IDs */
	/** TCP GSO Init command code */
#define TCP_GSO_CONTEXT_INIT_CMD			0x00000001
	/** TCP GSO Generate Segment command code */
#define TCP_GSO_GENERATE_SEG_CMD		0x00000002
	/** TCP GSO Discard Remaining Frame command code */
#define TCP_GSO_DISCARD_REMAINING_FRAME_CMD		0x00000003


/* TCP_GSO Commands Structure identifiers */
	/** TCP GSO Init command Structure identifier */
#define TCP_GSO_CONTEXT_INIT_CMD_STR	(TCP_GSO_MODULE_STATUS_ID | 	\
			TCP_GSO_CONTEXT_INIT_CMD)
	/** TCP GSO Generate Segment Command Structure identifier */
#define TCP_GSO_GENERATE_SEG_CMD_STR	(TCP_GSO_MODULE_STATUS_ID | 	\
			TCP_GSO_GENERATE_SEG_CMD)
	/** TCP GSO Discard Remaining Frame command Structure identifier */
#define TCP_GSO_DISCARD_REMAINING_CMD_STR	(TCP_GSO_MODULE_STATUS_ID | \
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
		/** Maximum Segment Size. */
	uint16_t mss;
		/** Padding. */
	int8_t  pad[6];
};

/**************************************************************************//**
@Description	TCP GSO Generate Segment Command structure.

		Includes information needed for GSO Generate Segment command.

*//***************************************************************************/
struct tcp_gso_generate_seg_command {
		/** TCP GSO Generate Segment command structure identifier. */
	uint32_t opcode;
		/** Command returned status. */
	int32_t  status;
		/** Command returned status. */
	uint64_t results;
		
		
};

uint16_t  aiop_verification_gso(uint16_t data_addr, uint16_t rem_data_size);


/** @} */ /* end of AIOP_GSO_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */


#endif /* __AIOP_VERIFICATION_GSO_H_ */
