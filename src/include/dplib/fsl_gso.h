/**************************************************************************//**
 @File          fsl_gso.h

 @Description   This file contains the AIOP SW TCP GSO API
*//***************************************************************************/
#ifndef __FSL_GSO_H
#define __FSL_GSO_H

#include "general.h"

/**************************************************************************//**
@Group		FSL_AIOP_TCP_GSO FSL AIOP TCP GSO

@Description	FSL AIOP TCP GSO

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	TCP_GSO_MACROS TCP GSO Macros

@Description	TCP GSO Macros

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group	TCP_GSO_GENERAL_DEFINITIONS TCP GSO General Definitions

 @Description TCP GSO General Definitions.

 @{
*//***************************************************************************/

	/** GSO context size definition. */
#define GSO_CONTEXT_SIZE	128
	/** GSO context definition. */
typedef uint8_t gso_ctx_t[GSO_CONTEXT_SIZE];


/** @} */ /* end of TCP_GSO_GENERAL_DEFINITIONS */


/**************************************************************************//**
 @Group	TCP_GSO_FLAGS TCP GSO Flags

 @Description Flags for gso_context_init() function.

 @{
*//***************************************************************************/

	/** GSO no flags indication. */
#define TCP_GSO_NO_FLAGS		0x00000000

/** @} */ /* end of TCP_GSO_FLAGS */

/**************************************************************************//**
@Group	TCP_GSO_GENERATE_SEG_STATUS  TCP GSO Statuses

@Description gso_generate_tcp_seg() return values

@{
*//***************************************************************************/

	/** Segmentation process complete. The last segment was generated. */
#define	GSO_GEN_TCP_SEG_STATUS_DONE	SUCCESS
	/** Segmentation process did not complete.
	 * Segment was generated and the user should call
	 * gso_generate_tcp_seg() again to generate another segment */
#define	GSO_GEN_TCP_SEG_STATUS_IN_PROCESS (TCP_GSO_MODULE_STATUS_ID | 0x1)

/** @} */ /* end of TCP_GSO_GENERATE_SEG_STATUS */

/** @} */ /* end of TCP_GSO_MACROS */


/**************************************************************************//**
@Group		TCP_GSO_Functions TCP GSO Functions

@Description	TCP GSO Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	gso_generate_tcp_seg

@Description	This function generates a single TCP segment and locates it in
		the default frame location in the workspace.

		Pre-condition - In the first iteration this function is called
		for a source packet, the source packet should be located
		at the default frame location in workspace.

		The remaining source frame is kept in the internal GSO
		structure.

		This function should be called repeatedly
		until the returned status indicates segmentation is completed
		(\ref GSO_GEN_TCP_SEG_STATUS_DONE).

@Param[in]	gso_ctx_addr - Address to the TCP GSO internal context. Must be
		initialized by gso_context_init() prior to the first call.

@Return		Status, please refer to \ref TCP_GSO_GENERATE_SEG_STATUS or
		\ref fdma_hw_errors or \ref fdma_sw_errors for more details.

@Cautions	None.
*//***************************************************************************/
int32_t gso_generate_tcp_seg(
		gso_ctx_t gso_ctx_addr);

/**************************************************************************//**
@Function	gso_discard_frame_remainder

@Description	This function discard the remainder packet being segmented in
		case the user decides to stop the segmentation process before
		its completion (before a \ref GSO_GEN_TCP_SEG_STATUS_DONE status
		is returned).

@Param[in]	gso_ctx_addr - Address to the TCP GSO internal context.

@Return		Status of the operation (\ref FDMA_DISCARD_FRAME_ERRORS,
		\ref fdma_hw_errors, \ref fdma_sw_errors).

@Cautions	Following this function no packet resides in the default frame
		location in the task defaults.
		This function should only be called after \ref
		GSO_GEN_TCP_SEG_STATUS_IN_PROCESS status is returned from
		gso_generate_tcp_seg() function call.
*//***************************************************************************/
int32_t gso_discard_frame_remainder(
		gso_ctx_t gso_ctx_addr);

/**************************************************************************//**
@Function	gso_context_init

@Description	This function initializes the GSO context structure that is
		used for the TCP GSO process of the packet.

		This function must be called once before each new packet
		segmentation process.

@Param[in]	flags - Please refer to \ref TCP_GSO_FLAGS.
@Param[in]	mss - Maximum Segment Size.
@Param[out]	gso_ctx_addr - Address to the TCP GSO internal context structure
		allocated by the user. Internally used by TCP GSO functions.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void gso_context_init(
		uint32_t flags,
		uint16_t mss,
		gso_ctx_t gso_ctx_addr);

/** @} */ /* end of TCP_GSO_Functions */



/** @} */ /* end of FSL_AIOP_TCP_GSO */



#endif /* __FSL_GSO_H */
