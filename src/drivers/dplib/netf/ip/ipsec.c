/**************************************************************************//**
@File		ipsec.c

@Description	This file contains the AIOP IPSec implementation.
		
*//***************************************************************************/

#include "common/types.h"
#include "common/spinlock.h"
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_ipsec.h"
#include "ipsec.h"
#include "cdma.h"

/* Global parameters */
__SHRAM struct ipsec_global_params global_params;

/**************************************************************************//**
* 	ipsec_init
*//****************************************************************************/
int32_t ipsec_init(uint32_t max_sa_no) {
	
	global_params.sa_count = max_sa_no;
	global_params.asa_bpid = 1; // TMP
	global_params.desc_bpid = 1; // TMP
	global_params.tmi = 1; // TMP
	global_params.spinlock = 0;

	return 0;
} /* End of ipsec_init */


/**************************************************************************//**
@Function		ipsec_generate_flc 

@Description	Generate SEC Flow Context Descriptor
*//***************************************************************************/
int32_t ipsec_generate_flc(
		uint64_t flc_address, /* Flow Context Address in external memory */
		uint16_t spid) /* Storage Profile ID of the SEC output frame */
{
	
	int32_t return_val;
	
	struct ipsec_flow_context flow_context;
	uint32_t *sp_addr = (uint32_t *)(IPSEC_PROFILE_SRAM_ADDR + 
							(spid<<IPSEC_STORAGE_PROFILE_SIZE_SHIFT));

	/* Word 0 */
	flow_context.word0_sdid = 0; //TODO: how to get this value? 
	flow_context.word0_res = 0; 

	/* Word 1 */
	flow_context.word1_sdl = 0 ; /* 5-0 SDL, 7-6 reserved */
	/* SDL = Shared Descriptor length */
	// TODO: need to get this from the RTA
	
	flow_context.word1_bits_15_8 = 0; /* 11-8 CRID, 14-12 reserved, 15 CRJD */
	flow_context.word1_bits23_16 = 0; /* 16	EWS,17 DAC,18-20?, 23-21 reserved */
	flow_context.word1_bits31_24 = 0; /* 24 RSC, 25 RBMT, 31-26 reserved */
	
	/* word 2  RFLC[31-0] */
	flow_context.word2_rflc_31_0 = 0; /* Not used for AIOP */

	/* word 3  RFLC[63-32] */
	flow_context.word3_rflc_63_32 = 0; /* Not used for AIOP */

	/* word 4 */ /* Not used, should be NULL */
	flow_context.word4_iicid = 0; /* 15-0  IICID */
	flow_context.word4_oicid = 0; /* 31-16 OICID */
	
	/* word 5 */ /* Not used for AIOP */
	flow_context.word5_31_0 = 0; /* 23-0 OFQID, 24 OSC, 25 OBMT, 29-26 revd */
		/* 31-30 ICR : AIOP is trusted user - no need for any restrictions. */
	
	/* word 6 */
	flow_context.word6_oflc_31_0 = 0; /* Not used for AIOP */
	
	/* word 7 */
	flow_context.word7_oflc_63_32 = 0; /* Not used for AIOP */
	
	/* Copy the standard Storage Profile to Flow Context words 8-15 */
	flow_context.storage_profile[0] = *(sp_addr +  0);
	flow_context.storage_profile[1] = *(sp_addr +  8);
	flow_context.storage_profile[2] = *(sp_addr + 16);
	flow_context.storage_profile[3] = *(sp_addr + 24);

	/* Write the Flow Context to external memory with CDMA */
	return_val = cdma_write(
			flc_address, /* ext_address */
			&flow_context, /* ws_src */
			IPSEC_FLOW_CONTEXT_SIZE); /* uint16_t size */

	return 0; // TMP
}




/**************************************************************************//**
*	ipsec_add_sa_descriptor
*//****************************************************************************/
int32_t ipsec_add_sa_descriptor(
		struct ipsec_descriptor_params *params,
		ipsec_handle_t *ipsec_handle)
{

	int32_t return_val;
	
	struct ipsec_descriptor_params *dummy = params; // Just to avoid warning
	
	
	/* Create a shared descriptor */
	
	/* Check if SAs counter reached zero */
	/* If not, decrement SA counter. If yes return with error */
	lock_spinlock((uint8_t *)&global_params.spinlock);
		
	if (global_params.sa_count == 0) {
		unlock_spinlock((uint8_t *)&global_params.spinlock);
		return -1; // TODO add error code
	} else {
		global_params.sa_count--;
		unlock_spinlock((uint8_t *)&global_params.spinlock);
	}
	
	/* Allocate a buffer for the FM parameters, 
	 * Flow Context and Shared Descriptor with CDMA. */
	/* allocate a buffer with the CDMA */
	return_val = (int32_t)cdma_acquire_context_memory(
		global_params.desc_bpid,
		ipsec_handle); /* context_memory */ 
	
	// TODO: check for allocation error
	
	/* TODO: Clear the buffer area?
	 *  Looks like CDMA can only clear the workspace, so it is not useful. */
	
	
	/* Read the storage profile from the Profile SRAM */
	// TODO
	
	
	/* Generate the SEC Flow Context descriptor and write to memory with CDMA */
	return_val = ipsec_generate_flc(
			((*ipsec_handle) + IPSEC_INTERNAL_PARMS_SIZE), 
				/* Flow Context Address in external memory */
			params->spid); /* Storage Profile ID of the SEC output frame */
	
	
	
	/* Prepare data structures for RTA. */
	
	/* Call the RTA with the shared buffer pointer 
	 * (adjacent to the flow context). 
	 * The RTA creates the descriptor and stores it in the memory 
	 * with CDMA commands. */
	
	/*	Prepare descriptor parameters:
	 * Kilobytes and packets lifetime limits.
	 * Modes indicators and other flags */
	
	/* Store the descriptor parameters to memory (CDMA write). */
	
	/* Create one-shot TMAN timers for the soft and hard seconds lifetime 
	 * limits, with callback to internal function 
	 * (including the descriptor handle and soft/hard indication arguments). */
	
	/* Return handle. */

	return 0; // TMP

} /* End of ipsec_add_sa_descriptor */

/**************************************************************************//**

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */
/** @} */ /* end of NETF */


