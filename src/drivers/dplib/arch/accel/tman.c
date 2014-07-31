/**************************************************************************//**
@File		tman.c

@Description	This file contains the AIOP SW TMAN API	implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "general.h"
#include "tman.h"
#include "dplib/fsl_tman.h"
#include "dplib/fsl_fdma.h"
#include "osm.h"
#include "dplib/fsl_ldpaa.h"
#include "inline_asm.h"
#ifdef SL_DEBUG
	#include "common/errors.h"
#endif

int tman_create_tmi(uint64_t tmi_mem_base_addr,
			uint32_t max_num_of_timers, uint8_t *tmi_id)
{
	/* command parameters and results */
	uint32_t arg1, arg2, icid_pl, va_bdi;
	unsigned int res1, res2, *tmi_state_ptr;
#ifdef SL_DEBUG
	uint32_t cnt = 0;
#endif

	/* Load ICID and PL */
	icid_pl = (uint32_t)
			__lhbr(HWC_ADC_ADDRESS + ADC_PL_ICID_OFFSET, (void *)0);
	/* Load VA and BDI */
	__lbz_d(va_bdi, HWC_ADC_ADDRESS + ADC_FDSRC_VA_FCA_BDI_OFFSET);

	/* Isolate ICID */
	arg1 = ((icid_pl << 16) & 0x7FFF0000) + TMAN_CMDTYPE_TMI_CREATE;
	/* Add BDI bit */
	/* Optimization: remove 1 cycle of or using rlwimi */
	/* equal to arg1 = (va_bdi << 31) | arg1; */
	__e_rlwimi(arg1, va_bdi, 31, 0, 0);
	/* Move PL bit to the right offset */
	icid_pl = (icid_pl << 11) & 0x04000000;
	/* Add PL and VA to max_num_of_timers */
	arg2 = icid_pl | ((va_bdi << 22) & 0x01000000) | max_num_of_timers;
	/* Store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__st64dw_d(tmi_mem_base_addr, HWC_ACC_IN_ADDRESS + 8);
	/* The next loop waits until the TMI Creation logic is not busy */
	do {
		/* call TMAN. */
		__e_hwacceli(TMAN_ACCEL_ID);
		/* Load command results */
		__ldw(&res1, &res2, HWC_ACC_OUT_ADDRESS, 0);
#ifdef SL_DEBUG
		cnt++;
		ASSERT_COND(cnt >= TMAN_MAX_RETRIES);
#endif
	} while (res1 == TMAN_TMI_CREATE_BUSY);
	/* Store tmi_id */
	*tmi_id = (uint8_t)res2;
	if (res1 == TMAN_TMR_DEPLETION_ERR)
		return (int)(-ENOSPC);
	tmi_state_ptr = (unsigned int*)((unsigned int)TMAN_TMSTATE_ADDRESS
			+ ((*tmi_id)<<5));
	while ((*tmi_state_ptr != TMAN_TMI_BUS_ERR) &&
			(*tmi_state_ptr != TMAN_TMI_ACTIVE))
	{
		/* YIELD. May not be optimized due to CTS behavior*/
		__e_hwacceli(YIELD_ACCEL_ID);
	}
	if (*tmi_state_ptr == TMAN_TMI_BUS_ERR)
		tman_exception_handler(__FILE__, __LINE__,
				(int)TMAN_TMI_CREATE_BUS_ERR);
	return (int)(TMAN_TMI_CREATE_SUCCESS);
}

int tman_delete_tmi(tman_cb_t tman_confirm_cb, uint32_t flags,
			uint8_t tmi_id, tman_arg_8B_t conf_opaque_data1,
			tman_arg_2B_t conf_opaque_data2)
{
	struct tman_tmi_input_extention extention_params
		__attribute__((aligned(TMAN_EXT_PARAM_ALIGNMENT)));
	/* command parameters and results */
	uint32_t res1, epid = EPID_TIMER_EVENT_IDX;
#ifdef SL_DEBUG
	uint32_t cnt = 0;
#endif

	/* extention_params.conf_opaque_data1 = conf_opaque_data1;
	Optimization: remove 1 cycle of store word (this rely on EABI) */
	__st64dw_b(conf_opaque_data1,
		  (uint32_t)(&extention_params.opaque_data1));
	/* The function callback */
	extention_params.hash = (uint32_t)tman_confirm_cb;
	/* extention_params.opaque_data2_epid =
		(uint32_t)(conf_opaque_data2 << 16) | confirmation_epid;
	Optimization: remove 2 cycles clear and shift */
	__e_rlwimi(epid, conf_opaque_data2, 16, 0, 15);
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
#ifdef SL_DEBUG
		cnt++;
		ASSERT_COND(cnt >= TMAN_MAX_RETRIES);
#endif
		/* check each loop instance to verify if in the time of the
		 * command another task did'nt deleted the same TMI */
		if ((res1 & TMAN_TMI_STATE_MASK) != TMAN_TMI_ACTIVE)
			tman_exception_handler(__FILE__, __LINE__, (int)res1);
	} while (res1 & TMAN_TMI_DEL_TMP_ERR_MASK);
	
	return (int)(TMAN_TMI_DEL_SUCCESS);
}

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
		return (int)(ENAVAIL);
	if ((res1 & TMAN_TMI_STATE_MASK) == TMAN_TMI_BUS_ERR)
		tman_exception_handler(__FILE__, __LINE__, 
				(int)TMAN_TMI_CREATE_BUS_ERR);
	/* In case TMI is being deleted or being created */
	return (int)(EACCES);
}

