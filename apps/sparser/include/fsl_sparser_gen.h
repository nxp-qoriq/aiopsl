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

/*******************************************************************************
 *                              - CAUTION -
 *
 * This code must not be distributed till we haven't a clear understanding
 * about what internals of the Parser may be exposed to customers.
 ******************************************************************************/

/**************************************************************************//**
@File		fsl_sparser_gen.h

@Description	Soft Parser byte-code generation API
*//***************************************************************************/
#ifndef __FSL_SPARSER_GEN_H
#define __FSL_SPARSER_GEN_H

#include "fsl_dbg.h"

/**************************************************************************//**
@Group		sparser_gen_g SPARSER GEN

@Description	Contains API functions and wrapper macros used to generate the
		byte code array of a Soft Parser.
@{
*//***************************************************************************/

/**************************************************************************//**
@Description	 enum sparser_jmp_flags - Jump destination post-fix flags

*//***************************************************************************/
enum sparser_jmp_flags {
	/* GOSUB flag */
	GF = 0x8000,
	/* Relative address flag */
	LF = 0x4000,
	/* Advance flag */
	AF = 0x2000,
	/* Sign flag */
	SF = 0x0400
};

/**************************************************************************//**
 @Group		SPARSER_LOC_LABELS Soft Parser Local Labels

 @Description	Defines the value of the first "local" label.

 @{
*//***************************************************************************/
/* Local Labels Base Address. The relative addressing mode is forced. */
#define SPARSER_LOC_LABEL_BASE	0x4800

/**************************************************************************//**
@Description	 enum sparser_local_labels - 16 Local Labels

*//***************************************************************************/
enum sparser_local_labels {
	/* Local Label 1 */
	SP_LABEL_1 = SPARSER_LOC_LABEL_BASE,
	SP_LABEL_2,
	SP_LABEL_3,
	SP_LABEL_4,
	SP_LABEL_5,
	SP_LABEL_6,
	SP_LABEL_7,
	SP_LABEL_8,
	SP_LABEL_9,
	SP_LABEL_10,
	SP_LABEL_11,
	SP_LABEL_12,
	SP_LABEL_13,
	SP_LABEL_14,
	SP_LABEL_15,
	/* Local Label 16 */
	SP_LABEL_16
};

/** @} end of group SPARSER_LOC_LABELS */

/**************************************************************************//**
@enum	sparser_hxs_dst

@Description Soft Parser HXS jump destinations

@{
*//***************************************************************************/
enum sparser_hxs_dst {
	/* Ethernet HXS destination */
	SP_ETH_HXS_DST = 0x000,
	/* LLC+SNAP HXS destination */
	SP_LLC_SNAP_HXS_DST = 0x001,
	/* VLAN HXS destination */
	SP_VLAN_HXS_DST = 0x002,
	/* PPPoE+PPP HXS destination */
	SP_PPPOE_PPP_HXS_DST = 0x003,
	/* MPLS HXS destination */
	SP_MPLS_HXS_DST = 0x004,
	/* ARP HXS destination */
	SP_ARP_HXS_DST = 0x005,
	/* IP HXS destination */
	SP_IP_HXS_DST = 0x006,
	/* IPv4 HXS destination */
	SP_IPV4_HXS_DST = 0x007,
	/* IPv6 HXS destination */
	SP_IPV6_HXS_DST = 0x008,
	/* GRE HXS destination */
	SP_GRE_HXS_DST = 0x009,
	/* MinEncap HXS destination */
	SP_MINENCAP_HXS_DST = 0x00A,
	/* Other L3 Shell HXS destination */
	SP_OTHER_L3_SHELL_HXS_DST = 0x00B,
	/* TCP HXS destination */
	SP_TCP_HXS_DST = 0x00C,
	/* UDP HXS destination */
	SP_UDP_HXS_DST = 0x00D,
	/* IPSec HXS destination */
	SP_IPSEC_HXS_DST = 0x00E,
	/* SCTP HXS destination */
	SP_SCTP_HXS_DST = 0x00F,
	/* DCCP HXS destination */
	SP_DCCP_HXS_DST = 0x010,
	/* Other L4 Shell HXS destination */
	SP_OTHER_L4_SHELL_HXS_DST = 0x011,
	/* GTP HXS destination */
	SP_GTP_HXS_DST = 0x012,
	/* ESP HXS destination */
	SP_ESP_HXS_DST = 0x013,
	/* VXLAN HXS destination */
	SP_VXLAN_HXS_DST = 0x014,
	/* L5 (and above) Shell HXS destination */
	SP_L5_SHELL_HXS_DST = 0x01E,
	/* Final Shell HXS destination */
	SP_FINAL_SHELL_HXS_DST = 0x01F,
	/* Return to hard HXS destination */
	SP_RETURN_TO_HXS_DST = 0x7FE,
	/* End Parsing HXS destination */
	SP_END_PARSING_HXS_DST = 0x7FF,
};

/** @} end of group sparser_hxs_dst */

/**************************************************************************//**
@enum	sparser_imm_val

@Description Soft Parser Immediate Value

@{
*//***************************************************************************/
enum sparser_imm_val {
	/* 16 bits immediate value */
	SP_IMM_16 = 1,
	/* 32 bits immediate value */
	SP_IMM_32,
	/* 48 bits immediate value */
	SP_IMM_48,
	/* 64 bits immediate value */
	SP_IMM_64
};

/** @} end of group sparser_imm_val */

