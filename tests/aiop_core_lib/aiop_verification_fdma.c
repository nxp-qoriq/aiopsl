/**************************************************************************//**
@File		aiop_verification_fdma.c

@Description	This file contains the AIOP FDMA SRs SW Verification.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "aiop_verification.h"
#include "aiop_verification_fdma.h"
#include "fsl_fdma.h"
#include "fdma.h"

/* Todo - __TASK uint8_t SPID; use this */

uint16_t aiop_verification_fdma(uint32_t asa_seg_addr)
{
	struct presentation_context *PRC =
			(struct presentation_context *) HWC_PRC_ADDRESS;
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint32_t flags;
	uint32_t i;


	opcode  = *((uint32_t *) asa_seg_addr);
	flags = 0x0;

	switch (opcode) {
	/* FDMA Initial frame presentation Command Verification */
	case FDMA_INIT_CMD_STR:
	{
		struct fdma_init_command *str =
			(struct fdma_init_command *) asa_seg_addr;
		if (str->SR)
			PRC_SET_SR_BIT(1);
		PRC->asapa_asaps	=
			(((uint16_t)(uint32_t)str->asa_address) &
					PRC_ASAPA_MASK) |
			((uint16_t)str->asa_size & PRC_ASAPS_MASK);
		PRC->ptapa_asapo	=
			(((uint16_t)(uint32_t)str->pta_address) &
					PRC_PTAPA_MASK) |
			((uint16_t)str->asa_offset & PRC_ASAPO_MASK);
		PRC->seg_length		= str->present_size;
		PRC->seg_address	= (uint16_t)(uint32_t)str->seg_address;
		PRC->seg_offset		= str->seg_offset;
		str->status = (int8_t)fdma_present_default_frame();
		str_size = sizeof(struct fdma_init_command);
		break;
	}
	/* FDMA Initial frame presentation explicit Command Verification */
	case FDMA_INIT_EXP_CMD_STR:
	{
		struct fdma_init_exp_command *str =
			(struct fdma_init_exp_command *) asa_seg_addr;
		struct fdma_present_frame_params params;
		flags |= ((str->NDS)  ? FDMA_INIT_NDS_BIT  : 0x0);
		flags |= ((str->SR)   ? FDMA_INIT_SR_BIT   : 0x0);
		params.asa_dst	= (void *)str->asa_dst;
		params.asa_offset	= str->asa_offset;
		params.asa_size		= str->asa_size;
		params.fd_src		= (void *)str->fd_src;
		params.flags		= flags;
		params.present_size	= str->present_size;
		params.pta_dst	= (void *)str->pta_dst;
		params.seg_dst	= (void *)str->seg_dst;
		params.seg_offset	= str->seg_offset;
		str->status = (int8_t)fdma_present_frame(&params);
		str->frame_handle	= params.frame_handle;
		str->seg_handle		= params.seg_handle;
		str->seg_length		= params.seg_length;
		str_size = sizeof(struct fdma_init_exp_command);
		break;
	}
	/* FDMA Present segment Command Verification */
	case FDMA_PRESENT_CMD_STR:
	{
		struct fdma_present_command *str =
			(struct fdma_present_command *) asa_seg_addr;
		flags |= ((str->SR) ? FDMA_PRES_SR_BIT : 0x0);
		str->status = (int8_t)fdma_present_default_frame_segment(flags,
				(void *)str->ws_dst, str->offset,
				str->present_size, &(str->seg_length),
				&(str->seg_handle));
		str_size = sizeof(struct fdma_present_command);
		break;
	}
	/* FDMA Read ASA segment Command Verification */
	case FDMA_READ_ASA_CMD_STR:
	{
		struct fdma_read_asa_command *str =
			(struct fdma_read_asa_command *) asa_seg_addr;
		flags |= ((str->SR) ? FDMA_PRES_SR_BIT : 0x0);
		str->status = (int8_t)fdma_read_default_frame_asa(flags,
				str->offset, str->present_size,
				(void *)str->ws_dst);
		str->seg_length = PRC_GET_ASA_SIZE();
		str_size = sizeof(struct fdma_read_asa_command);
		break;
	}
	/* FDMA Read PTA segment Command Verification */
	case FDMA_READ_PTA_CMD_STR:
	{
		struct fdma_read_pta_command *str =
			(struct fdma_read_pta_command *) asa_seg_addr;
		str->status = (int8_t)fdma_read_default_frame_pta(
				(void *)str->ws_dst);
		if (LDPAA_FD_GET_PTA(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV1_2;
		else if (LDPAA_FD_GET_PTV1(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV1;
		else if (LDPAA_FD_GET_PTV2(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV2;
		else
			str->seg_length = PTA_SIZE_NO_PTA;
		str_size = sizeof(struct fdma_read_pta_command);
		break;
	}
	/* FDMA Extend segment Command Verification */
	case FDMA_EXTEND_CMD_STR:
	{
		struct fdma_extend_command *str =
			(struct fdma_extend_command *) asa_seg_addr;
		flags |= (str->ST == 1) ?
			FDMA_ST_ASA_SEGMENT_BIT : FDMA_ST_DATA_SEGMENT_BIT;
		str->status = (int8_t)fdma_extend_default_segment_presentation(
				str->extend_size, (void *)str->ws_dst, flags);
		str->seg_length = PRC->seg_length;
		str_size = sizeof(struct fdma_extend_command);
		break;
	}
	/* FDMA Store Default Working Frame Command Verification */
	case FDMA_STORE_DEFAULT_WF_CMD_STR:
	{
		struct fdma_store_default_frame_command *str =
			(struct fdma_store_default_frame_command *)asa_seg_addr;
		*(uint8_t *) HWC_SPID_ADDRESS = str->spid;
		aiop_verification_replace_asa();
		str->status = (int8_t)fdma_store_default_frame_data();
		str_size = sizeof(struct fdma_store_default_frame_command);
		break;
	}
	/* FDMA Store Working Frame Command Verification */
	case FDMA_STORE_WF_CMD_STR:
	{
		struct fdma_store_frame_command *str =
			(struct fdma_store_frame_command *) asa_seg_addr;
		struct fdma_isolation_attributes isolation_attributes;
		aiop_verification_replace_asa();
		str->status = (int8_t)fdma_store_frame_data(str->frame_handle,
				str->spid, &isolation_attributes);
		str->icid = (isolation_attributes.bdi_icid) & ~0x8000;
		str->BDI = (uint8_t)((isolation_attributes.bdi_icid) & 0x8000);
		str->BMT = (uint8_t)(flags & FDMA_ICID_CONTEXT_BMT);
		str->PL = (uint8_t)(flags & FDMA_ICID_CONTEXT_PL);
		str->VA = (uint8_t)(flags & FDMA_ICID_CONTEXT_VA);
		str_size = sizeof(struct fdma_store_frame_command);
		break;
	}
	/* FDMA Enqueue working frame Command Verification */
	case FDMA_ENQUEUE_WF_CMD_STR:
	{
		struct fdma_enqueue_wf_command *str =
			(struct fdma_enqueue_wf_command *)asa_seg_addr;
		struct fdma_queueing_destination_params qdp;
		*(uint8_t *) HWC_SPID_ADDRESS = str->spid;
		flags |= ((str->TC == 1) ? (FDMA_EN_TC_TERM_BITS) :
		((str->TC == 2) ? (FDMA_EN_TC_CONDTERM_BITS) : 0x0));
		flags |= ((str->PS) ? FDMA_ENWF_PS_BIT : 0x0);

		aiop_verification_replace_asa();
		if (str->EIS) {
			str->status = (int8_t)
				fdma_store_and_enqueue_default_frame_fqid(
					str->qd_fqid, flags);
		} else{
			qdp.qd = (uint16_t)(str->qd_fqid);
			qdp.hash_value = str->hash_value;
			qdp.qd_priority = str->qd_priority;
			str->status = (int8_t)
				fdma_store_and_enqueue_default_frame_qd(
						&qdp, flags);
		}

		str_size = sizeof(struct fdma_enqueue_wf_command);
		break;
	}
	/* FDMA Enqueue FD Command Verification */
	case FDMA_ENQUEUE_FRAME_CMD_STR:
	{
		struct fdma_enqueue_frame_command *str =
			(struct fdma_enqueue_frame_command *)asa_seg_addr;
		struct fdma_queueing_destination_params qdp;
		flags |= ((str->TC == 1) ? (FDMA_EN_TC_TERM_BITS) :
		((str->TC == 2) ? (FDMA_EN_TC_CONDTERM_BITS) : 0x0));
		flags |= ((str->PS) ? FDMA_ENF_PS_BIT : 0x0);
		flags |= ((str->VA) ? FDMA_ENF_VA_BIT : 0x0);
		flags |= ((str->BMT) ? FDMA_ENF_BMT_BIT : 0x0);
		flags |= ((str->PL) ? FDMA_ENF_PL_BIT : 0x0);
		flags |= ((str->BDI) ? FDMA_ENF_BDI_BIT : 0x0);

		aiop_verification_replace_asa();
		if (str->EIS) {
			str->status = (int8_t)
				fdma_enqueue_default_fd_fqid(
					str->icid, flags, str->qd_fqid);
		} else{
			qdp.qd = (uint16_t)(str->qd_fqid);
			qdp.hash_value = str->hash_value;
			qdp.qd_priority = str->qd_priority;
			str->status = (int8_t)
				fdma_enqueue_default_fd_qd(
						str->icid, flags, &qdp);
		}

		str_size = sizeof(struct fdma_enqueue_frame_command);
		break;
	}
	/* FDMA Discard default frame Command Verification */
	case FDMA_DISCARD_DEFAULT_WF_CMD_STR:
	{
		struct fdma_discard_default_wf_command *str =
			(struct fdma_discard_default_wf_command *) asa_seg_addr;
		/*flags |= ((str->FS == 1) ? FDMA_DIS_FS_FD_BIT :
				FDMA_DIS_FS_HANDLE_BIT);*/
		/*flags |= FDMA_DIS_FS_HANDLE_BIT;*/
		flags |= (str->TC == 1) ? FDMA_DIS_WF_TC_BIT : ZERO ;
		str->status = (int8_t)fdma_discard_default_frame(flags);
		str_size = sizeof(struct fdma_discard_default_wf_command);
		break;
	}
	/* FDMA Discard frame Command Verification */
	case FDMA_DISCARD_WF_CMD_STR:
	{
		struct fdma_discard_wf_command *str =
			(struct fdma_discard_wf_command *) asa_seg_addr;
		/*flags |= ((str->FS == 1) ? FDMA_DIS_FS_FD_BIT :
				FDMA_DIS_FS_HANDLE_BIT);*/
		/*flags |= FDMA_DIS_FS_HANDLE_BIT;*/
		flags |= (str->TC == 1) ? FDMA_DIS_WF_TC_BIT : ZERO ;
		str->status = (int8_t)fdma_discard_frame(str->frame, flags);
		str_size = sizeof(struct fdma_discard_wf_command);
		break;
	}
	/* FDMA Terminate Task Command Verification */
	case FDMA_TERMINATE_TASK_CMD_STR:
	{
		struct fdma_terminate_task_command *str =
			(struct fdma_terminate_task_command *)
				asa_seg_addr;

		fdma_terminate_task();
		str_size = sizeof(struct fdma_terminate_task_command);
		break;
	}
	/* FDMA Replicate frame Command Verification */
	case FDMA_REPLICATE_CMD_STR:
	{
		struct fdma_replicate_frames_command *str =
			(struct fdma_replicate_frames_command *)
				asa_seg_addr;
		struct fdma_queueing_destination_params qdp;

		flags |= ((str->DSF) ? FDMA_REPLIC_DSF_BIT : 0x0);
		flags |= ((str->ENQ) ? FDMA_REPLIC_ENQ_BIT : 0x0);
		flags |= ((str->PS) ? FDMA_REPLIC_PS_BIT : 0x0);
		flags |= (str->CFA == 3) ?
			FDMA_CFA_COPY_BIT :
			((str->CFA == 2) ?
				FDMA_CFA_PTA_COPY_BIT :
				((str->CFA == 1) ?
					FDMA_CFA_ASA_COPY_BIT :
					FDMA_CFA_NO_COPY_BIT));
		if (str->ENQ)
			aiop_verification_replace_asa();
		if (str->EIS) {
			str->status = (int8_t)fdma_replicate_frame_fqid(
					str->frame_handle1, str->spid,
					str->qd_fqid, (void *)str->fd_dst,
					flags, &(str->frame_handle2));
		} else{
			qdp.qd = (uint16_t)(str->qd_fqid);
			qdp.hash_value = str->hash_value;
			qdp.qd_priority = str->qd_priority;
			str->status = (int8_t)fdma_replicate_frame_qd(
					str->frame_handle1, str->spid,
					&qdp, (void *)str->fd_dst, flags,
					&(str->frame_handle2));
		}
		str_size = sizeof(struct fdma_replicate_frames_command);
		break;
	}
	/* FDMA Concatenate frames Command Verification */
	case FDMA_CONCAT_CMD_STR:
	{
		struct fdma_concatenate_frames_command *str =
			(struct fdma_concatenate_frames_command *)
				asa_seg_addr;
		struct fdma_concatenate_frames_params params;
		flags |= ((str->SF) ? FDMA_CONCAT_SF_BIT : 0x0);
		flags |= ((str->PCA) ? FDMA_CONCAT_PCA_BIT : 0x0);

		params.flags	= flags;
		params.frame1	= str->frame1;
		params.frame2	= str->frame2;
		params.spid	= str->spid;
		params.trim	= str->trim;
		str->status = (int8_t)fdma_concatenate_frames(&params);
		str_size = sizeof(struct fdma_concatenate_frames_command);
		break;
	}
	/* FDMA Split frames Command Verification */
	case FDMA_SPLIT_CMD_STR:
	{
		struct fdma_split_frame_command *str =
			(struct fdma_split_frame_command *)
				asa_seg_addr;
		struct fdma_split_frame_params params;
		/*flags |= ((str->NSS) ? FDMA_INIT_NSS_BIT : 0x0);*/
		flags |= (str->CFA == 3) ?
			FDMA_CFA_COPY_BIT :
			((str->CFA == 2) ?
				FDMA_CFA_PTA_COPY_BIT :
				((str->CFA == 1) ?
					FDMA_CFA_ASA_COPY_BIT :
					FDMA_CFA_NO_COPY_BIT));
		flags |= (str->PSA == 2) ?
			FDMA_SPLIT_PSA_CLOSE_FRAME_BIT :
			((str->PSA == 1) ?
				FDMA_SPLIT_PSA_PRESENT_BIT :
				FDMA_SPLIT_PSA_NO_PRESENT_BIT);
		flags |= ((str->SM) ? FDMA_SPLIT_SM_BIT : 0x0);
		flags |= ((str->SR)  ? FDMA_SPLIT_SR_BIT  : 0x0);
		params.fd_dst		= (void *)str->fd_dst;
		params.flags		= flags;
		params.present_size	= str->present_size;
		params.split_size_sf	= str->split_size_sf;
		params.seg_dst	= (void *)str->seg_dst;
		params.seg_offset	= str->seg_offset;
		params.source_frame_handle = str->source_frame_handle;
		params.spid = str->spid;
		str->status = (int8_t)fdma_split_frame(&params);
		str->split_frame_handle	= params.split_frame_handle;
		str->seg_handle		= params.seg_handle;
		str->seg_length		= params.seg_length;
		str_size = sizeof(struct fdma_split_frame_command);
		break;
	}
	/* FDMA Trim Command Verification */
	case FDMA_TRIM_CMD_STR:
	{
		struct fdma_trim_command *str =
			(struct fdma_trim_command *) asa_seg_addr;
		str->status = (int8_t)fdma_trim_default_segment_presentation(
				str->offset, str->size);
		str_size = sizeof(struct fdma_trim_command);
		break;
	}
	/* FDMA Replace Command Verification */
	case FDMA_MODIFY_CMD_STR:
	{
		struct fdma_modify_command *str =
			(struct fdma_modify_command *) asa_seg_addr;
		uint8_t *address = (uint8_t *)(PRC_GET_SEGMENT_ADDRESS() +
						str->offset);
		for (i = 0; i < str->size; i++)
			*address++ = str->data[i%24];

		str->status = (int8_t)fdma_modify_default_segment_data(
				str->offset, str->size);
		str_size = sizeof(struct fdma_modify_command);
		break;
	}
	/* FDMA Replace Command Verification */
	case FDMA_REPLACE_CMD_STR:
	{
		struct fdma_replace_command *str =
			(struct fdma_replace_command *) asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT :
					   FDMA_REPLACE_SA_OPEN_BIT));
		str->status = (int8_t)fdma_replace_default_segment_data(
				str->to_offset, str->to_size,
				(void *)str->from_ws_src, str->from_size,
				(void *)str->ws_dst_rs, str->size_rs, flags);
		str->seg_length_rs = PRC->seg_length;
		str_size = sizeof(struct fdma_replace_command);
		break;
	}
	/* FDMA Insert data Command Verification */
	case FDMA_INSERT_DATA_CMD_STR:
	{
		struct fdma_insert_segment_data_command *str =
			(struct fdma_insert_segment_data_command *)asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT :
					   FDMA_REPLACE_SA_OPEN_BIT));
		str->status = (int8_t)fdma_insert_default_segment_data(
				str->to_offset, (void *)str->from_ws_src,
				str->insert_size, flags);
		str->seg_length_rs = PRC->seg_length;
		str_size = sizeof(struct fdma_insert_segment_data_command);
		break;
	}
	/* FDMA Delete data Command Verification */
	case FDMA_DELETE_DATA_CMD_STR:
	{
		struct fdma_delete_segment_data_command *str =
			(struct fdma_delete_segment_data_command *)asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT :
					   FDMA_REPLACE_SA_OPEN_BIT));
		str->status = (int8_t)fdma_delete_default_segment_data(
				str->to_offset, str->delete_target_size, flags);
		str->seg_length_rs = PRC->seg_length;
		str_size = sizeof(struct fdma_delete_segment_data_command);
		break;
	}
	/* FDMA Close segment Command Verification */
	case FDMA_CLOSE_SEG_CMD_STR:
	{
		struct fdma_close_segment_command *str =
			(struct fdma_close_segment_command *)asa_seg_addr;
		str->status = (int8_t)fdma_close_default_segment();
		str_size = sizeof(struct fdma_close_segment_command);
		break;
	}
	/* FDMA Replace ASA Command Verification */
	case FDMA_REPLACE_ASA_CMD_STR:
	{
		struct fdma_replace_asa_command *str =
			(struct fdma_replace_asa_command *) asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT :
					   FDMA_REPLACE_SA_OPEN_BIT));
		str->status = (int8_t)fdma_replace_default_asa_segment_data(
				str->to_offset, str->to_size,
				(void *)str->from_ws_src, str->from_size,
				(void *)str->ws_dst_rs, str->size_rs, flags);
		str->seg_length_rs = (*((uint16_t *) HWC_ACC_OUT_ADDRESS2)) &
				PRC_ASAPS_MASK;
		str_size = sizeof(struct fdma_replace_asa_command);
		break;
	}
	/* FDMA Replace PTA Command Verification */
	case FDMA_REPLACE_PTA_CMD_STR:
	{
		struct fdma_replace_pta_command *str =
			(struct fdma_replace_pta_command *) asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT :
					   FDMA_REPLACE_SA_OPEN_BIT));
		str->status = (int8_t)fdma_replace_default_pta_segment_data(
				flags, (void *)str->from_ws_src,
				(void *)str->ws_dst_rs,
				(enum fdma_pta_size_type)str->size);
		if (LDPAA_FD_GET_PTA(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV1_2;
		else if (LDPAA_FD_GET_PTV1(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV1;
		else if (LDPAA_FD_GET_PTV2(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV2;
		else
			str->seg_length = PTA_SIZE_NO_PTA;
		str_size = sizeof(struct fdma_replace_pta_command);
		break;
	}
	/* FDMA Checksum Command Verification */
	case FDMA_CKS_CMD_STR:
	{
		struct fdma_checksum_command *str =
			(struct fdma_checksum_command *) asa_seg_addr;
		str->status = (int8_t)fdma_calculate_default_frame_checksum(
				str->offset, str->size, &(str->checksum));
		str_size = sizeof(struct fdma_checksum_command);
		break;
	}
	/* FDMA Copy Command Verification */
	case FDMA_COPY_CMD_STR:
	{
		struct fdma_copy_command *str =
			(struct fdma_copy_command *) asa_seg_addr;
		flags |= ((str->SM) ? FDMA_COPY_SM_BIT : 0x0);
		flags |= ((str->DM) ? FDMA_COPY_DM_BIT : 0x0);
		str->status = (int8_t)fdma_copy_data(str->copy_size, flags,
				(void *)str->src, (void *)str->dst);
		str_size = sizeof(struct fdma_copy_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}


	return str_size;
}

void aiop_verification_replace_asa()
{
	/* Presentation Context */
	struct presentation_context *PRC;
	uint16_t asa_seg_addr;	/* ASA Segment Address */
	uint16_t asa_seg_size;  /* ASA Segment Size */
	uint32_t flags;

	/* initialize Additional Dequeue Context */
	PRC = (struct presentation_context *) HWC_PRC_ADDRESS;
	/* Initialize ASA variables */
	asa_seg_addr = (uint16_t)(PRC->asapa_asaps & PRC_ASAPA_MASK);
	asa_seg_size = (PRC->asapa_asaps & PRC_ASAPS_MASK);
	flags = FDMA_REPLACE_SA_OPEN_BIT;

	fdma_replace_default_asa_segment_data((uint16_t)ZERO, asa_seg_size,
			(void *)asa_seg_addr, asa_seg_size,
			(void *)asa_seg_addr, (uint16_t)ZERO, flags);
}


