/**************************************************************************//**
@File		fdma.c

@Description	This file contains the AIOP SW Frame Operations API
		implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "dplib/fsl_fdma.h"
#include "fdma.h"


int32_t fdma_present_default_frame(void)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	int8_t  res1;
	uint32_t flags  = 0;

	/* prepare command parameters */
	if (prc->seg_length == 0)
		flags |= FDMA_INIT_NDS_BIT;
	else
		arg2 = FDMA_INIT_CMD_ARG2((uint32_t)(prc->seg_address),
						prc->seg_offset);
	/* Todo - when compiler fixes bug C43 enable the following code
	 * instead of the 3 lines of code afterwards.
	if (prc->asapa_asaps & PRC_ASAPS_MASK)
		arg4 = FDMA_INIT_CMD_ARG4(prc->asapa_asaps & ~PRC_SR_MASK);
	else
		flags |= FDMA_INIT_NAS_BIT;*/
	if (!(prc->asapa_asaps & PRC_ASAPS_MASK))
		flags |= FDMA_INIT_NAS_BIT;
	arg4 = FDMA_INIT_CMD_ARG4(prc->asapa_asaps & ~PRC_SR_MASK);


	if ((prc->ptapa_asapo & PRC_PTAPA_MASK) == PRC_PTA_NOT_LOADED_ADDRESS)
		flags |= FDMA_INIT_NPS_BIT;

	arg1 = FDMA_INIT_CMD_ARG1(HWC_FD_ADDRESS,
			(flags | PRC_GET_SR_BIT_FDMA_CMD()));
	arg3 = FDMA_INIT_CMD_ARG3(prc->seg_length, prc->ptapa_asapo);


	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, ZERO);

	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	if (((int32_t)res1) >= FDMA_SUCCESS) {
		prc->seg_length = *((uint16_t *) (HWC_ACC_OUT_ADDRESS2));
		/* Todo - if NDS/NAS/NPS are added to prc update them */
		prc->handles =
			(((*((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_FRAME_HANDLE_OFFSET))) <<
			PRC_FRAME_HANDLE_BIT_OFFSET) &
			PRC_FRAME_HANDLE_MASK) |
			((*((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_SEG_HANDLE_OFFSET))) &
			PRC_SEGMENT_HANDLE_MASK);
	}

	if (res1 == FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR)
		prc->asapa_asaps = (prc->asapa_asaps & ~PRC_ASAPS_MASK) |
			(uint16_t)(LDPAA_FD_GET_ASAL(HWC_FD_ADDRESS));

	return (int32_t)(res1);
}

