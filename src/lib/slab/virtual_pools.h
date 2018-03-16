/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/***************************************************************************//*
@File          aiop_virtual_pools.h

@Description   Header file of the AIOP SW Virtual Pools (FSL internal)

@Cautions      None.
*//***************************************************************************/

#ifndef __AIOP_LIB_VIRTUAL_POOLS_H
#define __AIOP_LIB_VIRTUAL_POOLS_H

#include "general.h"

/**************************************************************************//**
 @Group		AIOP_VIRTUAL_POOLS

 @Description	AIOP Virtual Pools macros and functions

 @{
*//***************************************************************************/

typedef int (vpool_callback_t) (uint64_t context_address);

/* Virtual Pool structure */
struct virtual_pool_desc {
	int32_t max_bufs;
	int32_t committed_bufs;
	int32_t allocated_bufs;
	uint8_t spinlock;
	uint8_t flags;
	uint16_t bman_array_index;
};

/* Callback functions struct */
struct callback_s {
	vpool_callback_t *callback_func;
};

/* BMAN Pool structure */
struct bman_pool_desc {
	int32_t remaining;
    uint8_t spinlock;
    uint8_t flags;
    uint16_t bman_pool_id;
};

// TMP
struct virtual_pools_root_desc {
		uint64_t virtual_pool_struct;
		uint64_t callback_func_struct;
		uint32_t num_of_virtual_pools;
		uint32_t flags;
		uint32_t global_spinlock;
		uint32_t reserved;
};

/**************************************************************************//**
@Group			VIRTUAL_POOLS_PARAMETERS

@Description	Virtual Pool Parameters

@{
*//***************************************************************************/
//TODO: TMP. need to define these numbers in global project file?
/* Maximum number of virtual pools */
//#ifndef MAX_VIRTUAL_POOLS_NUM
//	#define MAX_VIRTUAL_POOLS_NUM 1000
//#endif
/* Maximum number of BMAN pools used by the virtual pools */
#ifndef MAX_VIRTUAL_BMAN_POOLS_NUM
	#define MAX_VIRTUAL_BMAN_POOLS_NUM 64
#endif

/* @} end of group VIRTUAL_POOLS_PARAMETERS */


/**************************************************************************//**
 @Group		VIRTUAL_POOLS_Enumerations Virtual Pools Enumerations

 @Description	Virtual Pools Enumerations

 @{
*//***************************************************************************/


/**************************************************************************//**
 @enum VIRTUAL_POOLS_STATUS

 @Description	AIOP Virtual Pools Module Error codes.
                A failure status reflects CDMA module errors or Virtual Pools
                module errors.
                The rightmost status byte reflects the CDMA returned status
                while the second byte reflects the Virtual Pools function
                status. // TBD
 @{
*//***************************************************************************/
// TODO: align to project error codes methodology
enum VIRTUAL_POOLS_STATUS {
	// Temporary values
	VIRTUAL_POOLS_SUCCESS = 0x0000,
	/**< Success. */
	VIRTUAL_POOLS_GENERAL_ERROR = 0x0100,
	/**< General Error. */
	VIRTUAL_POOLS_ID_ALLOC_FAIL = 0x0200,
	/**< Virtual ID allocation failed. */
	VIRTUAL_POOLS_ILLEGAL_ARGS = 0x0300,
	/**< A Virtual Pool function was called with illegal arguments. */
	VIRTUAL_POOLS_BUF_ALLOC_FAIL = 0x0400,
	/**< Virtual Pool buffer allocation failed */
	VIRTUAL_POOLS_CDMA_ERR = 0xCD00,
	/**< CDMA Error. CDMA Error code reflected in lowest byte */
	VIRTUAL_POOLS_BUF_NOT_RELEASED = 0x0500,
	/**< Reference counter not 0, requested buffer not released */
	VIRTUAL_POOLS_RELEASE_POOL_FAILED = 0x0600,
	/**< Release of a pool failed,
	 * 	since not all allocated buffers were released */
	VIRTUAL_POOLS_INSUFFICIENT_BUFFERS = 0x0700
	/**< Insufficient buffers to create a virtual pool */
};

/* @} end of enum VIRTUAL_POOLS_STATUS */

/* @} end of group VIRTUAL_POOLS_Enumerations */

/**************************************************************************//**
@Group			VIRTUAL_POOLS_Functions

@Description	AIOP Virtual Pools functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	__vpool_internal_release_buf

@Description	This function does the virtual pools counters handling
		upon releasing a buffer.

@Param[in]	virtual_pool_id - virtual pool ID.

*//***************************************************************************/
void __vpool_internal_release_buf (uint32_t virtual_pool_id);

