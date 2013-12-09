/**************************************************************************//**
 @File          gso.h

 @Description   This file contains the AIOP SW TCP GSO Internal API
*//***************************************************************************/
#ifndef __GSO_H
#define __GSO_H

#include "dplib/fsl_gso.h"


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
	/** Maximum Segment Size. */
	uint16_t mss;
	/** Urgent Pointer offset. */
	uint16_t urgent_pointer;
	/** Remaining frame handle. */
	uint8_t	rem_frame_handle;
		/** First Segment indication */
	uint8_t	first_seg;
	/** Padding */
	uint8_t	pad[6];
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

/** @} */ /* end of TCP_GSO_INTERNAL_MACROS */

/** @} */ /* end of FSL_AIOP_TCP_GSO_INTERNAL */

/** @} */ /* end of FSL_AIOP_GSO */

#endif /* __GSO_H */
