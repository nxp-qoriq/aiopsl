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
@File		net.h

@Description	This file contains the AIOP Header Modification internal API

*//***************************************************************************/

#ifndef __NET_H
#define __NET_H

/* MASK */
#define VLAN_VID_MASK		0x00000FFF
#define VLAN_PCP_MASK		0x0000E000
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


#endif /* __HEADER_MODIFICATION_H */
