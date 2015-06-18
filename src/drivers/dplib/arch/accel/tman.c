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
@File		tman.c

@Description	This file contains the AIOP SW TMAN API	implementation.

*//***************************************************************************/

#include "general.h"
#include "tman.h"
#include "dplib/fsl_tman.h"
#include "dplib/fsl_fdma.h"
#include "osm_inline.h"
#include "dplib/fsl_ldpaa.h"
#include "inline_asm.h"
#include "fsl_io_ccsr.h"

#ifndef REV2
	/* The next code is due to Errata ERR008205 */
	uint32_t tman_tmi_max_num_of_timers[256];
	/* End of Errata ERR008205 related code */
#endif

int tman_create_tmi(uint64_t tmi_mem_base_addr,
			uint32_t max_num_of_timers, uint8_t *tmi_id)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("tman.c", (uint32_t)tmi_mem_base_addr, ALIGNMENT_64B);
#endif
	
	/* command parameters and results */
	uint32_t arg1, arg2, cdma_cfg;
	unsigned int res1, res2, *tmi_state_ptr;


	/*Reading the ICID and AMQ from the CDMA to avoid configuring GPP ICID
	 * when calling the create TMI from host command interface */
	/* TODO - need to replace the below code to a similar to
	 * slab_module_init function in slab.c when the verification env will
	 * contain ARENA.
	 * i.e. cdma_cfg = ioread32_ccsr(&ccsr->cdma_regs.cfg); */
	cdma_cfg = ioread32_ccsr((uint32_t *)CDMA_BASE_ADDRESS);
	/* Isolate ICID */
	arg1 = ((cdma_cfg << 16) & 0x7FFF0000) + TMAN_CMDTYPE_TMI_CREATE;
	/* Add BDI bit */
	arg1 = arg1 | ((cdma_cfg & CDMA_BDI_MASK) << 12);
	/* Add PL and VA to max_num_of_timers */
	arg2 = ((cdma_cfg & CDMA_PL_VA_MASK) << 8) | 
			(max_num_of_timers & 0x00FFFFFF);

	/* Store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__st64dw_d(tmi_mem_base_addr, HWC_ACC_IN_ADDRESS + 8);
	/* The next loop waits until the TMI Creation logic is not busy */
	do {
		/* call TMAN. */
		__e_hwacceli(TMAN_ACCEL_ID);
		/* Load command results */
		__ldw(&res1, &res2, HWC_ACC_OUT_ADDRESS, 0);
	} while (res1 == TMAN_TMI_CREATE_BUSY);
	/* Store tmi_id */
	*tmi_id = (uint8_t)res2;
	if (res1 == TMAN_TMIID_DEPLETION_ERR)
		return (int)(-ENOSPC);
	
	tmi_state_ptr = (unsigned int*)((unsigned int)TMAN_CCSR_TMSTATE_ADDRESS
			+ ((*tmi_id)<<5));

	while ((*tmi_state_ptr != TMAN_TMI_BUS_ERR) &&
			(*tmi_state_ptr != TMAN_TMI_ACTIVE))
	{
		/* YIELD. May not be optimized due to CTS behavior*/
		sys_yield();
	}
#ifndef REV2
	/* Reading TMEV register value is due to Errata ERR008234 */
	if ((*tmi_state_ptr == TMAN_TMI_BUS_ERR) ||
		(*((uint32_t *) TMAN_TMEV_ADDRESS) & TMAN_TMEV_BUS_ERR_MASK))
		tman_exception_handler(TMAN_TMI_CREATE_FUNC_ID,
				__LINE__,
				(int)(TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_BUS_ERR));
	/* The next code line is due to Errata ERR008205 */
	tman_tmi_max_num_of_timers[*tmi_id] = max_num_of_timers;
#endif
	return (int)(TMAN_TMI_CREATE_SUCCESS);
}

