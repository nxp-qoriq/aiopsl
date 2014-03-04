/**************************************************************************//**
@File		fsl_keygen.h

@Description	This file contains the AIOP Software Key Generation API

		Copyright 2013-2014 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __FSL_KEYGEN_H
#define __FSL_KEYGEN_H

#include "common/types.h"

/* TODO remark on allocations of out params */
/**************************************************************************//**
 @Group		ACCEL ACCEL (Accelerator APIs)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group	FSL_TABLE Table

@Description	Freescale AIOP Table API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_TABLE_MACROS Table Macros

@Description	Freescale AIOP Table Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_CTLU_KCR_ATTRIBUTES Key Composition Rule Attributes
@{
*//***************************************************************************/
	/** Generic Extraction from start of frame. */
#define CTLU_KCR_LENGTH		0x40

/** @} */ /* end of FSL_CTLU_KCR_ATTRIBUTES */


/**************************************************************************//**
@Group	FSL_CTLU_KCR_BUILDER_GEC_FLAGS \
	 Key Composition Rule Builder Generic Extract Flags.
	User should select one of the followings.
@{
*//***************************************************************************/
	/** Generic Extraction from start of frame. */
#define CTLU_KCR_GEC_FRAME		0x80

	/** Generic Extraction from Parser Result. */
#define CTLU_KCR_GEC_PARSE_RES		0x40

/** @} */ /* end of FSL_CTLU_KCR_BUILDER_GEC_FLAGS */

/**************************************************************************//**
@Group	FSL_CTLU_KCR_BUILDER_EXT_LOOKUP_RES_FIELD \
	 Key Composition Rule Builder Lookup Result Field Extract
@{
*//***************************************************************************/
	/** Extract Opaque0 Field from Lookup Result */
#define CTLU_KCR_EXT_OPAQUE0		0x00
	/** Extract Opaque1 Field from Lookup Result */
#define CTLU_KCR_EXT_OPAQUE1		0x01
	/** Extract Opaque2 Field from Lookup Result */
#define CTLU_KCR_EXT_OPAQUE2		0x02

/** @} */ /* end of FSL_CTLU_KCR_BUILDER_EXT_LOOKUP_RES_FIELD */


/**************************************************************************//**
@Group	FSL_CTLU_STATUS Status returned to calling function
@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_CTLU_STATUS_KCR_CREATE Status returned from Key composition rule \
	create SR
@{
*//***************************************************************************/
	/** Command successful. A rule with matching KeyID was not found.
	Key composition rule was updated. */
#define CTLU_KCR_CREATE_SUCCESS				0x00000000
	/** Command successful. A rule with matching KeyID was found.
	Key composition rule was replaced. */
#define CTLU_KCR_CREATE_STATUS_KCR_REPLACED	(CTLU_STATUS_MGCF | 0x00010000)
/** Command failed. KeyID was not fetched from pool due to CDMA write error */
#define CTLU_KCR_CREATE_GET_ID_STATUS_CDMA_WR_FAILURE \
						(CTLU_STATUS_MGCF | 0x00000001)
/** Command failed. KeyID was not fetched from pool due to pool out of range */
#define CTLU_KCR_CREATE_GET_ID_STATUS_POOL_OUT_OF_RANGE \
						(CTLU_STATUS_MGCF | 0x00000002)
/** Command failed. KeyID was not fetched from pool due to CDMA read error */
#define CTLU_KCR_CREATE_GET_ID_STATUS_CDMA_RD_FAILURE \
						(CTLU_STATUS_MGCF | 0x00000003)

/** @} */ /* end of FSL_CTLU_STATUS_KCR_CREATE */


/**************************************************************************//**
@Group	FSL_CTLU_STATUS_KCR_REPLACE Status returned from Key composition rule \
	replace SR
@{
*//***************************************************************************/
	/** Command successful. A rule with matching KeyID was found.
	Key composition rule was replaced. */
#define CTLU_KCR_REPLACE_SUCCESS		0x00000000
	/** Command failed, A rule with matching KeyID was not found */
