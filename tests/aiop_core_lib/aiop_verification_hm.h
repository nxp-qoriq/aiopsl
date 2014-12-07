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
@File          aiop_verification_HM.h

@Description   This file contains the AIOP HM SW Verification Structures
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_HM_H_
#define __AIOP_VERIFICATION_HM_H_

#include "dplib/fsl_ldpaa.h"
#include "net/fsl_net.h"


/**************************************************************************//**
 @addtogroup		AIOP_Service_Routines_Verification

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_HM_SRs_Verification

 @Description	AIOP HM Verification structures definitions.

 @{
*//***************************************************************************/

#define HM_VERIF_ACCEL_ID	0xFE
	/**< HM accelerator ID For verification purposes*/
#define PARSER_INIT_BPID	1
	/**< A BPID to use for the parser init.
	 * This BPID needs to be initialized in BMAN in order for the
	 * HM tests to run correctly. The HM tests requires only one buffer. */
#define PARSER_INIT_BUFF_SIZE	128
	/**< A buffer size that corresponds to the PARSER_INIT_BPID to use for
	 * the parser init.
	 * This BPID needs to be initialized in BMAN in order for the
	 * HM tests to run correctly */
#define HM_VERIF_ACCEL_ID	0xFE
	/**< HM accelerator ID For verification purposes*/


/*! \enum e_hm_verif_cmd_type defines the statistics engine CMDTYPE field.*/
enum e_hm_verif_cmd_type {
	HM_CMDTYPE_VERIF_INIT = 0,
	HM_CMDTYPE_L2_HEADER_REMOVE,
	HM_CMDTYPE_VLAN_HEADER_REMOVE,
	HM_CMDTYPE_IPV4_MODIFICATION,
	HM_CMDTYPE_IPV4_MANGLE,
	HM_CMDTYPE_IPV4_DEC_TTL,
	HM_CMDTYPE_IPV4_TS_OPT,
	HM_CMDTYPE_IPV6_MODIFICATION,
	HM_CMDTYPE_IPV6_MANGLE,
	HM_CMDTYPE_IPV6_DEC_HOP_LIMIT,
	HM_CMDTYPE_IPV4_ENCAPSULATION,
	HM_CMDTYPE_IPV6_ENCAPSULATION,
	HM_CMDTYPE_EXT_IPV4_ENCAPSULATION,
	HM_CMDTYPE_EXT_IPV6_ENCAPSULATION,
	HM_CMDTYPE_IP_DECAPSULATION,
	HM_CMDTYPE_UDP_MODIFICATION,
	HM_CMDTYPE_TCP_MODIFICATION,
	HM_CMDTYPE_NAT_IPV4,
	HM_CMDTYPE_NAT_IPV6,
	HM_CMDTYPE_SET_VLAN_VID,
	HM_CMDTYPE_SET_VLAN_PCP,
	HM_CMDTYPE_SET_DL_SRC,
	HM_CMDTYPE_SET_DL_DST,
	HM_CMDTYPE_SET_NW_SRC,
	HM_CMDTYPE_SET_NW_DST,
	HM_CMDTYPE_SET_TP_SRC,
	HM_CMDTYPE_SET_TP_DST,
	HM_CMDTYPE_PUSH_VLAN,
	HM_CMDTYPE_PUSH_AND_SET_VLAN,
	HM_CMDTYPE_POP_VLAN,
	HM_CMDTYPE_ARP_RESPONSE,
	HM_CMDTYPE_SET_L2_SRC_DST,
	HM_CMDTYPE_IP_CKSUM_CALCULATE,
	HM_CMDTYPE_L4_UDP_TCP_CKSUM_CALC,
	HM_CMDTYPE_MPLS_HEADER_REMOVE,
	HM_CMDTYPE_PUSH_AND_SET_MPLS,
	HM_CMDTYPE_POP_MPLS,
	HM_CMDTYPE_PUSH_AND_SET_VXLAN,
	HM_CMDTYPE_POP_VXLAN,
	HM_CMDTYPE_SET_VXLAN_VID,
	HM_CMDTYPE_SET_VXLAN_FLAGS,
	HM_CMDTYPE_SET_TCP_SRC,
	HM_CMDTYPE_SET_TCP_DST,
	HM_CMDTYPE_SET_UDP_SRC,
	HM_CMDTYPE_SET_UDP_DST
};

/* HM Commands Structure identifiers */

#define HM_VERIF_INIT_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_VERIF_INIT)

#define HM_L2_HEADER_REMOVE_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_L2_HEADER_REMOVE)

#define HM_VLAN_HEADER_REMOVE_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_VLAN_HEADER_REMOVE)

#define HM_MPLS_HEADER_REMOVE_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_MPLS_HEADER_REMOVE)

#define HM_IPV4_MODIFICATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV4_MODIFICATION)

#define HM_IPV4_MANGLE_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV4_MANGLE)

