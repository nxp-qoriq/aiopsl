/**************************************************************************//**
@File		fsl_ipv4_checksum.h

@Description	This file contains the AIOP SW Internet Protocol Version 4
		Checksum API.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#ifndef __FSL_IPV4_CHECKSUM_H
#define __FSL_IPV4_CHECKSUM_H

#include "common/types.h"
#include "net/fsl_net.h"

/**************************************************************************//**
@Group		NETF NETF (Network Libraries)

@Description	AIOP Accelerator APIs

@{
*//***************************************************************************/
/**************************************************************************//**
@Group		AIOP_IP IP

@Description	AIOP IP related header modifications

@{
*//***************************************************************************/
/**************************************************************************//**
@Group	FSL_IPV4_CKSUM IPV4 Checksum

@Description	Freescale AIOP Internet Protocol Version 4 Checksum API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_IPV4_CKSUM_MACROS IPv4 Checksum Macros

@Description	Freescale AIOP IPv4 Checksum Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_IPV4_CKSUM_CALC_OPTIONS Options for \ref ipv4_cksum_calculate \
	 function.
@{
*//***************************************************************************/

/** No options */
#define IPV4_CKSUM_CALC_OPTIONS_NONE				0x00000000

/** Update FDMA option
 * When this options is set, the SR will call \ref
 * fdma_modify_default_segment_data to update the FDMA engine with the frame
 * header changes. */
#define IPV4_CKSUM_CALC_OPTIONS_UPDATE_FDMA			0x00000001

/** @} */ /* end of FSL_IPV4_CKSUM_CALC_OPTIONS */

/**************************************************************************//**
@Group	FSL_IPV4_CKSUM_STATUS Status returned to calling function
@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_IPV4_CKSUM_CALC_STATUS Status returned from \
	ipv4_cksum_calculate SR
@{
*//***************************************************************************/
/** Success */
#define IPV4_CKSUM_CALC_STATUS_SUCCESS		0x00000000

/** Failure, FDMA error occurred */
#define IPV4_CKSUM_CALC_STATUS_FDMA_FAILURE	0x80010000

/** @} */ /* end of FSL_IPV4_CKSUM_CALC_STATUS */

/** @} */ /* end of FSL_IPV4_CKSUM_STATUS */

/** @} */ /* end of FSL_IPV4_CKSUM_MACROS */

/**************************************************************************//**
@Group		FSL_IPV4_CKSUM_Functions IPv4 Checksum Functions

@Description	Freescale AIOP IPv4 Checksum Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	ipv4_cksum_calculate

@Description	Calculates and updates IPv4 header checksum.

		This function calculates and updates IPv4 header checksum.
		The IPv4 header must reside entirely in the default segment
		(which must be open in the workspace).
		The contents of the header must be updated, if needed, by FDMA
		replace command before calling this function.

		Implicit input parameters in Task Defaults: Segment Address,
		Segment Offset and Frame Handle.

		Implicitly updated values in Task Defaults: Parse Result[gross
		running sum] field.

@Param[in]	ipv4header - pointer to ipv4 header.
@Param[in]	options - Please refer to \ref FSL_IPV4_CKSUM_CALC_OPTIONS

@Return		Please refer to \ref FSL_IPV4_CKSUM_CALC_STATUS.

@Cautions	In this function the task yields. \n
		This function invalidates the Parser Result Gross Running Sum
		field.
*//***************************************************************************/
int32_t ipv4_cksum_calculate(struct ipv4hdr *ipv4header, uint32_t options);

/** @} */ /* end of FSL_IPV4_CKSUM_Functions */
/** @} */ /* end of FSL_IPV4_CKSUM */
/** @} */ /* end of IP */
/** @} */ /* end of NETF */


#endif /* __FSL_IPV4_CHCEKSUM_H */
