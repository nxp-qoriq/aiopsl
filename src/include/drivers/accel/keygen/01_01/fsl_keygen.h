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
@File		fsl_keygen.h

@Description	This file contains the AIOP Software Key Generation API

*//***************************************************************************/

#ifndef __FSL_KEYGEN_H
#define __FSL_KEYGEN_H

#include "types.h"
#include "general.h"
#include "fsl_parser.h"

/* TODO remark on allocations of out params */
/**************************************************************************//**
 @Group		ACCEL Accelerators APIs

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group	FSL_KEYGEN KEYGEN

@Description	Freescale AIOP Table API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_KEYGEN_MACROS KEYGEN Macros

@Description	Freescale AIOP KEYGEN Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_KEYGEN_KCR_ATTRIBUTES Key Composition Rule Attributes
@{
*//***************************************************************************/
	/** Generic Extraction from start of frame */
#define KEYGEN_KCR_LENGTH		0x40

/** @} */ /* end of FSL_KEYGEN_KCR_ATTRIBUTES */

/** @} */ /* end of FSL_KEYGEN_MACROS */

/**************************************************************************//**
@Group		FSL_KEYGEN_Enumerations KEYGEN Enumerations

@Description	KEYGEN Enumerations

@{
*//***************************************************************************/

/**************************************************************************//**
@enum	kcr_builder_gec_source

@Description	 Key Composition Rule Builder Generic Extract Source

@{
*//***************************************************************************/
enum kcr_builder_gec_source {
	/** Generic Extraction from start of frame */
	KEYGEN_KCR_GEC_FRAME = 0x0,
	/** Generic Extraction from Parser Result */
	KEYGEN_KCR_GEC_PARSE_RES = 0x1
};

/** @} */ /* end of kcr_builder_gec_source */

/**************************************************************************//**
@enum	kcr_builder_parse_result_offset

@Description	 Key Composition Rule Builder Parse Result Offset

@{
*//***************************************************************************/
enum kcr_builder_parse_result_offset {
	/** Shim Offset 1 field's offset in parser result */
	SHIM_OFFSET_1_OFFSET_IN_PR =
			offsetof(struct parse_result, shim_offset_1),
	/** Shim Offset 2 field's offset in parser result */
	SHIM_OFFSET_2_OFFSET_IN_PR =
			offsetof(struct parse_result, shim_offset_2),
	/** IP NH/protocol offset field's offset in parser result */
	IP_PID_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, ip_pid_offset),
	/** Ethernet offset field's offset in parser result */
	ETH_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, eth_offset),
	/** llc_snap offset field's offset in parser result */
	LLC_SNAP_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, llc_snap_offset),
	/** First VLAN's TCI offset field's offset in parser result */
	TCI1_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, vlan_tci1_offset),
	/** Last VLAN's TCI offset field's offset in parser result */
	TCIN_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, vlan_tcin_offset),
	/** Last Ethertype offset field's offset in parser result */
	LAST_ETYPE_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, last_etype_offset),
	/** PPPoE offset field's offset in parser result */
	PPPOE_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, pppoe_offset),
	/** First MPLS offset field's offset in parser result */
	MPLS_OFFSET_1_OFFSET_IN_PR =
			offsetof(struct parse_result, mpls_offset_1),
	/** Last MPLS offset field's offset in parser result */
	MPLS_OFFSET_N_OFFSET_IN_PR =
			offsetof(struct parse_result, mpls_offset_n),
	/** First IP or ARP offset field's offset in parser result */
	IP1_OR_ARP_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, ip1_or_arp_offset),
	/** Last IP or MinEncap offset field's offset in parser result */
	IPN_OR_MINENCAP0_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, ipn_or_minencapO_offset),
	/** GRE offset field's offset in parser result */
	GRE_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, gre_offset),
	/** Layer 4 offset field's offset in parser result */
	L4_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, l4_offset),
	/** GTP/ESP/IPsec offset field's offset in parser result */
	GTP_ESP_IPSEC_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, gtp_esp_ipsec_offset),
	/** Routing header offset of 1st frame field's offset in parser result*/
	ROUTING_HDR_OFFSET_1_OFFSET_IN_PR =
			offsetof(struct parse_result, routing_hdr_offset1),
	/** Routing header offset of 2nd frame field's offset in parser result*/
	ROUTING_HDR_OFFSET_2_OFFSET_IN_PR =
			offsetof(struct parse_result, routing_hdr_offset2),
	/** Next header offset field's offset in parser result */
	NXT_HDR_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, nxt_hdr_offset),
	/** IPv6 fragmentable part offset field's offset in parser result */
	IPV6_FRAG_OFFSET_OFFSET_IN_PR =
			offsetof(struct parse_result, ipv6_frag_offset),
	/** Soft parsing context */
	SOFT_PARSING_CONTEXT =
			offsetof(struct parse_result, soft_parsing_context[0])
};

