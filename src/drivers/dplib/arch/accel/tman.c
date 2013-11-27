/**************************************************************************//**
@File		tman.c

@Description	This file contains the AIOP SW TMAN API	implementation.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "general.h"
#include "tman.h"
#include "dplib/fsl_tman.h"


int32_t tman_create_tmi(uint64_t tmi_mem_base_addr,
			uint32_t max_num_of_timers, uint8_t *tmi_id)
{
	/* command parameters and results */
	uint32_t arg1, arg2, res1, res2;

	/* The bellow two code lines are there because of compiler warning */
	/* TODO need to remove the bellow lines when compiler will be fixed*/
	res1=0;
	res2=0;
	/******************************************************************/
	
	/* Load ICID, PL and BDI */
	__lwbrx(arg1, HWC_ADC_ADDRESS + ADC_PL_ICID_OFFSET);
	/* isolate PL and BDI bits */
	arg2 = ((arg1 << 16) & 0x80000000) | (arg1 << 31)
		| max_num_of_timers;
	/* Isolate ICID */
	arg1 = ((arg1 << 16) & 0x7FFF0000) + TMAN_CMDTYPE_TMI_CREATE;
	/* Store command parameters */
	__stdw(arg1, arg2, HWC_ACC_IN_ADDRESS, 0);
	__st64dw_d(tmi_mem_base_addr, HWC_ACC_IN_ADDRESS + 8);
	/* call TMAN. */
	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	__ldw(res1, res2, HWC_ACC_OUT_ADDRESS, 0);
	/* Store tmi_id */
	*tmi_id = (uint8_t)res2;
	/* Isolate status bits from the TMAN output */
	res1 &= TMAN_STATUS_MASK;
	return (int32_t)(res1);
}

int32_t tman_delete_tmi(uint8_t confirmation_epid, uint32_t flags,
			uint8_t tmi_id, uint64_t conf_opaque_data1,
			uint16_t conf_opaque_data2)
{
	struct tman_tmi_input_extention extention_params;
	/* command parameters and results */
	uint32_t res1;

	/* extention_params.conf_opaque_data1 = conf_opaque_data1;
	Optimization: remove 1 cycle of store word (this rely on EABI) */
	__st64dw_b(conf_opaque_data1,
		  (uint32_t)(&extention_params.opaque_data1));
	/* A non nested Scope ID */
	extention_params.hash = 0x1070;
	/* extention_params.opaque_data2_epid =
		(uint32_t)(conf_opaque_data2 << 16) | confirmation_epid;
	Optimization: remove 2 cycles clear and shift */
	__e_rlwimi(confirmation_epid, conf_opaque_data2, 16, 0, 15);
	__stw(confirmation_epid, 0, &(extention_params.opaque_data2_epid));
	/* Store first two command parameters */
	__stdw(flags, tmi_id, HWC_ACC_IN_ADDRESS, 0);

	/* arg1 = (uint32_t *)(HWC_ACC_OUT_ADDRESS + 8);
	*arg1 = (uint32_t)(&extention_params);
	Optimization: remove 1 cycle of address calc */
	__stw_d(&extention_params, HWC_ACC_IN_ADDRESS + 8);
	/* call TMAN. */
	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	/* Isolate status bits from the TMAN output */
	res1 &= TMAN_STATUS_MASK;
	return (int32_t)(res1);
}

int32_t tman_query_tmi(uint8_t tmi_id,
			struct tman_tmi_params *output_ptr)
{
	/* command parameters and results */
	uint32_t cmd_type = TMAN_CMDTYPE_TMI_QUERY, res1;
	/* Optimization: stdw is used to give the compiler chance to optimize
	   when using an inline function */
	__stdw(cmd_type, tmi_id, HWC_ACC_IN_ADDRESS, 0);
	/* Fill command parameters */
	__stw_d(output_ptr, HWC_ACC_IN_ADDRESS + 0xc);
	/* call TMAN. */
	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	/* Erase PL and BDI from output_ptr */
	*((uint8_t *)(&output_ptr->max_num_of_timers) + 3) = 0;
	/* Isolate status bits from the TMAN output */
	res1 &= TMAN_STATUS_MASK;
	return (int32_t)(res1);
}

