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
@File          verification_virtual_pools.c

@Description   	This file contains the AIOP Virtual Pools 
				SW Verification functions call
				
*//***************************************************************************/
/* Virtual Pools Usage Flow Example */
/*
 * VPOOL_INIT_CMD - initialize global structures. 
 *							Supply the max number of pools
 *							Run this command only once.
 *							
 * VPOOL_INIT_TOTAL_BMAN_BUFS_CMD - initialize structures with BMAN pools parameters 
 * 							Run this command for every BMAN pool ID used
 * 							
 * VPOOL_CREATE_POOL_CMD - initialize a virtual pool
 * 							Supply a Workspace address that will be updated with the virtual_pool ID. 
 * 							Supply virtual pool parameters (committed, max, bman ID, etc.)
 * 							Run this command for every virtual pool used
 * 							
 * VPOOL_ALLOCATE_BUF_CMD - allocate a context buffer
 * 							Supply a Workspace address that holds a valid virtual_pool ID
 * 							(allocated by VPOOL_CREATE_POOL_CMD)
 * 							Supply a Workspace address that will be updated with buffer address. 
 * 							Run this command for every buffer allocated 
 * 							 							 								
 * VPOOL_REFCOUNT_INCREMENT_CMD - increment the reference counter of a context buffer
 * 							Supply a Workspace address that holds a valid buffer address.
 * 							(allocated by VPOOL_ALLOCATE_BUF_CMD)
 * 							 
 * VPOOL_REFCOUNT_DECREMENT_AND_RELEASE_CMD - decrement reference counter and release buffer
 * 							if reached zero.
 * 							Supply a Workspace address that holds a valid buffer address.
 * 							Run this command 1 + (number of times VPOOL_REFCOUNT_INCREMENT_CMD
 * 							was called for the same buffer address) times. 
 * 														
 * VPOOL_RELEASE_POOL_CMD - release a virtual pool							
 * 							Supply a Workspace address that holds a valid virtual_pool ID
 * 							Run this command after all buffers in this virtual pool were released. 
 * 							
 */

#include "virtual_pools.h"
#include "aiop_verification.h"
#include "verification_virtual_pools.h"

struct virtual_pool_desc virtual_pools[MAX_VIRTUAL_POOLS_NUM];
struct callback_s callback_functions[MAX_VIRTUAL_POOLS_NUM];

int callback_counter = 0;

int32_t dummy_callback (uint64_t context_address)
{
	uint64_t dummy_addr = context_address;
	callback_counter++;
	return callback_counter;
}

