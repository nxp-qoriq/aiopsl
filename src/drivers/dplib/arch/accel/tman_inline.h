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
@File		tman_inline.h

@Description	This file contains the AIOP SW TMAN Inline API implementation.

*//***************************************************************************/

#ifndef __FSL_TMAN_INLINE_H
#define __FSL_TMAN_INLINE_H

#include "tman.h"
#include "fsl_tman.h"
#include "inline_asm.h"

/* The below pragma is to remove compilation warning:"return value expected" */
#pragma warn_missingreturn off
inline int tman_create_timer(uint8_t tmi_id, uint32_t flags,
			uint16_t duration, tman_arg_8B_t opaque_data1,
			tman_arg_2B_t opaque_data2, tman_cb_t tman_timer_cb,
			uint32_t *timer_handle)
{
	struct tman_tmi_input_extention extention_params
		__attribute__((aligned(TMAN_EXT_PARAM_ALIGNMENT)));
	uint32_t cmd_type = TMAN_CMDTYPE_TIMER_CREATE;
	unsigned int res1, res2;
	uint32_t epid = EPID_TIMER_EVENT_IDX;

	/* Fill command parameters */
	__stdw(cmd_type, (uint32_t)tmi_id, HWC_ACC_IN_ADDRESS, 0);

	/* extention_params.opaque_data1 = conf_opaque_data1; */
	__llstdw(opaque_data1, 0, (void *)(&extention_params.opaque_data1));

	extention_params.hash = (uint32_t)tman_timer_cb;
	/* extention_params.opaque_data2_epid =
			(uint32_t)(opaque_data2 << 16) | epid;
	Optimization: remove 2 cycles clear and shift */
	epid = __e_rlwimi(epid, opaque_data2, 16, 0, 15);
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
	/* call TMAN. */
	if ((__e_hwacceli_(TMAN_ACCEL_ID)) == TMAN_TMR_CREATE_SUCCESS) {
		__ldw(&res1, &res2, HWC_ACC_OUT_ADDRESS, 0);
		*timer_handle = res2;
		return (int)(TMAN_TMR_CREATE_SUCCESS);
	}
	/* Load command results */
	__ldw(&res1, &res2, HWC_ACC_OUT_ADDRESS, 0);
	
	if(res1 == TMAN_TMR_TMP_ERR1)
		return (int)(-EBUSY);
	if ((res1 == TMAN_TMR_CONF_WAIT_ERR) ||
			(res1 == TMAN_TMR_DEPLETION_ERR))
				return (int)(-ENOSPC);
	tman_exception_handler(TMAN_TMI_TIMER_CREATE_FUNC_ID, __LINE__, (int)res1);
}

inline int tman_delete_timer(uint32_t timer_handle, uint32_t flags)
{
	uint32_t res1;

	/* Store first two command parameters */
	/* Optimization: remove 1 cycle using EABI */
	__stdw(flags, timer_handle, HWC_ACC_IN_ADDRESS, 0);

	/* call TMAN. and check if passed.
	 * Optimization using compiler pattern*/

/* todo - change when compiler ticket ENGR00338394 is fixed */
	if(__e_hwacceli_(TMAN_ACCEL_ID) == TMAN_TMR_CREATE_SUCCESS)
		return (int)(TMAN_DEL_TMR_DELETE_SUCCESS);
	/* Load command results */
	res1 = *((uint32_t *) HWC_ACC_OUT_ADDRESS);

	/* The order of the error check is according to its frequency */

	/* To check if A=0 && CCP=1 */
	/* One shot - TO occurred.
	 * Periodic - Timer was deleted */
	if((res1 & TMAN_TMR_DEL_STATE_D_MASK) == TMAN_DEL_CCP_WAIT_ERR)
		return (int)(-EACCES);

	/* A=1 && CCP=1 */
	/* Periodic- cannot be deleted as it deals with TO */
	if(res1 == TMAN_DEL_PERIODIC_CCP_WAIT_ERR)
		return (int)(-ETIMEDOUT);
	/* To check if its a TMAN temporary error */
	if (res1 & TMAN_TMR_DEL_TMP_TYPE_MASK)
		return (int)(-ETIMEDOUT);

	/* The next code is due to Errata ERR008205 */
	if(res1 == TMAN_DEL_TMR_NOT_ACTIVE_ERR)
		return (int)(-ENAVAIL);
	/* End of Errata ERR008205 related code */

	/* In case TMI State errors and TMAN_DEL_TMR_NOT_ACTIVE_ERR,
	 * TMAN_DEL_TMR_DEL_ISSUED_ERR, TMAN_DEL_TMR_DEL_ISSUED_CONF_ERR */
	tman_exception_handler(TMAN_TMI_TIMER_DELETE_FUNC_ID, __LINE__, (int)res1);
}

#pragma warn_missingreturn on

inline void tman_get_timestamp(uint64_t *timestamp)
{
	*timestamp = *((uint64_t *) TMAN_TMTSTMP_ADDRESS);
}

inline void tman_timer_completion_confirmation(uint32_t timer_handle)
{
	*((uint32_t *)TMAN_TMCBCC_ADDRESS) = timer_handle;
}

#endif /* __FSL_TMAN_INLINE_H */
