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
@File		dpni_drv_rxtx_inline.h

@Description	Data Path Network Interface Inline API
*//***************************************************************************/
#ifndef __DPNI_DRV_RXTX_INLINE_H
#define __DPNI_DRV_RXTX_INLINE_H

#include "drv.h"
#include "general.h"
#include "types.h"
#include "fsl_fdma.h"

extern __TASK struct aiop_default_task_params default_task_params;
extern __SHRAM struct dpni_drv *nis;

__HOT_CODE inline int dpni_drv_send(uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	struct fdma_queueing_destination_params    enqueue_params;
	struct dpni_drv_params dpni_drv_params_local
				__attribute__((aligned(8)));
	struct dpni_drv_tx_params dpni_drv_tx_params_local
				__attribute__((aligned(8)));
	int err;

	dpni_drv = nis + ni_id; /* calculate pointer
					* to the send NI structure   */

	/* Load from SHRAM to local stack */
	dpni_drv_params_local = dpni_drv->dpni_drv_params_var;
	dpni_drv_tx_params_local = dpni_drv->dpni_drv_tx_params_var;

	/* take SPID from TX NIC*/
	*((uint8_t *)HWC_SPID_ADDRESS) = dpni_drv_params_local.spid;
	if ((dpni_drv_params_local.flags & DPNI_DRV_FLG_MTU_ENABLE) &&
		(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) >
					dpni_drv_tx_params_local.mtu))
			return DPNI_DRV_MTU_ERR;
	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params.qdbin = 0;
	enqueue_params.qd = dpni_drv_tx_params_local.qdid;
	enqueue_params.qd_priority = default_task_params.qd_priority;
	err = (int)fdma_store_and_enqueue_default_frame_qd(&enqueue_params, \
			FDMA_ENWF_NO_FLAGS);
	return err;
}


#endif /* __DPNI_DRV_RXTX_INLINE_H */