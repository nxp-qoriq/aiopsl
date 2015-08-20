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
@File          tman.h

@Description   This file contains the AIOP SW TMAN definitions (02_01)
*//***************************************************************************/

#ifndef __AIOP_TMAN_H
#define __AIOP_TMAN_H

#include "general.h"
#include "fsl_errors.h"


typedef void  /*__noreturn*/ (*tman_wrp_cb_t) (uint64_t arg1, uint16_t arg2);


/**************************************************************************//**
@Group		TMANReturnStatus TMAN functions return status

@Description	AIOP TMAN functions return status
@{
*//***************************************************************************/

/**************************************************************************//**
 @enum tman_tmi_create_status

 @Description	AIOP TMAN create TMI command status codes.

 @{
*//***************************************************************************/
enum tman_tmi_create_status {
	/** Success. */
	TMAN_TMI_CREATE_SUCCESS = 0,
	/** TMI Creation logic is currently busy with the previous TMI creation
	 * command. */
	TMAN_TMI_CREATE_BUSY = 0x81400040,
	/** All TMIs are used. A TMI must be deleted before a new one can
		be created. */
	TMAN_TMIID_DEPLETION_ERR = 0x814000C0,

	/** Bus error occurred when initializing the TMI timers memory*/
	TMAN_TMI_CREATE_BUS_ERR = 0x81400FFF
};

/** @} end of enum tman_tmi_create_status */

/**************************************************************************//**
 @enum tman_timer_create_status

 @Description	AIOP TMAN create timer command status codes.

 @{
*//***************************************************************************/
enum tman_timer_create_status {
	/** Success. */
	TMAN_TMR_CREATE_SUCCESS = 0,
	/** illegal Timer Create Fields Description[DURATION] */
	TMAN_ILLEGAL_DURATION_VAL_ERR = 0x81800010,
	/** The TMI timer list head is waiting for confirmation. In this case
	 * the timer handle will contain the list head. */
	TMAN_TMR_CONF_WAIT_ERR = 0x81800040,
	/** A non active TMI was provided as an input */
	TMAN_TMR_TMI_STATE_ERR = 0x81C00010,
	/** No more available timers in the TMI */
	TMAN_TMR_DEPLETION_ERR = 0x81C00020
};

/** @} end of enum tman_timer_create_status */

/**************************************************************************//**
 @enum tman_tmi_state_error

 @Description	AIOP TMAN TMI Engine state status codes.

 @{
*//***************************************************************************/
enum tman_tmi_state_ststus {
	/** A non active TMI was provided as an input */
	TMAN_TMI_NOT_ACTIVE = 0,
	/** TMI is active */
	TMAN_TMI_ACTIVE,
	/** TMI being purged */
	TMAN_TMI_PURGED,
	/** TMI Busy */
	TMAN_TMI_BUSY1 = 4,
	/** TMI Busy currently TMAN is busy creating the TMI */
	TMAN_TMI_BUSY2,
	/** TMI Failed to init due to Bus error */
	TMAN_TMI_BUS_ERR
};

/** @} end of enum tman_tmi_state_error */


/**************************************************************************//**
 @enum tman_timer_delete_status

 @Description	AIOP TMAN delete timer command status codes.

 @{
*//***************************************************************************/
enum tman_timer_delete_status {
	/** Success. */
	TMAN_DEL_TMR_DELETE_SUCCESS = 0,
	/** The timer aimed to be deleted expiration date is currently being
	 *  processed */
	TMAN_DEL_TMR_TMP_ERR = 0x81800030,
	/** A non active timer was provided as an input */
	TMAN_DEL_TMR_NOT_ACTIVE_ERR = 0x81800050,
	/** The timer has expired but it is pending a completion confirmation
	 * (done by calling the tman_timer_completion_confirmation function) */
	TMAN_DEL_CCP_WAIT_ERR = 0x81800051,
	/** The periodic timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function). Deleting with force expiration is forbidden */
	TMAN_DEL_PERIODIC_CCP_WAIT_ERR = 0x81800055,
	/** A delete command was already issued for this timer and the TMAN is
	 * in the process of deleting the timer. The timer will elapse in the
	 * future. */
	TMAN_DEL_TMR_DEL_ISSUED_ERR = 0x81800056,
	/** A delete command for this periodic timer was already issued.
	 * The timer has already elapsed and it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function). This timer will elapse one more time before being
	 * deleted */
	TMAN_DEL_TMR_DEL_ISSUED_CONF_ERR = 0x81800057,
};

