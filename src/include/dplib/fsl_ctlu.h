/**************************************************************************//**
@File		fsl_ctlu.h

@Description	This file contains the AIOP Software CTLU API

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __FSL_CTLU_H
#define __FSL_CTLU_H

#include "common/types.h"


/**************************************************************************//**
 @Group		ACCEL ACCEL (Accelerator APIs)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group	FSL_CTLU CTLU

@Description	Freescale AIOP CTLU API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_CTLU_MACROS CTLU Macros

@Description	Freescale AIOP CTLU Macros

@{
*//***************************************************************************/


/**************************************************************************//**
@Group	FSL_CTLU_TABLE_ATTRIBUTES Table Attributes

@Description	Table Attributes

	The table attributes are composed of the following sub fields:
	- Type field (Exact Match, Longest Prefix Match, etc..)
	- Location (External/PEB/Internal) field
	- Miss result options field
	- Aging Threshold field

	For more details of each one of the sub fields, please refer to:
	- \ref FSL_CTLU_TABLE_ATTRIBUTE_TYPE
	- \ref FSL_CTLU_TABLE_ATTRIBUTE_LOCATION
	- \ref FSL_CTLU_TABLE_ATTRIBUTE_MR
	- \ref FSL_CTLU_TABLE_ATTRIBUTE_AGT
@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_CTLU_TABLE_ATTRIBUTE_TYPE Table Type Attribute

@Description	Table Type

		These macros specifies the table type sub field offset and
		mask and the available table types. \n User should select one
		of the following defines (excluding mask and offset defines):
@{
*//***************************************************************************/
	/** Exact Match table */
#define CTLU_TBL_ATTRIBUTE_TYPE_EM		0x0000

	/** Longest Prefix Match table */
#define CTLU_TBL_ATTRIBUTE_TYPE_LPM		0x1000

/*
	 Ternary match Table for ACL
#define CTLU_TBL_ATTRIBUTE_TYPE_TCAM_ACL	0x2000

	 Algorithmic ACL
#define CTLU_TBL_ATTRIBUTE_TYPE_ALG_ACL		0x4000
*/

	/** Table type sub field mask */
#define CTLU_TBL_ATTRIBUTE_TYPE_MASK		0xF000

	/** Table type sub field offset */
#define CTLU_TBL_ATTRIBUTE_TYPE_OFFSET		12

/** @} */ /* end of FSL_CTLU_TABLE_TYPE */


/**************************************************************************//**
@Group	FSL_CTLU_TABLE_ATTRIBUTE_LOCATION Table Location Attribute

@Description	Table Location

		These macros specifies the table location sub field offset and
		mask and the available table locations. \n User should select
		one of the following defines (excluding mask and offset
		defines):
@{
*//***************************************************************************/
	/** Internal table (located in dedicated RAM) */
#define CTLU_TABLE_ATTRIBUTE_LOCATION_INT	0x0000

	/** Packet Express Buffer table */
#define CTLU_TABLE_ATTRIBUTE_LOCATION_PEB	0x0200

	/** External table (located in DDR) */
#define CTLU_TABLE_ATTRIBUTE_LOCATION_EXT	0x0300

	/** Table Location sub field mask */
#define CTLU_TABLE_ATTRIBUTE_LOCATION_MASK	0x0300

	/** Table Location sub field offset */
#define CTLU_TBL_TYPE_IEX_OFFSET		8

/** @} */ /* end of FSL_CTLU_TABLE_ATTRIBUTE_LOCATION */


/**************************************************************************//**
@Group	FSL_CTLU_TABLE_ATTRIBUTE_MR Table Miss Result Attribute

@Description	Table Miss Result Options

	These macros specifies options of the table miss result and mask and
	and offset of the sub filed.\n User should select one of the following
	defines (excluding mask and offset defines):
@{
*//***************************************************************************/
	/** Table without miss result */
#define CTLU_TBL_ATTRIBUTE_MR_NO_MISS	0x0000

	/** Table with miss result */
#define CTLU_TBL_ATTRIBUTE_MR_MISS	0x0080

	/** Miss result options sub field mask */
#define CTLU_TABLE_ATTRIBUTE_MR_MASK	0x00C0

	/** Miss result options sub field offset */
#define CTLU_TABLE_ATTRIBUTE_MR_OFFSET	6