void tman_delete_tmi(tman_cb_t tman_confirm_cb, uint32_t flags,
			uint8_t tmi_id, tman_arg_8B_t conf_opaque_data1,
			tman_arg_2B_t conf_opaque_data2)
{
	struct tman_tmi_input_extention extention_params
		__attribute__((aligned(TMAN_EXT_PARAM_ALIGNMENT)));
	/* command parameters and results */
	uint32_t res1, epid = EPID_TIMER_EVENT_IDX;

	/* The next code is due to Errata ERR008205 */
	uint32_t i, timer_handle, *tmi_statsntc_ptr, *tmi_statsnccp_ptr;
	int status;
	
	status = tman_query_tmi_sw(tmi_id);
	if (status != 0)
	{
		if (status == -ENAVAIL) {
			tman_exception_handler(TMAN_TMI_DELETE_FUNC_ID,
				__LINE__,
				(int)TMAN_TMR_TMI_STATE_ERR+
				TMAN_TMI_NOT_ACTIVE);
		}
		else {
			if (status == -ENOMEM)
			{
				tman_exception_handler(TMAN_TMI_DELETE_FUNC_ID,
				__LINE__,
				(int)TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_PURGED);
			}
			else {
				tman_exception_handler(TMAN_TMI_DELETE_FUNC_ID,
				__LINE__,
				(int)TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_BUS_ERR);
			}
		}
	}
	for (i = 1; i <= tman_tmi_max_num_of_timers[tmi_id]; i++)
	{
		timer_handle = (i << 8) | tmi_id;
		/* As in Rev1 only force expiration is supported */
		tman_delete_timer(timer_handle,	
				TMAN_TIMER_DELETE_MODE_FORCE_EXP);
	}
	tmi_statsntc_ptr = (uint32_t*)((uint32_t)TMAN_TMSTATNAT_ADDRESS
			+ (tmi_id<<5));
	tmi_statsnccp_ptr = (uint32_t*)((uint32_t)TMAN_TMSTATNCCP_ADDRESS
			+ (tmi_id<<5));
	while ((*tmi_statsntc_ptr != 0) && (*tmi_statsnccp_ptr != 0))
	{
		/* YIELD. May not be optimized due to CTS behavior*/
		sys_yield();
	}
	/* End of Errata ERR008205 related code */	

	/* extention_params.conf_opaque_data1 = conf_opaque_data1;
	Optimization: remove 1 cycle of store word (this rely on EABI) */
	__st64dw_b(conf_opaque_data1,
		  (uint32_t)(&extention_params.opaque_data1));
	/* The function callback */
	extention_params.hash = (uint32_t)tman_confirm_cb;
	/* extention_params.opaque_data2_epid =
		(uint32_t)(conf_opaque_data2 << 16) | confirmation_epid;
	Optimization: remove 2 cycles clear and shift */
	epid = __e_rlwimi(epid, conf_opaque_data2, 16, 0, 15);
	__stw(epid, 0, &(extention_params.opaque_data2_epid));
	/* Store first two command parameters */
	__stdw(flags, tmi_id, HWC_ACC_IN_ADDRESS, 0);

	/* arg1 = (uint32_t *)(HWC_ACC_OUT_ADDRESS + 8);
	*arg1 = (uint32_t)(&extention_params);
	Optimization: remove 1 cycle of address calc */
	__sthw_d(&extention_params, HWC_ACC_IN_ADDRESS + 0xA);
	/* The next loop waits until the TMI deletion logic is not busy */
	do {
		/* call TMAN. */
		__e_hwacceli(TMAN_ACCEL_ID);
		/* Load command results */
		res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
		/* check each loop instance to verify if in the time of the
		 * command another task did'nt deleted the same TMI */
		if (((res1 & TMAN_FAIL_BIT_MASK) != 0) && 
				((res1 & TMAN_TMI_DEL_TMP_ERR_MASK) == 0))
			tman_exception_handler(TMAN_TMI_DELETE_FUNC_ID,
					__LINE__, (int)res1);
	} while (res1 & TMAN_TMI_DEL_TMP_ERR_MASK);
	
}

#ifndef REV2
	/* The next code is due to Errata ERR008205 */
