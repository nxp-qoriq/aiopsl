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
@File		fdma_inline.h

@Description	This file contains the AIOP SW Frame Operations Inline API
		implementation.

*//***************************************************************************/

#ifndef __FSL_FDMA_INLINE_H
#define __FSL_FDMA_INLINE_H

#include "fsl_fdma.h"
#include "fdma.h"


inline int fdma_replace_default_segment_data(
		uint16_t to_offset,
		uint16_t to_size,
		void	 *from_ws_src,
		uint16_t from_size,
		void	 *ws_dst_rs,
		uint16_t size_rs,
		uint32_t flags)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
			(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	int8_t res1;
	
	/* prepare command parameters */
	flags = flags | FDMA_REPLACE_TAM_FLAG;
	arg1 = FDMA_REPLACE_CMD_ARG1(prc->frame_handle, prc->seg_handle, flags);
	arg4 = FDMA_REPLACE_CMD_ARG4(ws_dst_rs, size_rs);

	arg2 = FDMA_REPLACE_CMD_ARG2(to_offset, to_size);
	arg3 = FDMA_REPLACE_CMD_ARG3(from_ws_src, from_size);
	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));
	
	/* Update Task Defaults */
	if ((int32_t)res1 >= FDMA_SUCCESS) {
		if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
			prc->seg_address = (uint16_t)(uint32_t)ws_dst_rs;
			prc->seg_length = *((uint16_t *)HWC_ACC_OUT_ADDRESS2);
		}
		/* FD fields should be updated with a swap load/store */
		if (from_size != to_size)
			LDPAA_FD_UPDATE_LENGTH(HWC_FD_ADDRESS,
					from_size, to_size);

		if (flags & FDMA_REPLACE_SA_CLOSE_BIT)
			PRC_SET_NDS_BIT();

		if (res1 == FDMA_SUCCESS)
			return SUCCESS;
		else	/*FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR*/
			return FDMA_STATUS_UNABLE_PRES_DATA_SEG;
	}

	fdma_exception_handler(FDMA_REPLACE_DEFAULT_SEGMENT_DATA, __LINE__, 
					(int32_t)res1);

	return (int32_t)(res1);
}

inline void fdma_copy_data(
		uint16_t copy_size,
		uint32_t flags,
		void *src,
		void *dst)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;

	/* prepare command parameters */
	arg1 = FDMA_COPY_CMD_ARG1(copy_size, flags);
	/* store command parameters */
	__stdw(arg1, (uint32_t)src, HWC_ACC_IN_ADDRESS, 0);
	*((uint32_t *) HWC_ACC_IN_ADDRESS3) = (uint32_t)dst;
	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FPDMA_ACCEL_ID)) == FDMA_SUCCESS)
		return;
	
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	fdma_exception_handler(FDMA_COPY_DATA, __LINE__, (int32_t)res1);
}

