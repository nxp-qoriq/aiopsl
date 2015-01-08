/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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
