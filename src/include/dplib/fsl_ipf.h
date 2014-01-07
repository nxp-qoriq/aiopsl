/**************************************************************************//**
@File		fsl_ipf.h

@Description	This file contains the AIOP SW IP fragmentation API
*//***************************************************************************/
#ifndef __FSL_IPF_H
#define __FSL_IPF_H

#include "common/types.h"


/**************************************************************************//**
@Group		FSL_IPF FSL_IPF

@Description	Freescale AIOP IP Fragmentation

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_IPF_MACROS IP Fragmentation Macros
@{
*//***************************************************************************/
/**************************************************************************//**
 @Group	IPF_GENERAL_DEFINITIONS IPF General Definitions
 @{
*//***************************************************************************/

	/** IPF context size definition. */
#define IPF_CONTEXT_SIZE	64
	/** IPF context definition. */
typedef uint8_t ipf_ctx_t[IPF_CONTEXT_SIZE];


/** @} */ /* end of TCP_IPF_GENERAL_DEFINITIONS */


/**************************************************************************//**
 @Group	IPF_Flags Flags for ipf_generate_frag() function
 @{
*//***************************************************************************/
/** No flags indication. */
#define IPF_NO_FLAGS			0x00000000
/** Restore Original Fragments.
 * When set, fragmentation restores the original fragments of a reassembled
 * frame and ignores MTU.
 * Should not be set in case \ref IPF_SFV_QUERY returns the value 0. */
#define IPF_RESTORE_ORIGINAL_FRAGMENTS	0x00000001

/** @} */ /* end of IPF_Flags */

/**************************************************************************//**
@Group	IPF_GENERATE_FRAG_STATUS ipf_generate_frag() return values
@{
*//***************************************************************************/
/** Fragmentation process complete. The last fragment was generated */
#define	IPF_GEN_FRAG_STATUS_DONE		SUCCESS
/** Fragmentation process didn't complete.
 * Fragment was generated and the user SHOULD call generate_frag()
 * again to generate another fragment*/
#define	IPF_GEN_FRAG_STATUS_IN_PROCESS 	(IPF_MODULE_STATUS_ID | 0x1)
/** Fragmentation not done due to Length > MTU but DF=1 */
#define	IPF_GEN_FRAG_STATUS_DF_SET 	(IPF_MODULE_STATUS_ID | 0x2)
/** Restoration of original fragments can not be performed since SFV=0 */ 
#define	IPF_GEN_FRAG_STATUS_SFV_CLEAR 	(IPF_MODULE_STATUS_ID | 0x3)

/** @} */ /* end of IPF_GENERATE_FRAG_STATUS */

/**************************************************************************//**
@Group	FSL_IPF_SFV_MACROS IP Fragmentation SFV (Start Fragment Valid)bit macros
@{
*//***************************************************************************/
/**SFV bit clear.
 * This macro should be used in case the application has modified the
 * reassembled frame thus restoration of the original fragments is not possible.
 * This includes: modifications of the header, changes of frame boundaries
 * (e.g. IPSec, GRO).  */
#define IPF_SFV_CLEAR()  0 /*TODO maybe move to general.h ?*/
/** SFV bit query. This macro returns the value of SFV bit.
 * In case the value of SFV is 0, the user should not ask for restoration of
 * original fragments.*/
#define IPF_SFV_QUERY()	 1 /*TODO maybe move to general.h ?*/


/** @} */ /* end of FSL_IPF_SFV_MACROS */

/** @} */ /* end of FSL_IPF_MACROS */


/**************************************************************************//**
@Group		AIOP_IPF_Functions

@Description	AIOP IP Fragmentation Functions

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

@Cautions	No support in IPv6 jumbograms.
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

@Param[in]	flags - Please refer to \ref IPF_Flags.
@Param[in]	mtu - Maximum Transmit Unit.
		In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
		parameter is ignored.
@Param[out]	ipf_context_addr - Address to the IPF internal context
		structure allocated by the user. Internally used by
		IP Fragmentation functions.

@Return		None.

@Cautions	No support in IPv6 jumbograms.
*//***************************************************************************/
void ipf_context_init(uint32_t flags, uint16_t mtu, ipf_ctx_t ipf_context_addr);

/** @} */ /* end of AIOP_IPF_Functions */
/** @} */ /* end of FSL_IPF */


#endif /* __FSL_IPF_H */
