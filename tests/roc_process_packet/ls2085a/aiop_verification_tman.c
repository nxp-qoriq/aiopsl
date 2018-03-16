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
@File          aiop_verification_tman.c

@Description   This file contains the AIOP TMAN SRs SW Verification 01_01
*//***************************************************************************/

#include "fsl_tman.h"
#include "tman.h" /* only to check tman_query_tmi_sw function */
#include "fsl_frame_operations.h"

#include "aiop_verification.h"
#include "aiop_verification_tman.h"

extern __VERIF_GLOBAL uint8_t tmi_id;
__VERIF_GLOBAL uint8_t tman_spid;
__VERIF_GLOBAL struct fdma_amq tman_amq;

extern uint32_t tman_tmi_max_num_of_timers[256];

uint16_t aiop_verification_tman(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;


	opcode  = *((uint32_t *) asa_seg_addr);


	switch (opcode) {
	/* TMAN TMI Create Command Verification */
	case TMAN_TMI_CREATE_CMD_STR:
	{
		tman_spid = *((uint8_t *)HWC_SPID_ADDRESS);
		get_default_amq_attributes(&tman_amq);
		struct tman_tmi_create_command *str =
			(struct tman_tmi_create_command *) asa_seg_addr;
		str->status = tman_create_tmi(
				str->tmi_mem_base_addr,
				str->max_num_of_timers, &(str->tmi_id));
		tmi_id = str->tmi_id;
		str_size = sizeof(struct tman_tmi_create_command);
		break;
	}
	/* TMAN TMI delete Command Verification */
	case TMAN_TMI_DELETE_CMD_STR:
	{
		struct tman_tmi_delete_command *str =
			(struct tman_tmi_delete_command *) asa_seg_addr;
		if(str->cb_with_confirmation)
			tman_delete_tmi(
				&verif_tman_callback,
				str->mode_bits,
				str->tmi_id,
				str->conf_opaque_data1,
				str->conf_opaque_data2);
		else
			tman_delete_tmi(
				&verif_tman_callback_no_conf,
				str->mode_bits,
				str->tmi_id,
				str->conf_opaque_data1,
				str->conf_opaque_data2);
		str->max_num_of_timers = 
				tman_tmi_max_num_of_timers[str->tmi_id];
		str_size = sizeof(struct tman_tmi_delete_command);
		break;
	}
	case TMAN_TMI_QUERY_SW_CMD_STR:
	{
		struct tman_tmi_query_sw_command *str =
			(struct tman_tmi_query_sw_command *) asa_seg_addr;
		str->status = tman_query_tmi_sw(str->tmi_id);
		str_size = sizeof(struct tman_tmi_query_sw_command);
		break;
	}
	/* TMAN timer create Command Verification */
	case TMAN_TIMER_CREATE_CMD_STR:
	{
		struct tman_timer_create_command *str =
		(struct tman_timer_create_command *) asa_seg_addr;
		if(str->cb_with_confirmation)
			str->status = tman_create_timer(
				str->tmi_id,
				str->mode_bits,
				str->duration,
				str->opaque_data1,
				str->opaque_data2,
				&verif_tman_callback,
				&(str->timer_handle));
		else
			str->status = tman_create_timer(
				str->tmi_id,
				str->mode_bits,
				str->duration,
				str->opaque_data1,
				str->opaque_data2,
				&verif_tman_callback_no_conf,
				&(str->timer_handle));
		str_size = sizeof(struct tman_timer_create_command);
		break;
	}
	/* TMAN timer delete Command Verification */
	case TMAN_TIMER_DELETE_CMD_STR:
	{
		struct tman_timer_delete_command *str =
		(struct tman_timer_delete_command *) asa_seg_addr;
		str->status = tman_delete_timer(
				str->timer_handle,
				str->mode_bits);
		str_size = sizeof(struct tman_timer_delete_command);
		break;
	}
	/* TMAN timer recharge Command Verification */
	case TMAN_TIMER_RECHARGE_CMD_STR:
	{
		struct tman_timer_recharge_command *str =
		(struct tman_timer_recharge_command *) asa_seg_addr;
		tman_recharge_timer(str->timer_handle);
		str_size = sizeof(struct tman_timer_recharge_command);
		break;
	}
	/* TMAN timer query Command Verification */
	case TMAN_TIMER_QUERY_CMD_STR:
	{
		struct tman_timer_query_command *str =
		(struct tman_timer_query_command *) asa_seg_addr;
		tman_query_timer(str->timer_handle, &(str->state));
		str_size = sizeof(struct tman_timer_query_command);
		break;
	}
	/* TMAN timer completion confirmation Command Verification */
	case TMAN_TIMER_COMPLETION_CONF_CMD_STR:
	{
		struct tman_timer_comp_conf_command *str =
		(struct tman_timer_comp_conf_command *) asa_seg_addr;
		tman_timer_completion_confirmation(
				str->timer_handle);
		str_size = sizeof(struct tman_timer_comp_conf_command);
		break;
	}
	/* TMAN get TS Command Verification */
	case TMAN_GET_TS_CMD_STR:
	{
		struct tman_get_ts_command *str =
		(struct tman_get_ts_command *) asa_seg_addr;
		tman_get_timestamp(&(str->timestamp));
		str_size = sizeof(struct tman_get_ts_command);
		break;
	}

	default:
	{
		return STR_SIZE_ERR;
	}
	}


	return str_size;
}

void verif_tman_callback_no_conf(uint64_t opaque1, uint16_t opaque2)
{
	uint8_t frame_handle;
	
	*((uint8_t *)HWC_SPID_ADDRESS) = tman_spid;
	set_default_amq_attributes(&tman_amq);

	
	fdma_store_default_frame_data();
	create_frame((struct ldpaa_fd *)HWC_FD_ADDRESS,&opaque1,
				 sizeof(opaque1), &frame_handle);
	fdma_store_and_enqueue_frame_fqid(frame_handle, FDMA_EN_TC_TERM_BITS,
		(uint32_t)opaque2, tman_spid);
}


void verif_tman_callback(uint64_t opaque1, uint16_t opaque2)
{
	uint8_t frame_handle;
	
	
	*((uint8_t *)HWC_SPID_ADDRESS) = tman_spid;
	set_default_amq_attributes(&tman_amq);

	tman_timer_completion_confirmation(
			TMAN_GET_TIMER_HANDLE(HWC_FD_ADDRESS));
	/* TODO there is an issue that the create frame overwrites the FD where
	the TMAN parameters are saved */

	fdma_store_default_frame_data();
	create_frame((struct ldpaa_fd *)HWC_FD_ADDRESS,&opaque1,
			     sizeof(opaque1), &frame_handle);
	fdma_store_and_enqueue_frame_fqid(frame_handle, FDMA_EN_TC_TERM_BITS,
			(uint32_t)opaque2, tman_spid);
}
