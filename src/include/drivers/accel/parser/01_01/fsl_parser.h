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
@File          fsl_parser.h

@Description   This file contains the AIOP SW PARSER API

*//***************************************************************************/

#ifndef __FSL_PARSER_H
#define __FSL_PARSER_H

#include "fsl_types.h"
#include "fsl_ldpaa.h"

extern __TASK struct aiop_default_task_params default_task_params;

/**************************************************************************//**
 @Group		ACCEL Accelerators APIs

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		FSL_PARSER PARSER

@Description	Freescale AIOP PARSER API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_PARSER_MACROS PARSER Macros

@Description	Freescale AIOP PARSER Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_PARSER_FRAME_ATTRIBUTES_EXTENSION_MASKS Frame Attributes Extension Masks

@Description	Frame Attributes Extension Masks
		Used with \ref parse_result frame_attribute_flags_extension
@{
*//***************************************************************************/
	/** Routing header present in 2nd IPv6 header */
#define PARSER_ATT_IPV6_ROUTING_HDR_2               0x8000

/** @} *//* end of FSL_PARSER_FRAME_ATTRIBUTES_EXTENSION_MASKS */

/**************************************************************************//**
@Group	FSL_PARSER_FRAME_ATTRIBUTES_PART_1_MASKS Frame Attributes Masks 1

@Description	Frame Attributes Part 1 Masks
		Used with \ref parse_result frame_attribute_flags_1 field
@{
*//***************************************************************************/
	/** "Ethernet MAC" mask for frame_attribute_flags_1 */
#define PARSER_ATT_ETH_MAC_MASK                     0x00200000
	/** "Ethernet MAC Unicast DA" mask for frame_attribute_flags_1 */
#define PARSER_ATT_ETH_MAC_UNICAST_MASK             0x00100000
	/** "Ethernet MAC Multicast DA" mask for frame_attribute_flags_1 */
#define PARSER_ATT_ETH_MAC_MULTICAST_MASK           0x00080000
	/** "Ethernet MAC Broadcast DA" mask for frame_attribute_flags_1 */
#define PARSER_ATT_ETH_MAC_BROADCAST_MASK           0x00040000
	/** "BPDU" mask for frame_attribute_flags_1 */
#define PARSER_ATT_BPDU_MASK                        0x00020000
	/** "FCOE" mask for frame_attribute_flags_1 */
#define PARSER_ATT_FCOE_MASK                        0x00010000
	/** "FCOE Initial Protocol" mask for frame_attribute_flags_1 */
#define PARSER_ATT_FCOE_INIT_PROTOCOL_MASK          0x00008000
	/** "LLC+SNAP" mask for frame_attribute_flags_1 */
#define PARSER_ATT_LLC_SNAP_MASK                    0x00002000
	/** "Unknown LLC/OUI" mask for frame_attribute_flags_1 */
#define PARSER_ATT_UNKOWN_LLC_OUI_MASK              0x00001000
	/** "One VLAN" mask for frame_attribute_flags_1 */
#define PARSER_ATT_VLAN_1_MASK                      0x00000400
	/** "More than one VLAN" mask for frame_attribute_flags_1 */
#define PARSER_ATT_VLAN_N_MASK                      0x00000200
	/** "CFI in VLAN" mask for frame_attribute_flags_1 */
#define PARSER_ATT_CFI_IN_VLAN_MASK                 0x00000100
	/** "PPPOE+PPP" mask for frame_attribute_flags_1 */
#define PARSER_ATT_PPPOE_PPP_MASK                   0x00000040
	/** "One MPLS" mask for frame_attribute_flags_1 */
#define PARSER_ATT_MPLS_1_MASK                      0x00000010
	/** "More than one MPLS" mask for frame_attribute_flags_1 */
#define PARSER_ATT_MPLS_N_MASK                      0x00000008
	/** "ARP" mask for frame_attribute_flags_1 */
#define PARSER_ATT_ARP_MASK                         0x00000002

/** @} *//* end of FSL_PARSER_FRAME_ATTRIBUTES_PART_1_MASKS */

/**************************************************************************//**
@Group	FSL_PARSER_FRAME_ATT_ERROR_PART_1_MASKS Frame Attributes Error Masks 1

@Description	Frame Attribute Errors Part 1 Masks
		Used with \ref parse_result frame_attribute_flags_1 field
@{
*//***************************************************************************/

	/** "Soft parsing error in shim" mask for frame_attribute_flags_1 */
#define PARSER_ATT_SHIM_SOFT_PARSING_ERROR_MASK     0x00800000
	/** "Parsing error" mask for frame_attribute_flags_1 */
#define PARSER_ATT_PARSING_ERROR_MASK               0x00400000
	/** "Ethernet parsing error" mask for frame_attribute_flags_1 */
#define PARSER_ATT_ETH_PARSING_ERROR_MASK           0x00004000
	/** "LLC+SNAP parsing error" mask for frame_attribute_flags_1 */
#define PARSER_ATT_LLC_SNAP_PARSING_ERROR_MASK      0x00000800
	/** "VLAN parsing error" mask for frame_attribute_flags_1 */
#define PARSER_ATT_VLAN_PARSING_ERROR_MASK          0x00000080
	/** "PPPOE+PPP parsing error" mask for frame_attribute_flags_1 */
#define PARSER_ATT_PPPOE_PPP_PARSING_ERROR_MASK     0x00000020
	/** "MPLS parsing error" mask for frame_attribute_flags_1 */
#define PARSER_ATT_MPLS_PARSING_ERROR_MASK          0x00000004
	/** "ARP parsing error" mask for frame_attribute_flags_1 */
#define PARSER_ATT_ARP_PARSING_ERROR_MASK           0x00000001

/** @} *//* end of FSL_PARSER_FRAME_ATT_ERROR_PART_1_MASKS */



/**************************************************************************//**
@Group	FSL_PARSER_FRAME_ATTRIBUTES_PART_2_MASKS Frame Attributes Masks 2

@Description	Frame Attributes Part 2 Masks
		Used with \ref parse_result frame_attribute_flags_2 field
@{
*//***************************************************************************/
	/** "Unknown L2 EtherType field" mask for frame_attribute_flags_2 */
#define PARSER_ATT_L2_UNKOWN_PROTOCOL_MASK          0x80000000
	/** "Outer IPv4" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV4_1_MASK                      0x20000000
	/** "Outer IPv4 unicast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV4_1_UNICAST_MASK              0x10000000
	/** "Outer IPv4 multicast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV4_1_MULTICAST_MASK            0x08000000
	/** "Outer IPv4 broadcast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV4_1_BROADCAST_MASK            0x04000000
	/** "Inner IPv4" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV4_N_MASK                      0x02000000
	/** "Inner IPv4 unicast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV4_N_UNICAST_MASK              0x01000000
	/** "Inner IPv4 multicast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV4_N_MULTICAST_MASK            0x00800000
	/** "Inner IPv4 broadcast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV4_N_BROADCAST_MASK            0x00400000
	/** "Outer IPv6" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV6_1_MASK                      0x00200000
	/** "Outer IPv6 unicast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV6_1_UNICAST_MASK              0x00100000
	/** "Outer IPv6 multicast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV6_1_MULTICAST_MASK            0x00080000
	/** "Inner IPv6" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV6_N_MASK                      0x00040000
	/** "Inner IPv6 unicast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV6_N_UNICAST_MASK              0x00020000
	/** "Inner IPv6 multicast DA" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IPV6_N_MULTICAST_MASK            0x00010000
	/** "Outer IP options" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IP_1_OPTIONS_MASK                0x00008000
	/** "Outer IP unknown protocol/next header" mask for
	 * frame_attribute_flags_2 */
#define PARSER_ATT_IP_1_UNKNOWN_PROTOCOL_MASK       0x00004000
	/** "Outer IP is a fragment" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IP_1_IS_FRAGMENT_MASK            0x00002000
	/** "Outer IP is an initial fragment (fragment with offset 0)" mask
	 * for frame_attribute_flags_2 */
#define PARSER_ATT_IP_1_IS_INIT_FRAGMENT_MASK       0x00001000
	/** "Inner IP options" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IP_N_OPTIONS_MASK                0x00000400
	/** "Inner IP unknown protocol/next header" mask for 
	 * frame_attribute_flags_2 */