/**************************************************************************//**
@enum	sparser_faf_bit

@Description Soft Parser FAF bits offset

@{
*//***************************************************************************/
enum sparser_faf_bit {
	/* Routing header present in IPv6 header 2 */
	SP_FAF_IPV6_ROUTE_HDR2_PRESENT = 0,
	/* GTP Primed was detected */
	SP_FAF_IPV6_GTP_PRIMED_DETECTED = 1,
	/* VLAN with VID = 0 was detected */
	SP_FAF_VLAN_WITH_VID_0_DETECTED = 2,
	/* A PTP frame was detected */
	SP_FAF_PTP_DETECTED = 3,
	/* VXLAN was parsed */
	SP_FAF_VXLAN_PRESENT = 4,
	/* A VXLAN HXS parsing error was detected */
	SP_FAF_VXLAN_PARSE_ERR = 5,
	/* Ethernet control protocol (MAC DA is
	 * 01:80:C2:00:00:00-01:80:C2:00:00:00:FF) */
	SP_FAF_ETH_SLOW_PROTO_DETECTED = 6,
	/* IKE was detected at UDP port 4500 */
	SP_FAF_IKE_PRESENT = 7,
	/* Shim Shell Soft Parsing Error */
	SP_FAF_SHIM_SOFT_PARSE_ERR = 8,
	/* Parsing Error */
	SP_FAF_PARSING_ERR = 9,
	/* Ethernet MAC Present */
	SP_FAF_ETH_MAC_PRESENT = 10,
	/* Ethernet Unicast */
	SP_FAF_ETH_UNICAST = 11,
	/* Ethernet Multicast */
	SP_FAF_ETH_MULTICAST = 12,
	/* Ethernet Broadcast */
	SP_FAF_ETH_BROADCAST = 13,
	/* BPDU frame (MAC DA is 01:80:C2:00:00:00) */
	SP_FAF_BPDU_FRAME = 14,
	/* FCoE detected (EType is 0x8906 */
	SP_FAF_FCOE_DETECTED = 15,
	/* FIP detected (EType is 0x8914) */
	SP_FAF_FIP_DETECTED = 16,
	/* Ethernet Parsing Error */
	SP_FAF_ETH_PARSING_ERR = 17,
	/* LLC+SNAP Present */
	SP_FAF_LLC_SNAP_PRESENT = 18,
	/* Unknown LLC/OUI */
	SP_FAF_UNKNOWN_LLC_OUI = 19,
	/* LLC+SNAP Error */
	SP_FAF_LLC_SNAP_ERR = 20,
	/* VLAN 1 Present */
	SP_FAF_VLAN_1_PRESENT = 21,
	/* VLAN 1 Present */
	SP_FAF_VLAN_N_PRESENT = 22,
	/* CFI bit in a "8100" VLAN tag is set */
	SP_FAF_VLAN_8100_CFI_SET = 23,
	/* VLAN Parsing Error */
	SP_FAF_VLAN_PARSING_ERR = 24,
	/* PPPoE+PPP Present */
	SP_FAF_PPOE_PPP_PRESENT = 25,
	/* PPPoE+PPP Parsing Error */
	SP_FAF_PPOE_PPP_PARSING_ERR = 26,
	/* MPLS 1 Present */
	SP_FAF_MPLS_1_PRESENT = 27,
	/* MPLS n Present */
	SP_FAF_MPLS_N_PRESENT = 28,
	/* MPLS Parsing Error */
	SP_FAF_MPLS_PARSING_ERR = 29,
	/* ARP frame Present (Ethertype 0x0806 */
	SP_FAF_ARP_PRESENT = 30,
	/* ARP Parsing Error */
	SP_FAF_ARP_PARSING_ERR = 31,
	/* L2 Unknown Protocol */
	SP_FAF_L2_UNKNOWN_PROTO = 32,
	/* L2 Soft Parsing Error */
	SP_FAF_L2_SOFT_PARSE_ERR = 33,
	/* IPv4 1 Present */
	SP_FAF_IPV4_1_PRESENT = 34,
	/* IPv4 1 Unicast */
	SP_FAF_IPV4_1_UNICAST = 35,
	/* IPv4 1 Multicast */
	SP_FAF_IPV4_1_MULTICAST = 36,
	/* IPv4 1 Broadcast */
	SP_FAF_IPV4_1_BROADCAST = 37,
	/* IPv4 n Present */
	SP_FAF_IPV4_N_PRESENT = 38,
	/* IPv4 n Unicast */
	SP_FAF_IPV4_N_UNICAST = 39,
	/* IPv4 n Multicast */
	SP_FAF_IPV4_N_MULTICAST = 40,
	/* IPv4 n Broadcast */
	SP_FAF_IPV4_N_BROADCAST = 41,
	/* IPv6 1 Present */
	SP_FAF_IPV6_1_PRESENT = 42,
	/* IPv6 1 Unicast */
	SP_FAF_IPV6_1_UNICAST = 43,
	/* IPv6 1 Multicast */
	SP_FAF_IPV6_1_MULTICAST = 44,
	/* IPv6 n Present */
	SP_FAF_IPV6_N_PRESENT = 45,
	/* IPv6 n Unicast */
	SP_FAF_IPV6_N_UNICAST = 46,
	/* IPv6 n Multicast */
	SP_FAF_IPV6_N_MULTICAST = 47,
	/* IP 1 option present */
	SP_FAF_IP_OPTION_1_PRESENT = 48,
	/* IP 1 Unknown Protocol */
	SP_FAF_IP_1_UNKNOWN_PROTO = 49,
	/* IP 1 Packet is a fragment */
	SP_FAF_IP_1_IS_FRAGMENT = 50,
	/* IP 1 Packet is an initial fragment */
	SP_FAF_IP_1_IS_FIRST_FRAGMENT = 51,
	/* IP 1 Parsing Error */
	SP_FAF_IP_1_PARSING_ERR = 52,
	/* IP n option present */
	SP_FAF_IP_OPTION_N_PRESENT = 53,
	/* IP n Unknown Protocol */
	SP_FAF_IP_N_UNKNOWN_PROTO = 54,
	/* IP n Packet is a fragment */
	SP_FAF_IP_N_IS_FRAGMENT = 55,
	/* IP n Packet is an initial fragment */
	SP_FAF_IP_N_IS_FIRST_FRAGMENT = 56,
	/* ICMP detected (IP proto is 1 */
	SP_FAF_ICMP_DETECTED = 57,
	/* IGMP detected (IP proto is 2) */
	SP_FAF_IGMP_DETECTED = 58,
	/* ICMPv6 detected (IP proto is 3a) */
	SP_FAF_ICMP_V6_DETECTED = 59,
	/* UDP Light detected (IP proto is 136) */
	SP_FAF_UDP_LIGHT_DETECTED = 60,
	/* IP n Parsing Error */
	SP_FAF_IP_N_PARSING_ERR = 61,
	/* Min. Encap Present */
	SP_FAF_MIN_ENCAP_PRESENT = 62,
	/* Min. Encap S flag set */
	SP_FAF_MIN_ENCAP_S_FLAG_SET = 63,
	/* Min. Encap Parsing Error */
	SP_FAF_MIN_ENCAP_PARSING_ERR = 64,
	/* GRE Present */
	SP_FAF_GRE_PRESENT = 65,
	/* GRE R bit set */
	SP_FAF_GRE_R_BIT_SET = 66,
	/* GRE Parsing Error */
	SP_FAF_GRE_PARSING_ERR = 67,
	/* L3 Unknown Protocol */
	SP_FAF_L3_UNKNOWN_PROTO = 68,
	/* L3 Soft Parsing Error */
	SP_FAF_L3_SOFT_PARSING_ERR = 69,
	/* UDP Present */
	SP_FAF_UDP_PRESENT = 70,
	/* UDP Parsing Error */
	SP_FAF_UDP_PARSING_ERR = 71,
	/* TCP Present */
	SP_FAF_TCP_PRESENT = 72,
	/* TCP options present */
	SP_FAF_TCP_OPTIONS_PRESENT = 73,
	/* TCP Control bits 6-11 set */
	SP_FAF_TCP_CTRL_BITS_6_11_SET = 74,
	/* TCP Control bits 3-5 set */
	SP_FAF_TCP_CTRL_BITS_3_5_SET = 75,
	/* TCP Parsing Error */
	SP_FAF_TCP_PARSING_ERR = 76,
	/* IPSec Present */
	SP_FAF_IPSEC_PRESENT = 77,
	/* IPSec ESP found */
	SP_FAF_IPSEC_ESP_FOUND = 78,
	/* IPSec AH found */
	SP_FAF_IPSEC_AH_FOUND = 79,
	/* IPSec Parsing Error */
	SP_FAF_IPSEC_PARSING_ERR = 80,
	/* SCTP Present */
	SP_FAF_SCTP_PRESENT = 81,
	/* SCTP Parsing Error */
	SP_FAF_SCTP_PARSING_ERR = 82,
	/* DCCP Present */
	SP_FAF_DCCP_PRESENT = 83,
	/* DCCP Parsing Error */
	SP_FAF_DCCP_PARSING_ERR = 84,
	/* L4 Unknown Protocol */
	SP_FAF_L4_UNKNOWN_PROTO = 85,
	/* L4 Soft Parsing Error */
	SP_FAF_L4_SOFT_PARSING_ERR = 86,
	/* GTP Present */
	SP_FAF_GTP_PRESENT = 87,
	/* GTP Parsing Error */
	SP_FAF_GTP_PARSING_ERR = 88,
	/* ESP Present */
	SP_FAF_ESP_PRESENT = 89,
	/* ESP Parsing Error */
	SP_FAF_ESP_PARSING_ERR = 90,
	/* iSCSI detected (Port# 860) */
	SP_FAF_ISCSI_DETECTED = 91,
	/* Capwap-control detected (Port# 5246) */
	SP_FAF_CAPWAP_CTRL_DETECTED = 92,
	/* Capwap-data detected (Port# 5247) */
	SP_FAF_CAPWAP_DATA_DETECTED = 93,
	/* L5 Soft Parsing Error */
	SP_FAF_L5_SOFT_PARSING_ERR = 94,
	/* IPv6 Route hdr1 present */
	SP_FAF_IPV6_ROUTE_HDR1_PRESENT = 95,
	/* User defined soft parser bit #0 */
	SP_FAF_UD_SOFT_PARSER_0 = 96,
	/* User defined soft parser bit #1 */
	SP_FAF_UD_SOFT_PARSER_1 = 97,
	/* User defined soft parser bit #2 */
	SP_FAF_UD_SOFT_PARSER_2 = 98,
	/* User defined soft parser bit #3 */
	SP_FAF_UD_SOFT_PARSER_3 = 99,
	/* User defined soft parser bit #4 */
	SP_FAF_UD_SOFT_PARSER_4 = 100,
	/* User defined soft parser bit #5 */
	SP_FAF_UD_SOFT_PARSER_5 = 101,
	/* User defined soft parser bit #6 */
	SP_FAF_UD_SOFT_PARSER_6 = 102,
	/* User defined soft parser bit #7 */
	SP_FAF_UD_SOFT_PARSER_7 = 103,
};

/** @} end of group sparser_faf_bit */