inline int fdma_store_default_frame_data(void)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	/* storage profile ID */
	uint8_t spid = *((uint8_t *) HWC_SPID_ADDRESS);

	/* prepare command parameters */
	arg1 = FDMA_STORE_DEFAULT_CMD_ARG1(spid, PRC_GET_FRAME_HANDLE());
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) == FDMA_SUCCESS)
		return SUCCESS;

	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	if (res1 == FDMA_BUFFER_POOL_DEPLETION_ERR)
		return -ENOMEM;
	else
		fdma_exception_handler(FDMA_STORE_DEFAULT_FRAME_DATA, __LINE__, 
				(int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_present_default_frame(void)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t  res1;
	uint32_t flags;

	/* prepare command parameters */
	flags = FDMA_INIT_NAS_BIT | FDMA_INIT_NPS_BIT;
	if (PRC_GET_NDS_BIT()){
		flags |= FDMA_INIT_NDS_BIT;
		flags |= PRC_GET_SR_BIT_FDMA_CMD();
		arg1 = FDMA_INIT_CMD_ARG1(HWC_FD_ADDRESS, flags);
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	} else {
		arg1 = FDMA_INIT_CMD_ARG1(HWC_FD_ADDRESS,
				(flags | PRC_GET_SR_BIT_FDMA_CMD()));
		arg2 = FDMA_INIT_CMD_ARG2((uint32_t)(prc->seg_address),
						prc->seg_offset);
		arg3 = FDMA_INIT_CMD_ARG3(prc->seg_length, 0);
		/* store command parameters */
		__stqw(arg1, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);
	}

	/* call FDMA Accelerator */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	if ((res1 == FDMA_SUCCESS) ||
		(res1 == FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR)) {
		prc->seg_length = *((uint16_t *) (HWC_ACC_OUT_ADDRESS2));
		prc->frame_handle = *((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_FRAME_HANDLE_OFFSET));
		prc->seg_handle = *((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_SEG_HANDLE_OFFSET));

		if (res1 == FDMA_SUCCESS)
			return SUCCESS;
		else /* FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR */
			return (int32_t)res1;
	}

	if (res1 == FDMA_FD_ERR)
		return -EIO;
	else
		fdma_exception_handler(FDMA_PRESENT_DEFAULT_FRAME, __LINE__, 
				(int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_insert_default_segment_data(
		uint16_t to_offset,
		void	 *from_ws_src,
		uint16_t insert_size,
		uint32_t flags)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
			(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint16_t seg_size_rs;
	void	 *ws_address_rs;
	int8_t res1;

	/* prepare command parameters */
	flags = flags | FDMA_REPLACE_TAM_FLAG;
	arg1 = FDMA_REPLACE_CMD_ARG1(prc->frame_handle, prc->seg_handle, flags);
	arg2 = FDMA_REPLACE_CMD_ARG2(to_offset, 0);
	arg3 = FDMA_REPLACE_CMD_ARG3(from_ws_src, insert_size);
	if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
		ws_address_rs = (void *) PRC_GET_SEGMENT_ADDRESS();
		seg_size_rs = PRC_GET_SEGMENT_LENGTH();
		arg4 = FDMA_REPLACE_CMD_ARG4(ws_address_rs, seg_size_rs);
	}
	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	/* Update Task Defaults */
	if ((int32_t)res1 >= FDMA_SUCCESS) {
		if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
			prc->seg_address = (uint16_t)(uint32_t)ws_address_rs;
			prc->seg_length = *((uint16_t *)HWC_ACC_OUT_ADDRESS2);
		}
		/* FD fields should be updated with a swap load/store */
		LDPAA_FD_UPDATE_LENGTH(HWC_FD_ADDRESS, insert_size, 0);

		if (flags & FDMA_REPLACE_SA_CLOSE_BIT)
			PRC_SET_NDS_BIT();

		if (res1 == FDMA_SUCCESS)
			return SUCCESS;
		else	/*FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR*/
			return FDMA_STATUS_UNABLE_PRES_DATA_SEG;
	}

	fdma_exception_handler(FDMA_INSERT_DEFAULT_SEGMENT_DATA, __LINE__, 
				(int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_present_default_frame_segment(
		uint32_t flags,
		void	 *ws_dst,
		uint16_t offset,
		uint16_t present_size)
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t  res1;

	/* prepare command parameters */
	arg1 = FDMA_PRESENT_CMD_ARG1(PRC_GET_FRAME_HANDLE(), 
			(flags | FDMA_ST_DATA_SEGMENT_BIT));
	arg2 = FDMA_PRESENT_CMD_ARG2((uint32_t)ws_dst, offset);
	arg3 = FDMA_PRESENT_CMD_ARG3(present_size);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS3)) = arg3;

	/* call FDMA Accelerator */
       __e_hwacceli_(FPDMA_ACCEL_ID);
	
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	if ((res1 == FDMA_SUCCESS) ||
		(res1 == FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR)) {
		PRC_SET_SEGMENT_ADDRESS((uint16_t)(uint32_t)ws_dst);
		PRC_SET_SEGMENT_OFFSET(offset);
		PRC_SET_SEGMENT_LENGTH(*((uint16_t *)(HWC_ACC_OUT_ADDRESS2)));
		PRC_SET_SEGMENT_HANDLE(*((uint8_t *)(HWC_ACC_OUT_ADDRESS2 +
						FDMA_SEG_HANDLE_OFFSET)));
		PRC_RESET_NDS_BIT();
		if (flags & FDMA_PRES_SR_BIT)
			PRC_SET_SR_BIT();
		else
			PRC_RESET_SR_BIT();

		if (res1 == FDMA_SUCCESS)
			return SUCCESS;
		else	/*FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR*/
			return FDMA_STATUS_UNABLE_PRES_DATA_SEG;
	}

	fdma_exception_handler(FDMA_PRESENT_DEFAULT_FRAME_SEGMENT, 
				__LINE__, (int32_t)res1);

	return (int32_t)(res1);
}

inline void fdma_modify_default_segment_data(
		uint16_t offset,
		uint16_t size)
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t res1;

	/* prepare command parameters */
	arg1 = FDMA_REPLACE_CMD_ARG1(
			PRC_GET_FRAME_HANDLE(), PRC_GET_SEGMENT_HANDLE(), 
			FDMA_REPLACE_TAM_FLAG);

	arg2 = FDMA_REPLACE_CMD_ARG2(offset, size);
	arg3 = FDMA_REPLACE_CMD_ARG3(
			(PRC_GET_SEGMENT_ADDRESS() + offset), size);
	/* store command parameters */
	__stqw(arg1, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);
	/* call FDMA Accelerator */
       __e_hwacceli_(FODMA_ACCEL_ID);

	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	if (res1 != FDMA_SUCCESS)
		fdma_exception_handler(FDMA_MODIFY_DEFAULT_SEGMENT_DATA, 
				__LINE__, (int32_t)res1);
	
}

inline void fdma_modify_default_segment_full_data()
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t res1;

	/* prepare command parameters */
	arg1 = FDMA_REPLACE_CMD_ARG1(
			PRC_GET_FRAME_HANDLE(), PRC_GET_SEGMENT_HANDLE(), 
			FDMA_REPLACE_TAM_FLAG);
	arg2 = FDMA_REPLACE_CMD_ARG2(0, PRC_GET_SEGMENT_LENGTH());
	arg3 = FDMA_REPLACE_CMD_ARG3(
			(PRC_GET_SEGMENT_ADDRESS()), PRC_GET_SEGMENT_LENGTH());
	/* store command parameters */
	__stqw(arg1, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);

	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	if (res1 != FDMA_SUCCESS)
		fdma_exception_handler(FDMA_MODIFY_DEFAULT_SEGMENT_FULL_DATA, 
				__LINE__, (int32_t)res1);
	
}