#define PARSER_ATT_IP_N_UNKNOWN_PROTOCOL_MASK       0x00000200
	/** "Inner IP is a fragment" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IP_N_IS_FRAGMENT_MASK            0x00000100
	/** "Inner IP is an initial fragment (fragment with offset 0)" mask
	 * for frame_attribute_flags_2 */
#define PARSER_ATT_IP_N_IS_INIT_FRAGMENT_MASK       0x00000080
	/** "ICMP" mask for frame_attribute_flags_2 */
#define PARSER_ATT_ICMP_MASK                        0x00000040
	/** "IGMP" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IGMP_MASK                        0x00000020
	/** "ICMPv6" mask for frame_attribute_flags_2 */
#define PARSER_ATT_ICMPV6_MASK                      0x00000010
	/** "UDP Lite" mask for frame_attribute_flags_2 */
#define PARSER_ATT_UDP_LITE_MASK                    0x00000008
	/** "Min Encap" mask for frame_attribute_flags_2 */
#define PARSER_ATT_MIN_ENCAP_MASK                   0x00000002
	/** "Min Encap S flag set" mask for frame_attribute_flags_2 */
#define PARSER_ATT_MIN_ENCAP_S_FLAG_MASK            0x00000001

/** @} *//* end of FSL_PARSER_FRAME_ATT_ERROR_PART_2_MASKS */


/**************************************************************************//**
@Group	FSL_PARSER_FRAME_ATT_ERROR_PART_2_MASKS Frame Attributes Error Masks 2

@Description	Frame Attribute Errors Part 2 Masks
		Used with \ref parse_result frame_attribute_flags_2 field
@{
*//***************************************************************************/
	/** "L2 soft parsing error" mask for frame_attribute_flags_2 */
#define PARSER_ATT_L2_SOFT_PARSING_ERROR_MASK       0x40000000
	/** "Outer IP parsing error" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IP_1_PARSING_ERROR_MASK          0x00000800
	/** "Last IP parsing error" mask for frame_attribute_flags_2 */
#define PARSER_ATT_IP_N_PARSING_ERROR_MASK          0x00000004


/** @} *//* end of FSL_PARSER_FRAME_ATTRIBUTES_PART_2_MASKS */

/**************************************************************************//**
@Group	FSL_PARSER_FRAME_ATTRIBUTES_PART_3_MASKS Frame Attributes Masks 3

@Description	Frame Attributes Part 3 Masks
		Used with \ref parse_result frame_attribute_flags_3 field
@{
*//***************************************************************************/
	/** "GRE" mask for frame_attribute_flags_3 */
#define PARSER_ATT_GRE_MASK                         0x40000000
	/** "GRE R bit set" mask for frame_attribute_flags_3 */
#define PARSER_ATT_GRE_R_BIT_SET_MASK               0x20000000
	/** "Unknown L3 next protocol" mask for frame_attribute_flags_3 */
#define PARSER_ATT_L3_UNKOWN_PROTOCOL_MASK          0x08000000
	/** "UDP" mask for frame_attribute_flags_3 */
#define PARSER_ATT_UDP_MASK                         0x02000000
	/** "TCP" mask for frame_attribute_flags_3 */
#define PARSER_ATT_TCP_MASK                         0x00800000
	/** "TCP options" mask for frame_attribute_flags_3 */
#define PARSER_ATT_TCP_OPTIONS_MASK                 0x00400000
	/** "TCP control bits 6-11 set" mask for frame_attribute_flags_3 */
#define PARSER_ATT_TCP_CONTROLS_6_11_SET_MASK       0x00200000
	/** "TCP control bits 3-5 set" mask for frame_attribute_flags_3 */
#define PARSER_ATT_TCP_CONTROLS_3_5_SET_MASK        0x00100000
	/** "IPSec" mask for frame_attribute_flags_3 */
#define PARSER_ATT_IPSEC_MASK                       0x00040000
	/** "IPSec ESP" mask for frame_attribute_flags_3 */
#define PARSER_ATT_IPSEC_ESP_MASK                   0x00020000
	/** "IPSec AH" mask for frame_attribute_flags_3 */
#define PARSER_ATT_IPSEC_AH_MASK                    0x00010000
	/** "SCTP" mask for frame_attribute_flags_3 */
#define PARSER_ATT_SCTP_MASK                        0x00004000
	/** "DCCP" mask for frame_attribute_flags_3 */
#define PARSER_ATT_DCCP_MASK                        0x00001000
	/** "Unknown L4 next protocol" mask for frame_attribute_flags_3 */
#define PARSER_ATT_L4_UNKOWN_PROTOCOL_MASK          0x00000400
	/** "GTP" mask for frame_attribute_flags_3 */
#define PARSER_ATT_GTP_MASK                         0x00000100
	/** "ESP or IKE over UDP" mask for frame_attribute_flags_3 */
#define PARSER_ATT_ESP_OR_IKE_OVER_UDP_MASK         0x00000040
	/** "iSCSI" mask for frame_attribute_flags_3 */
#define PARSER_ATT_ISCSI_MASK                       0x00000010
	/** "Capwap control" mask for frame_attribute_flags_3 */
#define PARSER_ATT_CAPWAP_CONTROL_MASK              0x00000008
	/** "Capwap data" mask for frame_attribute_flags_3 */
#define PARSER_ATT_CAPWAP_DATA_MASK                 0x00000004
	/** Routing header present in 1st IPv6 header */
#define PARSER_ATT_IPV6_ROUTING_HDR_1               0x00000001

/** @} *//* end of FSL_PARSER_FRAME_ATTRIBUTES_PART_3_MASKS */

/**************************************************************************//**
@Group	FSL_PARSER_FRAME_ATT_ERROR_PART_3_MASKS Frame Attributes Error Masks 3

@Description	Frame Attribute Errors Part 3 Masks
		Used with \ref parse_result frame_attribute_flags_3 field
@{
*//***************************************************************************/
	/** "Min Encap parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_MIN_ENCAP_PARSING_ERROR_MASK     0x80000000
	/** "GRE parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_GRE_PARSING_ERROR_MASK           0x10000000
	/** "L3 soft parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_L3_SOFT_PARSING_ERROR_MASK       0x04000000
	/** "UDP parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_UDP_PARSING_ERROR_MASK           0x01000000
	/** "TCP parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_TCP_PARSING_ERROR_MASK           0x00080000
	/** "IPSec parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_IPSEC_PARSING_ERROR_MASK         0x00008000
	/** "SCTP parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_SCTP_PARSING_ERROR_MASK          0x00002000
	/** "DCCP parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_DCCP_PARSING_ERROR_MASK          0x00000800
	/** "L4 soft parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_L4_SOFT_PARSING_ERROR_MASK       0x00000200
	/** "GTP parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_GTP_PARSING_ERROR_MASK           0x00000080
	/** "ESP or IKE over UDP parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_ESP_OR_IKE_OVER_UDP_PARSING_ERROR_MASK  0x00000020
	/** "L5 soft parsing error" mask for frame_attribute_flags_3 */
#define PARSER_ATT_L5_SOFT_PARSING_ERROR_MASK       0x00000002

/** @} *//* end of FSL_PARSER_FRAME_ATTRIBUTES_PART_3_MASKS */

/**************************************************************************//**
@Group		FSL_PARSER_ERROR_CODES PARSER Error Codes

@Description	When parsing error is indicated in the Frame Attribute Flags,
		an error code is returned in the "Parse Error Code" field in
		the parser result.
		The error codings are priority based thus regardless of which
		error is found first the more severe is reported.
@{
*//***************************************************************************/
	/** Parsing attempted to exceed presentation_length */
#define PARSER_EXCEED_BLOCK_LIMIT                       0x01
	/** Frame Truncation: Frame Parsing reached end of frame while parsing
	 * a header that expects more data */
#define PARSER_FRAME_TRUNCATION                         0x02
	/** Attempting to access an undefined or reserved HXS TODO*/
/*#define PARSER_INVALID_HXS				0x03*/
	/** Ethernet 802.3 length is larger than the frame received */
#define PARSER_ETH_802_3_TRUNCATION                     0x10
	/** PPPoE length is larger than the frame received */
#define PARSER_PPPOE_TRUNCATION                         0x20
	/** PPP MTU exceeds 1492 bytes */
#define PARSER_PPPOE_MTU_VIOLATED                       0x21
	/** PPP version field != 0x1 */