/** @} */ /* end of kcr_builder_parse_result_offset */

/**************************************************************************//**
@enum kcr_builder_protocol_fecid

@Description	Key Composition Rule Builder Protocol FECID

@{
*//***************************************************************************/
enum kcr_builder_protocol_fecid {
	/** FECID of Generic Extraction Command */
	KEYGEN_KCR_GEC_FECID = 0x00,
	/** FECID of User defined constant */
	KEYGEN_KCR_UDC_FECID = 0x01,
	/** FECID of Valid Field */
	KEYGEN_KCR_VF_FECID = 0x02,
	/** FECID of MAC destination address */
	KEYGEN_KCR_MACDST_FECID = 0x03,
	/** FECID of MAC source address */
	KEYGEN_KCR_MACSRC_FECID = 0x04,
	/** FECID of VLAN TCI from the first Q-Tag in the frame */
	KEYGEN_KCR_VLANTCI_1_FECID = 0x05,
	/** FECID of VLAN TCI from the last Q-Tag in the frame */
	KEYGEN_KCR_VLANTCI_N_FECID = 0x06,
	/** FECID of Ethernet Type field */
	KEYGEN_KCR_ETYPE_FECID = 0x07,
	/** FECID of PPPoE Session ID Field */
	KEYGEN_KCR_PPPSID_FECID = 0x08,
	/** FECID of PPP Protocol ID Field */
	KEYGEN_KCR_PPPPID_FECID = 0x09,
	/** FECID of MPLS first label with TC */
	KEYGEN_KCR_MPLSL_1_FECID = 0x0A,
	/** FECID of MPLS second label with TC
	( present in the frame if MPLSOffset_n-MPLSOffset_1>= 4)*/
	KEYGEN_KCR_MPLSL_2_FECID = 0x0B,
	/** FECID of MPLS last label with TC */
	KEYGEN_KCR_MPLSL_N_FECID = 0x0C,
	/** FECID of ARP Operation */
	KEYGEN_KCR_ARP_OP_FECID = 0x0D,
	/** FECID of ARP Sender protocol Address */
	KEYGEN_KCR_ARP_SPA_FECID = 0x0E,
	/** FECID of ARP Target protocol Address */
	KEYGEN_KCR_ARP_TPA_FECID = 0x0F,
	/** FECID of ARP Sender Hardware Address */
	KEYGEN_KCR_ARP_SHA_FECID = 0x10,
	/** FECID of ARP Target Hardware Address */
	KEYGEN_KCR_ARP_THA_FECID = 0x11,
	/** FECID of first (outer) IPv4/6 header Source Address */
	KEYGEN_KCR_IPSRC_1_FECID = 0x12,
	/** FECID of first (outer) IPv4/6 header Destination Address */
	KEYGEN_KCR_IPDST_1_FECID = 0x13,
	/** FECID of first (outer) IPv4 Protocol Type or IPv6 next header */
	KEYGEN_KCR_PTYPE_1_FECID = 0x14,
	/** FECID of first (outer) TOS (IPv4) or Traffic Class (IPv6) */
	KEYGEN_KCR_IPTOS_TC_1_FECID = 0x15,
	/** FECID ofIP Identification for IP Reassembly
	* (In IPv4 in IP header, in IPv6 in fragment extension) */
	KEYGEN_KCR_IPID_1_FECID = 0x16,
	/** FECID of first (outer) IPv6 Flow Label */
	KEYGEN_KCR_IPV6FL_1_FECID = 0x17,
	/** FECID of last IP source / Min. Encap Source Address
	FECID of last (inner) IPv4/6 Source Address or Min. Encap
	Source Address Field */
	KEYGEN_KCR_IPSRC_N_FECID = 0x18,
	/** FECID of last IP dest / Min. Encap dest Address
	FECID of last (inner) IPv4/6 Destination or Min. Encap Destination
	Address field */
	KEYGEN_KCR_IPDST_N_FECID = 0x19,
	/** FECID of last IPv4 Protocol Type / IPv6 NH / Encap dest Address
	FECID of IPv4 Protocol Type Field or IPv6 next header of the
	last (inner) IP header, or Min. Encap protocol type */
	KEYGEN_KCR_PTYPE_N_FECID = 0x1A,
	/** FECID of last (inner) TOS (IPv4) or Traffic Class(IPv6) */
	KEYGEN_KCR_IPTOS_TC_N_FECID = 0x1B,
	/** FECID of IP Identification for IP Reassembly
	(In IPv4 in IP header, in IPv6 in fragment extension) */
	KEYGEN_KCR_IPID_N_FECID = 0x1C,
	/** FECID of IPv6 Flow Label of the last (inner) IP header */
	KEYGEN_KCR_IPV6FL_N_FECID = 0x1D,
	/** FECID of GRE Protocol Type field */
	KEYGEN_KCR_GREPTYPE_FECID = 0x1E,
	/** FECID of TCP or UDP or SCTP or DCCP source Port Field */
	KEYGEN_KCR_L4PSRC_FECID = 0x1F,
	/** FECID of TCP or UDP or SCTP or DCCP destination Port Field */
	KEYGEN_KCR_L4PDST_FECID = 0x20,
	/** FECID of 14th byte of the TCP header, contains TCP flags */
	KEYGEN_KCR_TFLG_FECID = 0x21,
	/** FECID of IPSec SPI field */
	KEYGEN_KCR_IPSECSPI_FECID = 0x22,
	/** FECID of IPSec (AH only) Next Header field */
	KEYGEN_KCR_IPSECNH_FECID = 0x23,
	/** FECID of GPRS Tunnel endpoint Identification */
	KEYGEN_KCR_GTP_TEID_FECID = 0x24,
/*  Following 2 FECIDs are not supported in REV1 due to TKT241788
	 * FECID of ICMP type 
	KEYGEN_KCR_ICMP_TYPE_FECID = 0x25,
	* FECID of ICMP code 
	KEYGEN_KCR_ICMP_CODE_FECID = 0x26,
*/
	/** FECID of Next Header */
	KEYGEN_KCR_NXT_HDR_FECID = 0x27
	/* TODO check spec  for updates */
};
/** @} */ /* end of kcr_builder_protocol_fecid */

