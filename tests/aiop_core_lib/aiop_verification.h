/**************************************************************************//**
@File		aiop_verification.h

@Description	This file contains the AIOP SW Verification Structures

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_H_
#define __AIOP_VERIFICATION_H_

#include "general.h"
#include "aiop_verification_fdma.h"
#include "aiop_verification_tman.h"
#include "aiop_verification_ste.h"
#include "aiop_verification_cdma.h"
#include "aiop_verification_ctlu.h"
#include "aiop_verification_parser.h"
#include "aiop_verification_hm.h"
#include "verification_virtual_pools.h"
#include "aiop_verification_gso.h"
#include "aiop_verification_ipf.h"

	/**< ACCEL_ID cmd mask */
#define ACCEL_ID_CMD_MASK		0xFFFF0000
	/**< AIOP Terminate Flow Verification command code */
#define AIOP_TERMINATE_FLOW_CMD		0xFFFF
	/**< AIOP Terminate Flow Verification command structure identifier */
#define AIOP_TERMINATE_FLOW_CMD_STR	(AIOP_TERMINATE_FLOW_CMD << 16)
	/**< Struct size error code */
#define STR_SIZE_ERR			0xFFFF
	/**< Struct size error code */
#define STR_SIZE_BIG			0xFFFE
	/**< Buffer Data chunk size. */
#define DATA_SIZE	256
	/**< Buffer Data chunk address in workspace. */
#define WS_DATA_ADDR	0x100
	/**< IPF Fragment's fragmentation commands mask */
#define IPF_FRAGMENT_FRAGMENTATION_COMMANDS_MASK 0x00000010
/**************************************************************************//**
 @Group		AIOP_Verification

 @Description	AIOP Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_Service_Routines_Verification

 @Description	AIOP Service Routines Verification structures definitions.

 @{
*//***************************************************************************/


/** @} */ /* end of AIOP_Service_Routines_Verification */


/**************************************************************************//**
 @Group		AIOP_FMs_Verification

 @Description	AIOP Applications Verification structures definitions.

 @note		All structures should be aligned to 64bit

 @{
*//***************************************************************************/


/*GSO, IPF, GRO, IPR, ...*/


/** @} */ /* end of AIOP_FMs_Verification */


/**************************************************************************//**
@Description	AIOP Terminate Flow Verification Command structure.

		This command ends the verification process.

*//***************************************************************************/
struct aiop_terminate_flow_command {
	uint32_t opcode;
		/**< AIOP Terminate Flow Verification command structure
		 * identifier. */
	uint8_t	pad[4];
		/**< 64-bit alignment. */
};


void aiop_verification();
void aiop_verification_fm();


/** @} */ /* end of AIOP_Verification */

#endif /* __AIOP_VERIFICATION_H_ */