#define CTLU_KCR_REPLACE_FAIL_INVALID_KID	(CTLU_STATUS_MGCF | 0x00000001)

/** @} */ /* end of FSL_CTLU_STATUS_KCR_REPLACE */


/**************************************************************************//**
@Group	FSL_CTLU_STATUS_KCR_DELETE Status returned from Key \
	 composition rule delete SR
@{
*//***************************************************************************/
	/** Command successful. A rule with matching KeyID was deleted */
#define CTLU_KCR_DELETE_SUCCESS					0x00000000
	/** Command failed. A rule with matching KeyID was not found */
#define CTLU_KCR_DELETE_FAIL_INVALID_KID	(CTLU_STATUS_MGCF | 0x00000001)
/** Command failed. KeyID was not returned to pool due to CDMA write error */
#define CTLU_KCR_DELETE_RELEASE_ID_STATUS_CDMA_WR_FAILURE\
					(CTLU_STATUS_MGCF | 0x00000001)
/** Command failed. KeyID was not returned to pool due to pool out of range */
#define CTLU_KCR_DELETE_RELEASE_ID_STATUS_POOL_OUT_OF_RANGE\
					(CTLU_STATUS_MGCF | 0x00000002)
/** Command failed. KeyID was not returned to pool due to CDMA read error */
#define CTLU_KCR_DELETE_RELEASE_ID_STATUS_CDMA_RD_FAILURE\
					(CTLU_STATUS_MGCF | 0x00000003)

/** @} */ /* end of FSL_CTLU_STATUS_KCR_DELETE */


/**************************************************************************//**
@Group	FSL_CTLU_STATUS_KCR_QUERY Status returned from Key composition rule \
	 query SR
@{
*//***************************************************************************/
	/** Command successful */
#define CTLU_KCR_QUERY_STATUS_SUCCESS	CTLU_STATUS_MGCWD

/** @} */ /* end of FSL_CTLU_STATUS_KCR_QUERY */

/**************************************************************************//**
@Group	FSL_CTLU_STATUS_KCR Status returned from Key Composition Rule Builder
@{
*//***************************************************************************/
	/** Successful KCR Builder Operation */
#define CTLU_KCR_SUCCESSFUL_OPERATION		0x00000000
	/** General Extraction Extract Size Error */
#define CTLU_KCR_EXTRACT_SIZE_ERR		0x80000001
	/** Protocol Based General Extraction Error */
#define CTLU_KCR_PROTOCOL_GEC_ERR		0x80000002
	/** Protocol Based General Extraction Parser Result Offset Error */
#define CTLU_KCR_PR_OFFSET_ERR			0x80000003
	/** General Extraction Extract Offset Error */
#define CTLU_KCR_EXTRACT_OFFSET_ERR		0x80000004
	/** User Defined Extraction Error */
#define CTLU_KCR_UDC_FEC_ERR			0x80000005
	/** Mask Offset Larger than 0x0F Error */
#define CTLU_KCR_MASK_OFFSET_ERR		0x80000006
	/** Lookup Result Field Extraction Error */
#define CTLU_KCR_BUILDER_EXT_LOOKUP_RES_ERR	0x80000007
	/** Key Composition Rule Size exceeds KCR max size (64 bytes) */
#define CTLU_KCR_SIZE_ERR			0x80000008

/** @} */ /* end of FSL_CTLU_STATUS_KCR */

/** @} */ /* end of FSL_CTLU_STATUS */

/** @} */ /* end of FSL_CTLU_MACROS */

/**************************************************************************//**
@Group		FSL_CTLU_Enumerations CTLU Enumerations

@Description	CTLU Enumerations

@{
*//***************************************************************************/