/**************************************************************************//**
@Function	sparser_begin_bytecode_wrt

@Description	Initializes a Soft Parser byte-code writing. Mandatory call.

@Param[in]	pc : Starting Program Counter of the Soft Parser.

@Param[in]	sp : Soft Parser byte-code array. Must be aligned on a four
		bytes boundary.

@Param[in]	sz : Soft Parser byte-code array size, in bytes. Must be a
		multiple of four.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_begin_bytecode_wrt(uint16_t pc, uint8_t *sp, uint16_t sz);

/**************************************************************************//**
@Function	sparser_bytecode_dump

@Description	Dumps Soft Parser byte-code.

@Return		None

*//***************************************************************************/
void sparser_bytecode_dump(void);

/**************************************************************************//**
@Function	sparser_end_bytecode_wrt

@Description	Finalizes a Soft Parser byte-code writing. Mandatory call.

@Return		None

*//***************************************************************************/
void sparser_end_bytecode_wrt(void);

/******************************************************************************/
void sparser_set_label(uint8_t label);

/**************************************************************************//**
@Function	sparser_gen_nop

@Description	Generates NOP opcode.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_nop(void);

/**************************************************************************//**
@Function	sparser_gen_advance_hb_by_wo

@Description	Adds the value of Window Offset to Header Base and resets the
		value of Window Offset to zero. Used within to advance to HB
		for the following HXS. It is appropriate to apply when WO is
		pointing to the first byte that the next HXS is to examine.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_advance_hb_by_wo(void);

/**************************************************************************//**
@Function	sparser_gen_clr_wr0

@Description	Clears WR0.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_clr_wr1

@Description	Clears WR1.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_checksum16_wr1_to_wr0

@Description	Used to 1's complement checksums (that is, the verify checksum
		algorithm used by TCP,UDP,IP).
		Performs 16-bit 1's complement addition of the four 16-bit
		values within WR1 and the least significant 16-bits of WR0 and
		places the result in the least significant 16-bits of WR0
		(same position).

		WR0(6,7) <= WR0(6,7)+WR1(0,1)+WR1(2,3)+WR1(4,5)+WR1(6,7)+carry

		To verify a checksum over frame or header data, load up to 8
		bytes into WR1 (1 cycle) then execute this instruction (1 cycle)
		and repeat as necessary. Once the entire header or frame has
		been included, the result in WR0(6,7) should = 0xFFFF if the
		checksum is valid.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_checksum16_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_return_sub

@Description	Returns from the previous Gosub call to the immediate next
		instruction location (This distance varies depending on which
		instruction issued the gosub.
		Currently only a stack depth of one is supported. An Invalid
		Soft Parser Instruction will be triggered if a Return is
		attempted when there was no previous Gosub

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_return_sub(void);

/**************************************************************************//**
@Function	sparser_gen_case1_dj_wr_to_wr

@Description	Compares, on equality, the least significant 16-bits of WR0 to
		the least significant 16-bits in WR1. It they are equal, jumps
		to the destination jmp_dst_1, otherwise (the default case) jumps
		to the destination jmp_dst_2.
		if (WR0[48:63] == WR1[48:63])
			jump to "jmp_dst_1"
		else
			jump to "jmp_dst_2"
		If the "GF" flag is configured on a jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		If the "AF" flag is configured on a jump destination, then the
		respective jump is treated as an advancement to a new hard or
		soft HXS (the HB is updated to HB + WO; WO, WR0 and WR1 are
		reset).
		A jump destination may be a "local" label.

@Param[in]	jmp_dst1 : Jump destination address if WR0[48:63] == WR1[48:63].

@Param[in]	jmp_dst2 : Jump destination address if WR0[48:63] =! WR1[48:63].

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_case1_dj_wr_to_wr(uint16_t jmp_dst_1, uint16_t jmp_dst_2);

/**************************************************************************//**
@Function	sparser_gen_case2_dc_wr_to_wr

@Description	Compares, on equality, the least significant 16-bits of WR0 to
		each of the two right most 16-bit values in WR1. Jumps to the
		destination corresponding to the first match. If no match,
		default is to continue.
		if (WR0[48:63] == WR1[48:63])
			jump to "jmp_dst_1"
		else if (WR0[48:63] == WR1[32:47])
			jump to "jmp_dst_2"
		else
			continues with the next instruction
		If the "GF" flag is configured on a jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		If the "AF" flag is configured on a jump destination, then the
		respective jump is treated as an advancement to a new hard or
		soft HXS (the HB is updated to HB + WO; WO, WR0 and WR1 are
		reset).
		A jump destination may be a "local" label.

@Param[in]	jmp_dst1 : Jump destination address if WR0[48:63] == WR1[48:63].

@Param[in]	jmp_dst2 : Jump destination address if WR0[48:63] == WR1[32:47].

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_case2_dc_wr_to_wr(uint16_t jmp_dst_1, uint16_t jmp_dst_2);

/**************************************************************************//**
@Function	sparser_gen_case2_dj_wr_to_wr

@Description	Compares, on equality, the least significant 16-bits of WR0 to
		each of the two right most 16-bit values in WR1. Jumps to the
		destination corresponding to the first match. If no match,
		default is to jump.
		if (WR0[48:63] == WR1[48:63])
			jump to "jmp_dst_1"
		else if (WR0[48:63] == WR1[32:47])
			jump to "jmp_dst_2"
		else
			jump to "jmp_dst_3"
		If the "GF" flag is configured on a jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		If the "AF" flag is configured on a jump destination, then the
		respective jump is treated as an advancement to a new hard or
		soft HXS (the HB is updated to HB + WO; WO, WR0 and WR1 are
		reset).
		A jump destination may be a "local" label.

@Param[in]	jmp_dst1 : Jump destination address if WR0[48:63] == WR1[48:63].

@Param[in]	jmp_dst2 : Jump destination address if WR0[48:63] == WR1[32:47].

@Param[in]	jmp_dst3 : Jump destination address if WR0[48:63] != WR1[32:47]
		and WR0[48:63] != WR1[32:47].

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_case2_dj_wr_to_wr(uint16_t jmp_dst_1, uint16_t jmp_dst_2,
				   uint16_t jmp_dst_3);

/**************************************************************************//**
@Function	sparser_gen_case3_dc_wr_to_wr

@Description	Compares, on equality, the least significant 16-bits of WR0 to
		each of the three right most 16-bit values in WR1. Jumps to the
		destination corresponding to the first match. If no match,
		default is to continue.
		if (WR0[48:63] == WR1[48:63])
			jump to "jmp_dst_1"
		else if (WR0[48:63] == WR1[32:47])
			jump to "jmp_dst_2"
		else if (WR0[48:63] == WR1[16:31])
			jump to "jmp_dst_3"
		else
			continues with the next instruction
		If the "GF" flag is configured on a jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		If the "AF" flag is configured on a jump destination, then the
		respective jump is treated as an advancement to a new hard or
		soft HXS (the HB is updated to HB + WO; WO, WR0 and WR1 are
		reset).
		A jump destination may be a "local" label.

@Param[in]	jmp_dst1 : Jump destination address if WR0[48:63] == WR1[48:63].

@Param[in]	jmp_dst2 : Jump destination address if WR0[48:63] == WR1[32:47].

@Param[in]	jmp_dst3 : Jump destination address if WR0[48:63] != WR1[32:47]
		and WR0[48:63] != WR1[32:47].

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_case3_dc_wr_to_wr(uint16_t jmp_dst_1, uint16_t jmp_dst_2,
				   uint16_t jmp_dst_3);

/**************************************************************************//**
@Function	sparser_gen_case3_dj_wr_to_wr

@Description	Compares, on equality, the least significant 16-bits of WR0 to
		each of the three right most 16-bit values in WR1. Jumps to the
		destination corresponding to the first match. If no match,
		default is to jump.
		if (WR0[48:63] == WR1[48:63])
			jump to "jmp_dst_1"
		else if (WR0[48:63] == WR1[32:47])
			jump to "jmp_dst_2"
		else if (WR0[48:63] == WR1[16:31])
			jump to "jmp_dst_3"
		else
			jump to "jmp_dst_4"
		If the "GF" flag is configured on a jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		If the "AF" flag is configured on a jump destination, then the
		respective jump is treated as an advancement to a new hard or
		soft HXS (the HB is updated to HB + WO; WO, WR0 and WR1 are
		reset).
		A jump destination may be a "local" label.

@Param[in]	jmp_dst1 : Jump destination address if WR0[48:63] == WR1[48:63].

@Param[in]	jmp_dst2 : Jump destination address if WR0[48:63] == WR1[32:47].

@Param[in]	jmp_dst3 : Jump destination address if WR0[48:63] == WR1[16:31].

@Param[in]	jmp_dst4 : Jump destination address if WR0[48:63] != WR1[32:47],
		WR0[48:63] != WR1[32:47] and WR0[48:63] != WR1[16:31].

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_case3_dj_wr_to_wr(uint16_t jmp_dst_1, uint16_t jmp_dst_2,
				   uint16_t jmp_dst_3, uint16_t jmp_dst_4);

/**************************************************************************//**
@Function	sparser_gen_case4_dc_wr_to_wr

@Description	Compares, on equality, the least significant 16-bits of WR0 to
		each of the four right most 16-bit values in WR1. Jumps to the
		destination corresponding to the first match. If no match,
		default is to continue.
		if (WR0[48:63] == WR1[48:63])
			jump to "jmp_dst_1"
		else if (WR0[48:63] == WR1[32:47])
			jump to "jmp_dst_2"
		else if (WR0[48:63] == WR1[16:31])
			jump to "jmp_dst_3"
		else if (WR0[48:63] == WR1[0:15])
			jump to "jmp_dst_4"
		else
			continues with the next instruction
		If the "GF" flag is configured on a jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		If the "AF" flag is configured on a jump destination, then the
		respective jump is treated as an advancement to a new hard or
		soft HXS (the HB is updated to HB + WO; WO, WR0 and WR1 are
		reset).
		A jump destination may be a "local" label.

@Param[in]	jmp_dst1 : Jump destination address if WR0[48:63] == WR1[48:63].

@Param[in]	jmp_dst2 : Jump destination address if WR0[48:63] == WR1[32:47].

@Param[in]	jmp_dst3 : Jump destination address if WR0[48:63] == WR1[16:31].

@Param[in]	jmp_dst4 : Jump destination address if WR0[48:63] == WR1[0:15].

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_case4_dc_wr_to_wr(uint16_t jmp_dst_1, uint16_t jmp_dst_2,
				   uint16_t jmp_dst_3, uint16_t jmp_dst_4);

/**************************************************************************//**
@Function	sparser_gen_jump_to_l2_protocol

@Description	Execution jumps to the HXS decoded in the NxtHdr field of the
		Parse Array. Used to jump to the next HXS based on a protocol
		code.
		The jump is treated as an advancement to a new HXS (the HB is
		updated to HB + WO; WO, WR0 and WR1 are reset).
		NxtHdr is evaluated against the following EtherTypes:
			0x05DC or less:
				jump to LLC-SNAP
			0x0800:
				jump to IPv4
			0x0806:
				jump to ARP
			0x86dd:
				jump to IPv6
			0x8847, 0x8848:
				jump to MPLS
			0x8100, 0x88A8,ConfigTPID1,ConfigTPID2:
				jump to VLAN
			0x8864:
				jump to PPPoE+PPP
			unknown:
				jump to Other L3 Shell

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_jump_to_l2_protocol(void);

/**************************************************************************//**
@Function	sparser_gen_jump_to_l3_protocol

@Description	Execution jumps to the HXS decoded in the NxtHdr field of the
		Parse Array. Used to jump to the next HXS based on a protocol
		code.
		The jump is treated as an advancement to a new HXS (the HB is
		updated to HB + WO; WO, WR0 and WR1 are reset).
		NxtHdr is evaluated against the following IP protocol codes:
			 4:		jump to IPv4
			 6:		jump to TCP
			17:		jump to UDP
			33:		jump to DCCP
			41:		jump to IPv6
			47:		jump to GRE
			50,51:		jump to IPSec
			55:		jump to MinEncap
			132:		jump to SCTP
			unknown:	jump to Other L4 Shell

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_jump_to_l3_protocol(void);

/**************************************************************************//**
@Function	sparser_gen_jump_to_l4_protocol

@Description	Execution jumps to the HXS decoded in the NxtHdr field of the
		Parse Array. Used to jump to the next HXS based on a protocol
		code.
		The jump is treated as an advancement to a new HXS (the HB is
		updated to HB + WO; WO, WR0 and WR1 are reset).
		NxtHdr is evaluated against the following TCP/UDP ports:
			2123:		jump to GTP(GTP-C)
			2152:		jump to GTP(GTP-U)
			3386:		jump to GTP(GTP)
			4500:		jump to ESP
			4789:		jump to VXLAN
			unknown:	jump to Other L5+ Shell

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_jump_to_l4_protocol(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr0_to_wr0

@Description	Adds the least significant 32 bits of WR0 to the least
		significant 32 bits of WR0 and stores the result into WR0.
		No carry bit is captured. The most significant 32 bits of WR0
		are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr0_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr0_to_wr1

@Description	Adds the least significant 32 bits of WR0 to the least
		significant 32 bits of WR1 and stores the result into WR1.
		No carry bit is captured. The most significant 32 bits of WR0
		and WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr0_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr1_to_wr1

@Description	Adds the least significant 32 bits of WR1 to the least
		significant 32 bits of WR1 and stores the result into WR1.
		No carry bit is captured. The most significant 32 bits of WR1
		are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr1_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr1_to_wr0

@Description	Adds the least significant 32 bits of WR1 to the least
		significant 32 bits of WR0 and stores the result into WR0.
		No carry bit is captured. The most significant 32 bits of WR0
		and WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr0_to_wr0

@Description	Subtracts the least significant 32 bits of WR0 from the least
		significant 32 bits of WR0 and stores the result into WR0.
		No underflow bit is captured. The most significant 32 bits of
		WR0 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr0_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr0_to_wr1

@Description	Subtracts the least significant 32 bits of WR0 from the least
		significant 32 bits of WR1 and stores the result into WR1.
		No underflow bit is captured. The most significant 32 bits of
		WR0 and WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr0_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr1_to_wr1

@Description	Subtracts the least significant 32 bits of WR1 from the least
		significant 32 bits of WR1 and stores the result into WR1.
		No underflow bit is captured. The most significant 32 bits of
		WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr1_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr1_to_wr0

@Description	Subtracts the least significant 32 bits of WR1 from the least
		significant 32 bits of WR0 and stores the result into WR0.
		No underflow bit is captured. The most significant 32 bits of
		WR0 and WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr0_imm_to_wr0

@Description	Adds the immediate 16/32 bits value to the least significant
		32 bits of WR0 and stores the result into WR0. No carry bit is
		captured. The most significant 32 bits of WR0 are not affected
		by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16 or SP_IMM_32).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr0_imm_to_wr0(uint32_t imm,
				      enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_add32_wr0_imm_to_wr1

@Description	Adds the immediate 16/32 bits value to the least significant
		32 bits of WR0 and stores the result into WR1. No carry bit is
		captured. The most significant 32 bits of WR0 and WR1 are not
		affected by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16 or SP_IMM_32).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr0_imm_to_wr1(uint32_t imm,
				      enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_add32_wr1_imm_to_wr1

@Description	Adds the immediate 16/32 bits value to the least significant
		32 bits of WR1 and stores the result into WR1. No carry bit is
		captured. The most significant 32 bits of WR1 are not affected
		by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16 or SP_IMM_32).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr1_imm_to_wr1(uint32_t imm,
				      enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_add32_wr1_imm_to_wr0

@Description	Adds the immediate 16/32 bits value to the least significant
		32 bits of WR1 and stores the result into WR0. No carry bit is
		captured. The most significant 32 bits of WR0 and WR1 are not
		affected by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16 or SP_IMM_32).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr1_imm_to_wr0(uint32_t imm,
				      enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr0_imm_to_wr0

@Description	Subtracts the immediate 16/32 bits value from the least
		significant 32 bits of WR0 and stores the result into WR0.
		No underflow bit is captured. The most significant 32 bits of
		WR0 are not affected by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16 or SP_IMM_32).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr0_imm_to_wr0(uint32_t imm,
				      enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr0_imm_to_wr1

@Description	Subtracts the immediate 16/32 bits value from the least
		significant 32 bits of WR0 and stores the result into WR1.
		No underflow bit is captured. The most significant 32 bits of
		WR0 and WR1 are not affected by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16 or SP_IMM_32).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr0_imm_to_wr1(uint32_t imm,
				      enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr1_imm_to_wr1

@Description	Subtracts the immediate 16/32 bits value from the least
		significant 32 bits of WR1 and stores the result into WR1.
		No underflow bit is captured. The most significant 32 bits of
		WR1 are not affected by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16 or SP_IMM_32).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr1_imm_to_wr1(uint32_t imm,
				      enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr1_imm_to_wr0

@Description	Subtracts the immediate 16/32 bits value from the least
		significant 32 bits of WR1 and stores the result into WR0.
		No underflow bit is captured. The most significant 32 bits of
		WR0 and WR1 are not affected by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16 or SP_IMM_32).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr1_imm_to_wr0(uint32_t imm,
				      enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_or_wr0_wr1_to_wr0

@Description	The 64-bit values contained in WR0 and WR1 are combined with
		the bitwise OR operation and assigned to the WR0.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_or_wr0_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_or_wr0_wr1_to_wr1

@Description	The 64-bit values contained in WR0 and WR1 are combined with
		the bitwise OR operation and assigned to the WR1.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_or_wr0_wr1_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_and_wr0_wr1_to_wr0

@Description	The 64-bit values contained in WR0 and WR1 are combined with
		the bitwise AND operation and assigned to the WR0.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_and_wr0_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_and_wr0_wr1_to_wr1

@Description	The 64-bit values contained in WR0 and WR1 are combined with
		the bitwise AND operation and assigned to the WR1.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_and_wr0_wr1_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_xor_wr0_wr1_to_wr0

@Description	The 64-bit values contained in WR0 and WR1 are combined with
		the bitwise XOR operation and assigned to the WR0.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_xor_wr0_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_xor_wr0_wr1_to_wr1

@Description	The 64-bit values contained in WR0 and WR1 are combined with
		the bitwise XOR operation and assigned to the WR1.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_xor_wr0_wr1_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_clr_wr0_wr1_to_wr0

@Description	No bitwise operation is performed. WR0 is cleared.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr0_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_clr_wr0_wr1_to_wr1

@Description	No bitwise operation is performed. WR1 is cleared.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr0_wr1_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_cmp_wr0_eq_wr1

@Description	Compares, on equality, WR0 with WR1. If WR0 equals WR1 then
		instructions execution will continue from the "jmp_dst" address,
		otherwise the next instruction will be executed.
		If the "GF" flag is configured on the jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		The jump destination may be a "local" label.

@Param[in]	jmp_dst : Jump destination address.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_cmp_wr0_eq_wr1(uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_cmp_wr0_ne_wr1

@Description	Compares, on non-equality, WR0 with WR1. If WR0 differs from WR1
		then instructions execution will continue from the "jmp_dst"
		address, otherwise the next instruction will be executed.
		If the "GF" flag is configured on the jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		The jump destination may be a "local" label.

@Param[in]	jmp_dst : Jump destination address.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_cmp_wr0_ne_wr1(uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_cmp_wr0_gt_wr1

@Description	Checks if WR0 is greater than WR1. If this is true, then
		instructions execution will continue from the "jmp_dst"
		address, otherwise the next instruction will be executed.
		If the "GF" flag is configured on the jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		The jump destination may be a "local" label.

@Param[in]	jmp_dst : Jump destination address.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_cmp_wr0_gt_wr1(uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_cmp_wr0_lt_wr1

@Description	Checks if WR0 is less than WR1. If this is true, then
		instructions execution will continue from the "jmp_dst"
		address, otherwise the next instruction will be executed.
		If the "GF" flag is configured on the jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		The jump destination may be a "local" label.

@Param[in]	jmp_dst : Jump destination address.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_cmp_wr0_lt_wr1(uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_cmp_wr0_ge_wr1

@Description	Checks if WR0 is greater than or equals WR1. If this is true,
		then instructions execution will continue from the "jmp_dst"
		address, otherwise the next instruction will be executed.
		If the "GF" flag is configured on the jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		The jump destination may be a "local" label.

@Param[in]	jmp_dst : Jump destination address.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_cmp_wr0_ge_wr1(uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_cmp_wr0_le_wr1

@Description	Checks if WR0 is less than or equals WR1. If this is true,
		then instructions execution will continue from the "jmp_dst"
		address, otherwise the next instruction will be executed.
		If the "GF" flag is configured on the jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		The jump destination may be a "local" label.

@Param[in]	jmp_dst : Jump destination address.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_cmp_wr0_le_wr1(uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_cmp_wr0_nop_wr1

@Description	No comparison operation is performed between WR0 and WR1.
		Instructions execution will continue with the next instruction.

@Param[in]	jmp_dst : Jump destination address (does not care but must be
		valid).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_cmp_wr0_nop_wr1(uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_ld_wr0_to_wo

@Description	Loads the least significant 8 bits of WR0 into the Window
		Offset.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_wr0_to_wo(void);

/**************************************************************************//**
@Function	sparser_gen_ld_wr1_to_wo

@Description	Loads the least significant 8 bits of WR1 into the Window
		Offset.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_wr1_to_wo(void);

/**************************************************************************//**
@Function	sparser_gen_add_wr0_to_wo

@Description	Adds the least significant 8 bits of WR0 to the Window
		Offset.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add_wr0_to_wo(void);

/**************************************************************************//**
@Function	sparser_gen_add_wr1_to_wo

@Description	Adds the least significant 8 bits of WR1 to the Window
		Offset.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add_wr1_to_wo(void);

/**************************************************************************//**
@Function	sparser_gen_or_wr0_imm_to_wr0

@Description	The 64-bit value contained in WR0 is combined, using the OR
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR0.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_or_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_or_wr0_imm_to_wr1

@Description	The 64-bit value contained in WR0 is combined, using the OR
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR1.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_or_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_or_wr1_imm_to_wr1

@Description	The 64-bit value contained in WR1 is combined, using the OR
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR1.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_or_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_or_wr1_imm_to_wr0

@Description	The 64-bit value contained in WR1 is combined, using the OR
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR0.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_or_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_and_wr0_imm_to_wr0

@Description	The 64-bit value contained in WR0 is combined, using the AND
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR0.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_and_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_and_wr0_imm_to_wr1

@Description	The 64-bit value contained in WR0 is combined, using the AND
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR1.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_and_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_and_wr1_imm_to_wr1

@Description	The 64-bit value contained in WR1 is combined, using the AND
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR1.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_and_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_and_wr1_imm_to_wr0

@Description	The 64-bit value contained in WR1 is combined, using the AND
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR0.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_and_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_and_wr0_imm_to_wr0

@Description	The 64-bit value contained in WR0 is combined, using the AND
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR0.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_xor_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_xor_wr0_imm_to_wr1

@Description	The 64-bit value contained in WR0 is combined, using the XOR
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR1.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_xor_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_xor_wr1_imm_to_wr1

@Description	The 64-bit value contained in WR1 is combined, using the XOR
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR1.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_xor_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_xor_wr1_imm_to_wr0

@Description	The 64-bit value contained in WR1 is combined, using the XOR
		bitwise function, with an immediate value of 16, 32, 48 or 64
		bits (left padded with zeros to 64 bits when necessary).
		The 64-bit result is assigned to the WR0.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_xor_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_clr_wr0_imm_to_wr0

@Description	No bitwise operation is performed. WR0 is cleared.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_clr_wr0_imm_to_wr1

@Description	No bitwise operation is performed. WR1 is cleared.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_clr_wr1_imm_to_wr1

@Description	No bitwise operation is performed. WR1 is cleared.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_clr_wr1_imm_to_wr0

@Description	No bitwise operation is performed. WR0 is cleared.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_shl_wr0_by_sv

@Description	Shifts WR0 to the left by 1 to 64 bits. Zeros are shifted into
		the least significant bits.

@Param[in]	shift : Shift value (1 to 64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_shl_wr0_by_sv(uint8_t shift);

/**************************************************************************//**
@Function	sparser_gen_shl_wr1_by_sv

@Description	Shifts WR1 to the left by 1 to 64 bits. Zeros are shifted into
		the least significant bits.

@Param[in]	shift : Shift value (1 to 64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_shl_wr1_by_sv(uint8_t shift);

/**************************************************************************//**
@Function	sparser_gen_shr_wr0_by_sv

@Description	Shifts WR0 to the right by 1 to 64 bits. Zeros are shifted into
		the most significant bits.

@Param[in]	shift : Shift value (1 to 64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_shr_wr0_by_sv(uint8_t shift);

/**************************************************************************//**
@Function	sparser_gen_shr_wr1_by_sv

@Description	Shifts WR1 to the right by 1 to 64 bits. Zeros are shifted into
		the most significant bits.

@Param[in]	shift : Shift value (1 to 64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_shr_wr1_by_sv(uint8_t shift);

/**************************************************************************//**
@Function	sparser_gen_ld_imm_bits_to_wr0

@Description	Stores the "n" least significant bits of the immediate value of
		16, 32, 48 or 64 bits to the "n" least significant bits of WR0.
		The bits to the left of the "n" bits are zeroed in WR0.

@Param[in]	n : Number of bits to load (1 to 16, 1 to 32, 1 to 48 or 1 to
		64 depending on the size of the immediate value).

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_imm_bits_to_wr0(uint8_t n, uint64_t imm,
				    enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_lds_imm_bits_to_wr0

@Description	The current value of WR0 is shifted to the left by "n" bits and
		then the "n" least significant bits of the immediate value of
		16, 32, 48 or 64 bits are stored to the "n" least significant
		bits of WR0.

@Param[in]	n : Number of bits to load (1 to 16, 1 to 32, 1 to 48 or 1 to
		64 depending on the size of the immediate value).

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_lds_imm_bits_to_wr0(uint8_t n, uint64_t imm,
				     enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_ld_imm_bits_to_wr1

@Description	Stores the "n" least significant bits of the immediate value of
		16, 32, 48 or 64 bits to the "n" least significant bits of WR1.
		The bits to the left of the "n" bits are zeroed in WR1.

@Param[in]	n : Number of bits to load (1 to 16, 1 to 32, 1 to 48 or 1 to
		64 depending on the size of the immediate value).

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_imm_bits_to_wr1(uint8_t n, uint64_t imm,
				    enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_lds_imm_bits_to_wr1

@Description	The current value of WR1 is shifted to the left by "n" bits and
		then the "n" least significant bits of the immediate value of
		16, 32, 48 or 64 bits are stored to the "n" least significant
		bits of WR1.

@Param[in]	n : Number of bits to load (1 to 16, 1 to 32, 1 to 48 or 1 to
		64 depending on the size of the immediate value).

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_lds_imm_bits_to_wr1(uint8_t n, uint64_t imm,
				     enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_set_faf_bit

@Description	Sets the Frame Attribute Flags at the indicated position.

@Param[in]	faf_bit : Frame Attribute Flag bit (a "sparser_faf_bit"
		enumerated value).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_set_faf_bit(enum sparser_faf_bit faf_bit);

/**************************************************************************//**
@Function	sparser_gen_clear_faf_bit

@Description	Clears the Frame Attribute Flags at the indicated position.

@Param[in]	faf_bit : Frame Attribute Flag bit (a "sparser_faf_bit"
		enumerated value).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clear_faf_bit(enum sparser_faf_bit faf_bit);

/**************************************************************************//**
@Function	sparser_gen_ld_sv_to_wo

@Description	Loads a 8-bit value into the Window Offset register.

@Param[in]	sv : 8-bit value.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_sv_to_wo(uint8_t sv);

/**************************************************************************//**
@Function	sparser_gen_add_sv_to_wo

@Description	Adds a 8-bit value to the Window Offset register.

@Param[in]	sv : 8-bit value.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add_sv_to_wo(uint8_t sv);

/**************************************************************************//**
@Function	sparser_gen_st_imm_bytes_to_ra

@Description	Stores "n" bytes, from an immediate value, into the Parse Array
		starting at the "pos" byte positions.

@Param[in]	pos : Starting position in the Parse Array

@Param[in]	n : Number of bytes (1 to 8)

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size (SP_IMM_16/SP_IMM_32/SP_IMM_48/
		SP_IMM_64).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_st_imm_bytes_to_ra(uint8_t pos, uint8_t n, uint64_t imm,
				    enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_jmp_faf

@Description	Execution jumps to the specified instruction if the flag indexed
		by "faf_bit" in the Frame Attribute Flags is set. If the flag is
		cleared execution continues with the next instruction.
		If the "GF" flag is configured on the jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		If the "AF" flag is configured on the jump destination, then the
		respective jump is treated as an advancement to a new hard or
		soft HXS (the HB is updated to HB + WO; WO, WR0 and WR1 are
		reset).
		A jump destination may be a "local" label.

@Param[in]	faf_bit : Frame Attribute Flag bit (a "sparser_faf_bit"
		enumerated value).

@Param[in]	jmp_dst : Jump destination PC.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_jmp_faf(enum sparser_faf_bit faf_bit, uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_ld_pa_to_wr0

@Description	Loads "n" bytes into the WR0, from the "pos" byte position of
		the Parameters Array.

@Param[in]	pos : Starting byte position in Parameters Array (0 to 63).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_pa_to_wr0(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_lds_pa_to_wr0

@Description	Loads "n" bytes into the WR0, from the "pos" byte position of
		the Parameters Array.
		Before loading, the WR0 register is shifted to the left with
		"8 * n" positions

@Param[in]	pos : Starting byte position in Parameters Array (0 to 63).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_lds_pa_to_wr0(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_ld_pa_to_wr1

@Description	Loads "n" bytes into the WR1, from the "pos" byte position of
		the Parameters Array.

@Param[in]	pos : Starting byte position in Parameters Array (0 to 63).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_pa_to_wr1(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_lds_pa_to_wr1

@Description	Loads "n" bytes into the WR1, from the "pos" byte position of
		the Parameters Array.
		Before loading, the WR1 register is shifted to the left with
		"8 * n" positions

@Param[in]	pos : Starting byte position in Parameters Array (0 to 63).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_lds_pa_to_wr1(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_jmp

@Description	Execution jumps to the specified instruction address.
		If the "GF" flag is configured on the jump destination, then the
		jump is treated as a gosub.
		If the "LF" flag is configured on a jump destination, then the
		jump destination is treated as a relative address (otherwise it
		is an absolute address). If a relative negative jump is to be
		performed, the SF flag must be configured.
		If the "AF" flag is configured on the jump destination, then the
		respective jump is treated as an advancement to a new hard or
		soft HXS (the HB is updated to HB + WO; WO, WR0 and WR1 are
		reset).
		A jump destination may be a "local" label.

@Param[in]	jmp_dst : Jump destination PC.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_jmp(uint16_t jmp_dst);

/**************************************************************************//**
@Function	sparser_gen_st_wr0_to_ra

@Description	Stores the leftmost "n" bytes of WR0, into the Parse Array at
		the "pos" starting byte position.

@Param[in]	pos : Starting byte position in Parse Array (0 to 127).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_st_wr0_to_ra(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_st_wr1_to_ra

@Description	Stores the leftmost "n" bytes of WR1, into the Parse Array at
		the "pos" starting byte position.

@Param[in]	pos : Starting byte position in Parse Array (0 to 127).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_st_wr1_to_ra(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_ld_ra_to_wr0

@Description	Loads "n" bytes into the WR0, from the "pos" byte position of
		the Parse Array.

@Param[in]	pos : Starting byte position in Parse Array (0 to 127).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_ra_to_wr0(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_lds_ra_to_wr0

@Description	Loads "n" bytes into the WR0, from the "pos" byte position of
		the Parse Array.
		Before loading, the WR0 register is shifted to the left with
		"8 * n" positions

@Param[in]	pos : Starting byte position in Parse Array (0 to 127).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_lds_ra_to_wr0(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_ld_ra_to_wr1

@Description	Loads "n" bytes into the WR1, from the "pos" byte position of
		the Parse Array.

@Param[in]	pos : Starting byte position in Parse Array (0 to 127).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_ra_to_wr1(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_lds_ra_to_wr1

@Description	Loads "n" bytes into the WR1, from the "pos" byte position of
		the Parse Array.
		Before loading, the WR1 register is shifted to the left with
		"8 * n" positions

@Param[in]	pos : Starting byte position in Parse Array (0 to 127).

@Param[in]	n : Number of bytes to load (1 to 8).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_lds_ra_to_wr1(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_ld_fw_to_wr0

@Description	Loads "n" bits into the WR0, from the "pos" bit position of the
		Frame Window.

@Param[in]	pos : Starting bit position in Frame Window (0 to 127).

@Param[in]	n : Number of bits to load (1 to 128).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_fw_to_wr0(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_lds_fw_to_wr0

@Description	Loads "n" bits into WR0, from the "pos" bit position of the
		Frame Window.
		Before loading, the WR0 register is shifted to the left with
		"n" positions

@Param[in]	pos : Starting bit position in Frame Window (0 to 127).

@Param[in]	n : Number of bits to load (1 to 128).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_lds_fw_to_wr0(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_ld_fw_to_wr1

@Description	Loads "n" bits into WR1 from the "pos" bit position of the
		Frame Window.

@Param[in]	pos : Starting bit position in Frame Window (0 to 127).

@Param[in]	n : Number of bits to load (1 to 128).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_ld_fw_to_wr1(uint8_t pos, uint8_t n);

/**************************************************************************//**
@Function	sparser_gen_lds_fw_to_wr1

@Description	Loads "n" bits into WR1 from the "pos" bit position of the
		Frame Window.
		Before loading, the WR1 register is shifted to the left with
		"n" positions

@Param[in]	pos : Starting bit position in Frame Window (0 to 127).

@Param[in]	n : Number of bits to load (1 to 128).

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_lds_fw_to_wr1(uint8_t pos, uint8_t n);

/******************************************************************************/
/* Macro definitions */
#define SPARSER_BEGIN(_pc, _sp, _sz) sparser_begin_bytecode_wrt(_pc, _sp, _sz)

