/**************************************************************************//**
@File		fsl_ipf.h

@Description	This file contains the AIOP SW IP fragmentation API
*//***************************************************************************/
#ifndef __FSL_IPF_H
#define __FSL_IPF_H

#include "common/types.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		FSL_IPF IPF

@Description	Freescale AIOP IP Fragmentation

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	IPF_MACROS IPF Macros
@{
*//***************************************************************************/
/**************************************************************************//**
 @Group	IPF_GENERAL_DEFINITIONS IPF General Definitions
 @{
*//***************************************************************************/

	/** IPF context size definition. */
#define IPF_CONTEXT_SIZE	64
	/** IPF context definition. Must be aligned to 32 Bytes.*/
typedef uint8_t ipf_ctx_t[IPF_CONTEXT_SIZE];


/** @} */ /* end of TCP_IPF_GENERAL_DEFINITIONS */


/**************************************************************************//**
 @Group	IPF_FLAGS IPF Flags

 @Description	Flags for ipf_generate_frag() function

 @{
*//***************************************************************************/
/** No flags indication. */
#define IPF_NO_FLAGS			0x00000000
/** Restore Original Fragments.
 * When set, fragmentation restores the original fragments of a reassembled
 * frame and ignores MTU. */
#define IPF_RESTORE_ORIGINAL_FRAGMENTS	0x00000001

/** @} */ /* end of IPF_FLAGS */

/**************************************************************************//**
@Group	IPF_GENERATE_FRAG_STATUS  IPF Return Statuses

@Description ipf_generate_frag() return values

@{
*//***************************************************************************/
/** Fragmentation process complete. The last fragment was generated */
#define	IPF_GEN_FRAG_STATUS_DONE		SUCCESS
/** Fragmentation process didn't complete.
 * Fragment was generated and the user SHOULD call generate_frag()
 * again to generate another fragment*/
#define	IPF_GEN_FRAG_STATUS_IN_PROCESS	(IPF_MODULE_STATUS_ID | 0x1)
/** Fragmentation not done due to Length > MTU but DF=1 */
#define	IPF_GEN_FRAG_STATUS_DF_SET	(IPF_MODULE_STATUS_ID | 0x2)

/** @} */ /* end of IPF_GENERATE_FRAG_STATUS */

/** @} */ /* end of IPF_MACROS */


/**************************************************************************//**
@Group		IPF_Functions IPF Functions

@Description	AIOP IPF Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	ipf_generate_frag

@Description	This function generates a single IP fragment and locates in
		the default frame location in the workspace.

		Pre-condition - In the first iteration this function is called
		for a source packet, the source packet should be located
		at the default frame location in workspace.

		The remaining source frame is kept in the internal IPF
		structure, and remains open until fragmentation process is
		complete (\ref IPF_GEN_FRAG_STATUS_DONE).

		This function should be called repeatedly
		until the returned status indicates fragmentation is complete
		(\ref IPF_GEN_FRAG_STATUS_DONE).

@Param[in]	ipf_context_addr - Address to the IPF internal context. Must
		be initialized by ipf_context_init() prior to the first call.

@Return		Status. Please refer to \ref IPF_GENERATE_FRAG_STATUS or
		\ref fdma_hw_errors or \ref fdma_sw_errors for more details.

@Cautions	1. In the output fragment, ASA & PTA are not presented.
		2. No support in IPv6 jumbograms.
		3. Since during fragmentation process of an IPv6 frame, fragment
		extension (8 bytes) is added to the header, user must ensure
		that either 8 bytes are available in the headroom, or that
		Presented segment size is large enough to include these 8 bytes
		in addition to any existing headers presented.
*//***************************************************************************/
int32_t ipf_generate_frag(ipf_ctx_t ipf_context_addr);

/**************************************************************************//**
@Function	ipf_discard_frame_remainder

@Description	This function discards the remanider of the frame being
		fragmented in case the user decides to stop the fragmentation
		process before its completion (before
		\ref IPF_GEN_FRAG_STATUS_DONE status is returned).

@Param[in]	ipf_context_addr - Address to the IPF internal context.

@Return		Status of the operation (\ref FDMA_DISCARD_FRAME_ERRORS).

@Cautions	Following this function no packet resides in the default frame
		location in the task defaults.
		This function should only be called after \ref
		IPF_GEN_FRAG_STATUS_IN_PROCESS status is returned from \ref
		ipf_generate_frag() function call.
*//***************************************************************************/
int32_t ipf_discard_frame_remainder(ipf_ctx_t ipf_context_addr);

/**************************************************************************//**
@Function	ipf_context_init

@Description	This function initializes the IPF context structure that is
		used for the IP fragmentation process.

@Param[in]	flags - Please refer to \ref IPF_FLAGS.
@Param[in]	mtu - Maximum Transmit Unit.
		In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
		parameter is ignored.
@Param[out]	ipf_context_addr - Address to the IPF internal context
		structure allocated by the user. Internally used by
		IP Fragmentation functions. Must be aligned to 32 bytes.

@Return		None.

@Cautions	No support in IPv6 jumbograms.
*//***************************************************************************/
void ipf_context_init(uint32_t flags, uint16_t mtu, ipf_ctx_t ipf_context_addr);

/** @} */ /* end of IPF_Functions */
/** @} */ /* end of FSL_IPF */
/** @} */ /* end of NETF */


#endif /* __FSL_IPF_H */