/* This part was originally in the file fsl_aiop_virtual_pools.h */

/*************************************************************************//**
@Function	vpool_allocate_buf

@Description	This routine requests allocation of a Context memory block
		related to a virtual pool according to the pool ID given.
		The returned value is a 64 bit address to the context in
		external memory (DDR/PEB).
		The routine is utilizing the CDMA module for the actual allocation,
		by calling the cdma_acquire_context_memory function.

@Param[in]	virtual_pool_id - the virtual pool ID.
@Param[out]	context_address - A pointer where to return
			the acquired 64 bit address of the Context memory.

@Return		Status - Success or Failure.
		A failure status reflects CDMA module errors or Virtual Pools
		module errors. (\ref cdma_errors VIRTUAL_POOLS_STATUS).

@Cautions	The maximum legal id value is MAX_AIOP_VIRTUAL_POOLS_MEM.
@Cautions	This function performs a task switch. ???

*//***************************************************************************/
int32_t vpool_allocate_buf(uint32_t virtual_pool_id,
		uint64_t *context_address);

/*************************************************************************//**
@Function	vpool_release_buf

@Description	This routine releases (frees) the Context memory block
		related to a virtual pool according to the pool ID given.
		The routine is utilizing the CDMA module for the actual
		deallocation.

@Param[in]	virtual_pool_id - the virtual pool ID.
@Param[in]	context_address - Address of Context memory to be released.

@Return		Status - Success or Failure.
		A failure status reflects CDMA module errors or Virtual Pools
		module errors. (\ref cdma_errors VIRTUAL_POOLS_STATUS).

@remark		This function is not considering the reference counter value,
		and releases the buffer even if it is not zero.
		It should not be used by an application in a normal flow.
		For decrementing the reference counter and releasing a buffer,
		use the function vpool_refcount_decrement_and_release.

@Cautions	A mutex lock (if exists) will not be released.
@Cautions	This function yields

*//***************************************************************************/
void vpool_release_buf(uint32_t virtual_pool_id,
		uint64_t context_address);

/*************************************************************************//**
@Function	vpool_refcount_increment

@Description	This routine increments the reference counter of a
		Context memory object.

@Param[in]	context_address - Address of the Context buffer.

@remark		The provided context_address must be of a valid Context buffer.

@Cautions	This function yields.

*//***************************************************************************/
void vpool_refcount_increment(
		uint64_t context_address);

/*************************************************************************//**
@Function	vpool_decr_ref_counter

@Description	This routine decrements the reference counter of a
		Context buffer, and if the counter reached zero performs
		the following actions:
		1. If this virtual pool contains a non-NULL callback function,
		   it calls the function.
		2. Releases (frees) the Context memory buffer.

		The routine is utilizing the CDMA module for the actual
		decrement and deallocation.

@Param[in]	virtual_pool_id - the virtual pool ID.
@Param[in]	context_address - Address of Context memory.
@Param[out]	callback_status - Returned status of the callback.

@Return		Status - Success or Failure.
		A failure status reflects CDMA module errors or Virtual Pools
		module errors. (\ref cdma_errors VIRTUAL_POOLS_STATUS).

@remark		This function is doing a reference counter decrement,
		and only if the counter reached zero, releases the buffer.

@Cautions	A mutex lock (if exists) will not be released.
@Cautions	This function yields

*//***************************************************************************/
int32_t vpool_refcount_decrement_and_release(
		uint32_t virtual_pool_id,
		uint64_t context_address,
		int32_t *callback_status
		);

/*************************************************************************//**
@Function	vpool_create_pool

@Description	This routine creates a virtual pool structure and returns
		the ID of it.

@Param[in]	bman_pool_id - BMAN pool ID from which the virtual pool
		allocates buffers.
@Param[in] 	max_bufs - the maximum number of Context buffers that this
		virtual pool can allocate.
@Param[in] 	committed_bufs - the number of Context buffers that this
		virtual pool must be able to allocate.
@Param[in] 	flags - control flags. Should be all zeros.

@Param[in] 	callback_func - Reference to a callback function. This function
		will invoked when calling vpool_refcount_decrement_and_release
		and a buffer related to this virtual pool should be deallocated.

@Param[out]	virtual_pool_id - virtual pool ID.
 	 	A pointer where to return the virtual pool ID.
@Return		Status - Success or Failure. (\ref VIRTUAL_POOLS_STATUS).

*//***************************************************************************/
int32_t vpool_create_pool(
		uint16_t bman_pool_id,
		int32_t max_bufs,
		int32_t committed_bufs,
		uint32_t flags,
		vpool_callback_t *callback_func,
		uint32_t *virtual_pool_id
		);