int tman_query_tmi_sw(uint8_t tmi_id){
	unsigned int *tmi_state_ptr;
	tmi_state_ptr = (unsigned int*)((unsigned int)TMAN_CCSR_TMSTATE_ADDRESS
			+ (tmi_id<<5));
	if((*tmi_state_ptr & TMAN_TMI_STATE_MASK) == TMAN_TMI_ACTIVE)
		return (int)(SUCCESS);
	if((*tmi_state_ptr & TMAN_TMI_STATE_MASK) == TMAN_TMI_NOT_ACTIVE)
		return (int)(-ENAVAIL);
	if((*tmi_state_ptr & TMAN_TMI_STATE_MASK) == TMAN_TMI_BUS_ERR)
		return (int)(-ENOMEM);
	return (int)(-EACCES);
}
/* End of Errata ERR008205 related code */
#endif

#ifdef REV2
int tman_query_tmi(uint8_t tmi_id,
			struct tman_tmi_params *output_ptr)
{
	/* command parameters and results */
	uint32_t cmd_type = TMAN_CMDTYPE_TMI_QUERY, res1;
	/* Optimization: stdw is used to give the compiler chance to optimize
	   when using an inline function */
	__stdw(cmd_type, tmi_id, HWC_ACC_IN_ADDRESS, 0);
	/* Fill command parameters */
	__sthw_d(output_ptr, HWC_ACC_IN_ADDRESS + 0x8);
	/* call TMAN. */
	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	/* Optimization: remove one cycle compared to and statement */
	/* Erase PL and BDI from output_ptr */
	*(uint8_t *)(&output_ptr->max_num_of_timers) = 0;
	if (!((res1) & TMAN_FAIL_BIT_MASK))
			return (int)(TMAN_TMI_QUERY_SUCCESS);
	if ((res1 & TMAN_TMI_STATE_MASK) == TMAN_TMI_NOT_ACTIVE)
		return (int)(-ENAVAIL);
	if ((res1 & TMAN_TMI_STATE_MASK) == TMAN_TMI_BUS_ERR)
		tman_exception_handler(TMAN_TMI_TMI_QUERY_FUNC_ID,
			__LINE__, 
			(int)(TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_BUS_ERR));
	/* In case TMI is being deleted or being created */
	return (int)(-EACCES);
}
#endif


#ifdef REV2
int tman_increase_timer_duration(uint32_t timer_handle, uint16_t duration)
{
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_MODIFY, res1;
#ifdef SL_DEBUG
	uint32_t cnt = 0;
#endif

	/* Store first two command parameters */
	/* Optimization: remove 1 cycle using EABI */
	__stdw(cmd_type, timer_handle, HWC_ACC_IN_ADDRESS, 0);
	/* Store third command parameters */
	/* Optimization: remove 1 cycle of clearing duration upper bits */
	*(uint16_t *)(HWC_ACC_IN_ADDRESS + 0x8) = duration;

	do {
		/* call TMAN. */
		__e_hwacceli(TMAN_ACCEL_ID);
		/* Load command results */
		res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
#ifdef SL_DEBUG
		cnt++;
		ASSERT_COND(cnt >= TMAN_MAX_RETRIES);
#endif
	} while ((res1 == TMAN_TMR_TMP_ERR1) || (res1 == TMAN_TMR_TMP_ERR2));
	return (int)(res1);
}
#endif

#ifdef REV2
int tman_recharge_timer(uint32_t timer_handle)
#else
void tman_recharge_timer(uint32_t timer_handle)
#endif
{
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_RECHARGE;
#ifdef REV2
	uint32_t res1;
#endif
	
	/* Store first two command parameters */
	__stdw(cmd_type, timer_handle, HWC_ACC_IN_ADDRESS, 0);

	/* call TMAN. and check if passed. 
	 * Optimization using compiler pattern*/
#ifndef REV2
	__e_hwacceli_(TMAN_ACCEL_ID);
#else
	if(__e_hwacceli_(TMAN_ACCEL_ID) == 0)
		return (int)(TMAN_REC_TMR_SUCCESS);

	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);

	/* optimization: all the errors except TMI state errors starts with
	 *  0x08_00XX */
	if ((res1 & TMAN_TMR_REC_STATE_MASK) ||
			(res1 == TMAN_REC_TMR_NOT_ACTIVE_ERR))
		tman_exception_handler(TMAN_TMI_TIMER_RECHARGE_FUNC_ID,
			__LINE__, (int)res1);
	return (int)(-ETIMEDOUT);
