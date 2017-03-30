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
/* Local Labels Base Address. The relative addressing mode is forced. Do not
 * change this definition */
#define SPARSER_LOC_LABEL_BASE	0x4800

/**************************************************************************//**
@Description	 enum sparser_local_label - 16 Local Labels

*//***************************************************************************/
enum sparser_local_label {
	/* Local Label 1 */
	sp_label_1 = SPARSER_LOC_LABEL_BASE,
	sp_label_2,
	sp_label_3,
	sp_label_4,
	sp_label_5,
	sp_label_6,
	sp_label_7,
	sp_label_8,
	sp_label_9,
	sp_label_10,
	sp_label_11,
	sp_label_12,
	sp_label_13,
	sp_label_14,
	sp_label_15,
	/* Local Label 16 */
	sp_label_16
};

/** @} end of group SPARSER_LOC_LABELS */

/**************************************************************************//**
@enum	sparser_hxs_dst

@Description Soft Parser HXS jump destinations

@{
*//***************************************************************************/
enum sparser_hxs_dst {
	/* Ethernet HXS destination */
	sp_eth_hxs_dst = 0x000,
	/* LLC+SNAP HXS destination */
	sp_llc_snap_hxs_dst = 0x001,
	/* VLAN HXS destination */
	sp_vlan_hxs_dst = 0x002,
	/* PPPoE+PPP HXS destination */
	sp_pppoe_ppp_hxs_dst = 0x003,
	/* MPLS HXS destination */
	sp_mpls_hxs_dst = 0x004,
	/* ARP HXS destination */
	sp_arp_hxs_dst = 0x005,
	/* IP HXS destination */
	sp_ip_hxs_dst = 0x006,
	/* IPv4 HXS destination */
	sp_ipv4_hxs_dst = 0x007,
	/* IPv6 HXS destination */
	sp_ipv6_hxs_dst = 0x008,
	/* GRE HXS destination */
	sp_gre_hxs_dst = 0x009,
	/* MinEncap HXS destination */
	sp_minencap_hxs_dst = 0x00a,
	/* Other L3 Shell HXS destination */
	sp_other_l3_shell_hxs_dst = 0x00b,
	/* TCP HXS destination */
	sp_tcp_hxs_dst = 0x00c,
	/* UDP HXS destination */
	sp_udp_hxs_dst = 0x00d,
	/* IPSec HXS destination */
	sp_ipsec_hxs_dst = 0x00e,
	/* SCTP HXS destination */
	sp_sctp_hxs_dst = 0x00f,
	/* DCCP HXS destination */
	sp_dccp_hxs_dst = 0x010,
	/* Other L4 Shell HXS destination */
	sp_other_l4_shell_hxs_dst = 0x011,
	/* GTP HXS destination */
	sp_gtp_hxs_dst = 0x012,
	/* ESP HXS destination */
	sp_esp_hxs_dst = 0x013,
	/* VXLAN HXS destination */
	sp_vxlan_hxs_dst = 0x014,
	/* L5 (and above) Shell HXS destination */
	sp_l5_shell_hxs_dst = 0x01e,
	/* Final Shell HXS destination */
	sp_final_shell_hxs_dst = 0x01f,
	/* Return to hard HXS destination */
	sp_return_to_hxs_dst = 0x7fe,
	/* End Parsing HXS destination */
	sp_end_parsing_hxs_dst = 0x7ff,
};

/** @} end of group sparser_hxs_dst */

/**************************************************************************//**
@enum	sparser_imm_val

@Description Soft Parser Immediate Value

@{
*//***************************************************************************/
enum sparser_imm_val {
	/* 16 bits immediate value */
	sp_imm_16 = 1,
	/* 32 bits immediate value */
	sp_imm_32,
	/* 48 bits immediate value */
	sp_imm_48,
	/* 64 bits immediate value */
	sp_imm_64
};

/** @} end of group sparser_imm_val */