/**************************************************************************//**
@enum	keygen_hw_accel_id

@Description	IDs of hardware table lookup accelerator

@{
*//***************************************************************************/
enum keygen_hw_accel_id {
	/** MFLU accelerator ID */
	KEYGEN_ACCEL_ID_MFLU = MFLU_ACCEL_ID,
	/** CTLU accelerator ID */
	KEYGEN_ACCEL_ID_CTLU = CTLU_ACCEL_ID
};

/** @} */ /* end of keygen_hw_accel_id */

/** @} */ /* end of FSL_KEYGEN_Enumerations */

/**************************************************************************//**
@Group		FSL_KEYGEN_STRUCTS KEYGEN Structures

@Description	Freescale AIOP KEYGEN Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Key Composition Rule (kcr) builder
*//***************************************************************************/
struct	kcr_builder {
	/** KCR as defined by CTLU
	must be initialized by keygen_kcr_builder_init() before use */
	uint8_t  kcr[KEYGEN_KCR_LENGTH];

	/** KCR length
	Number of bytes the FECs occupy */
	uint8_t  kcr_length;
};


/**************************************************************************//**
@Description	Key Composition Rule (kcr) builder FEC single mask
*//***************************************************************************/
struct	kcr_builder_fec_single_mask {
	/** Bit-wise mask
	Applied to the extracted header at the corresponding offset from its
	beginning.
	0 - Corresponding bit is masked
	1 - Corresponding bit is not masked */
	uint8_t	mask;

