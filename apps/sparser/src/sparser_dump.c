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

#include "fsl_types.h"
#include "fsl_stdio.h"
#include "fsl_sparser_disa.h"
#include "fsl_sparser_dump.h"

#include "fsl_dbg.h"
/* If "fsl_dbg.h" is not included ASSERT_COND and pr_err must be redefined */
#ifndef ASSERT_COND
	#define ASSERT_COND(_cond)
#endif
#ifndef pr_err
	#define pr_err(...)
#endif

/******************************************************************************/
#ifdef HAVE_AIOP_CDMA_MUTEX
	/* In order to include "fsl_cdma.h", following paths must be added to
	 * the project :
	 *
	 * ${ProjDirPath}/../../../../aiopsl/src/include/drivers/ldpaa
	 * ${ProjDirPath}/../../../../aiopsl/src/include/drivers/ldpaa/workspace
	 * ${ProjDirPath}/../../../../aiopsl/src/include/drivers/accel/cdma
	 * ${ProjDirPath}/../../../../aiopsl/src/drivers/
	 * ${ProjDirPath}/../../../../aiopsl/src/drivers/accel/cdma
	 */
	#include "fsl_cdma.h"

	uint64_t sp_print_lock;

	#define LOCK(_a)	cdma_mutex_lock_take(_a, CDMA_MUTEX_WRITE_LOCK)
	#define UNLOCK(_a)	cdma_mutex_lock_release(_a)
#else
	#define LOCK(_a)
	#define UNLOCK(_a)
#endif		/* HAVE_AIOP_CDMA_MUTEX */

struct frame_attr {
	char		*fld_name;
	uint8_t		faf_offset;
	uint32_t	fld_mask;
};

struct frame_attr_ext {
	char		*fld_name;
	uint8_t		faf_ext_offset;
	uint16_t	fld_mask;
};