int32_t fdma_present_frame(
		struct fdma_present_frame_params *params)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	int8_t  res1;

	/* prepare command parameters */
	if (!(params->flags & FDMA_INIT_NDS_BIT))
		arg2 = FDMA_INIT_CMD_ARG2((uint32_t)(params->seg_dst),
					params->seg_offset);
	if (params->asa_size == 0)
		params->flags |= FDMA_INIT_NAS_BIT;
	else
		arg4 = FDMA_INIT_EXP_CMD_ARG4(params->asa_dst,
				params->asa_size);

	if ((uint32_t)(params->pta_dst) == PRC_PTA_NOT_LOADED_ADDRESS)
		params->flags |= FDMA_INIT_NPS_BIT;

	arg1 = FDMA_INIT_CMD_ARG1(((uint32_t)params->fd_src), params->flags);
	/*arg2 = FDMA_INIT_CMD_ARG2((uint32_t)(params->seg_address),
						params->seg_offset);*/
	arg3 = FDMA_INIT_EXP_CMD_ARG3(params->present_size, params->pta_dst,
				params->asa_offset);
	/*arg4 = FDMA_INIT_EXP_CMD_ARG4(params->asa_address,
					params->asa_size);*/


	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, ZERO);

	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	if (res1 == FDMA_UNABLE_TO_PRESENT_FULL_ASA_ERR)
		arg4 = (arg4 & ~PRC_ASAPS_MASK) |
			(uint32_t)(LDPAA_FD_GET_ASAL(HWC_FD_ADDRESS));

	if (((int32_t)res1) >= FDMA_SUCCESS) {
		params->frame_handle = *((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_FRAME_HANDLE_OFFSET));
		if (!(params->flags & FDMA_INIT_NDS_BIT)) {
			params->seg_length = *((uint16_t *)
					(HWC_ACC_OUT_ADDRESS2));
			params->seg_handle = *((uint8_t *)
					(HWC_ACC_OUT_ADDRESS2 +
					FDMA_SEG_HANDLE_OFFSET));
			/* Update Task Defaults */
			if (((uint32_t)params->fd_src) == HWC_FD_ADDRESS) {
				/* Todo - if NDS/NAS/NPS are added to prc update
				 * them */
				prc->seg_address = (uint16_t)
					((uint32_t)params->seg_dst);
				prc->seg_length = params->seg_length;
				prc->seg_offset = params->seg_offset;
				if (params->flags & FDMA_INIT_SR_BIT)
					PRC_SET_SR_BIT(PRC_SR_MASK);
				prc->handles = ((params->frame_handle <<
						PRC_FRAME_HANDLE_BIT_OFFSET) &
						PRC_FRAME_HANDLE_MASK) |
						(params->seg_handle &
						PRC_SEGMENT_HANDLE_MASK);
				prc->ptapa_asapo = (uint16_t)arg3;
				prc->asapa_asaps =
					(prc->asapa_asaps & PRC_SR_MASK) |
					((uint16_t)arg4 & ~PRC_SR_MASK);
			}
		} else if (((uint32_t)params->fd_src) == HWC_FD_ADDRESS) {
			/* Todo - if NDS/NAS/NPS are added to prc update them */
			prc->handles = (params->frame_handle <<
				PRC_FRAME_HANDLE_BIT_OFFSET) &
				PRC_FRAME_HANDLE_MASK;
			prc->ptapa_asapo = (uint16_t)arg3;
			prc->asapa_asaps = (prc->asapa_asaps & PRC_SR_MASK) |
					((uint16_t)arg4 & ~PRC_SR_MASK);
		}
	}
	return (int32_t)(res1);
}

int32_t fdma_present_default_frame_segment(
		uint32_t flags,
		void	 *ws_dst,
		uint16_t offset,
		uint16_t present_size,
		uint16_t *seg_length,
		uint8_t	 *seg_handle)
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t  res1;

	/* This command may be invoked only on Data segment */
	if ((PRC_GET_SEGMENT_HANDLE() == FDMA_ASA_SEG_HANDLE) ||
	    (PRC_GET_SEGMENT_HANDLE() == FDMA_PTA_SEG_HANDLE))
		return FDMA_NO_DATA_SEGMENT_HANDLE;
	/* prepare command parameters */
	arg1 = FDMA_PRESENT_CMD_ARG1(PRC_GET_HANDLES(),
			(flags | FDMA_ST_DATA_SEGMENT_BIT));
	arg2 = FDMA_PRESENT_CMD_ARG2((uint32_t)ws_dst, offset);
	arg3 = FDMA_PRESENT_CMD_ARG3(present_size);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS3)) = arg3;

	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	/* load command results */
	*seg_length = *((uint16_t *)(HWC_ACC_OUT_ADDRESS2));
	*seg_handle = *((uint8_t *)(HWC_ACC_OUT_ADDRESS2 +
			FDMA_SEG_HANDLE_OFFSET));
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	return (int32_t)(res1);
}

int32_t fdma_read_default_frame_asa(
		uint32_t flags,
		uint16_t offset,
		uint16_t present_size,
		void	 *ws_dst)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t  res1;

	/* prepare command parameters */
#ifdef NEXT_RELEASE
	/*Todo - SR bit is not relevant for ASA presentation in Present command.
	 * Since the flags parameters here has only the SR bit, the flags
	 * parameters should be removed from the function. */
#endif /* NEXT_RELEASE */
	arg1 = FDMA_PRESENT_CMD_ARG1(PRC_GET_HANDLES(),
			(flags | FDMA_ST_ASA_SEGMENT_BIT));
	arg2 = FDMA_PRESENT_CMD_ARG2((uint32_t)ws_dst, offset);
	arg3 = FDMA_PRESENT_CMD_ARG3(present_size);
	/* store command parameters */
	__stqw(arg1, arg2, arg3, ZERO, HWC_ACC_IN_ADDRESS, ZERO);

	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	prc->asapa_asaps = (((uint16_t)prc->asapa_asaps) & PRC_SR_MASK) |
			((uint16_t)((uint32_t)ws_dst) & PRC_ASAPA_MASK) |
			((*((uint16_t *)HWC_ACC_OUT_ADDRESS2)) &
						PRC_ASAPS_MASK);
	PRC_SET_ASA_OFFSET(offset);
	return (int32_t)(res1);
}