#endif
}

void tman_query_timer(uint32_t timer_handle,
			enum e_tman_query_timer *state)
{
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_QUERY, res1;

	/* Store first two command parameters */
	__stdw(cmd_type, timer_handle, HWC_ACC_IN_ADDRESS, 0);

	/* call TMAN. */
	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);

	if ((res1 & 0x4) == 0)
		res1 &= 0x1;
	else
		res1 &= TMAN_TMR_QUERY_STATE_MASK;
	
	*state = (enum e_tman_query_timer)res1;
}

#pragma push
#pragma force_active on
void tman_timer_callback(void)
{
	tman_cb_t tman_cb;
	tman_arg_8B_t tman_cb_arg1;
	tman_arg_2B_t tman_cb_arg2;

	tman_cb = (tman_cb_t)__lwbr(HWC_FD_ADDRESS+FD_HASH_OFFSET, 0);
	tman_cb_arg1 = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
	tman_cb_arg2 =
		(tman_arg_2B_t)__lhbr(HWC_FD_ADDRESS+FD_OPAQUE1_OFFSET, 0);
	osm_task_init();
	(*(tman_cb))(tman_cb_arg1, tman_cb_arg2);
	fdma_terminate_task();
}

int tman_create_timer_wrp(uint8_t tmi_id, uint32_t flags,
			uint16_t duration, tman_arg_8B_t opaque_data1,
			tman_arg_2B_t opaque_data2, tman_cb_t tman_timer_cb,
			uint32_t *timer_handle)
{
	return tman_create_timer(tmi_id, flags, duration, opaque_data1,
			 	 opaque_data2, tman_timer_cb, timer_handle);
}

int tman_delete_timer_wrp(uint32_t timer_handle, uint32_t flags)
{
	return tman_delete_timer(timer_handle, flags);
}

#pragma pop

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"

#pragma stackinfo_ignore on