#define PARSER_PPPOE_VERSION_INVALID                    0x22
	/** PPP type field != 0x1 */
#define PARSER_PPPOE_TYPE_INVALID                       0x23
	/** PPP code field != 0x0 */
#define PARSER_PPPOE_CODE_INVALID                       0x24
	/** PPP Session ID == 0xffff */
#define PARSER_PPPOE_SESSION_ID_INVALID                 0x25
	/** IPv4 total length field exceeds the received packet length
	 * (excludes L2 header) */
#define PARSER_IPV4_PACKET_TRUNCATION                   0x40
	/** IPv4 checksum is incorrect */
#define PARSER_IPV4_CHECKSUM_ERROR                      0x41
	/** Version number in IPv4 packet != 4 */
#define PARSER_IPV4_VERSION_ERROR                       0x42
	/** IPv4 Minumun Fragment Size Error - IPv4 non-last fragment does not
	 * contain at least eight data bytes */
#define PARSER_IPV4_MIN_FRAG_SIZE_ERROR                 0x43
	/** IPv4 Header Length Error: IPv4 header length is less than 20 bytes,
	 * or exceeds the received packet length (excludes L2 header), or
	 * exceeds the IPv4 header Total Packet Length */
#define PARSER_IPV4_HEADER_LENGTH_ERROR                 0x44
	/** IPv6 Packet Truncation: IPv6 Payload length + 40 bytes header
	 * exceeds the received packet length (excludes L2 header), or Payload
	 * length == 0x0 */
#define PARSER_IPV6_PACKET_TRUNCATION                   0x48
	/** IPv6 Extension Headers Violation: IPv6 Extension Headers applied
	 * that are in violation of the IPv6 specification */
#define PARSER_IPV6_EXTENSION_HEADER_VIOLATION           0x49
	/** Version number in IPv6 packet != 6 */
#define PARSER_IPV6_VERSION_ERROR                       0x4A
	/** Routing Header Error: A routing header of type 0 encountered with
	 * header length not an even value or larger than twice the segs left */
#define PARSER_IPV6_ROUTING_HEADER_ERROR                0x4B
	/** GRE version is non zero */
#define PARSER_GRE_VERSION_ERROR                        0x50
	/** Minimum encapsulation checksum is incorrect */
#define PARSER_MINENC_CHECKSUM_ERROR                    0x58
	/** TCP offset < 5 */
#define PARSER_TCP_INVALID_OFFSET                       0x68
	/** TCP Data offset > IP length in Parse Array */
#define PARSER_TCP_PACKET_TRUNCATION                    0x69
	/** TCP checksum is incorrect */
#define PARSER_TCP_CHECKSUM_ERROR                       0x6A
	/** None of TCP flags are set, or following TCP flags set together:
	 * SYN & FIN, SYN & RST, FIN & RST, SYN & FIN & RST */
#define PARSER_TCP_BAD_FLAGS                            0x6B
	/** UPD Length Error: UDP length < 8, or does not fit inside the
	 * IP length when it is not a fragment */
#define PARSER_UDP_LENGTH_ERROR                         0x70
	/** UDP checksum == 0 for IPv6 */
#define PARSER_UDP_CHECKSUM_ZERO                        0x71
	/** UDP checksum is incorrect */
#define PARSER_UDP_CHECKSUM_ERROR                       0x72
	/** SCTP source or destination port was set to 0 */
#define PARSER_SCTP_PORT_0_DETECTED                     0x80
	/** GTP version is not 1 or 2 */
#define PARSER_GTP_UNSUPPORTED_VERSION                  0x98
	/** Invalid GTP' or GTPv1 Protocol type */
#define PARSER_GTP_INVALID_PROTOCOL_TYPE                0x99
	/** Invalid GTP' length bit */
#define PARSER_GTP_INVALID_L_BIT_ERROR                  0x9A

/** @} *//* end of FSL_PARSER_ERROR_CODES */


/**************************************************************************//**
@Group	FSL_PARSER_ERROR_QUERIES Parse Result Error Queries

@Description	These macros return a non-zero value in case an error in
		the relevant frame's attribute was found.
		These macros are working on the default working frame's
		parse result.
@{
*//***************************************************************************/

/** Returns a non-zero value in case Soft parsing error in shim is found.
 * This general flag may be used for reporting an error in case of soft HXS */
#define PARSER_IS_SHIM_SOFT_PARSING_ERROR_DEFAULT()\
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_SHIM_SOFT_PARSING_ERROR_MASK)
/** Returns a non-zero value in case parsing error is found.
 * The error code is reported in the Parse Result
 * (\ref FSL_PARSER_ERROR_CODES)*/
#define PARSER_IS_PARSING_ERROR_DEFAULT()\
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of Ethernet parsing error */
#define PARSER_IS_ETH_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_ETH_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of LLC+SNAP parsing error */
#define PARSER_IS_LLC_SNAP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_LLC_SNAP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of VLAN parsing error */
#define PARSER_IS_VLAN_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_VLAN_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of PPPOE+PPP parsing error */
#define PARSER_IS_PPPOE_PPP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_PPPOE_PPP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of MPLS parsing error */
#define PARSER_IS_MPLS_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_MPLS_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of ARP parsing error */
#define PARSER_IS_ARP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_ARP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of L2 soft parsing error
 * (can be either error in shim or in hard HXS Soft Sequence Attachment) */
#define PARSER_IS_L2_SOFT_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_L2_SOFT_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of Outer IP parsing error */
#define PARSER_IS_OUTER_IP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_1_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of Inner IP parsing error
 *(only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_N_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of Last IP parsing error */
#define PARSER_IS_LAST_IP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_N_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of Min Encap parsing error */
#define PARSER_IS_MIN_ENCAP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_MIN_ENCAP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of GRE parsing error */
#define PARSER_IS_GRE_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_GRE_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of L3 soft parsing error
 * (can be either error in shim or in hard HXS Soft Sequence Attachment) */
#define PARSER_IS_L3_SOFT_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_L3_SOFT_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of UDP parsing error */
#define PARSER_IS_UDP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_UDP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of TCP parsing error */
#define PARSER_IS_TCP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_TCP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of IPSec parsing error */
#define PARSER_IS_IPSEC_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_IPSEC_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of SCTP parsing error */
#define PARSER_IS_SCTP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_SCTP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of DCCP parsing error */
#define PARSER_IS_DCCP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_DCCP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of L4 soft parsing error
 * (can be either error in shim or in hard HXS Soft Sequence Attachment) */
#define PARSER_IS_L4_SOFT_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_L4_SOFT_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of GTP parsing error */
#define PARSER_IS_GTP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_GTP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of ESP or IKE parsing error */
#define PARSER_IS_ESP_OR_IKE_OVER_UDP_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_ESP_OR_IKE_OVER_UDP_PARSING_ERROR_MASK)
/** Returns a non-zero value in case of L5 soft parsing error
 * (can be either error in shim or in hard HXS Soft Sequence Attachment) */
#define PARSER_IS_L5_SOFT_PARSING_ERROR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_L5_SOFT_PARSING_ERROR_MASK)

/** @} */ /* end of FSL_PARSER_ERROR_QUERIES */


/**************************************************************************//**
@Group	FSL_PARSER_PR_QUERIES Parse Result Attributes Queries

@Description	These macros return a non-zero value in case the relevant
		attribute was found in the frame.
		These macros are working on the default working frame's
		parse result.
@{
*//***************************************************************************/
/** Returns a non-zero value in case Ethernet MAC is found */
#define PARSER_IS_ETH_MAC_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_ETH_MAC_MASK)
/** Returns a non-zero value in case Ethernet MAC with unicast DA is found */
#define PARSER_IS_ETH_MAC_UNICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_ETH_MAC_UNICAST_MASK)
/** Returns a non-zero value in case Ethernet MAC with multicast DA is found */
#define PARSER_IS_ETH_MAC_MULTICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_ETH_MAC_MULTICAST_MASK)
/** Returns a non-zero value in case Ethernet MAC with broadcast DA is found */
#define PARSER_IS_ETH_MAC_BROADCAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_ETH_MAC_BROADCAST_MASK)
/** Returns a non-zero value in case BPDU is found */
#define PARSER_IS_BPDU_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_BPDU_MASK)
/** Returns a non-zero value in case FCOE is found */
#define PARSER_IS_FCOE_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_FCOE_MASK)
/** Returns a non-zero value in case FCOE Initial Protocol is found */
#define PARSER_IS_FCOE_INIT_PROTOCOL_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_FCOE_INIT_PROTOCOL_MASK)
/** Returns a non-zero value in case LLC+SNAP is found */
#define PARSER_IS_LLC_SNAP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_LLC_SNAP_MASK)
/** Returns a non-zero value in case Unknown LLC/OUI is found
 * (LLC is not AAAA03 or OUI is not zero or Ethernet Length is <= 8).
 * Next HXS to be executed is the Other L3 shell.*/
#define PARSER_IS_UNKNOWN_LLC_OUI_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_UNKOWN_LLC_OUI_MASK)
/** Returns a non-zero value in case at least one VLAN is found */
#define PARSER_IS_ONE_VLAN_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_VLAN_1_MASK)
/** Returns a non-zero value in case more than one VLAN is found */
#define PARSER_IS_MORE_THAN_ONE_VLAN_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_VLAN_N_MASK)
/** Returns a non-zero value in case CFI in VLAN is set */
#define PARSER_IS_CFI_IN_VLAN_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_CFI_IN_VLAN_MASK)
/** Returns a non-zero value in case PPPoE+PPP is found */
#define PARSER_IS_PPPOE_PPP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_PPPOE_PPP_MASK)
/** Returns a non-zero value in case at least one MPLS is found */
#define PARSER_IS_ONE_MPLS_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_MPLS_1_MASK)
/** Returns a non-zero value in case more than one MPLS is found */
#define PARSER_IS_MORE_THAN_ONE_MPLS_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_MPLS_N_MASK)
/** Returns a non-zero value in case ARP is found */
#define PARSER_IS_ARP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_1 & PARSER_ATT_ARP_MASK)
/** Returns a non-zero value in case of an Unknown L2 next protocol
 Meaning, when next HXS to be executed is the Other L3 shell.
 (e.g. when EtherType field is set to an unrecognized value,
 or in case of IP HXS when neither IPv4 nor IPv6 are recognized).
 Next HXS to be executed is the Other L3 shell. */
#define PARSER_IS_L2_UNKNOWN_PROTOCOL_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_L2_UNKOWN_PROTOCOL_MASK)
/** Returns a non-zero value in case IP (IPv4/IPv6) is found */
#define PARSER_IS_IP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & \
	(PARSER_ATT_IPV4_1_MASK | PARSER_ATT_IPV6_1_MASK))