int32_t fdma_read_default_frame_pta(
		void *ws_dst)
{
	/* command parameters and results */
	uint32_t arg1, arg2;
	int8_t  res1;

	if (((uint16_t)((uint32_t)ws_dst)) == PRC_PTA_NOT_LOADED_ADDRESS)
		return FDMA_INVALID_PTA_ADDRESS;
	/* prepare command parameters */
	arg1 = FDMA_PRESENT_CMD_ARG1(PRC_GET_HANDLES(),
			FDMA_ST_PTA_SEGMENT_BIT);
	arg2 = FDMA_PRESENT_CMD_ARG2((uint16_t)((uint32_t)ws_dst), ZERO);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);

	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	PRC_SET_PTA_ADDRESS((uint16_t)((uint32_t)ws_dst));

	return (int32_t)(res1);
}

int32_t fdma_extend_default_segment_presentation(
		uint16_t extend_size,
		void	 *ws_dst,
		uint32_t flags)
{
	/* command parameters and results */
	uint32_t arg1, arg2;
	int8_t  res1;
	/* prepare command parameters */
	if (flags & FDMA_ST_ASA_SEGMENT_BIT)
		arg1 = FDMA_EXTEND_CMD_ASA_ARG1(PRC_GET_HANDLES());
	else
		arg1 = FDMA_EXTEND_CMD_ARG1(PRC_GET_HANDLES());
	arg2 = FDMA_EXTEND_CMD_ARG2((uint32_t)ws_dst, extend_size);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FPDMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	/* Update Task Defaults */
	if (((int32_t)res1) >= FDMA_SUCCESS) {
		if (flags & FDMA_ST_ASA_SEGMENT_BIT)
			PRC_SET_ASA_SIZE(*((uint16_t *)
					(HWC_ACC_OUT_ADDRESS2)));
		else
			PRC_SET_SEGMENT_LENGTH(*((uint16_t *)
					(HWC_ACC_OUT_ADDRESS2)));
	}
	return (int32_t)(res1);
}

int32_t fdma_store_default_frame_data(void)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	/* storage profile ID */
	uint8_t spid = *((uint8_t *) HWC_SPID_ADDRESS);

	/* prepare command parameters */
	arg1 = FDMA_STORE_DEFAULT_CMD_ARG1(spid, PRC_GET_HANDLES());
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	   __accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	return (int32_t)(res1);
}

int32_t fdma_store_frame_data(
		uint8_t frame_handle,
		uint8_t spid,
		struct fdma_isolation_attributes *isolation_attributes)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	/* storage profile ID */

	/* prepare command parameters */
	arg1 = FDMA_STORE_CMD_ARG1(spid, frame_handle);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	   __accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	isolation_attributes->bdi_icid = *((uint16_t *)HWC_ACC_OUT_ADDRESS);
	isolation_attributes->flags = (*((uint16_t *)
		(HWC_ACC_OUT_ADDRESS + FDMA_STORE_CMD_OUT_FLAGS_OFFSET)))
		& FDMA_STORE_CMD_OUT_FLAGS_MASK;
	return (int32_t)(res1);
}

int32_t fdma_store_and_enqueue_default_frame_fqid(
		uint32_t fqid,
		uint32_t flags)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	/* storage profile ID */
	uint8_t spid = *((uint8_t *) HWC_SPID_ADDRESS);

	/* prepare command parameters */
	flags |= FDMA_EN_EIS_BIT;
	arg1 = FDMA_ENQUEUE_WF_ARG1(spid, PRC_GET_HANDLES(), flags);
	/* store command parameters */
	__stdw(arg1, fqid, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	return (int32_t)(res1);
}

int32_t fdma_store_and_enqueue_default_frame_qd(
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
	arg1 = FDMA_ENQUEUE_WF_ARG1(spid, PRC_GET_HANDLES(), flags);
	arg2 = FDMA_ENQUEUE_WF_QD_ARG2(qdp->qd_priority, qdp->qd);
	arg3 = FDMA_ENQUEUE_WF_QD_ARG3(qdp->hash_value);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS3)) = arg3;
	/*__stqw(arg1, arg2, arg3, 0, HWC_ACC_IN_ADDRESS, 0);*/

	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	return (int32_t)(res1);
}

