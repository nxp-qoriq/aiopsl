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
@File		fsl_sparser_disa.h

@Description	Soft Parser Disassembler/Built-In Simulator API
*//***************************************************************************/
#ifndef __FSL_SPARSER_DISA_H
#define __FSL_SPARSER_DISA_H

#include "sparser_build_flags.h"

/**************************************************************************//**
@Group		sparser_sim_g SPARSER SIM

@Description	Contains APIs for :
		- Soft Parser Disassembler
		- Soft Parser Built-In Simulator
		- Soft Parser Built-In Simulator dumping functions :
			- Parse Error code
			- Frame Attribute Flags
			- Parse Result
@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		SPARSER_SIM_MAX_PKT_LEN Simulated Parsed Packet Length

 @Description	Define the maximum length of the packet to be parsed by a
		Soft Parser executed on the built-in simulator.

 @{
*//***************************************************************************/
/* Maximum length of the packet used by the built-in simulator */
#define SP_SIM_MAX_FRM_LEN		256
/** @} end of group SPARSER_SIM_MAX_PKT_LEN */

/**************************************************************************//**
 @Group		SPARSER_SIM_MAX_PCLIM Simulated Parsing Cycle Limit

 @Description	Define the maximum value of the of the Parsing Cycle Limit
		simulated register.

 @{
*//***************************************************************************/
/* Maximum parsing cycles limit */
#define SP_SIM_MAX_CYCLE_LIMIT		8191
/** @} end of group SPARSER_SIM_MAX_PCLIM */

/**************************************************************************//**
@Description	Parse Result structure
		Please refer to the parser specification for more details.

*//***************************************************************************/
#pragma pack(push, 1)

struct sp_parse_result {
	/* Next header */
	uint16_t	nxt_hdr;
	/* Frame Attribute Flags Extension */
	uint16_t	frame_attribute_flags_extension;
	/* Frame Attribute Flags (part 1) */
	uint32_t	frame_attribute_flags_1;
	/* Frame Attribute Flags (part 2) */
	uint32_t	frame_attribute_flags_2;
	/* Frame Attribute Flags (part 3) */
	uint32_t	frame_attribute_flags_3;
	/* Shim Offset 1 */
	uint8_t		shim_offset_1;
	/* Shim Offset 2 */
	uint8_t		shim_offset_2;
	/* Outer IP protocol field offset */
#ifndef LS2085A_REV1
	uint8_t		ip_1_pid_offset;
#else
	uint8_t		ip_pid_offset;
#endif
	/* Ethernet offset */
	uint8_t		eth_offset;
	/* LLC+SNAP offset */
	uint8_t		llc_snap_offset;
	/* First VLAN's TCI field offset*/
	uint8_t		vlan_tci1_offset;
	/* Last VLAN's TCI field offset*/
	uint8_t		vlan_tcin_offset;
	/* Last Ethertype offset*/
	uint8_t		last_etype_offset;
	/* PPPoE offset */
	uint8_t		pppoe_offset;
	/* First MPLS offset */
	uint8_t		mpls_offset_1;
	/* Last MPLS offset */
	uint8_t		mpls_offset_n;
#ifndef LS2085A_REV1
	/* Layer 3 (Outer IP, ARP, FCoE or FIP) offset */
	uint8_t		l3_offset;
#else
	/* Outer IP or ARP offset */
	uint8_t		ip1_or_arp_offset;
#endif
	/* Inner IP or MinEncap offset*/
	uint8_t		ipn_or_minencap_offset;
	/* GRE offset */
	uint8_t		gre_offset;
	/* Layer 4 offset*/
	uint8_t		l4_offset;
#ifndef LS2085A_REV1
	/* Layer 5 offset */
	uint8_t		l5_offset;
#else
	/* GTP/ESP/IPsec offset */
	uint8_t		gtp_esp_ipsec_offset;
#endif
	/* Routing header offset of 1st IPv6 header */
	uint8_t		routing_hdr_offset1;
	/* Routing header offset of 2nd IPv6 header */
	uint8_t		routing_hdr_offset2;
	/* Next header offset */
	uint8_t		nxt_hdr_offset;
	/* IPv6 fragmentable part offset */
	uint8_t		ipv6_frag_offset;
	/* Frame's untouched running sum, input to parser */
	uint16_t	gross_running_sum;
	/* Running Sum */
	uint16_t	running_sum;
	/* Parse Error code */
	uint8_t		parse_error_code;
#ifndef LS2085A_REV1
	/* Offset to the next header field before IPv6 fragment extension */
	uint8_t		nxt_hdr_before_ipv6_frag_ext;
	/* Inner IP Protocol field offset */
	uint8_t		ip_n_pid_offset;
	/* Reserved for Soft parsing context*/
	uint8_t		soft_parsing_context[21];
#else
	/* Reserved for Soft parsing context*/
	uint8_t		soft_parsing_context[23];
#endif
};

#pragma pack(pop)

/**************************************************************************//**
@Description	Parse Array structure
		Please refer to the parser specification for more details.

*//***************************************************************************/
#pragma pack(push, 1)

