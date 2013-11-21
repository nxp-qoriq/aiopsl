/**************************************************************************//**
@File		aiop_verification_checksum.h

@Description	This file contains the AIOP Checksum SW Verification Structures

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_VERIFICATION_CHECKSUM_H_
#define __AIOP_VERIFICATION_CHECKSUM_H_

#include "fsl_ldpaa.h"

/**************************************************************************//**
 @addtogroup		AIOP_Service_Routines_Verification

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_CHECKSUM_SRs_Verification

 @Description	AIOP Checksum Verification structures definitions.

 @{
*//***************************************************************************/

/** \enum checksum_verif_cmd_type defines the parser verification CMDTYPE
 * field. */
enum checksum_verif_cmd_type {
	CKSUM_CALC_IPV4_HEADER_CHECKSUM_VERIF_CMDTYPE = 0,
	CKSUM_CALC_UDP_TCP_CHECKSUM_VERIF_CMDTYPE
};

#define CKSUM_CALC_IPV4_HEADER_CHECKSUM_STR \
		((CTLU_PARSE_CLASSIFY_ACCEL_ID << 16) | \
		CKSUM_CALC_IPV4_HEADER_CHECKSUM_VERIF_CMDTYPE)

#define CKSUM_CALC_UDP_TCP_CHECKSUM_STR \
		((CTLU_PARSE_CLASSIFY_ACCEL_ID << 16) | \
		CKSUM_CALC_UDP_TCP_CHECKSUM_VERIF_CMDTYPE)

/**************************************************************************//**
@Description	Calculate IPv4 Header Checksum Command structure.

		Includes information needed for Checksum Commands verification.
*//***************************************************************************/
struct cksum_calc_ipv4_header_checksum_command {
	uint32_t opcode;
	int32_t  status;
	uint32_t ipv4header;
};

/**************************************************************************//**
@Description	Calculate UDP TCP Checksum Command structure.

		Includes information needed for Checksum Commands verification.
*//***************************************************************************/
struct cksum_calc_udp_tcp_checksum_command {
	uint32_t opcode;
	int32_t  status;
};

uint16_t aiop_verification_checksum(uint32_t asa_seg_addr);

/** @} */ /* end of AIOP_CHECKSUM_SRs_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */


#endif /* __AIOP_VERIFICATION_CHECKSUM_H_ */
