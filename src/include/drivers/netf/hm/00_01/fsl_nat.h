/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		fsl_nat.h

@Description	This file contains the AIOP NAT Header Modification - NAT HM API

@Cautions	Please note that the parse results must be updated before
		calling functions defined in this file.

*//***************************************************************************/

#ifndef __FSL_NAT_H
#define __FSL_NAT_H

#include "fsl_types.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group		AIOP_NAT NAT

 @Description	AIOP NAT related API

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group		AIOP_NAT_HM NAT Header Modification

 @Description	AIOP NAT related header modifications API

 @{
*//***************************************************************************/

/** No IP or L4 (TCP/UDP) headers were found */
#define NO_L4_IP_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0800)

/**************************************************************************//**
@Group		HM_NAT_Modes HM NAT Modes

@Description	NAT Header Modification Modes

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		HMNATModeBits NAT header modification mode bits

@{
*//***************************************************************************/

	/** If set, update L4 checksum (if needed).*/
#define NAT_MODIFY_MODE_L4_CHECKSUM 0x01
	/** If set, the original IP Src address will be replaced.*/
#define NAT_MODIFY_MODE_IPSRC 0x02
	/** If set, the original IP Dst address will be replaced.*/
#define NAT_MODIFY_MODE_IPDST 0x04
	/** If set, the original L4 Src port will be replaced.*/
#define NAT_MODIFY_MODE_L4SRC 0x08
	/** If set, the original L4 Dst port will be replaced.*/
#define NAT_MODIFY_MODE_L4DST 0x10
	/** If set, the original acknowledgment number will be updated.
		The tcp_seq_num_delta signed integer will be added/subtracted
		to/from the SeqNum value.*/
#define NAT_MODIFY_MODE_TCP_SEQNUM 0x20
	/** If set, the original acknowledgment number will be updated.
		The tcp_seq_num_delta signed integer will be added/subtracted
		to/from the AckNum value.*/
#define NAT_MODIFY_MODE_TCP_ACKNUM 0x40

/* @} end of group HMNATModeBits */
/* @} end of group HM_NAT_Modes */

/**************************************************************************//**
@Group		FSL_HM_NAT_Functions HM NAT related functions

@Description	NAT related Header Modification functions

@{
*//***************************************************************************/


/*************************************************************************//**
@Function	nat_ipv4

@Description	Replace/update fields in the outer IPv4 and UDP/TCP headers
		(if exist).

		It automatically generates the IP checksum and optionally
		can update the UDP/TCP checksum.

		If the incoming frame contains TCP/UDP (the original UDP
		checksum!= 0), the UDP/TCP checksum is recalculated based
		on original checksum and the change in relevant header fields.

@Param[in]	flags - \link HMNATModeBits NAT modification mode bits
			\endlink

@Param[in]	ip_src_addr - Src address header to be replaced.
@Param[in]	ip_dst_addr - Dst address header to be replaced.
@Param[in]	l4_src_port - The Src port header to be replaced.
@Param[in]	l4_dst_port - The Dst port header to be replaced.
@Param[in]	tcp_seq_num_delta - This signed integer will be
		added/subtracted to/from the SeqNum value.
@Param[in]	tcp_ack_num_delta - This signed integer will be
		added/subtracted to/from the AckNum value.

@Return		Success or Failure.
		Failure in case:\n
		1. There was no IPv4 or L4 (TCP/UDP) header in the frame.\n
		2. in case HM_NAT_MODIFY_MODE_TCP_SEQNUM or
		HM_NAT_MODIFY_MODE_TCP_ACKNUM were set and no TCP was found.
		In this case all the rest of the fields except the above two
		will be replaced correctly and, if requested, the L4 checksum
		will be calculated.

@Cautions	The parse results must be updated before
		calling this operation.
*//***************************************************************************/
int nat_ipv4(uint8_t flags, uint32_t ip_src_addr,
		uint32_t ip_dst_addr, uint16_t l4_src_port,
		uint16_t l4_dst_port, int16_t tcp_seq_num_delta,
		int16_t tcp_ack_num_delta);


/*************************************************************************//**
@Function	nat_ipv6

@Description	Replace/update fields in the outer IPv6 and UDP/TCP headers
		(if exist).

		It automatically generates the IP checksum and optionally
		can update the UDP/TCP checksum.

		If the incoming frame contains TCP/UDP (the original UDP
		checksum!= 0), the UDP/TCP checksum is recalculated based
		on original checksum and the change in relevant header fields.

@Param[in]	flags - \link HMNATModeBits NAT modification mode bits
			\endlink

@Param[in]	ip_src_addr - Points to the IP Src address header to
		be replaced (in internal memory).
@Param[in]	ip_dst_addr - Points to the IP Dst address header to
		be replaced (in internal memory)
@Param[in]	l4_src_port - The Src port header to be replaced.
@Param[in]	l4_dst_port - The Dst port header to be replaced.
@Param[in]	tcp_seq_num_delta - This signed integer will be
		added/subtracted to/from the SeqNum value.
@Param[in]	tcp_ack_num_delta - This signed integer will be
		added/subtracted to/from the AckNum value.

@Return		Success or Failure (There was no IPv6/L4 header in the frame).

@Cautions	The parse results must be updated before
		calling this operation.
*//***************************************************************************/
int nat_ipv6(uint8_t flags, uint32_t *ip_src_addr,
		uint32_t *ip_dst_addr, uint16_t l4_src_port,
		uint16_t l4_dst_port, int16_t tcp_seq_num_delta,
		int16_t tcp_ack_num_delta);

/* @} end of group FSL_HM_NAT_Functions */
/* @} end of group AIOP_NAT_HM */
/* @} end of group AIOP_NAT */
/* @} end of group NETF */


#endif /* __FSL_NAT_H */