void tman_exception_handler(enum tman_function_identifier func_id,
		uint32_t line,
		int32_t status)
{

	char *func_name;

	/* Translate function ID to function name string */
	switch(func_id) {
	case TMAN_TMI_CREATE_FUNC_ID:
		func_name = "tman_create_tmi";
		break;
	case TMAN_TMI_DELETE_FUNC_ID:
		func_name = "tman_delete_tmi";
		break;
	case TMAN_TMI_TMI_QUERY_FUNC_ID:
		func_name = "tman_query_tmi";
		break;
	case TMAN_TMI_TIMER_CREATE_FUNC_ID:
		func_name = "tman_create_timer";
		break;
	case TMAN_TMI_TIMER_DELETE_FUNC_ID:
		func_name = "tman_delete_timer";
		break;
	case TMAN_TMI_TIMER_MODIFY_FUNC_ID:
		func_name = "tman_increase_timer_duration";
		break;
	case TMAN_TMI_TIMER_RECHARGE_FUNC_ID:
		func_name = "tman_recharge_timer";
		break;
	case TMAN_TIMER_QUERY_FUNC_ID:
		func_name = "tman_query_timer";
		break;
	default:
		/* create own exception */
		exception_handler(__FILE__,
				  "tman_exception_handler",
				  __LINE__,
				  "tman_exception_handler got unknown"
				  "function identifier.\n");
	}
	
	switch(status) {
	case TMAN_TMIID_DEPLETION_ERR:
		exception_handler(__FILE__, func_name, line,
				"All TMIs are used. A TMI must be deleted "
				"before a new one can be created. \n");
		break;
	case TMAN_ILLEGAL_DURATION_VAL_ERR:
		exception_handler(__FILE__, func_name, line,
				"Illegal Timer duration. "
				"The duration must have a value larger than 10 "
				"ticks and smaller than 2^16-10 ticks.\n");
		break;
	case TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_NOT_ACTIVE:
		exception_handler(__FILE__, func_name, line,
				"A non active TMI was provided as an input.\n");
		break;
	case TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_PURGED:
		exception_handler(__FILE__, func_name, line,
				"A TMI that is being deleted was provided.\n");
		break;
	case TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_BUSY1:
		exception_handler(__FILE__, func_name, line,
				"The TMI that was provided is currently being "
				"initialized.\n");
		break;
	case TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_BUSY2:
		exception_handler(__FILE__, func_name, line,
				"The TMI that was provided is currently being "
				"initialized.\n");
		break;
	case TMAN_TMR_TMI_STATE_ERR+TMAN_TMI_BUS_ERR:
		exception_handler(__FILE__, func_name, line,
				"Failed to initialize due to system bus "
				"error.\n");
		break;

	case TMAN_TMR_DEPLETION_ERR:
		exception_handler(__FILE__, func_name, line,
				"No more available timers in the TMI.\n");
		break;

	case TMAN_DEL_TMR_NOT_ACTIVE_ERR:
	case TMAN_REC_TMR_NOT_ACTIVE_ERR:
		exception_handler(__FILE__, func_name, line,
				"A non active timer was provided as an"
				" input.\n");
		break;
	case TMAN_DEL_CCP_WAIT_ERR:
		exception_handler(__FILE__, func_name, line,
				"The one shot timer has expired but it is "
				"pending a completion confirmation (done by "
				"calling the tman_timer_completion_confirmation"
				" function).\n");
		break;
	case TMAN_DEL_PERIODIC_CCP_WAIT_ERR:
		exception_handler(__FILE__, func_name, line,
				"The timer has expired but it is "
				"pending a completion confirmation (done by "
				"calling the tman_timer_completion_confirmation"
				" function).\n");
		break;
	case TMAN_DEL_TMR_DEL_ISSUED_ERR:
		exception_handler(__FILE__, func_name, line,
				"A delete command was already issued for this "
				"timer and the TMAN is in the process of "
				"deleting the timer. The timer will elapse in "
				"the future.\n");
		break;
	case TMAN_DEL_TMR_DEL_ISSUED_CONF_ERR:
		exception_handler(__FILE__, func_name, line,
				"A delete command was already issued. The "
				"timer has already elapsed and it is pending"
				" a completion confirmation "
				"(done by calling the "
				"tman_timer_completion_confirmation function)"
				" This timer will elapse one more time "
				" before being deleted\n");
		break;
#ifdef REV2
	case TMAN_MOD_TMR_NOT_ACTIVE_ERR:
		exception_handler(__FILE__, func_name, line,
				"A non active timer was provided as an"
				" input.\n");
		break;
	case TMAN_MOD_CCP_WAIT_ERR:
		exception_handler(__FILE__, func_name, line,
				"The one shot timer has expired but it is "
				"pending a completion confirmation (done by "
				"calling the tman_timer_completion_confirmation"
				" function).\n");
		break;
	case TMAN_MOD_PERIODIC_CCP_WAIT_ERR:
		exception_handler(__FILE__, func_name, line,
				"The periodic timer has expired but it is "
				"pending a completion confirmation (done by "
				"calling the tman_timer_completion_confirmation"
				" function).\n");
		break;
	case TMAN_MOD_TMR_DEL_ISSUED_ERR:
		exception_handler(__FILE__, func_name, line,
				"A delete command was already issued for this "
				"timer and the TMAN is in the process of "
				"deleting the timer. The timer will elapse in "
				"the future.\n");
		break;
	case TMAN_MOD_TMR_DEL_ISSUED_CONF_ERR:
		exception_handler(__FILE__, func_name, line,
				"A delete command was already issued. The "
				"timer has already elapsed for the last time "
				"and it is pending a completion confirmation "
				"(done by calling the "
				"tman_timer_completion_confirmation"
				" function)\n");
		break;
#endif

	default:
		exception_handler(__FILE__, func_name, line,
				"Unknown or Invalid status.\n");
       }
}
#pragma pop

