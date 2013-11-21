/**************************************************************************//**
@File		fsl_ipf.h

@Description	This file contains the AIOP SW IP fragmentation API
*//***************************************************************************/
#ifndef __FSL_IPF_H
#define __FSL_IPF_H

#include "general.h"

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
 @Group	IPF_Flags Flags for ipf_generate_frag() function
 @{
*//***************************************************************************/
/** Restore Original Fragments.
 * When set, fragmentation restores the original fragments of a reassembled
 * frame and ignores MTU.
 * Should not be set in case \ref IPF_SFV_QUERY returns the value 0. */
#define IPF_RESTORE_ORIGINAL_FRAGMENTS	0x0001

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
/** Length > MTU but DF=1 */
#define	IPF_GEN_FRAG_STATUS_DF_SET 	(IPF_MODULE_STATUS_ID | 0x2)

/** @} */ /* end of IPF_GENERATE_FRAG_STATUS */

/**************************************************************************//**
@Group	FSL_IPF_SFV_MACROS IP Fragmentation SFV (Start Fragment Valid)bit macros
@{
*//***************************************************************************/
/**SFV bit clear
 * This macro should be used in case the application has modified the
 * reassembled frame thus restoration of the original fragments is not possible.
 * This includes: modifications of the header, changes of frame boundaries
 * (e.g. IPSec, GRO).  */
#define IPF_SFV_CLEAR   /*TODO maybe move to general.h ?*/
/** SFV bit query. This macro returns the value of SFV bit.
 * In case the value of SFV is 0, the user should not ask for restoration of
 * original fragments.*/
#define IPF_SFV_QUERY	/*TODO maybe move to general.h ?*/


/** @} */ /* end of FSL_IPF_SFV_MACROS */

/** @} */ /* end of FSL_IPF_MACROS */

/**************************************************************************//**
@Group		FSL_IPF_STRUCTS IP Fragmentation Structures

@Description	Freescale AIOP IP Fragmentation Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IP Fragmentation Context Internally used by IPF functions.
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
	/** Remaining frame handle*/
	uint8_t rem_frame_handle;
	/** Fragment index */
	uint8_t frag_index;       
	/** First fragment indication */
	uint8_t	first_frag;
	/** Padding */
	uint8_t  pad[3];
};

/** @} */ /* end of FSL_IPF_STRUCTS */

/**************************************************************************//**
@Group		AIOP_IPF_Functions

@Description	AIOP IPF Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	ipf_generate_frag

@Description	This function generates a single IP fragment and locates in
		the default frame location in the workspace.

@Param[in,out]	ipf_ctx - Pointer to IP fragmentation context. Must be
		initialized by \ref ipf_context_init() prior to the first call.

@Return		Status. Please refer to \ref IPF_GENERATE_FRAG_STATUS or
		\ref fdma_hw_errors or \ref fdma_sw_errors for more details.

@Cautions	The frame to be fragmented must be presented.
		No support in IPv6 jumbograms.
*//***************************************************************************/
int32_t ipf_generate_frag(struct ipf_context *ipf_ctx);

/**************************************************************************//**
@Function	ipf_discard_frame_remainder

@Description	This function discards the remanider of the frame being
		fragmented in case the user decides to stop the fragmentation
		process before its completion.

@Param[in,out]	ipf_ctx - Pointer to IP fragmentation context.
		Please refer to \ref ipf_context structure description for more
		details.

@Return		Status. Please refer to \ref fdma_hw_errors for more details.

@Cautions	This function should only be called after \ref
		IPF_GEN_FRAG_STATUS_IN_PROCESS status is returned from \ref
		ipf_generate_frag() function call.
*//***************************************************************************/
int32_t ipf_discard_frame_remainder(struct ipf_context *ipf_ctx);

/**************************************************************************//**
@Function	ipf_context_init

@Description	This function initializes the IPF context structure that is
		used for the IP fragmentation process.

@Param[in]	flags - Please refer to \ref IPF_Flags.
@Param[in]	mtu - Maximum Transmit Unit.
	 	In case \ref IPF_RESTORE_ORIGINAL_FRAGMENTS flag is set, this
	 	parameter is ignored.
@Param[out]	ipf_ctx - Pointer to IP fragmentation context structure
		allocated by the user. Internally used by IP Fragmentation
		functions.

@Return		None.

@Cautions	No support in IPv6 jumbograms.
*//***************************************************************************/
inline void ipf_context_init(uint32_t flags, uint16_t mtu,
	struct ipf_context *ipf_ctx)
{
	ipf_ctx->first_frag = 1;
	ipf_ctx->flags = flags;
	ipf_ctx->mtu = mtu;
}

/** @} */ /* end of AIOP_IPF_Functions */
/** @} */ /* end of FSL_IPF */

#endif /* __FSL_IPF_H */
