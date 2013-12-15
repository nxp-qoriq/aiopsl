/**************************************************************************//**
@File          verification_virtual_pools.c

@Description   	This file contains the AIOP Virtual Pools 
				SW Verification functions call
				
*//***************************************************************************/

#include "aiop_verification.h"
#include "verification_virtual_pools.h"
#include "virtual_pools.h"

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
					str->virtual_pool_id,
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
			str->status = vpool_release_buf(
					str->virtual_pool_id,
					str->context_address_ptr);
			str_size = sizeof(struct vpool_release_buf_cmd);
			break;
		}
		
		/* vpool_refcount_increment Command Verification */
		case VPOOL_REFCOUNT_INCREMENT_CMD:
		{
			struct vpool_refcount_increment_cmd *str =
				(struct vpool_refcount_increment_cmd *)asa_seg_addr;
			str->status = vpool_refcount_increment(
					str->context_address_ptr);
			str_size = sizeof(struct vpool_refcount_increment_cmd);
			break;
		}
		
		/* vpool_refcount_decrement_and_release Command Verification */
		case VPOOL_REFCOUNT_DECREMENT_AND_RELEASE_CMD:
		{
			struct vpool_refcount_decrement_and_release_cmd *str =
				(struct vpool_refcount_decrement_and_release_cmd *)asa_seg_addr;
			str->status = vpool_refcount_decrement_and_release(
					str->virtual_pool_id,
					str->context_address_ptr);
			str_size = sizeof(struct vpool_refcount_decrement_and_release_cmd);
			break;
		}
		
		/* vpool_create_pool Command Verification */
		case VPOOL_CREATE_POOL_CMD:
		{
			struct vpool_create_pool_cmd *str =
				(struct vpool_create_pool_cmd *)asa_seg_addr;
			str->status = vpool_create_pool(
					str->bman_pool_id,
					str->max_bufs,
					str->committed_bufs,
					str->flags,
					(int32_t (*)(uint64_t))str->callback_func,
					&str->virtual_pool_id);
			str_size = sizeof(struct vpool_create_pool_cmd);
			break;
		}
		
		/* vpool_release_pool_cmd Command Verification */
		case VPOOL_RELEASE_POOL_CMD:
		{
			struct vpool_release_pool_cmd *str =
				(struct vpool_release_pool_cmd *)asa_seg_addr;
			str->status = vpool_release_pool(str->virtual_pool_id);
			str_size = sizeof(struct vpool_release_pool_cmd);
			break;
		}
		
		/* vpool_read_pool_cmd Command Verification */
		case VPOOL_READ_POOL_CMD:
		{
			struct vpool_read_pool_cmd *str =
				(struct vpool_read_pool_cmd *)asa_seg_addr;
			str->status = vpool_read_pool(
					str->virtual_pool_id,
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
					str->virtual_pool_struct,
					str->callback_func_struct,
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
					str->total_avail_bufs,
					str->buf_size
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