#define SPARSER_DUMP	sparser_bytecode_dump()

#define SPARSER_END	sparser_end_bytecode_wrt()

#define SP_LABEL(_a) sparser_set_label(_a)

/******************************************************************************/
/* 0:NOP */
#define NOP sparser_gen_nop()

/******************************************************************************/
/* 1:ADVANCE_HB_BY_WO */
#define ADVANCE_HB_BY_WO sparser_gen_advance_hb_by_wo()

/******************************************************************************/
/* 2:CLR_WRx */
#define CLR_WR0 sparser_gen_clr_wr0()

#define CLR_WR1 sparser_gen_clr_wr1()

/******************************************************************************/
/* 3:CHECKSUM16_WR1_TO_WR0 */
#define CHECKSUM16_WR1_TO_WR0 sparser_gen_checksum16_wr1_to_wr0()

/******************************************************************************/
/* 4:RETURN_SUB*/
#define RETURN_SUB sparser_gen_return_sub()

/******************************************************************************/
/* 5: .. 10 :CASEx_DJ_WR_to_WR
 * CASEx_DC_WR_to_WR */
#define CASE1_DJ_WR_to_WR(_jmp_dst_1, _jmp_dst_2)		\
		sparser_gen_case1_dj_wr_to_wr(_jmp_dst_1, _jmp_dst_2)