int32_t fdma_enqueue_default_fd_fqid(
		uint16_t icid ,
		uint32_t flags,
		uint32_t fqid)
{
	/* command parameters and results */
	uint32_t arg1, arg3;
	int8_t res1;
	/* storage profile ID */
	/* prepare command parameters */
	flags |= FDMA_EN_EIS_BIT;
	arg1 = FDMA_ENQUEUE_FRAME_ARG1(flags);
	arg3 = FDMA_ENQUEUE_FRAME_ARG3(flags, icid);
	/* store command parameters */
	__stdw(arg1, fqid, HWC_ACC_IN_ADDRESS, ZERO);
	*((uint32_t *) HWC_ACC_IN_ADDRESS3) = arg3;
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	return (int32_t)(res1);
}

int32_t fdma_enqueue_default_fd_qd(
		uint16_t icid ,
		uint32_t flags,
		struct fdma_queueing_destination_params *enqueue_params)
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t res1;
	/* storage profile ID */
	/* prepare command parameters */
	flags &= ~FDMA_EN_EIS_BIT;
	arg1 = FDMA_ENQUEUE_FRAME_ARG1(flags);
	arg2 = FDMA_ENQUEUE_WF_QD_ARG2(enqueue_params->qd_priority,
			enqueue_params->qd);
	arg3 = FDMA_ENQUEUE_FRAME_QD_ARG3(flags, icid,
			enqueue_params->hash_value);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);
	*((uint32_t *) HWC_ACC_IN_ADDRESS3) = arg3;
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	return (int32_t)(res1);
}

int32_t fdma_discard_default_frame(uint32_t flags)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	/* prepare command parameters */
	/*if (flags & FDMA_DIS_FS_HANDLE_BIT)*/
		arg1 = FDMA_DISCARD_ARG1_WF(PRC_GET_HANDLES(), flags);
	/*else
		arg1 = FDMA_DISCARD_ARG1_FD(HWC_FD_ADDRESS, flags);*/
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	return (int32_t)(res1);
}

int32_t fdma_discard_frame(uint16_t frame, uint32_t flags)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	/* prepare command parameters */
	/*if (flags & FDMA_DIS_FS_HANDLE_BIT)*/
		arg1 = FDMA_DISCARD_ARG1_FRAME(frame, flags);
	/*else
		arg1 = FDMA_DISCARD_ARG1_FD(HWC_FD_ADDRESS, flags);*/
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	return (int32_t)(res1);
}

void fdma_terminate_task(void)
{
	/* command parameters and results */
	uint32_t arg1;
	/* prepare command parameters */
	arg1 = FDMA_TERM_TASK_CMD_ARG1();
	*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = arg1;
	/* call FDMA Accelerator */
	__e_hwacceli_(FODMA_ACCEL_ID);
}

int32_t fdma_replicate_frame_fqid(
		uint8_t	frame_handle1,
		uint8_t	spid,
		uint32_t fqid,
		void *fd_dst,
		uint32_t flags,
		uint8_t *frame_handle2)
{
	/* command parameters and results */
	uint32_t arg1, arg3;
	int8_t  res1;

	flags |= FDMA_REPLIC_EIS_BIT;
	/* prepare command parameters */
	arg1 = FDMA_REPLIC_CMD_ARG1(spid, frame_handle1, flags);
	arg3 = FDMA_REPLIC_CMD_ARG3_FQID((uint32_t)fd_dst);
	/* store command parameters */
	__stdw(arg1, fqid, HWC_ACC_IN_ADDRESS, ZERO);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS3)) = arg3;
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	*frame_handle2 = *((uint8_t *) (FDMA_REPLIC_FRAME_HANDLE_OFFSET));

	return (int32_t)(res1);
}

