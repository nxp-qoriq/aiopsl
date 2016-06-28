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
@File          aiop_verification_cwapr.c

@Description   This file contains the AIOP CWAPR FMs SW Verification
*//***************************************************************************/

#include "aiop_verification.h"
#include "aiop_verification_cwapr.h"
#include "aiop_verification_data.h"
#include "fsl_frame_operations.h"

extern __VERIF_GLOBAL uint64_t verif_cwapr_instance_handle[16];
extern __PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];


uint16_t aiop_verification_cwapr(uint32_t asa_seg_addr)
{
	
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint64_t cwapr_instance_handle;
	struct scope_status_params scope_status;


#ifdef CLOSE_MODEL
	cwapr_instance_handle_t cwapr_instance;
	cwapr_instance_handle_t *cwapr_instance_ptr;
#endif
	opcode  = *((uint32_t *) asa_seg_addr);


	switch (opcode) {
		/* CWAPR Init Command Verification */
		case CWAPR_VERIF_INIT_CMD_STR:
		{
			struct cwapr_init_verif_command *str =
				(struct cwapr_init_verif_command *) asa_seg_addr;
			/* cwapr_init(str->max_buffers, str->flags); */
			cwapr_init();
			str_size = sizeof(struct cwapr_init_verif_command);
			break;
		}
		/* CWAPR create instance Command Verification */
		case CWAPR_CREATE_INSTANCE_CMD_STR:
		{
			struct cwapr_create_instance_command *str =
				(struct cwapr_create_instance_command *) asa_seg_addr;
		/*	str->status = cwapr_create_instance(
					&(str->cwapr_params),
					&verif_cwapr_instance_handle);*/
			/* Initialize Time out callback function */
			str->cwapr_params.timeout_cb = &cwapr_timeout_cb_verif;

			str->status = cwapr_create_instance(
					&(str->cwapr_params),
					&cwapr_instance_handle);

			verif_cwapr_instance_handle[str->instance_index] =
							cwapr_instance_handle;
			str_size = sizeof(struct cwapr_create_instance_command);
			break;
		}
		/* CWAPR reassemble Command Verification */
		case CWAPR_REASSEMBLE_CMD_STR:
		{
			struct cwapr_reassemble_command *str =
				(struct cwapr_reassemble_command *) asa_seg_addr;
			str->status = cwapr_reassemble(
					verif_cwapr_instance_handle[str->instance_index], 
					str->tunnel_id);
			str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
			str_size = sizeof(struct cwapr_reassemble_command);
			/* Get OSM status (ordering scope mode and levels) */
			osm_get_scope(&scope_status);
			if(scope_status.scope_mode == CONCURRENT)
				osm_scope_transition_to_exclusive_with_increment_scope_id();
			break;
		}
#ifdef CLOSE_MODEL
		case CWAPR_CREATE_INSTANCE_CMD_STR:
		{
			struct cwapr_create_instance_command *str =
				(struct cwapr_create_instance_command *) asa_seg_addr;
			if(str->cwapr_instance_ref)
				cwapr_instance_ptr = 
				(cwapr_instance_handle_t*) str->cwapr_instance;
			else 
				cwapr_instance_ptr = &(str->cwapr_instance);

			
			
			str->status = cwapr_create_instance(
					&(str->cwapr_params),
					cwapr_instance_ptr);
			str_size = sizeof(struct cwapr_create_instance_command);
			break;
		}
		/* CWAPR reassemble Command Verification */
		case CWAPR_REASSEMBLE_CMD_STR:
		{
			struct cwapr_reassemble_command *str =
				(struct cwapr_reassemble_command *) asa_seg_addr;
			if(str->cwapr_instance_ref)
				cwapr_instance = 
		(cwapr_instance_handle_t)*((uint32_t *) str->cwapr_instance);
			else 
				cwapr_instance = str->cwapr_instance;

			str->status = cwapr_reassemble(cwapr_instance);
			str_size = sizeof(struct cwapr_reassemble_command);
			break;
		}
#endif
		
		/* CWAPR delete instance Command Verification */
		case CWAPR_DELETE_INSTANCE_CMD_STR:
		{
			struct cwapr_delete_instance_command *str =
				(struct cwapr_delete_instance_command *) asa_seg_addr;
//			if(str->cwapr_instance_ref)
//				cwapr_instance = 
//		(cwapr_instance_handle_t)*((uint32_t *) str->cwapr_instance);
//			else 
//				cwapr_instance = str->cwapr_instance;

			str->confirm_delete_cb = (cwapr_del_cb_t *)
						 &cwapr_delete_instance_cb_verif;

			str->status = cwapr_delete_instance(
				verif_cwapr_instance_handle[str->instance_index],
				str->confirm_delete_cb,
				str->delete_arg);
			str_size = sizeof(struct cwapr_delete_instance_command);
			break;
		}
		/* CWAPR max reassembled frame size Command Verification */
		case CWAPR_MODIFY_REASS_FRM_SIZE_CMD_STR:
		{
			struct cwapr_modify_max_reass_frm_size_command *str =
				(struct cwapr_modify_max_reass_frm_size_command *) asa_seg_addr;
 //           if(str->cwapr_instance_ref)
 //           	cwapr_instance = 
 //           		(cwapr_instance_handle_t)*((uint32_t *) str->cwapr_instance);
 //           else 
 //           	cwapr_instance = str->cwapr_instance;

			cwapr_modify_max_reass_frm_size(
				verif_cwapr_instance_handle[str->instance_index],
				str->max_reass_frm_size);
			str_size = sizeof(struct cwapr_modify_max_reass_frm_size_command);
			break;
		}

		/* CWAPR modify TO value Command Verification */
		case CWAPR_MODIFY_TO_VALUE_CMD_STR:
		{
			struct cwapr_modify_timeout_value_command *str =
				(struct cwapr_modify_timeout_value_command *) asa_seg_addr;
/*            if(str->cwapr_instance_ref)
            	cwapr_instance = 
            		(cwapr_instance_handle_t)*((uint32_t *) str->cwapr_instance);
            else 
            	cwapr_instance = str->cwapr_instance;
*/
			cwapr_modify_timeout_value(
				verif_cwapr_instance_handle[str->instance_index],
				str->reasm_timeout_value);
			str_size = sizeof(struct cwapr_modify_timeout_value_command);
			break;
		}
		/* CWAPR get reassembled frame counter Command Verification */
		case CWAPR_GET_REASS_FRM_CNTR_CMD_STR:
		{
			struct cwapr_get_reass_frm_cntr_command *str =
				(struct cwapr_get_reass_frm_cntr_command *) asa_seg_addr;
/*            if(str->cwapr_instance_ref)
            	cwapr_instance = 
            		(cwapr_instance_handle_t)*((uint32_t *) str->cwapr_instance);
            else 
            	cwapr_instance = str->cwapr_instance;
*/
			cwapr_get_reass_frm_cntr(
				verif_cwapr_instance_handle[str->instance_index],
				&str->reass_frm_cntr);
			str_size = sizeof(struct cwapr_get_reass_frm_cntr_command);
			break;
		}
		default:
		{
			return STR_SIZE_ERR;
		}
	}


	return str_size;
}