struct sp_parse_array {
	/* General Purpose Registered Values 0-15 */
	uint8_t			gprv[16];
	/* Parser Result */
	struct sp_parse_result	pr;
	/* IPv4 SA and IPv4 DA extracted for pseudo header.
	 * IPv6 SA extracted for pseudo header */
	uint8_t			ipv4_sa_da_ipv6_sa[16];
	/* IPv6 DA extracted for pseudo header */
	uint8_t			ipv6_da[16];
	/* Soft Parser Error Report Control and Flags */
	uint8_t			sprec_misc_flags[2];
	/* IP length */
	uint16_t		ip_length;
	/* Routing Type */
	uint8_t			routing_type;
	/* Reserved */
	uint8_t			res1[6];
	/* FD Length */
	uint8_t			fd_length[3];
	/* Reserved */
	uint8_t			res2;
	/* Status/Error Vector */
	uint8_t			status_error;
};

#pragma pack(pop)

/**************************************************************************//**
@Function	sparser_disa

@Description	Disassemble a Soft Parser byte-code.

@Param[in]	pc : Starting Program Counter of the Soft Parser.

@Param[in]	byte_code : Soft Parser byte-code array. Must be aligned on a
		four bytes boundary.

@Param[in]	sp_size : Soft Parser byte-code array size, in bytes. Must be
		a multiple of four.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_disa(uint16_t pc, uint8_t *byte_code, int sp_size);

/**************************************************************************//**
@Function	sparser_disa_instr

@Description	Disassemble a Soft Parser instruction.

@Param[in]	pc : Instruction PC

@Param[in]	bytes : Instruction bytes.

@Param[in]	len : Instruction length in bytes.

@Return		No return on failure. Prints error messages, showing what error
		occurred.

*//***************************************************************************/
void sparser_disa_instr(uint16_t pc, uint8_t *bytes, uint16_t len);

/**************************************************************************//**
@Function	sparser_sim_init

@Description	Initialize the built-in simulator (mandatory call).

@Return		None

*//***************************************************************************/
void sparser_sim_init(void);

/**************************************************************************//**
@Function	sparser_sim_set_parse_array

@Description	Initialize the application provided Parse Array structure.
		Call this function if the under development Soft Parser
		reads/writes bytes from/into the Parse Array.

@Param[in]	sp_parse_array : Parse Array structure.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_sim_init_parse_array(struct sp_parse_array *ra);

/**************************************************************************//**
@Function	sparser_sim_set_parse_array

@Description	Set the application provided Parse Array structure onto the
		built-in simulator. Call this function if the under development
		Soft Parser reads/writes bytes from/into the Parse Array.

@Param[in]	sp_parse_array : Parse Array structure.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_sim_set_parse_array(struct sp_parse_array *ra);

/**************************************************************************//**
@Function	sparser_sim_set_parameter_array

@Description	Set the application provided Parameter Array onto the built-in
		simulator. Call this function if the under development Soft
		Parser reads bytes from the Parameters Array.

@Param[in]	pa : Parameter Array bytes.

@Param[in]	offset : Offset of the Soft Parser parameters into the shared
		Parameters Array (64 bytes array).

@Param[in]	size : Soft Parser parameters size in bytes.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_sim_set_parameter_array(uint8_t *pa, uint8_t offset, uint8_t size);

/**************************************************************************//**
@Function	sparser_sim_set_parsed_pkt

@Description	Set the application provided packet to be parsed onto the
		built-in simulator. Call this function if the under development
		Soft Parser reads bytes from the Packet to be parsed.

@Param[in]	prs_pkt : Packet to be parsed

@Param[in]	pkt_size : Packet size in bytes (at most 256 bytes).

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_sim_set_parsed_pkt(uint8_t *prs_pkt, uint16_t pkt_size);

/**************************************************************************//**
@Function	sparser_sim_set_header_base

@Description	Set the application provided Header Base register value onto the
		built-in simulator. Call this function if the under development
		Soft Parser reads bytes from the Packet to be parsed.

@Param[in]	hb : Header Base register value

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_sim_set_header_base(uint16_t hb);

/**************************************************************************//**
@Function	sparser_sim_set_pc_limit

@Description	Set the application provided Parsing Cycles Limit register value
		onto the built-in simulator. By default built-in simulator
		PCLIM register value is reset (PCLIM verification is disabled).

@Param[in]	pc_limit : Parsing Cycles Limit register value

@Param[in]	pkt_size : Packet size in bytes (at most 256 bytes).

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_sim_set_pc_limit(uint16_t pc_limit);

/**************************************************************************//**
@Function	sparser_sim

@Description	Simulate the execution of a Soft Parser using the built-in
		simulator.

@Param[in]	pc : Starting Program Counter of the Soft Parser.

@Param[in]	byte_code : Soft Parser byte-code array. Must be aligned on a
		four bytes boundary.

@Param[in]	sp_size : Soft Parser byte-code array size, in bytes. Must be
		a multiple of four.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_sim(uint16_t pc, uint8_t *byte_code, int sp_size);

/**************************************************************************//**
@Function	sparser_sim_parse_error_print

@Description	Prints built-in simulator detailed parse error code information.

@Return		None.

*//***************************************************************************/
void sparser_sim_parse_error_print(void);

/**************************************************************************//**
@Function	sparser_sim_frame_attributes_dump

@Description	Prints built-in simulator detailed Frame Attribute Flags
		information.

@Return		None.

*//***************************************************************************/
void sparser_sim_frame_attributes_dump(void);

/**************************************************************************//**
@Function	sparser_sim_parse_result_dump

@Description	Prints built-in simulator detailed Parse Results information.

@Return		None.

*//***************************************************************************/
void sparser_sim_parse_result_dump(void);

/** @} */ /* end of sparser_sim_g SPARSER SIM group */
#endif		/* __FSL_SPARSER_DISA_H */