/** Returns a non-zero value in case Outer IPv4 is found */
#define PARSER_IS_OUTER_IPV4_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV4_1_MASK)
/** Returns a non-zero value in case Outer IPv4, unicast DA is found */
#define PARSER_IS_OUTER_IPV4_UNICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV4_1_UNICAST_MASK)
/** Returns a non-zero value in case Outer IPv4, multicast DA is found */
#define PARSER_IS_OUTER_IPV4_MULTICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV4_1_MULTICAST_MASK)
/** Returns a non-zero value in case Outer IPv4, broadcast DA is found */
#define PARSER_IS_OUTER_IPV4_BROADCAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV4_1_BROADCAST_MASK)
/** Returns a non-zero value in case more than one IP (IPv4/IPv6) is found */
#define PARSER_IS_TUNNELED_IP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & \
	(PARSER_ATT_IPV4_N_MASK | PARSER_ATT_IPV6_N_MASK))
/** Returns a non-zero value in case Inner IPv4 is found
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IPV4_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV4_N_MASK)
/** Returns a non-zero value in case Inner IPv4, unicast DA is found
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IPV4_UNICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV4_N_UNICAST_MASK)
/** Returns a non-zero value in case Inner IPv4, multicast DA is found
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IPV4_MULTICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV4_N_MULTICAST_MASK)
/** Returns a non-zero value in case Inner IPv4, broadcast DA is found
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IPV4_BROADCAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV4_N_BROADCAST_MASK)
/** Returns a non-zero value in case Outer IPv6 is found */
#define PARSER_IS_OUTER_IPV6_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV6_1_MASK)
/** Returns a non-zero value in case Outer IPv6, unicast DA is found */
#define PARSER_IS_OUTER_IPV6_UNICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV6_1_UNICAST_MASK)
/** Returns a non-zero value in case Outer IPv6, multicast DA is found */
#define PARSER_IS_OUTER_IPV6_MULTICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV6_1_MULTICAST_MASK)
/** Returns a non-zero value in case Inner IPv6 is found
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IPV6_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV6_N_MASK)
/** Returns a non-zero value in case Inner IPv6, unicast DA is found
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IPV6_UNICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV6_N_UNICAST_MASK)
/** Returns a non-zero value in case Inner IPv6, multicast DA is found
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IPV6_MULTICAST_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IPV6_N_MULTICAST_MASK)
/** Returns a non-zero value in case Outer IP with options is found */
#define PARSER_IS_OUTER_IP_OPTIONS_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_1_OPTIONS_MASK)
/** Returns a non-zero value in case of an Outer IP unknown next header
 * Next HXS to be executed is the Other L4 shell. */
#define PARSER_IS_OUTER_IP_UNKNOWN_PROTOCOL_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_1_UNKNOWN_PROTOCOL_MASK)
/** Returns a non-zero value in case Outer IP is a fragment */
#define PARSER_IS_OUTER_IP_FRAGMENT_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_1_IS_FRAGMENT_MASK)
/** Returns a non-zero value in case Outer IP is an initial fragment
 * (fragment with offset 0)*/
#define PARSER_IS_OUTER_IP_INIT_FRAGMENT_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_1_IS_INIT_FRAGMENT_MASK)
/** Returns a non-zero value in case Inner IP with options is found
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IP_OPTIONS_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_N_OPTIONS_MASK)
/** Returns a non-zero value in case of an Inner IP unknown next header
 * (only in case of more than 1 IP header in the frame)
  * Next HXS to be executed is the Other L4 shell. */
#define PARSER_IS_INNER_IP_UNKNOWN_PROTOCOL_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_N_UNKNOWN_PROTOCOL_MASK)
/** Returns a non-zero value in case Inner IP is a fragment
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IP_FRAGMENT_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_N_IS_FRAGMENT_MASK)
/** Returns a non-zero value in case Inner IP is an initial fragment
 * (fragment with offset 0)
 * (only in case of more than 1 IP header in the frame) */
#define PARSER_IS_INNER_IP_INIT_FRAGMENT_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IP_N_IS_INIT_FRAGMENT_MASK)
/** Returns a non-zero value in case ICMP is found
 * (Note that ICMP is not indicated for non initial fragment) */
#define PARSER_IS_ICMP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_ICMP_MASK)
/** Returns a non-zero value in case IGMP is found
 * (Note that IGMP is not indicated for non initial fragment) */
#define PARSER_IS_IGMP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_IGMP_MASK)
/** Returns a non-zero value in case ICMPv6 is found
 * (Note that ICMPv6 is indicated for non initial fragment) */
#define PARSER_IS_ICMPV6_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_ICMPV6_MASK)
/** Returns a non-zero value in case UDP Lite is found
 * (Note that UDP Lite is indicated for non initial fragment) */
#define PARSER_IS_UDP_LITE_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_UDP_LITE_MASK)
/** Returns a non-zero value in case Min Encap is found */
#define PARSER_IS_MIN_ENCAP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_MIN_ENCAP_MASK)
/** Returns a non-zero value in case Min Encap with S flag set is found */
#define PARSER_IS_MIN_ENCAP_S_FLAG_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_2 & PARSER_ATT_MIN_ENCAP_S_FLAG_MASK)
/** Returns a non-zero value in case GRE is found */
#define PARSER_IS_GRE_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_GRE_MASK)
/** Returns a non-zero value in case GRE with R bit set is found */
#define PARSER_IS_GRE_R_BIT_SET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_GRE_R_BIT_SET_MASK)
/** Returns a non-zero value in case of an Unknown L3 next protocol
 * Next HXS to be executed is the Other L4 shell. */