void cwapr_delete_instance_cb_verif(uint64_t arg)
{
	struct cwapr_fdma_enqueue_wf_command str;
	struct fdma_queueing_destination_params qdp;
	uint8_t dummy_data;
	uint32_t flags = 0;
	uint8_t frame_handle;
        struct fdma_amq amq;
        uint16_t icid;
        uint8_t tmp;

	if (arg == 0)
		return;
	cdma_read((void *)&str, arg,
			(uint16_t)sizeof(struct cwapr_fdma_enqueue_wf_command));
	
	/* setting SPID */
        *((uint8_t *)HWC_SPID_ADDRESS) = str.spid;
        icid = (uint16_t)(storage_profile[str.spid].ip_secific_sp_info >> 48);
        icid = ((icid << 8) & 0xff00) | ((icid >> 8) & 0xff);
	tmp = (uint8_t)(storage_profile[str.spid].ip_secific_sp_info >> 40);
        if (tmp & 0x08)
               flags |= FDMA_ICID_CONTEXT_BDI;
        if (tmp & 0x04)
               flags |= FDMA_ICID_CONTEXT_PL;
	if (storage_profile[str.spid].mode_bits2 & sp1_mode_bits2_VA_MASK)
		flags |= FDMA_ICID_CONTEXT_VA;
        amq.icid = icid;
        amq.flags = (uint16_t) flags;
        set_default_amq_attributes(&amq);

	create_frame((struct ldpaa_fd *)HWC_FD_ADDRESS,&dummy_data, 1, 
			&frame_handle);


	flags = ((str.TC == 1) ? (FDMA_EN_TC_TERM_BITS) : 0x0);
	flags |= ((str.PS) ? FDMA_ENWF_PS_BIT : 0x0);

	if (str.EIS) {
		fdma_store_and_enqueue_frame_fqid(
				frame_handle,
				flags,
				str.qd_fqid,
				str.spid);

	} else{
		qdp.qd = (uint16_t)(str.qd_fqid);
		qdp.qdbin = str.qdbin;
		qdp.qd_priority = str.qd_priority;
		fdma_store_and_enqueue_frame_qd(
			frame_handle,
			flags,
			&qdp,
			str.spid);

	}
	fdma_terminate_task();
}

void cwapr_timeout_cb_verif(uint64_t arg, uint32_t flags)
{
	struct fdma_enqueue_wf_command str;
	struct fdma_queueing_destination_params qdp;
	uint64_t addr;
	uint32_t fdma_flags = 0;

	if (arg == 0)
		return;
	cdma_read((void *)&str, arg,
			(uint16_t)sizeof(struct fdma_enqueue_wf_command));

	*(uint8_t *) HWC_SPID_ADDRESS = str.spid;
	fdma_flags |= ((str.TC == 1) ? (FDMA_EN_TC_TERM_BITS) : 0x0);
	fdma_flags |= ((str.PS) ? FDMA_ENWF_PS_BIT : 0x0);

	if(flags == CWAPR_TO_CB_FIRST_FRAG) {
		addr = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
		addr |= 1;
		LDPAA_FD_SET_ADDR(HWC_FD_ADDRESS, addr);
	}
	if (str.EIS) {
		str.status = (int8_t)
			fdma_store_and_enqueue_default_frame_fqid(
				str.qd_fqid, fdma_flags);
	} else{
		qdp.qd = (uint16_t)(str.qd_fqid);
		qdp.qdbin = str.qdbin;
		qdp.qd_priority = str.qd_priority;
		str.status = (int8_t)
			fdma_store_and_enqueue_default_frame_qd(
					&qdp, fdma_flags);
	}
	fdma_terminate_task();
}