struct frame_attr frame_attr_arr[] = {
	/* Frame Attribute Flags 1 */
	/* 000 */ {"User Defined                            ", 0, 0xff000000},
	/* 008 */ {"Shim Shell Soft Parsing Error           ", 0, 0x00800000},
	/* 009 */ {"Parsing Error                           ", 0, 0x00400000},
	/* 010 */ {"Ethernet MAC Present                    ", 0, 0x00200000},
	/* 011 */ {"Ethernet Unicast                        ", 0, 0x00100000},
	/* 012 */ {"Ethernet Multicast                      ", 0, 0x00080000},
	/* 013 */ {"Ethernet Broadcast                      ", 0, 0x00040000},
	/* 014 */ {"BPDU frame (MAC DA is 01:80:C2:00:00:00)", 0, 0x00020000},
	/* 015 */ {"FCoE detected (Ether type is 0x8906)    ", 0, 0x00010000},
	/* 016 */ {"FIP detected (Ether type is 0x8914)     ", 0, 0x00008000},
	/* 017 */ {"Ethernet Parsing Error                  ", 0, 0x00004000},
	/* 018 */ {"LLC+SNAP Present                        ", 0, 0x00002000},
	/* 019 */ {"Unknown LLC/OUI                         ", 0, 0x00001000},
	/* 020 */ {"LLC+SNAP Error                          ", 0, 0x00000800},
	/* 021 */ {"VLAN 1 Present                          ", 0, 0x00000400},
	/* 022 */ {"VLAN n Present                          ", 0, 0x00000200},
	/* 023 */ {"CFI bit in a \"8100\" VLAN tag is set     ", 0, 0x00000100},
	/* 024 */ {"VLAN Parsing Error                      ", 0, 0x00000080},
	/* 025 */ {"PPPoE+PPP Present                       ", 0, 0x00000040},
	/* 026 */ {"PPPoE+PPP Parsing Error                 ", 0, 0x00000020},
	/* 027 */ {"MPLS 1 Present                          ", 0, 0x00000010},
	/* 028 */ {"MPLS n Present                          ", 0, 0x00000008},
	/* 029 */ {"MPLS Parsing Error                      ", 0, 0x00000004},
	/* 030 */ {"ARP frame Present (Ethertype 0x0806)    ", 0, 0x00000002},
	/* 031 */ {"ARP Parsing Error                       ", 0, 0x00000001},
	/* Frame Attribute Flags 2 */
	/* 032 */ {"L2 Unknown Protocol                     ", 1, 0x80000000},
	/* 033 */ {"L2 Soft Parsing Error                   ", 1, 0x40000000},
	/* 034 */ {"IPv4 1 Present                          ", 1, 0x20000000},
	/* 035 */ {"IPv4 1 Unicast                          ", 1, 0x10000000},
	/* 036 */ {"IPv4 1 Multicast                        ", 1, 0x08000000},
	/* 037 */ {"IPv4 1 Broadcast                        ", 1, 0x04000000},
	/* 038 */ {"IPv4 n Present                          ", 1, 0x02000000},
	/* 039 */ {"IPv4 n Unicast                          ", 1, 0x01000000},
	/* 040 */ {"IPv4 n Multicast                        ", 1, 0x00800000},
	/* 041 */ {"IPv4 n Broadcast                        ", 1, 0x00400000},
	/* 042 */ {"IPv6 1 Present                          ", 1, 0x00200000},
	/* 043 */ {"IPv6 1 Unicast                          ", 1, 0x00100000},
	/* 044 */ {"IPv6 1 Multicast                        ", 1, 0x00080000},
	/* 045 */ {"IPv6 n Present                          ", 1, 0x00040000},
	/* 046 */ {"IPv6 n Unicast                          ", 1, 0x00020000},
	/* 047 */ {"IPv6 n Multicast                        ", 1, 0x00010000},
	/* 048 */ {"IP 1 option present                     ", 1, 0x00008000},
	/* 049 */ {"IP 1 Unknown Protocol                   ", 1, 0x00004000},
	/* 050 */ {"IP 1 Packet is a fragment               ", 1, 0x00002000},
	/* 051 */ {"IP 1 Packet is an initial fragment      ", 1, 0x00001000},
	/* 052 */ {"IP 1 Parsing Error                      ", 1, 0x00000800},
	/* 053 */ {"IP n option present                     ", 1, 0x00000400},
	/* 054 */ {"IP n Unknown Protocol                   ", 1, 0x00000200},
	/* 055 */ {"IP n Packet is a fragment               ", 1, 0x00000100},
	/* 056 */ {"IP n Packet is an initial fragment      ", 1, 0x00000080},
	/* 057 */ {"ICMP detected (IP proto is 1)           ", 1, 0x00000040},
	/* 058 */ {"IGMP detected (IP proto is 2)           ", 1, 0x00000020},
	/* 059 */ {"ICMPv6 detected (IP proto is 3a)        ", 1, 0x00000010},
	/* 060 */ {"UDP Light detected (IP proto is 136)    ", 1, 0x00000008},
	/* 061 */ {"IP n Parsing Error                      ", 1, 0x00000004},
	/* 062 */ {"Min. Encap Present                      ", 1, 0x00000002},
	/* 063 */ {"Min. Encap S flag set                   ", 1, 0x00000001},
	/* Frame Attribute Flags 3 */
	/* 064 */ {"Min. Encap Parsing Error                ", 2, 0x80000000},
	/* 065 */ {"GRE Present                             ", 2, 0x40000000},
	/* 066 */ {"GRE R bit set                           ", 2, 0x20000000},
	/* 067 */ {"GRE Parsing Error                       ", 2, 0x10000000},
	/* 068 */ {"L3 Unknown Protocol                     ", 2, 0x08000000},
	/* 069 */ {"L3 Soft Parsing Error                   ", 2, 0x04000000},
	/* 070 */ {"UDP Present                             ", 2, 0x02000000},
	/* 071 */ {"UDP Parsing Error                       ", 2, 0x01000000},
	/* 072 */ {"TCP Present                             ", 2, 0x00800000},
	/* 073 */ {"TCP options present                     ", 2, 0x00400000},
	/* 074 */ {"TCP Control bits 6-11 set               ", 2, 0x00200000},
	/* 075 */ {"TCP Control bits 3-5 set                ", 2, 0x00100000},
	/* 076 */ {"TCP Parsing Error                       ", 2, 0x00080000},
	/* 077 */ {"IPSec Present                           ", 2, 0x00040000},
	/* 078 */ {"IPSec ESP found                         ", 2, 0x00020000},
	/* 079 */ {"IPSec AH found                          ", 2, 0x00010000},
	/* 080 */ {"IPSec Parsing Error                     ", 2, 0x00008000},
	/* 081 */ {"SCTP Present                            ", 2, 0x00004000},
	/* 082 */ {"SCTP Parsing Error                      ", 2, 0x00002000},
	/* 083 */ {"DCCP Present                            ", 2, 0x00001000},
	/* 084 */ {"DCCP Parsing Error                      ", 2, 0x00000800},
	/* 085 */ {"L4 Unknown Protocol                     ", 2, 0x00000400},
	/* 086 */ {"L4 Soft Parsing Error                   ", 2, 0x00000200},
	/* 087 */ {"GTP Present                             ", 2, 0x00000100},
	/* 088 */ {"GTP Parsing Error                       ", 2, 0x00000080},
	/* 089 */ {"ESP Present                             ", 2, 0x00000040},
	/* 090 */ {"ESP Parsing Error                       ", 2, 0x00000020},
	/* 091 */ {"iSCSI detected (Port# 860)              ", 2, 0x00000010},
	/* 092 */ {"Capwap-control detected (Port# 5246)    ", 2, 0x00000008},
	/* 093 */ {"Capwap-data detected (Port# 5247)       ", 2, 0x00000004},
	/* 094 */ {"L5 Soft Parsing Error                   ", 2, 0x00000002},
	/* 095 */ {"IPv6 Route hdr1 present                 ", 2, 0x00000001},
	/* 096 */ {NULL,				       0, 0x00000000}
};

