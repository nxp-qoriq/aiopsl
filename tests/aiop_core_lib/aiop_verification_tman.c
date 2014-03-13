/**************************************************************************//**
@File          aiop_verification_tman.c

@Description   This file contains the AIOP TMAN SRs SW Verification
*//***************************************************************************/

#include "dplib/fsl_tman.h"

#include "aiop_verification.h"
#include "aiop_verification_tman.h"


uint16_t aiop_verification_tman(uint32_t asa_seg_addr)
{
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;


	opcode  = *((uint32_t *) asa_seg_addr);


	switch (opcode) {
	/* TMAN TMI Create Command Verification */
	case TMAN_TMI_CREATE_CMD_STR:
	{
		struct tman_tmi_create_command *str =
			(struct tman_tmi_create_command *) asa_seg_addr;
		str->status = tman_create_tmi(
				str->tmi_mem_base_addr,
				str->max_num_of_timers, &(str->tmi_id));
		str_size = sizeof(struct tman_tmi_create_command);
		break;
	}
	/* TMAN TMI delete Command Verification */
	case TMAN_TMI_DELETE_CMD_STR:
	{
		struct tman_tmi_delete_command *str =
			(struct tman_tmi_delete_command *) asa_seg_addr;
		str->status = tman_delete_tmi(
				&verif_tmi_delete_callback,
				str->mode_bits,
				str->tmi_id,
				str->conf_opaque_data1,
				str->conf_opaque_data2);
		str_size = sizeof(struct tman_tmi_delete_command);
		break;
	}
	/* TMAN TMI query Command Verification */
	case TMAN_TMI_QUERY_CMD_STR:
	{
		struct tman_tmi_query_command *str =
			(struct tman_tmi_query_command *) asa_seg_addr;
		str->status = tman_query_tmi(
				str->tmi_id,
				&(str->tmi_params));
		str_size = sizeof(struct tman_tmi_query_command);
		break;
	}
	/* TMAN timer create Command Verification */
	case TMAN_TIMER_CREATE_CMD_STR:
	{
		struct tman_timer_create_command *str =
		(struct tman_timer_create_command *) asa_seg_addr;
		str->status = tman_create_timer(
				str->tmi_id,
				str->mode_bits,
				str->duration,
				str->opaque_data1,
				str->opaque_data2,
				&verif_timer_callback,
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
	/* TMAN timer increase duration Command Verification */
	case TMAN_TIMER_INC_DURATION_CMD_STR:
	{
		struct tman_timer_increase_duration_command *str =
		(struct tman_timer_increase_duration_command *) asa_seg_addr;
		str->status = tman_increase_timer_duration(
				str->timer_handle,
				str->duration);
		str_size = sizeof(struct tman_timer_increase_duration_command);
		break;
	}
	/* TMAN timer recharge Command Verification */
	case TMAN_TIMER_RECHARGE_CMD_STR:
	{
		struct tman_timer_recharge_command *str =
		(struct tman_timer_recharge_command *) asa_seg_addr;
		str->status = tman_recharge_timer(
				str->timer_handle);
		str_size = sizeof(struct tman_timer_recharge_command);
		break;
	}
	/* TMAN timer query Command Verification */
	case TMAN_TIMER_QUERY_CMD_STR:
	{
		struct tman_timer_query_command *str =
		(struct tman_timer_query_command *) asa_seg_addr;
		str->status = tman_query_timer(
				str->timer_handle,
				&(str->state));
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

void verif_timer_callback(uint64_t opaque1, uint16_t opaque2)
{
	opaque2 = 0;
	opaque1 = 0;
	tman_timer_completion_confirmation(
			TMAN_GET_TIMER_HANDLE(HWC_FD_ADDRESS));
	fdma_terminate_task();
}

void verif_tmi_delete_callback(uint64_t opaque1, uint16_t opaque2)
{
	struct ldpaa_fd fd __attribute__((aligned(sizeof(struct ldpaa_fd))));
	uint8_t frame_handle;
	uint8_t spid = *((uint8_t *)HWC_SPID_ADDRESS);

	fdma_create_frame(&fd,&opaque1, sizeof(opaque1), &frame_handle);
	tman_timer_completion_confirmation(
			TMAN_GET_TIMER_HANDLE(HWC_FD_ADDRESS));
	fdma_store_and_enqueue_frame_fqid(frame_handle, FDMA_EN_TC_TERM_BITS,
			(uint32_t)opaque2, spid);
}