	/** Mask offset
	The offset from the beginning of the extracted header where mask is
	applied (offset value can be between 0x00-0x0F).
	It is up to the user to ensure that the offset is not larger than the
	extracted header. */
	uint8_t	mask_offset;
};



/**************************************************************************//**
@Description	Key Composition Rule (kcr) builder FEC mask array
*//***************************************************************************/
struct	kcr_builder_fec_mask {
	/** An array of up to 4 pairs of bit-wise masks and offsets.
	Masks are applied to the extracted header at the corresponding offset
	from its beginning */
	struct kcr_builder_fec_single_mask single_mask[4];

	/** Number of masks (1-4) */
	uint8_t	num_of_masks;
};

/** @} */ /* end of FSL_KEYGEN_STRUCTS */

#include "keygen_inline.h"

/**************************************************************************//**
@Group		FSL_KEYGEN_Functions KEYGEN Functions

@Description	Freescale AIOP KEYGEN Functions

@{
*//***************************************************************************/


/* ######################################################################### */
/* ######################## Key and Hash Generation ######################## */
/* ######################################################################### */

/**************************************************************************//**
@Function	keygen_kcr_builder_init

@Description	Initializes key composition rule 
		(kcr).

		This function should be called before any call to other
		functions from keygen_kcr_builder() function family.

@Param[in,out]	kb - kcr builder pointer.
		Must be located in the workspace.
		Must be aligned to 16B boundary.

@Return		None.
*//***************************************************************************/
inline void keygen_kcr_builder_init(struct kcr_builder *kb);


/**************************************************************************//**
@Function	keygen_kcr_builder_add_constant_fec

@Description	Adds user defined constant Field Extract Command (FEC)
		for key composition rule (kcr).

@Param[in]	constant - 1 bytes of user defined constant.
@Param[in]	num - Number of replications (1-16) of the constant in the key.
@Param[in,out]	kb - kcr builder pointer (located in the workspace).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EINVAL - KCR exceeds maximum KCR size (64 bytes).
*//***************************************************************************/
int keygen_kcr_builder_add_constant_fec(uint8_t constant, uint8_t num,
					  struct kcr_builder *kb);


/**************************************************************************//**
@Function	keygen_kcr_builder_add_input_value_fec

@Description	Adds Field Extract Command (FEC) to key composition rule (kcr)
		for extraction of an input value (user_metadata) supplied in
		\ref keygen_gen_key.

@Param[in]	offset - Offset in input value given in keygen_gen_key.
@Param[in]	extract_size - size of extraction.
		Please note that (offset + extract_size) must not exceed 8.
@Param[in]	mask - a structure of up to 4 bitwise masks from defined
		offsets. If user is not interested in mask for this FEC,
		this parameter should be NULL.
@Param[in,out]	kb - kcr builder pointer (located in the workspace).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EINVAL - KCR exceeds maximum KCR size (64 bytes).
*//***************************************************************************/
int keygen_kcr_builder_add_input_value_fec(uint8_t offset,
					   uint8_t extract_size,
					   struct kcr_builder_fec_mask *mask,
					   struct kcr_builder *kb);


/**************************************************************************//**
@Function	keygen_kcr_builder_add_protocol_specific_field

@Description	Adds protocol specific Field Extract Command (FEC) for
		key composition rule.

@Param[in]	protocol_fecid - User should choose one of the FECID's in:
		\ref kcr_builder_protocol_fecid
@Param[in]	mask - a structure of up to 4 bitwise masks from defined
		offsets. If user is not interested in mask for this FEC,
		this parameter should be NULL.
@Param[in,out]	kb - kcr builder pointer (located in the workspace).

@remark		Note that extraction will take place only if there is no
		parsing error related to this fecid.
		In case parsing error exists, the fec is considered invalid.
		The key composition places the value of "000..." in the field
		(the number of 0s corresponds to the size of the field).
		The user can call keygen_kcr_builder_add_valid_field_fec
		function in order to get indications of which fec's are valid.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EINVAL - KCR exceeds maximum KCR size (64 bytes).
*//***************************************************************************/
inline int keygen_kcr_builder_add_protocol_specific_field
		(enum kcr_builder_protocol_fecid protocol_fecid,
		struct kcr_builder_fec_mask *mask,
		struct kcr_builder *kb);

