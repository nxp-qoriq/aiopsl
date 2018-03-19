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
@File		dpni_drv_rxtx_inline.h

@Description	Data Path Network Interface Inline API
*//***************************************************************************/
#ifndef __DPNI_DRV_RXTX_INLINE_H
#define __DPNI_DRV_RXTX_INLINE_H

#include "drv.h"
#include "general.h"
#include "fsl_types.h"
#include "fsl_fdma.h"
#include "fsl_parser.h"
#include "osm_inline.h"
#include "fsl_dbg.h"

extern __TASK struct aiop_default_task_params default_task_params;
extern struct dpni_drv *nis;
extern __PROFILE_SRAM
	struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];
extern uint8_t egress_parse_profile_id;

static inline void sl_prolog_common(void)
{
	struct dpni_drv *dpni_drv;
#ifndef AIOP_VERIF
#ifndef DISABLE_ASSERTIONS
	struct dpni_drv_params dpni_drv_params_local
				__attribute__((aligned(8)));
#endif
#endif
	
	dpni_drv = nis + PRC_GET_PARAMETER(); /* calculate pointer
						* to the send NI structure   */
	osm_task_init();

	/* Load from SHRAM to local stack */
#ifndef AIOP_VERIF
#ifndef DISABLE_ASSERTIONS

	dpni_drv_params_local = dpni_drv->dpni_drv_params_var;
#ifdef LS2085A_REV1
	ASSERT_COND_LIGHT(dpni_drv_params_local.starting_hxs == 0);
#endif
	ASSERT_COND_LIGHT(dpni_drv_params_local.prpid == 0);
	ASSERT_COND_LIGHT(dpni_drv_params_local.flags & DPNI_DRV_FLG_PARSE);
	ASSERT_COND_LIGHT(dpni_drv_params_local.flags & DPNI_DRV_FLG_PARSER_DIS);
#endif
#endif

	*((uint8_t *)HWC_SPID_ADDRESS) = dpni_drv->dpni_drv_params_var.spid;

	SET_FRAME_TYPE(PRC_GET_FRAME_HANDLE(), HWC_FD_ADDRESS);
}

inline int sl_prolog(void)
{
	int err;
	struct parse_result *pr;
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);
	
	default_task_params.parser_profile_id = 0;
	default_task_params.parser_starting_hxs = 0;
	default_task_params.qd_priority = ((*((uint8_t *)(HWC_ADC_ADDRESS + \
			ADC_WQID_PRI_OFFSET)) & ADC_WQID_MASK) >> 4);
	err = parse_result_generate_basic();
	sl_prolog_common();
	return err;
}

inline int sl_prolog_with_ref_take(void)
{
	int err;
	struct parse_result *pr;
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);
	
	default_task_params.parser_profile_id = 0;
	default_task_params.parser_starting_hxs = 0;
	default_task_params.qd_priority = ((*((uint8_t *)(HWC_ADC_ADDRESS + \
			ADC_WQID_PRI_OFFSET)) & ADC_WQID_MASK) >> 4);

	err = parse_result_generate_basic_with_ref_take();
	
	sl_prolog_common();
	return err;
}

inline int sl_prolog_with_custom_header(uint16_t start_hxs)
{
	int			err;
	struct parse_result	*pr;

#ifdef LS2085A_REV1
	/* On Rev1 platforms having a custom header, as the first header in the
	 * packet, is not supported. The starting HXS must be 0 (Ethernet) and
	 * the starting offset must be 0.*/
	start_hxs = 0;
#endif
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);
	default_task_params.parser_profile_id = 0;
	default_task_params.parser_starting_hxs = start_hxs;
	default_task_params.qd_priority =
		((*((uint8_t *)(HWC_ADC_ADDRESS + ADC_WQID_PRI_OFFSET)) &
		    ADC_WQID_MASK) >> 4);
#ifdef LS2085A_REV1
	/* Packet parsing is started by a hard HXS. */
	err = parse_result_generate((enum parser_starting_hxs_code)start_hxs,
				    0, 0);
#else
	/* Packet parsing is started by the soft parser loaded at the
	 * "start_hxs" PC address in the instructions memory of the AIOP
	 * Parser. */
	err = parse_result_generate(start_hxs, 0, 0);