/**************************************************************************//**
@enum	kcr_builder_parse_result_offset

@Description	 Key Composition Rule Builder Parse Result Offset

@{
*//***************************************************************************/
enum kcr_builder_parse_result_offset {
	/** Running Sum field's offset in parser result */
	RUNNING_SUM_OFFSET_IN_PR = 0x2,
	/** Frame Attribute Flags field's offset in parser result */
	FRAME_ATTRIBUTE_FLAGS_OFFSET_IN_PR = 0x4,
	/** Shim Offset 1 field's offset in parser result */
	SHIM_OFFSET_1_OFFSET_IN_PR = 0x10,
	/** Shim Offset 2 field's offset in parser result */
	SHIM_OFFSET_2_OFFSET_IN_PR = 0x11,
	/** IP NH/protocol offset field's offset in parser result */
	IP_PID_OFFSET_OFFSET_IN_PR = 0x12,
	/** Ethernet offset field's offset in parser result */
	ETH_OFFSET_OFFSET_IN_PR = 0x13,
	/** llc_snap offset field's offset in parser result */
	LLC_SNAP_OFFSET_OFFSET_IN_PR = 0x14,
	/** First VLAN's TCI offset field's offset in parser result */
	TCI1_OFFSET_OFFSET_IN_PR = 0x15,
	/** Last VLAN's TCI offset field's offset in parser result */
	TCIN_OFFSET_OFFSET_IN_PR = 0x16,
	/** Last Ethertype offset field's offset in parser result */
	LAST_ETYPE_OFFSET_OFFSET_IN_PR = 0x17,
	/** PPPoE offset field's offset in parser result */
	PPPOE_OFFSET_OFFSET_IN_PR = 0x18,
	/** First MPLS offset field's offset in parser result */
	MPLS_OFFSET_1_OFFSET_IN_PR = 0x19,
	/** Last MPLS offset field's offset in parser result */
	MPLS_OFFSET_N_OFFSET_IN_PR = 0x1A,
	/** First IP or ARP offset field's offset in parser result */
	IP1_OR_ARP_OFFSET_OFFSET_IN_PR = 0x1B,
	/** Last IP or MinEncap offset field's offset in parser result */
	IPN_OR_MINENCAP0_OFFSET_OFFSET_IN_PR = 0x1C,
	/** GRE offset field's offset in parser result */
	GRE_OFFSET_OFFSET_IN_PR = 0x1D,
	/** Layer 4 offset field's offset in parser result */
	L4_OFFSET_OFFSET_IN_PR = 0x1E,
	/** GTP/ESP/IPsec offset field's offset in parser result */
	GTP_ESP_IPSEC_OFFSET_OFFSET_IN_PR = 0x1F,
	/** Next header field's offset in parser result */
	NXT_HDR_OFFSET_IN_PR = 0x20,
	/** Next header offset field's offset in parser result */
	NXT_HDR_OFFSET_OFFSET_IN_PR = 0x22,
	/** IPv6 fragmentable part offset field's offset in parser result */
	IPV6_FRAG_OFFSET_OFFSET_IN_PR = 0x23,
	/** Soft parsing context */
	SOFT_PARSING_CONTEXT = 0x27
};

/** @} */ /* end of kcr_builder_parse_result_offset */