#define CASE2_DC_WR_to_WR(_jmp_dst_1, _jmp_dst_2)		\
		sparser_gen_case2_dc_wr_to_wr(_jmp_dst_1, _jmp_dst_2)

#define CASE2_DJ_WR_to_WR(_jmp_dst_1, _jmp_dst_2, _jmp_dst_3)		\
		sparser_gen_case2_dj_wr_to_wr(_jmp_dst_1, _jmp_dst_2,	\
					      _jmp_dst_3)

#define CASE3_DC_WR_to_WR(_jmp_dst_1, _jmp_dst_2, _jmp_dst_3)		\
		sparser_gen_case3_dc_wr_to_wr(_jmp_dst_1, _jmp_dst_2,	\
					      _jmp_dst_3)

#define CASE3_DJ_WR_to_WR(_jmp_dst_1, _jmp_dst_2, _jmp_dst_3, _jmp_dst_4) \
		sparser_gen_case3_dj_wr_to_wr(_jmp_dst_1, _jmp_dst_2,	  \
					      _jmp_dst_3, _jmp_dst_4)

#define CASE4_DC_WR_to_WR(_jmp_dst_1, _jmp_dst_2, _jmp_dst_3, _jmp_dst_4) \
		sparser_gen_case4_dc_wr_to_wr(_jmp_dst_1, _jmp_dst_2,	  \
					      _jmp_dst_3, _jmp_dst_4)