#define PARSER_IS_L3_UNKOWN_PROTOCOL_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_L3_UNKOWN_PROTOCOL_MASK)
/** Returns a non-zero value in case UDP is found */
#define PARSER_IS_UDP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_UDP_MASK)
/** Returns a non-zero value in case TCP is found */
#define PARSER_IS_TCP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_TCP_MASK)
/** Returns a non-zero value in case TCP or UDP is found */
#define PARSER_IS_TCP_OR_UDP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & (PARSER_ATT_TCP_MASK | PARSER_ATT_UDP_MASK))
/** Returns a non-zero value in case TCP with options is found */
#define PARSER_IS_TCP_OPTIONS_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_TCP_OPTIONS_MASK)
/** TCP Control bits 6-11 query. Returns a non-zero value when at least one of
 * TCP control bits 6-11 is set */
#define PARSER_IS_TCP_CONTROLS_6_11_SET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_TCP_CONTROLS_6_11_SET_MASK)
/** TCP Control bits 3-5 query. Returns a non-zero value in case at least one of
 *  TCP control bits 3-5 is set */
#define PARSER_IS_TCP_CONTROLS_3_5_SET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_TCP_CONTROLS_3_5_SET_MASK)
/** Returns a non-zero value in case IPSec is found */
#define PARSER_IS_IPSEC_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_IPSEC_MASK)
/** Returns a non-zero value in case IPSec ESP is found */
#define PARSER_IS_IPSEC_ESP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_IPSEC_ESP_MASK)
/** Returns a non-zero value in case IPSec AH is found */
#define PARSER_IS_IPSEC_AH_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_IPSEC_AH_MASK)
/** Returns a non-zero value in case SCTP is found */
#define PARSER_IS_SCTP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_SCTP_MASK)
/** Returns a non-zero value in case DCCP is found */
#define PARSER_IS_DCCP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_DCCP_MASK)
/** Returns a non-zero value in case of an unknown L4 next protocol
 * Next HXS to be executed is the Other L5 shell. */
#define PARSER_IS_L4_UNKOWN_PROTOCOL_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_L4_UNKOWN_PROTOCOL_MASK)
/** Returns a non-zero value in case GTP is found */
#define PARSER_IS_GTP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_GTP_MASK)
/** Returns a non-zero value in case ESP or IKE over UDP is found */
#define PARSER_IS_ESP_OR_IKE_OVER_UDP_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_ESP_OR_IKE_OVER_UDP_MASK)
/** Returns a non-zero value in case IKE over UDP is found */
#define PARSER_IS_IKE_OVER_UDP_DEFAULT() \
	((PARSER_IS_ESP_OR_IKE_OVER_UDP_DEFAULT() && \
	(*((uint32_t *)(PARSER_GET_L5_POINTER_DEFAULT())) == 0)) \
	? (1) : (0))
/** Returns a non-zero value in case ESP over UDP is found */
#define PARSER_IS_ESP_OVER_UDP_DEFAULT() \
	((PARSER_IS_ESP_OR_IKE_OVER_UDP_DEFAULT() && \
	(*((uint32_t *)(PARSER_GET_L5_POINTER_DEFAULT())) != 0)) \
	? (1) : (0))
/** Returns a non-zero value in case iSCSI is found */
#define PARSER_IS_ISCSI_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_ISCSI_MASK)
/** Returns a non-zero value in case CapWap control is found */
#define PARSER_IS_CAPWAP_CONTROL_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_CAPWAP_CONTROL_MASK)
/** Returns a non-zero value in case CapWap data is found */
#define PARSER_IS_CAPWAP_DATA_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_CAPWAP_DATA_MASK)
/** Returns a non-zero value in case Routing hdr in 1st IPv6 header is found */
#define PARSER_IS_ROUTING_HDR_IN_1ST_IPV6_HDR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_3 & PARSER_ATT_IPV6_ROUTING_HDR_1)
/** Returns a non-zero value in case Routing hdr in 2nd IPv6 header is found */
#ifndef REV2
#define PARSER_IS_ROUTING_HDR_IN_2ND_IPV6_HDR_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	frame_attribute_flags_extension & PARSER_ATT_IPV6_ROUTING_HDR_2)
/** Returns a non-zero value in case VxLAN is found */
#define PARSER_IS_VXLAN_DEFAULT() (PARSER_IS_UDP_DEFAULT() && \
	((*((uint16_t *)((uint32_t)PARSER_GET_L4_POINTER_DEFAULT() + 2)) == 4789)) \
	? (1) : (0))
#endif
/** @} */ /* end of FSL_PARSER_PR_QUERIES */


/**************************************************************************//**
@Group	FSL_PARSER_PR_GETTERS Parse Result Getters

@Description	These macros return the offset of the relevant protocol
		in the frame. Offset 0xFF indicates that the corresponding
		protocol was not found in the frame.
		These macros are working on the default working frame's
		parse result.
@{
*//***************************************************************************/

/** Get Next header */
#define PARSER_GET_NEXT_HEADER_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->nxt_hdr)
/** Get Shim1 offset*/
#define PARSER_GET_SHIM1_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->shim_offset_1)
/** Get Shim2 offset*/
#define PARSER_GET_SHIM2_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->shim_offset_2)
/** Get the IP Protocol Identifier offset of the first IP */
#define PARSER_GET_IP_PID_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->ip_pid_offset)
/** Get the ETHERNET header offset */
#define PARSER_GET_ETH_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->eth_offset)
/** Get the LLC+SNAP header offset */
#define PARSER_GET_LLC_SNAP_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->llc_snap_offset)
/** Get the first VLAN TCI offset */
#define PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->vlan_tci1_offset)
/** Get the last VLAN TCI offset */
#define PARSER_GET_LAST_VLAN_TCI_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->vlan_tcin_offset)
/** Get the last EtherType offset */
#define PARSER_GET_LAST_ETYPE_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->last_etype_offset)
/** Get the PPPoE header offset */
#define PARSER_GET_PPPOE_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->pppoe_offset)
/** Get the first MPLS offset */
#define PARSER_GET_FIRST_MPLS_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->mpls_offset_1)
/** Get the last MPLS offset */
#define PARSER_GET_LAST_MPLS_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->mpls_offset_n)
/** Get the outer IP header offset*/
#define PARSER_GET_OUTER_IP_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->ip1_or_arp_offset)
/** Get the ARP header offset*/
#define PARSER_GET_ARP_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->ip1_or_arp_offset)
/** Get the inner IP header offset*/
#define PARSER_GET_INNER_IP_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	ipn_or_minencapO_offset)
/** Get the MinEncap header offset*/
#define PARSER_GET_MINENCAP_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)-> \
	ipn_or_minencapO_offset)
/** Get the GRE header offset */
#define PARSER_GET_GRE_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->gre_offset)
/** Get the L4 (UDP/TCP/SCTP/DCCP/IGMP/UDP Lite) header offset */
#define PARSER_GET_L4_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->l4_offset)
/** Get L5 (GTP/ESP/IPsec/iSCSI/CapWap/PTP(not in REV1) header offset */
#define PARSER_GET_L5_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->gtp_esp_ipsec_offset)
/** Get Routing Header offset in 1st IPv6 header */
#define PARSER_GET_1ST_IPV6_ROUTING_HDR_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->routing_hdr_offset1)
/** Get Routing Header offset in 2nd IPv6 header */
#define PARSER_GET_2ND_IPV6_ROUTING_HDR_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->routing_hdr_offset2)
/** Get Next header offset
 * (offset to the last result of the parsed header of the next header type) */
#define PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->nxt_hdr_offset)
/** Get IPv6 fragment header offset */
#define PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->ipv6_frag_offset)
/** Get Gross Running Sum */
#define PARSER_GET_GROSS_RUNNING_SUM_CODE_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->gross_running_sum)
/** Get Running Sum */
#define PARSER_GET_RUNNING_SUM_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->running_sum)
/** Get Parse Error Code */
#define PARSER_GET_PARSE_ERROR_CODE_DEFAULT() \
	(((struct parse_result *)HWC_PARSE_RES_ADDRESS)->parse_error_code)
/** @} */ /* end of FSL_PARSER_PR_GETTERS */

/**************************************************************************//**
@Group	FSL_PARSER_POINTER_IN_FRMAE_GETTERS Pointer in Frame Getters

@Description	These macros return the pointer to the relevant protocol
		in the frame.
		These macros are working on the default working frame's
		parse result.
@{
*//***************************************************************************/

/** Get the pointer to Shim1
 in the default frame presented in the workspace */
#define PARSER_GET_SHIM1_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_SHIM1_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to Shim2
 in the default frame presented in the workspace */