struct frame_attr_ext frame_attr_ext_arr[] = {
	/* Frame Attribute Flags Extension */
	/* 096 */ {"IPv6 Route hdr2 present                 ", 0, 0x8000},
	/* 097 */ {"Reserved                                ", 0, 0x7fff},
	/* 112 */ {NULL,				       0, 0x0000}
};

struct parse_err {
	uint16_t	code;
	char		*err_name;
};

struct parse_err parse_errors[] = {
	{0, "No Error"},
	{1, "Exceeds Block Limit"},
	{2, "Frame Truncation"},
	{3, "Invalid HXS"},
	{16, "LLC+SNAP - 802.3 Truncation"},
	{32, "PPPoE - Truncation"},
	{33, "PPPoE - MTU violated"},
	{34, "PPPoE - Version Invalid"},
	{35, "PPPoE - Type Invalid"},
	{36, "PPPoE - Code Invalid"},
	{37, "PPPoE - Session ID Invalid"},
	{64, "IPv4 - IP Packet Truncation"},
	{65, "IPv4 - IP Checksum error"},
	{66, "IPv4 - IP Version error"},
	{67, "IPv4 - Minimum Fragment Size error"},
	{68, "IPv4 - Header Length error"},
	{72, "IPv6 - IP Truncation"},
	{73, "IPv6 - Extension Header Violation"},
	{74, "IPv6 - IP Version error"},
	{75, "IPv6 - Routing Header Error"},
	{80, "GRE - Version Error"},
	{88, "MinEnc - Checksum error"},
	{96, "Other L3 - User defined"},
	{97, "Other L3 - User defined"},
	{98, "Other L3 - User defined"},
	{99, "Other L3 - User defined"},
	{100, "Other L3 - User defined"},
	{101, "Other L3 - User defined"},
	{102, "Other L3 - User defined"},
	{103, "Other L3 - User defined"},
	{104, "TCP - Invalid Offset"},
	{105, "TCP - Truncation"},
	{106, "TCP - Checksum error"},
	{107, "TCP - Bad flags"},
	{112, "UDP - Length error"},
	{113, "UDP - UDP checksum was 0 for IPv6"},
	{114, "UDP - Checksum error"},
	{128, "SCTP - Port 0 detected"},
	{144, "Other L4 - User defined"},
	{145, "Other L4 - User defined"},
	{146, "Other L4 - User defined"},
	{147, "Other L4 - User defined"},
	{148, "Other L4 - User defined"},
	{149, "Other L4 - User defined"},
	{150, "Other L4 - User defined"},
	{151, "Other L4 - User defined"},
	{152, "GTP - Unsupported version (not 1 or 2)"},
	{153, "GTP - Invalid PT"},
	{154, "GTP - Invalid L bit"},
	{168, "Other L5+ - User defined"},
	{169, "Other L5+ - User defined"},
	{170, "Other L5+ - User defined"},
	{171, "Other L5+ - User defined"},
	{172, "Other L5+ - User defined"},
	{173, "Other L5+ - User defined"},
	{174, "Other L5+ - User defined"},
	{175, "Other L5+ - User defined"},
	{184, "Soft parsing - User defined"},
	{185, "Soft parsing - User defined"},
	{186, "Soft parsing - User defined"},
	{187, "Soft parsing - User defined"},
	{188, "Soft parsing - User defined"},
	{189, "Soft parsing - User defined"},
	{190, "Soft parsing - User defined"},
	{191, "Soft parsing - User defined"},
	{0xFFFF, NULL}
};