/* The below pragma is to remove compilation warning:"return value expected" */
#pragma warn_missingreturn off
int tman_create_timer(uint8_t tmi_id, uint32_t flags,
			uint16_t duration, tman_arg_8B_t opaque_data1,
			tman_arg_2B_t opaque_data2, tman_cb_t tman_timer_cb,
			uint32_t *timer_handle)
{
	struct tman_tmi_input_extention extention_params
		__attribute__((aligned(TMAN_EXT_PARAM_ALIGNMENT)));
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_CREATE;
	unsigned int res1, res2;
	uint32_t epid = EPID_TIMER_EVENT_IDX;
#ifdef SL_DEBUG
	uint32_t cnt = 0;
#endif

	/* Fill command parameters */
	__stdw(cmd_type, (uint32_t)tmi_id, HWC_ACC_IN_ADDRESS, 0);

	/* extention_params.opaque_data1 = conf_opaque_data1; */
	__llstdw(opaque_data1, 0, (void *)(&extention_params.opaque_data1));

	extention_params.hash = (uint32_t)tman_timer_cb;
	/* extention_params.opaque_data2_epid =
			(uint32_t)(opaque_data2 << 16) | epid;
	Optimization: remove 2 cycles clear and shift */
	__e_rlwimi(epid, opaque_data2, 16, 0, 15);
	__stw(epid, 0, &(extention_params.opaque_data2_epid));

	/* arg1 = (uint32_t *)(HWC_ACC_OUT_ADDRESS + 8);
	*arg1 = (uint32_t)(&extention_params);
	Optimization: remove 1 cycle of address calc */
	__sthw_d(&extention_params, HWC_ACC_IN_ADDRESS + 0xA);

	/* Fill command parameters */
	/* Optimization: remove 1 cycle of clearing duration upper bits
	   (no casting is allowed) */
	__or(flags, flags, duration);
	*(uint32_t *)(HWC_ACC_IN_ADDRESS + 0xc) = flags;
	do {
		/* call TMAN. */
		__e_hwacceli(TMAN_ACCEL_ID);
		/* Load command results */
		__ldw(&res1, &res2, HWC_ACC_OUT_ADDRESS, 0);
#ifdef SL_DEBUG
		cnt++;
		ASSERT_COND(cnt >= TMAN_MAX_RETRIES);
#endif
	} while (res1 == TMAN_TMR_TMP_ERR1);
	/*Todo need to see if to remove the loop and to treat this as a fatal
	 * error. This is due to setting the time to TMAN process lagging */

	*timer_handle = res2;
	if (!((res1) & TMAN_FAIL_BIT_MASK))
			return (int)(TMAN_TMR_CREATE_SUCCESS);
	if ((res1 == TMAN_TMR_CONF_WAIT_ERR) ||
			(res1 == TMAN_TMR_DEPLETION_ERR))
				return (int)(-ENOSPC);
	tman_exception_handler(__FILE__, __LINE__, (int)res1);
}
#pragma warn_missingreturn on

int tman_delete_timer(uint32_t timer_handle, uint32_t flags)
{
	uint32_t res1;
#ifdef SL_DEBUG
	uint32_t cnt = 0;
#endif

	/* Store first two command parameters */
	/* Optimization: remove 1 cycle using EABI */
	__stdw(flags, timer_handle, HWC_ACC_IN_ADDRESS, 0);

	do {
		/* call TMAN. */
		__e_hwacceli(TMAN_ACCEL_ID);
		/* Load command results */
		res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
#ifdef SL_DEBUG
		cnt++;
		ASSERT_COND(cnt >= TMAN_MAX_RETRIES);
#endif
	} while (res1 == TMAN_TMR_TMP_ERR2);
	return (int)(res1);
}

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

