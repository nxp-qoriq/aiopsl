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
@File		cdma.c

@Description	This file contains the AIOP CDMA Operations API implementation.
*//***************************************************************************/

#include "general.h"
#include "common/types.h"

#include "dplib/fsl_cdma.h"


int cdma_acquire_context_memory(
		uint16_t pool_id,
		uint64_t *context_memory) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3 = 0, arg4 = 0;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_ACQUIRE_CONTEXT_MEM_CMD;
	arg2 = CDMA_ACQUIRE_CONTEXT_MEM_CMD_ARG2((uint32_t)context_memory ,
			pool_id);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) == CDMA_SUCCESS)
		return 0;
	if (((int32_t)res1) == CDMA_BUFFER_POOL_DEPLETION_ERR)
		return -ENOSPC;
	cdma_exception_handler(CDMA_ACQUIRE_CONTEXT_MEMORY, __LINE__,
					(int32_t)res1);
	return -1;
}

void cdma_release_context_memory(
		uint64_t context_address) {

	/* command parameters and results */
	uint32_t arg1, arg2 = 0, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_RELEASE_CONTEXT_MEM_CMD;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) != CDMA_SUCCESS)
		cdma_exception_handler(CDMA_RELEASE_CONTEXT_MEMORY, __LINE__,
						(int32_t)res1);
}

/*void cdma_write(
		uint64_t ext_address,
		void *ws_src,
		uint16_t size) {

	 command parameters and results 
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	 prepare command parameters 
	arg1 = CDMA_WRITE_CMD_ARG1();
	arg2 = CDMA_WRITE_CMD_ARG2(size, (uint32_t)ws_src);
	arg3 = (uint32_t)(ext_address>>32);
	arg4 = (uint32_t)(ext_address);

	 store command parameters 
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	 call CDMA 
	__e_hwacceli_(CDMA_ACCEL_ID);

	 load command results 
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) != CDMA_SUCCESS)
		cdma_exception_handler(CDMA_WRITE, __LINE__,(int32_t)res1);
}*/

/*
void cdma_write_with_mutex(
		uint64_t ext_address,
		uint32_t flags,
		void *ws_src,
		uint16_t size) {

	 command parameters and results 
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	 prepare command parameters 
	arg1 = CDMA_WRITE_WITH_MUTEX_CMD_ARG1(flags);
	arg2 = CDMA_WRITE_WITH_MUTEX_CMD_ARG2(size, (uint32_t)ws_src);
	arg3 = (uint32_t)(ext_address>>32);
	arg4 = (uint32_t)(ext_address);

	 store command parameters 
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	 call CDMA 
	__e_hwacceli_(CDMA_ACCEL_ID);

	 load command results 
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) != CDMA_SUCCESS)
		cdma_exception_handler(CDMA_WRITE_WITH_MUTEX, __LINE__,
				(int32_t)res1);
}
*/

/*void cdma_refcount_increment(
		uint64_t context_address) {

	 command parameters and results 
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	 prepare command parameters 
	arg1 = CDMA_REFCNT_INC_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	 store command parameters 
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	 call CDMA 
	__e_hwacceli_(CDMA_ACCEL_ID);

	 load command results 
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) != CDMA_SUCCESS)
		cdma_exception_handler(CDMA_REFCOUNT_INCREMENT, __LINE__,
				(int32_t)res1);
}*/

/*int cdma_refcount_decrement(
		uint64_t context_address) {

	 command parameters and results 
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	 prepare command parameters 
	arg1 = CDMA_REFCNT_DEC_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	 store command parameters 
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	 call CDMA 
	__e_hwacceli_(CDMA_ACCEL_ID);

	 load command results 
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));


	if (((int32_t)res1) == CDMA_SUCCESS)
		return 0;
	if (((int32_t)res1) == CDMA_REFCOUNT_DECREMENT_TO_ZERO)
		return (int32_t)(res1);
	cdma_exception_handler(CDMA_REFCOUNT_DECREMENT, __LINE__,
			(int32_t)res1);
	return -1;
}*/

