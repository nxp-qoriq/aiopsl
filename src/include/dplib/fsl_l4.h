/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
@File		fsl_l4.h

@Description	This file contains layer 4 functions (L4 API).

@Cautions	Please note that the parse results must be updated before
		calling functions defined in this file.

*//***************************************************************************/

#ifndef __FSL_LIB_L4_H
#define __FSL_LIB_L4_H

#include "common/types.h"
#include "dplib/fsl_gso.h"
#include "dplib/fsl_gro.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group		AIOP_L4 L4

 @Description	AIOP L4 related API

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group		AIOP_L4_HM L4 Header Modifications

 @Description	AIOP L4 related header modifications API

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		HM_L4_Modes HM L4 Modes

@Description	L4 Header Modification Modes

@{
*//***************************************************************************/

/** The frame not contain a UDP header */
#define NO_UDP_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0200)
/** No TCP header was found */
#define NO_TCP_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0500)
/** No TCP MSS Option was found */
#define NO_TCP_MSS_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0600)
/** No TCP or UDP headers were found */
#define NO_L4_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0700)
/* TODO Ask Hezi if this define should be changed */

/**************************************************************************//**
@Group		HMUDPModeBits UDP header modification mode bits

@{
*//***************************************************************************/

	/** If set, update L4 checksum.*/
#define L4_UDP_MODIFY_MODE_L4_CHECKSUM 0x01
	/** If set, the original UDP Src port will be replaced.*/
#define L4_UDP_MODIFY_MODE_UDPSRC 0x02
	/** If set, the original UDP Dst port will be replaced.*/
#define L4_UDP_MODIFY_MODE_UDPDST 0x04

/* @} end of group HMUDPModeBits */


/**************************************************************************//**
@Group		HMTCPModeBits TCP header modification mode bits

@{
*//***************************************************************************/

	/** If set, update L4 checksum.*/
#define L4_TCP_MODIFY_MODE_L4_CHECKSUM 0x01
	/** If set, the original TCP Src port will be replaced.*/
#define L4_TCP_MODIFY_MODE_TCPSRC 0x02
	/** If set, the original TCP Dst port will be replaced.*/
#define L4_TCP_MODIFY_MODE_TCPDST 0x04
	/** If set, the original acknowledgment number will be updated.
		The tcp_seq_num_delta signed integer will be added/subtracted
		to/from the SeqNum value.*/
#define L4_TCP_MODIFY_MODE_SEQNUM 0x08
	/** If set, the original acknowledgment number will be updated.
		The tcp_seq_num_delta signed integer will be added/subtracted
		to/from the AckNum value.*/
#define L4_TCP_MODIFY_MODE_ACKNUM 0x10
	/** If set, the original maximum segment size will be replaced.*/
#define L4_TCP_MODIFY_MODE_MSS 0x20

/* @} end of group HMTCPModeBits */


/**************************************************************************//**
@Group		HML4UDPTCPCksumCalcModeBits L4 UDP TCP Checksum Calculation \
		 mode bits

@{
*//***************************************************************************/

	/** No Mode bits */
#define L4_UDP_TCP_CKSUM_CALC_MODE_NONE			0x00

	/** Don't Update FDMA mode bit
	 * If set, the SR will not call \ref fdma_modify_default_segment_data
	 * to update the FDMA engine with the frame header changes. */
#define L4_UDP_TCP_CKSUM_CALC_MODE_DONT_UPDATE_FDMA	0x01

/* @} end of group HML4UDPTCPCksumCalcModeBits */
/* @} end of group HM_L4_Modes */

/**************************************************************************//**
@Group		FSL_HM_L4_Functions HM L4 related functions

@Description	L4 related Header Modification functions

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	l4_udp_header_modification

@Description	Replace addresses in the UDP header (if exist) of
		a frame. It optionally can update the UDP checksum.

		If the original UDP checksum!= 0, the UDP checksum is
		recalculated based on original checksum and the change in
		relevant header fields. In case the UDP checksum == 0 the
		checksum will be calculated from scratch (a costly operation).


@Param[in]	flags - \link HMUDPModeBits UDP modification mode bits
			\endlink

@Param[in]	udp_src_port - The Src port header to be replaced.
@Param[in]	udp_dst_port - The Dst port header to be replaced.

@Return		Success or Failure (There was no UDP header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.
*//***************************************************************************/
int l4_udp_header_modification(uint8_t flags,
		uint16_t udp_src_port, uint16_t udp_dst_port);


/*************************************************************************//**
@Function	l4_tcp_header_modification

@Description	Replace fields in the TCP header (if exist) of a frame.

		The TCP checksum is recalculated based on original checksum
		and the change in relevant header fields.

		This function takes care of wrapping around of seq/ack numbers.

@Param[in]	flags - \link HMTCPModeBits TCP modification mode bits
			\endlink

@Param[in]	tcp_src_port - The Src port header to be replaced.
@Param[in]	tcp_dst_port - The Dst port header to be replaced.
@Param[in]	tcp_seq_num_delta - This signed integer will be
		added/subtracted to/from the SeqNum value.
@Param[in]	tcp_ack_num_delta - This signed integer will be
		added/subtracted to/from the AckNum value.
@Param[in]	tcp_mss - The MSS header to be replaced.

@Return		Success or Failure
		Failure in case:\n
		1. There was no TCP header in the frame.\n
		2. in case the MSS was needed to be replaced and no MSS was
		found an error will be returned. In this case all the rest of
		the fields except the MSS will be replaced correctly and, if
		requested, the TCP checksum will be calculated.

@Cautions	The parse results must be updated before
		calling this operation.
*//***************************************************************************/
int l4_tcp_header_modification(uint8_t flags, uint16_t tcp_src_port,
		uint16_t tcp_dst_port, int16_t tcp_seq_num_delta,
		int16_t tcp_ack_num_delta, uint16_t tcp_mss);

/*************************************************************************//**
@Function	l4_set_tp_src

@Description	Replace TCP/UDP source port. The UDP/TCP checksum is updated
		automatically.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	src_port - The new TCP/UDP source port.

@Return		Success or Failure (There was no TCP/UDP header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.\n
		This function assumes the original TCP header checksum is valid.
*//***************************************************************************/
int l4_set_tp_src(uint16_t src_port);


/*************************************************************************//**
@Function	l4_set_tp_dst

@Description	Replace TCP/UDP destination port. The UDP/TCP checksum is
		updated automatically.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	dst_port - The new TCP/UDP destination port.

@Return		Success or Failure (There was no TCP/UDP header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.\n
		This function assumes the original TCP header checksum is valid.

*//***************************************************************************/
int l4_set_tp_dst(uint16_t dst_port);

/*************************************************************************//**
@Function	l4_set_tcp_src

@Description	Replace TCP source port. The TCP checksum is updated
		automatically.
		This function assumes the presence of TCP header.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	src_port - The new TCP source port.

@Return		None.

@Cautions	The parse results must be updated before calling this operation.\n
		This function assumes the original TCP header checksum is valid.
*//***************************************************************************/
void l4_set_tcp_src(uint16_t src_port);

/*************************************************************************//**
@Function	l4_set_tcp_dst

@Description	Replace TCP destination port. The TCP checksum is updated
		automatically.
		This function assumes the presence of TCP header.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	dst_port - The new TCP destination port.

@Return		None.

@Cautions	The parse results must be updated before calling this operation.\n
		This function assumes the original TCP header checksum is valid.
*//***************************************************************************/
void l4_set_tcp_dst(uint16_t dst_port);

/*************************************************************************//**
@Function	l4_set_udp_src

@Description	Replace UDP source port. The UDP checksum is updated
		automatically.
		This function assumes the presence of UDP header.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	src_port - The new UDP source port.

@Return		None.

@Cautions	The parse results must be updated before calling this operation.\n
		This function assumes the original UDP header checksum is valid.
		In case that UDP checksum == 0, the checksum will not update.
*//***************************************************************************/
void l4_set_udp_src(uint16_t src_port);

/*************************************************************************//**
@Function	l4_set_udp_dst

@Description	Replace UDP destination port. The UDP checksum is updated
		automatically.
		This function assumes the presence of UDP header.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	dst_port - The new UDP destination port.

@Return		None.

@Cautions	The parse results must be updated before calling this operation.\n
		This function assumes the original UDP header checksum is valid.
		In case that UDP checksum == 0, the checksum will not update.
*//***************************************************************************/
void l4_set_udp_dst(uint16_t dst_port);

/**************************************************************************//**
@Function	l4_udp_tcp_cksum_calc

@Description	Calculates and updates frame's UDP/TCP checksum.

		The UDP/TCP header must reside entirely in the default segment
		(which must be open in the workspace).

		Checksum field is always updated (also when UDP[checksum]
		field is zero), unless an error occurred.

		Implicit input parameters in Task Defaults: Segment Address,
		Segment Offset, Frame Handle, Parser Result, Parser Profile ID
		and Starting HXS.

		Implicitly updated values in Task Defaults: Parse Result.

@Param[in]	flags - Please refer to \ref HML4UDPTCPCksumCalcModeBits

@Return		Success, FDMA failure or Parser failure.

@Cautions	In this function the task yields. \n
		Parse Result (excluding Gross Running Sum field) must be valid.
		If Parse Result[Gross Running Sum] field is not valid a
		significant performance degradation is expected.
		This function invalidates the Parser Result Gross Running Sum
		field.
*//***************************************************************************/
int l4_udp_tcp_cksum_calc(uint8_t flags);
/* @} end of group FSL_HM_L4_Functions */
/* @} end of group AIOP_L4_HM */
/* @} end of group AIOP_L4 */
/* @} end of group NETF */


#endif /* __FSL_LIB_L4_H */
