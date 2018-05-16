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

/**************************************************************************//**
@File		fdma_inline.h

@Description	This file contains the AIOP SW Frame Operations Inline API
		implementation.

*//***************************************************************************/

#ifndef __FSL_FDMA_INLINE_H
#define __FSL_FDMA_INLINE_H

#include "fsl_fdma.h"
#include "fdma.h"
#include "fsl_osm.h"


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

inline void fdma_dma_data(
		uint16_t copy_size,
		uint16_t icid,
		void *ws_addr,
		uint64_t sys_addr,
		uint32_t flags)
{
	/* command parameters and results */
	uint32_t arg1, arg2;
	int8_t res1;

	/* prepare command parameters */
	arg1 = FDMA_DMA_CMD_ARG1(icid, flags);
	arg2 = FDMA_DMA_CMD_ARG2(copy_size, (uint32_t)ws_addr);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__llstdw(sys_addr, HWC_ACC_IN_ADDRESS3, 0);
	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FPDMA_ACCEL_ID)) == FDMA_SUCCESS)
		return;

	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	fdma_exception_handler(FDMA_DMA_DATA, __LINE__, (int32_t)res1);
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
	arg1 = FDMA_PRESENT_CMD_ARG1(PRC_GET_FRAME_HANDLE(), flags);
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

inline int fdma_store_and_ordered_enqueue_default_frame_qd(
	struct fdma_queueing_destination_params *qdp,
	uint32_t        flags)
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
	if (__e_ordhwacceli_(FODMA_ACCEL_ID, OSM_SCOPE_TRANSITION_TO_EXCL_OP,
			OSM_SCOPE_ID_STAGE_INCREMENT_MASK) == FDMA_SUCCESS) {
		if (flags & FDMA_ENWF_RL_BIT)
			REGISTER_OSM_CONCURRENT;
		else
			REGISTER_OSM_EXCLUSIVE;
		return SUCCESS;
	}

	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	if (res1 == FDMA_ENQUEUE_FAILED_ERR) {
		/* Enqueue rejection will still relinquish */
		if (flags & FDMA_ENWF_RL_BIT)
			REGISTER_OSM_CONCURRENT;
		else
			REGISTER_OSM_EXCLUSIVE;
		return -EBUSY;
	} else if (res1 == FDMA_BUFFER_POOL_DEPLETION_ERR) {
		/* No relinquish on buffer pool depletion */
		REGISTER_OSM_EXCLUSIVE;
		return -ENOMEM;
	} else
		fdma_exception_handler(
				FDMA_STORE_AND_ORDERED_ENQUEUE_DEFAULT_FRAME_QD,
				__LINE__, (int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_prestore_and_ordered_enqueue_default_fd_qd(
		uint32_t flags,
		struct fdma_queueing_destination_params *enqueue_params)
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t res1;
	uint32_t amq;
	uint8_t spid = *((uint8_t *) HWC_SPID_ADDRESS);

	/* prepare command parameters */
	arg1 = FDMA_STORE_DEFAULT_CMD_ARG1(spid, PRC_GET_FRAME_HANDLE());
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) != FDMA_SUCCESS) {

		/* load command results */
		res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	
		if (res1 == FDMA_BUFFER_POOL_DEPLETION_ERR)
			return -ENOMEM;
		else
			fdma_exception_handler(
					FDMA_PRESTORE_AND_ORDERED_ENQUEUE_DEFAULT_FD_QD,
					__LINE__, (int32_t)res1);
	}
	
	/* Recover AMQ bits from store frame command and use to enqueue FD */
	amq = *(uint32_t *)HWC_ACC_OUT_ADDRESS2;
	
	/* prepare command parameters */
	flags &= ~FDMA_EN_EIS_BIT;
	arg1 = FDMA_ENQUEUE_FRAME_ARG1(flags);
	arg2 = FDMA_ENQUEUE_WF_QD_ARG2(enqueue_params->qd_priority,
			enqueue_params->qd);
	/* BDI and ICID from store cmd */
	arg3 = (amq<<16) | (uint32_t)enqueue_params->qdbin;
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	*((uint32_t *) HWC_ACC_IN_ADDRESS3) = arg3;
	/* call FDMA Accelerator */
	if (__e_ordhwacceli_(FODMA_ACCEL_ID, OSM_SCOPE_TRANSITION_TO_EXCL_OP,
				OSM_SCOPE_ID_STAGE_INCREMENT_MASK) == FDMA_SUCCESS) {
		/* optimal case is terminate but if not then set the OSM state */
			if (flags & FDMA_ENWF_RL_BIT)
				REGISTER_OSM_CONCURRENT;
			else
				REGISTER_OSM_EXCLUSIVE;
			return SUCCESS;
	}

	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	if (res1 == FDMA_ENQUEUE_FAILED_ERR) {
		if (flags & FDMA_ENWF_RL_BIT)
			REGISTER_OSM_CONCURRENT;
		else
			REGISTER_OSM_EXCLUSIVE;
		return -EBUSY;
	} else
		fdma_exception_handler(
				FDMA_PRESTORE_AND_ORDERED_ENQUEUE_DEFAULT_FD_QD,
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

inline int fdma_present_frame_without_segments(
		struct ldpaa_fd *fd,
		uint32_t flags,
		uint16_t icid,
		uint8_t *frame_handle)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("fdma.c", (uint32_t)fd, ALIGNMENT_32B);
#endif
	
	/* command parameters and results */
	uint32_t arg1, arg4;
	int8_t  res1;

	/* prepare command parameters */
	arg1 = FDMA_INIT_CMD_ARG1((uint32_t)fd, (flags | FDMA_INIT_NDS_BIT));

	/* store command parameters */
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;

	if (flags & FDMA_INIT_AS_BIT)
		arg4 = FDMA_INIT_EXP_AMQ_CMD_ARG4(flags, icid, 0, 0);
	else
		arg4 = 0;

	__stdw(PTA_NOT_LOADED_ADDRESS, arg4, HWC_ACC_IN_ADDRESS3, 0);

	/* call FDMA Accelerator */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	if (res1 == FDMA_SUCCESS) {
		*frame_handle = *((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_FRAME_HANDLE_OFFSET));
		if ((uint32_t)fd == HWC_FD_ADDRESS) {
			PRC_SET_FRAME_HANDLE(*frame_handle);
			PRC_SET_NDS_BIT();
		}
		return SUCCESS;
	}
	
	if (res1 == FDMA_FD_ERR)
		return -EIO;
	else
		fdma_exception_handler(FDMA_PRESENT_FRAME_WITHOUT_SEGMENTS, 
					__LINE__, (int32_t)res1);

	return (int32_t)res1;
}

