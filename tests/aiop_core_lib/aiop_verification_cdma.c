/**************************************************************************//**
@File          aiop_verification_cdma.c

@Description   This file contains the AIOP CDMA SRs SW Verification
*//***************************************************************************/

#include "dplib/fsl_cdma.h"
#include "cdma.h"

#include "aiop_verification.h"
#include "aiop_verification_cdma.h"


uint16_t aiop_verification_cdma(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint32_t flags;


	opcode  = *((uint32_t *) asa_seg_addr);
	flags = 0x0;

	switch (opcode) {
		/* CDMA Acquire Context Command Verification */
		case CDMA_ACQUIRE_CONTEXT_MEM_CMD_STR:
		{
			struct cdma_acquire_context_memory_command *str =
				(struct cdma_acquire_context_memory_command *) asa_seg_addr;
			str->status = (int8_t)cdma_acquire_context_memory(
					str->context_size, str->pool_id, 
					(uint64_t *)str->context_memory);
			str->context_memory_addr = *((uint64_t *)(str->context_memory));
			str_size = sizeof(struct cdma_acquire_context_memory_command);
			break;
		}
		/* CDMA Release Context Command Verification */
		case CDMA_RELEASE_CONTEXT_MEM_CMD_STR:
		{
			struct cdma_release_context_memory_command *str =
				(struct cdma_release_context_memory_command *) asa_seg_addr;
			str->status = (int8_t)cdma_release_context_memory(*((uint64_t *)(str->context_memory)));
			str_size = sizeof(struct cdma_release_context_memory_command);
			break;
		}	
		/* CDMA Read Command Verification */
		case CDMA_READ_CMD_STR:
		{
			struct cdma_read_command *str =
				(struct cdma_read_command *) asa_seg_addr;
			str->status = (int8_t)cdma_read(
					(void *)str->ws_dst, *((uint64_t *)(str->context_memory)), 
					str->size);
			str_size = sizeof(struct cdma_read_command);
			break;
		}
		/* CDMA Write Command Verification */
		case CDMA_WRITE_CMD_STR:
		{
			struct cdma_write_command *str =
				(struct cdma_write_command *) asa_seg_addr;
			str->status = (int8_t)cdma_write(
					*((uint64_t *)(str->context_memory)), (void *)str->ws_src, 
					str->size);
			str_size = sizeof(struct cdma_write_command);
			break;
		}
		/* CDMA Mutex Lock Take Verification */
		case CDMA_MUTEX_LOCK_TAKE_CMD_STR:
		{
			struct cdma_mutex_lock_take_command *str =
				(struct cdma_mutex_lock_take_command *) asa_seg_addr;
			str->status = (int8_t)cdma_mutex_lock_take(
					str->mutex_id, str->flags);
			str_size = sizeof(struct cdma_mutex_lock_take_command);
			break;
		}
		/* CDMA Mutex Lock Release Verification */
		case CDMA_MUTEX_LOCK_RELEASE_CMD_STR:
		{
			struct cdma_mutex_lock_release_command *str =
				(struct cdma_mutex_lock_release_command *) asa_seg_addr;
			str->status = (int8_t)cdma_mutex_lock_release(
					str->mutex_id);
			str_size = sizeof(struct cdma_mutex_lock_release_command);
			break;
		}
		/* CDMA Read with Mutex Command Verification */
		case CDMA_READ_WITH_MUTEX_CMD_STR:
		{
			struct cdma_read_with_mutex_command *str =
				(struct cdma_read_with_mutex_command *) asa_seg_addr;
			str->status = (int8_t)cdma_read_with_mutex(
					*((uint64_t *)(str->context_memory)), str->flags,
					(void *)str->ws_dst, str->size);
			str_size = sizeof(struct cdma_read_with_mutex_command);
			break;
		}
		/* CDMA Write with Mutex Command Verification */
		case CDMA_WRITE_WITH_MUTEX_CMD_STR:
		{
			struct cdma_write_with_mutex_command *str =
				(struct cdma_write_with_mutex_command *) asa_seg_addr;
			str->status = (int8_t)cdma_write_with_mutex(
					*((uint64_t *)(str->context_memory)), str->flags, 
					(void *)str->ws_src, str->size);
			str_size = sizeof(struct cdma_write_with_mutex_command);
			break;
		}
		/* CDMA Refcount Increment Command Verification */
		case CDMA_REFCOUNT_INC_CMD_STR:
		{
			struct cdma_refcount_increment_command *str =
				(struct cdma_refcount_increment_command *) asa_seg_addr;
			str->status = (int8_t)cdma_refcount_increment(
					*((uint64_t *)(str->context_memory)));
			str_size = sizeof(struct cdma_refcount_increment_command);
			break;
		}
		/* CDMA Refcount Decrement Command Verification */
		case CDMA_REFCOUNT_DEC_CMD_STR:
		{
			struct cdma_refcount_decrement_command *str =
				(struct cdma_refcount_decrement_command *) asa_seg_addr;
			str->status = (int8_t)cdma_refcount_decrement(
					*((uint64_t *)(str->context_memory)));
			str_size = sizeof(struct cdma_refcount_decrement_command);
			break;
		}
		/* CDMA Refcount Decrement Command and Release Verification */
		case CDMA_REFCOUNT_DEC_AND_RELEASE_CMD_STR:
		{
			struct cdma_refcount_decrement_and_release_command *str =
				(struct cdma_refcount_decrement_and_release_command *) asa_seg_addr;
			str->status = (int8_t)cdma_refcount_decrement_and_release(
					*((uint64_t *)(str->context_memory)));
			str_size = sizeof(struct cdma_refcount_decrement_and_release_command);
			break;
		}
		/* CDMA Lock DMA Read and Increment Command Verification */
		case CDMA_LOCK_DMA_READ_AND_INC_CMD_STR:
		{
			struct cdma_write_lock_dma_read_and_increment_command *str =
				(struct cdma_write_lock_dma_read_and_increment_command *) asa_seg_addr;
			str->status = (int8_t)cdma_write_lock_dma_read_and_increment(
					(void *)str->ws_dst, *((uint64_t *)(str->context_memory)),
					str->size);
			str_size = sizeof(struct cdma_write_lock_dma_read_and_increment_command);
			break;
		}
		/* CDMA Write Release Lock and Decrement Command Verification */
		case CDMA_WRITE_RELEASE_LOCK_AND_DEC_CMD_STR:
		{
			struct cdma_write_release_lock_and_decrement_command *str =
				(struct cdma_write_release_lock_and_decrement_command *) asa_seg_addr;
			str->status = (int8_t)cdma_write_release_lock_and_decrement(
					*((uint64_t *)(str->context_memory)), (void *)str->ws_src,
					str->size);
			str_size = sizeof(struct cdma_write_release_lock_and_decrement_command);
			break;
		}
		/* CDMA memory init Command Verification */
		case CDMA_WS_MEM_INIT_CMD_STR:
		{
			struct cdma_ws_memory_init_command *str =
				(struct cdma_ws_memory_init_command *) asa_seg_addr;
			str->status = (int8_t)cdma_ws_memory_init(
					(void *)str->ws_dst, str->size, 
					str->data_pattern);
			str_size = sizeof(struct cdma_ws_memory_init_command);
			break;
		}
		/* CDMA access context memory Command Verification */
		case CDMA_ACCESS_CONTEXT_MEM_CMD_STR:
		{
			struct cdma_access_context_memory_command *str =
				(struct cdma_access_context_memory_command *) asa_seg_addr;
			str->status = (int8_t)cdma_access_context_memory(
					*((uint64_t *)(str->context_memory)), str->flags, 
					str->offset, (void *)str->ws_address, 
					str->dma_param, &(str->refcount_value));
			str_size = sizeof(struct cdma_access_context_memory_command);
			break;
		}
		default:
		{
			return STR_SIZE_ERR;
		}
	}


	return str_size;
}
