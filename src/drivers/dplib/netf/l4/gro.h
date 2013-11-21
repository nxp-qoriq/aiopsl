/**************************************************************************//**
 @File          gro.h

 @Description   This file contains the AIOP SW internal TCP GRO API
*//***************************************************************************/
#ifndef __GRO_H
#define __GRO_H

#include "fsl_gro.h"


/**************************************************************************//**
@Group		TCP_GRO_INTERNAL_STRUCTS TCP GRO Internal Structures

@Description	AIOP TCP GRO Internal Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GRO Internal Context.
*//***************************************************************************/
struct gro_context {
		/** Aggregated packet FD  */
	struct ldpaa_fd agg_fd;
		/** Aggregation parameters  */
	struct gro_context_params params;
		/** Next expected sequence number. */
	uint32_t next_seq;
		/** Last received acknowledgment number. */
	uint32_t last_ack;
		/** Aggregated packet timestamp value. */
	uint32_t timestamp;
		/** Internal TCP GRO flags */
	uint32_t internal_flags;
		/** TCP GRO aggregation flags */
	uint32_t flags;
		/** Padding */
	uint8_t	pad[4];
};

/** @} */ /* end of TCP_GRO_INTERNAL_STRUCTS */


/**************************************************************************//**
@Group		AIOP_TCP_GRO_INTERNAL AIOP TCP GRO Internal

@Description	AIOP TCP GRO Internal

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	TCP_GRO_INTERNAL_MACROS TCP GRO Internal Macros

@Description	TCP GRO Internal Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GRO_GENERAL_INT_DEFINITIONS TCP GRO General Internal Definitions

 @Description TCP GRO General Internal Definitions.

 @{
*//***************************************************************************/

	/** Size of GRO Context. */
#define SIZEOF_GRO_CONTEXT 	(sizeof(struct gro_context))
	/* GRO internal struct size assertion check */
#pragma warning_errors on
ASSERT_STRUCT_SIZE(SIZEOF_GRO_CONTEXT, GRO_CONTEXT_SIZE);
#pragma warning_errors off

/** @} */ /* end of TCP_GRO_GENERAL_INT_DEFINITIONS */

/**************************************************************************//**
 @Group	TCP_GRO_INTERNAL_FLAGS TCP GRO Internal Flags

 @Description TCP GRO Internal Flags.

 @{
*//***************************************************************************/

	/** If set, TCP header of the GRO aggregation has timestamp. 	*/
#define TCP_GRO_HAS_TIMESTAMP		0x00000001
	/** If set, TCP GRO is in exclusive mode. Otherwise, TCP GRO is in
	 * concurrent mode. 	*/
#define TCP_GRO_OSM_EXLUSIVE_MODE	0x00000002
	/** IP header reserved1 ECN bit of the GRO aggregation. */
#define TCP_GRO_ECN1			0x00010000
	/** IP header reserved2 ECN bit of the GRO aggregation. */
#define TCP_GRO_ECN2			0x00020000

/** @} */ /* end of TCP_GRO_INTERNAL_FLAGS */

/**************************************************************************//**
 @Group	TCP_GRO_INTERNAL_MASKS TCP GRO Internal Masks

 @Description TCP GRO Internal Masks.

 @{
*//***************************************************************************/

/** ECN mask value of the IP header of the GRO aggregation.
 * The Mask should be used on the GRO internal flags \ref TCP_GRO_INTERNAL_FLAGS
 * in order to get the ECN value of the first segment. */
#define TCP_GRO_ECN_MASK	0x00030000

/** @} */ /* end of TCP_GRO_INTERNAL_MASKS */

/** @} */ /* end of TCP_GRO_INTERNAL_MACROS */




/** @} */ /* end of AIOP_TCP_GRO_INTERNAL */





#endif /* __GRO_H */