#define PARSER_GET_SHIM2_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_SHIM2_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to IP Protocol Identifier of the first IP
 in the default frame presented in the workspace */
#define PARSER_GET_IP_PID_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_IP_PID_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to ETHERNET
 in the default frame presented in the workspace */
#define PARSER_GET_ETH_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_ETH_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to LLC+SNAP
 in the default frame presented in the workspace */
#define PARSER_GET_LLC_SNAP_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_LLC_SNAP_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to first VLAN TCI
 in the default frame presented in the workspace */
#define PARSER_GET_FIRST_VLAN_TCI_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to last VLAN TCI
 in the default frame presented in the workspace */
#define PARSER_GET_LAST_VLAN_TCI_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_LAST_VLAN_TCI_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to last EtherType
 in the default frame presented in the workspace*/
#define PARSER_GET_LAST_ETYPE_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_LAST_ETYPE_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to PPPoE header
 in the default frame presented in the workspace */
#define PARSER_GET_PPPOE_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_PPPOE_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to first MPLS
 in the default frame presented in the workspace */
#define PARSER_GET_FIRST_MPLS_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_FIRST_MPLS_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to last MPLS
 in the default frame presented in the workspace */
#define PARSER_GET_LAST_MPLS_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_LAST_MPLS_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to outer IP header
 in the default frame presented in the workspace */
#define PARSER_GET_OUTER_IP_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to ARP header
 in the default frame presented in the workspace */
#define PARSER_GET_ARP_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_ARP_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to inner IP header
 in the default frame presented in the workspace */
#define PARSER_GET_INNER_IP_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_INNER_IP_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to MinEncap header
 in the default frame presented in the workspace */
#define PARSER_GET_MINENCAP_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_MINENCAP_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to GRE
 in the default frame presented in the workspace */
#define PARSER_GET_GRE_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_GRE_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to L4 (UDP/TCP/SCTP/DCCP) header
 in the default frame presented in the workspace */
#define PARSER_GET_L4_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_L4_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to L5 (GTP/ESP/IPsec) header
 in the default frame presented in the workspace */
#define PARSER_GET_L5_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_L5_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to Routing Header in 1st IPv6 header */
#define PARSER_GET_1ST_IPV6_ROUTING_HDR_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_1ST_IPV6_ROUTING_HDR_OFFSET_DEFAULT() \
			+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to Routing Header in 2nd IPv6 header */
#define PARSER_GET_2ND_IPV6_ROUTING_HDR_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_2ND_IPV6_ROUTING_HDR_OFFSET_DEFAULT() \
			+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to Next header (last result of the parsed header of the
 * next header type) in the default frame presented in the workspace */
#define PARSER_GET_NEXT_HEADER_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())
/** Get the pointer to IPv6 fragment header
 in the default frame presented in the workspace */
#define PARSER_GET_IPV6_FRAG_HEADER_POINTER_DEFAULT() \
	(void *)((uint16_t)PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT() \
		+ PRC_GET_SEGMENT_ADDRESS())

/** @} */ /* end of FSL_PARSER_POINTER_IN_FRMAE_GETTERS */

/**************************************************************************//**
@Group	FSL_PARSER_SETTERS PARSER Setters

@Description	These macros set parser parameters in the default task params.

@{
*//***************************************************************************/
	/** Macro to set parser_profile_id in the default task params */
#define PARSER_SET_PRPID(_val)						\
	(default_task_params.parser_profile_id = (uint8_t)_val)
	/** Macro to set parser_starting_hxs in the default task params */
#define PARSER_SET_STARTING_HXS(_val)					\
	(default_task_params.parser_starting_hxs = (uint16_t)_val)

/** @} */ /* end of FSL_PARSER_SETTERS */

/**************************************************************************//**
@Group	FSL_PARSER_GEN_PARSE_RESULT_FLAGS Flags for parse_result_generate function
@{
*//***************************************************************************/
/** No flags */
#define PARSER_NO_FLAGS				0x0
/** Validate L3 checksum flag */
#define PARSER_VALIDATE_L3_CHECKSUM		0x10
/** Validate L4 checksum flag */
#define PARSER_VALIDATE_L4_CHECKSUM		0x08
/** Validate L3 & L4 checksum flags */
#define PARSER_VALIDATE_L3_L4_CHECKSUM		0x18

/** @} */ /* end of FSL_PARSER_GEN_PARSE_RESULT_FLAGS */


/**************************************************************************//**
@Group	FSL_PARSER_HXS_CONFIG PARSER HXS configuration in parse profile defines

@Description	For configuring each HXS (Header Examination Sequence) in the
		Parse Profile Record, user should use the flags relevant to
		each HXS configuration by performing "OR" between the flags
		and the index for soft sequence start address.
@{
*//***************************************************************************/
	/** HXS Config Enable. When set to 1, it enables soft extension of
	 * examination instructions beginning at every protocol HXS
	 * configuration Soft Sequence Start.*/
#define PARSER_PRP_HXS_CONFIG_EN		0x8000
	/** Mask Error reporting. 0 disabled, report error from this HXS to the
	 * Parser Error Status \n
	 * 1 enabled, do not report error */
#define PARSER_PRP_HXS_CONFIG_ERM		0x1000
	/** Enable MTU checking */
#define PARSER_PRP_PPP_HXS_CONFIG_EMC		0x2000
	/** MPLS Label Interpretation enable. When this bit is set, next HXS to
	 * be executed is based on the interpretation of the last MPLS label
	 * (can be either IPv4/IPv6/Other L3 shell/Default next parse sequence).
	 * When the bit is cleared, the Frame Parsing advances to MPLS Default
	 * Next Parse. */
#define PARSER_PRP_MPLS_HXS_CONFIG_LIE		0x1000
	/** Routing Header Enable. When not set (by default), the routing header
	 * is ignored and the destination address from the Main header is used
	 * instead. */
#define PARSER_PRP_IPV6_HXS_CONFIG_RHE		0x2000
	/** Short Packet Padding Removal From Checksum Calculation. When set to
	 * 1, the contribution of the padded region at the end of a frame is
	 * removed from the checksum calculation. */
#define PARSER_PRP_TCP_UDP_HXS_CONFIG_SPPR	0x2000


/** @} */ /* end of FSL_PARSER_HXS_CONFIG */

/** @} */ /* end of FSL_PARSER_MACROS */


/**************************************************************************//**
 @Group		PARSER_Enumerations PARSER Enumerations

 @Description	PARSER Enumerations

 @{
*//***************************************************************************/

/**************************************************************************//**
@enum	parser_starting_hxs_code

@Description PARSER Starting HXS code

@{
*//***************************************************************************/
enum parser_starting_hxs_code {
	/** Ethernet Starting HXS coding */
	 PARSER_ETH_STARTING_HXS = 0x0000,
	/** LLC+SNAP Starting HXS coding */
	 PARSER_LLC_SNAP_STARTING_HXS = 0x0001,
	/** VLAN Starting HXS coding */
	 PARSER_VLAN_STARTING_HXS = 0x0002,
	/** PPPoE+PPP Starting HXS coding */
	 PARSER_PPPOE_PPP_STARTING_HXS = 0x0003,
	/** MPLS Starting HXS coding */
	 PARSER_MPLS_STARTING_HXS = 0x0004,
	/** ARP Starting HXS coding */
	 PARSER_ARP_STARTING_HXS = 0x0005,
	/** IP Starting HXS coding */
	 PARSER_IP_STARTING_HXS	 = 0x0006,
	/** GRE Starting HXS coding */
	 PARSER_GRE_STARTING_HXS = 0x0009,
	/** MinEncap Starting HXS coding */
	 PARSER_MINENCAP_STARTING_HXS = 0x000A,
	/** Other L3 Shell Starting HXS coding */
	 PARSER_OTHER_L3_SHELL_STARTING_HXS = 0x000B,
	/** TCP Starting HXS coding */
	 PARSER_TCP_STARTING_HXS = 0x000C,
	/** UDP Starting HXS coding */
	 PARSER_UDP_STARTING_HXS = 0x000D,
	/** IPSec Starting HXS coding */
	 PARSER_IPSEC_STARTING_HXS = 0x000E,
	/** SCTP Starting HXS coding */
	 PARSER_SCTP_STARTING_HXS = 0x000F,
	/** DCCP Starting HXS coding */
	 PARSER_DCCP_STARTING_HXS = 0x0010,
	/** Other L4 Shell Starting HXS coding */
	 PARSER_OTHER_L4_SHELL_STARTING_HXS = 0x0011,
	/** GTP Starting HXS coding */
	 PARSER_GTP_STARTING_HXS = 0x0012,
	/** ESP Starting HXS coding */
	 PARSER_ESP_STARTING_HXS = 0x0013,
	/** L5 (and above) Shell Starting HXS coding */
	 PARSER_L5_SHELL_STARTING_HXS = 0x001E,
	/** Final Shell Starting HXS coding */
	 PARSER_FINAL_SHELL_STARTING_HXS = 0x001F
};