/*************************************************************************//**
@Function	vpool_release_pool

@Description	This routine releases a virtual pool structure.
				The virtual pool ID becomes free.

@Param[in]	virtual_pool_id - virtual pool ID to be released.

@Return		Status - Success or Failure. (\ref VIRTUAL_POOLS_STATUS).

*//***************************************************************************/
int32_t vpool_release_pool(uint32_t virtual_pool_id);

/*************************************************************************//**
@Function	vpool_read_pool

@Description	This function reads the virtual pool structure parameters.

@Param[in]	virtual_pool_id - virtual pool ID.
@Param[out]	bman_pool_id - A pointer where to return the  BMAN pool ID.
@Param[out] max_bufs - A pointer where to return the maximum number of
		Context buffers that this virtual pool can allocate.
@Param[out] committed_bufs - A pointer where to return the number of
		Context buffers that this virtual pool must be able to allocate.
@Param[out] allocated_bufs - A pointer where to return the current number of
		Context buffers already allocated by this virtual pool.
@Param[out] flags - A pointer where to return the virtual pool flags.
@Param[out] callback_func - A pointer where to return the callback function
		reference.

@Return		Status - Success or Failure. (\ref VIRTUAL_POOLS_STATUS).

*//***************************************************************************/
int32_t vpool_read_pool(uint32_t virtual_pool_id,
		uint16_t *bman_pool_id,
		int32_t *max_bufs,
		int32_t *committed_bufs,
		int32_t *allocated_bufs,
		uint32_t *flags,
		int32_t *callback_func
		);

/*************************************************************************//**
@Function	vpool_init

@Description	This function initializes the virtual pools structure.
		Memory blocks for the virtual pools and callback functions
		structure should be allocated by the initializing function,
		and references to these memory blocks are the input to this
		function.

@Param[in]	virtual_pool_struct - address of the virtual pools structure
		memory block.
@Param[in] 	callback_func_struct - address of the callback functions
		memory block.
		A NULL in this field indicates that no callback functions
		are available. In this case a memory block is not required.
@Param[in] 	num_of_virtual_pools - number of virtual pools.
@Param[in] 	flags - control flags. Should be all zeros.

@Return		Status - Success or Failure. (\ref VIRTUAL_POOLS_STATUS).

*//***************************************************************************/
int32_t vpool_init(
		uint64_t virtual_pool_struct,
		uint64_t callback_func_struct,
		uint32_t num_of_virtual_pools,
		uint32_t flags
);


/*************************************************************************//**
@Function	vpool_init_total_bman_bufs

@Description	This function initializes the total number of BMAN buffers
		available for the virtual pools function from each BMAN pool.
		This function should be called once for each BMAN pool ID
		used by the virtual pools function.

@Param[in]	bman_pool_id - BMAN pool ID.
@Param[in] 	total_avail_bufs - The total number of available buffers in
		bman_pool_id BMAN pool.

@Return		Status - Success or Failure. (\ref VIRTUAL_POOLS_STATUS).

*//***************************************************************************/
int32_t vpool_init_total_bman_bufs(
		uint16_t bman_pool_id,
		int32_t total_avail_bufs
		);

/*************************************************************************//**
@Function	vpool_add_total_bman_bufs

@Description	This function adds additional_bufs number of BMAN buffers to
		the total number of BMAN buffers initialized with
		vpool_init_total_bman_bufs function.
		This function should can be called multiple times .

@Param[in]	bman_pool_id - BMAN pool ID.
@Param[in] 	additional_bufs - The total number of buffers to add to the
		bman_pool_id BMAN pool.

@Return		Status - Success or Failure. (\ref VIRTUAL_POOLS_STATUS).

*//***************************************************************************/
int32_t vpool_add_total_bman_bufs(
		uint16_t bman_pool_id,
		int32_t additional_bufs);

/*************************************************************************//**
@Function	vpool_decr_total_bman_bufs

@Description	This function decrements less_bufs number of BMAN buffers to
		the total number of BMAN buffers initialized with
		vpool_init_total_bman_bufs function.
		This function should can be called multiple times .

@Param[in]	bman_pool_id - BMAN pool ID.
@Param[in] 	less_bufs - The total number of buffers to decrement from the
		bman_pool_id BMAN pool.

@Return		Status - Success or Failure. (\ref VIRTUAL_POOLS_STATUS).

*//***************************************************************************/
int32_t vpool_decr_total_bman_bufs( 
		uint16_t bman_pool_id,
		int32_t less_bufs);

// TODO: all functions should be inline


/** @} end of group VIRTUAL_POOLS_Functions */

/** @} */ /* end of AIOP_VIRTUAL_POOLS */

#endif /* _AIOP_LIB_VIRTUAL_POOLS_H */