/**************************************************************************//**
@enum kcr_builder_protocol_fecid

@Description	Key Composition Rule Builder Protocol FECID

@{
*//***************************************************************************/
enum kcr_builder_protocol_fecid {
	/** FECID of Generic Extraction Command */
	CTLU_KCR_GEC_FECID = 0x00,
	/** FECID of User defined constant */
	CTLU_KCR_UDC_FECID = 0x01,
	/** FECID of Valid Field */
	CTLU_KCR_VF_FECID = 0x02,
	/** FECID of MAC destination address */
	CTLU_KCR_MACDST_FECID = 0x03,
	/** FECID of MAC source address */
	CTLU_KCR_MACSRC_FECID = 0x04,
	/** FECID of VLAN TCI from the first Q-Tag in the frame */
	CTLU_KCR_VLANTCI_1_FECID = 0x05,
	/** FECID of VLAN TCI from the last Q-Tag in the frame */
	CTLU_KCR_VLANTCI_N_FECID = 0x06,
	/** FECID of Ethernet Type field */
	CTLU_KCR_ETYPE_FECID = 0x07,
	/** FECID of PPPoE Session ID Field */
	CTLU_KCR_PPPSID_FECID = 0x08,
	/** FECID of PPP Protocol ID Field */
	CTLU_KCR_PPPPID_FECID = 0x09,
	/** FECID of MPLS first label with TC */
	CTLU_KCR_MPLSL_1_FECID = 0x0A,
	/** FECID of MPLS second label with TC
	( present in the frame if MPLSOffset_n-MPLSOffset_1>= 4)*/
	CTLU_KCR_MPLSL_2_FECID = 0x0B,
	/** FECID of MPLS last label with TC */
	CTLU_KCR_MPLSL_N_FECID = 0x0C,
	/** FECID of ARP Operation */
	CTLU_KCR_ARP_OP_FECID = 0x0D,
	/** FECID of ARP Sender protocol Address */
	CTLU_KCR_ARP_SPA_FECID = 0x0E,
	/** FECID of ARP Target protocol Address */
	CTLU_KCR_ARP_TPA_FECID = 0x0F,
	/** FECID of ARP Sender Hardware Address */
	CTLU_KCR_ARP_SHA_FECID = 0x10,
	/** FECID of ARP Target Hardware Address */
	CTLU_KCR_ARP_THA_FECID = 0x11,
	/** FECID of first (outer) IPv4/6 header Source Address */
	CTLU_KCR_IPSRC_1_FECID = 0x12,
	/** FECID of first (outer) IPv4/6 header Destination Address */
	CTLU_KCR_IPDST_1_FECID = 0x13,
	/** FECID of first (outer) IPv4 Protocol Type or IPv6 next header */
	CTLU_KCR_PTYPE_1_FECID = 0x14,
	/** FECID of first (outer) TOS (IPv4) or Traffic Class (IPv6) */
	CTLU_KCR_IPTOS_TC_1_FECID = 0x15,
	/** FECID ofIP Identification for IP Reassembly
	* (In IPv4 in IP header, in IPv6 in fragment extension) */
	CTLU_KCR_IPID_1_FECID = 0x16,
	/** FECID of first (outer) IPv6 Flow Label */
	CTLU_KCR_IPV6FL_1_FECID = 0x17,
	/** FECID of last IP source / Min. Encap Source Address
	FECID of last (inner) IPv4/6 Source Address or Min. Encap
	Source Address Field */
	CTLU_KCR_IPSRC_N_FECID = 0x18,
	/** FECID of last IP dest / Min. Encap dest Address
	FECID of last (inner) IPv4/6 Destination or Min. Encap Destination
	Address field */
	CTLU_KCR_IPDST_N_FECID = 0x19,
	/** FECID of last IPv4 Protocol Type / IPv6 NH / Encap dest Address
	FECID of IPv4 Protocol Type Field or IPv6 next header of the
	last (inner) IP header, or Min. Encap protocol type */
	CTLU_KCR_PTYPE_N_FECID = 0x1A,
	/** FECID of last (inner) TOS (IPv4) or Traffic Class(IPv6) */
	CTLU_KCR_IPTOS_TC_N_FECID = 0x1B,
	/** FECID of IP Identification for IP Reassembly
	(In IPv4 in IP header, in IPv6 in fragment extension) */
	CTLU_KCR_IPID_N_FECID = 0x1C,
	/** FECID of IPv6 Flow Label of the last (inner) IP header */
	CTLU_KCR_IPV6FL_N_FECID = 0x1D,
	/** FECID of GRE Protocol Type field */
	CTLU_KCR_GREPTYPE_FECID = 0x1E,
	/** FECID of TCP or UDP or SCTP or DCCP source Port Field */
	CTLU_KCR_L4PSRC_FECID = 0x1F,
	/** FECID of TCP or UDP or SCTP or DCCP destination Port Field */
	CTLU_KCR_L4PDST_FECID = 0x20,
	/** FECID of 14th byte of the TCP header, contains TCP flags */
	CTLU_KCR_TFLG_FECID = 0x21,
	/** FECID of IPSec SPI field */
	CTLU_KCR_IPSECSPI_FECID = 0x22,
	/** FECID of IPSec (AH only) Next Header field */
	CTLU_KCR_IPSECNH_FECID = 0x23,
	/** FECID of GPRS Tunnel endpoint Identification */
	CTLU_KCR_GTP_TEID_FECID = 0x24,
	/** FECID of ICMP type */
	CTLU_KCR_ICMP_TYPE_FECID = 0x25,
	/** FECID of ICMP code */
	CTLU_KCR_ICMP_CODE_FECID = 0x26,
	/** FECID of PBB ISID */
	CTLU_KCR_PBB_ISID_FECID = 0x27,
	/** FECID of Next Header */
	CTLU_KCR_NXT_HDR_FECID = 0x28
	/* TODO check spec  for updates */
};
/** @} */ /* end of kcr_builder_protocol_fecid */

