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
@File		cdma_inline.h

@Description	This file contains the AIOP SW CDMA Inline API implementation.

*//***************************************************************************/

#ifndef __CDMA_INLINE_H
#define __CDMA_INLINE_H


#include "cdma.h"
#include "fsl_cdma.h"
#include "general.h"
#include "fsl_types.h"


inline int cdma_acquire_context_memory(
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
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return 0;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) == CDMA_BUFFER_POOL_DEPLETION_ERR)
		return -ENOSPC;
	cdma_exception_handler(CDMA_ACQUIRE_CONTEXT_MEMORY, __LINE__,
					(int32_t)res1);
	return -1;
}

inline void cdma_release_context_memory(
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
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_RELEASE_CONTEXT_MEMORY, __LINE__,(int32_t)res1);
}

inline void cdma_refcount_increment(
		uint64_t context_address) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;
	
#ifdef DISABLE_REF_CNT
	 return;
#endif

	/* prepare command parameters */
	arg1 = CDMA_REFCNT_INC_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_REFCOUNT_INCREMENT, __LINE__, (int32_t)res1);
}

inline int cdma_refcount_decrement(
		uint64_t context_address) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;
	
#ifdef DISABLE_REF_CNT
	 return 0;
#endif

	/* prepare command parameters */
	arg1 = CDMA_REFCNT_DEC_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return 0;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) == CDMA_REFCOUNT_DECREMENT_TO_ZERO)
		return (int32_t)(res1);
	cdma_exception_handler(CDMA_REFCOUNT_DECREMENT, __LINE__,
			(int32_t)res1);
	return -1;
}

inline void cdma_read_with_mutex_cache_wrp(
		uint64_t ext_address,
		uint32_t flags,
		uint32_t arg2) {

	/* command parameters and results */
	uint32_t arg1, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_READ_WITH_MUTEX_CMD_ARG1(flags);
	arg3 = (uint32_t)(ext_address>>32);
	arg4 = (uint32_t)(ext_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_READ_WITH_MUTEX, __LINE__, (int32_t)res1);
}

inline void cdma_read_with_mutex_no_cache(
		uint64_t ext_address,
		uint32_t flags,
		void *ws_dst,
		uint16_t size) {

	uint32_t arg2;
	arg2 = CDMA_READ_WITH_MUTEX_NO_CACHE_CMD_ARG2(size, (uint32_t)ws_dst);
	cdma_read_with_mutex_cache_wrp(ext_address, flags, arg2);
}

inline void cdma_read_with_mutex(
		uint64_t ext_address,
		uint32_t flags,
		void *ws_dst,
		uint16_t size) {

	uint32_t arg2;
	arg2 = CDMA_READ_WITH_MUTEX_CMD_ARG2(size, (uint32_t)ws_dst);
	cdma_read_with_mutex_cache_wrp(ext_address, flags, arg2);
}

inline void cdma_read_cache_wrp(uint64_t ext_address,
		uint32_t arg2) {
	/* command parameters and results */
	uint32_t arg1, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_READ_CMD_ARG1();
	arg3 = (uint32_t)(ext_address>>32);
	arg4 = (uint32_t)(ext_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_READ, __LINE__,(int32_t)res1);
}


inline void cdma_read_with_no_cache(
		void *ws_dst,
		uint64_t ext_address,
		uint16_t size) {

	uint32_t arg2;
	arg2 = CDMA_READ_CMD_NO_CACHE_ARG2(size, (uint32_t)ws_dst);
	cdma_read_cache_wrp(ext_address, arg2);
}

inline void cdma_read(
		void *ws_dst,
		uint64_t ext_address,
		uint16_t size) {

	uint32_t arg2;
	arg2 = CDMA_READ_CMD_ARG2(size, (uint32_t)ws_dst);
	cdma_read_cache_wrp(ext_address, arg2);
}

inline void cdma_write(
		uint64_t ext_address,
		void *ws_src,
		uint16_t size) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_WRITE_CMD_ARG1();
	arg2 = CDMA_WRITE_CMD_ARG2(size, (uint32_t)ws_src);
	arg3 = (uint32_t)(ext_address>>32);
	arg4 = (uint32_t)(ext_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_WRITE, __LINE__,(int32_t)res1);
}

