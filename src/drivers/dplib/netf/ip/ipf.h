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
@Description	IPF Context Internally used by IPF functions.
*//***************************************************************************/
struct ipf_context {
	 /** Remaining frame's FD  */
	struct ldpaa_fd rem_fd;
	/** Frame ID. Used for IPv6 fragmentation extension in case of
	 * fragmentation according to MTU.
	 * In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
	 * parameter is ignored and the ID is inherited from the original
	 * fragments. */
	uint32_t frame_id;
	/** Flags - Please refer to \ref IPF_Flags */
	uint32_t flags;
	/** Maximum Transmit Unit
	 * In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
	 * parameter is ignored. */
	uint16_t mtu;
	/** MTU payload length */
	uint16_t mtu_payload_length; 
	/** Split size */
	uint16_t split_size;
	/** Fragment index (for SF mode) */
	uint16_t frag_index;
	/** Fragment Header Offset array (for SF mode) */
	uint16_t frag_header_offset[16];
	/** Previous Fragment Offset */
	uint16_t prev_frag_offset;
	/** Remaining frame handle*/
	uint8_t rem_frame_handle;
	/** First fragment indication */
	uint8_t	first_frag;
};

/** @} */ /* end of IPF_INTERNAL_STRUCTS */


/**************************************************************************//**
@Group	IPF_INTERNAL_MACROS IPF Internal Macros

@Description	IPF Internal Macros

@{
*//***************************************************************************/

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

/** @} */ /* end of AIOP_IPF_INTERNAL */



#endif /* __GSO_H */