inline int fdma_store_frame_data(
		uint8_t frame_handle,
		uint8_t spid,
		struct fdma_amq *amq)
{
	/* command parameters and results */
	uint32_t arg1;
	uint16_t bdi_icid;
	int8_t res1;
	/* storage profile ID */

	/* prepare command parameters */
	arg1 = FDMA_STORE_CMD_ARG1(spid, frame_handle);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	bdi_icid = *((uint16_t *)
		(HWC_ACC_OUT_ADDRESS2 + FDMA_STORE_CMD_OUT_ICID_OFFSET));
	amq->icid = bdi_icid & ~(FDMA_ICID_CONTEXT_BDI);
	amq->flags =
		(((*((uint16_t *)HWC_ACC_OUT_ADDRESS2)) &
			(FDMA_ICID_CONTEXT_PL | FDMA_ICID_CONTEXT_eVA)) |
		(uint16_t)(bdi_icid & FDMA_ICID_CONTEXT_BDI));

	if (res1 == FDMA_SUCCESS)
		return SUCCESS;
	else if (res1 == FDMA_BUFFER_POOL_DEPLETION_ERR)
		return -ENOMEM;
	else
		fdma_exception_handler(FDMA_STORE_FRAME_DATA, __LINE__, 
				(int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_split_frame(
		struct fdma_split_frame_params *params)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	uint32_t split_frame_length;
	uint32_t res2_sm; /* restore result in case of split by SF bit */
	int8_t  res1;

	/* prepare command parameters */

	arg1 = FDMA_SPLIT_CMD_ARG1(params->spid,
				params->source_frame_handle, params->flags);

	arg2 = FDMA_SPLIT_CMD_ARG2((uint32_t)(params->seg_dst),
			params->seg_offset);
	arg3 = FDMA_SPLIT_CMD_ARG3(params->present_size);
	arg4 = FDMA_SPLIT_CMD_ARG4((uint32_t)(params->fd_dst),
			params->split_size_sf);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);

	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	if ((res1 == FDMA_SUCCESS) ||
		(res1 == FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR) ||
		(res1 == FDMA_BUFFER_POOL_DEPLETION_ERR)) {
		
		params->split_frame_handle = *((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_SEG_HANDLE_OFFSET));
		if (!(params->flags & FDMA_SPLIT_PSA_CLOSE_FRAME_BIT)) {
			if (params->flags & FDMA_SPLIT_SM_BIT) { 
				/* save result of split command */
				res2_sm = *((uint32_t *)HWC_ACC_OUT_ADDRESS2);
				get_frame_length(params->split_frame_handle, 
						&(split_frame_length));
				params->split_size_sf = 
						(uint16_t)split_frame_length;
				/* restore result of split command */
				*((uint32_t *)HWC_ACC_OUT_ADDRESS2) = res2_sm;
			}
			LDPAA_FD_SET_LENGTH((uint32_t)params->fd_dst,
					params->split_size_sf);
		}
		if (params->flags & FDMA_SPLIT_PSA_PRESENT_BIT) {
			params->seg_length = *((uint16_t *)
					(HWC_ACC_OUT_ADDRESS2));
			params->seg_handle = *((uint8_t *)
				(HWC_ACC_OUT_ADDRESS2 +
				FDMA_FRAME_HANDLE_OFFSET));
			/* Update Task Defaults */
			if (((uint32_t)params->fd_dst) == HWC_FD_ADDRESS) {
				prc->seg_address = (uint16_t)
				((uint32_t)params->seg_dst);
				prc->seg_length = params->seg_length;
				prc->seg_offset = params->seg_offset;
				prc->frame_handle = params->split_frame_handle;
				prc->seg_handle = params->seg_handle;
				(params->flags & FDMA_SPLIT_SR_BIT) ?
					PRC_SET_SR_BIT() : PRC_RESET_SR_BIT();
			}	
		}
		/* Update Task Defaults */
		else if ((((uint32_t)params->fd_dst) == HWC_FD_ADDRESS) &&
			!(params->flags & (FDMA_SPLIT_PSA_PRESENT_BIT | 
					FDMA_SPLIT_PSA_CLOSE_FRAME_BIT))) {
				prc->frame_handle = params->split_frame_handle;
		}
		
		if ((((uint32_t)params->fd_dst) != HWC_FD_ADDRESS) && 
		    (params->source_frame_handle ==  PRC_GET_FRAME_HANDLE())) {
			if (params->flags & FDMA_SPLIT_SM_BIT &&
			    !(params->flags & FDMA_SPLIT_PSA_CLOSE_FRAME_BIT)) {
				/* save result of split command */
				res2_sm = *((uint32_t *)HWC_ACC_OUT_ADDRESS2);
				get_frame_length(params->split_frame_handle, 
						&(split_frame_length));
				params->split_size_sf = 
						(uint16_t)split_frame_length;
				/* restore result of split command */
				*((uint32_t *)HWC_ACC_OUT_ADDRESS2) = res2_sm;
			}
			LDPAA_FD_UPDATE_LENGTH(HWC_FD_ADDRESS, 0, 
				params->split_size_sf);
		}

		if ((res1 == FDMA_SUCCESS))
			return SUCCESS;
		else if (res1 == FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR)
			return FDMA_STATUS_UNABLE_PRES_DATA_SEG;
		else	/* FDMA_BUFFER_POOL_DEPLETION_ERR */
			return -ENOMEM;
	}

	if (res1 == FDMA_UNABLE_TO_SPLIT_ERR)
		return -EINVAL;
	else
		fdma_exception_handler(FDMA_SPLIT_FRAME, __LINE__, 
				(int32_t)res1);

	return (int32_t)(res1);
}

inline void get_frame_length(uint8_t frame_handle, uint32_t *length)
{
	/* command parameters and results */
	int8_t res1;
	
	/* prepare + store command parameters */
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = 
			FDMA_GET_FRAME_LENGTH_CMD_ARG1(frame_handle);

	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) == FDMA_SUCCESS)
	{
		*length = *((uint32_t *)HWC_ACC_OUT_ADDRESS2);
		return;
	}

	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));
	
	fdma_exception_handler(FDMA_GET_FRAME_LENGTH, 
			__LINE__, (int32_t)res1);
}