inline void cdma_write_with_mutex(
		uint64_t ext_address,
		uint32_t flags,
		void *ws_src,
		uint16_t size) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_WRITE_WITH_MUTEX_CMD_ARG1(flags);
	arg2 = CDMA_WRITE_WITH_MUTEX_CMD_ARG2(size, (uint32_t)ws_src);
	arg3 = (uint32_t)(ext_address>>32);
	arg4 = (uint32_t)(ext_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_WRITE_WITH_MUTEX, __LINE__,(int32_t)res1);
}


inline void cdma_mutex_lock_take(
		uint64_t mutex_id,
		uint32_t flags) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_EXT_MUTEX_LOCK_TAKE_CMD_ARG1(flags);
	arg2 = 0;
	arg3 = (uint32_t)(mutex_id>>32);
	arg4 = (uint32_t)(mutex_id);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_MUTEX_LOCK_TAKE, __LINE__, (int32_t)res1);
}

inline void cdma_mutex_lock_release(
		uint64_t mutex_id) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_EXT_MUTEX_LOCK_RELEASE_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(mutex_id>>32);
	arg4 = (uint32_t)(mutex_id);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_MUTEX_LOCK_RELEASE, __LINE__, (int32_t)res1);	
}


inline void cdma_ephemeral_reference_take() 
{

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_EPHEMERAL_REF_TAKE_CMD_ARG1();
	arg2 = 0;
	arg3 = 0;
	arg4 = 0;

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_EPHEMERAL_REFERENCE_TAKE, __LINE__, (int32_t)res1);	
}


inline void cdma_ephemeral_reference_release_all() 
{

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_EPHEMERAL_REF_RELEASE_ALL_CMD_ARG1();
	arg2 = 0;
	arg3 = 0;
	arg4 = 0;

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_EPHEMERAL_REFERENCE_RELEASE_ALL, __LINE__, (int32_t)res1);	
}


inline void cdma_ephemeral_reference_sync() 
{

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_EPHEMERAL_REF_SYNC_CMD_ARG1();
	arg2 = 0;
	arg3 = 0;
	arg4 = 0;

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_EPHEMERAL_REFERENCE_SYNC, __LINE__, (int32_t)res1);	
}


inline int cdma_access_context_memory(
		uint64_t context_address,
		uint32_t flags,
		uint16_t offset,
		void *ws_address,
		uint16_t dma_param,
		uint32_t *refcount_value) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_ACCESS_CONTEXT_MEM_CMD_ARG1(offset, flags);
	arg2 = CDMA_ACCESS_CONTEXT_MEM_CMD_ARG2(dma_param,
			(uint32_t)ws_address);
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS) {
		*refcount_value = *((uint32_t *)(HWC_ACC_OUT_ADDRESS+
						CDMA_REF_CNT_OFFSET));
		return 0;
	} 
		
	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	*refcount_value = *((uint32_t *)(HWC_ACC_OUT_ADDRESS+
				CDMA_REF_CNT_OFFSET));

	if (((int32_t)res1) == (CDMA_REFCOUNT_DECREMENT_TO_ZERO))
		return (int32_t)(res1);
	if (((int32_t)res1) == (CDMA_MUTEX_LOCK_FAILED))
		return -EBUSY;
	cdma_exception_handler(CDMA_ACCESS_CONTEXT_MEMORY,__LINE__,
			(int32_t)res1);
	return -1;
}

inline void cdma_ws_memory_init(
		void *ws_dst,
		uint16_t size,
		uint32_t data_pattern) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4 = 0;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_MEMORY_INIT_CMD;
	arg2 = CDMA_MEMORY_INIT_CMD_ARG2(size, (uint32_t)ws_dst);
	arg3 = data_pattern;

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	cdma_exception_handler(CDMA_WS_MEMORY_INIT,__LINE__,(int32_t)res1);
}

inline int cdma_refcount_decrement_and_release(
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
	if ((__e_hwacceli_(CDMA_ACCEL_ID)) == CDMA_SUCCESS)
		return 0;

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	if (((int32_t)res1) == CDMA_REFCOUNT_DECREMENT_TO_ZERO)
		return (int32_t)(res1);
	cdma_exception_handler(CDMA_REFCOUNT_DECREMENT_AND_RELEASE, __LINE__,
			(int32_t)res1);
	return -1;
}

#endif /* __CDMA_INLINE_H */