#define HM_IPV4_DEC_TTL_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV4_DEC_TTL)

#define HM_IPV4_TS_OPT_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV4_TS_OPT)

#define HM_IPV6_MODIFICATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV6_MODIFICATION)

#define HM_IPV6_MANGLE_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV6_MANGLE)

#define HM_IPV6_DEC_HOP_LIMIT_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV6_DEC_HOP_LIMIT)

#define HM_IPV4_ENCAPSULATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV4_ENCAPSULATION)

#define HM_IPV6_ENCAPSULATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IPV6_ENCAPSULATION)

#define HM_IPV4_EXT_ENCAPSULATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_EXT_IPV4_ENCAPSULATION)

#define HM_IPV6_EXT_ENCAPSULATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_EXT_IPV6_ENCAPSULATION)

#define HM_IP_DECAPSULATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IP_DECAPSULATION)

#define HM_UDP_MODIFICATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_UDP_MODIFICATION)

#define HM_TCP_MODIFICATION_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_TCP_MODIFICATION)

#define HM_NAT_IPV4_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_NAT_IPV4)

#define HM_NAT_IPV6_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_NAT_IPV6)

#define HM_SET_VLAN_VID_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_VLAN_VID)

#define HM_SET_VLAN_PCP_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_VLAN_PCP)

#define HM_SET_DL_SRC_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_DL_SRC)

#define HM_SET_DL_DST_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_DL_DST)

#define HM_SET_NW_SRC_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_NW_SRC)

#define HM_SET_NW_DST_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_NW_DST)

#define HM_SET_TP_SRC_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_TP_SRC)

#define HM_SET_TP_DST_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_TP_DST)

#define HM_SET_TCP_SRC_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_TCP_SRC)

#define HM_SET_TCP_DST_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_TCP_DST)

#define HM_SET_UDP_SRC_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_UDP_SRC)

#define HM_SET_UDP_DST_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_UDP_DST)

#define HM_PUSH_VLAN_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_PUSH_VLAN)

#define HM_PUSH_AND_SET_VLAN_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_PUSH_AND_SET_VLAN)

#define HM_POP_VLAN_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_POP_VLAN)

#define HM_PUSH_AND_SET_VXLAN_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_PUSH_AND_SET_VXLAN)

#define HM_SET_VXLAN_VID_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_VXLAN_VID)

#define HM_SET_VXLAN_FLAGS_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_VXLAN_FLAGS)

#define HM_POP_VXLAN_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_POP_VXLAN)

#define HM_PUSH_AND_SET_MPLS_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_PUSH_AND_SET_MPLS)

#define HM_POP_MPLS_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_POP_MPLS)

#define HM_ARP_RESPONSE_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_ARP_RESPONSE)

#define HM_SET_L2_SRC_DST_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_SET_L2_SRC_DST)

#define HM_IP_CKSUM_CALCULATE_CMD_STR		((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_IP_CKSUM_CALCULATE)

#define HM_L4_UDP_TCP_CKSUM_CALC_CMD_STR	((HM_MODULE << 16) | \
		(uint32_t)HM_CMDTYPE_L4_UDP_TCP_CKSUM_CALC)

/**************************************************************************//**
@Description	HM verification init Command structure.

		This command inits the parser that are needed for the HM
		verification. This command must be called before calling the
		HM commands (it only needed to be called once in the
		beginning of the test).
*//***************************************************************************/
struct hm_init_verif_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint16_t	parser_starting_hxs;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM L2 header remove Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_l2_header_remove_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
};

/**************************************************************************//**
@Description	HM VLAN header remove Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_vlan_remove_command {
	uint32_t	opcode;
	/**< Command structure identifier. */
	int32_t		status;
};

/**************************************************************************//**
@Description	HM MPLS header remove Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_mpls_remove_command {
	uint32_t	opcode;
	/**< Command structure identifier. */
};