#endif
	sl_prolog_common();
	return err;
}

static inline void dpni_send_prepare(uint16_t ni_id,
			struct fdma_queueing_destination_params *enqueue_params)
{
	struct dpni_drv *dpni_drv;
#ifndef AIOP_VERIF
#ifndef DISABLE_ASSERTIONS
	struct dpni_drv_params dpni_drv_params_local
				__attribute__((aligned(8)));
#endif
#endif
	/*struct dpni_drv_tx_params dpni_drv_tx_params_local
				__attribute__((aligned(8)));*/

	dpni_drv = nis + ni_id; /* calculate pointer
					* to the send NI structure   */

	/* Load from SHRAM to local stack */
#ifndef AIOP_VERIF
#ifndef DISABLE_ASSERTIONS
	dpni_drv_params_local = dpni_drv->dpni_drv_params_var;
	ASSERT_COND_LIGHT(!(dpni_drv_params_local.flags & DPNI_DRV_FLG_MTU_ENABLE));
#endif
#endif
	/*dpni_drv_tx_params_local = dpni_drv->dpni_drv_tx_params_var;*/

	/* take SPID from TX NIC - not needed since same SPID as receive */
	//*((uint8_t *)HWC_SPID_ADDRESS) = dpni_drv_params_local.spid;
	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params->qdbin = 0;
	enqueue_params->qd = dpni_drv->dpni_drv_tx_params_var.qdid;
	enqueue_params->qd_priority = default_task_params.qd_priority;
}

inline int dpni_drv_send(uint16_t ni_id, uint32_t flags)
{
	struct fdma_queueing_destination_params    enqueue_params;
	uint32_t dpni_attributes;

	dpni_attributes = flags & DPNI_DRIVER_SEND_MODE_ATTRIBUTE_MASK;
	flags &= ~DPNI_DRIVER_SEND_MODE_ATTRIBUTE_MASK;

	dpni_send_prepare(ni_id, &enqueue_params);

	if (dpni_attributes == DPNI_DRV_SEND_MODE_ORDERED)
		return fdma_store_and_ordered_enqueue_default_frame_qd(
				&enqueue_params, flags);
	if (dpni_attributes == DPNI_DRV_SEND_MODE_PRESTORE_ORDERED)
		return fdma_prestore_and_ordered_enqueue_default_fd_qd(
				flags, &enqueue_params);
		return fdma_store_and_enqueue_default_frame_qd(&enqueue_params, flags);
}

inline void sl_tman_expiration_task_prolog(uint16_t spid)
{
	struct fdma_amq amq;
	uint16_t icid, flags = 0;
	uint8_t tmp;
	struct storage_profile *sp_addr = &storage_profile[0];

	sp_addr += spid;
	*((uint8_t *)HWC_SPID_ADDRESS) = (uint8_t)spid;
	default_task_params.parser_profile_id = 0;
	default_task_params.parser_starting_hxs = 0;
	icid = (uint16_t)(sp_addr->ip_secific_sp_info >> 48);
	icid = ((icid << 8) & 0xff00) | ((icid >> 8) & 0xff);
	tmp = (uint8_t)(sp_addr->ip_secific_sp_info >> 40);
	if (tmp & 0x08)
		flags |= FDMA_ICID_CONTEXT_BDI;
	if (tmp & 0x04)
		flags |= FDMA_ICID_CONTEXT_PL;
	if (sp_addr->mode_bits2 & sp1_mode_bits2_VA_MASK)
		flags |= FDMA_ICID_CONTEXT_VA;
	amq.icid = icid;
	amq.flags = flags;
	set_default_amq_attributes(&amq);
}

inline uint16_t task_get_receive_niid(void)
{
	return (uint16_t)PRC_GET_PARAMETER();
}

inline void task_set_tx_tc(uint8_t tc)
{
	default_task_params.qd_priority = tc;
}

inline uint8_t task_get_tx_tc(void)
{
	return default_task_params.qd_priority;
}

inline void task_switch_to_egress_parse_profile(uint16_t start_hxs)
{
	default_task_params.parser_profile_id = egress_parse_profile_id;
	default_task_params.parser_starting_hxs = start_hxs;
}

#endif /* __DPNI_DRV_RXTX_INLINE_H */