/**************************************************************************//**
@enum	sparser_faf_bit

@Description Soft Parser FAF bits offset

@{
*//***************************************************************************/
enum sparser_faf_bit {
	/* Routing header present in IPv6 header 2 */
	sp_faf_ipv6_route_hdr2_present = 0,
	/* GTP Primed was detected */
	sp_faf_ipv6_gtp_primed_detected = 1,
	/* VLAN with VID = 0 was detected */
	sp_faf_vlan_with_vid_0_detected = 2,
	/* A PTP frame was detected */
	sp_faf_ptp_detected = 3,
	/* VXLAN was parsed */
	sp_faf_vxlan_present = 4,
	/* A VXLAN HXS parsing error was detected */
	sp_faf_vxlan_parse_err = 5,
	/* Ethernet control protocol (MAC DA is
	 * 01:80:C2:00:00:00-01:80:C2:00:00:00:FF) */
	sp_faf_eth_slow_proto_detected = 6,
	/* IKE was detected at UDP port 4500 */
	sp_faf_ike_present = 7,
	/* Shim Shell Soft Parsing Error */
	sp_faf_shim_soft_parse_err = 8,
	/* Parsing Error */
	sp_faf_parsing_err = 9,
	/* Ethernet MAC Present */
	sp_faf_eth_mac_present = 10,
	/* Ethernet Unicast */
	sp_faf_eth_unicast = 11,
	/* Ethernet Multicast */
	sp_faf_eth_multicast = 12,
	/* Ethernet Broadcast */
	sp_faf_eth_broadcast = 13,
	/* BPDU frame (MAC DA is 01:80:C2:00:00:00) */
	sp_faf_bpdu_frame = 14,
	/* FCoE detected (EType is 0x8906 */
	sp_faf_fcoe_detected = 15,
	/* FIP detected (EType is 0x8914) */
	sp_faf_fip_detected = 16,
	/* Ethernet Parsing Error */
	sp_faf_eth_parsing_err = 17,
	/* LLC+SNAP Present */
	sp_faf_llc_snap_present = 18,
	/* Unknown LLC/OUI */
	sp_faf_unknown_llc_oui = 19,
	/* LLC+SNAP Error */
	sp_faf_llc_snap_err = 20,
	/* VLAN 1 Present */
	sp_faf_vlan_1_present = 21,
	/* VLAN 1 Present */
	sp_faf_vlan_n_present = 22,
	/* CFI bit in a "8100" VLAN tag is set */
	sp_faf_vlan_8100_cfi_set = 23,
	/* VLAN Parsing Error */
	sp_faf_vlan_parsing_err = 24,
	/* PPPoE+PPP Present */
	sp_faf_ppoe_ppp_present = 25,
	/* PPPoE+PPP Parsing Error */
	sp_faf_ppoe_ppp_parsing_err = 26,
	/* MPLS 1 Present */
	sp_faf_mpls_1_present = 27,
	/* MPLS n Present */
	sp_faf_mpls_n_present = 28,
	/* MPLS Parsing Error */
	sp_faf_mpls_parsing_err = 29,
	/* ARP frame Present (Ethertype 0x0806 */
	sp_faf_arp_present = 30,
	/* ARP Parsing Error */
	sp_faf_arp_parsing_err = 31,
	/* L2 Unknown Protocol */
	sp_faf_l2_unknown_proto = 32,
	/* L2 Soft Parsing Error */
	sp_faf_l2_soft_parse_err = 33,
	/* IPv4 1 Present */
	sp_faf_ipv4_1_present = 34,
	/* IPv4 1 Unicast */
	sp_faf_ipv4_1_unicast = 35,
	/* IPv4 1 Multicast */
	sp_faf_ipv4_1_multicast = 36,
	/* IPv4 1 Broadcast */
	sp_faf_ipv4_1_broadcast = 37,
	/* IPv4 n Present */
	sp_faf_ipv4_n_present = 38,
	/* IPv4 n Unicast */
	sp_faf_ipv4_n_unicast = 39,
	/* IPv4 n Multicast */
	sp_faf_ipv4_n_multicast = 40,
	/* IPv4 n Broadcast */
	sp_faf_ipv4_n_broadcast = 41,
	/* IPv6 1 Present */
	sp_faf_ipv6_1_present = 42,
	/* IPv6 1 Unicast */
	sp_faf_ipv6_1_unicast = 43,
	/* IPv6 1 Multicast */
	sp_faf_ipv6_1_multicast = 44,
	/* IPv6 n Present */
	sp_faf_ipv6_n_present = 45,
	/* IPv6 n Unicast */
	sp_faf_ipv6_n_unicast = 46,
	/* IPv6 n Multicast */
	sp_faf_ipv6_n_multicast = 47,
	/* IP 1 option present */
	sp_faf_ip_option_1_present = 48,
	/* IP 1 Unknown Protocol */
	sp_faf_ip_1_unknown_proto = 49,
	/* IP 1 Packet is a fragment */
	sp_faf_ip_1_is_fragment = 50,
	/* IP 1 Packet is an initial fragment */
	sp_faf_ip_1_is_first_fragment = 51,
	/* IP 1 Parsing Error */
	sp_faf_ip_1_parsing_err = 52,
	/* IP n option present */
	sp_faf_ip_option_n_present = 53,
	/* IP n Unknown Protocol */
	sp_faf_ip_n_unknown_proto = 54,
	/* IP n Packet is a fragment */
	sp_faf_ip_n_is_fragment = 55,
	/* IP n Packet is an initial fragment */
	sp_faf_ip_n_is_first_fragment = 56,
	/* ICMP detected (IP proto is 1 */
	sp_faf_icmp_detected = 57,
	/* IGMP detected (IP proto is 2) */
	sp_faf_igmp_detected = 58,
	/* ICMPv6 detected (IP proto is 3a) */
	sp_faf_icmp_v6_detected = 59,
	/* UDP Light detected (IP proto is 136) */
	sp_faf_udp_light_detected = 60,
	/* IP n Parsing Error */
	sp_faf_ip_n_parsing_err = 61,
	/* Min. Encap Present */
	sp_faf_min_encap_present = 62,
	/* Min. Encap S flag set */
	sp_faf_min_encap_s_flag_set = 63,
	/* Min. Encap Parsing Error */
	sp_faf_min_encap_parsing_err = 64,
	/* GRE Present */
	sp_faf_gre_present = 65,
	/* GRE R bit set */
	sp_faf_gre_r_bit_set = 66,
	/* GRE Parsing Error */
	sp_faf_gre_parsing_err = 67,
	/* L3 Unknown Protocol */
	sp_faf_l3_unknown_proto = 68,
	/* L3 Soft Parsing Error */
	sp_faf_l3_soft_parsing_err = 69,
	/* UDP Present */
	sp_faf_udp_present = 70,
	/* UDP Parsing Error */
	sp_faf_udp_parsing_err = 71,
	/* TCP Present */
	sp_faf_tcp_present = 72,
	/* TCP options present */
	sp_faf_tcp_options_present = 73,
	/* TCP Control bits 6-11 set */
	sp_faf_tcp_ctrl_bits_6_11_set = 74,
	/* TCP Control bits 3-5 set */
	sp_faf_tcp_ctrl_bits_3_5_set = 75,
	/* TCP Parsing Error */
	sp_faf_tcp_parsing_err = 76,
	/* IPSec Present */
	sp_faf_ipsec_present = 77,
	/* IPSec ESP found */
	sp_faf_ipsec_esp_found = 78,
	/* IPSec AH found */
	sp_faf_ipsec_ah_found = 79,
	/* IPSec Parsing Error */
	sp_faf_ipsec_parsing_err = 80,
	/* SCTP Present */
	sp_faf_sctp_present = 81,
	/* SCTP Parsing Error */
	sp_faf_sctp_parsing_err = 82,
	/* DCCP Present */
	sp_faf_dccp_present = 83,
	/* DCCP Parsing Error */
	sp_faf_dccp_parsing_err = 84,
	/* L4 Unknown Protocol */
	sp_faf_l4_unknown_proto = 85,
	/* L4 Soft Parsing Error */
	sp_faf_l4_soft_parsing_err = 86,
	/* GTP Present */
	sp_faf_gtp_present = 87,
	/* GTP Parsing Error */
	sp_faf_gtp_parsing_err = 88,
	/* ESP Present */
	sp_faf_esp_present = 89,
	/* ESP Parsing Error */
	sp_faf_esp_parsing_err = 90,
	/* iSCSI detected (Port# 860) */
	sp_faf_iscsi_detected = 91,
	/* Capwap-control detected (Port# 5246) */
	sp_faf_capwap_ctrl_detected = 92,
	/* Capwap-data detected (Port# 5247) */
	sp_faf_capwap_data_detected = 93,
	/* L5 Soft Parsing Error */
	sp_faf_l5_soft_parsing_err = 94,
	/* IPv6 Route hdr1 present */
	sp_faf_ipv6_route_hdr1_present = 95,
	/* User defined soft parser bit #0 */
	sp_faf_ud_soft_parser_0 = 96,
	/* User defined soft parser bit #1 */
	sp_faf_ud_soft_parser_1 = 97,
	/* User defined soft parser bit #2 */
	sp_faf_ud_soft_parser_2 = 98,
	/* User defined soft parser bit #3 */
	sp_faf_ud_soft_parser_3 = 99,
	/* User defined soft parser bit #4 */
	sp_faf_ud_soft_parser_4 = 100,
	/* User defined soft parser bit #5 */
	sp_faf_ud_soft_parser_5 = 101,
	/* User defined soft parser bit #6 */
	sp_faf_ud_soft_parser_6 = 102,
	/* User defined soft parser bit #7 */
	sp_faf_ud_soft_parser_7 = 103,
};

