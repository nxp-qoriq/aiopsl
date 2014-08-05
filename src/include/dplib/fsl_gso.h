/**************************************************************************//**
 @File          fsl_gso.h

 @Description   This file contains the AIOP SW TCP GSO API
*//***************************************************************************/
#ifndef __FSL_GSO_H
#define __FSL_GSO_H

#include "common/types.h"
#include "dplib/fsl_ldpaa.h"

/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		FSL_AIOP_GSO GSO

@Description	FSL_AIOP_GSO

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	GSO_MACROS GSO Macros

@Description	GSO Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GSO_GENERAL_DEFINITIONS TCP GSO General Definitions

 @Description TCP GSO General Definitions.

 @{
*//***************************************************************************/

	/** TCP GSO context size definition. */
#define TCP_GSO_CONTEXT_SIZE	64
	/** TCP GSO context definition.
	 * Must be aligned to 32 bytes. */
typedef uint8_t tcp_gso_ctx_t[TCP_GSO_CONTEXT_SIZE]
		__attribute__((aligned(sizeof(struct ldpaa_fd))));


/** @} */ /* end of TCP_GSO_GENERAL_DEFINITIONS */


/**************************************************************************//**
 @Group	TCP_GSO_FLAGS TCP GSO Flags

 @Description Flags for tcp_gso_context_init() function.

 @{
*//***************************************************************************/

	/** GSO no flags indication. */
#define TCP_GSO_NO_FLAGS		0x00000000

/** @} */ /* end of TCP_GSO_FLAGS */

/**************************************************************************//**
@Group	TCP_GSO_GENERATE_SEG_STATUS  TCP GSO Statuses

@Description tcp_gso_generate_seg() return values

@{
*//***************************************************************************/

	/** Segmentation process complete. The last segment was generated. */
#define	TCP_GSO_GEN_SEG_STATUS_DONE					\
						SUCCESS
	/** Segmentation process did not complete.
	 * Segment was generated and the user should call
	 * gso_generate_tcp_seg() again to generate another segment */
#define	TCP_GSO_GEN_SEG_STATUS_IN_PROCESS				\
						(TCP_GSO_MODULE_STATUS_ID | 0x1)
	/** Segmentation process cannot start since the syn/rst flags are set.*/
#define	TCP_GSO_GEN_SEG_STATUS_SYN_RST_SET				\
						(TCP_GSO_MODULE_STATUS_ID | 0x2)

/** @} */ /* end of TCP_GSO_GENERATE_SEG_STATUS */

/** @} */ /* end of GSO_MACROS */


/**************************************************************************//**
@Group		GSO_Functions GSO Functions

@Description	GSO Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	tcp_gso_generate_seg

@Description	This function generates a single TCP segment and locates it in
		the default frame location in the workspace.

		Pre-condition - In the first iteration this function is called
		for a source packet, the source packet should be located
		at the default frame location in workspace.

		The remaining source frame is kept in the internal GSO
		structure.

		This function should be called repeatedly
		until the returned status indicates segmentation is completed
		(\ref TCP_GSO_GEN_SEG_STATUS_DONE).

@Param[in]	tcp_gso_context_addr - Address to the TCP GSO internal context.
		Must be initialized by gso_context_init() prior to the first
		call. Must be aligned to Frame Descriptor size.

@Return		GSO Status (\ref TCP_GSO_GENERATE_SEG_STATUS), or
		negative value on error.

@Retval		EBADFD - Received packet FD contain errors (FD.err != 0).
		Recommendation is to either force discard of the frame (call
		\ref fdma_force_discard_frame) or enqueue the frame.
		The packet was not segmented.
@Retval		ENOMEM - Received packet cannot be stored due to buffer pool
		depletion. Recommendation is to discard the frame.
		The packet was not segmented.

@Cautions	None.
*//***************************************************************************/
int tcp_gso_generate_seg(
		tcp_gso_ctx_t tcp_gso_context_addr);

/**************************************************************************//**
@Function	tcp_gso_discard_frame_remainder

@Description	This function discard the remainder packet being segmented in
		case the user decides to stop the segmentation process before
		its completion (before a \ref TCP_GSO_GEN_SEG_STATUS_DONE status
		is returned).

@Param[in]	tcp_gso_context_addr - Address to the TCP GSO internal context.
		Must be aligned to Frame Descriptor size.

@Return	0 - Success

@Cautions	Following this function no packet resides in the default frame
		location in the task defaults.
		This function should only be called after \ref
		TCP_GSO_GEN_SEG_STATUS_IN_PROCESS status is returned from
		gso_generate_tcp_seg() function call.
*//***************************************************************************/
int tcp_gso_discard_frame_remainder(
		tcp_gso_ctx_t tcp_gso_context_addr);

/**************************************************************************//**
@Function	tcp_gso_context_init

@Description	This function initializes the GSO context structure that is
		used for the TCP GSO process of the packet.

		This function must be called once before each new packet
		segmentation process.

@Param[in]	flags - Please refer to \ref TCP_GSO_FLAGS.
@Param[in]	mss - Maximum Segment Size.
@Param[out]	tcp_gso_context_addr - Address to the TCP GSO internal context
		structure located in the workspace by the user. Internally used
		by TCP GSO functions.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void tcp_gso_context_init(
		uint32_t flags,
		uint16_t mss,
		tcp_gso_ctx_t tcp_gso_context_addr);

/** @} */ /* end of GSO_Functions */
/** @} */ /* end of FSL_AIOP_GSO */
/** @} */ /* end of NETF */


#endif /* __FSL_GSO_H */