int tman_recharge_timer(uint32_t timer_handle)
{
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_RECHARGE, res1;
#ifdef SL_DEBUG
	uint32_t cnt = 0;
#endif

	/* Store first two command parameters */
	__stdw(cmd_type, timer_handle, HWC_ACC_IN_ADDRESS, 0);

	/* call TMAN. */
	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	if (!((res1) & TMAN_FAIL_BIT_MASK))
			return (int)(TMAN_REC_TMR_SUCCESS);
	if (res1 == TMAN_REC_TMR_CURR_ELAPSE)
		return (int)(-ETIMEDOUT);
//	fatal-TMAN_REC_TMR_NOT_ACTIVE_ERR, TMAN_REC_TMR_BUSY and all TMI states
	return (int)(-ETIMEDOUT);
}

int tman_query_timer(uint32_t timer_handle,
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
	return (int)(TMAN_TMR_QUERY_SUCCESS);
}

void tman_timer_completion_confirmation(uint32_t timer_handle)
{
	*((uint32_t *)TMAN_TMCBCC_ADDRESS) = timer_handle;
}

void tman_get_timestamp(uint64_t *timestamp)
{
	*timestamp = *((uint64_t *) TMAN_TMTSTMP_ADDRESS);
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

#pragma pop

void tman_exception_handler(char *filename, uint32_t line, int32_t status)
{

	/*TODO Fatal error*/
	switch(status) {
	case TMAN_TMIID_DEPLETION_ERR:
		exception_handler(filename, line,
				"All TMIs are used. A TMI must be deleted \
				before a new one can be created.");
		break;
	case TMAN_ILLEGAL_DURATION_VAL_ERR:
		exception_handler(filename, line,
				"Illegal Timer duration.\
				The duration must have a value larger than 10 \
				ticks and smaller than 2^16-10 ticks.");
		break;
	case TMAN_TMR_TMI_NOT_ACTIVE:
		exception_handler(filename, line,
				"A non active TMI was provided as an input.");
		break;
	case TMAN_TMR_DEPLETION_ERR:
		exception_handler(filename, line,
				"No more available timers in the TMI.");
		break;

	case TMAN_DEL_TMR_NOT_ACTIVE_ERR:
		exception_handler(filename, line,
				"A non active timer was provided as an input.");
		break;
	case TMAN_DEL_CCP_WAIT_ERR:
		exception_handler(filename, line,
				"The one shot timer has expired but it is \
				pending a completion confirmation (done by \
				calling the tman_timer_completion_confirmation \
				function).");
		break;
	case TMAN_DEL_PERIODIC_CCP_WAIT_ERR:
		exception_handler(filename, line,
				"The periodic timer has expired but it is \
				pending a completion confirmation (done by \
				calling the tman_timer_completion_confirmation \
				function).");
		break;
	case TMAN_DEL_TMR_DEL_ISSUED_ERR:
		exception_handler(filename, line,
				"A delete command was already issued for this \
				timer and the TMAN is in the process of \
				deleting the timer. The timer will elapse in \
				the future.");
		break;
	case TMAN_DEL_TMR_DEL_ISSUED_CONF_ERR:
		exception_handler(filename, line,
				"A delete command was already issued. The \
				timer has already elapsed for the last time \
				and it is pending a completion confirmation \
				(done by calling the \
				tman_timer_completion_confirmation function)");
		break;

	case TMAN_MOD_TMR_NOT_ACTIVE_ERR:
		exception_handler(filename, line,
				"A non active timer was provided as an input.");
		break;
	case TMAN_MOD_CCP_WAIT_ERR:
		exception_handler(filename, line,
				"The one shot timer has expired but it is \
				pending a completion confirmation (done by \
				calling the tman_timer_completion_confirmation \
				function).");
		break;
	case TMAN_MOD_PERIODIC_CCP_WAIT_ERR:
		exception_handler(filename, line,
				"The periodic timer has expired but it is \
				pending a completion confirmation (done by \
				calling the tman_timer_completion_confirmation \
				function).");
		break;
	case TMAN_MOD_TMR_DEL_ISSUED_ERR:
		exception_handler(filename, line,
				"A delete command was already issued for this \
				timer and the TMAN is in the process of \
				deleting the timer. The timer will elapse in \
				the future.");
		break;
	case TMAN_MOD_TMR_DEL_ISSUED_CONF_ERR:
		exception_handler(filename, line,
				"A delete command was already issued. The \
				timer has already elapsed for the last time \
				and it is pending a completion confirmation \
				(done by calling the \
				tman_timer_completion_confirmation function)");
		break;


	default:
		exception_handler(filename, line,
				"UNKNOWN error occurred");
       }
}