/**************************************************************************//**
@enum	table_hw_accel_id

@Description	IDs of hardware table lookup accelerator

@{
*//***************************************************************************/
enum keygen_hw_accel_id {
	/** MFLU accelerator ID */
	KEYGEN_ACCEL_ID_MFLU = 0x02,
	/** CTLU accelerator ID */
	KEYGEN
	_ACCEL_ID_CTLU = 0x05
};

/** @} */ /* end of table_hw_accel_id */

/** @} */ /* end of FSL_CTLU_Enumerations */

/**************************************************************************//**
@Group		FSL_CTLU_STRUCTS CTLU Structures

@Description	Freescale AIOP CTLU Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Key Composition Rule (kcr) builder
*//***************************************************************************/
struct	ctlu_kcr_builder {
	/** KCR as defined by CTLU
	must be initialized by ctlu_kcr_builder_init() before use */
	uint8_t  kcr[CTLU_KCR_LENGTH];

	/** KCR length
	Number of bytes the NFEC and FECs occupy */
	uint8_t  kcr_length;
};


/**************************************************************************//**
@Description	Key Composition Rule (kcr) builder FEC single mask
*//***************************************************************************/
struct	ctlu_kcr_builder_fec_single_mask {
	/** Bit-wise mask
	Applied to the extracted header at the corresponding offset from its
	beginning */
	uint8_t	mask;

	/** Mask offset.
	offset value can be between 0x00-0x0F.
	It is up to the user to ensure that the offset is not larger than the
	extracted header. */
	uint8_t	mask_offset;
};



/**************************************************************************//**
@Description	Key Composition Rule (kcr) builder FEC mask array
*//***************************************************************************/
struct	ctlu_kcr_builder_fec_mask {
	/** An array of up to 4 pairs of bit-wise masks and offsets.
	Masks are applied to the extracted header at the corresponding offset
	from its beginning */
	struct ctlu_kcr_builder_fec_single_mask single_mask[4];

	/** Number of masks (1-4) */
	uint8_t	num_of_masks;
};

/** @} */ /* end of FSL_CTLU_STRUCTS */


/**************************************************************************//**
@Group		FSL_CTLU_Functions Table Functions

@Description	Freescale AIOP Table Functions

@{
*//***************************************************************************/


/* ######################################################################### */
/* ######################## Key and Hash Generation ######################## */
/* ######################################################################### */

/**************************************************************************//**
@Function	ctlu_kcr_builder_init

@Description	Initializes key composition rule (kcr).

		This function should be called before any call to other
		functions from ctlu_kcr_builder() function family.

@Param[in,out]	kb - kcr builder pointer. Must not be null (user should
		allocate memory for this structure).

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR.
*//***************************************************************************/
int32_t ctlu_kcr_builder_init(struct ctlu_kcr_builder *kb);