/**************************************************************************//**
@Function	keygen_kcr_builder_add_protocol_based_generic_fec

@Description	Adds protocol based generic extraction Field Extract Command
		(FEC) for key composition rule (kcr).

@Param[in]	pr_offset - offset in parser result to select the protocol
		specific offset. Field extraction starts from this offset.
		Please refer to \ref kcr_builder_parse_result_offset.
@Param[in]	extract_offset - offset from the beginning of the protocol
		header. In Rev1: Must not exceed 0xF.
@Param[in]	extract_size - size of extraction (1-16 bytes).
@Param[in]	mask - a structure of up to 4 bitwise masks from defined
		offsets. If user is not interested in mask for this FEC,
		this parameter should be NULL.
@Param[in,out]	kb - kcr builder pointer (located in the workspace).

@remark		Note that extraction (using \ref keygen_gen_key) will take place
		only if all following conditions are met:
			- The corresponding parse result offset is not 0xFF.
			- The corresponding "Present condition in parser frame
			attribute flags" is met.
			- The corresponding "Error condition in parser frame
			attribute flags" is NOT met (i.e. no errors).
		
		If any condition is not met, the fec is considered invalid.
		The key composition places the value of "000..." in the field
		(the number of 0s corresponds to the size of the field which
		does not meet the condition).
		The user can call keygen_kcr_builder_add_valid_field_fec
		function in order to get indications of which fec's are valid.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EINVAL - KCR exceeds maximum KCR size (64 bytes).
*//***************************************************************************/
int keygen_kcr_builder_add_protocol_based_generic_fec(
	enum kcr_builder_parse_result_offset pr_offset,
	uint8_t extract_offset, uint8_t extract_size,
	struct kcr_builder_fec_mask *mask, struct kcr_builder *kb);


/**************************************************************************//**
@Function	keygen_kcr_builder_add_generic_extract_fec

@Description	Adds generic extraction Field Extract Command (FEC) for
		key composition rule (kcr).

		This function adds to kcr a fec which will cause extraction of
		specified offset and size out of the frame or the parse result.

@Param[in]	offset - offset in frame or parse result.
		Please note that:
		in case of extraction from frame offset must not exceed 0xFF.
		in case of extraction from parse result offset must not exceed
		0x3F.
@Param[in]	extract_size - size of extraction (1-16 bytes).
@Param[in]	gec_source - Please refer to \ref kcr_builder_gec_source.
@Param[in]	mask - a structure of up to 4 bitwise masks from defined
		offsets. If user is not interested in mask for this FEC,
		this parameter should be NULL.
@Param[in,out]	kb - kcr builder pointer (located in the workspace).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EINVAL - KCR exceeds maximum KCR size (64 bytes).
*//***************************************************************************/
int keygen_kcr_builder_add_generic_extract_fec(uint8_t offset,
	uint8_t extract_size, enum kcr_builder_gec_source gec_source,
	struct kcr_builder_fec_mask *mask, struct kcr_builder *kb);



/**************************************************************************//**
@Function	keygen_kcr_builder_add_valid_field_fec

@Description	Adds Field Extract Command (FEC) valid field for
		key composition rule.

		This function adds to kcr a valid field (VF) fec, which
		includes valid bits for last 8 consecutive fec's in the kcr.
		Each bit in the VF corresponds to one field which was extracted
		before VF. The bit is set if the corresponding extracted field
		is valid.
		This can be used to distinguish between an field in the key
		which its value is 0 - this can be either due to this field's
		real value in the frame (in this case valid bit = 1) or because
		this field could not be extracted from the frame (e.g. due to
		parsing	error) and therefore the value 0 was placed in the key
		instead (in this case valid bit = 0)).

@Param[in]	mask - 1 byte mask.
@Param[in,out]	kb - kcr builder pointer (located in the workspace).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EINVAL - KCR exceeds maximum KCR size (64 bytes).
*//***************************************************************************/
int keygen_kcr_builder_add_valid_field_fec(uint8_t mask,
					 struct kcr_builder *kb);