int32_t fdma_replicate_frame_qd(
		uint8_t	frame_handle1,
		uint8_t	spid,
		struct fdma_queueing_destination_params *enqueue_params,
		void *fd_dst,
		uint32_t flags,
		uint8_t *frame_handle2)
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t  res1;

	flags &= ~FDMA_REPLIC_EIS_BIT;
	/* prepare command parameters */
	arg1 = FDMA_REPLIC_CMD_ARG1(spid, frame_handle1, flags);
	arg2 = FDMA_REPLIC_CMD_ARG2_QD(enqueue_params->qd_priority,
			enqueue_params->qd);
	arg3 = FDMA_REPLIC_CMD_ARG3_QD((uint32_t)fd_dst,
			enqueue_params->hash_value);
	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);
	*((uint32_t *)(HWC_ACC_IN_ADDRESS3)) = arg3;
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));
	*frame_handle2 = *((uint8_t *) (FDMA_REPLIC_FRAME_HANDLE_OFFSET));

	return (int32_t)(res1);
}

int32_t fdma_concatenate_frames(
		struct fdma_concatenate_frames_params *params)
{
	/* command parameters and results */
	uint32_t arg1, arg2;
	int8_t  res1;

	/* prepare command parameters */
	arg1 = FDMA_CONCAT_CMD_ARG1(params->spid, params->trim, params->flags);
	arg2 = FDMA_CONCAT_CMD_ARG2(params->frame2, params->frame1);

	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);

	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	return (int32_t)(res1);
}


int32_t fdma_split_frame(
		struct fdma_split_frame_params *params)
{
	/* Presentation Context Pointer */
	struct presentation_context *prc =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	/* command parameters and results */
	uint32_t arg1, arg2, arg3, arg4;
	int8_t  res1;

	/* prepare command parameters */
	arg1 = FDMA_SPLIT_CMD_ARG1(params->spid, params->source_frame_handle,
			params->flags);
	arg2 = FDMA_SPLIT_CMD_ARG2((uint32_t)(params->seg_dst),
			params->seg_offset);
	arg3 = FDMA_SPLIT_CMD_ARG3(params->present_size);
	arg4 = FDMA_SPLIT_CMD_ARG4((uint32_t)(params->fd_dst),
			params->split_size_sf);

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, ZERO);

	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *) (FDMA_STATUS_ADDR));

	if ((res1 == FDMA_SUCCESS) ||
		(res1 == FDMA_UNABLE_TO_PRESENT_FULL_SEGMENT_ERR)) {
		params->split_frame_handle = *((uint8_t *)
			(HWC_ACC_OUT_ADDRESS2 + FDMA_SEG_HANDLE_OFFSET));
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
				prc->handles =
					((params->split_frame_handle << 4) &
						PRC_FRAME_HANDLE_MASK) |
					(params->seg_handle &
						PRC_SEGMENT_HANDLE_MASK);
				prc->ptapa_asapo = PRC_PTA_NOT_LOADED_ADDRESS;
				prc->asapa_asaps =
					(params->flags & FDMA_SPLIT_SR_BIT) ?
							PRC_SR_MASK : ZERO;
			}
		}
		/* Update Task Defaults */
		else if ((((uint32_t)params->fd_dst) == HWC_FD_ADDRESS) &&
			((params->flags & (FDMA_SPLIT_PSA_PRESENT_BIT |
					FDMA_SPLIT_PSA_CLOSE_FRAME_BIT)) == 0)){
				prc->handles =
					((params->split_frame_handle << 4) &
					PRC_FRAME_HANDLE_MASK);
				prc->ptapa_asapo = PRC_PTA_NOT_LOADED_ADDRESS;
				prc->asapa_asaps = ZERO;
		}
	}

	return (int32_t)(res1);
}

int32_t fdma_trim_default_segment_presentation(uint16_t offset, uint16_t size)
{
	/* command parameters and results */
	uint32_t arg1, arg2;
	int8_t res1;

	arg1 = FDMA_TRIM_CMD_ARG1(PRC_GET_HANDLES());
	arg2 = FDMA_TRIM_CMD_ARG2(offset, size);

	/* store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));
	/* Update Task Defaults */
	if (res1 == FDMA_SUCCESS)
		PRC_SET_SEGMENT_LENGTH(size);

	return (int32_t)(res1);
}

