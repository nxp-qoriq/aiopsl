/**************************************************************************//**
@File		cdma.c

@Description	This file contains the AIOP CDMA Operations API implementation.
*//***************************************************************************/

#include "general.h"
#include "common/types.h"

#include "cdma.h"


int32_t cdma_acquire_context_memory(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0); */
	/*__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0); */
	/*__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	*((uint32_t *)(HWC_ACC_IN_ADDRESS+4)) = arg2;
	*((uint32_t *)(HWC_ACC_IN_ADDRESS+8)) = arg3;  */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_release_context_memory(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_read(
		void *ws_dst,
		uint64_t ext_address,
		uint16_t size) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_READ_CMD_ARG1();
	arg2 = CDMA_READ_CMD_ARG2(size, (uint32_t)ws_dst);
	arg3 = (uint32_t)(ext_address>>32);
	arg4 = (uint32_t)(ext_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_write(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_mutex_lock_take(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_mutex_lock_release(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_read_with_mutex(
		uint64_t ext_address,
		uint32_t flags,
		void *ws_dst,
		uint16_t size) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_READ_WITH_MUTEX_CMD_ARG1(flags);
	arg2 = CDMA_READ_WITH_MUTEX_CMD_ARG2(size, (uint32_t)ws_dst);
	arg3 = (uint32_t)(ext_address>>32);
	arg4 = (uint32_t)(ext_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_write_with_mutex(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_refcount_increment(
		uint64_t context_address) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_REFCNT_INC_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_refcount_decrement(
		uint64_t context_address) {

	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint8_t res1;

	/* prepare command parameters */
	arg1 = CDMA_REFCNT_DEC_CMD_ARG1();
	arg2 = 0;
	arg3 = (uint32_t)(context_address>>32);
	arg4 = (uint32_t)(context_address);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_refcount_decrement_and_release(
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

	return (int32_t)(res1);
}

int32_t cdma_write_lock_dma_read_and_increment(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);

}

int32_t cdma_write_release_lock_and_decrement(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);

}
int32_t cdma_ws_memory_init(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS + 8)) = arg3; */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_access_context_memory(
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
	/* Todo - Note to HW/Compiler team:
	stqw intrinsic can be used here (__stqw with 4 arg is not working).
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__stdw(arg3, arg4, HWC_ACC_IN_ADDRESS + 8, 0); */

	/* call CDMA */
	__e_hwacceli_(CDMA_ACCEL_ID);

	/* load command results */
	res1 = *((uint8_t *)(HWC_ACC_OUT_ADDRESS+CDMA_STATUS_OFFSET));
	*refcount_value = *((uint32_t *)(HWC_ACC_OUT_ADDRESS+
				CDMA_REF_CNT_OFFSET));

	return (int32_t)(res1);
}

int32_t cdma_refcount_get(
		uint64_t context_address,
		uint32_t *refcount_value) {

	/* command parameters and results */
	int32_t res1;

	/* Increment ref counter */
	res1 = cdma_refcount_increment(context_address);

	if (res1 == CDMA_SUCCESS)
		/* Decrement ref counter without release */
		res1 = cdma_refcount_decrement(context_address);

	/* load command results */
	*refcount_value = *((uint32_t *)(HWC_ACC_OUT_ADDRESS+
					CDMA_REF_CNT_OFFSET));

	return (int32_t)(res1);
	}


