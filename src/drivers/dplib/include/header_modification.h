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
@File		header_modification.h

@Description	This file contains the AIOP Header Modification internal API

*//***************************************************************************/

#ifndef __HEADER_MODIFICATION_H
#define __HEADER_MODIFICATION_H

#define VXLAN_VID_MASK		0xFFFFFF00
#define VXLAN_SIZE		8
#define VXLAN_VID_SHIFT		8
#define VLAN_VID_MASK		0x00000FFF
#define VLAN_PCP_MASK		0x0000E000
#define VLAN_PCP_SHIFT		13
#define VLAN_SIZE		4
#define NO_OFFSET_FOUND		0xFF
#define IPV6_ECN_MASK		0xFFCFFFFF
#define IPV4_ECN_MASK		0xFC
#define MIN_REPRESENT_SIZE	128
#define TCP_DATA_OFFSET_SHIFT	2
#define TCP_NO_OPTION_SIZE	20
#define IPV6_ADDR_SIZE		32
#define IPV4_PROTOCOL_ID	0x04
#define IPV6_PROTOCOL_ID	0x29


#define IPV4_DSCP_MASK		0x03
#define IPV4_DF_MASK		0xbfff
#define IPV6_DSCP_MASK		0xF03FFFFF
#define ETYPE_IPV4		0x0800
#define ETYPE_IPV6		0x86DD
#define ETYPE_SIZE		2
#define MPLS_LABEL_MASK		0x00001fff
#define MPLS_LABEL_IPV4		0x00000000
#define MPLS_LABEL_IPV6		0x00002000
#define MPLS_SIZE		4
#define IPV6_TC_MASK		0xf00fffff
#define IPV6_FLOW_MASK		0xfff00000

/* ARP definitions */
#define ARPHDR_ETHER_PRO_TYPE	1
#define ARPHDR_IPV4_PRO_TYPE	0x800
#define ARPHDR_ETH_HDR_LEN	14
#define ARPHDR_IPV4_ADDR_LEN	4
#define	ARP_REQUEST_OP		1
#define	ARP_REPLY_OP		2
#define ARP_PKT_MIN_LEN 	64
#define ARP_HDR_LEN		28
#define BROADCAST_MAC		0xFFFFFFFFFFFF
#define ARP_ETHERTYPE		0x0806


#define PARSER_CLEAR_RUNNING_SUM() \
(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->gross_running_sum) = 0


#endif /* __HEADER_MODIFICATION_H */
