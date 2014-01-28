/**************************************************************************//**
 @File          ipf.h

 @Description   This file contains the AIOP SW IP Fragmentation Internal API
*//***************************************************************************/
#ifndef __IPF_H
#define __IPF_H

#include "dplib/fsl_ipf.h"

/**************************************************************************//**
@Group		AIOP_IPF_INTERNAL  AIOP IPF Internal

@Description	AIOP IPF Internal

@{
*//***************************************************************************/


/**************************************************************************//**
@Group		IPF_INTERNAL_STRUCTS IPF Internal Structures

@Description	AIOP IPF Internal Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPF parameters used by IPF functions for split by MTU.
*//***************************************************************************/
struct ipf_mtu_params {
	/** Maximum Transmit Unit
	 * In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
	 * parameter is ignored. */
	uint16_t mtu;
	/** MTU payload length */
	uint16_t mtu_payload_length;
	/** Split size */
	uint16_t split_size;
	/** Previous Fragment Offset */
	uint16_t prev_frag_offset;
};

/**************************************************************************//**
@Description	IPF union parameters used by IPF functions.
*//***************************************************************************/
union ipf_mtu_sf_params {
	struct ipf_mtu_params mtu_params;
	uint16_t frag_header_offset[4];
};


/**************************************************************************//**
@Description	IPF Context Internally used by IPF functions.
*//***************************************************************************/
struct ipf_context {
	 /** Remaining frame's FD  */
	struct ldpaa_fd rem_fd;
	/** Union of ipf_mtu_params and ipf_sf_params */
	union ipf_mtu_sf_params ipf_params ;
	/** Frame ID. Used for IPv6 fragmentation extension in case of
	 * fragmentation according to MTU.
	 * In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
	 * parameter is ignored and the ID is inherited from the original
	 * fragments. */
	uint32_t frame_id;
	/** Flags - Please refer to \ref IPF_Flags */
	uint32_t flags;
	/** Fragment index (for SF mode) */
	uint16_t frag_index;
	/** Original Starting HXS for Parser from Task default */
	uint16_t parser_starting_hxs;
	/** Original segment address from PRC */
	uint16_t prc_seg_address;
	/** Original segment length from PRC */
	uint16_t prc_seg_length;
	/** Remaining payload length */
	uint16_t remaining_payload_length;
	/** Original Parser Profile ID from Task default */
	uint8_t parser_profile_id;
	/** Remaining frame handle*/
	uint8_t rem_frame_handle;
	/** First fragment indication */
	uint8_t	first_frag;
	/** IPv4 indication */
	uint8_t ipv4;
	/* Padding */
	uint8_t	pad[2];
};

/**************************************************************************//**
@Description	Parameters saved in a buffer for original fragments restoration.
*//***************************************************************************/
struct params_for_restoration {
	uint64_t ipv6_fragment_header;
	uint16_t frag_header_offsets[64];
	uint16_t ipv6_first_frag_payload_length;
	uint8_t pad[6];
};

/**************************************************************************//**
@Description	IPv6 Fragment Header.
*//***************************************************************************/
struct ipv6_fragment_header {
	uint8_t  next_header;
	uint8_t  reserved;
	uint16_t fragment_offset_flags;
	uint32_t id;
};

/** @} */ /* end of IPF_INTERNAL_STRUCTS */


/**************************************************************************//**
@Group	IPF_INTERNAL_MACROS IPF Internal Macros

@Description	IPF Internal Macros

@{
*//***************************************************************************/
/**************************************************************************//**
 @Group	IPF_DEFINES IPF Defines

 @Description IPF Defines.

 @{
*//***************************************************************************/
/** Size of IPv6 Fragment header */
#define IPV6_FRAGMENT_HEADER_LENGTH  8
/** Maximum number of restored fragments */
/*#define MAX_NUM_OF_FRAGMENTS 64 */
/** Mask for IPv6 Fragment Header M flag */
#define IPV6_HDR_M_FLAG_MASK 0x0001

/** @} */ /* end of IPF_DEFINES */

/**************************************************************************//**
 @Group	IPF_GENERAL_INT_DEFINITIONS IPF General Internal Definitions

 @Description IPF General Internal Definitions.

 @{
*//***************************************************************************/

	/** Size of IPF Context. */
#define SIZEOF_IPF_CONTEXT	(sizeof(struct ipf_context))

#pragma warning_errors on
/** IPF internal struct size assertion check. */
ASSERT_STRUCT_SIZE(SIZEOF_IPF_CONTEXT, IPF_CONTEXT_SIZE);
#pragma warning_errors off

/** @} */ /* end of IPF_GENERAL_INT_DEFINITIONS */

/** @} */ /* end of IPF_INTERNAL_MACROS */


inline int32_t ipf_restore_orig_fragment(struct ipf_context *ipf_ctx);
inline int32_t ipf_after_split_ipv6_fragment(struct ipf_context *ipf_ctx);
inline int32_t ipf_after_split_ipv4_fragment(struct ipf_context *ipf_ctx);
int32_t ipf_split_fragment(struct ipf_context *ipf_ctx);
int32_t ipf_move_remaining_frame(struct ipf_context *ipf_ctx);
int32_t ipf_insert_ipv6_frag_header(struct ipf_context *ipf_ctx,
		uint16_t frag_hdr_offset);

/** @} */ /* end of AIOP_IPF_INTERNAL */



#endif /* __GSO_H */