/** @} end of enum tman_timer_delete_status */

/**************************************************************************//**
 @enum tman_timer_mod_status

 @Description	AIOP TMAN increase timer duration command status codes.

 @{
*//***************************************************************************/
enum tman_timer_mod_status {
	/** Success. */
	TMAN_MOD_TMR_SUCCESS = 0,
	/** illegal Timer Create Fields Description[DURATION] */
	TMAN_MOD_ILLEGAL_DURATION_VAL_ERR = 0x81800010,
	/** A non active timer was provided as an input */
	TMAN_MOD_TMR_NOT_ACTIVE_ERR = 0x81800060,
	/** The one shot timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_MOD_CCP_WAIT_ERR = 0x81800061,
	/** The periodic timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_MOD_PERIODIC_CCP_WAIT_ERR = 0x81800065,
	/** A delete command was already issued for this timer and the TMAN is
	 * in the process of deleting the timer. The timer will elapse in the
	 * future. */
	TMAN_MOD_TMR_DEL_ISSUED_ERR = 0x81800066,
	/** A delete command was already issued. The timer has already elapsed
	 * for the last time and it is pending a completion confirmation
	 * (done by calling the tman_timer_completion_confirmation function) */
	TMAN_MOD_TMR_DEL_ISSUED_CONF_ERR = 0x81800067,
};
/** @} end of enum tman_timer_mod_status */

/**************************************************************************//**
 @enum tman_timer_recharge_status

 @Description	AIOP TMAN timer recharge command status codes.

 @{
*//***************************************************************************/
enum tman_timer_recharge_status {
	/** Success. */
	TMAN_REC_TMR_SUCCESS = 0,
	/** A non active timer was provided as an input */
	TMAN_REC_TMR_NOT_ACTIVE_ERR = 0x81800070,
	/** The one shot timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_REC_CCP_WAIT_ERR = 0x81800071,
	/** The periodic timer has expired but it is pending a completion
	 * confirmation (done by calling the tman_timer_completion_confirmation
	 * function) */
	TMAN_REC_PERIODIC_CCP_WAIT_ERR = 0x81800075,
	/** A delete command was already issued for this timer and the TMAN is
	 * in the process of deleting the timer. The timer will elapse in the
	 * future. */
	TMAN_REC_TMR_DEL_ISSUED_ERR = 0x81800076,
	/** A delete command was already issued. The timer has already elapsed
	 * for the last time and it is pending a completion confirmation
	 * (done by calling the tman_timer_completion_confirmation function) */
	TMAN_REC_TMR_DEL_ISSUED_CONF_ERR = 0x81800077,
	/** The timer is elapsing in this timer tick */
	TMAN_REC_TMR_CURR_ELAPSE = 0x81800020,
	/** Because of processing load the TMAN is lagging behind the wall
	 * clock. This causes the timer aimed to be recharged expiration date
	 * to currently being processed. */
	TMAN_REC_TMR_BUSY = 0x81800030,
	/** The TMI state error */
	TMAN_REC_TMI_STATE_ERR = 0x81C00010
};

/** @} end of enum tman_timer_recharge_status */

/** @} end of group TMANReturnStatus */


/*! \enum e_tman_cmd_type Defines the TMAN CMDTYPE field.*/
enum e_tman_cmd_type {
	TMAN_CMDTYPE_TMI_CREATE = 0x1000,
	TMAN_CMDTYPE_TMI_DELETE = 0x1011,
	TMAN_CMDTYPE_TMI_DELETE_FORCE = 0x1012,
	TMAN_CMDTYPE_TMI_QUERY = 0x1023,
	TMAN_CMDTYPE_TIMER_CREATE = 0x2010,
	TMAN_CMDTYPE_TIMER_DELETE = 0x2001,
	TMAN_CMDTYPE_TIMER_DELETE_FORCE,
	TMAN_CMDTYPE_TIMER_DELETE_FORCE_WAIT,
	TMAN_CMDTYPE_TIMER_MODIFY,
	TMAN_CMDTYPE_TIMER_RECHARGE,
	TMAN_CMDTYPE_TIMER_QUERY
};