/** @} */ /* end of FSL_CTLU_TABLE_ATTRIBUTE_MR */


/**************************************************************************//**
@Group	FSL_CTLU_TABLE_ATTRIBUTE_AGT Table Aging Threshold Attribute

@Description	Table Aging Threshold

	These macros specifies Aging Threshold attribute mask and and offset of
	the sub field.
	Aging Threshold is used for removal of aged rules in the table.
	This feature is only enabled to table rules in which
	\ref CTLU_RULE_TIMESTAMP_AGT_ENABLE is enabled in rule[options].
	If enabled, and Current timestamp - rule[timestamp] > 2^AGT (where AGT
	is the value configured in this sub field ) the aging
	function removes the lookup table rule from the lookup table.
	The units in which timestamp is measured are determined according to
	CTLU IOP_CTLU_TIMESTAMP_WINDOW register configuration.
	The sub field is specified by \ref CTLU_TABLE_ATTRIBUTE_AGT_MASK
	and \ref CTLU_TABLE_ATTRIBUTE_AGT_OFFSET (The mask determines the
	size and position of the field).
	NOTE: This field must be cleared unless \ref
	FSL_CTLU_TABLE_ATTRIBUTE_TYPE is set to CTLU_TBL_ATTRIBUTE_TYPE_EM
	(i.e. This field can only be used in exact match tables).

@{
*//***************************************************************************/
	/** Aging Threshold Mask */
#define CTLU_TABLE_ATTRIBUTE_AGT_MASK	0x001F

	/** Aging Threshold field offset */
#define CTLU_TABLE_ATTRIBUTE_AGT_OFFSET	0

/** @} */ /* end of FSL_CTLU_TABLE_ATTRIBUTE_AGT */

/** @} */ /* end of FSL_CTLU_TABLE_ATTRIBUTES */


/**************************************************************************//**
@Group		FSL_CTLU_TABLE_RULE_RESULT_TYPES Table Rule Results Types

@Description	Table Rule Results Types

		User should select one of the following defines:
@{
*//***************************************************************************/
	/** Result is used for chaining of lookups */
#define CTLU_RULE_RESULT_TYPE_CHAINING	0x81

	/** Result is 8B of opaque data and 8B DDR pointer.
	Uses for reference counting */
#define CTLU_RULE_RESULT_TYPE_REFERENCE	0x91

	/** Result is 16B of opaque data */
#define CTLU_RULE_RESULT_TYPE_OPAQUES	0xB1

/** @} *//* end of FSL_CTLU_TABLE_RULE_RESULT_TYPES */


/**************************************************************************//**
@Group	FSL_CTLU_TABLE_RULE_OPTIONS Table Rule Options
@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_CTLU_TABLE_RULE_OPTIONS_TIMESTAMP Table Rule Timestamp Options

@Description	Table Rule Timestamp Options

		User should select one of the following:

@{
*//***************************************************************************/
	/** Timestamp is disabled for this rule*/
#define	CTLU_RULE_TIMESTAMP_NONE	0x00

	/** Enables timestamp update per rule. */
#define	CTLU_RULE_TIMESTAMP_ENABLE	0x80

	/** Enables timestamp update per rule and aging.
	 * Aging is described in \ref FSL_CTLU_TABLE_ATTRIBUTE_AGT.
	 * NOTE: This option must not be used unless at table creation
	 * \ref FSL_CTLU_TABLE_ATTRIBUTE_TYPE was set to
	 * CTLU_TBL_ATTRIBUTE_TYPE_EM. (i.e. this options is available only
	 * in exact match tables)*/
#define CTLU_RULE_TIMESTAMP_AGT_ENABLE	0xC0

/** @} *//* end of FSL_CTLU_TABLE_RULE_OPTIONS_TIMESTAMP */

/** @} *//* end of FSL_CTLU_TABLE_RULE_OPTIONS */


/**************************************************************************//**
@Group	FSL_CTLU_KEY_DEFINES Table Rule Key
@{
*//***************************************************************************/
	/** Exact Match key size */
#define CTLU_TABLE_RULE_KEY_EXACT_MATCH_SIZE	124
	/** Exact Match key padding */
#define CTLU_KEY_EXACT_MATCH_PADDING		4
	/** IPv4 LPM key size */
#define CTLU_KEY_LPM_IPV4_SIZE			0x08
	/** IPv6 LPM key size */
