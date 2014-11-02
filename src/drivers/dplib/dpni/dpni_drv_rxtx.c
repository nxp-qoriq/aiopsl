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

#include "general.h"
#include "types.h"
#include "fsl_dpni.h"
#include "fsl_fdma.h"
#include "fsl_parser.h"
#include "osm.h"

#include "fsl_dpni_drv.h"


#define __ERR_MODULE__  MODULE_DPNI


extern __TASK struct aiop_default_task_params default_task_params;

/* TODO - get rid */
extern __SHRAM struct dpni_drv *nis;

#pragma push
#pragma force_active on

void receive_cb(void)
{	
	struct dpni_drv *dpni_drv;
#ifndef AIOP_VERIF
#ifndef DISABLE_ASSERTIONS
	struct dpni_drv_params dpni_drv_params_local
				__attribute__((aligned(8)));
#endif
#endif
	struct parse_result *pr;
	int32_t parse_status;

	dpni_drv = nis + PRC_GET_PARAMETER(); /* calculate pointer
						* to the send NI structure   */
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);

	osm_task_init();

	/* Load from SHRAM to local stack */
#ifndef AIOP_VERIF
#ifndef DISABLE_ASSERTIONS
	dpni_drv_params_local = dpni_drv->dpni_drv_params_var;

	ASSERT_COND_LIGHT(dpni_drv_params_local.starting_hxs == 0);
	ASSERT_COND_LIGHT(dpni_drv_params_local.prpid == 0);
	ASSERT_COND_LIGHT(dpni_drv_params_local.flags & DPNI_DRV_FLG_PARSE);
	ASSERT_COND_LIGHT(dpni_drv_params_local.flags & DPNI_DRV_FLG_PARSER_DIS);
#endif
#endif

	*((uint8_t *)HWC_SPID_ADDRESS) = dpni_drv->dpni_drv_params_var.spid;
	default_task_params.parser_profile_id = 0;
	default_task_params.parser_starting_hxs = 0;
	default_task_params.qd_priority = ((*((uint8_t *)(HWC_ADC_ADDRESS + \
			ADC_WQID_PRI_OFFSET)) & ADC_WQID_MASK) >> 4);
	
	parse_status = parse_result_generate_basic();
	if (parse_status) {
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		fdma_terminate_task();
	}

	dpni_drv->rx_cbs(dpni_drv->arg);
	fdma_terminate_task();
}


#pragma pop

int dpni_drv_explicit_send(uint16_t ni_id, struct ldpaa_fd *fd)
{
	struct dpni_drv *dpni_drv;
	struct fdma_queueing_destination_params    enqueue_params;
#ifndef AIOP_VERIF
#ifndef DISABLE_ASSERTIONS
	struct dpni_drv_params dpni_drv_params_local
				__attribute__((aligned(8)));
#endif
#endif
	/*struct dpni_drv_tx_params dpni_drv_tx_params_local
				__attribute__((aligned(8)));*/
	int err;
	uint32_t flags = 0;
	uint16_t icid;
	uint8_t va_bdi;

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

	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params.qdbin = 0;
	enqueue_params.qd = dpni_drv->dpni_drv_tx_params_var.qdid;
	enqueue_params.qd_priority = default_task_params.qd_priority;
	/* Assuming user already called fdma_create_frame() and saved fd in the
	 *  TLS */
	/* TODO maybe in future HW the fdma_enqueue_fd_qd command will support
	 * taking ICID and relevant bits from default values */
	/* It is more accurate taking the ICID from the SPID but in AIOP 
	 * it should be the same value as the default */
	va_bdi = *((uint8_t *)(HWC_ADC_ADDRESS + ADC_FDSRC_VA_FCA_BDI_OFFSET));
	if (va_bdi & ADC_BDI_MASK)
		flags |= FDMA_ENF_BDI_BIT;
	/*if (va_bdi & ADC_VA_MASK)
		flags |= FDMA_ENF_VA_BIT;*/
	icid = LH_SWAP(HWC_ADC_ADDRESS + ADC_PL_ICID_OFFSET, 0);
	/*if (icid & ADC_PL_MASK)
		flags |= FDMA_ENF_PL_BIT;*/
	icid &= ADC_ICID_MASK;
	err = (int)fdma_enqueue_fd_qd(fd, flags, &enqueue_params, icid);
	return err;
}

/* TODO : replace by macros/inline funcs */
int dpni_get_receive_niid(void)
{
	return (int)PRC_GET_PARAMETER();
}


/* TODO : replace by macros/inline funcs */
int dpni_set_send_niid(uint16_t niid)
{
	default_task_params.send_niid = niid;
	return 0;
}


/* TODO : replace by macros/inline funcs */
int dpni_get_send_niid(void)
{
	return (int)default_task_params.send_niid;
}