uint16_t verification_virtual_pools(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint32_t flags;

	opcode  = *((uint32_t *) asa_seg_addr);
	flags = 0x0;

	switch (opcode) {
		/* vpool_allocate_buf Command Verification */
		case VPOOL_ALLOCATE_BUF_CMD:
		{
			struct vpool_allocate_buf_cmd *str =
				(struct vpool_allocate_buf_cmd *)asa_seg_addr;
			str->status = vpool_allocate_buf(
					*((uint32_t *)str->virtual_pool_id_ptr),
					(uint64_t *)str->context_address_ptr);
			str->context_address = *((uint64_t *)str->context_address_ptr);
			str_size = sizeof(struct vpool_allocate_buf_cmd);
			break;
		}
				
		/* vpool_release_buf Command Verification */
		case VPOOL_RELEASE_BUF_CMD:
		{
			struct vpool_release_buf_cmd *str =
				(struct vpool_release_buf_cmd *)asa_seg_addr;
			vpool_release_buf(
					*((uint32_t *)str->virtual_pool_id_ptr),
					*((uint64_t *)str->context_address_ptr));
			str->status = 0;
			str_size = sizeof(struct vpool_release_buf_cmd);
			break;
		}
		
		/* vpool_refcount_increment Command Verification */
		case VPOOL_REFCOUNT_INCREMENT_CMD:
		{
			struct vpool_refcount_increment_cmd *str =
				(struct vpool_refcount_increment_cmd *)asa_seg_addr;
			vpool_refcount_increment(
					*((uint64_t *)str->context_address_ptr));
			str->status = 0;
			str_size = sizeof(struct vpool_refcount_increment_cmd);
			break;
		}
		
		/* vpool_refcount_decrement_and_release Command Verification */
		case VPOOL_REFCOUNT_DECREMENT_AND_RELEASE_CMD:
		{
			struct vpool_refcount_decrement_and_release_cmd *str =
				(struct vpool_refcount_decrement_and_release_cmd *)asa_seg_addr;
			str->status = vpool_refcount_decrement_and_release(
					*((uint32_t *)str->virtual_pool_id_ptr),
					*((uint64_t *)str->context_address_ptr),
					(int32_t *)str->callback_status_ptr);

			str_size = sizeof(struct vpool_refcount_decrement_and_release_cmd);
			break;
		}
		
		/* vpool_create_pool Command Verification */
		case VPOOL_CREATE_POOL_CMD:
		{
			struct vpool_create_pool_cmd *str =
				(struct vpool_create_pool_cmd *)asa_seg_addr;
			
			/* If the callback function is not null */
			if (str->callback_func) 
				str->callback_func = (uint32_t)(&dummy_callback);
							
			str->status = vpool_create_pool(
					str->bman_pool_id,
					str->max_bufs,
					str->committed_bufs,
					str->flags,
					(vpool_callback_t *)str->callback_func,
					((uint32_t *)str->virtual_pool_id_ptr)
					);
			str_size = sizeof(struct vpool_create_pool_cmd);
			break;
		}
		
		/* vpool_release_pool_cmd Command Verification */
		case VPOOL_RELEASE_POOL_CMD:
		{
			struct vpool_release_pool_cmd *str =
				(struct vpool_release_pool_cmd *)asa_seg_addr;
			str->status = vpool_release_pool(
					*((uint32_t *)str->virtual_pool_id_ptr)
					);
			str_size = sizeof(struct vpool_release_pool_cmd);
			break;
		}
		
		/* vpool_read_pool_cmd Command Verification */
		case VPOOL_READ_POOL_CMD:
		{
			struct vpool_read_pool_cmd *str =
				(struct vpool_read_pool_cmd *)asa_seg_addr;
			str->status = vpool_read_pool(
					*((uint32_t *)str->virtual_pool_id_ptr),
					&str->bman_pool_id,
					&str->max_bufs,
					&str->committed_bufs,
					&str->allocated_bufs,
					&str->flags,
					&str->callback_func
					);
			str_size = sizeof(struct vpool_read_pool_cmd);
			break;
		}
		
		/* vpool_init_cmd Command Verification */
		case VPOOL_INIT_CMD:
		{
			
			struct vpool_init_cmd *str =
				(struct vpool_init_cmd *)asa_seg_addr;
			str->status = vpool_init(
					//str->virtual_pool_struct,
					(uint64_t)virtual_pools,
					//str->callback_func_struct,
					(uint64_t)callback_functions,
					str->num_of_virtual_pools,
					str->flags
					);
			str_size = sizeof(struct vpool_init_cmd);
			break;
		}
		
		/* vpool_init_total_bman_bufs_cmd Command Verification */
		case VPOOL_INIT_TOTAL_BMAN_BUFS_CMD:
		{
			struct vpool_init_total_bman_bufs_cmd *str =
				(struct vpool_init_total_bman_bufs_cmd *)asa_seg_addr;
			str->status = vpool_init_total_bman_bufs(
					str->bman_pool_id,
					str->total_avail_bufs
					);
			str_size = sizeof(struct vpool_init_total_bman_bufs_cmd);
			break;
		}
		
		/* vpool_add_total_bman_bufs_cmd Command Verification */
		case VPOOL_ADD_TOTAL_BMAN_BUFS_CMD:
		{
			struct vpool_add_total_bman_bufs_cmd *str =
				(struct vpool_add_total_bman_bufs_cmd *)asa_seg_addr;
			str->status = vpool_add_total_bman_bufs(
					str->bman_pool_id,
					str->additional_bufs
					);
			str_size = sizeof(struct vpool_add_total_bman_bufs_cmd);
			break;
		}
	
		default:
		{
			return STR_SIZE_ERR;
		}
	}


	return str_size;
}