inline int fdma_insert_segment_data(
		struct fdma_insert_segment_data_params *params)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
			(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	int8_t res1;

	/* prepare command parameters */
	params->flags = (params->flags) | FDMA_REPLACE_TAM_FLAG;
	arg1 = FDMA_REPLACE_EXP_CMD_ARG1(params->seg_handle,
			params->frame_handle, params->flags);
	arg2 = FDMA_REPLACE_CMD_ARG2(params->to_offset, 0);
	arg3 = FDMA_REPLACE_CMD_ARG3(params->from_ws_src, params->insert_size);
	if (params->flags & FDMA_REPLACE_SA_REPRESENT_BIT)
		arg4 = FDMA_REPLACE_CMD_ARG4(params->ws_dst_rs,
				params->size_rs);
	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, 0);
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	/* Update Task Defaults */
	if ((int32_t)res1 >= FDMA_SUCCESS) {
		if (params->flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
			params->seg_length_rs = *((uint16_t *)
					(HWC_ACC_OUT_ADDRESS2));
			if (params->seg_handle == PRC_GET_SEGMENT_HANDLE()) {
				prc->seg_address = (uint16_t)(uint32_t)
							params->ws_dst_rs;
				prc->seg_length = *((uint16_t *)
							HWC_ACC_OUT_ADDRESS2);
			}
		}

		if (params->frame_handle == PRC_GET_FRAME_HANDLE())
			/* FD fields should be updated with a swap load/store */
			LDPAA_FD_UPDATE_LENGTH(HWC_FD_ADDRESS,
					params->insert_size, 0);

		if ((params->seg_handle == PRC_GET_SEGMENT_HANDLE()) &&
			(params->flags & FDMA_REPLACE_SA_CLOSE_BIT))
			PRC_SET_NDS_BIT();

		if (res1 == FDMA_SUCCESS)
			return SUCCESS;
		else	/*FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR*/
			return FDMA_STATUS_UNABLE_PRES_DATA_SEG;
	}

	fdma_exception_handler(FDMA_INSERT_SEGMENT_DATA, __LINE__, 
			(int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_present_default_frame_without_segments(void)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t  res1;

	/* prepare command parameters */
	arg1 = FDMA_INIT_CMD_ARG1((uint32_t)HWC_FD_ADDRESS, FDMA_INIT_NDS_BIT);

	/* store command parameters */
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	__stdw(PTA_NOT_LOADED_ADDRESS, 0, HWC_ACC_IN_ADDRESS3, 0);

	/* call FDMA Accelerator */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	if (res1 == FDMA_SUCCESS) {
		PRC_SET_FRAME_HANDLE(*((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_FRAME_HANDLE_OFFSET)));
		PRC_SET_NDS_BIT();

		return SUCCESS;
	}

	if (res1 == FDMA_FD_ERR)
		return -EIO;
	else
		fdma_exception_handler(
				FDMA_PRESENT_DEFAULT_FRAME_WITHOUT_SEGMENTS, 
				__LINE__, (int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_present_default_frame_default_segment()
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t  res1;

	/* prepare command parameters */
	arg1 = FDMA_PRESENT_CMD_ARG1_WITH_NO_FLAGS(PRC_GET_FRAME_HANDLE());
	arg2 = FDMA_PRESENT_CMD_ARG2((uint32_t)PRC_GET_SEGMENT_ADDRESS(),
			PRC_GET_SEGMENT_OFFSET());
	arg3 = FDMA_PRESENT_CMD_ARG3(PRC_GET_SEGMENT_LENGTH());
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS3)) = arg3;

	/* call FDMA Accelerator */
	__e_hwacceli_(FPDMA_ACCEL_ID);

	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	if ((res1 == FDMA_SUCCESS) ||
		(res1 == FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR)) {
		PRC_SET_SEGMENT_LENGTH(*((uint16_t *)(HWC_ACC_OUT_ADDRESS2)));
		PRC_SET_SEGMENT_HANDLE(*((uint8_t *)(HWC_ACC_OUT_ADDRESS2 +
						FDMA_SEG_HANDLE_OFFSET)));
		PRC_RESET_NDS_BIT();
		if (res1 == FDMA_SUCCESS)
			return SUCCESS;
		else	/*FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR*/
			return FDMA_STATUS_UNABLE_PRES_DATA_SEG;
	}

	fdma_exception_handler(FDMA_PRESENT_DEFAULT_FRAME_DEFAULT_SEGMENT, 
			__LINE__, (int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_xon(enum fdma_flow_control_type type, uint32_t id, uint8_t tc)
{
	/* command parameters and results */
	uint32_t	arg1, arg2, arg3;
	uint32_t	bdi, icid;
	int8_t		res1;
	struct additional_dequeue_context *adc;

	adc = (struct additional_dequeue_context *)HWC_ADC_ADDRESS;
	icid = (uint32_t)(LH_SWAP(0, &adc->pl_icid) & ADC_ICID_MASK);
	bdi = (uint32_t)(adc->fdsrc_va_fca_bdi & ADC_BDI_MASK);

	if (type == FLOW_CONTROL_TC) {
		/* prepare command parameters */
		arg1 = FDMA_FLOW_CTRL_ARG1(tc, 0, type);
		arg2 = FDMA_FLOW_CTRL_ARG2(id);
		arg3 = 0;
	} else if (type == FLOW_CONTROL_FQID) {
		/* prepare command parameters */
		arg1 = FDMA_FLOW_CTRL_ARG1(0, 0, type);
		arg2 = FDMA_FLOW_CTRL_ARG2(id);
		arg3 = FDMA_FLOW_CTRL_ARG3(bdi, icid);
	} else {
		return -EINVAL;
	}

	/* store command parameters */
	__stqw(arg1, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) == FDMA_SUCCESS)
		return SUCCESS;
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	fdma_exception_handler(FDMA_FLOW_CONTROL_XON,
			       __LINE__, (int32_t)res1);

	return (int32_t)(res1);
}

inline int fdma_xoff(enum fdma_flow_control_type type, uint32_t id, uint8_t tc)
{
	/* command parameters and results */
	uint32_t	arg1, arg2, arg3;
	uint32_t	bdi, icid;
	int8_t		res1;
	struct additional_dequeue_context *adc;

	adc = (struct additional_dequeue_context *)HWC_ADC_ADDRESS;
	icid = (uint32_t)(LH_SWAP(0, &adc->pl_icid) & ADC_ICID_MASK);
	bdi = (uint32_t)(adc->fdsrc_va_fca_bdi & ADC_BDI_MASK);

	if (type == FLOW_CONTROL_TC) {
		/* prepare command parameters */
		arg1 = FDMA_FLOW_CTRL_ARG1(tc, 1, type);
		arg2 = FDMA_FLOW_CTRL_ARG2(id);
		arg3 = 0;
	} else if (type == FLOW_CONTROL_FQID) {
		/* prepare command parameters */
		arg1 = FDMA_FLOW_CTRL_ARG1(0, 1, type);
		arg2 = FDMA_FLOW_CTRL_ARG2(id);
		arg3 = FDMA_FLOW_CTRL_ARG3(bdi, icid);
	} else {
		return -EINVAL;
	}

	/* store command parameters */
	__stqw(arg1, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);

	/* call FDMA Accelerator */
	if ((__e_hwacceli_(FODMA_ACCEL_ID)) == FDMA_SUCCESS)
		return SUCCESS;
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	fdma_exception_handler(FDMA_FLOW_CONTROL_XOFF,
			       __LINE__, (int32_t)res1);

	return (int32_t)(res1);
}

#endif /* __FSL_FDMA_INLINE_H */