int32_t fdma_modify_default_segment_data(
		uint16_t offset,
		uint16_t size)
{
	/* command parameters and results */
	uint32_t arg1, arg2, arg3;
	int8_t res1;

	/* This command may be invoked only on Data segment */
	if ((PRC_GET_SEGMENT_HANDLE() == FDMA_ASA_SEG_HANDLE) ||
	    (PRC_GET_SEGMENT_HANDLE() == FDMA_PTA_SEG_HANDLE))
		return FDMA_NO_DATA_SEGMENT_HANDLE;
	/* prepare command parameters */
	arg1 = FDMA_REPLACE_CMD_ARG1(
			PRC_GET_HANDLES(), FDMA_REPLACE_SA_OPEN_BIT);
	arg2 = FDMA_REPLACE_CMD_ARG2(offset, size);
	arg3 = FDMA_REPLACE_CMD_ARG3(
			(PRC_GET_SEGMENT_ADDRESS() + offset), size);
	/* store command parameters */
	__stqw(arg1, arg2, arg3, ZERO, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	return (int32_t)(res1);
}

int32_t fdma_replace_default_segment_data(
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

	/* This command may be invoked only on Data segment */
	if ((PRC_GET_SEGMENT_HANDLE() == FDMA_ASA_SEG_HANDLE) ||
	    (PRC_GET_SEGMENT_HANDLE() == FDMA_PTA_SEG_HANDLE))
		return FDMA_NO_DATA_SEGMENT_HANDLE;
	/* prepare command parameters */
	arg1 = FDMA_REPLACE_CMD_ARG1(prc->handles, flags);
	arg2 = FDMA_REPLACE_CMD_ARG2(to_offset, to_size);
	arg3 = FDMA_REPLACE_CMD_ARG3(from_ws_src, from_size);
	arg4 = FDMA_REPLACE_CMD_ARG4(ws_dst_rs, size_rs);
	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	/* Update Task Defaults */
	if (res1 >= FDMA_SUCCESS) {
		if (to_size != from_size) {
			if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
				prc->seg_address = (uint16_t)(uint32_t)
							ws_dst_rs;
				prc->seg_length = *((uint16_t *)
						HWC_ACC_OUT_ADDRESS2);

			} /*else {
				prc->seg_length += from_size ;
				prc->seg_length -= to_size;
			}*/
			/* FD fields should be updated with a swap load/store */
			if (from_size != to_size)
				LDPAA_FD_UPDATE_LENGTH(HWC_FD_ADDRESS,
						from_size, to_size);
		}
#ifdef NEXT_RELEASE
		if (flags & FDMA_REPLACE_SA_CLOSE_BIT)
			PRC_SET_NDS_BIT(PRC_NDS_MASK);
#endif /* NEXT_RELEASE */
	}

	return (int32_t)(res1);
}

int32_t fdma_insert_default_segment_data(
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
	void	 *ws_address_rs;
	int8_t res1;

	/* This command may be invoked only on Data segment */
	if ((PRC_GET_SEGMENT_HANDLE() == FDMA_ASA_SEG_HANDLE) ||
	    (PRC_GET_SEGMENT_HANDLE() == FDMA_PTA_SEG_HANDLE))
		return FDMA_NO_DATA_SEGMENT_HANDLE;
	/* prepare command parameters */
	arg1 = FDMA_REPLACE_CMD_ARG1(prc->handles, flags);
	arg2 = FDMA_REPLACE_CMD_ARG2(to_offset, ZERO);
	arg3 = FDMA_REPLACE_CMD_ARG3(from_ws_src, insert_size);
	if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
		ws_address_rs = (void *)
			(PRC_GET_SEGMENT_ADDRESS() - insert_size);
		arg4 = FDMA_REPLACE_CMD_ARG4(ws_address_rs,
				PRC_GET_SEGMENT_LENGTH());
	}
	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	/* Update Task Defaults */
	if (res1 >= FDMA_SUCCESS) {
		if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
			prc->seg_address = (uint16_t)(uint32_t)
						ws_address_rs;
			prc->seg_length = *((uint16_t *)
					HWC_ACC_OUT_ADDRESS2);
		} /*else {
			prc->seg_length += insert_size ;
		}*/
		/* FD fields should be updated with a swap load/store */
		LDPAA_FD_UPDATE_LENGTH(HWC_FD_ADDRESS, insert_size, ZERO);

#ifdef NEXT_RELEASE
		if (flags & FDMA_REPLACE_SA_CLOSE_BIT)
			PRC_SET_NDS_BIT(PRC_NDS_MASK);
