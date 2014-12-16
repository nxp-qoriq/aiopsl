/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
@File          aiop_verification_tman.h

@Description   This file contains the AIOP TMAN SW Verification Structures
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_TMAN_H_
#define __AIOP_VERIFICATION_TMAN_H_

#include "dplib/fsl_ldpaa.h"
#include "dplib/fsl_tman.h"


/**************************************************************************//**
 @addtogroup		AIOP_Service_Routines_Verification

 @{
*//***************************************************************************/


/**************************************************************************//**
 @Group		AIOP_TMAN_SRs_Verification

 @Description	AIOP TMAN Verification structures definitions.

 @{
*//***************************************************************************/

/*! \enum e_tman_verif_cmd_type Defines the TMAN CMDTYPE field.*/
enum e_tman_verif_cmd_type {
	TMAN_CMDTYPE_TMI_CREATE_VERIF = 0,
	TMAN_CMDTYPE_TMI_DELETE_VERIF,
	TMAN_CMDTYPE_TMI_QUERY_VERIF,
	TMAN_CMDTYPE_TIMER_CREATE_VERIF,
	TMAN_CMDTYPE_TIMER_DELETE_VERIF,
	TMAN_CMDTYPE_TIMER_INC_DURATION_VERIF,
	TMAN_CMDTYPE_TIMER_RECHARGE_VERIF,
	TMAN_CMDTYPE_TIMER_QUERY_VERIF,
	TMAN_CMDTYPE_COMPLETION_CONF_VERIF,
	TMAN_CMDTYPE_GET_TS_VERIF,
	TMAN_CMDTYPE_TMI_QUERY_SW_VERIF
};
/* TMAN Commands Structure identifiers */
#define TMAN_TMI_CREATE_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TMI_CREATE_VERIF)

#define TMAN_TMI_DELETE_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TMI_DELETE_VERIF)

#ifndef REV2
#define TMAN_TMI_QUERY_SW_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TMI_QUERY_SW_VERIF)
#endif

#define TMAN_TIMER_CREATE_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TIMER_CREATE_VERIF)

#define TMAN_TIMER_DELETE_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TIMER_DELETE_VERIF)

#ifdef REV2
#define TMAN_TMI_QUERY_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TMI_QUERY_VERIF)

#define TMAN_TIMER_INC_DURATION_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TIMER_INC_DURATION_VERIF)

#define TMAN_TIMER_RECHARGE_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TIMER_RECHARGE_VERIF)
#endif

#define TMAN_TIMER_QUERY_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_TIMER_QUERY_VERIF)

#define TMAN_TIMER_COMPLETION_CONF_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_COMPLETION_CONF_VERIF)

#define TMAN_GET_TS_CMD_STR	((TMAN_MODULE << 16) | \
		(uint32_t)TMAN_CMDTYPE_GET_TS_VERIF)

/**************************************************************************//**
@Description	TMAN TMI Create Command structure.

		Includes information needed for TMAN TMI Create Command
		verification.
*//***************************************************************************/
struct tman_tmi_create_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint64_t	tmi_mem_base_addr;
	uint32_t	max_num_of_timers;
	uint8_t		tmi_id;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	TMAN TMI delete Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_tmi_delete_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	mode_bits;
	tman_arg_8B_t	conf_opaque_data1;
#ifndef REV2
	uint32_t	max_num_of_timers;
#endif
	tman_cb_t	tman_confirm_cb;
	tman_arg_2B_t	conf_opaque_data2;
	uint8_t		tmi_id;
	uint8_t		cb_with_confirmation;
};

#ifdef REV2
/**************************************************************************//**
@Description	TMAN TMI query Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_tmi_query_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint8_t		pad[12];
	struct tman_tmi_params tmi_params;
	int32_t		status;
	uint8_t		tmi_id;
	uint8_t		pad2[3];
};
#endif

#ifndef REV2
/**************************************************************************//**
@Description	TMAN TMI query Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_tmi_query_sw_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint8_t		tmi_id;
	uint8_t		pad2[3];
};
#endif

/**************************************************************************//**
@Description	TMAN timer create Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_timer_create_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	mode_bits;
	uint64_t	opaque_data1;
	int32_t		status;
	uint32_t	timer_handle;
	tman_cb_t	tman_timer_cb;
	uint16_t	opaque_data2;
	uint16_t	duration;
	uint8_t		tmi_id;
	uint8_t		cb_with_confirmation;
	uint8_t		pad[6];
};

/**************************************************************************//**
@Description	TMAN timer delete Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_timer_delete_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	mode_bits;
	uint32_t	timer_handle;
};


#ifdef REV2
/**************************************************************************//**
@Description	TMAN timer increase duration Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_timer_increase_duration_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	timer_handle;
	uint16_t	duration;
	uint8_t		pad[2];
};

/**************************************************************************//**
@Description	TMAN timer recharge Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_timer_recharge_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	int32_t		status;
	uint32_t	timer_handle;
};
#endif

/**************************************************************************//**
@Description	TMAN timer query Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_timer_query_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	timer_handle;
	enum e_tman_query_timer state;
	uint8_t		pad[3];
};

/**************************************************************************//**
@Description	TMAN timer completion confirmation Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_timer_comp_conf_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint32_t	timer_handle;
};

/**************************************************************************//**
@Description	TMAN get TS Command structure.

		Includes information needed for TMAN Command verification.
*//***************************************************************************/
struct tman_get_ts_command {
	uint32_t	opcode;
		/**< Command structure identifier. */
	uint8_t		pad[4];
	uint64_t	timestamp;
};

uint16_t aiop_verification_tman(uint32_t asa_seg_addr);
void verif_tman_callback(uint64_t opaque1, uint16_t opaque2);
void verif_tman_callback_no_conf(uint64_t opaque1, uint16_t opaque2);

/** @} */ /* end of AIOP_TMAN_SRs_Verification */

/** @} */ /* end of AIOP_Service_Routines_Verification */



#endif /* __AIOP_VERIFICATION_TMAN_H_ */