/** @} */ /* end of parser_starting_hxs_code */

/** @} */ /* end of group PARSER_Enumerations */



/**************************************************************************//**
@Group		FSL_PARSER_STRUCTS PARSER Structures

@Description	Freescale AIOP PARSER Structures

@{
*//***************************************************************************/


/**************************************************************************//**
@Description	Parse Result structure

		Please refer to the parser specification for more details.
*//***************************************************************************/

#pragma pack(push, 1)
struct parse_result {
	/** Next header */
	volatile uint16_t	nxt_hdr;
	/** Frame Attribute Flags Extension */
	volatile uint16_t	frame_attribute_flags_extension;
	/** Frame Attribute Flags (part 1) */
	volatile uint32_t	frame_attribute_flags_1;
	/** Frame Attribute Flags (part 2) */
	volatile uint32_t	frame_attribute_flags_2;
	/** Frame Attribute Flags (part 3) */
	volatile uint32_t	frame_attribute_flags_3;
	/** Shim Offset 1 */
	volatile uint8_t	shim_offset_1;
	/** Shim Offset 2 */
	volatile uint8_t	shim_offset_2;
	/** IP protocol field offset */
	volatile uint8_t	ip_pid_offset;
	/** Ethernet offset */
	volatile uint8_t	eth_offset;
	/** LLC+SNAP offset */
	volatile uint8_t	llc_snap_offset;
	/** First VLAN's TCI field offset*/
	volatile uint8_t	vlan_tci1_offset;
	/** Last VLAN's TCI field offset*/
	volatile uint8_t	vlan_tcin_offset;
	/** Last Ethertype offset*/
	volatile uint8_t	last_etype_offset;
	/** PPPoE offset */
	volatile uint8_t	pppoe_offset;
	/** First MPLS offset */
	volatile uint8_t	mpls_offset_1;
	/** Last MPLS offset */
	volatile uint8_t	mpls_offset_n;
	/** Outer IP or ARP offset */
	volatile uint8_t	ip1_or_arp_offset;
	/** Inner IP or MinEncap offset*/
	volatile uint8_t	ipn_or_minencapO_offset;
	/** GRE offset */
	volatile uint8_t	gre_offset;
	/** Layer 4 offset*/
	volatile uint8_t	l4_offset;
	/** GTP/ESP/IPsec offset */
	volatile uint8_t	gtp_esp_ipsec_offset;
	/** Routing header offset of 1st IPv6 header */
	volatile uint8_t	routing_hdr_offset1;
	/** Routing header offset of 2nd IPv6 header */
	volatile uint8_t	routing_hdr_offset2;
	/** Next header offset */
	volatile uint8_t	nxt_hdr_offset;
	/** IPv6 fragmentable part offset */
	volatile uint8_t	ipv6_frag_offset;
	/** Frame's untouched running sum, input to parser */
	volatile uint16_t	gross_running_sum;
	/** Running Sum */
	volatile uint16_t	running_sum;
	/** Parse Error code.
	 * Please refer to \ref FSL_PARSER_ERROR_CODES*/
	volatile uint8_t	parse_error_code;
	/** Reserved for Soft parsing context*/
	volatile uint8_t	soft_parsing_context[23];
};
#pragma pack(pop)

/**************************************************************************//**
@Description	Vlan HXS Configuration in Parser Profile Record structure
*//***************************************************************************/
#pragma pack(push, 1)
struct	vlan_hxs_configuration {
	/** This field includes: Bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index for soft
	 * sequence start address. Reserved bits must be clear. */
	uint16_t  en_erm_soft_seq_start;
	/** configured TPID 1. Configures a TPID value to indicate a VLAN tag
	 * in addition to the common TPID values 0x8100 and 0x88A8 */
	uint16_t  configured_tpid_1;
	/** configured TPID 2. Configures a TPID value to indicate a VLAN tag
	 * in addition to the common TPID values 0x8100 and 0x88A8 */
	uint16_t  configured_tpid_2;
};
#pragma pack(pop)

/**************************************************************************//**
@Description	MPLS HXS Configuration in Parser Profile Record structure
*//***************************************************************************/
#pragma pack(push, 1)
struct	mpls_hxs_configuration {
	/** This field includes: Bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index for soft
	 * sequence start address. Reserved bits must be clear. */
	uint16_t  en_erm_soft_seq_start;
	/** This field includes: Bit LIE (refer to \ref FSL_PARSER_HXS_CONFIG)
	 * and 11-bit index for indicating the HXS or soft sequence start
	 * address to advance after the MPLS HXS when the MPLS Label
	 * Interpretation is disabled or MPLS label is >15.
	 * Index value must be equal or greater than IPv4.
	 * Reserved bits must be clear. */
	uint16_t  lie_dnp;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Parser Profile Record structure

		Parsing is directed through parse profiles which provide
		a template of parsing actions to be taken.
		64 profiles are supported.
		Parser Profile is built of many different HXS (Header
		Examination Sequence) configurations. Each configuration
		includes some flags for actions to be taken, and a
		Soft Sequence Attachment record that is indexed by that
		HXS Soft Sequence Start field. When enabled, the soft sequence
		attachment is executed at the end of the hard HXS execution,
		just before the hard HXS branches out to the next HXS.