/*! \enum tman_function_identifier Defines the provided TMAN functions.*/
enum tman_function_identifier {
	TMAN_TMI_CREATE_FUNC_ID,
	TMAN_TMI_DELETE_FUNC_ID,
	TMAN_TMI_TMI_QUERY_FUNC_ID,
	TMAN_TMI_TIMER_CREATE_FUNC_ID,
	TMAN_TMI_TIMER_DELETE_FUNC_ID,
	TMAN_TMI_TIMER_MODIFY_FUNC_ID,
	TMAN_TMI_TIMER_RECHARGE_FUNC_ID,
	TMAN_TIMER_QUERY_FUNC_ID
};


#define TMAN_QUERY_MAX_NT_MASK	0x00FFFFFF
#define TMAN_STATUS_MASK	0xF8000000
/** CDMA Peripheral base address */
#define CDMA_BASE_ADDRESS	0x0208d000
/** CDMA BDI Bit Mask */
#define CDMA_BDI_MASK	0x00080000
/** CDMA PL and VA Bits Mask */
#define CDMA_PL_VA_MASK	0x00050000
/** TMAN Peripheral base address */
#define TMAN_BASE_ADDRESS	0x02020000
/** TMCBCC- TMan Callback Completion Confirmation */
#define TMAN_TMCBCC_ADDRESS	(TMAN_BASE_ADDRESS+0x014)
/** TMTSTMP- TMan TMAN Timestamp register address */
#define TMAN_TMTSTMP_ADDRESS	(TMAN_BASE_ADDRESS+0x020)
/** TMEV- TMan Error Event register base address */
#define TMAN_TMEV_ADDRESS	(TMAN_BASE_ADDRESS+0x38)
/** TMSTATNAT- TMan TMAN Stats Num of Active Timers register base address */
#define TMAN_TMSTATNAT_ADDRESS	(TMAN_BASE_ADDRESS+0x2008)
/** TMSTATNCCP- TMan TMAN Stats Number of Callback Confirmation Pending
 * register base address */
#define TMAN_TMSTATNCCP_ADDRESS	(TMAN_BASE_ADDRESS+0x200C)
/** TMSTATE- TMan TMAN State register base address */
#define TMAN_TMSTATE_ADDRESS	(TMAN_BASE_ADDRESS+0x2018)
/** TMSTATE- TMan TMAN State register address Workaround for TKT254640 */
#define TMAN_CCSR_TMSTATE_ADDRESS	0x020a2018
/** TMan Dedicated EPID */
#define EPID_TIMER_EVENT_IDX	1
/** Offset to USER_OPAQUE1 in FD */
#define FD_OPAQUE1_OFFSET	0x1A
/** Offset to HASH in FD */
#define FD_HASH_OFFSET		0x1C
/** Number of command retries - for debug purposes */
#define TMAN_MAX_RETRIES	100000
/** The TMI deletion logic is currently busy with another TMI delete */
#define TMAN_TMI_DEL_TMP_ERR_MASK	0x00000020
/** The TMI query command error */
#define TMAN_TMI_CMD_ERR	0x81400000
/** A mask that defines the query TMI command temporary error type */
#define TMAN_TMI_QUERY_TMP_ERR_MASK	0x00000020
/** If the TMI query was successful */
#define TMAN_TMI_QUERY_SUCCESS		0x00000000
/** The TMI state bits mask */
#define TMAN_TMI_STATE_MASK		0x0000000F
/** Fail status bit mask */
#define TMAN_FAIL_BIT_MASK		0x80000000
/** Timer commands temporary error 1 */
#define TMAN_TMR_TMP_ERR1	0x81800020
/** Timer commands temporary error 2 */
#define TMAN_TMR_TMP_ERR2	0x81800030
/** Timer commands temporary error 3 */
#define TMAN_TMR_TMP_ERR3	0x81800040
/** Timer delete command state error type mask */
#define TMAN_TMR_DEL_STATE_TYPE_MASK	0x00000040
/** Timer delete command temporary error type mask */
#define TMAN_TMR_DEL_TMP_TYPE_MASK	0x00000020
/** Timer delete command state errors bit mask */
#define TMAN_TMR_DEL_STATE_MASK		0x00000007
/** Timer delete command state CCP bit mask */
#define TMAN_TMR_DEL_STATE_CCP_MASK	0x00000001
/** Timer delete command state Deleted bit mask */
#define TMAN_TMR_DEL_STATE_D_MASK	0xFFFFFFFD
/** Timer recharge command TMI state errors bit mask */
#define TMAN_TMR_REC_STATE_MASK		0x00800000
/** Timer query command state bits mask */
#define TMAN_TMR_QUERY_TMI_STATE_MASK	0x80C00100
/** Timer query command state bits mask */
#define TMAN_TMR_QUERY_STATE_MASK	0x7
/** Alignment that the TMAN requires for the input/output extension params */
#define TMAN_EXT_PARAM_ALIGNMENT	16
/** Mask to filter the System Bus error event in the TMEV register */
#define TMAN_TMEV_BUS_ERR_MASK	0x80000000