inline int fdma_discard_fd(struct ldpaa_fd *fd, uint16_t icid, uint32_t flags)
{
	/* command parameters and results */
	uint32_t arg1, arg3;
	int8_t res1;
	
	/* prepare command parameters */
	flags |= FDMA_DIS_FS_FD_BIT;
	arg1 = FDMA_DISCARD_ARG1_FRAME((uint32_t)fd, flags);
	if (!(flags & FDMA_DIS_AS_BIT))
		arg3 = FDMA_DISCARD_ARG3_FRAME(flags, icid);
	/* store command parameters */
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	*((uint32_t *)(HWC_ACC_IN_ADDRESS3)) = arg3;
	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) == FDMA_SUCCESS)
		return FDMA_SUCCESS;
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	if (res1 == FDMA_FD_ERR)
		return -EIO;
	else
		fdma_exception_handler(FDMA_DISCARD_FRAME, __LINE__, 
				(int32_t)res1);
	
	return FDMA_SUCCESS;
}

inline void fdma_close_default_segment(void)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	uint8_t seg_handle;

	seg_handle = PRC_GET_SEGMENT_HANDLE();

	/* prepare command parameters */
	arg1 = FDMA_CLOSE_SEG_CMD_ARG1(PRC_GET_FRAME_HANDLE(), seg_handle);
	/* store command parameters */
	__stdw(arg1, 0, HWC_ACC_IN_ADDRESS, 0);
	*((uint32_t *) HWC_ACC_IN_ADDRESS3) = 0;
	/* call FDMA Accelerator */
       __e_hwacceli_(FODMA_ACCEL_ID);

	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	if (res1 == FDMA_SUCCESS)
		PRC_SET_NDS_BIT();
	else
		fdma_exception_handler(FDMA_CLOSE_DEFAULT_SEGMENT, __LINE__, 
				(int32_t)res1);
}