/******************************************************************************/
/* 11:JMP_TO_Lx_PROTOCOL */
#define JMP_TO_L2_PROTOCOL sparser_gen_jump_to_l2_protocol()

#define JMP_TO_L3_PROTOCOL sparser_gen_jump_to_l3_protocol()

#define JMP_TO_L4_PROTOCOL sparser_gen_jump_to_l4_protocol()

/******************************************************************************/
/* 12:ADD32_WRx_TO_WRx
 * SUB32_WRx_TO_WRx */
#define ADD32_WR0_TO_WR0 sparser_gen_add32_wr0_to_wr0()

#define ADD32_WR0_TO_WR1 sparser_gen_add32_wr0_to_wr1()

#define ADD32_WR1_TO_WR1 sparser_gen_add32_wr1_to_wr1()

#define ADD32_WR1_TO_WR0 sparser_gen_add32_wr1_to_wr0()

#define SUB32_WR0_TO_WR0 sparser_gen_sub32_wr0_to_wr0()

#define SUB32_WR0_TO_WR1 sparser_gen_sub32_wr0_to_wr1()

#define SUB32_WR1_TO_WR1 sparser_gen_sub32_wr1_to_wr1()

#define SUB32_WR1_TO_WR0 sparser_gen_sub32_wr1_to_wr0()