int cdma_refcount_decrement_and_release(
		uint64_t context_address) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_REFCNT_DEC_REL_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) == CDMA_SUCCESS)
		return 0;
	if (((int32_t)res1) == CDMA_REFCOUNT_DECREMENT_TO_ZERO)
		return (int32_t)(res1);
	cdma_exception_handler(CDMA_REFCOUNT_DECREMENT_AND_RELEASE, __LINE__,
			(int32_t)res1);
	return -1;
}

void cdma_write_lock_dma_read_and_increment(
		void *ws_dst,
		uint64_t context_address,
		uint16_t size){

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_ARG1();
	arg2 = CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_ARG2(size,
			(uint32_t)ws_dst);
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) != CDMA_SUCCESS)
		cdma_exception_handler(CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT,
				__LINE__,(int32_t)res1);
}

int cdma_write_release_lock_and_decrement(
		uint64_t context_address,
		void *ws_src,
		uint16_t size){

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_WRITE_REL_LOCK_AND_DEC_CMD_ARG1();
	arg2 = CDMA_WRITE_REL_LOCK_AND_DEC_CMD_ARG2(size,
			(uint32_t)ws_src);
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) == CDMA_SUCCESS)
		return 0;
	if (((int32_t)res1) == CDMA_REFCOUNT_DECREMENT_TO_ZERO)
		return (int32_t)(res1);
	cdma_exception_handler(CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT,__LINE__,
			(int32_t)res1);
	return -1;
}

/*void cdma_ws_memory_init(
		void *ws_dst,
		uint16_t size,
		uint32_t data_pattern) {

	 command parameters and results 
	uint32_t arg1, arg2, arg3, arg4 = 0;
	uint8_t res1;

	 prepare command parameters 
	arg1 = CDMA_MEMORY_INIT_CMD;
	arg2 = CDMA_MEMORY_INIT_CMD_ARG2(size, (uint32_t)ws_dst);
	arg3 = data_pattern;

	 store command parameters 
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	 call CDMA 
	__e_hwacceli_(CDMA_ACCEL_ID);

	 load command results 
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) != CDMA_SUCCESS)
		cdma_exception_handler(CDMA_WS_MEMORY_INIT,__LINE__,
				(int32_t)res1);
}*/

void cdma_refcount_get(
		uint64_t context_address,
		uint32_t *refcount_value) {

	/* Increment ref counter */
	cdma_refcount_increment(context_address);

	/* Decrement ref counter without release */
	cdma_refcount_decrement(context_address);
	
	/* load command results */
	*refcount_value = *((uint32_t *)(HWC_ACC_OUT_ADDRESS+
					CDMA_REF_CNT_OFFSET));
}

void cdma_read_wrp(void *ws_dst, uint64_t ext_address, uint16_t size)
{
	cdma_read(ws_dst, ext_address, size);
}

void cdma_read_with_mutex_wrp(uint64_t ext_address, uint32_t flags,
				void *ws_dst, uint16_t size)
{
	cdma_read_with_mutex(ext_address, flags, ws_dst, size);
}

int cdma_access_context_memory_wrp(uint64_t context_address,
					uint32_t flags,
					uint16_t offset,
					void *ws_address,
					uint16_t dma_param,
					uint32_t *refcount_value)
{
	return cdma_access_context_memory(context_address,
					   flags,
					   offset,
					   ws_address,
					   dma_param,
					   refcount_value);
}

void cdma_write_wrp(uint64_t ext_address, void *ws_src, uint16_t size)
{
	cdma_write(ext_address, ws_src, size);
}

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

