/**************************************************************************//**
 @File          gso.h

 @Description   This file contains the AIOP SW TCP GSO Internal API
*//***************************************************************************/
#ifndef __GSO_H
#define __GSO_H

#include "dplib/fsl_gso.h"
#include "dplib/fsl_ldpaa.h"


/** \addtogroup FSL_AIOP_GSO
 *  @{
 */

/**************************************************************************//**
@Group		FSL_AIOP_TCP_GSO_INTERNAL FSL AIOP TCP GSO Internal

@Description	FSL AIOP TCP GSO Internal

@{
*//***************************************************************************/


/**************************************************************************//**
@Group		TCP_GSO_INTERNAL_STRUCTS TCP GSO Internal Structures

@Description	AIOP TCP GSO Internal Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GSO Context Internally used by GSO functions.
*//***************************************************************************/
struct tcp_gso_context {
	/** Remaining packet FD  */
	struct ldpaa_fd rem_fd;
	/** Flags - Please refer to \ref TCP_GSO_FLAGS */
	uint32_t flags;
	/** Internal TCP GSO flags - Please refer to 
	 * \ref TCP_GSO_INTERNAL_FLAGS */
	uint32_t internal_flags;
	/** Split Size. */
	uint16_t split_size;
	/** Headers Size. */
	uint16_t headers_size;
	/** Urgent Pointer offset. */
	uint16_t urgent_pointer;
	/** Remaining frame handle. */
	uint8_t	rem_frame_handle;
	/** First Segment indication */
	uint8_t	first_seg;
	/** Padding */
	uint8_t	pad[4];
};

/** @} */ /* end of TCP_GSO_INTERNAL_STRUCTS */


/**************************************************************************//**
@Group	TCP_GSO_INTERNAL_MACROS TCP GSO Internal Macros

@Description	TCP GSO Internal Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GSO_GENERAL_INT_DEFINITIONS TCP GSO General Internal Definitions

 @Description TCP GSO General Internal Definitions.

 @{
*//***************************************************************************/

	/** Size of GSO Context. */
#define SIZEOF_GSO_CONTEXT	(sizeof(struct tcp_gso_context))
	/* GSO internal struct size assertion check */
#pragma warning_errors on
ASSERT_STRUCT_SIZE(SIZEOF_GSO_CONTEXT, TCP_GSO_CONTEXT_SIZE);
#pragma warning_errors off

/** @} */ /* end of TCP_GSO_GENERAL_INT_DEFINITIONS */

/**************************************************************************//**
 @Group	TCP_GSO_INTERNAL_FLAGS TCP GSO Internal Flags

 @Description TCP GSO Internal Flags.

 @{
*//***************************************************************************/

	/** If set, the FIN flag in the TCP header of the GSO aggregation is set. */
#define TCP_GSO_FIN_BIT		0x00000001
	/** If set, the PSH flag in the TCP header of the GSO aggregation is set. */
#define TCP_GSO_PSH_BIT		0x00000008	

/** @} */ /* end of TCP_GSO_INTERNAL_FLAGS */

/** @} */ /* end of TCP_GSO_INTERNAL_MACROS */

/** @} */ /* end of FSL_AIOP_TCP_GSO_INTERNAL */

/** @} */ /* end of FSL_AIOP_GSO */

#endif /* __GSO_H */
