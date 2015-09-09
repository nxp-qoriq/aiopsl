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
@File		aiop_verification_fdma.c

@Description	This file contains the AIOP FDMA SRs SW Verification.

*//***************************************************************************/

#include "fsl_fdma.h"
#include "fsl_frame_operations.h"

/* TODO - get rid of this! */
/* Yariv: not sure why Shlomi changed itand wrote it */
#include "fdma.h"

#include "aiop_verification.h"
#include "aiop_verification_fdma.h"

void set_implicit_qdp(struct fdma_queueing_destination_params *qdp);

uint16_t aiop_verification_fdma(uint32_t asa_seg_addr)
{
	struct presentation_context *prc =
			(struct presentation_context *) HWC_PRC_ADDRESS;
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint32_t flags;


	opcode  = *((uint32_t *) asa_seg_addr);
	flags = 0x0;

	switch (opcode) {
	/* FDMA Initial frame presentation Command Verification */
	case FDMA_INIT_CMD_STR:
	{
		struct fdma_init_command *str =
			(struct fdma_init_command *) asa_seg_addr;
		prc->asapa_asaps	=
			(((uint16_t)(uint32_t)str->asa_address) &
					PRC_ASAPA_MASK) |
			((uint16_t)str->asa_size & PRC_ASAPS_MASK);
		if (str->SR)
			PRC_SET_SR_BIT();
		if (str->NDS)
			PRC_SET_NDS_BIT();
		prc->ptapa_asapo	=
			(((uint16_t)(uint32_t)str->pta_address) &
					PRC_PTAPA_MASK) |
			((uint16_t)str->asa_offset & PRC_ASAPO_MASK);
		prc->seg_length		= str->present_size;
		prc->seg_address	= (uint16_t)(uint32_t)str->seg_address;
		prc->seg_offset		= str->seg_offset;
		str->status = (int8_t)fdma_present_default_frame();
		str_size = (uint16_t)sizeof(struct fdma_init_command);
		break;
	}
	/* FDMA Initial frame presentation explicit Command Verification */
	case FDMA_INIT_EXP_CMD_STR:
	{
		struct fdma_init_exp_command *str =
			(struct fdma_init_exp_command *) asa_seg_addr;
		struct fdma_present_frame_params params;
		flags |= ((str->NDS) ? FDMA_INIT_NDS_BIT : 0x0);
		flags |= ((str->SR) ? FDMA_INIT_SR_BIT : 0x0);
		flags |= ((str->AS) ? FDMA_INIT_AS_BIT : 0x0);
		if (str->AS) {
			flags |= ((str->VA) ? FDMA_INIT_VA_BIT : 0x0);
			flags |= ((str->PL) ? FDMA_INIT_PL_BIT : 0x0);
			flags |= ((str->BDI) ? FDMA_INIT_BDI_BIT : 0x0);
			params.icid = str->icid;
		}
		params.asa_dst	= (void *)str->asa_dst;
		params.asa_offset	= str->asa_offset;
		params.asa_size		= str->asa_size;
		params.fd_src		= (struct ldpaa_fd *)str->fd_src;
		params.flags		= flags;
		params.present_size	= str->present_size;
		params.pta_dst	= (void *)str->pta_dst;
		params.seg_dst	= (void *)str->seg_dst;
		params.seg_offset	= str->seg_offset;
		str->status = (int8_t)fdma_present_frame(&params);
		str->frame_handle	= params.frame_handle;
		if (str->NDS == 1)
			str->seg_length	= 0;
		else {
			str->seg_length	= params.seg_length;
			str->seg_handle	= params.seg_handle;
		}
		str_size = (uint16_t)sizeof(struct fdma_init_exp_command);
		break;
	}
	/* FDMA Initial frame presentation without segments Command
	 * Verification */
	case FDMA_INIT_NO_SEG_CMD_STR:
	{
		struct fdma_init_no_seg_command *str =
			(struct fdma_init_no_seg_command *) asa_seg_addr;

		str->status =
			(int8_t)fdma_present_default_frame_without_segments();
		str_size = (uint16_t)sizeof(struct fdma_init_no_seg_command);
		break;
	}
	/* FDMA Initial frame presentation explicit Command Verification */
	case FDMA_INIT_NO_SEG_EXP_CMD_STR:
	{
		struct fdma_init_no_seg_exp_command *str =
			(struct fdma_init_no_seg_exp_command *) asa_seg_addr;

		flags |= ((str->AS) ? FDMA_INIT_AS_BIT : 0x0);
		if (str->AS) {
			flags |= ((str->VA) ? FDMA_INIT_VA_BIT : 0x0);
			flags |= ((str->PL) ? FDMA_INIT_PL_BIT : 0x0);
			flags |= ((str->BDI) ? FDMA_INIT_BDI_BIT : 0x0);
		}

		str->status = (int8_t)fdma_present_frame_without_segments(
				(void *)(str->fd_src), flags, str->icid,
				&str->frame_handle);

		str_size = (uint16_t)
			sizeof(struct fdma_init_no_seg_exp_command);
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
				str->present_size);
		str->seg_length = PRC_GET_SEGMENT_LENGTH();
		str->seg_handle = PRC_GET_SEGMENT_HANDLE();
		str_size = (uint16_t)sizeof(struct fdma_present_command);
		break;
	}
	/* FDMA Present segment Command Verification */
	case FDMA_PRESENT_DEF_SEG_CMD_STR:
	{
		struct fdma_present_default_seg_command *str =
			(struct fdma_present_default_seg_command *)asa_seg_addr;
		str->status =
			(int8_t)fdma_present_default_frame_default_segment();
		str->seg_length = PRC_GET_SEGMENT_LENGTH();
		str->seg_handle = PRC_GET_SEGMENT_HANDLE();
		str_size = (uint16_t)
			sizeof(struct fdma_present_default_seg_command);
		break;
	}
	/* FDMA Read ASA segment Command Verification */
	case FDMA_READ_ASA_CMD_STR:
	{
		struct fdma_read_asa_command *str =
			(struct fdma_read_asa_command *) asa_seg_addr;
		str->status = (int8_t)fdma_read_default_frame_asa(
				(void *)str->ws_dst,
				str->offset, str->present_size);
		str->seg_length = PRC_GET_ASA_SIZE();
		str_size = (uint16_t)sizeof(struct fdma_read_asa_command);
		break;
	}
	/* FDMA Read PTA segment Command Verification */
	case FDMA_READ_PTA_CMD_STR:
	{
		struct fdma_read_pta_command *str =
			(struct fdma_read_pta_command *) asa_seg_addr;
		str->status = (int8_t)fdma_read_default_frame_pta(
				(void *)str->ws_dst);
		if (LDPAA_FD_GET_PTA(HWC_FD_ADDRESS) && 
		    LDPAA_FD_GET_PTV1(HWC_FD_ADDRESS) &&
		    LDPAA_FD_GET_PTV2(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV1_2;
		else if (LDPAA_FD_GET_PTV1(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV1;
		else if (LDPAA_FD_GET_PTV2(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV2;
		else
			str->seg_length = PTA_SIZE_NO_PTA;
		str_size = (uint16_t)sizeof(struct fdma_read_pta_command);
		break;
	}
	/* FDMA Present segment Command Verification */
	case FDMA_PRESENT_EXP_CMD_STR:
	{
		struct fdma_present_exp_command *str =
			(struct fdma_present_exp_command *) asa_seg_addr;
		struct fdma_present_segment_params params;
		flags |= ((str->SR) ? FDMA_PRES_SR_BIT : 0x0);
		params.flags = flags;
		params.frame_handle = str->frame_handle;
		params.offset = str->offset;
		params.present_size = str->present_size;
		params.ws_dst = (void *)str->ws_dst;
		str->status = (int8_t)fdma_present_frame_segment(&params);
		str_size = (uint16_t)sizeof(struct fdma_present_exp_command);
		str->seg_length = params.seg_length;
		str->seg_handle = params.seg_handle;

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
		str->seg_length = prc->seg_length;
		str_size = (uint16_t)sizeof(struct fdma_extend_command);
		break;
	}
	/* FDMA Store Default Working Frame Command Verification */
	case FDMA_STORE_DEFAULT_WF_CMD_STR:
	{
		struct fdma_store_default_frame_command *str =
			(struct fdma_store_default_frame_command *)asa_seg_addr;
#ifdef AIOP_VERIF
		*(uint8_t *) HWC_SPID_ADDRESS = str->spid;
#endif
		str->status = (int8_t)fdma_store_default_frame_data();
		str_size = (uint16_t)
				sizeof(struct fdma_store_default_frame_command);
		break;
	}
	/* FDMA Store Working Frame Command Verification */
	case FDMA_STORE_WF_CMD_STR:
	{
		struct fdma_store_frame_command *str =
			(struct fdma_store_frame_command *) asa_seg_addr;
		struct fdma_amq amq;

		str->status = (int8_t)fdma_store_frame_data(str->frame_handle,
				str->spid, &amq);
		str->icid = amq.icid;
		str->BDI = (uint8_t)
			(amq.flags & FDMA_ICID_CONTEXT_BDI);
		str->PL = (uint8_t)
			(amq.flags & FDMA_ICID_CONTEXT_PL);
		str->VA = (uint8_t)
			(amq.flags & FDMA_ICID_CONTEXT_VA);
		str_size = (uint16_t)sizeof(struct fdma_store_frame_command);
		break;
	}
	/* FDMA Enqueue working frame Command Verification */
	case FDMA_ENQUEUE_WF_CMD_STR:
	{
		struct fdma_enqueue_wf_command *str =
			(struct fdma_enqueue_wf_command *)asa_seg_addr;
		struct fdma_queueing_destination_params qdp;
#ifdef AIOP_VERIF
		*(uint8_t *) HWC_SPID_ADDRESS = str->spid;
#endif
		flags |= (str->TC == 2) ?
			FDMA_EN_TC_CONDTERM_BITS : 
			((str->TC == 1) ? (FDMA_EN_TC_TERM_BITS) : 0x0);
		flags |= ((str->RL == 1) ? (FDMA_ENWF_RL_BIT) : 0x0);
		flags |= ((str->PS) ? FDMA_ENWF_PS_BIT : 0x0);

		if (str->EIS) {
			str->status = (int8_t)
				fdma_store_and_enqueue_default_frame_fqid(
					str->qd_fqid, flags);
		} else{
			if (str->implicit_qd_params) {
				set_implicit_qdp(&qdp);
			} else {
				qdp.qd = (uint16_t)(str->qd_fqid);
				qdp.qdbin = str->qdbin;
				qdp.qd_priority = str->qd_priority;
			}
			str->status = (int8_t)
				fdma_store_and_enqueue_default_frame_qd(
						&qdp, flags);
		}

		str_size = (uint16_t)sizeof(struct fdma_enqueue_wf_command);
		break;
	}
	/* FDMA Enqueue working frame explicit Command Verification */
	case FDMA_ENQUEUE_WF_EXP_CMD_STR:
	{
		struct fdma_enqueue_wf_exp_command *str =
			(struct fdma_enqueue_wf_exp_command *)asa_seg_addr;
		struct fdma_queueing_destination_params qdp;
		flags |= (str->TC == 2) ?
			FDMA_EN_TC_CONDTERM_BITS : 
			((str->TC == 1) ? (FDMA_EN_TC_TERM_BITS) : 0x0);
		flags |= ((str->PS) ? FDMA_ENWF_PS_BIT : 0x0);
		flags |= ((str->RL == 1) ? (FDMA_ENWF_RL_BIT) : 0x0);
		if (str->EIS) {
			str->status = (int8_t)
				fdma_store_and_enqueue_frame_fqid(
					str->frame_handle, flags,
					str->qd_fqid, str->spid);
		} else{
			if (str->implicit_qd_params) {
				set_implicit_qdp(&qdp);
			} else {
				qdp.qd = (uint16_t)(str->qd_fqid);
				qdp.qdbin = str->qdbin;
				qdp.qd_priority = str->qd_priority;
			}
			str->status = (int8_t)
				fdma_store_and_enqueue_frame_qd(
						str->frame_handle, flags,
						&qdp, str->spid);
		}

		str_size = (uint16_t)sizeof(struct fdma_enqueue_wf_exp_command);
		break;
	}
	/* FDMA Enqueue FD Command Verification */
	case FDMA_ENQUEUE_FRAME_CMD_STR:
	{
		struct fdma_enqueue_frame_command *str =
			(struct fdma_enqueue_frame_command *)asa_seg_addr;
		struct fdma_queueing_destination_params qdp;
		flags |= (str->TC == 2) ?
			FDMA_EN_TC_CONDTERM_BITS : 
			((str->TC == 1) ? (FDMA_EN_TC_TERM_BITS) : 0x0);
		flags |= ((str->PS) ? FDMA_ENF_PS_BIT : 0x0);
		flags |= ((str->BDI) ? FDMA_ENF_BDI_BIT : 0x0);
		flags |= ((str->RL == 1) ? (FDMA_ENF_RL_BIT) : 0x0);
		flags |= ((str->AS == 1) ? (FDMA_ENF_AS_BIT) : 0x0);

		if (str->EIS) {
			str->status = (int8_t)
				fdma_enqueue_default_fd_fqid(
					str->icid, flags, str->qd_fqid);
		} else{
			if (str->implicit_qd_params) {
				set_implicit_qdp(&qdp);
			} else {
				qdp.qd = (uint16_t)(str->qd_fqid);
				qdp.qdbin = str->qdbin;
				qdp.qd_priority = str->qd_priority;
			}
			str->status = (int8_t)
				fdma_enqueue_default_fd_qd(
						str->icid, flags, &qdp);
		}

		str_size = (uint16_t)sizeof(struct fdma_enqueue_frame_command);
		break;
	}
	/* FDMA Enqueue FD explicit Command Verification */
	case FDMA_ENQUEUE_FRAME_EXP_CMD_STR:
	{
		struct fdma_enqueue_frame_exp_command *str =
			(struct fdma_enqueue_frame_exp_command *)asa_seg_addr;
		struct fdma_queueing_destination_params qdp;
		flags |= (str->TC == 2) ?
			FDMA_EN_TC_CONDTERM_BITS : 
			((str->TC == 1) ? (FDMA_EN_TC_TERM_BITS) : 0x0);
		flags |= ((str->PS) ? FDMA_ENF_PS_BIT : 0x0);
		flags |= ((str->BDI) ? FDMA_ENF_BDI_BIT : 0x0);
		flags |= ((str->RL == 1) ? (FDMA_ENF_RL_BIT) : 0x0);
		flags |= ((str->AS == 1) ? (FDMA_ENF_AS_BIT) : 0x0);

		if (str->EIS) {
			str->status = (int8_t)
				fdma_enqueue_fd_fqid(str->fd, flags,
					str->qd_fqid, str->icid);
		} else{
			if (str->implicit_qd_params) {
				set_implicit_qdp(&qdp);
			} else {
				qdp.qd = (uint16_t)(str->qd_fqid);
				qdp.qdbin = str->qdbin;
				qdp.qd_priority = str->qd_priority;
			}
			str->status = (int8_t)
				fdma_enqueue_fd_qd(str->fd, flags,
						&qdp, str->icid);
		}

		str_size = (uint16_t)
				sizeof(struct fdma_enqueue_frame_exp_command);
		break;
	}
	/* FDMA Discard default frame Command Verification */
	case FDMA_DISCARD_DEFAULT_WF_CMD_STR:
	{
		struct fdma_discard_default_wf_command *str =
			(struct fdma_discard_default_wf_command *) asa_seg_addr;
		flags |= (str->TC == 1) ? FDMA_DIS_WF_TC_BIT : 0 ;
		fdma_discard_default_frame(flags);
		str->status = SUCCESS;
		str_size = (uint16_t)
				sizeof(struct fdma_discard_default_wf_command);
		break;
	}
	/* FDMA Discard frame Command Verification */
	case FDMA_DISCARD_WF_CMD_STR:
	{
		struct fdma_discard_wf_command *str =
			(struct fdma_discard_wf_command *) asa_seg_addr;
		flags |= (str->TC == 1) ? FDMA_DIS_WF_TC_BIT : 0 ;
		fdma_discard_frame(str->frame, flags);
		str->status = SUCCESS;
		str_size = (uint16_t)sizeof(struct fdma_discard_wf_command);
		break;
	}
	/* FDMA Discard FD Verification */
	case FDMA_DISCARD_FD_CMD_STR:
	{
		struct fdma_discard_fd_command *str =
			(struct fdma_discard_fd_command *) asa_seg_addr;

		flags |= (str->TC == 1) ? FDMA_DIS_FRAME_TC_BIT : 0 ;
		flags |= ((str->AS == 1) ? (FDMA_ENF_AS_BIT) : 0x0);
		flags |= ((str->BDI == 1) ? (FDMA_DIS_BDI_BIT) : 0x0);
		flags |= ((str->PL == 1) ? (FDMA_DIS_PL_BIT) : 0x0);
		flags |= ((str->VA == 1) ? (FDMA_DIS_VA_BIT) : 0x0);
		str->status = (int8_t)fdma_discard_fd(
			(struct ldpaa_fd *)(str->fd_dst), str->icid, flags);
		str_size = (uint16_t)sizeof(struct fdma_discard_fd_command);
		break;
	}
	/* FDMA Discard FD Verification */
	case FDMA_FORCE_DISCARD_FD_CMD_STR:
	{
		struct fdma_force_discard_fd_command *str =
			(struct fdma_force_discard_fd_command *) asa_seg_addr;

		flags |= (str->TC == 1) ? FDMA_DIS_FRAME_TC_BIT : 0 ;
		flags |= ((str->AS == 1) ? (FDMA_ENF_AS_BIT) : 0x0);
		flags |= ((str->BDI == 1) ? (FDMA_DIS_BDI_BIT) : 0x0);
		flags |= ((str->PL == 1) ? (FDMA_DIS_PL_BIT) : 0x0);
		flags |= ((str->VA == 1) ? (FDMA_DIS_VA_BIT) : 0x0);
		str->status = (int8_t)fdma_force_discard_fd(
				(struct ldpaa_fd *)(str->fd_dst), str->icid, flags);
		str_size = (uint16_t)
				sizeof(struct fdma_force_discard_fd_command);
		break;
	}
	/* FDMA Terminate Task Command Verification */
	case FDMA_TERMINATE_TASK_CMD_STR:
	{
		fdma_terminate_task();
		str_size = (uint16_t)sizeof(struct fdma_terminate_task_command);
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
		flags |= ((str->RL == 1) ? FDMA_REPLIC_RL_BIT : 0x0);
		flags |= (str->CFA == 3) ?
			FDMA_CFA_COPY_BIT :
			((str->CFA == 2) ?
				FDMA_CFA_PTA_COPY_BIT :
				((str->CFA == 1) ?
					FDMA_CFA_ASA_COPY_BIT :
					FDMA_CFA_NO_COPY_BIT));
		if (str->EIS) {
			str->status = (int8_t)fdma_replicate_frame_fqid(
					str->frame_handle1, str->spid,
					str->qd_fqid, (void *)str->fd_dst,
					flags, &(str->frame_handle2));
		} else{
			qdp.qd = (uint16_t)(str->qd_fqid);
			qdp.qdbin = str->qdbin;
			qdp.qd_priority = str->qd_priority;
			str->status = (int8_t)fdma_replicate_frame_qd(
					str->frame_handle1, str->spid,
					&qdp, (void *)str->fd_dst, flags,
					&(str->frame_handle2));
		}
		str_size = (uint16_t)
				sizeof(struct fdma_replicate_frames_command);
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
		str->amq = params.amq;
		str_size = (uint16_t)
				sizeof(struct fdma_concatenate_frames_command);
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
		params.fd_dst		= (struct ldpaa_fd *)str->fd_dst;
		params.flags		= flags;
		params.present_size	= str->present_size;
		params.split_size_sf	= str->split_size_sf;
		params.seg_dst	= (void *)str->seg_dst;
		params.seg_offset	= str->seg_offset;
		params.source_frame_handle = str->source_frame_handle;
		params.spid = str->spid;
		str->status = (int8_t)fdma_split_frame(&params);
		str->split_frame_handle	= params.split_frame_handle;
		if (str->PSA == 1) {
			str->seg_handle		= params.seg_handle;
			str->seg_length		= params.seg_length;
		} else {
			str->seg_handle		= 0;
			str->seg_length		= 0;
		}
		str_size = (uint16_t)
				sizeof(struct fdma_split_frame_command);
		break;
	}
	/* FDMA Trim Command Verification */
	case FDMA_TRIM_CMD_STR:
	{
		struct fdma_trim_command *str =
			(struct fdma_trim_command *) asa_seg_addr;
		fdma_trim_default_segment_presentation(str->offset, str->size);
		str->status = SUCCESS;
		str_size = (uint16_t)
				sizeof(struct fdma_trim_command);
		break;
	}
	/* FDMA Replace Command Verification */
	case FDMA_MODIFY_CMD_STR:
	{
		struct fdma_modify_command *str =
			(struct fdma_modify_command *) asa_seg_addr;

		fdma_modify_default_segment_data(str->offset, str->size);
		str->prc = *((struct presentation_context *)HWC_PRC_ADDRESS);
		str->prc.osrc_oep_osel_osrm = 0;
		str->prc.param = 0;
		str->status = SUCCESS;
		str_size = (uint16_t)
				sizeof(struct fdma_modify_command);
		break;
	}
	/* FDMA Replace Command Verification */
	case FDMA_MODIFY_FULL_SEG_CMD_STR:
	{
		struct fdma_modify_full_seg_command *str =
			(struct fdma_modify_full_seg_command *) asa_seg_addr;

		fdma_modify_default_segment_full_data();
		str->prc = *((struct presentation_context *)HWC_PRC_ADDRESS);
		str->prc.osrc_oep_osel_osrm = 0;
		str->prc.param = 0;
		str->status = SUCCESS;
		str_size = (uint16_t)
				sizeof(struct fdma_modify_full_seg_command);
		break;
	}
#ifdef REV2
	/* FDMA Replace Command Verification */
	case FDMA_MODIFY_SEG_CMD_STR:
	{
		struct fdma_modify_seg_command *str =
			(struct fdma_modify_seg_command *) asa_seg_addr;

		fdma_modify_segment_data(str->frame_handle, str->seg_handle,
			str->offset, str->size, (void *)(str->from_ws_src));
		str->status = SUCCESS;
		str_size = (uint16_t)
				sizeof(struct fdma_modify_seg_command);
		break;
	}
#endif /*REV2*/
	/* FDMA Replace Command Verification */
	case FDMA_REPLACE_CMD_STR:
	{
		struct fdma_replace_command *str =
			(struct fdma_replace_command *) asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT : 0));
		str->status = (int8_t)fdma_replace_default_segment_data(
				str->to_offset, str->to_size,
				(void *)str->from_ws_src, str->from_size,
				(void *)str->ws_dst_rs, str->size_rs, flags);
		str->prc = *((struct presentation_context *)HWC_PRC_ADDRESS);
		str->prc.osrc_oep_osel_osrm = 0;
		str->prc.param = 0;
		if (str->SA == 1)
			str->seg_length_rs = prc->seg_length;
		else if (str->SA == 2)
			str->seg_length_rs = 0;
		str_size = (uint16_t)
				sizeof(struct fdma_replace_command);
		break;
	}
	/* FDMA Insert data Command Verification */
	case FDMA_INSERT_DATA_CMD_STR:
	{
		struct fdma_insert_segment_data_command *str =
			(struct fdma_insert_segment_data_command *)asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT : 0));
		str->status = (int8_t)fdma_insert_default_segment_data(
				str->to_offset, (void *)str->from_ws_src,
				str->insert_size, flags);
		str->prc = *((struct presentation_context *)HWC_PRC_ADDRESS);
		str->prc.osrc_oep_osel_osrm = 0;
		str->prc.param = 0;
		if (str->SA == 1)
			str->seg_length_rs = prc->seg_length;
		else if (str->SA == 2)
			str->seg_length_rs = 0;
		str_size = (uint16_t)
				sizeof(struct fdma_insert_segment_data_command);
		break;
	}
	/* FDMA Insert data Command Verification */
	case FDMA_INSERT_EXP_DATA_CMD_STR:
	{
		struct fdma_insert_segment_data_exp_command *str =
		    (struct fdma_insert_segment_data_exp_command *)asa_seg_addr;
		struct fdma_insert_segment_data_params params;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT : 0));
		params.flags = flags;
		params.frame_handle = str->frame_handle;
		params.from_ws_src = (void *)str->from_ws_src;
		params.insert_size = str->insert_size;
		params.seg_handle = str->seg_handle;
		params.size_rs = str->size_rs;
		params.to_offset = str->to_offset;
		params.ws_dst_rs = (void *)str->ws_dst_rs;
		str->status = (int8_t)fdma_insert_segment_data(&params);
		if (str->SA == 1)
			str->seg_length_rs = params.seg_length_rs;
		else if (str->SA == 2)
			str->seg_length_rs = 0;
		str_size = (uint16_t)
			sizeof(struct fdma_insert_segment_data_exp_command);
		break;
	}
	/* FDMA Delete data Command Verification */
	case FDMA_DELETE_DATA_CMD_STR:
	{
		struct fdma_delete_segment_data_command *str =
			(struct fdma_delete_segment_data_command *)asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT : 0));
		str->status = (int8_t)fdma_delete_default_segment_data(
				str->to_offset, str->delete_target_size, flags);
		str->prc = *((struct presentation_context *)HWC_PRC_ADDRESS);
		str->prc.osrc_oep_osel_osrm = 0;
		str->prc.param = 0;
		if (str->SA == 1)
			str->seg_length_rs = prc->seg_length;
		else if (str->SA == 2)
			str->seg_length_rs = 0;
		str_size = (uint16_t)
				sizeof(struct fdma_delete_segment_data_command);
		break;
	}
	/* FDMA Delete data explicit Command Verification */
	case FDMA_DELETE_DATA_EXP_CMD_STR:
	{
		struct fdma_delete_segment_data_exp_command *str =
		    (struct fdma_delete_segment_data_exp_command *)asa_seg_addr;
		struct fdma_delete_segment_data_params params;

		params.flags = ((str->SA == 1) ?
			FDMA_REPLACE_SA_REPRESENT_BIT :
			((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT : 0));
		params.delete_target_size = str->delete_target_size;
		params.frame_handle = str->frame_handle;
		params.seg_handle = str->seg_handle;
		params.size_rs = str->size_rs;
		params.to_offset = str->to_offset;
		params.ws_dst_rs = (void *)(str->ws_dst_rs);

		str->status = (int8_t)fdma_delete_segment_data(&params);
		if (str->SA == 1)
			str->seg_length_rs = prc->seg_length;
		else if (str->SA == 2)
			str->seg_length_rs = 0;

		str_size = (uint16_t)
			sizeof(struct fdma_delete_segment_data_exp_command);
		break;
	}
	/* FDMA Close segment Command Verification */
	case FDMA_CLOSE_SEG_CMD_STR:
	{
		struct fdma_close_segment_command *str =
			(struct fdma_close_segment_command *)asa_seg_addr;
		fdma_close_default_segment();
		str->status = SUCCESS;
		str_size = (uint16_t)
				sizeof(struct fdma_close_segment_command);
		break;
	}
	/* FDMA Close segment Command Verification */
	case FDMA_CLOSE_SEG_EXP_CMD_STR:
	{
		struct fdma_close_segment_exp_command *str =
			(struct fdma_close_segment_exp_command *)asa_seg_addr;
		fdma_close_segment(str->frame_handle, str->seg_handle);
		str->status = SUCCESS;
		str_size = (uint16_t)
				sizeof(struct fdma_close_segment_exp_command);
		break;
	}
	/* FDMA Replace ASA Command Verification */
	case FDMA_REPLACE_ASA_CMD_STR:
	{
		struct fdma_replace_asa_command *str =
			(struct fdma_replace_asa_command *) asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT : 0));
		str->status = (int8_t)fdma_replace_default_asa_segment_data(
				str->to_offset, str->to_size,
				(void *)str->from_ws_src, str->from_size,
				(void *)str->ws_dst_rs, str->size_rs, flags);

		if (str->SA == 1)
			str->seg_length_rs = (*((uint16_t *)
				HWC_ACC_OUT_ADDRESS2)) & PRC_ASAPS_MASK;
		else if (str->SA == 2)
			str->seg_length_rs = 0;
		str_size = (uint16_t)
				sizeof(struct fdma_replace_asa_command);
		break;
	}
	/* FDMA Replace PTA Command Verification */
	case FDMA_REPLACE_PTA_CMD_STR:
	{
		struct fdma_replace_pta_command *str =
			(struct fdma_replace_pta_command *) asa_seg_addr;
		flags |= ((str->SA == 1) ? FDMA_REPLACE_SA_REPRESENT_BIT :
			 ((str->SA == 2) ? FDMA_REPLACE_SA_CLOSE_BIT : 0));
		str->status = (int8_t)fdma_replace_default_pta_segment_data(
				flags, (void *)str->from_ws_src,
				(void *)str->ws_dst_rs,
				(enum fdma_pta_size_type)str->size);
		if (str->SA == 2)
			str->seg_length = 0;
		else if (LDPAA_FD_GET_PTA(HWC_FD_ADDRESS) && 
			 LDPAA_FD_GET_PTV1(HWC_FD_ADDRESS) &&
			 LDPAA_FD_GET_PTV2(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV1_2;
		else if (LDPAA_FD_GET_PTV1(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV1;
		else if (LDPAA_FD_GET_PTV2(HWC_FD_ADDRESS))
			str->seg_length = PTA_SIZE_PTV2;
		else
			str->seg_length = PTA_SIZE_NO_PTA;
		str_size = (uint16_t)sizeof(struct fdma_replace_pta_command);
		break;
	}
	/* FDMA Checksum Command Verification */
	case FDMA_CKS_CMD_STR:
	{
		struct fdma_checksum_command *str =
			(struct fdma_checksum_command *) asa_seg_addr;
		fdma_calculate_default_frame_checksum(
				str->offset, str->size, &(str->checksum));
		str->status = SUCCESS;
		str_size = (uint16_t)sizeof(struct fdma_checksum_command);
		break;
	}
	/* FDMA Get Working frame length Command Verification */
	case FDMA_GWFL_CMD:
	{
		struct fdma_get_wf_length_command *str =
			(struct fdma_get_wf_length_command *) asa_seg_addr;
		get_frame_length(str->frame_handle, &(str->length));
		str->status = SUCCESS;
		str_size = (uint16_t)sizeof(struct fdma_get_wf_length_command);
		break;
	}
	/* FDMA Copy Command Verification */
	case FDMA_COPY_CMD_STR:
	{
		struct fdma_copy_command *str =
			(struct fdma_copy_command *) asa_seg_addr;
		flags |= ((str->SM) ? FDMA_COPY_SM_BIT : 0x0);
		flags |= ((str->DM) ? FDMA_COPY_DM_BIT : 0x0);
		fdma_copy_data(str->copy_size, flags,
				(void *)str->src, (void *)str->dst);
		str->status = SUCCESS;
		str_size = (uint16_t)sizeof(struct fdma_copy_command);
		break;
	}
	/* FDMA DMA Data Command Verification */
	case FDMA_DMA_CMD_STR:
	{
		struct fdma_dma_data_command *str =
			(struct fdma_dma_data_command *) asa_seg_addr;
		flags |= str->DA << 8;
		flags |= ((str->VA) ? FDMA_DMA_eVA_BIT : 0x0);
		flags |= ((str->BMT) ? FDMA_DMA_BMT_BIT : 0x0);
		flags |= ((str->PL) ? FDMA_DMA_PL_BIT : 0x0);
		fdma_dma_data(str->copy_size, str->icid, (void *)str->loc_addr,
				str->sys_addr, flags);
		str->status = SUCCESS;
		str_size = (uint16_t)sizeof(struct fdma_dma_data_command);
		break;
	}
	/* FDMA Acquire Buffer Verification */
	case FDMA_ACQUIRE_BUFFER_CMD_STR:
	{
		struct fdma_acquire_buffer_command *str =
			(struct fdma_acquire_buffer_command *) asa_seg_addr;
		flags |= ((str->bdi) ? FDMA_ACQUIRE_BDI_BIT : 0x0);
		str->status = (int8_t)fdma_acquire_buffer(str->icid, flags,
				str->bpid, (void *)str->dst);
		str_size = (uint16_t)sizeof(struct fdma_acquire_buffer_command);
		break;
	}
	/* FDMA Release Buffer Verification */
	case FDMA_RELEASE_BUFFER_CMD_STR:
	{
		struct fdma_release_buffer_command *str =
			(struct fdma_release_buffer_command *) asa_seg_addr;
		flags |= ((str->bdi) ? FDMA_ACQUIRE_BDI_BIT : 0x0);
		fdma_release_buffer(str->icid, flags, str->bpid, str->addr);
		str->status = SUCCESS;
		str_size = (uint16_t)sizeof(struct fdma_release_buffer_command);
		break;
	}
	/* FDMA create frame Command Verification */
	case FDMA_CREATE_FRAME_CMD_STR:
	{
		struct fdma_create_frame_command *str =
			(struct fdma_create_frame_command *) asa_seg_addr;
		str->status = (int8_t)create_frame(
				(struct ldpaa_fd *)(str->fd_src),
				(void *)(str->data), str->size,
				&(str->frame_handle));
		str_size = (uint16_t)sizeof(struct fdma_create_frame_command);
		break;
	}
	/* FDMA create FD Command Verification */
	case FDMA_CREATE_FD_CMD_STR:
	{
		struct fdma_create_fd_command *str =
			(struct fdma_create_fd_command *) asa_seg_addr;
		str->status = (int8_t)create_fd(
				(struct ldpaa_fd *)(str->fd_src),
				(void *)(str->data), str->size, str->spid);
		str->fd = *((struct ldpaa_fd *)(str->fd_src));
		str_size = (uint16_t)sizeof(struct fdma_create_fd_command);
		break;
	}
	/* FDMA create ARP request broadcast frame Command Verification */
	case FDMA_CREATE_FRAME_ARP_REQ_BRD_CMD_STR:
	{
		struct fdma_create_arp_request_broadcast_frame_command *str =
		    (struct fdma_create_arp_request_broadcast_frame_command *)
		    	    asa_seg_addr;
		str->status = (int8_t)create_arp_request_broadcast(
				(struct ldpaa_fd *)(str->fd_src),
				str->local_ip, str->target_ip,
				&(str->frame_handle));
		str_size = (uint16_t)sizeof(
			struct fdma_create_arp_request_broadcast_frame_command);
		break;
	}
	/* FDMA create ARP request frame Command Verification */
	case FDMA_CREATE_FRAME_ARP_REQ_CMD_STR:
	{
		struct fdma_create_arp_request_frame_command *str =
			(struct fdma_create_arp_request_frame_command *)
				asa_seg_addr;
		str->status = (int8_t)create_arp_request(
				(struct ldpaa_fd *)(str->fd_src),
				str->local_ip, str->target_ip, str->target_eth,
				&(str->frame_handle));
		str_size = (uint16_t)sizeof(
			struct fdma_create_arp_request_frame_command);
		break;
	}
	default:
	{
		return STR_SIZE_ERR;
	}
	}


	return str_size;
}

void set_implicit_qdp(struct fdma_queueing_destination_params *qdp)
{
	struct dpni_drv *dpni_drv;
	
	/* calculate pointer to the send NI structure   */
	dpni_drv = nis + PRC_GET_PARAMETER(); 
	
	qdp->qdbin = 0;
	qdp->qd = dpni_drv->dpni_drv_tx_params_var.qdid;
	qdp->qd_priority = default_task_params.qd_priority;
}