#endif /* NEXT_RELEASE */
	}
	return (int32_t)(res1);
}

int32_t fdma_delete_default_segment_data(
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

	/* This command may be invoked only on Data segment */
	if ((PRC_GET_SEGMENT_HANDLE() == FDMA_ASA_SEG_HANDLE) ||
	    (PRC_GET_SEGMENT_HANDLE() == FDMA_PTA_SEG_HANDLE))
		return FDMA_NO_DATA_SEGMENT_HANDLE;
	/* prepare command parameters */
	arg1 = FDMA_REPLACE_CMD_ARG1(prc->handles, flags);
	arg2 = FDMA_REPLACE_CMD_ARG2(to_offset, delete_target_size);
	arg3 = FDMA_REPLACE_CMD_ARG3(0, 0);
	if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
		ws_address_rs = (void *)
			(PRC_GET_SEGMENT_ADDRESS() + delete_target_size);
		size_rs = PRC_GET_SEGMENT_LENGTH() - delete_target_size;
		arg4 = FDMA_REPLACE_CMD_ARG4(ws_address_rs, size_rs);
	}

	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	/* Update Task Defaults */
	if (res1 >= FDMA_SUCCESS) {
		if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
			prc->seg_address = (uint16_t)(uint32_t)
						ws_address_rs;
			prc->seg_length = *((uint16_t *)
					HWC_ACC_OUT_ADDRESS2);
		} /*else {
			prc->seg_length -= delete_target_size;
		}*/
		/* FD fields should be updated with a swap load/store */
		LDPAA_FD_UPDATE_LENGTH(HWC_FD_ADDRESS, ZERO,
				delete_target_size);

#ifdef NEXT_RELEASE
		if (flags & FDMA_REPLACE_SA_CLOSE_BIT)
			PRC_SET_NDS_BIT(PRC_NDS_MASK);
#endif /* NEXT_RELEASE */
	}
	return (int32_t)(res1);
}

int32_t fdma_close_default_segment(void)
{
	/* command parameters and results */
	uint32_t arg1;
	int8_t res1;
	uint8_t seg_handle;

	/* This command may be invoked only on Data segment */
	seg_handle = PRC_GET_SEGMENT_HANDLE();
	if ((seg_handle == FDMA_ASA_SEG_HANDLE) ||
	    (seg_handle == FDMA_PTA_SEG_HANDLE))
		return FDMA_NO_DATA_SEGMENT_HANDLE;
	/* prepare command parameters */
	arg1 = FDMA_CLOSE_SEG_CMD_ARG1(PRC_GET_HANDLES(), seg_handle);
	/* store command parameters */
	__stdw(arg1, ZERO, HWC_ACC_IN_ADDRESS, ZERO);
	*((uint32_t *) HWC_ACC_IN_ADDRESS3) = ZERO;
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));
#ifdef NEXT_RELEASE
	if (res1 == FDMA_SUCCESS)
		PRC_SET_NDS_BIT(PRC_NDS_MASK);
#endif /* NEXT_RELEASE */
	return (int32_t)(res1);
}