/******************************************************************************/
/* 13:ADD32_WRx_IMMxx_TO_WRx
 * SUB32_WRx_IMMxx_TO_WRx */
#define ADD32_WR0_IMM_TO_WR0(_imm, _imm_sz)	\
	sparser_gen_add32_wr0_imm_to_wr0(_imm, _imm_sz)

#define ADD32_WR0_IMM_TO_WR1(_imm, _imm_sz)	\
	sparser_gen_add32_wr0_imm_to_wr1(_imm, _imm_sz)

#define ADD32_WR1_IMM_TO_WR1(_imm, _imm_sz)	\
	sparser_gen_add32_wr1_imm_to_wr1(_imm, _imm_sz)

#define ADD32_WR1_IMM_TO_WR0(_imm, _imm_sz)	\
	sparser_gen_add32_wr1_imm_to_wr0(_imm, _imm_sz)

#define SUB32_WR0_IMM_TO_WR0(_imm, _imm_sz)	\
	sparser_gen_sub32_wr0_imm_to_wr0(_imm, _imm_sz)

#define SUB32_WR0_IMM_TO_WR1(_imm, _imm_sz)	\
	sparser_gen_sub32_wr0_imm_to_wr1(_imm, _imm_sz)

#define SUB32_WR1_IMM_TO_WR1(_imm, _imm_sz)	\
	sparser_gen_sub32_wr1_imm_to_wr1(_imm, _imm_sz)

#define SUB32_WR1_IMM_TO_WR0(_imm, _imm_sz)	\
	sparser_gen_sub32_wr1_imm_to_wr0(_imm, _imm_sz)

/******************************************************************************/
/* 14:OR_WR0_WR1_TO_WRx
 * AND_WR0_WR1_TO_WRx
 * XOR_WR0_WR1_TO_WRx
 * CLR_WR0_WR1_TO_WRx */
#define OR_WR0_WR1_TO_WR0 sparser_gen_or_wr0_wr1_to_wr0()
#define OR_WR0_WR1_TO_WR1 sparser_gen_or_wr0_wr1_to_wr1()

#define AND_WR0_WR1_TO_WR0 sparser_gen_and_wr0_wr1_to_wr0()
#define AND_WR0_WR1_TO_WR1 sparser_gen_and_wr0_wr1_to_wr1()

#define XOR_WR0_WR1_TO_WR0 sparser_gen_xor_wr0_wr1_to_wr0()
#define XOR_WR0_WR1_TO_WR1 sparser_gen_xor_wr0_wr1_to_wr1()

#define CLR_WR0_WR1_TO_WR0 sparser_gen_clr_wr0_wr1_to_wr0()
#define CLR_WR0_WR1_TO_WR1 sparser_gen_clr_wr0_wr1_to_wr1()

/******************************************************************************/
/* 15:CMP_WR0_xx_WR1 */
#define CMP_WR0_EQ_WR1(_jmp_dst) sparser_gen_cmp_wr0_eq_wr1(_jmp_dst)

#define CMP_WR0_NE_WR1(_jmp_dst) sparser_gen_cmp_wr0_ne_wr1(_jmp_dst)

#define CMP_WR0_GT_WR1(_jmp_dst) sparser_gen_cmp_wr0_gt_wr1(_jmp_dst)