int32_t tman_create_timer(uint8_t tmi_id, uint32_t flags,
			uint16_t duration, uint64_t opaque_data1,
			uint16_t opaque_data2, uint8_t epid,
			uint32_t scope_id, uint32_t *timer_handle)
{
	struct tman_tmi_input_extention extention_params;
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_CREATE, res1;

	/* Fill command parameters */
	__stdw(cmd_type, (uint32_t)tmi_id, HWC_ACC_IN_ADDRESS, 0);

	/* extention_params.opaque_data1 = conf_opaque_data1;
	todo need to replace that with __st64dw when compiler intrinsic will
	be ready which will remove one cycle - e_add16i r3,rsp,16*/

	__st64dw_b(opaque_data1,
		  (uint32_t)(&extention_params.opaque_data1));
	extention_params.hash = scope_id;
	/* extention_params.opaque_data2_epid =
			(uint32_t)(opaque_data2 << 16) | epid;
	Optimization: remove 2 cycles clear and shift */
	__e_rlwimi(epid, opaque_data2, 16, 0, 15);
	__stw(epid, 0, &(extention_params.opaque_data2_epid));

	/* arg1 = (uint32_t *)(HWC_ACC_OUT_ADDRESS + 8);
	*arg1 = (uint32_t)(&extention_params);
	Optimization: remove 1 cycle of address calc */
	__stw_d(&extention_params, HWC_ACC_IN_ADDRESS + 8);

	/* Fill command parameters */
	/* Optimization: remove 1 cycle of clearing duration upper bits
	   (no casting is allowed) */
	__or(flags, flags, duration);
	*(uint32_t *)(HWC_ACC_IN_ADDRESS + 0xc) = flags;

	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	*timer_handle = *((uint32_t *) (HWC_ACC_OUT_ADDRESS+4));
	/* Isolate status bits from the TMAN output */
	res1 &= TMAN_STATUS_MASK;
	return (int32_t)(res1);
}

int32_t tman_delete_timer(uint32_t timer_handle, uint32_t flags)
{
	uint32_t res1;

	/* Store first two command parameters */
	/* Optimization: remove 1 cycle using EABI */
	__stdw(flags, timer_handle, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	/* Isolate status bits from the TMAN output */
	res1 &= TMAN_STATUS_MASK;
	return (int32_t)(res1);
}

int32_t tman_increase_timer_duration(uint32_t timer_handle, uint16_t duration)
{
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_MODIFY, res1;

	/* Store first two command parameters */
	/* Optimization: remove 1 cycle using EABI */
	__stdw(cmd_type, timer_handle, HWC_ACC_IN_ADDRESS, 0);
	/* Store third command parameters */
	/* Optimization: remove 1 cycle of clearing duration upper bits */
	*(uint16_t *)(HWC_ACC_IN_ADDRESS + 0x8) = duration;

	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	/* Isolate status bits from the TMAN output */
	res1 &= TMAN_STATUS_MASK;
	return (int32_t)(res1);
}

int32_t tman_recharge_timer(uint32_t timer_handle)
{
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_RECHARGE, res1;

	/* Store first two command parameters */
	__stdw(cmd_type, timer_handle, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	/* Isolate status bits from the TMAN output */
	res1 &= TMAN_STATUS_MASK;
	return (int32_t)(res1);
}

int32_t tman_query_timer(uint32_t timer_handle,
			enum e_tman_query_timer *state)
{
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_QUERY, res1;

	/* Store first two command parameters */
	__stdw(cmd_type, timer_handle, HWC_ACC_IN_ADDRESS, 0);

	__e_hwacceli(TMAN_ACCEL_ID);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);
	/* performs *state = res1 & 0x03 */
	/* Optimization: remove 2 cycles of clearing duration upper bits */
	*((uint8_t *)(state)) = (uint8_t)res1;
	/* Isolate status bits from the TMAN output */
	res1 &= TMAN_STATUS_MASK;
	return (int32_t)(res1 & 0xF8000000);
}

void tman_timer_completion_confirmation(uint32_t timer_handle)
{
	*((uint32_t *)TMAN_TMCBCC_ADDRESS) = timer_handle;
}

void tman_get_timestamp(uint64_t *timestamp)
{
	/* todo reevaluate when compiler performance issue resolved
	 * int32_t  reg_addr = (uint32_t)TMAN_TMTSTMP_ADDRESS; */
	*timestamp = *((uint64_t *) TMAN_TMTSTMP_ADDRESS);
	/* todo __ld64dw_b(*timestamp, reg_addr);*/
	/* todo __st64dw_b(*timestamp, timestamp); */
}