/******************************************************************************/
/* Macro definitions */
#define IS_ONE_BIT_FIELD(_mask)					\
	(!((_mask) & ((_mask) - 1)) || (_mask == 1))

/******************************************************************************/
void sparser_parse_error_print(struct sp_parse_result *pr)
{
	struct parse_err	*err;

	ASSERT_COND(pr);
	LOCK(sp_print_lock);
	err = &parse_errors[0];
	while (err->err_name && err->code != pr->parse_error_code)
		err++;
	if (!err->err_name)
		fsl_print("%d : Unknown Parse Error Code\n\n",
			  pr->parse_error_code);
	else
		fsl_print("Parse Error Code %d : %s\n\n",
			  err->code, err->err_name);
	UNLOCK(sp_print_lock);
}

/******************************************************************************/
void sparser_frame_attributes_dump(struct sp_parse_result *pr)
{
	uint32_t		*pdw;
	uint16_t		*pw;
	struct frame_attr	*frm_attr;
	struct frame_attr_ext	*frm_attr_ext;

	#define PRINT_ONLY_IF_PRESENT	0

#if (PRINT_ONLY_IF_PRESENT == 1)
	#define fa_print_sb()						  \
	do {								  \
		if (*pdw & frm_attr->fld_mask)				  \
			fsl_print("\t %s : Yes\n", frm_attr->fld_name);	  \
	} while (0)

	#define fa_print_mb_0x8()					  \
	do {								  \
		if (*pdw & frm_attr->fld_mask)				  \
			fsl_print("\t %s : 0x%08x\n", frm_attr->fld_name, \
				  *pdw & frm_attr->fld_mask);		  \
	} while (0)

	#define fa_print_mb_0x4()					  \
	do {								  \
		if (*pdw & frm_attr->fld_mask)				  \
			fsl_print("\t %s : 0x%04x\n", frm_attr->fld_name, \
				  *pdw & frm_attr->fld_mask);		  \
	} while (0)
#else
	#define fa_print_sb()						  \
	do {								  \
		fsl_print("\t %s : ", frm_attr->fld_name);		  \
		if (*pdw & frm_attr->fld_mask)				  \
			fsl_print("Yes\n");				  \
		else							  \
			fsl_print("No\n");				  \
	} while (0)

	#define fa_print_mb_0x8()					  \
		fsl_print("\t %s : 0x%08x\n", frm_attr->fld_name,	  \
			  *pdw & frm_attr->fld_mask)

	#define fa_print_mb_0x4()					  \
		fsl_print("\t %s : 0x%04x\n", frm_attr->fld_name,	  \
			  *pdw & frm_attr->fld_mask)
#endif
	ASSERT_COND(pr);
	LOCK(sp_print_lock);
	/* Frame Attribute Flags 1, 2, 3 */
	fsl_print("Dump of Frame Attribute Flags\n");
	frm_attr = &frame_attr_arr[0];
	while (frm_attr->fld_name) {
		pdw = (uint32_t *)&pr->frame_attribute_flags_1;
		pdw += frm_attr->faf_offset;
		if (IS_ONE_BIT_FIELD(frm_attr->fld_mask))
			fa_print_sb();
		else
			fa_print_mb_0x8();
		frm_attr++;
	}
	fsl_print("\n");

	/* Frame Attribute Flags Extension */
#if (PRINT_ONLY_IF_PRESENT == 1)
	if (!pr->frame_attribute_flags_extension) {
		UNLOCK(sp_print_lock);
		return;	/* Noting to show */
	}
#endif
	fsl_print("Dump of Frame Attribute Flags Extension\n");
	frm_attr_ext = &frame_attr_ext_arr[0];
	while (frm_attr_ext->fld_name) {
		pw = (uint16_t *)&pr->frame_attribute_flags_extension;
		pw += frm_attr_ext->faf_ext_offset;
		if (IS_ONE_BIT_FIELD(frm_attr_ext->fld_mask)) {
			fsl_print("\t %s : ", frm_attr_ext->fld_name);
			if (*pdw & frm_attr_ext->fld_mask)
				fsl_print("Yes\n");
			else
				fsl_print("No\n");
		} else {
			fsl_print("\t %s : 0x%04x\n", frm_attr_ext->fld_name,
				  *pdw & frm_attr_ext->fld_mask);
		}
		frm_attr_ext++;
	}
	fsl_print("\n");

	UNLOCK(sp_print_lock);
}

