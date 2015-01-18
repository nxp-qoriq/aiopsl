/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File          verification_virtual_pools.h

@Description   	This file contains the AIOP Virtual Pools
				SW Verification Structures
*//***************************************************************************/

/* */

#ifndef __VERIFICATION_VIRTUAL_POOLS_H_
#define __VERIFICATION_VIRTUAL_POOLS_H_

/* Define the Virtual pools 'accelerator' ID */
/* This is just an arbitrary number, since it is not a H/W accelerator */
/* H/W accelerators use only 7 bits. The CDMA ID is 0x0D */
#ifndef VPOOL_ACCEL_ID
	#define	VPOOL_ACCEL_ID 0x8D
#endif

#ifndef MAX_VIRTUAL_POOLS_NUM
	#define MAX_VIRTUAL_POOLS_NUM 1000
#endif

/* Virtual Pools Commands */
#define	VPOOL_ALLOCATE_BUF_CMD                     0 | (VPOOL_MODULE << 16)
#define	VPOOL_RELEASE_BUF_CMD                      1 | (VPOOL_MODULE << 16)
#define	VPOOL_REFCOUNT_INCREMENT_CMD               2 | (VPOOL_MODULE << 16)
#define	VPOOL_REFCOUNT_DECREMENT_AND_RELEASE_CMD   3 | (VPOOL_MODULE << 16)
#define	VPOOL_CREATE_POOL_CMD                      4 | (VPOOL_MODULE << 16)
#define	VPOOL_RELEASE_POOL_CMD                     5 | (VPOOL_MODULE << 16)
#define	VPOOL_READ_POOL_CMD                        6 | (VPOOL_MODULE << 16)
#define	VPOOL_INIT_CMD                             7 | (VPOOL_MODULE << 16)
#define	VPOOL_INIT_TOTAL_BMAN_BUFS_CMD             8 | (VPOOL_MODULE << 16)
#define	VPOOL_ADD_TOTAL_BMAN_BUFS_CMD              9 | (VPOOL_MODULE << 16)

/** \addtogroup Service_Routines_Verification
 *  @{
 */


/**************************************************************************//**
 @Group		Virtual_Pools_Verification

 @Description	AIOP Virtual Pools Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	Virtual Pools
				vpool_allocate_buf verification command Structure.
				Actual command:
 	 	 	 	int32_t vpool_allocate_buf(uint32_t virtual_pool_id,
 	 	 	 	 	 uint64_t *context_address);

*//***************************************************************************/
struct vpool_allocate_buf_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint32_t virtual_pool_id_ptr;
		/**< Virtual pool ID */
	uint32_t context_address_ptr;
		/**< Pointer to the buffer address */
	uint32_t pad1;
		/**< Padding */
	uint64_t context_address;
		/**< Buffer address */
	int32_t status;
		/**< Command return status */
	uint32_t pad2;
		/**< Padding */
};

/**************************************************************************//**
@Description	Virtual Pools
				vpool_release_buf verification command Structure.
				Actual command:
 	 	 	 	int32_t vpool_release_buf(uint32_t virtual_pool_id,
					uint64_t context_address);

*//***************************************************************************/
struct vpool_release_buf_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint32_t virtual_pool_id_ptr;
		/**< Virtual pool ID */
	uint32_t context_address_ptr;
		/**< Pointer to the buffer address */
	int32_t status;
		/**< Command return status */
};

/**************************************************************************//**
@Description	Virtual Pools
				vpool_refcount_increment verification command Structure.
				Actual command:
 	 	 	 	int32_t vpool_refcount_increment(
					uint64_t context_address);

*//***************************************************************************/
struct vpool_refcount_increment_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint32_t context_address_ptr;
		/**< Pointer to the buffer address */
	int32_t status;
		/**< Command return status */
	uint32_t pad;
		/**< Padding to 64 bit boundary */
};


/**************************************************************************//**
@Description	Virtual Pools
				vpool_refcount_decrement_and_release
				verification command Structure.
				Actual command:
 	 	 	 	int32_t vpool_refcount_decrement_and_release(
					uint32_t virtual_pool_id,
					uint64_t context_address);

*//***************************************************************************/
struct vpool_refcount_decrement_and_release_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint32_t virtual_pool_id_ptr;
		/**< Virtual pool ID */
	uint32_t context_address_ptr;
		/**< Pointer to the buffer address */
	int32_t status;
		/**< Command return status */
	int32_t callback_status_ptr;
	/**< Pointer to Callback function return status */

};

