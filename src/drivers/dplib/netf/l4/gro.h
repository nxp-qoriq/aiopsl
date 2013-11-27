/**************************************************************************//**
 @File          gro.h

 @Description   This file contains the AIOP SW internal TCP GRO API
*//***************************************************************************/
#ifndef __GRO_H
#define __GRO_H

#include "dplib/fsl_gro.h"
#include "dplib/fsl_fdma.h"


/**************************************************************************//**
@Group		TCP_GRO_INTERNAL_STRUCTS TCP GRO Internal Structures

@Description	AIOP TCP GRO Internal Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	TCP GRO Internal Context.
*//***************************************************************************/
/* Currently the struct size is 116 bytes.
 * The fixed definition size is 128 bytes.
 * In case additional fields are added to the struct we can either:
 * Modify the fixed size
 * OR
 * Remove un-needed fields such as the limits (we get them every time)
 * and the timeout_flags and tmi_id of the timeout parameters (16 bytes
 * of un-needed bytes currently)*/

struct tcp_gro_context {
		/** Aggregation parameters  */
	struct tcp_gro_context_params params;
		/** Aggregated packet FD  */
	struct ldpaa_fd agg_fd;
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
		/** Aggregated packet isolation attributes.
		 * Todo - in case all the segments have the same isolation
		 * attributes there is no need to save this structure since the
		 * attributes can be taken from the new segment Additional
		 * Dequeue Context area. */
	struct fdma_isolation_attributes agg_fd_isolation_attributes;
		/** TMAN Instance ID. */
	uint32_t timer_handle;
		/** Padding */
	uint8_t	pad[4];
};


/**************************************************************************//**
@Description	GRO Global parameters
*//***************************************************************************/

struct gro_global_parameters {
		/** GRO Timeout flags \ref GRO_INTERNAL_TIMEOUT_FLAGS. */
	uint32_t timeout_flags;
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
#define SIZEOF_GRO_CONTEXT	(sizeof(struct tcp_gro_context))
	/* GRO internal struct size assertion check */
#pragma warning_errors on
ASSERT_STRUCT_SIZE(SIZEOF_GRO_CONTEXT, TCP_GRO_CONTEXT_SIZE);
#pragma warning_errors off

/** @} */ /* end of TCP_GRO_GENERAL_INT_DEFINITIONS */


/**************************************************************************//**
@Group		GRO_INTERNAL_TIMEOUT_FLAGS GRO Internal Timeout Flags

@Description GRO Timeout Flags.


| 0-9 |     10-11    | 12 |   13 - 15   | 16 - 31  |
|-----|--------------|----|-------------|----------|
|     |AIOP_priority |TPRI| Granularity |          |


Recommended default values: Granularity:GRO_MODE_100_USEC_TO_GRANULARITY
			    TPRI : not set (low priority)
			    AIOP task priority: low
@{
*//***************************************************************************/


/* The following defines will be used to set the timeout timer tick size.*/
/**< 1 uSec timeout timer ticks*/
#define GRO_MODE_USEC_TO_GRANULARITY		0x00000000
/**< 10 uSec timeout timer ticks*/
#define GRO_MODE_10_USEC_TO_GRANULARITY		0x00010000
/**< 100 uSec timeout timer ticks*/
#define GRO_MODE_100_USEC_TO_GRANULARITY	0x00020000
/**< 1 mSec timeout timer ticks*/
#define GRO_MODE_MSEC_TO_GRANULARITY		0x00030000
/**< 10 mSec timeout timer ticks*/
#define GRO_MODE_10_MSEC_TO_GRANULARITY		0x00040000
/**< 100 mSec timeout timer ticks*/
#define GRO_MODE_100_MSEC_TO_GRANULARITY	0x00050000
/**< 1 Sec timeout timer ticks*/
#define GRO_MODE_SEC_TO_GRANULARITY		0x00060000

/**< If set, timeout priority task is high. */
#define GRO_MODE_TPRI				0x00080000

/* The following defines will be used to set the AIOP task priority
 * of the created timeout task.*/
/**< Low priority AIOP task*/
#define GRO_MODE_LOW_PRIORITY_TASK		0x00000000
/**< Middle priority AIOP task*/
#define GRO_MODE_MID_PRIORITY_TASK		0x00100000
/**< High priority AIOP task*/
#define GRO_MODE_HIGH_PRIORITY_TASK		0x00200000

/* @} end of group GRO_INTERNAL_TIMEOUT_FLAGS */

/**************************************************************************//**
 @Group	TCP_GRO_INTERNAL_FLAGS TCP GRO Internal Flags

 @Description TCP GRO Internal Flags.

 @{
*//***************************************************************************/

	/** If set, TCP header of the GRO aggregation has timestamp.	*/
#define TCP_GRO_HAS_TIMESTAMP		0x00000001
	/** If set, TCP GRO is in exclusive mode. Otherwise, TCP GRO is in
	 * concurrent mode.	*/
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


/**************************************************************************//**
@Group		TCP_GRO_INTERNAL_FUNCTIONS TCP GRO Internal Functions

@Description	TCP GRO Internal Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	gro_init

@Description	Initialize the GRO
		infrastructure.

		This function should be called once.

		Any other GRO function cannot be called before this function is
		called.

@Param[in]	timeout_flags - GRO Timeout flags
		\ref GRO_INTERNAL_TIMEOUT_FLAGS.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void gro_init(uint32_t timeout_flags);


/** @} */ /* end of TCP_GRO_INTERNAL_FUNCTIONS */


/** @} */ /* end of AIOP_TCP_GRO_INTERNAL */





#endif /* __GRO_H */