/**************************************************************************//**
@Function	ctlu_kcr_builder_add_constant_fec

@Description	Adds user defined constant Field Extract Command (FEC)
		for key composition rule (kcr).

@Param[in]	constant - 1 bytes of user defined constant.
@Param[in]	num - Number of replications (1-16) of the constant in the key.
@Param[in,out]	kb - kcr builder pointer.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR.
*//***************************************************************************/
int32_t ctlu_kcr_builder_add_constant_fec(uint8_t constant, uint8_t num,
					  struct ctlu_kcr_builder *kb);


/**************************************************************************//**
@Function	ctlu_kcr_builder_add_protocol_specific_field

@Description	Adds protocol specific Field Extract Command (FEC) for
		key composition rule.

@Param[in]	protocol_fecid - User should choose one of the FECID's in:
		\ref kcr_builder_protocol_fecid
@Param[in]	mask - a structure of up to 4 bitwise masks from defined
		offsets. If user is not interested in mask for this FEC,
		this parameter should be NULL.
@Param[in,out]	kb - kcr builder pointer.

		Note that extraction will take place only if there is no
		parsing error related to this fecid.
		In case parsing error exists, the fec is considered invalid.
		The key composition places the value of "000..." in the field
		(the number of 0s corresponds to the size of the field).
		The user can call ctlu_kcr_builder_add_valid_field_fec
		function in order to get indications of which fec's are valid.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR.
*//***************************************************************************/
int32_t ctlu_kcr_builder_add_protocol_specific_field
		(enum kcr_builder_protocol_fecid protocol_fecid,
		struct ctlu_kcr_builder_fec_mask *mask,
		struct ctlu_kcr_builder *kb);

/**************************************************************************//**
@Function	ctlu_kcr_builder_add_protocol_based_generic_fec

@Description	Adds protocol based generic extraction Field Extract Command
		(FEC) for key composition rule (kcr).

@Param[in]	pr_offset - offset in parser result to select the protocol
		specific offset. Field extraction starts from this offset.
		Please refer to \ref kcr_builder_parse_result_offset.
@Param[in]	extract_offset - offset from the beginning of the protocol
		header.
@Param[in]	extract_size - size of extraction.
@Param[in]	mask - a structure of up to 4 bitwise masks from defined
		offsets. If user is not interested in mask for this FEC,
		this parameter should be NULL.
@Param[in,out]	kb - kcr builder pointer.

		Note that extraction will take place only if all following
		conditions are met:
		- The corresponding parse result offset is not 0xFF
		- The corresponding "Present condition in parser frame
		attribute flags" is met
		- The corresponding "Error condition in parser frame
		attribute flags" is NOT met (i.e. no errors)

		If any condition is not met, the fec is considered invalid.
		The key composition places the value of "000..." in the field
		(the number of 0s corresponds to the size of the field which
		does not meet the condition).
		The user can call ctlu_kcr_builder_add_valid_field_fec
		function in order to get indications of which fec's are valid.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR.
*//***************************************************************************/
int32_t ctlu_kcr_builder_add_protocol_based_generic_fec(
	enum kcr_builder_parse_result_offset pr_offset,
	uint8_t extract_offset, uint8_t extract_size,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb);


/**************************************************************************//**
@Function	ctlu_kcr_builder_add_generic_extract_fec

@Description	Adds generic extraction Field Extract Command (FEC) for
		key composition rule (kcr).

		This function adds to kcr a fec which will cause extraction of
		specified offset and size out of the frame or the parse result.

@Param[in]	offset - offset in frame or parse result.
@Param[in]	extract_size - size of extraction.
@Param[in]	flags - Please refer to \ref FSL_CTLU_KCR_BUILDER_GEC_FLAGS
@Param[in]	mask - a structure of up to 4 bitwise masks from defined
		offsets. If user is not interested in mask for this FEC,
		this parameter should be NULL.
@Param[in,out]	kb - kcr builder pointer.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR.
*//***************************************************************************/
int32_t ctlu_kcr_builder_add_generic_extract_fec(uint8_t offset,
	uint8_t extract_size, uint32_t flags,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb);