/******************************************************************************/
void sparser_parse_result_dump(struct sp_parse_result *pr)
{
	uint8_t		*pb;
	int		i;

	#define PRINT_ONLY_IF_PRESENT	0

#if (PRINT_ONLY_IF_PRESENT == 1)
	#define pr_print_fld(_a, _b)					  \
	do {								  \
		if (_b != 0xFF)						  \
			fsl_print(_a, _b, _b);				  \
	} while (0)
#else
	#define pr_print_fld(_a, _b)					  \
			fsl_print(_a, _b, _b)
#endif
	ASSERT_COND(pr);
	LOCK(sp_print_lock);
	fsl_print("Dump of Parse Result\n");
	/* Next header */
	fsl_print("nxt_hdr                         = 0x%04x\n",
		  pr->nxt_hdr);
	/* Frame Attribute Flags Extension */
	fsl_print("frame_attribute_flags_extension = 0x%04x\n",
		  pr->frame_attribute_flags_extension);
	/* Frame Attribute Flags (part 1) */
	fsl_print("frame_attribute_flags_1         = 0x%08x\n",
		  pr->frame_attribute_flags_1);
	/* Frame Attribute Flags (part 2) */
	fsl_print("frame_attribute_flags_2         = 0x%08x\n",
		  pr->frame_attribute_flags_2);
	/* Frame Attribute Flags (part 3) */
	fsl_print("frame_attribute_flags_3         = 0x%08x\n",
		  pr->frame_attribute_flags_3);
	/* Shim Offset 1 */
	pr_print_fld("shim_offset_1                   = %d (0x%02x)\n",
		     pr->shim_offset_1);
	/* Shim Offset 2 */
	pr_print_fld("shim_offset_2                   = %d (0x%02x)\n",
		     pr->shim_offset_2);
#ifndef LS2085A_REV1
	/* IP protocol field offset */
	pr_print_fld("ip_1_pid_offset                 = %d (0x%02x)\n",
		     pr->ip_1_pid_offset);
#else
	pr_print_fld("ip_pid_offset                   = %d (0x%02x)\n",
		     pr->ip_pid_offset);
#endif
	/* Ethernet offset */
	pr_print_fld("eth_offset                      = %d (0x%02x)\n",
		     pr->eth_offset);
	/* LLC+SNAP offset */
	pr_print_fld("llc_snap_offset                 = %d (0x%02x)\n",
		     pr->llc_snap_offset);
	/* First VLAN's TCI field offset*/
	pr_print_fld("vlan_tci1_offset                = %d (0x%02x)\n",
		     pr->vlan_tci1_offset);
	/* Last VLAN's TCI field offset*/
	pr_print_fld("vlan_tcin_offset                = %d (0x%02x)\n",
		     pr->vlan_tcin_offset);
	/* Last Ethertype offset*/
	pr_print_fld("last_etype_offset               = %d (0x%02x)\n",
		     pr->last_etype_offset);
	/* PPPoE offset */
	pr_print_fld("pppoe_offset                    = %d (0x%02x)\n",
		     pr->pppoe_offset);
	/* First MPLS offset */
	pr_print_fld("mpls_offset_1                   = %d (0x%02x)\n",
		     pr->mpls_offset_1);
	/* Last MPLS offset */
	pr_print_fld("mpls_offset_n                   = %d (0x%02x)\n",
		     pr->mpls_offset_n);
#ifndef LS2085A_REV1
	/* Outer IP or ARP offset */
	pr_print_fld("l3_offset (IP/ARP)              = %d (0x%02x)\n",
		     pr->l3_offset);
#else
	/* Outer IP or ARP offset */
	pr_print_fld("ip1_or_arp_offset (IP/ARP)      = %d (0x%02x)\n",
		     pr->ip1_or_arp_offset);
#endif
	/* Inner IP or MinEncap offset*/
	pr_print_fld("ipn_or_minencap_offset          = %d (0x%02x)\n",
		     pr->ipn_or_minencap_offset);
	/* GRE offset */
	pr_print_fld("gre_offset                      = %d (0x%02x)\n",
		     pr->gre_offset);
	/* Layer 4 offset*/
	pr_print_fld("l4_offset                       = %d (0x%02x)\n",
		     pr->l4_offset);
#ifndef LS2085A_REV1
	/* GTP/ESP/IPsec offset */
	pr_print_fld("l5_offset (GTP/ESP/IPsec)       = %d (0x%02x)\n",
		     pr->l5_offset);
#else
	/* GTP/ESP/IPsec offset */
	pr_print_fld("gtp_esp_ipsec_offset            = %d (0x%02x)\n",
		     pr->gtp_esp_ipsec_offset);
#endif
	/* Routing header offset of 1st IPv6 header */
	pr_print_fld("routing_hdr_offset1             = %d (0x%02x)\n",
		     pr->routing_hdr_offset1);
	/* Routing header offset of 2nd IPv6 header */
	pr_print_fld("routing_hdr_offset2             = %d (0x%02x)\n",
		     pr->routing_hdr_offset2);
	/* Next header offset */
	pr_print_fld("nxt_hdr_offset                  = %d (0x%02x)\n",
		     pr->nxt_hdr_offset);
	/* IPv6 fragmentable part offset */
	pr_print_fld("ipv6_frag_offset                = %d (0x%02x)\n",
		     pr->ipv6_frag_offset);
	/* Frame's untouched running sum, input to parser */
	fsl_print("gross_running_sum               = 0x%04x\n",
		  pr->gross_running_sum);
	/* Running Sum */
	fsl_print("running_sum                     = 0x%04x\n",
		  pr->running_sum);
	/* Parse Error code.
	 * Please refer to \ref FSL_PARSER_ERROR_CODES*/
	fsl_print("parse_error_code                = 0x%02x\n",
		  pr->parse_error_code);
#ifndef LS2085A_REV1
	/* Offset to the next header field before IPv6 fragment
	 * extension */
	pr_print_fld("nxt_hdr_before_ipv6_frag_ext    = %d (0x%02x)\n",
		     pr->nxt_hdr_before_ipv6_frag_ext);
	/** Inner IP Protocol field offset */
	pr_print_fld("ip_n_pid_offset (Inner)         = %d (0x%02x)\n",
		     pr->ip_n_pid_offset);
	/* Reserved for Soft parsing context*/
	fsl_print("soft_parsing_context            =\n");
	pb = (uint8_t *)&pr->soft_parsing_context[0];
	for (i = 0; i < 21; i++)
		fsl_print("%02x ", *pb++);
	fsl_print("\n");
#else
	/* Reserved for Soft parsing context*/
	fsl_print("soft_parsing_context            =\n");
	pb = (uint8_t *)&pr->soft_parsing_context[0];
	for (i = 0; i < 23; i++)
		fsl_print("%02x ", *pb++);
	fsl_print("\n");
#endif
	fsl_print("\n");
	UNLOCK(sp_print_lock);
}
