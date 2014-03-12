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
		0, /* TMP size; will be removed in future arch def */
		global_params.desc_bpid,
		ipsec_handle); /* context_memory */ 
	
	// TODO: check for allocation error
	
	/* TODO: Clear the buffer area?
	 *  Looks like CDMA can only clear the workspace, so it is not useful. */
	
	
	/* Read the storage profile from the Profile SRAM */
	// TODO
	
	/* Generate the SEC Flow Context descriptor and write to memory with CDMA */
	
	
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