#define CMP_WR0_LT_WR1(_jmp_dst) sparser_gen_cmp_wr0_lt_wr1(_jmp_dst)

#define CMP_WR0_GE_WR1(_jmp_dst) sparser_gen_cmp_wr0_ge_wr1(_jmp_dst)

#define CMP_WR0_LE_WR1(_jmp_dst) sparser_gen_cmp_wr0_le_wr1(_jmp_dst)

#define CMP_WR0_NOP_WR1(_jmp_dst) sparser_gen_cmp_wr0_nop_wr1(_jmp_dst)

/******************************************************************************/
/* 16:LD_WR1_TO_WO
ADD_WR0_TO_WO */
#define LD_WR0_TO_WO sparser_gen_ld_wr0_to_wo()

#define LD_WR1_TO_WO sparser_gen_ld_wr1_to_wo()

#define ADD_WR0_TO_WO sparser_gen_add_wr0_to_wo()

#define ADD_WR1_TO_WO sparser_gen_add_wr1_to_wo()

/******************************************************************************/
/* 17:OR_WRx_IMM_TO_WRx
 * AND_WRx_IMM_TO_WRx
 * XOR_WRx_IMM_TO_WRx
 * CLR_WRx_IMM_TO_WRx */
#define OR_WR0_IMM_TO_WR0(_imm, _sz) sparser_gen_or_wr0_imm_to_wr0(_imm, _sz)
#define OR_WR0_IMM_TO_WR1(_imm, _sz) sparser_gen_or_wr0_imm_to_wr1(_imm, _sz)
#define OR_WR1_IMM_TO_WR1(_imm, _sz) sparser_gen_or_wr1_imm_to_wr1(_imm, _sz)
#define OR_WR1_IMM_TO_WR0(_imm, _sz) sparser_gen_or_wr1_imm_to_wr0(_imm, _sz)

#define AND_WR0_IMM_TO_WR0(_imm, _sz) sparser_gen_and_wr0_imm_to_wr0(_imm, _sz)
#define AND_WR0_IMM_TO_WR1(_imm, _sz) sparser_gen_and_wr0_imm_to_wr1(_imm, _sz)
#define AND_WR1_IMM_TO_WR1(_imm, _sz) sparser_gen_and_wr1_imm_to_wr1(_imm, _sz)
#define AND_WR1_IMM_TO_WR0(_imm, _sz) sparser_gen_and_wr1_imm_to_wr0(_imm, _sz)

#define XOR_WR0_IMM_TO_WR0(_imm, _sz) sparser_gen_xor_wr0_imm_to_wr0(_imm, _sz)
#define XOR_WR0_IMM_TO_WR1(_imm, _sz) sparser_gen_xor_wr0_imm_to_wr1(_imm, _sz)
#define XOR_WR1_IMM_TO_WR1(_imm, _sz) sparser_gen_xor_wr1_imm_to_wr1(_imm, _sz)
#define XOR_WR1_IMM_TO_WR0(_imm, _sz) sparser_gen_xor_wr1_imm_to_wr0(_imm, _sz)

#define CLR_WR0_IMM_TO_WR0(_imm, _sz) sparser_gen_clr_wr0_imm_to_wr0(_imm, _sz)
#define CLR_WR0_IMM_TO_WR1(_imm, _sz) sparser_gen_clr_wr0_imm_to_wr1(_imm, _sz)
#define CLR_WR1_IMM_TO_WR1(_imm, _sz) sparser_gen_clr_wr1_imm_to_wr1(_imm, _sz)
#define CLR_WR1_IMM_TO_WR0(_imm, _sz) sparser_gen_clr_wr1_imm_to_wr0(_imm, _sz)

/******************************************************************************/
/* 18:SHL_WRx_BY_SV */
#define SHL_WR0_BY_SV(_sv) sparser_gen_shl_wr0_by_sv(_sv)
#define SHL_WR1_BY_SV(_sv) sparser_gen_shl_wr1_by_sv(_sv)

/******************************************************************************/
/* 19:SHR_WRx_BY_SV */
#define SHR_WR0_BY_SV(_sv) sparser_gen_shr_wr0_by_sv(_sv)
#define SHR_WR1_BY_SV(_sv) sparser_gen_shr_wr1_by_sv(_sv)

/******************************************************************************/
/* 20:LDx_IMM_BITS_TO_WRx */
#define LD_IMM_BITS_TO_WR0(_n, _imm, _imm_sz)		\
	sparser_gen_ld_imm_bits_to_wr0(_n, _imm, _imm_sz)

#define LDS_IMM_BITS_TO_WR0(_n, _imm, _imm_sz)		\
	sparser_gen_lds_imm_bits_to_wr0(_n, _imm, _imm_sz)

#define LD_IMM_BITS_TO_WR1(_n, _imm, _imm_sz)		\
	sparser_gen_ld_imm_bits_to_wr1(_n, _imm, _imm_sz)

#define LDS_IMM_BITS_TO_WR1(_n, _imm, _imm_sz)		\
	sparser_gen_lds_imm_bits_to_wr1(_n, _imm, _imm_sz)

/******************************************************************************/
/* 21:SET_FAF_BIT
 * CLEAR_FAF_BIT */
#define SET_FAF_BIT(_pos) sparser_gen_set_faf_bit(_pos)

#define CLR_FAF_BIT(_pos) sparser_gen_clear_faf_bit(_pos)

/******************************************************************************/
/* 22:LD_SV_TO_WO */
#define LD_SV_TO_WO(_sv) sparser_gen_ld_sv_to_wo(_sv)

/******************************************************************************/
/* 23:ADD_SV_TO_WO */
#define ADD_SV_TO_WO(_sv) sparser_gen_add_sv_to_wo(_sv)

/******************************************************************************/
/* 24:ST_IMM_BYTES_TO_RA */
#define ST_IMM_BYTES_TO_RA(_pos, _n, _imm, _imm_sz)	\
	sparser_gen_st_imm_bytes_to_ra(_pos, _n, _imm, _imm_sz)

/******************************************************************************/
/* 25:JMP_FAF */
#define JMP_FAF(_pos, _jmp_dst)		sparser_gen_jmp_faf(_pos, _jmp_dst)

/******************************************************************************/
/* 26:LDx_PA_TO_WRx */
#define LD_PA_TO_WR0(_pos, _n) sparser_gen_ld_pa_to_wr0(_pos, _n)

#define LDS_PA_TO_WR0(_pos, _n) sparser_gen_lds_pa_to_wr0(_pos, _n)

#define LD_PA_TO_WR1(_pos, _n) sparser_gen_ld_pa_to_wr1(_pos, _n)

#define LDS_PA_TO_WR1(_pos, _n) sparser_gen_lds_pa_to_wr1(_pos, _n)

/******************************************************************************/
/* 27:JMP */
#define JMP(_jmp_dst) sparser_gen_jmp(_jmp_dst)

/******************************************************************************/
/* 28:ST_WRx_TO_RA */
#define ST_WR0_TO_RA(_pos, _n) sparser_gen_st_wr0_to_ra(_pos, _n)

#define ST_WR1_TO_RA(_pos, _n) sparser_gen_st_wr1_to_ra(_pos, _n)

/******************************************************************************/
/* 28:LDx_RA_TO_WRx */
#define LD_RA_TO_WR0(_pos, _n) sparser_gen_ld_ra_to_wr0(_pos, _n)

#define LDS_RA_TO_WR0(_pos, _n) sparser_gen_lds_ra_to_wr0(_pos, _n)

#define LD_RA_TO_WR1(_pos, _n) sparser_gen_ld_ra_to_wr1(_pos, _n)

#define LDS_RA_TO_WR1(_pos, _n) sparser_gen_lds_ra_to_wr1(_pos, _n)

/******************************************************************************/
/* 30:LDx_FW_TO_WRx */
#define LD_FW_TO_WR0(_pos, _n) sparser_gen_ld_fw_to_wr0(_pos, _n)

#define LDS_FW_TO_WR0(_pos, _n) sparser_gen_lds_fw_to_wr0(_pos, _n)

#define LD_FW_TO_WR1(_pos, _n) sparser_gen_ld_fw_to_wr1(_pos, _n)

#define LDS_FW_TO_WR1(_pos, _n) sparser_gen_lds_fw_to_wr1(_pos, _n)

/** @} */ /* end of sparser_gen_g SPARSER GEN group */
#endif		/* __FSL_SPARSER_GEN_H */