/**************************************************************************//**
@Description	HM IPv4 Header Modification Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv4_modification_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	ip_src_addr;
	uint32_t	ip_dst_addr;
	uint16_t	id;
	uint8_t		flags;
	uint8_t		tos;
};

/**************************************************************************//**
@Description	HM IPv4 Mangle Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv4_mangle_command {
	uint32_t	opcode;
	uint8_t		flags;
	uint8_t		dscp;
	uint8_t		ttl;
	uint8_t		pad;
};

/**************************************************************************//**
@Description	HM IPv4 decrement TTL Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv4_dec_ttl_command {
	uint32_t	opcode;
};

/**************************************************************************//**
@Description	HM IPv4 time stamp option Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv4_ts_opt_command {
	uint32_t	opcode;
	int		status;
	uint32_t	ip_address;
};

/**************************************************************************//**
@Description	HM IPv6 Header Modification Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv6_modification_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	flow_label;
	uint8_t		flags;
	uint8_t		tc;
	uint8_t		ip_src_addr[16];
	uint8_t		ip_dst_addr[16];
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM IPv6 Mangle Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv6_mangle_command {
	uint32_t	opcode;
	uint32_t	flow_label;
	uint8_t		flags;
	uint8_t		dscp;
	uint8_t		hop_limit;
	uint8_t		pad;
};

/**************************************************************************//**
@Description	HM IPv6 decrement Hop Limit Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv6_dec_hop_limit_command {
	uint32_t	opcode;
};

/**************************************************************************//**
@Description	HM IPv4 Header Encapsulation Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv4_encapsulation_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	struct ipv4hdr	ipv4_header_ptr;
	uint8_t		flags;
	uint8_t		ipv4_header_size;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM IPv6 Header Encapsulation Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv6_encapsulation_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	struct ipv6hdr	ipv6_header_ptr;
	uint8_t		flags;
	uint8_t		ipv6_header_size;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM IP Header Decapsulation Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ip_decapsulation_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint8_t		flags;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	HM UDP Header Modification Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_udp_modification_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	udp_src_port;
	uint16_t	udp_dst_port;
	uint8_t		flags;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	HM TCP Header Modification Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_tcp_modification_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	tcp_src_port;
	uint16_t	tcp_dst_port;
	int16_t		tcp_seq_num_delta;
	int16_t		tcp_ack_num_delta;
	uint16_t	tcp_mss;
	uint8_t		flags;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	HM IPv4 NAT Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv4_nat_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	ip_src_addr;
	uint32_t	ip_dst_addr;
	uint16_t	l4_src_port;
	uint16_t	l4_dst_port;
	int16_t		tcp_seq_num_delta;
	int16_t		tcp_ack_num_delta;
	uint8_t		flags;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	HM IPv6 NAT Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_ipv6_nat_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	ip_src_addr[4];
	uint32_t	ip_dst_addr[4];
	uint16_t	l4_src_port;
	uint16_t	l4_dst_port;
	int16_t		tcp_seq_num_delta;
	int16_t		tcp_ack_num_delta;
	uint8_t		flags;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	HM set VLAN VID Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_vlan_vid_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	vlan_vid;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM set VLAN PCP Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_vlan_pcp_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint8_t		vlan_pcp;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	HM set MAC SRC or DST Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_set_dl_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint8_t		mac_addr[6];
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM set IPv4 SRC or DST address Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_set_nw_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	ipv4_addr;
};

/**************************************************************************//**
@Description	HM set UDP/TCP SRC/DST ports Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_set_tp_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint16_t	port;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM set TCP port Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_set_tcp_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint16_t	port;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM set UDP port Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_set_udp_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint16_t	port;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM push VLAN Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_push_vlan_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint16_t	ethertype;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	HM push and set VLAN Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_push_and_set_vlan_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	vlan_tag;
};

/**************************************************************************//**
@Description	HM pop VLAN Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_pop_vlan_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
};

/**************************************************************************//**
@Description	HM push and set MPLS Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_push_and_set_mpls_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	mpls_header;
	uint16_t 	ethertype;
	uint8_t 	pad[2];
};

/**************************************************************************//**
@Description	HM pop MPLS Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_pop_mpls_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
};

/**************************************************************************//**
@Description	HM set VXLAN VID Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_vxlan_vid_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	vxlan_vid;
};

/**************************************************************************//**
@Description	HM set VXLAN flags Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_vxlan_flags_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint8_t		flags;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	HM push and set VXLAN Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_push_and_set_vxlan_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t    header_ptr;
	uint16_t 	header_size;
	uint8_t 	pad[2];
};

/**************************************************************************//**
@Description	HM pop VXLAN Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_pop_vxlan_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
};

/**************************************************************************//**
@Description	HM ARP Response Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_arp_response_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
};

/**************************************************************************//**
@Description	HM Set L2 source and destination addresses Command structure.

		Includes information needed for HM Command verification.
*//***************************************************************************/
struct hm_set_l2_src_dst_command {
	uint32_t	opcode;
		/**< Target MAC address. */
	uint8_t target_hw_addr[6];
		/**< Command structure identifier. */
	int32_t		status;
};

/**************************************************************************//**
@Description	Calculate IPv4 Header Checksum Command structure.

		Includes information needed for Checksum Commands verification.
*//***************************************************************************/
struct hm_ip_cksum_calculation_command {
	uint32_t opcode;
	uint32_t ipv4header;
	uint8_t  flags;
};

/**************************************************************************//**
@Description	Calculate UDP TCP Checksum Command structure.

		Includes information needed for Checksum Commands verification.
*//***************************************************************************/
struct hm_l4_udp_tcp_cksum_calc_command {
	uint32_t opcode;
	int32_t  status;
	uint8_t  flags;
};

void aiop_hm_init_parser();

uint16_t aiop_verification_hm(uint32_t asa_seg_addr);

/** @} */ /* end of AIOP_HM_SRs_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */


#endif /* __AIOP_VERIFICATION_HM_H_ */