/** @} end of group sparser_faf_bit */

/**************************************************************************//**
@Description	 enum sparser_ra_offset - Parse Array fields offset

*//***************************************************************************/
enum sparser_ra_offset {
	sp_ra_gprv_0 = 0,				/* 2 */
	sp_ra_gprv_1 = 2,				/* 2 */
	sp_ra_gprv_2 = 4,				/* 2 */
	sp_ra_gprv_3 = 6,				/* 2 */
	sp_ra_gprv_4 = 8,				/* 2 */
	sp_ra_gprv_5 = 10,				/* 2 */
	sp_ra_gprv_6 = 12,				/* 2 */
	sp_ra_gprv_7 = 14,				/* 2 */
	sp_ra_nxt_hdr = 16,				/* 2 */
	/* FAF Extension [18:19] */			/* 2 */
	/* FAF [20:31] */				/* 12 */
	sp_ra_pr_shim_offset_1 = 32,
	sp_ra_pr_shim_offset_2 = 33,
#ifndef LS2085A_REV1
	sp_ra_pr_ip_1_pid_offset = 34,
#else
	sp_ra_pr_ip_pid_offset = 34,
#endif
	sp_ra_pr_eth_offset = 35,
	sp_ra_pr_llc_snap_offset = 36,
	sp_ra_pr_vlan_tci_offset_1 = 37,
	sp_ra_pr_vlan_tci_offset_n = 38,
	sp_ra_pr_last_etype_offset = 39,
	sp_ra_pr_last_pppoe_offset = 40,
	sp_ra_pr_mpls_offset_1 = 41,
	sp_ra_pr_mpls_offset_n = 42,
#ifndef LS2085A_REV1
	sp_ra_pr_l3_offset = 43,
#else
	sp_ra_pr_ip1_or_arp_offset = 43,
#endif
	sp_ra_pr_ip_or_minencap_offset = 44,
	sp_ra_pr_gre_offset = 45,
	sp_ra_pr_l4_offset = 46,
#ifndef LS2085A_REV1
	sp_ra_pr_l5_offset = 47,
#else
	sp_ra_pr_gtp_esp_ipsec_offset = 47,
#endif
	sp_ra_pr_routing_hdr_offset_1 = 48,
	sp_ra_pr_routing_hdr_offset_2 = 49,
	sp_ra_pr_nxt_hdr_offset = 50,
	sp_ra_pr_ip_v6_frag_offset = 51,
	sp_ra_pr_gross_running_sum_offset = 52,		/* 2 */
	sp_ra_pr_running_sum_offset = 54,		/* 2 */
	sp_ra_pr_parse_error_code_offset = 56,
#ifndef LS2085A_REV1
	sp_ra_pr_nxt_hdr_frag_offset = 57,
	sp_ra_pr_ip_n_pid_offset = 58,
	sp_ra_pr_soft_parser_context_offset = 59,	/* 21 */
#else
	sp_ra_pr_soft_parser_context_offset = 57,	/* 23 */
#endif
	sp_ra_ipv4_sa_da_or_ipv6_sa = 80,		/* 16 */
	sp_ra_ipv6_da = 96,				/* 16 */
	sp_ra_sprec_misc_flags = 112,			/* 2 */
	sp_ra_ip_length = 114,				/* 2 */
	sp_ra_routing_type = 116,
	/* Reserved [117:122] */			/* 6 */
	sp_ra_fd_length = 123,				/* 3 */
	/* Reserved [126] */
	sp_ra_status_error = 127
};