int32_t fdma_replace_default_asa_segment_data(
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
	/*uint16_t size_diff;*/
	int8_t res1;

	/* prepare command parameters */
	arg1 = FDMA_REPLACE_PTA_ASA_CMD_ARG1(
			FDMA_ASA_SEG_HANDLE, prc->handles, flags);
	arg2 = FDMA_REPLACE_CMD_ARG2(to_offset, to_size);
	arg3 = FDMA_REPLACE_CMD_ARG3(from_ws_src, from_size);
	arg4 = FDMA_REPLACE_CMD_ARG4(ws_dst_rs, size_rs);
	/* store command parameters */
	__stqw(arg1, arg2, arg3, arg4, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	/* Update Task Defaults */
	if ((res1 >= FDMA_SUCCESS) && (to_size != from_size)) {
		if (flags & FDMA_REPLACE_SA_REPRESENT_BIT) {
			prc->asapa_asaps =
				(((uint16_t)prc->asapa_asaps) & PRC_SR_MASK) |
				(((uint16_t)((uint32_t)ws_dst_rs)) &
							PRC_ASAPA_MASK) |
				((*((uint16_t *)HWC_ACC_OUT_ADDRESS2)) &
						PRC_ASAPS_MASK);
		} /*else {
			if (from_size > to_size) {
				size_diff = from_size - to_size;
				prc->asapa_asaps = (prc->asapa_asaps &
						~PRC_ASAPS_MASK) |
					((prc->asapa_asaps + size_diff) &
							PRC_ASAPS_MASK);
			} else {
				size_diff = to_size - from_size;
				prc->asapa_asaps = (prc->asapa_asaps &
						~PRC_ASAPS_MASK) |
					((prc->asapa_asaps - size_diff) &
							PRC_ASAPS_MASK);
			}
		}*/
	}

	/* Update FD ASA fields */
	if (res1 >= FDMA_SUCCESS)
		LDPAA_FD_SET_ASAL(HWC_FD_ADDRESS,
			LDPAA_FD_GET_ASAL(HWC_FD_ADDRESS) + (uint8_t)from_size -
							(uint8_t)to_size);

	return (int32_t)(res1);
}

int32_t fdma_replace_default_pta_segment_data(
		uint32_t flags,
		void	 *from_ws_src,
		void	 *ws_dst_rs,
		enum fdma_pta_size_type size_type)
{
	/* command parameters and results */
	uint32_t arg1, arg3, arg4;
	int8_t res1;

	/* prepare command parameters */
	if ((flags & FDMA_REPLACE_SA_REPRESENT_BIT) &&
		(((uint16_t)((uint32_t)ws_dst_rs)) ==
				PRC_PTA_NOT_LOADED_ADDRESS))
			return FDMA_INVALID_PTA_ADDRESS;

	arg1 = FDMA_REPLACE_PTA_ASA_CMD_ARG1(
			FDMA_PTA_SEG_HANDLE, PRC_GET_HANDLES(), flags);
	arg3 = FDMA_REPLACE_CMD_ARG3(from_ws_src, size_type);
	arg4 = FDMA_REPLACE_CMD_ARG4(ws_dst_rs, ZERO);
	/* store command parameters */
	__stqw(arg1, ZERO, arg3, arg4, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	if ((res1 >= FDMA_SUCCESS)) {
		/* in case of representing the PTA, update the PTA address in
		 * the prc */
		if (flags & FDMA_REPLACE_SA_REPRESENT_BIT)
			PRC_SET_PTA_ADDRESS(
				(((uint16_t)((uint32_t)ws_dst_rs))));
		/* Update FD PTA fields only in case PTA was modified */
		if (size_type & PTA_SIZE_PTV1) {
			LDPAA_FD_SET_PTV1(HWC_FD_ADDRESS, FD_PTV1_MASK);
			LDPAA_FD_SET_PTV2(HWC_FD_ADDRESS, ZERO);
		}
		if (size_type & PTA_SIZE_PTV2) {
			LDPAA_FD_SET_PTV2(HWC_FD_ADDRESS, FD_PTV2_MASK);
			LDPAA_FD_SET_PTV1(HWC_FD_ADDRESS, ZERO);
		}
		if (size_type & PTA_SIZE_PTV1_2) {
			LDPAA_FD_SET_PTA(HWC_FD_ADDRESS, FD_PTA_MASK);
			LDPAA_FD_SET_PTV1(HWC_FD_ADDRESS, FD_PTV1_MASK);
			LDPAA_FD_SET_PTV2(HWC_FD_ADDRESS, FD_PTV2_MASK);
		}
	}

	return (int32_t)(res1);
}

int32_t fdma_calculate_default_frame_checksum(
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
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, ZERO);
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));
	*checksum = *((uint16_t *)(HWC_ACC_OUT_ADDRESS2+FDMA_CHECKSUM_OFFSET));

	return (int32_t)(res1);
}

int32_t fdma_copy_data(
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
	__stdw(arg1, (uint32_t)src, HWC_ACC_IN_ADDRESS, ZERO);
	*((uint32_t *) HWC_ACC_IN_ADDRESS3) = (uint32_t)dst;
	/* call FDMA Accelerator */
	/* Todo - Note to Hw/Compiler team:
	__accel_call() should return success/fail indication */
	__e_hwacceli_(FODMA_ACCEL_ID);
	/* load command results */
	res1 = *((int8_t *)(FDMA_STATUS_ADDR));

	return (int32_t)(res1);
}