inline int fdma_delete_default_segment_data(
		uint16_t to_offset,
		uint16_t delete_target_size,
		uint32_t flags)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
			(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	void	 *ws_address_rs;
	uint16_t size_rs;
	int8_t res1;

	/* prepare command parameters */
	flags = flags | FDMA_REPLACE_TAM_FLAG;
	arg1 = FDMA_REPLACE_CMD_ARG1(prc->frame_handle, prc->seg_handle, flags);
	arg2 = FDMA_REPLACE_CMD_ARG2(to_offset, delete_target_size);
	arg3 = FDMA_REPLACE_CMD_ARG3(0, 0);
	if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
		ws_address_rs = (void *)PRC_GET_SEGMENT_ADDRESS();
		size_rs = PRC_GET_SEGMENT_LENGTH();
		arg4 = FDMA_REPLACE_CMD_ARG4(ws_address_rs, size_rs);
	}

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);

	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));
	
	/* Update Task Defaults */
	if ((int32_t)res1 >= FDMA_SUCCESS) {
		if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
			prc->seg_address = (uint16_t)(uint32_t)ws_address_rs;
			prc->seg_length = *((uint16_t *)HWC_ACC_OUT_ADDRESS2);
		}
		/* FD fields should be updated with a swap load/store */
		LDPAA_FD_UPDATE_LENGTH(HWC_FD_ADDRESS, 0,
				delete_target_size);

		if (flags & FDMA_REPLACE_SA_CLOSE_BIT)
			PRC_SET_NDS_BIT();

		if (res1 == FDMA_SUCCESS)
			return SUCCESS;
		else	/*FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR*/
			return FDMA_STATUS_UNABLE_PRES_DATA_SEG;
	}

	fdma_exception_handler(FDMA_DELETE_DEFAULT_SEGMENT_DATA, __LINE__, 
				(int32_t)res1);

	return (int32_t)(res1);
}

inline void fdma_discard_default_frame(uint32_t flags)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	/* prepare command parameters */
	arg1 = FDMA_DISCARD_ARG1_WF(PRC_GET_FRAME_HANDLE(), flags);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) == FDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	fdma_exception_handler(FDMA_DISCARD_DEFAULT_FRAME, __LINE__, 
			(int32_t)res1);
}

inline void fdma_calculate_default_frame_checksum(
		uint16_t offset,
		uint16_t size,
		uint16_t *checksum)
{
	/* command parameters and results */
	uint32_t arg1, arg2;
	int8_t res1;

	/* prepare command parameters */
	arg1 = FDMA_CKS_CMD_ARG1(PRC_GET_FRAME_HANDLE());
	arg2 = FDMA_CKS_CMD_ARG2(offset, size);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) == FDMA_SUCCESS) {
		*checksum = *((uint16_t *)(HWC_ACC_OUT_ADDRESS2+FDMA_CHECKSUM_OFFSET));
		return;
	}

	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));
	fdma_exception_handler(FDMA_CALCULATE_DEFAULT_FRAME_CHECKSUM, 
				__LINE__, (int32_t)res1);
}

inline int fdma_store_and_enqueue_default_frame_qd(
		struct fdma_queueing_destination_params *qdp,
		uint32_t	flags)
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t res1;
	/* storage profile ID */
	uint8_t spid = *((uint8_t *) HWC_SPID_ADDRESS);

	/* prepare command parameters */
	flags &= ~FDMA_EN_EIS_BIT;
	arg1 = FDMA_ENQUEUE_WF_ARG1(spid, PRC_GET_FRAME_HANDLE(), flags);
	arg2 = FDMA_ENQUEUE_WF_QD_ARG2(qdp->qd_priority, qdp->qd);
	arg3 = FDMA_ENQUEUE_WF_QD_ARG3(qdp->qdbin);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS3)) = arg3;
	/*__stqw(arg1, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);*/

	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) == FDMA_SUCCESS)
		return SUCCESS;
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	if (res1 == FDMA_ENQUEUE_FAILED_ERR)
		return -EBUSY;
	else if (res1 == FDMA_BUFFER_POOL_DEPLETION_ERR)
		return -ENOMEM;
	else
		fdma_exception_handler(FDMA_STORE_AND_ENQUEUE_DEFAULT_FRAME_QD, 
					__LINE__, (int32_t)res1);

	return (int32_t)(res1);
}

inline void fdma_terminate_task(void)
{
	/* command parameters and results */
	uint32_t arg1;
	/* prepare command parameters */
	arg1 = FDMA_TERM_TASK_CMD_ARG1();
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
}

#endif /* __FSL_FDMA_INLINE_H */
