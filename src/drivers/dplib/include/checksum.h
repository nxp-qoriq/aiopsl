/**************************************************************************//**
@File		checksum.h

@Description	This file contains checksum utilities

*//***************************************************************************/

#ifndef __CHECKSUM_H
#define __CHECKSUM_H

#include "common/types.h"

/**************************************************************************//**
@Group	CKSUM Checksum (Internal)

@Description	AIOP Checksum utilities

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		CKSUM_Functions Checksum Functions

@Description	AIOP Checksum Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	cksum_ones_complement_sum16

@Description	Calculates a 1's complement sum of two 16 bit arguments.

@Param[in]	arg1 - first argument.

@Param[in]	arg2 - second argument.

@Return		1's complement sum of the two 16 bit arguments.

@Cautions	None
*//***************************************************************************/
uint16_t cksum_ones_complement_sum16(uint16_t arg1, uint16_t arg2);

/**************************************************************************//**
@Function	cksum_ones_complement_dec16

@Description	Calculates a 1's complement subtraction of two 16 bit arguments.

@Param[in]	arg1 - first argument.

@Param[in]	arg2 - second argument to be subtracted from the first argument.

@Return		1's complement subtraction of the two 16 bit arguments.

@Cautions	None
*//***************************************************************************/
uint16_t cksum_ones_complement_dec16(uint16_t arg1, uint16_t arg2);

/**************************************************************************//**
@Function	cksum_update_uint32

@Description	Updates the IPv4/UDP/TCP CS after updating 4 consecutive
		bytes in the IPv4/UDP/TCP header. The update is being done
		directly to the workspace memory.

@Param[in]	cs_ptr - Pointer to the IPv4/UDP/TCP CheckSum.

@Param[in]	old_val - The original value of the 4 bytes changed in the
		header.

@Param[in]	new_val - The new value of the 4 bytes changed in the header.

@Return		None.

@Cautions	Replace of the segment is not performed in this SR. In order
		to update the frame in external memory there is a need to use
		FDMA SRs.
*//***************************************************************************/
void cksum_update_uint32(register uint16_t *cs_ptr,
		register uint32_t old_val,
		register uint32_t new_val);

/**************************************************************************//**
@Function	cksum_accumulative_update_uint32

@Description	Updates the IPv4/UDP/TCP CS after updating 4 consecutive
		bytes in the IPv4/UDP/TCP header. The update is being done
		to the cksum parameter.

@Param[in]	cksum - The initial checksum value to be updated.

@Param[in]	old_val - The original value of the 4 bytes changed in the
		header.

@Param[in]	new_val - The new value of the 4 bytes changed in the header.

@Return		The updated cksum value.

@Cautions	None.
*//***************************************************************************/
uint16_t cksum_accumulative_update_uint32(register uint16_t cksum,
		register uint32_t old_val,
		register uint32_t new_val);

/** @} */ /* end of CKSUM_Functions */

/** @} */ /* end of CKSUM */

#endif /* __CHECKSUM_H */