/**************************************************************************//**
@Description	TMI input extension params
*//***************************************************************************/
struct tman_tmi_input_extention {
		/** data to be associated with the confirmation task. */
	uint64_t	opaque_data1;
		/** Hash value for the confirmation task ScopeID */
	uint32_t	hash;
		/** data to be associated with the confirmation task.
		and EPID */
	uint32_t	opaque_data2_epid;
};


/**************************************************************************//**
@Function	tman_timer_callback

@Description	Callback function, called for every timer expiration and for
		TMI delete completion confirmation.
		This function will call the user function the argument
		specified in the tman_delete_tmi and tman_create_timer
		functions.

@Param		None.

@Return		None.
@Cautions	This is a none return function.

*//***************************************************************************/
void tman_timer_callback(void);

/**************************************************************************//**
@Function	tman_create_timer_wrp

@Description	Wrapper to the function tman_create_timer.
		See description of the function tman_create_timer.

@Param[in]	tmi_id  - TMAN Instance ID. (TMI ID)
@Param[in]	flags - \link TMANTimerCreateModeBits TMAN timer create
		flags \endlink
@Param[in]	duration - Timer duration time (the number of timer ticks).
		The duration must have a value larger than 10 ticks and smaller
		than 2^16-10 ticks.
@Param[in]	opaque_data1 - Data to be associated with to the created task.
@Param[in]	opaque_data2 - Data to be associated with to the created task.
@Param[in]	tman_timer_cb - A callback function used for the task created
		upon timer expiration.
@Param[out]	timer_handle - the handle of the timer for future reference.
		The handle includes the tmi ID and timer ID values.

@Return		0 on success, or negative value on error.
@Retval		ENOSPC - All timers are used. A timer must be deleted or elapse
		and confirmed before a new one can be created. This error can
		occur when the SW has missed a confirmation for a timer.
@Retval		EBUSY - The timer was not created due to high TMAN and AIOP
		load.

@Cautions	This function performs a task switch.

*//***************************************************************************/
int tman_create_timer_wrp(uint8_t tmi_id, uint32_t flags,
			uint16_t duration, uint64_t opaque_data1,
			uint16_t opaque_data2, tman_wrp_cb_t tman_timer_cb,
			uint32_t *timer_handle);

/**************************************************************************//**
@Function	tman_delete_timer_wrp

@Description	Wrapper of the function tman_delete_timer.
		See description of the function tman_delete_timer.

@Param[in]	timer_handle - The handle of the timer to be deleted.
@Param[in]	flags - \link TMANTimerDeleteModeBits TMAN timer
		delete flags \endlink.

@Return		0 on success, or negative value on error.
@Retval		ETIMEDOUT - The timer cannot be deleted. The timer aimed to be
		deleted expiration date is currently being processed by the
		TMAN. The timer will elapse shortly.
		In case of periodic timer the tman_delete_timer should be
		called at the expiration routine to avoid this error. If this
		error do happen for a periodic timer than it is consider as
		a fatal error (the timer period is too short to handle the
		timer expiration callback function).
@Retval		EACCES - The timer cannot be deleted.
		For one shot timer this error should be treated as an ETIMEDOUT
		error.
		For a periodic timer this error should be treated as a fatal
		error (a delete command was already issued for this periodic
		timer).
@Retval		ENAVAIL - The timer cannot be deleted. The timer is not an
		active one. This should be treated as a fatal error.
		When Errata ERR008205 will be fixed this error will
		automatically generate a fatal error.

@Cautions	This function performs a task switch.

*//***************************************************************************/
int tman_delete_timer_wrp(uint32_t timer_handle, uint32_t flags);


/**************************************************************************//**
@Function	tman_exception_handler

@Description	Function that distribute the TMAN errors

@Param[in]	filename - The file in which the error originated.
@Param[in]	line - The line in the file in which the error originated.
@Param[in]	status - The error status returned by the TMAN.

@Return		None.
@Cautions	This is a none return function.

*//***************************************************************************/
void tman_exception_handler(enum tman_function_identifier func_id,
		uint32_t line,
		int32_t status);

#endif /* __AIOP_TMAN_H */