		This is a general 16-bit HXS config description:

|   0  |  1   |       2        |     3     |  4   |        5 - 15          |
|------|------|----------------|-----------|------|------------------------|
|  EN  |  --  |  EMC/RHE/SPPR  |  ERM/LIE  |  --  |  Soft Sequence Start   |

*//***************************************************************************/
#pragma pack(push, 1)
struct parse_profile_record {
	/** Reserved for compliance with HW format. User should not access
	 * this field. */
	uint32_t  reserved1;
	/** Eth HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  eth_hxs_config;
	/** LLC/SNAP HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  llc_snap_hxs_config;
	/** VLAN HXS config. Refer to vlan_hxs_configuration struct
	 * description. */
	struct    vlan_hxs_configuration vlan_hxs_config;
	/** PPP/PPPOE HXS configuration. Includes bits EN, EMC, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  pppoe_ppp_hxs_config;
	/** MPLS HXS config. Refer to mpls_hxs_configuration struct
	 * description. */
	struct	  mpls_hxs_configuration mpls_hxs_config;
	/** ARP HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  arp_hxs_config;
	/** IP HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  ip_hxs_config;
	/** IPv4 HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  ipv4_hxs_config;
	/** IPv6 HXS configuration. Includes bits EN, RHE, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  ipv6_hxs_config;
	/** GRE HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  gre_hxs_config;
	/** MINENC HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  minenc_hxs_config;
	/** Other L3 HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address.
	 * This is the next HXS to be executed in case of an L2 Unknown
	 * Protocol.
	 * The Other L3 HXS does not provide any header parsing and validation
	 * results. It can act as a termination point for the parsing or entry
	 * point to a soft HXS. Reserved bits must be clear. */
	uint16_t  other_l3_shell_hxs_config;
	/** TCP HXS configuration. Includes bits EN, SPPR, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  tcp_hxs_config;
	/** UDP HXS configuration. Includes bits EN, SPPR, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  udp_hxs_config;
	/** IPSec HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  ipsec_hxs_config;
	/** SCTP HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  sctp_hxs_config;
	/** DCCP HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  dccp_hxs_config;
	/** Other L4 HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address.
	 * This is the next HXS to be executed in case of an L3 Unknown
	 * Protocol.
	 * The Other L4 HXS does not provide any header parsing and validation
	 * results. It can act as a termination point for the parsing or entry
	 * point to a soft HXS. Reserved bits must be clear. */
	uint16_t  other_l4_shell_hxs_config;
	/** GTP HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  gtp_hxs_config;
	/** ESP HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. Reserved bits must be clear. */
	uint16_t  esp_hxs_config;
	/** Reserved for compliance with HW format. User should not access this
	 * field */
	uint16_t  reserved2;
	/** L5 Shell (and above) HXS config. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address.
	 * This is the next HXS to be executed in case of an L4 Unknown
	 * Protocol.
	 * The Other L5 HXS does not provide any header parsing and validation
	 * results. It can act as a termination point for the parsing or
	 * entry point to a soft HXS. */
	uint16_t  l5_shell_hxs_config;
	/** Final Shell HXS configuration. Includes bits EN, ERM
	 * (refer to \ref FSL_PARSER_HXS_CONFIG) and 11-bit index (16-bit words)
	 * for soft sequence start address. */
	uint16_t  final_shell_hxs_config;
	/** Soft Examination Parameter Array (W0-W15). */
	uint32_t  soft_examination_param_array[16];
};
#pragma pack(pop)

/**************************************************************************//**
@Description	Parser Profile Input to create/replace parse profile commands
*//***************************************************************************/
struct parse_profile_input {
	/** Reserved for compliance with HW format. User should not access this
	 * field */
	uint8_t reserved[8];
	/** Parse Profile Record */
	struct parse_profile_record parse_profile;
};


/** @} */ /* end of FSL_PARSER_STRUCTS */


/**************************************************************************//**
@Group		FSL_PARSER_Functions PARSER Functions

@Description	Freescale AIOP PARSER Functions

@{
*//***************************************************************************/


/**************************************************************************//**
@Function	parser_profile_create

@Description	Creates Parser Profile which provides a template
		of parsing actions to be taken.

@Param[in]	parse_profile - Parse Profile Input the user should fill
		(located in the workspace).
		Must be 16 bytes aligned.
@Param[out]	prpid - Parse Profile ID (located in the workspace).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		ENOSPC - No more Parse Profiles are available (all 64 are taken)

@Cautions	In this function the task yields.
*//***************************************************************************/
int parser_profile_create(struct parse_profile_input *parse_profile,
	uint8_t *prpid);

/**************************************************************************//**
@Function	parser_profile_replace

@Description	Replaces Parser Profile which provides a template
		of parsing actions to be taken.

@Param[in]	parse_profile - Parse Profile Input. User should
		locate this structure in the workspace and fill it.
		Must be 16 bytes aligned.
@Param[in]	prpid - Parse Profile ID.

@Return		None.

@Cautions	In this function the task yields.
*//***************************************************************************/
void parser_profile_replace(struct parse_profile_input *parse_profile,
	uint8_t prpid);

/**************************************************************************//**
@Function	parser_profile_delete

@Description	Deletes Parser Profile.

@Param[in]	prpid - Parse Profile ID.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		ENAVAIL - All Parse Profiles are already deleted.

@Cautions	In this function the task yields.
*//***************************************************************************/
int parser_profile_delete(uint8_t prpid);

/**************************************************************************//**
@Function	parser_profile_query

@Description	Returns Parser Profile Entry for a provided Parser Profile ID.

@Param[in]	prpid - Parse Profile ID (located in the workspace).
@Param[out]	parse_profile - Points to a user preallocated memory in the
 	 	workspace to which the parse profile entry will be written.
		Must be 16 bytes aligned.
		Note: In Rev1, only first 48 bytes are correct.

@Return		None.

@Cautions	In this function the task yields.
*//***************************************************************************/
void parser_profile_query(uint8_t prpid,
			struct parse_profile_input *parse_profile);


#include "parser_inline.h"

/**************************************************************************//**
@Function	parse_result_generate_default

@Description	Runs parser with default task parameters and generates
		parse result.
		This function provides, on a per Parse Profile basis,
		examination of a frame at the start of the frame (offset 0)
		with presumption of the first header type as configured in the
		default starting HXS.

		Implicit input parameters:
		FD, Segment address, Segment size, Parser Profile ID,
		Starting HXS.

		Implicitly updated values in Task Defaults in the HWC:
		Parser Result.

@Param[in]	flags - Please refer to \ref FSL_PARSER_GEN_PARSE_RESULT_FLAGS.

@Return		0 on Success, or negative value on error.
		The exact error code can be discovered by using
		PARSER_GET_PARSE_ERROR_CODE_DEFAULT(). See error codes in
		\ref FSL_PARSER_ERROR_CODES.

@Retval		0 - Success
@Retval		EIO - Parsing Error
@Retval		EIO - L3 Checksum Validation Error
@Retval		EIO - L4 Checksum Validation Error
@Retval		ENOSPC - Block Limit Exceeds (Frame Parsing reached the limit
		of the minimum between presentation_length and 256 bytes before
		completing all parsing)

@Cautions	In this function the task yields.
 	  	Presented header address in the workspace must be aligned to
 	  	16 bytes.
		In case gross running sum is clear, and L4 validation is not
		required, running sum field in the parse result is not valid.
 	 	In case L4 validation is required but the gross running sum is
 	 	not correct, the user must clear it before calling parser.
 	 	This function may result in a fatal error. 	
*//***************************************************************************/
inline int parse_result_generate_default(uint8_t flags);

/**************************************************************************//**
@Function	parse_result_generate

@Description	Runs parser and generates parse result.
		This function provides, on a per Parse Profile basis,
		the ability to begin the examination of a frame at a different
		offset within the frame with a different presumption of the
		first header type.

		Implicit input parameters:
		FD, Segment address, Segment size, Parser Profile ID.

		Implicitly updated values in Task Defaults in the HWC:
		Parser Result.

@Param[in]	starting_hxs - Starting HXS for the parser.
		Please refer to \ref parser_starting_hxs_code.
@Param[in]	starting_offset - Offset from the presented segment where
		parsing is to start. (Segment is presented in:
		Presentation Context [SEGMENT ADDRESS])
@Param[in]	flags - Please refer to \ref FSL_PARSER_GEN_PARSE_RESULT_FLAGS.

@Return		0 on Success, or negative value on error.
		The exact error code can be discovered by using
		PARSER_GET_PARSE_ERROR_CODE_DEFAULT(). See error codes in
		\ref FSL_PARSER_ERROR_CODES.

@Retval		0 - Success
@Retval		EIO - Parsing Error
@Retval		EIO - L3 Checksum Validation Error
@Retval		EIO - L4 Checksum Validation Error
@Retval		ENOSPC - Block Limit Exceeds (Frame Parsing reached the limit
		of the minimum between presentation_length and 256 bytes before
		completing all parsing)

@Cautions	In this function the task yields.
 	  	Presented header address in the workspace must be aligned to
 	  	16 bytes.
		In case gross running sum is clear, and L4 validation is not
		required, running sum field in the parse result is not valid.
 	 	In case L4 validation is required but the gross running sum is
 	 	not correct, the user must clear it before calling parser.
 	 	This function may result in a fatal error.
*//***************************************************************************/
inline int parse_result_generate(enum parser_starting_hxs_code starting_hxs,
	uint8_t starting_offset, uint8_t flags);

/**************************************************************************//**
@Function	parse_result_generate_basic

@Description	Runs parser and generates parse result, with the following
		arguments: PRPID = 0, starting_hxs = 0 (Ethernet),
		starting_offset = 0, no checksum validation.

		Implicit input parameters:
		FD, Segment address, Segment size.

		Implicitly updated values in Task Defaults in the HWC:
		Parser Result.

@Return		0 on Success, or negative value on error.
		The exact error code can be discovered by using
		PARSER_GET_PARSE_ERROR_CODE_DEFAULT(). See error codes in
		\ref FSL_PARSER_ERROR_CODES.

@Retval		0 - Success
@Retval		EIO - Parsing Error
@Retval		ENOSPC - Block Limit Exceeds (Frame Parsing reached the limit
		of the minimum between presentation_length and 256 bytes before
		completing all parsing)

@Cautions	In this function the task yields.
 	  	Presented header address in the workspace must be aligned to
 	  	16 bytes.
 	 	If input gross running sum is not correct, both "gross running
 	 	sum" and "running sum" fields in the parse result are not valid.
 	 	This function may result in a fatal error.
*//***************************************************************************/
inline int parse_result_generate_basic(void);


/** @} */ /* end of FSL_PARSER_Functions */
/** @} */ /* end of FSL_PARSER */
/** @} */ /* end of ACCEL */


#endif /* __FSL_PARSER_H */
