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
@File		fsl_frame_operations.h

@Description	This file contains the AIOP SW Frame Operations API

*//***************************************************************************/

#ifndef __FSL_FRAME_OPERATIONS_H
#define __FSL_FRAME_OPERATIONS_H

#include "common/types.h"
#include "fsl_errors.h"
#include "dplib/fsl_ldpaa.h"

/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_NETF_GENERAL GENERAL

 @Description	AIOP NETF general API

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Group		FSL_AIOP_FRAME_OPERATIONS AIOP Frame Operations

 @Description	FSL AIOP Frame Operations

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		FRAME_OPERSTIONS_Functions Frame Operations Functions

 @Description	Frame Operations Functions

 @{
*//***************************************************************************/

/**************************************************************************//**
@Function	create_frame

@Description	Create a frame from scratch and fill it with user specified
		data.

		Implicit input parameters in Task Defaults: SPID (Storage
		Profile ID), task default AMQ attributes (ICID, PL, VA, BDI).

		Implicitly updated values in Task Defaults in case the FD
		address is located in the default FD address
		(\ref HWC_FD_ADDRESS): ASA size(zeroed), PTA address(zeroed),
		segment length(zeroed), segment offset(zeroed), segment handle,
		NDS bit(reset), frame handle.

		In case this is the default frame, a default segment will be
		presented, task defaults segment address and
		segment length(max(size, \ref DEFAULT_SEGMENT_SIZE)) will
		be implicitly updated, and the parse results will be updated.

		In case this is not the default frame, in order to present a
		data segment of this frame after the function returns,
		fdma_present_frame_segment() should be called (opens a
		data segment of the frame).

@Param[in]	fd - Pointer to the workspace location of the frame descriptor
		of the created frame \ref ldpaa_fd.
		On a success return this pointer will point to a valid FD.
		The FD address in workspace must be aligned to 32 bytes.
@Param[in]	data - A pointer to the workspace data to be inserted to the
		frame.
@Param[in]	size - data size.
@Param[out]	frame_handle - Pointer to the opened working frame handle.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EIO - Parsing Error(Relevant in case this is the default frame).
		Recommendation is to discard the frame.
@Retval		ENOSPC - Block Limit Exceeds (Frame Parsing reached the limit
		of 256 bytes before completing all parsing). (Relevant in case
		this is the default frame).
		Recommendation is to discard the frame.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@Cautions	Due to PDM TKT254401 this function can work only on the default 
		frame.
@Cautions	In this Service Routine the task yields.
@Cautions	The FD address in workspace must be aligned to 32 bytes.
@Cautions	The frame FD is overwritten in this function.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int create_frame(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size,
		uint8_t *frame_handle);

/**************************************************************************//**
@Function	create_fd

@Description	Create a frame from scratch and fill it with user specified
		data.

		After filling the frame, it will be closed (i.e. - The working
		frame will be closed and the FD will be updated in workspace).

		Implicit input parameters in Task Defaults: SPID (Storage
		Profile ID), task default AMQ attributes (ICID, PL, VA, BDI).

		Implicitly updated values in Task Defaults in case the FD
		address is located in the default FD address
		(\ref HWC_FD_ADDRESS): ASA size(zeroed), PTA address(zeroed),
		segment length(zeroed), segment offset(zeroed), NDS bit(reset).

		In case this is the default frame, in order to present a data
		segment of this frame after the function returns, the
		presentation context values have to be modified prior to calling
		fdma_present_default_frame() (opens the default frame and
		optionally present a segment).

		In case this is not the default frame, in order to present a
		data segment of this frame after the function returns,
		fdma_present_frame() should be called (opens the frame and
		optionally present a segment).

@Param[in]	fd - Pointer to the workspace location of the frame descriptor
		of the created frame \ref ldpaa_fd.
		On a success return this pointer will point to a valid FD.
		The FD address in workspace must be aligned to 32 bytes.
@Param[in]	data - A pointer to the workspace data to be inserted to the
		frame.
@Param[in]	size - data size.
@Param[in]	spid - Storage profile ID to be used in case this is not the 
		default frame.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success.
@Retval		ENOMEM - Failed due to buffer pool depletion.

@remark		FD is updated.

@Cautions	Due to PDM TKT254401 this function can work only on the default 
		frame.
@Cautions	In this Service Routine the task yields.
@Cautions	The FD address in workspace must be aligned to 32 bytes.
@Cautions	The frame FD is overwritten in this function.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int create_fd(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size,
		uint8_t spid);

/**************************************************************************//**
@Function	create_arp_request_broadcast

@Description	Create an ARP Request broadcast frame from scratch. This
		function creates a new frame with ETH + ARP Request headers.

		Implicit input parameters in Task Defaults: SPID (Storage
		Profile ID), task default AMQ attributes (ICID, PL, VA, BDI).

		Implicitly updated values in Task Defaults in case the FD
		address is located in the default FD address
		(\ref HWC_FD_ADDRESS): ASA size(zeroed), PTA address(zeroed),
		segment length(zeroed), segment offset(zeroed), segment handle,
		NDS bit(reset), frame handle.

		In case this is the default frame, a default segment will be
		presented, and the parse results will be updated.

		In case this is not the default frame, in order to present a
		data segment of this frame after the function returns,
		fdma_present_frame_segment() should be called (opens a
		data segment of the frame).

@Param[in]	fd - Pointer to the workspace location of the frame descriptor
		of the created frame \ref ldpaa_fd.
		On a success return this pointer will point to a valid FD.
		The FD address in workspace must be aligned to 32 bytes.
@Param[in]	local_ip - local IPv4 address.
@Param[in]	target_ip - destination IPv4 address.
@Param[out]	frame_handle - Pointer to the opened working frame handle.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EIO - Parsing Error(Relevant in case this is the default frame).
		Recommendation is to discard the frame.
@Retval		ENOSPC - Block Limit Exceeds (Frame Parsing reached the limit
		of 256 bytes before completing all parsing). (Relevant in case
		this is the default frame).
		Recommendation is to discard the frame.

@Cautions	Due to PDM TKT254401 this function can work only on the default 
		frame.
@Cautions	In this Service Routine the task yields.
@Cautions	The FD address in workspace must be aligned to 32 bytes.
@Cautions	The frame FD is overwritten in this function.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int create_arp_request_broadcast(
		struct ldpaa_fd *fd,
		uint32_t local_ip,
		uint32_t target_ip,
		uint8_t *frame_handle);

/**************************************************************************//**
@Function	create_arp_request

@Description	Create an ARP Request frame from scratch. This function creates
		a new frame with ETH + ARP Request headers. It can be used for
		creating a unicast or multicast frames.

		Implicit input parameters in Task Defaults: SPID (Storage
		Profile ID), task default AMQ attributes (ICID, PL, VA, BDI).

		Implicitly updated values in Task Defaults in case the FD
		address is located in the default FD address
		(\ref HWC_FD_ADDRESS): ASA size(zeroed), PTA address(zeroed),
		segment length(zeroed), segment offset(zeroed), segment handle,
		NDS bit(reset), frame handle.

		In case this is the default frame, a default segment will be
		presented, and the parse results will be updated.

		In case this is not the default frame, in order to present a
		data segment of this frame after the function returns,
		fdma_present_frame_segment() should be called (opens a
		data segment of the frame).

@Param[in]	fd - Pointer to the workspace location of the frame descriptor
		of the created frame \ref ldpaa_fd.
		On a success return this pointer will point to a valid FD.
		The FD address in workspace must be aligned to 32 bytes.
@Param[in]	local_ip - local IPv4 address.
@Param[in]	target_ip - destination IPv4 address.
@Param[in]	target_eth - target MAC address.
@Param[out]	frame_handle - Pointer to the opened working frame handle.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EIO - Parsing Error(Relevant in case this is the default frame).
		Recommendation is to discard the frame.
@Retval		ENOSPC - Block Limit Exceeds (Frame Parsing reached the limit
		of 256 bytes before completing all parsing). (Relevant in case
		this is the default frame).
		Recommendation is to discard the frame.

@Cautions	Due to PDM TKT254401 this function can work only on the default 
		frame.
@Cautions	In this Service Routine the task yields.
@Cautions	The FD address in workspace must be aligned to 32 bytes.
@Cautions	The frame FD is overwritten in this function.
@Cautions	This function may result in a fatal error.
*//***************************************************************************/
int create_arp_request(
		struct ldpaa_fd *fd,
		uint32_t local_ip,
		uint32_t target_ip,
		uint8_t *target_eth,
		uint8_t *frame_handle);

/** @} end of group FRAME_OPERSTIONS_Functions */

/** @} */ /* end of FSL_AIOP_FRAME_OPERATIONS */

/** @} */ /* end of AIOP_NETF_GENERAL */

/** @} */ /* end of NETF */


#endif /* __FSL_FRAME_OPERATIONS_H */