/**************************************************************************//**
@Function	ctlu_kcr_builder_add_lookup_result_field_fec

@Description	This function adds extracted lookup result field
		Field Extract Command (FEC) for key composition rule (kcr).

@Param[in]	extract_field - Please refer to \ref
		FSL_CTLU_KCR_BUILDER_EXT_LOOKUP_RES_FIELD.
@Param[in]	offset_in_opaque - Offset in Opaque0 or Opaque1 in lookup
		result.
@Param[in]	extract_size_in_opaque - size of extraction in case of Opaque0
		or Opaque1.
@Param[in]	mask - a structure of up to 4 bitwise masks from defined
		offsets. If user is not interested in mask for this FEC,
		this parameter should be NULL.
@Param[in,out]	kb - kcr builder pointer.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR.
*//***************************************************************************/
int32_t ctlu_kcr_builder_add_lookup_result_field_fec(uint8_t extract_field,
	uint8_t offset_in_opaque, uint8_t extract_size_in_opaque,
	struct ctlu_kcr_builder_fec_mask *mask, struct ctlu_kcr_builder *kb);


/**************************************************************************//**
@Function	ctlu_kcr_builder_add_valid_field_fec

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
@Param[in,out]	kb - kcr builder pointer.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR.
*//***************************************************************************/
int32_t ctlu_kcr_builder_add_valid_field_fec(uint8_t mask,
					 struct ctlu_kcr_builder *kb);


/**************************************************************************//**
@Function	ctlu_kcr_create

@Description	Creates key composition rule.

@Param[in]	kcr - Key composition rule.
@Param[out]	keyid - Key ID.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR_CREATE

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_kcr_create(enum keygen_hw_accel_id acc_id,
			uint8_t *kcr,
			uint8_t *keyid);


/**************************************************************************//**
@Function	ctlu_kcr_replace

@Description	Replaces key composition rule of a specified keyid.

@Param[in]	kcr - Key composition rule.
@Param[in]	keyid - Key ID.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR_REPLACE

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_kcr_replace(enum keygen_hw_accel_id acc_id,
			 uint8_t *kcr,
			 uint8_t keyid);


/**************************************************************************//**
@Function	ctlu_kcr_delete

@Description	Deletes key composition rule.

@Param[in]	keyid - Key ID.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR_DELETE

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_kcr_delete(enum keygen_hw_accel_id acc_id,
			uint8_t keyid);


/**************************************************************************//**
@Function	ctlu_kcr_query

@Description	Returns the key composition rule of a given key ID.

@Param[in]	keyid - The key ID.
@Param[out]	kcr - Key composition rule.
@Param[out]	size - Size of the key that the kcr creates.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR_QUERY

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_kcr_query(enum keygen_hw_accel_id acc_id,
		       uint8_t keyid, uint8_t *kcr,
		       uint8_t *size);


/**************************************************************************//**
@Function	ctlu_gen_key

@Description	Extracts a key from a frame and returns it.

@Param[in]	keyid - The key ID to be used for the key extraction.
@Param[out]	key - The key. This structure is allocated by the user and must
		be aligned to 16B boundary
@Param[out]	key_size - Key size in bytes. Must be allocated by the caller.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_gen_key(enum keygen_hw_accel_id acc_id,
		     uint8_t keyid,
		     union ctlu_key *key,
		     uint8_t *key_size);


/**************************************************************************//**
@Function	ctlu_gen_hash

@Description	Generates a hash value from a given key.

@Param[in]	key - The key to generate hash from. Must be aligned to 16B
		boundary
@Param[in]	key_size - Key size in bytes.
@Param[out]	hash - The hash result. Must be allocated by the caller to this
		function.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_gen_hash(union ctlu_key *key, uint8_t key_size, uint32_t *hash);

/** @} */ /* end of FSL_TABLE_Functions */
/** @} */ /* end of FSL_TABLE */
/** @} */ /* end of ACCEL */


#endif /* __FSL_KEYGEN_H */