#define CTLU_KEY_LPM_IPV6_SIZE			0x14
	/** IPv4 LPM key padding */
#define CTLU_KEY_LPM_IPV4_PADDING		119
	/** IPv6 LPM key padding */
#define CTLU_KEY_LPM_IPV6_PADDING		107

/** @} */ /* end of FSL_CTLU_KEY_DEFINES */


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
@Group	FSL_CTLU_STATUS_GENERAL General status returned from CTLU
@{
*//***************************************************************************/

/** Command failed general status bit.
A general bit that is set in some errors conditions */
#define CTLU_STATUS_MGCF	0x80000000

/* Parser status are intentionally missing here */

/** Table Lookup Miss
 * This status is set when a matching rule is not found. Note that on chained
 * lookups this status is set only if the last lookup results in a miss. */
#define CTLU_STATUS_MISS	0x00000800 | (CTLU_ACCEL_ID << 24)

/** Key Composition Error
 * This status is set when a key composition error occurs meaning one of the
 * following:
 * - Invalid Key Composition ID was used.
 * - Key Size Error.
 * */
#define CTLU_STATUS_KSE		0x00000400 | (CTLU_ACCEL_ID << 24)

/** Extract Out Of Frame Header
 * This status is set if key composition attempts to extract a field which is
 * not in the frame header either because it is placed beyond the first 256
 * bytes of the frame, or because the frame is shorter than the index evaluated
 * for the extraction. */
#define CTLU_STATUS_EOFH	0x00000200 | (CTLU_ACCEL_ID << 24)

/** Maximum Number Of Chained Lookups Is Reached
 * This status is set if the number of table lookups performed by the CTLU
 * reached the threshold. */
#define CTLU_STATUS_MNLE	0x00000100 | (CTLU_ACCEL_ID << 24)

/** Invalid Table ID
 * This status is set if the lookup table associated with the TID is not
 * initialized. */
#define CTLU_STATUS_TIDE	0x00000080 | (CTLU_ACCEL_ID << 24) | \
						CTLU_STATUS_MGCF

/** Policer Initialization Entry Error*/ //TODO consider to remove as HW removes it.
#define CTLU_STATUS_PIEE	0x00000040 | (CTLU_ACCEL_ID << 24)

/** Resource is not available
 * */
#define CTLU_STATUS_NORSC	0x00000020 | (CTLU_ACCEL_ID << 24) | \
						CTLU_STATUS_MGCF
/** Resource Is Temporarily Not Available
 * Temporarily Not Available occurs if an other resource is in the process of
 * being freed up. Once the process ends, the resource may be available for new
 * allocation (availability is not guaranteed). */
#define CTLU_STATUS_TEMPNOR	0x00000010 | CTLU_STATUS_NORSC

/** ICID Protection Is Violated
 * */
#define CTLU_STATUS_ICIDE	0x00000008 | (CTLU_ACCEL_ID << 24) | \
						CTLU_STATUS_MGCF
/** @} */ /* end of FSL_CTLU_STATUS_GENERAL */

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
#define CTLU_KCR_CREATE_STATUS_KCR_REPLACED 	(CTLU_STATUS_MGCF | 0x00010000)
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
@enum 	kcr_builder_parse_result_offset

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
enum kcr_builder_protocol_fecid{
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

/** @} */ /* end of FSL_CTLU_Enumerations */

/**************************************************************************//**
@Group		FSL_CTLU_STRUCTS CTLU Structures

@Description	Freescale AIOP CTLU Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Table Rule Result Chaining Parameters
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_rule_result_chain_parameters{
	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint32_t reserved0;

	/** Table ID */
	uint16_t table_id;

	/** Key ID */
	uint8_t  keyid;

	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved1;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	\ref ctlu_table_rule_result structure ctlu_op0_refptr_clp field

		This field can be used either:
		- As an opaque field of 8 bytes (\ref ctlu_table_rule_result
		type field should be set to
		\ref CTLU_RULE_RESULT_TYPE_OPAQUES). \n Returned as part of
		lookup result.
		- As a pointer to CDMA application context which has a
		reference counter (\ref ctlu_table_rule_result type field
		should be set to \ref CTLU_RULE_RESULT_TYPE_REFERENCE). \n
		Returned as part of lookup result
		- As a structure containing table ID and key ID parameters for
		a chained lookup (\ref ctlu_table_rule_result type field
		should be set to \ref CTLU_RULE_RESULT_TYPE_CHAINING).
*//***************************************************************************/
#pragma pack(push, 1)
union ctlu_op0_refptr_clp{
	/** Opaque0
	Returned as part of lookup result */
	uint64_t opaque0;

	/** Reference Pointer
	Pointer to CDMA application context.
	The CTLU increments or decrements the reference counter when necessary.
	Returned as part of lookup result. */
	uint64_t reference_pointer;

	/** Chaining Parameters
	A structure that contains table ID and key composition ID parameters
	of the chained lookup. */
	struct ctlu_rule_result_chain_parameters chain_parameters;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Rule Result

		This structure represents the table rule result. Some of the
		fields defined here are returned after lookup, see fields
		specification for more details. \n This structure is used as a
		part of table rule, but also as a stand alone result (e.g.
		table miss result).
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_table_rule_result {
	/** Result Type
	Should be set to one of the types specified in
	\link FSL_CTLU_TABLE_RULE_RESULT_TYPES Result Types \endlink macros. */
	uint8_t  type;

	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint16_t reserved;

	/** Opaque2
	Returned as part of lookup result. Not valid when type is set to
	CTLU_RULE_RESULT_TYPE_CHAINING */
	uint8_t  opaque2;

	/** Opaque0 or Reference Pointer or Chained Lookup Parameters
	This field can be used either:
	- As an opaque field of 8 bytes (type field should be set to
	\ref CTLU_RULE_RESULT_TYPE_OPAQUES). \n Returned as part of lookup
	result.
	- As a pointer to CDMA application context which has a reference
	counter (type field should be set to
	\ref CTLU_RULE_RESULT_TYPE_REFERENCE). \n Returned as part of lookup
	result.
	- As a structure containing table ID and key ID parameters for a
	chained lookup (type field should be set to
	\ref CTLU_RULE_RESULT_TYPE_CHAINING).
	*/
	union ctlu_op0_refptr_clp op_rptr_clp;

	/** Opaque1
	Returned as part of lookup result. Not valid when type is set to
	CTLU_RULE_RESULT_TYPE_CHAINING */
	uint64_t opaque1;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Exact Match Key Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_key_exact_match {
	/** Exact Match key */
	uint8_t  key[CTLU_TABLE_RULE_KEY_EXACT_MATCH_SIZE];

	/** Padding */
	uint8_t  pad[CTLU_KEY_EXACT_MATCH_PADDING];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	LPM IPv4 Key Structure

		The CTLU searches for the LPM of the concatenation of
		{exact_match, full_ipv4_address}.
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_key_lpm_ipv4 {
	/** Exact Match bytes */
	uint32_t exact_match;

	/** IPv4 Address */
	uint32_t addr;

	/** Prefix length.
	In lookups defines the maximum prefix length for this search. The CTLU
	does not search for a prefix length larger than prefix_length. For
	lookup on all prefixes prefix_length = 0xFF. */
	uint8_t	 prefix_length;

	/** Padding */
	uint8_t  pad[CTLU_KEY_LPM_IPV4_PADDING];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	LPM IPv6 Key Structure

		The CTLU searches for the LPM of the concatenation of
		{exact_match, full_ipv6_address}.
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_key_lpm_ipv6 {
	/** Exact Match bytes */
	uint32_t exact_match;

	/** IPv6 Address (8 MSB) */
	uint64_t addr0;

	/** IPv6 Address (8 LSB) */
	uint64_t addr1;

	/** Prefix length.
	In lookups defines the maximum prefix length for this search. The CTLU
	does not search for a prefix length larger than prefix_length. For
	lookup on all prefixes prefix_length = 0xFF. */
	uint8_t  prefix_length;

	/** Padding */
	uint8_t  pad[CTLU_KEY_LPM_IPV6_PADDING];
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Union of 3 Table Result Types

		This union includes:
		- struct ctlu_key_lpm_ipv4
		- struct ctlu_key_lpm_ipv6
		- struct ctlu_key_em
*//***************************************************************************/
union ctlu_key {
		/** Exact Match Key */
	struct ctlu_key_exact_match key_em;
		/** LPM IPv4 Key */
	struct ctlu_key_lpm_ipv4 key_lpm_ipv4;
		/** LPM IPv6 Key */
	struct ctlu_key_lpm_ipv6 key_lpm_ipv6;
};


/**************************************************************************//**
@Description	Table Rule
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_table_rule {
	/** Rule's key
	The structure to be passed is one of the following:
	 - \ref ctlu_key_exact_match
	 - \ref ctlu_key_lpm_ipv4
	 - \ref ctlu_key_lpm_ipv6 */
	union ctlu_key key;

	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint64_t reserved0;

	/** Table Rule flags
	Please refer to \link FSL_CTLU_TABLE_RULE_OPTIONS table rule options
	\endlink for more details.*/
	uint8_t  options;

	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved1[3];

	/** Table Rule Result */
	struct ctlu_table_rule_result result;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Table Lookup Result

		The structure returned from the CTLU upon a successful lookup.
*//***************************************************************************/
#pragma pack(push, 1)
struct ctlu_lookup_result {
	/** Opaque0 or Reference Pointer
	This field can be either:
	- 8 bytes of opaque data.
	- A pointer to CDMA application context which has a reference pointer.
	Depending on the matching rule result type */
	uint64_t opaque0_or_reference;

	/** Opaque1 */
	uint64_t opaque1;

	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint16_t reserved0;

	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint8_t  reserved1;

	/** Opaque2 */
	uint8_t  opaque2;

	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint64_t reserved2;

	/** Reserved
	Reserved for compliance with HW format.
	User should not access this field. */
	uint32_t reserved3;
};
#pragma pack(pop)


/**************************************************************************//**
@Description	Create Table Parameters
*//***************************************************************************/
struct ctlu_table_create_params {
	/** Committed Number Of Rules
	The table committed number of rules, at any point in time the table can
	contain at least this number of rules.*/
	uint32_t committed_rules;

	/** Max Number Of Rules
	The max number of rules this table can contain. This number is not
	guaranteed in contrast to committed_rules. Meaning, trying to add a
	rule to a table that already contains committed_rules might fail. */
	uint32_t max_rules;

	/** Miss Result
	A default rule that is chosen when no match is found. */
	struct ctlu_table_rule_result  miss_result;

	/** Table Attributes
	Please refer to \link FSL_CTLU_TABLE_ATTRIBUTES Table Attributes macros
	\endlink for more details. */
	uint16_t attributes;

	/** Table Key Size
	In a case of LPM table:
	 - Should be set to 0x08 for IPv4
	 - Should be set to 0x14 for IPv6
	Please note that this value is not returned through
	\ref ctlu_table_get_params() function. */
	uint8_t  key_size;
};


/**************************************************************************//**
@Description	Get Table Parameters Output
*//***************************************************************************/
struct ctlu_table_get_params_output {
	/** Table's current number of rules */
	uint32_t current_rules;

	/** Committed Number Of Rules
	The table committed number of rules, at any point in time the table can
	contain at least this number of rules.*/
	uint32_t committed_rules;

	/** Max Number Of Rules
	The max number of rules this table can contain. This number is not
	guaranteed in contrast to committed_rules. Meaning, trying to add a
	rule to a table that already contains committed_rules might fail. */
	uint32_t max_rules;

	/** Table Attributes
	Please refer to \link FSL_CTLU_TABLE_ATTRIBUTES Table Attributes macros
	\endlink for more details. */
	uint16_t attributes;
};


/**************************************************************************//**
@Description	Key Composition Rule (kcr) builder
*//***************************************************************************/
struct	ctlu_kcr_builder{
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
struct	ctlu_kcr_builder_fec_single_mask{
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
struct	ctlu_kcr_builder_fec_mask{
	/** An array of up to 4 pairs of bit-wise masks and offsets.
	Masks are applied to the extracted header at the corresponding offset
	from its beginning */
	struct ctlu_kcr_builder_fec_single_mask single_mask[4];

	/** Number of masks (1-4) */
	uint8_t	num_of_masks;
};

/** @} */ /* end of FSL_CTLU_STRUCTS */


/**************************************************************************//**
@Group		FSL_CTLU_Functions CTLU Functions

@Description	Freescale AIOP CTLU Functions

@{
*//***************************************************************************/

/* ######################################################################### */
/* ########################### Table Operations ############################ */
/* ######################################################################### */

/**************************************************************************//**
@Function	ctlu_table_create

@Description	Creates a new table.

@Param[in]	tbl_params - The table parameters.

@Param[out]	table_id - Table ID. A unique table identification number to be
		used for future table references.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_create(struct ctlu_table_create_params *tbl_params,
			  uint16_t *table_id);


/**************************************************************************//**
@Function	ctlu_table_update_miss_result

@Description	Updates specific table miss result.

@Param[in]	table_id - Table ID.

@Param[in]	miss_result - A default rule that is chosen when no match is
		found.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	This function should only be called if the table was defined
		with a miss entry (i.e. CTLU_TBL_ATTRIBUTE_MR_MISS was set in
		table attributes).
		In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_update_miss_result(uint16_t table_id,
				      struct ctlu_table_rule_result
					     *miss_result,
				      uint32_t flags);


/**************************************************************************//**
@Function	ctlu_table_get_params

@Description	A getter for the table parameters.
		This function does not return the table miss result.

@Param[in]	table_id - Table ID.
@Param[out]	tbl_params - Table parameters.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_get_params(uint16_t table_id,
			      struct ctlu_table_get_params_output *tbl_params);


/**************************************************************************//**
@Function	table_get_miss_result

@Description	A getter for the table miss result.

		If miss result is of CTLU_RULE_RESULT_TYPE_REFERENCE
		type, user should decrement the application context reference
		count after usage.

@Param[in]	table_id - Table ID.
@Param[out]	miss_result - A default rule data that is chosen when no match
		is found.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_get_miss_result(uint16_t table_id,
				   struct ctlu_table_rule_result *miss_result);


/**************************************************************************//**
@Function	ctlu_table_delete

@Description	Deletes a specified table.

		After a table is deleted, all reference counters related to the
		application contexts pointed by the table results will be
		decremented.

@Param[in]	table_id - Table ID.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_delete(uint16_t table_id);


/* ######################################################################### */
/* ########################### Rule Operations ############################# */
/* ######################################################################### */

/**************************************************************************//**
@Function	ctlu_table_rule_create

@Description	Adds a rule to a specified table.

		If the rule key already exists, the rule will not be added and
		a status will be returned.

@Param[in]	table_id - Table ID.
@Param[in]	rule - The rule to be added.
@Param[in]	key_size - Key size.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_rule_create(uint16_t table_id, struct ctlu_table_rule *rule,
			       uint8_t key_size);


/**************************************************************************//**
@Function	ctlu_table_rule_create_or_replace

@Description	Adds/replaces a rule to a specified table.

		If the rule key already exists, the rule will be replaced by
		the one specified in the function's parameters. Else, a new
		rule will be created in the table.

@Param[in]	table_id - Table ID.
@Param[in]	rule - The rule to be added.
@Param[in]	key_size - Key size.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_rule_create_or_replace(uint16_t table_id,
					  struct ctlu_table_rule *rule,
					  uint8_t key_size,
					  uint32_t flags);
//TODO add flags documentation

/**************************************************************************//**
@Function	aiop_ctlu_table_replace_rule

@Description	Replaces a specified rule in the table.

		The rule's key is not modifiable. Caller to this function
		supplies the key of the rule to be replaced.

@Param[in]	table_id - Table ID.
@Param[in]	rule - Table rule, contains the rule's key, with which the
		rule to be replaced will be found and contain the rule
		result to be replaced.
@Param[in]	key_size - Key size.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	The key must be the exact same key that was used for the rule
		creation.
		In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_rule_replace(uint16_t table_id,
				struct ctlu_table_rule *rule,
				uint8_t key_size,
				uint32_t flags);
//TODO add flags documentation


/**************************************************************************//**
@Function	ctlu_table_rule_query

@Description	Queries a rule in the table.

@Param[in]	table_id - Table ID.
@Param[in]	key - Key of the rule to be queried.
@Param[in]	key_size - Key size in bytes.
@Param[out]	result - The result of the query.
@Param[out]	timestamp - Timestamp of the result. Timestamp is not valid
		unless the rule queried for was created with suitable options
		(Please refer to \ref FSL_CTLU_TABLE_RULE_OPTIONS for more
		details).

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	NOTE: If the result is of type that contains pointer to CDMA
		application buffer (refer to struct ctlu_table_rule_result
		documentation) this function will not increment the reference
		counter of the buffer. For query functions that does increment
		the reference counter please refer to table lookup function.
		In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_rule_query(uint16_t table_id, union ctlu_key *key,
			      uint8_t key_size, struct ctlu_table_rule_result
			      *result, uint32_t *timestamp);


/**************************************************************************//**
@Function	table_delete_rule

@Description	Deletes a specified rule in the table.

@Param[in]	table_id - Table ID.
@Param[in]	key - Key of the rule to be deleted.
@Param[in]	key_size - Key size.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	The key must be the exact same key that was used for the rule
		creation.
		In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_rule_delete(uint16_t table_id, union ctlu_key *key,
			       uint8_t key_size, uint32_t flags);
//TODO add flags documentation

/* ######################################################################### */
/* ############################# Table Lookups ############################# */
/* ######################################################################### */


/**************************************************************************//**
@Function	ctlu_table_lookup_by_keyid

@Description	Performs a lookup with a predefined key.

		If Opaque0 result field is an application context pointer, user
		should decrement the application context reference count after
		usage.

		Implicit input parameters in Task Defaults: Segment Address,
		Segment Size and Parse Results.

@Param[in]	table_id - Table ID.
@Param[in]	keyid - A key ID for the table lookups (key ID specifies
		how to build a key).
@Param[out]	lookup_result - Points to a user preallocated memory to which
		the table lookup result will be written.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_table_lookup_by_keyid(uint16_t table_id, uint8_t keyid,
				struct ctlu_lookup_result *lookup_result);


/**************************************************************************//**
@Function	ctlu_table_lookup_by_key

@Description	Performs a lookup with a key built by the user.

		If Opaque0 result field is an application context pointer, user
		should decrement the application context reference count after
		usage.

@Param[in]	table_id - Table ID.
@Param[in]	key - Lookup key.
@Param[in]	key_size - Key size.
@Param[out]	lookup_result - Points to a user preallocated memory to which
		the table lookup result will be written.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
		This lookup cannot be used for chaining of lookups.
*//***************************************************************************/
int32_t ctlu_table_lookup_by_key(uint16_t table_id,
				 union ctlu_key *key,
				 uint8_t key_size,
				 struct ctlu_lookup_result *lookup_result);


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
int32_t ctlu_kcr_create(uint8_t *kcr, uint8_t *keyid);


/**************************************************************************//**
@Function	ctlu_kcr_replace

@Description	Replaces key composition rule of a specified keyid.

@Param[in]	kcr - Key composition rule.
@Param[in]	keyid - Key ID.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR_REPLACE

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_kcr_replace(uint8_t *kcr, uint8_t keyid);


/**************************************************************************//**
@Function	ctlu_kcr_delete

@Description	Deletes key composition rule.

@Param[in]	keyid - Key ID.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR_DELETE

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_kcr_delete(uint8_t keyid);


/**************************************************************************//**
@Function	ctlu_kcr_query

@Description	Returns the key composition rule of a given key ID.

@Param[in]	keyid - The key ID.
@Param[out]	kcr - Key composition rule.
@Param[out]	size - Size of the key that the kcr creates.

@Return		Please refer to \ref FSL_CTLU_STATUS_KCR_QUERY

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_kcr_query(uint8_t keyid, uint8_t *kcr, uint8_t *size);


/**************************************************************************//**
@Function	ctlu_gen_key

@Description	Extracts a key from a frame and returns it.

@Param[in]	keyid - The key ID to be used for the key extraction.
@Param[out]	key - The key.
@Param[out]	key_size - Key size.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_gen_key(uint8_t keyid, union ctlu_key *key, uint8_t *key_size);


/**************************************************************************//**
@Function	ctlu_gen_hash

@Description	Generates a hash value from a given key.

@Param[in]	key - The key to generate hash from.
@Param[in]	key_size - Key size.
@Param[out]	hash - The hash result.

@Return		Please refer to \ref FSL_CTLU_STATUS_GENERAL

@Cautions	In this function the task yields.
*//***************************************************************************/
int32_t ctlu_gen_hash(union ctlu_key *key, uint8_t key_size, uint32_t *hash);

/** @} */ /* end of FSL_CTLU_Functions */
/** @} */ /* end of FSL_CTLU */
/** @} */ /* end of ACCEL */


#endif /* __FSL_CTLU_H */
