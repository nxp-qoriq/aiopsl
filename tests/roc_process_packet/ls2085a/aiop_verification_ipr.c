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
@File          aiop_verification_ipr.c

@Description   This file contains the AIOP IPR FMs SW Verification
*//***************************************************************************/

#include "aiop_verification.h"
#include "aiop_verification_ipr.h"
#include "aiop_verification_data.h"
#include "fsl_frame_operations.h"

extern __VERIF_GLOBAL uint64_t verif_ipr_instance_handle[16];
extern __PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];


uint16_t aiop_verification_ipr(uint32_t asa_seg_addr)
{
	
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint64_t ipr_instance_handle;
	struct scope_status_params scope_status;


#ifdef CLOSE_MODEL
	ipr_instance_handle_t ipr_instance;
	ipr_instance_handle_t *ipr_instance_ptr;
#endif
	opcode  = *((uint32_t *) asa_seg_addr);


	switch (opcode) {
		/* IPR Init Command Verification */
		case IPR_VERIF_INIT_CMD_STR:
		{
			struct ipr_init_verif_command *str =
				(struct ipr_init_verif_command *) asa_seg_addr;
			/* ipr_init(str->max_buffers, str->flags); */
			ipr_init();
			str_size = sizeof(struct ipr_init_verif_command);
			break;
		}
		/* IPR create instance Command Verification */
		case IPR_CREATE_INSTANCE_CMD_STR:
		{
			struct ipr_create_instance_command *str =
				(struct ipr_create_instance_command *) asa_seg_addr;
		/*	str->status = ipr_create_instance(
					&(str->ipr_params),
					&verif_ipr_instance_handle);*/
			/* Initialize Time out callback function */
			str->ipr_params.ipv4_timeout_cb = &ipr_timeout_cb_verif;
			str->ipr_params.ipv6_timeout_cb = &ipr_timeout_cb_verif;

			str->status = ipr_create_instance(
					&(str->ipr_params),
					&ipr_instance_handle);

			verif_ipr_instance_handle[str->instance_index] =
							ipr_instance_handle;
			str_size = sizeof(struct ipr_create_instance_command);
			break;
		}
		/* IPR reassemble Command Verification */
		case IPR_REASSEMBLE_CMD_STR:
		{
			struct ipr_reassemble_command *str =
				(struct ipr_reassemble_command *) asa_seg_addr;
			str->status = 
		ipr_reassemble(verif_ipr_instance_handle[str->instance_index]);
			str->pr = *((struct parse_result *) HWC_PARSE_RES_ADDRESS);
			str_size = sizeof(struct ipr_reassemble_command);
			/* Get OSM status (ordering scope mode and levels) */
			osm_get_scope(&scope_status);
			if(scope_status.scope_mode == CONCURRENT)
		osm_scope_transition_to_exclusive_with_increment_scope_id();
			break;
		}
#ifdef CLOSE_MODEL
		case IPR_CREATE_INSTANCE_CMD_STR:
		{
			struct ipr_create_instance_command *str =
				(struct ipr_create_instance_command *) asa_seg_addr;
			if(str->ipr_instance_ref)
				ipr_instance_ptr = 
				(ipr_instance_handle_t*) str->ipr_instance;
			else 
				ipr_instance_ptr = &(str->ipr_instance);

			
			
			str->status = ipr_create_instance(
					&(str->ipr_params),
					ipr_instance_ptr);
			str_size = sizeof(struct ipr_create_instance_command);
			break;
		}
		/* IPR reassemble Command Verification */
		case IPR_REASSEMBLE_CMD_STR:
		{
			struct ipr_reassemble_command *str =
				(struct ipr_reassemble_command *) asa_seg_addr;
			if(str->ipr_instance_ref)
				ipr_instance = 
		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
			else 
				ipr_instance = str->ipr_instance;

			str->status = ipr_reassemble(ipr_instance);
			str_size = sizeof(struct ipr_reassemble_command);
			break;
		}
#endif
		
		/* IPR delete instance Command Verification */
		case IPR_DELETE_INSTANCE_CMD_STR:
		{
			struct ipr_delete_instance_command *str =
				(struct ipr_delete_instance_command *) asa_seg_addr;
//			if(str->ipr_instance_ref)
//				ipr_instance = 
//		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
//			else 
//				ipr_instance = str->ipr_instance;

			str->confirm_delete_cb = (ipr_del_cb_t *)
						 &ipr_delete_instance_cb_verif;

			str->status = ipr_delete_instance(
				verif_ipr_instance_handle[str->instance_index],
				str->confirm_delete_cb,
				str->delete_arg);
			str_size = sizeof(struct ipr_delete_instance_command);
			break;
		}
		/* IPR max reassembled frame size Command Verification */
		case IPR_MODIFY_REASS_FRM_SIZE_CMD_STR:
		{
			struct ipr_modify_max_reass_frm_size_command *str =
				(struct ipr_modify_max_reass_frm_size_command *) asa_seg_addr;
 //           if(str->ipr_instance_ref)
 //           	ipr_instance = 
 //           		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
 //           else 
 //           	ipr_instance = str->ipr_instance;

			ipr_modify_max_reass_frm_size(
				verif_ipr_instance_handle[str->instance_index],
				str->max_reass_frm_size);
			str_size = sizeof(struct ipr_modify_max_reass_frm_size_command);
			break;
		}
		/* IPR modify min fragment size Command Verification */
		case IPR_MODIFY_MODIFY_MIN_FRAG_SIZE_IPV4_CMD_STR:
		{
			struct ipr_modify_min_frag_size_command *str =
				(struct ipr_modify_min_frag_size_command *) asa_seg_addr;
 //           if(str->ipr_instance_ref)
 //           	ipr_instance = 
 //           		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
 //           else 
 //           	ipr_instance = str->ipr_instance;

			ipr_modify_min_frag_size_ipv4(
				verif_ipr_instance_handle[str->instance_index],
				str->min_frag_size);
			str_size = sizeof(struct ipr_modify_min_frag_size_command);
			break;
		}
		case IPR_MODIFY_MODIFY_MIN_FRAG_SIZE_IPV6_CMD_STR:
		{
			struct ipr_modify_min_frag_size_command *str =
				(struct ipr_modify_min_frag_size_command *) asa_seg_addr;
 //           if(str->ipr_instance_ref)
 //           	ipr_instance = 
 //           		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
 //           else 
 //           	ipr_instance = str->ipr_instance;

			ipr_modify_min_frag_size_ipv6(
				verif_ipr_instance_handle[str->instance_index],
				str->min_frag_size);
			str_size = sizeof(struct ipr_modify_min_frag_size_command);
			break;
		}

		/* IPR modify IPv4 TO value Command Verification */
		case IPR_MODIFY_TO_VALUE_IPV4_CMD_STR:
		{
			struct ipr_modify_timeout_value_ipv4_command *str =
				(struct ipr_modify_timeout_value_ipv4_command *) asa_seg_addr;
/*            if(str->ipr_instance_ref)
            	ipr_instance = 
            		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
            else 
            	ipr_instance = str->ipr_instance;
*/
			ipr_modify_timeout_value_ipv4(
				verif_ipr_instance_handle[str->instance_index],
				str->reasm_timeout_value_ipv4);
			str_size = sizeof(struct ipr_modify_timeout_value_ipv4_command);
			break;
		}
		/* IPR modify IPv6 TO value Command Verification */
		case IPR_MODIFY_TO_VALUE_IPV6_CMD_STR:
		{
			struct ipr_modify_timeout_value_ipv6_command *str =
				(struct ipr_modify_timeout_value_ipv6_command *) asa_seg_addr;
/*            if(str->ipr_instance_ref)
            	ipr_instance = 
            		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
            else 
            	ipr_instance = str->ipr_instance;
*/
			ipr_modify_timeout_value_ipv6(
				verif_ipr_instance_handle[str->instance_index],
				str->reasm_timeout_value_ipv6);
			str_size = sizeof(struct ipr_modify_timeout_value_ipv6_command);
			break;
		}
		/* IPR get reassembled frame counter Command Verification */
		case IPR_GET_REASS_FRM_CNTR_CMD_STR:
		{
			struct ipr_get_reass_frm_cntr_command *str =
				(struct ipr_get_reass_frm_cntr_command *) asa_seg_addr;
/*            if(str->ipr_instance_ref)
            	ipr_instance = 
            		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
            else 
            	ipr_instance = str->ipr_instance;
*/
			ipr_get_reass_frm_cntr(
				verif_ipr_instance_handle[str->instance_index],
				str->flags,
				&str->reass_frm_cntr);
			str_size = sizeof(struct ipr_get_reass_frm_cntr_command);
			break;
		}
		default:
		{
			return STR_SIZE_ERR;
		}
	}


	return str_size;
}

void ipr_delete_instance_cb_verif(uint64_t arg)
{
	struct ipr_fdma_enqueue_wf_command str;
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
			(uint16_t)sizeof(struct ipr_fdma_enqueue_wf_command));
	
        /* setting SPID = 0 */
        *((uint8_t *)HWC_SPID_ADDRESS) = str.spid;
        icid = (uint16_t)(storage_profile[str.spid].ip_secific_sp_info >> 48);
        icid = ((icid << 8) & 0xff00) | ((icid >> 8) & 0xff);
        tmp = (uint8_t)(storage_profile[0].ip_secific_sp_info >> 40);
        if (tmp & 0x08)
               flags |= FDMA_ICID_CONTEXT_BDI;
        if (tmp & 0x04)
               flags |= FDMA_ICID_CONTEXT_PL;
        if (storage_profile[0].mode_bits2 & sp1_mode_bits2_VA_MASK)
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

void ipr_timeout_cb_verif(uint64_t arg, uint32_t flags)
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

	if(flags == IPR_TO_CB_FIRST_FRAG) {
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

