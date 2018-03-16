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
@File		net.h

@Description	This file contains the AIOP Header Modification internal API

*//***************************************************************************/

#ifndef __NET_H
#define __NET_H

#include "fsl_net.h"

/* MASK */
#define VLAN_VID_MASK		0x00000FFF
#define VLAN_PCP_MASK		0x0000E000
#define VLAN_TPID_MASK		0xFFFF0000
#define VLAN_CFI_MASK		0x00001000
#define VXLAN_VID_MASK		0xFFFFFF00
#define MPLS_LABEL_MASK		0x00001fff
#define IPV4_ECN_MASK		0xFC
#define IPV4_DSCP_MASK		0x03
#define IPV4_DF_MASK		0xbfff
#define IPV6_DSCP_MASK		0xF03FFFFF
#define IPV6_FLOW_MASK		0xfff00000
#define IPV6_TC_MASK		0xf00fffff
#define IPV6_ECN_MASK		0xFFCFFFFF

/* Shift */
#define VXLAN_VID_SHIFT			8
#define VLAN_PCP_SHIFT			13
#define TCP_DATA_OFFSET_SHIFT	2

/* Size */
#define TCP_NO_OPTION_SIZE	20
#define IPV6_ADDR_SIZE		32
#define ETYPE_SIZE			2

/* ARP definitions */
#define ARPHDR_IPV4_ADDR_LEN	4
#define	ARP_REQUEST_OP			1
#define	ARP_REPLY_OP			2
#define ARP_PKT_MIN_LEN 		64
#define ARP_HDR_LEN				28
#define BROADCAST_MAC			0xFFFFFFFFFFFF

#define PARSER_CLEAR_RUNNING_SUM() \
(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->gross_running_sum) = 0

/**************************************************************************//**
@Group		IP_INTERNAL  IP Internal

@Description	IP Internal

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		AIOP_General_Protocols_IPV6_HDR_Offsets IPv6 Header Offsets
@{
*//***************************************************************************/
#define  IPV6_NEXT_HDR_OFFSET 6 /*!< IPv6 next header offset*/

/** @} */ /* end of AIOP_General_Protocols_IPV6_HDR_Offsets */


/**************************************************************************//**
@Group		AIOP_General_MASKS IPv6 Last Header MASKS
@{
*//***************************************************************************/
#define  IPV6_NO_EXTENSION 0x80000000 /*!< IPv6 no extension mask*/

/** @} */ /* end of AIOP_General_MASKS */

/**************************************************************************//**
@Group		IPV6_LAST_HEADER_Definitions ipv6_last_header flags defines
@{
*//***************************************************************************/
#define  LAST_HEADER_BEFORE_FRAG 0 /*!< last header offset before frag*/
#define  LAST_HEADER 1 /*!< last header offset*/

/** @} */ /* end of IPV6_LAST_HEADER_Definitions */

/**************************************************************************//**
 @Group		IPv4_TS_OPT_Getters_Setters

 @Description	IPv4 time-stamp option Getters/Setters

 @{
*//***************************************************************************/

#define TS_OPT_GET_FIRST_WORD() *((uint32_t *)ip_opt_ptr)
#define TS_OPT_GET_LENGTH() *(ip_opt_ptr + 1)
#define TS_OPT_GET_PTR() *(ip_opt_ptr + 2)
#define TS_OPT_GET_OVRFLOW_FLAG() *(ip_opt_ptr + 3)
#define TS_OPT_SET_PTR(var) \
do { \
	ptr_next_ts += var; \
	*(ip_opt_ptr + 2) = ptr_next_ts; \
} while (0)
#define TS_OPT_SET_OVRFLOW_FLAG() *(ip_opt_ptr + 3) = overflow_flag

#define TS_OPT_GET_FLAG() overflow_flag & 0xf
#define AIOP_IPOPT_TS_TSONLY    0  /* timestamps only */
#define AIOP_IPOPT_TS_TSANDADDR 1  /* timestamps and addresses */
#define AIOP_IPOPT_TS_PRESPEC   3  /* specified modules only */

/** @} */ /* end of group IPv4_TS_OPT_Getters_Setters */

/**************************************************************************//**
@Group		IP_INTERNAL_Functions IP Internal Functions

@Description	IP Internal Functions

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	ipv6_last_header

@Description	Return the pointer to fragment or last extension IPv6 header.

@Param[in]	ipv6_hdr - pointer to IPv6 header
@Param[in]	flag - (\ref IPV6_LAST_HEADER_Definitions).

@Return		Pointer to next header of last extension before fragment or last
		IPv6 extension pointer (depend on flag).
		The MSB bit indicate whether there is a extension
		(1=no extension)

@Cautions	The function assume a valid IPv6 frame
*//***************************************************************************/
uint32_t ipv6_last_header(struct ipv6hdr *ipv6_hdr, uint8_t flag);

/** @} */ /* end of IP_INTERNAL_Functions */

/** @} */ /* end of IP_INTERNAL */

#endif /* __HEADER_MODIFICATION_H */