/**************************************************************************//**
@Function	keygen_kcr_create

@Description	Creates key composition rule. Up to 256 rules are supported.

@Param[in]	acc_id - Accelerator ID.
@Param[in]	kcr - Key composition rule. Must be aligned to 16B boundary.
		(part of struct kcr_builder (located in the workspace)).
@Param[out]	keyid - Key ID (located in the workspace).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		ENOSPC - No more KCR's are available (all 256 are taken).

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int keygen_kcr_create(enum keygen_hw_accel_id acc_id,
			uint8_t *kcr,
			uint8_t *keyid);


/**************************************************************************//**
@Function	keygen_kcr_replace

@Description	Replaces key composition rule of a specified keyid.

@Param[in]	acc_id - Accelerator ID.
@Param[in]	kcr - Key composition rule. Must be aligned to 16B boundary.
		(part of struct kcr_builder (located in the workspace)).
@Param[in]	keyid - Key ID.

@Return		None.

@Cautions	In this function the task yields.
*//***************************************************************************/
void keygen_kcr_replace(enum keygen_hw_accel_id acc_id,
			 uint8_t *kcr,
			 uint8_t keyid);


/**************************************************************************//**
@Function	keygen_kcr_delete

@Description	Deletes key composition rule.

@Param[in]	acc_id - Accelerator ID.
@Param[in]	keyid - Key ID (located in the workspace).

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		ENAVAIL - All KCR's are already deleted.

@Cautions	In this function the task yields.
*//***************************************************************************/
int keygen_kcr_delete(enum keygen_hw_accel_id acc_id,
			uint8_t keyid);


/**************************************************************************//**
@Function	keygen_kcr_query

@Description	Returns the key composition rule of a given key ID.

@Param[in]	acc_id - Accelerator ID.
@Param[in]	keyid - The key ID (located in the workspace).
@Param[out]	kcr - Key composition rule. Must be aligned to 16B boundary.
		(part of struct kcr_builder (located in the workspace)).

@Return		None.

@Cautions	In this function the task yields.
*//***************************************************************************/
void keygen_kcr_query(enum keygen_hw_accel_id acc_id,
		       uint8_t keyid,
		       uint8_t *kcr);


/**************************************************************************//**
@Function	keygen_gen_key

@Description	Extracts a key from a frame and returns it.
				The fields order in the key is according to the FECs order
				in the Key Composition Rule that is related to the keyid.

@Param[in]	acc_id - Accelerator ID.
@Param[in]	keyid - The key ID to be used for the key extraction.
@Param[in]	user_metadata - user_metadata field for key composition.
		(will be taken only if the KCR includes
		keygen_kcr_builder_add_input_value_fec).
@Param[out]	key - The key. 128 bytes (regardless of actual key size)
		which should be located in the workspace and must be aligned to
		16B boundary.
@Param[out]	key_size - Key size in bytes.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		EIO - Extract Out Of Frame Header.

@Cautions	In this function the task yields.
 	 	This function may result in a fatal error.
 	 	Presented header address in the workspace must be aligned to
 	  	16 bytes.
 	 	In Rev1: Due to HW bug (ERR008450) the maximum key size allowed
 	 	is 80 bytes for CTLU and 48 bytes for MFLU.
*//***************************************************************************/
inline int keygen_gen_key(enum keygen_hw_accel_id acc_id,
		     uint8_t keyid,
		     uint64_t user_metadata,
		     void *key,
		     uint8_t *key_size);


/**************************************************************************//**
@Function	keygen_gen_hash

@Description	Generates a hash value from a given key.

@Param[in]	key - The key to generate hash from (located in the workspace).
		Must be aligned to 16B boundary.
@Param[in]	key_size - Key size in bytes.
@Param[out]	hash - The hash result.

@Return		0 on Success.

@Cautions	In this function the task yields.
 	 	This function may result in a fatal error.
*//***************************************************************************/
int keygen_gen_hash(void *key, uint8_t key_size, uint32_t *hash);

/** @} */ /* end of FSL_TABLE_Functions */
/** @} */ /* end of FSL_TABLE */
/** @} */ /* end of ACCEL */


#endif /* __FSL_KEYGEN_H */
