/**************************************************************************//**
@File		aiop_virtual_pools.c

@Description	This file contains the AIOP Virtual Pools 
				Operations implementation.
*//***************************************************************************/

#include "virtual_pools.h"
#include "fsl_spinlock.h"
#include "dplib/fsl_cdma.h"

//struct virtual_pool_desc virtual_pools[MAX_VIRTUAL_POOLS_NUM];
struct bman_pool_desc virtual_bman_pools[MAX_VIRTUAL_BMAN_POOLS_NUM];

struct virtual_pools_root_desc virtual_pools_root;


/***************************************************************************
 * vpool_create_pool
 ***************************************************************************/
int32_t vpool_create_pool( 
		uint16_t bman_pool_id, 
		int32_t max_bufs, 
		int32_t committed_bufs,
		uint32_t flags,
		int32_t (*callback_func)(uint64_t),
		uint32_t *virtual_pool_id)
{
	
	uint32_t vpool_id;
	uint32_t num_of_virtual_pools = virtual_pools_root.num_of_virtual_pools;
	struct virtual_pool_desc *virtual_pool = (struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	struct callback_s *callback = (struct callback_s *)virtual_pools_root.callback_func_struct;
	callback->callback_func = callback_func;

	#ifdef SL_DEBUG
		/* Check the arguments correctness */
		if (bman_pool_id >= MAX_VIRTUAL_BMAN_POOLS_NUM)
			return VIRTUAL_POOLS_ILLEGAL_ARGS;

		/* max_bufs must be equal or greater than committed_bufs */
		if (committed_bufs > max_bufs)
			return VIRTUAL_POOLS_ILLEGAL_ARGS;	
	
		/* committed_bufs and max_bufs must not be 0 */
		if ((!committed_bufs) || (!max_bufs))
			return VIRTUAL_POOLS_ILLEGAL_ARGS;	
	#endif

	
	/* Spinlock this BMAN pool counter */
	lock_spinlock((uint8_t *)&virtual_bman_pools[bman_pool_id].spinlock);
	
	/* Check if there are enough buffers to commit */
	if (virtual_bman_pools[bman_pool_id].remaining >= committed_bufs) {
		/* decrement the total available BMAN pool buffers */ 
		virtual_bman_pools[bman_pool_id].remaining -= committed_bufs;
		
		unlock_spinlock((uint8_t *)&virtual_bman_pools[bman_pool_id].spinlock);

	} else {
		unlock_spinlock((uint8_t *)&virtual_bman_pools[bman_pool_id].spinlock);
		return VIRTUAL_POOLS_INSUFFICIENT_BUFFERS;
	}
		
	lock_spinlock((uint8_t *)&vpool_spinlock_var);
	
	/* Allocate a virtual pool ID */
	/* Return with error if it was not possible to allocate a virtual pool */
	for(vpool_id = 0; vpool_id < num_of_virtual_pools; vpool_id++) {
		if(virtual_pool->max_bufs == 0) {
			virtual_pool->max_bufs = max_bufs; /* use max_bufs as indicator */
			break;
		}
		virtual_pool++; /* increment the pointer */
	}
	
	unlock_spinlock((uint8_t *)&vpool_spinlock_var);	
	
	*virtual_pool_id = vpool_id; /* Return the ID */

	/* Return with error if no pool is available */
	if (vpool_id == num_of_virtual_pools)
		return VIRTUAL_POOLS_ID_ALLOC_FAIL;
	
	virtual_pool->committed_bufs = committed_bufs;
	virtual_pool->allocated_bufs = 0; // TODO: Should it be '0'?
	virtual_pool->bman_pool_id = bman_pool_id;
	virtual_pool->flags = (uint8_t)flags;
	
	/* Check if a callback structure exists and initialize the entry */
	if (virtual_pools_root.callback_func_struct != NULL) {
		callback += vpool_id;
		callback->callback_func = callback_func;
	}

	return VIRTUAL_POOLS_SUCCESS;
} /* End of vpool_create_pool */


/***************************************************************************
 * vpool_release_pool
 ***************************************************************************/
int32_t vpool_release_pool(uint32_t virtual_pool_id)
{
	uint16_t bman_pool_id;

	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	virtual_pool += virtual_pool_id;
	
	bman_pool_id = virtual_pool->bman_pool_id;
	
	lock_spinlock((uint8_t *)&vpool_spinlock_var);
	
	if (virtual_pool->allocated_bufs != 0) {
		unlock_spinlock((uint8_t *)&vpool_spinlock_var);	
		return VIRTUAL_POOLS_RELEASE_POOL_FAILED;	
	}
	
	/* max_bufs = 0 indicates a free pool */
	virtual_pool->max_bufs = 0; 
	
	//TODO: put vpool_spinlock_var as part of the root structure
	// TODO; can it use the virtual pool spinlock?
	unlock_spinlock((uint8_t *)&vpool_spinlock_var);	
	
	/* Increment the total available BMAN pool buffers */ 
	aiop_atomic_incr32(&virtual_bman_pools[bman_pool_id].remaining, 
			virtual_pool->committed_bufs);
		
	return VIRTUAL_POOLS_SUCCESS;	
} /* End of vpool_release_pool */

/***************************************************************************
 * vpool_read_pool
 ***************************************************************************/
int32_t vpool_read_pool(uint32_t virtual_pool_id, 
		uint16_t *bman_pool_id, 
		int32_t *max_bufs, 
		int32_t *committed_bufs,
		int32_t *allocated_bufs,
		uint32_t *flags,
		int32_t *callback_func)

{
	
	// TODO: remove this if moving to handle
	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	virtual_pool += virtual_pool_id;
	
	*max_bufs = virtual_pool->max_bufs;
	*committed_bufs = virtual_pool->committed_bufs;
	*allocated_bufs = virtual_pool->allocated_bufs; 
	*bman_pool_id = virtual_pool->bman_pool_id;
	*flags =	(uint8_t)virtual_pool->flags;
	*callback_func = 0; // TODO: need to check if callback exists and return it
	
	return VIRTUAL_POOLS_SUCCESS;	
}

/***************************************************************************
 * vpool_init
 ***************************************************************************/
int32_t vpool_init(
		uint64_t *virtual_pool_struct, // Change to uint64_t (not pointer)
		uint64_t *callback_func_struct,
		uint32_t num_of_virtual_pools,
		uint32_t flags)
{
	int32_t i;
	// Add mask for downcast
	struct virtual_pool_desc *virtual_pool = (struct virtual_pool_desc *)virtual_pool_struct;

	virtual_pools_root.virtual_pool_struct = virtual_pool_struct;
	virtual_pools_root.callback_func_struct = callback_func_struct;
	virtual_pools_root.num_of_virtual_pools = num_of_virtual_pools;
	virtual_pools_root.flags = flags;
	
	/* Init 'max' to zero, since it's an indicator to pool ID availability */
	for(i = 0; i < num_of_virtual_pools; i++) {
		virtual_pool->max_bufs = 0;
		virtual_pool++; /* increment the pointer */
	}
	return VIRTUAL_POOLS_SUCCESS;	
}


/***************************************************************************
 * vpool_init_total_bman_bufs
 ***************************************************************************/
int32_t vpool_init_total_bman_bufs( 
		uint16_t bman_pool_id, 
		int32_t total_avail_bufs, // TODO: remove
		uint32_t buf_size)
{
	#ifdef SL_DEBUG
		/* Check the arguments correctness */
		if (bman_pool_id >= MAX_VIRTUAL_BMAN_POOLS_NUM)
			return VIRTUAL_POOLS_ILLEGAL_ARGS;
	#endif

	//TODO: bman_pool_id as index is a problem
		
	lock_spinlock((uint8_t *)&vpool_spinlock_var); // TODO: Remove spinlock
	
	virtual_bman_pools[bman_pool_id].remaining = total_avail_bufs;
	virtual_bman_pools[bman_pool_id].buf_size = buf_size;

	unlock_spinlock((uint8_t *)&vpool_spinlock_var);	

	return VIRTUAL_POOLS_SUCCESS;
}


/***************************************************************************
 * vpool_add_total_bman_bufs
 ***************************************************************************/
int32_t vpool_add_total_bman_bufs( 
		uint16_t bman_pool_id, 
		int32_t additional_bufs)
{
	
	#ifdef SL_DEBUG
		/* Check the arguments correctness */
		if (bman_pool_id >= MAX_VIRTUAL_BMAN_POOLS_NUM)
				return VIRTUAL_POOLS_ILLEGAL_ARGS;
	#endif
		
	/* Increment the total available BMAN pool buffers */ 
	aiop_atomic_incr32(&virtual_bman_pools[bman_pool_id].remaining, 
			additional_bufs);

	return VIRTUAL_POOLS_SUCCESS;
}

/***************************************************************************
 * vpool_allocate_buf
 ***************************************************************************/
int32_t vpool_allocate_buf(uint32_t virtual_pool_id, 
		uint64_t *context_address)
{
	int32_t return_val;
	int allocate = FALSE;
	uint16_t bman_pool_id;
	
	// TODO: remove this if moving to handle
	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	virtual_pool += virtual_pool_id;

	
	bman_pool_id = virtual_pool->bman_pool_id;
	              
	lock_spinlock((uint8_t *)&virtual_pool->spinlock);

	/* First check if there are still available buffers in the VP committed area */ 
	if(virtual_pool->allocated_bufs < 
			virtual_pool->committed_bufs) 	{
		allocate = TRUE;
	/* Else, check if there are still available buffers in the VP max-committed area */ 
	} else if (virtual_pool->allocated_bufs < virtual_pool->max_bufs) {
		/* There is still an extra space in the virtual pool, check BMAN pool */
		
		/* spinlock this BMAN pool counter */
		lock_spinlock((uint8_t *)&virtual_bman_pools[bman_pool_id].spinlock);
		
		if (virtual_bman_pools[bman_pool_id].remaining > 0)
			allocate = TRUE;
		else
			unlock_spinlock((uint8_t *)&virtual_bman_pools[bman_pool_id].spinlock);

	}
		
	/* Request CDMA to allocate a buffer*/
	if (allocate) {
		
		// TODO: what happens if CDMA failed? Need to increment?
		virtual_pool->allocated_bufs++;
	
		unlock_spinlock((uint8_t *)&virtual_pool->spinlock);

		//aiop_atomic_decr32(&virtual_bman_pools[bman_pool_id].remaining, 1);
		/* Decrement the counter. It is still under spinlock here */
		virtual_bman_pools[bman_pool_id].remaining--; // TODO: this should be done only if it was allocated from the common area???!!!
		/* unlock spinlock of this BMAN pool counter */
		unlock_spinlock((uint8_t *)&virtual_bman_pools[bman_pool_id].spinlock);
		
		// TODO: still need to look at all corner cases. The idea is that bman_allocate 
		// MK: "can work in parallel with several cores so we do not take semaphores !"

		/* allocate a buffer with the CDMA */
		return_val = (int32_t)cdma_acquire_context_memory(
				(uint32_t)virtual_bman_pools[bman_pool_id].buf_size, 
				(uint16_t)bman_pool_id, /* uint16_t pool_id */
				(uint64_t *)context_address); /* uint64_t *context_memory */ 

		/* If allocation failed, undo the counters increment/decrement */
		if (return_val) {
			//vpool_atomic_decr(&virtual_pools[virtual_pool_id].allocated_bufs, 1); // Yariv: this was missing from MK code
			//vpool_atomic_incr(&virtual_bman_pools[bman_pool_id].remaining, 1);
			aiop_atomic_decr32(&virtual_pool->allocated_bufs, 1); // Yariv: this was missing from MK code
			aiop_atomic_incr32(&virtual_bman_pools[bman_pool_id].remaining, 1);
		}
	
		return (VIRTUAL_POOLS_CDMA_ERR | return_val);
	} else {
		//vpool_open_spinlock((uint8_t *)&virtual_pools[virtual_pool_id].spinlock);
		unlock_spinlock((uint8_t *)&virtual_pool->spinlock);
		return VIRTUAL_POOLS_BUF_ALLOC_FAIL;
	}
	
}


/***************************************************************************
 * vpool_release_buf
 ***************************************************************************/
int32_t vpool_release_buf(uint32_t virtual_pool_id, 
		uint64_t context_address)
{
	int32_t return_val;
	int32_t cdma_status;
	
	cdma_status = cdma_release_context_memory(context_address);
	
	if (!cdma_status) {
		return_val = __vpool_internal_release_buf(virtual_pool_id); 
		return return_val | cdma_status; /* Keep original CDMA return value */
	} else {
		return VIRTUAL_POOLS_BUF_NOT_RELEASED | cdma_status; 
	}
		
}

/***************************************************************************
 * __vpool_internal_release_buf
 ***************************************************************************/
int32_t __vpool_internal_release_buf(uint32_t virtual_pool_id)
{
	uint16_t bman_pool_id;
	
	// TODO: remove this if moving to handle
	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	virtual_pool += virtual_pool_id;
	
	bman_pool_id = virtual_pool->bman_pool_id;
	
	lock_spinlock((uint8_t *)&virtual_pool->spinlock);

	/* First check if buffers were allocated from the common pool */ 
	if(virtual_pool->allocated_bufs > 
			virtual_pool->committed_bufs) 	{
		/* One buffer returns to the common pool */
		aiop_atomic_incr32(&virtual_bman_pools[bman_pool_id].remaining, 1);
	}	
	
	virtual_pool->allocated_bufs--;
	
	unlock_spinlock((uint8_t *)&virtual_pool->spinlock);
	
	return VIRTUAL_POOLS_SUCCESS;
}

/***************************************************************************
 * vpool_decr_ref_counter
 ***************************************************************************/
int32_t vpool_refcount_decrement_and_release(uint32_t virtual_pool_id,
		uint64_t context_address)
{
	int32_t return_val;
	int32_t cdma_status;
	
	/* cdma_refcount_decrement_and_release: 
	 *	This routine decrements reference count of Context memory
	 *	object. If resulting reference count is zero, the following
	 *	CDMA_REFCOUNT_DECREMENT_TO_ZERO status code is reported
	 *	and the Context memory block is automatically released to the
	 *	BMan pool it was acquired from.
	*/
	
	// TODO: check if callback exists
	// If yes, decrement counter -> callback -> release
	// else, do the combo command
	
	cdma_status = cdma_refcount_decrement_and_release(context_address);
	if (cdma_status == CDMA_REFCOUNT_DECREMENT_TO_ZERO) {
		return_val = __vpool_internal_release_buf(virtual_pool_id) | 
				(int32_t)cdma_status; /* Keep original CDMA return value */
		return return_val;
	} else {
		return VIRTUAL_POOLS_BUF_NOT_RELEASED | (int32_t)cdma_status; 
	}
		
}