/** @} end of group sparser_ra_offset */

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

/**************************************************************************//**
@Function	sparser_set_label

@Description	Set a label, that may be used as a jump destination, in the
		soft parser byte-code. A label may be used once.

@Param[in]	label : A label value from the sparser_local_label enumeration.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_set_label(enum sparser_local_label label);

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
@Function	sparser_gen_add32_wr1_wr0_to_wr0

@Description	Adds the least significant 32 bits of WR1 to the least
		significant 32 bits of WR0 and stores the result into WR0.
		No carry bit is captured. The most significant 32 bits of WR0
		are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr1_wr0_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr1_wr0_to_wr1

@Description	Adds the least significant 32 bits of WR1 to the least
		significant 32 bits of WR0 and stores the result into WR1.
		No carry bit is captured. The most significant 32 bits of WR0
		and WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr1_wr0_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr0_wr1_to_wr1

@Description	Adds the least significant 32 bits of WR0 to the least
		significant 32 bits of WR1 and stores the result into WR1.
		No carry bit is captured. The most significant 32 bits of WR1
		are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr0_wr1_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr0_wr1_to_wr0

@Description	Adds the least significant 32 bits of WR0 to the least
		significant 32 bits of WR1 and stores the result into WR0.
		No carry bit is captured. The most significant 32 bits of WR0
		and WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_add32_wr0_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr1_from_wr0_to_wr0

@Description	Subtracts the least significant 32 bits of WR1 from the least
		significant 32 bits of WR0 and stores the result into WR0.
		No underflow bit is captured. The most significant 32 bits of
		WR0 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr1_from_wr0_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr1_from_wr0_to_wr1

@Description	Subtracts the least significant 32 bits of WR1 from the least
		significant 32 bits of WR0 and stores the result into WR1.
		No underflow bit is captured. The most significant 32 bits of
		WR0 and WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr1_from_wr0_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr0_from_wr1_to_wr1

@Description	Subtracts the least significant 32 bits of WR0 from the least
		significant 32 bits of WR1 and stores the result into WR1.
		No underflow bit is captured. The most significant 32 bits of
		WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr0_from_wr1_to_wr1(void);

/**************************************************************************//**
@Function	sparser_gen_sub32_wr0_from_wr1_to_wr0

@Description	Subtracts the least significant 32 bits of WR0 from the least
		significant 32 bits of WR1 and stores the result into WR0.
		No underflow bit is captured. The most significant 32 bits of
		WR0 and WR1 are not affected by this instruction.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_sub32_wr0_from_wr1_to_wr0(void);

/**************************************************************************//**
@Function	sparser_gen_add32_wr0_imm_to_wr0

@Description	Adds the immediate 16/32 bits value to the least significant
		32 bits of WR0 and stores the result into WR0. No carry bit is
		captured. The most significant 32 bits of WR0 are not affected
		by this instruction.

@Param[in]	imm : 16/32 bits immediate value.

@Param[in]	imm_sz : Immediate value size - sp_imm_16 or sp_imm_32.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16 or sp_imm_32.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16 or sp_imm_32.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16 or sp_imm_32.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16 or sp_imm_32.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16 or sp_imm_32.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16 or sp_imm_32.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16 or sp_imm_32.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_xor_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_clr_wr0_imm_to_wr0

@Description	No bitwise operation is performed. WR0 is cleared.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_clr_wr0_imm_to_wr1

@Description	No bitwise operation is performed. WR1 is cleared.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_clr_wr1_imm_to_wr1

@Description	No bitwise operation is performed. WR1 is cleared.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_gen_clr_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz);

/**************************************************************************//**
@Function	sparser_gen_clr_wr1_imm_to_wr0

@Description	No bitwise operation is performed. WR0 is cleared.

@Param[in]	imm : 16/32/48/64 bits immediate value.

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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

@Param[in]	imm_sz : Immediate value size - sp_imm_16, sp_imm_32, sp_imm_48
		or sp_imm_64.

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
/* 12:ADD32_WRx_WRx_TO_WRx
 * SUB32_WRx_FROM_WRx_TO_WRx */
#define ADD32_WR1_WR0_TO_WR0 sparser_gen_add32_wr1_wr0_to_wr0()

#define ADD32_WR1_WR0_TO_WR1 sparser_gen_add32_wr1_wr0_to_wr1()

#define ADD32_WR0_WR1_TO_WR1 sparser_gen_add32_wr0_wr1_to_wr1()

#define ADD32_WR0_WR1_TO_WR0 sparser_gen_add32_wr0_wr1_to_wr0()

#define SUB32_WR1_FROM_WR0_TO_WR0 sparser_gen_sub32_wr1_from_wr0_to_wr0()

#define SUB32_WR1_FROM_WR0_TO_WR1 sparser_gen_sub32_wr1_from_wr0_to_wr1()

#define SUB32_WR0_FROM_WR1_TO_WR1 sparser_gen_sub32_wr0_from_wr1_to_wr1()

#define SUB32_WR0_FROM_WR1_TO_WR0 sparser_gen_sub32_wr0_from_wr1_to_wr0()

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