/**************************************************************************//**
@Description	Virtual Pools
				vpool_create_pool
				verification command Structure.
				Actual command:
				int32_t vpool_create_pool(
					uint16_t bman_pool_id,
					int32_t max_bufs,
					int32_t committed_bufs,
					uint32_t flags,
					int32_t (*callback_func)(uint64_t),
					uint32_t *virtual_pool_id
					);

*//***************************************************************************/
struct vpool_create_pool_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint16_t bman_pool_id;
		/**< BMAN pool ID */
	uint16_t pad1;
		/**< Padding */
	int32_t max_bufs;
		/**< Maximum number of buffers in this pool */
	int32_t committed_bufs;
		/**< Committed number of buffers in this pool */
	uint32_t flags;
		/**< Control flags */
	uint32_t callback_func;
		/**< Reference to a callback function */
	uint32_t virtual_pool_id_ptr;
		/**< Virtual pool ID returned value */
	int32_t status;
		/**< Command return status */
};

/**************************************************************************//**
@Description	Virtual Pools
				vpool_release_pool
				verification command Structure.
				Actual command:
				int32_t vpool_release_pool(uint32_t virtual_pool_id);

*//***************************************************************************/
struct vpool_release_pool_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint32_t virtual_pool_id_ptr;
		/**< Virtual pool ID */
	int32_t status;
		/**< Command return status */
	uint32_t pad;
		/**< Padding to 64 bit boundary */
};


/**************************************************************************//**
@Description	Virtual Pools
				vpool_read_pool
				verification command Structure.
				Actual command:
				int32_t vpool_read_pool(uint32_t virtual_pool_id,
					uint16_t *bman_pool_id,
					int32_t *max_bufs,
					int32_t *committed_bufs,
					int32_t *allocated_bufs,
					uint32_t *flags,
					int32_t *callback_func
					);
*//***************************************************************************/
struct vpool_read_pool_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint32_t virtual_pool_id_ptr;
		/**< Virtual pool ID */
	uint16_t bman_pool_id;
		/**< BMAN pool ID */
	uint16_t pad1;
		/**< Padding */
	int32_t max_bufs;
		/**< Maximum number of buffers in this pool */
	int32_t committed_bufs;
		/**< Committed number of buffers in this pool */
	int32_t allocated_bufs;
		/**< Number of current allocated buffers from this pool */
	uint32_t flags;
		/**< Control flags */
	int32_t callback_func;
		/**< Reference to a callback function */
	int32_t status;
		/**< Command return status */
	uint32_t pad2;
		/**< Padding */
};

/**************************************************************************//**
@Description	Virtual Pools
				vpool_init
				verification command Structure.
				Actual command:
				int32_t vpool_init(
					uint64_t *virtual_pool_struct,
					uint64_t *callback_func_struct,
					uint32_t num_of_virtual_pools,
					uint32_t flags
					);

*//***************************************************************************/
struct vpool_init_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint32_t pad1;
		/**< Padding */
	//uint64_t virtual_pool_struct;
		/**< Reference to the head of the virtual pools structure. */
	//uint64_t callback_func_struct;
		/**< Reference to the head of the callback functions structure. */
	uint32_t num_of_virtual_pools;
		/**< Number of virtual pools */
	uint32_t flags;
		/**< Control flags */
	int32_t status;
		/**< Command return status */
	uint32_t pad2;
		/**< Padding */
};

/**************************************************************************//**
@Description	Virtual Pools
				vpool_init_total_bman_bufs
				verification command Structure.
				Actual command:
				int32_t vpool_init_total_bman_bufs(
					uint16_t bman_pool_id,
					int32_t total_avail_bufs
					);

*//***************************************************************************/
struct vpool_init_total_bman_bufs_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint16_t bman_pool_id;
		/**< BMAN pool ID */
	uint16_t pad1;
		/**< Padding */
	int32_t total_avail_bufs;
		/**< Total number of buffers in this BMAN pool */
	int32_t status;
		/**< Command return status */
	uint32_t pad2;
		/**< Padding to 64 bit boundary */
};

/**************************************************************************//**
@Description	Virtual Pools
				vpool_add_total_bman_bufs
				verification command Structure.
				Actual command:
				int32_t vpool_add_total_bman_bufs(
					uint16_t bman_pool_id,
					int32_t additional_bufs);

*//***************************************************************************/
struct vpool_add_total_bman_bufs_cmd {
	uint32_t opcode;
		/**< Command Structure identifier. */
	uint16_t bman_pool_id;
		/**< BMAN pool ID */
	uint16_t pad1;
		/**< Padding */
	int32_t additional_bufs;
		/**< Number of buffers to add to the total of this BMAN pool */
	int32_t status;
		/**< Command return status */
};

/** @} */ /* end of Virtual_Pools_Verification */

/** @}*/ /* end of Service_Routines_Verification */

int32_t dummy_callback (uint64_t context_address);

uint16_t verification_virtual_pools(uint32_t asa_seg_addr);

#endif /* __VERIFICATION_VIRTUAL_POOLS_H_ */