void cdma_exception_handler(enum cdma_function_identifier func_id,
		     uint32_t line,
		     int32_t status)
{
	char *func_name;
	char *err_msg;
	
	/* Translate function ID to function name string */
	switch(func_id) {
	case CDMA_READ:
		func_name = "cdma_read";
		break;
	case CDMA_WRITE:
		func_name = "cdma_write";
		break;
	case CDMA_MUTEX_LOCK_TAKE:
		func_name = "cdma_mutex_lock_take";
		break;
	case CDMA_MUTEX_LOCK_RELEASE:
		func_name = "cdma_mutex_lock_release";
		break;
	case CDMA_READ_WITH_MUTEX:
		func_name = "cdma_read_with_mutex";
		break;
	case CDMA_WRITE_WITH_MUTEX:
		func_name = "cdma_write_with_mutex";
		break;
	case CDMA_WS_MEMORY_INIT:
		func_name = "cdma_ws_memory_init";
		break;
	case CDMA_REFCOUNT_GET:
		func_name = "cdma_refcount_get";
		break;
	case CDMA_ACQUIRE_CONTEXT_MEMORY:
		func_name = "cdma_acquire_context_memory";
		break;
	case CDMA_RELEASE_CONTEXT_MEMORY:
		func_name = "cdma_release_context_memory";
		break;
	case CDMA_REFCOUNT_DECREMENT_AND_RELEASE:
		func_name = "cdma_refcount_decrement_and_release";
		break;
	case CDMA_REFCOUNT_INCREMENT:
		func_name = "cdma_refcount_increment";
		break;
	case CDMA_REFCOUNT_DECREMENT:
		func_name = "cdma_refcount_decrement";
		break;
	case CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT:
		func_name = "cdma_write_lock_dma_read_and_increment";
		break;
	case CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT:
		func_name = "cdma_write_release_lock_and_decrement";
		break;
	case CDMA_ACCESS_CONTEXT_MEMORY:
		func_name = "cdma_access_context_memory";
		break;
	default:
		/* create own exception */
		func_name = "Unknown Function";
	}
	
	/* Translate error ID to error name string */
	switch (status) {
	case CDMA_BUFFER_POOL_DEPLETION_ERR:
		err_msg = "Failed due to buffer pool depletion.\n";
		break;
	case CDMA_REFCOUNT_DECREMENT_TO_ZERO:
		err_msg = "Decrement reference count caused the reference "
				"count to go to zero. (not an error).\n";
		break;
	case CDMA_MUTEX_LOCK_FAILED:
		err_msg = "Mutex lock failed on a Try Lock request.\n";
		break;
	case CDMA_MUTEX_DEPLETION_ERR:
		err_msg = "Mutex lock depletion (max of 4 reached for the "
				"task).\n";
		break;
	case CDMA_INVALID_DMA_COMMAND_ARGS_ERR:
		err_msg = "Invalid DMA command arguments.\n";
		break;
	case CDMA_INVALID_DMA_COMMAND_ERR:
		err_msg = "Invalid DMA command.\n";
		break;
	case CDMA_REFCOUNT_INCREMENT_ERR:
		err_msg = "Increment reference count failed, count is at max "
				"value.\n";
		break;
	case CDMA_REFCOUNT_DECREMENT_ERR:
		err_msg = "Decrement reference count failed, count is at "
				"zero.\n";
		break;
	case CDMA_REFCOUNT_INVALID_OPERATION_ERR:
		err_msg = "Invalid reference count operation, address is not "
				"base address.\n";
		break;
	case CDMA_INVALID_MUTEX_LOCK_REQ_ERR:
		err_msg = "Invalid mutex lock request, the task already has a "
				"mutex on this address\n";
		break;
	case CDMA_INVALID_MUTEX_RELEASE_ERR:
		err_msg = "Invalid mutex lock release, address not found in "
				"active mutex lock list.\n";
		break;
	case CDMA_INTERNAL_MEMORY_ECC_ERR:
		err_msg = "Internal memory ECC uncorrectable ECC error.\n";
		break;
	case CDMA_WORKSPACE_MEMORY_READ_ERR:
		err_msg = "Workspace memory read Error.\n";
		break;
	case CDMA_WORKSPACE_MEMORY_WRITE_ERR:
		err_msg = "Workspace memory write Error.\n";
		break;
	case CDMA_SYSTEM_MEMORY_READ_ERR:
		err_msg = "System memory read error (permission or ECC).\n";
		break;
	case CDMA_SYSTEM_MEMORY_WRITE_ERR:
		err_msg = "System memory write error (permission or ECC).\n";
		break;
	case CDMA_INTERNAL_ERR:
		err_msg = "Internal error (SRU depletion).\n";
		break;
	default:
		err_msg = "Unknown or Invalid status.\n";
	}
	
	exception_handler(__FILE__, func_name, line, err_msg);
}

#pragma pop
