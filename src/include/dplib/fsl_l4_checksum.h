/**************************************************************************//**
@File		fsl_l4_checksum.h

@Description	This file contains the AIOP SW Layer 4 Checksum API.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#ifndef __FSL_L4_CHECKSUM_H
#define __FSL_L4_CHECKSUM_H

#include "common/types.h"


/**************************************************************************//**
@Group	FSL_L4_CKSUM FSL_AIOP_L4_Checksum

@Description	Freescale AIOP Layer 4 Checksum API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_L4_CKSUM_MACROS L4 Checksum Macros

@Description	Freescale AIOP Layer 4 Checksum Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_L4_CKSUM_STATUS Status returned to calling function
@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_L4_CKSUM_STATUS_CALC_UDP_TCP Status returned from \
	 \ref l4_cksum_calc_udp_tcp_checksum SR
@{
*//***************************************************************************/
/** Success */
#define L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_SUCCESS		0x00000000

/** Default frame is not UDP/TCP (according to parser result) */
#define L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_NON_UDP_TCP		0x00000001

/** Failure, FDMA error occurred */
#define L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_FDMA_FAILURE		0x80010000

/** Failure, Parser error occurred */
#define L4_CKSUM_CALC_UDP_TCP_CKSUM_STATUS_PARSER_FAILURE	0x80020000

/** @} */ /* end of FSL_L4_CKSUM_STATUS_CALC_UDP_TCP */

/** @} */ /* end of FSL_L4_CKSUM_STATUS */

/** @} */ /* end of FSL_L4_CKSUM_MACROS */

/**************************************************************************//**
@Group		FSL_L4_CKSUM_Functions L4 Checksum Functions

@Description	Freescale AIOP Layer 4 Checksum Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	l4_cksum_calc_udp_tcp_checksum

@Description	Calculates and updates frame's UDP/TCP checksum.

		The UDP/TCP header must reside entirely in the default segment
		(which must be open in the workspace).

		Checksum field is always updated (also when UDP[checksum]
		field is zero), unless an error occurred.

		Implicit input parameters in Task Defaults: Segment Address,
		Segment Offset, Frame Handle, Parser Result, Parser Profile ID
		and Starting HXS.

		Implicitly updated values in Task Defaults: Parse Result.

@Param[in]	flags - Flags for this function. Please refer TODO

@Return		Please refer to \ref FSL_L4_CKSUM_STATUS_CALC_UDP_TCP.

@Cautions	In this function the task yields. \n
		Parse Result (excluding Gross Running Sum field) must be valid.
		If Parse Result[Gross Running Sum] field is not valid a
		significant performance degradation is expected.
		This function invalidates the Parser Result Gross Running Sum
		field.
*//***************************************************************************/
int32_t cksum_calc_udp_tcp_checksum(uint32_t flags);

/** @} */ /* end of FSL_L4_CKSUM_Functions */
/** @} */ /* end of FSL_L4_CKSUM */


#endif /* __FSL_L4_CHCEKSUM_H */
