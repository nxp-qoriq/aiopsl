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
@File		fsl_dpci_mng.h

@Description	DPCI wrapper that is internally used by Service Layer
*//***************************************************************************/

#ifndef __FSL_DPCI_MNG_H
#define __FSL_DPCI_MNG_H

#include "fsl_dpci.h"

#define DPCI_DYNAMIC_MAX	64

struct dpci_mng_tbl {
	uint32_t mc_dpci_id;	/**< DPCI id used by MC to send events */
	int32_t  count;
	int      max;
	uint32_t ic[DPCI_DYNAMIC_MAX];				/**< 0xFFFFFFFF is not valid, must be atomic*/
	uint32_t dpci_id[DPCI_DYNAMIC_MAX];			/**< dpci ids not tokens */
	uint32_t dpci_id_peer[DPCI_DYNAMIC_MAX];		/**< dpci ids not tokens */
	uint32_t tx_queue[DPCI_DYNAMIC_MAX][DPCI_PRIO_NUM];	/**< Use DPCI_PRIO_NUM between DPCI jumps */
	uint16_t token[DPCI_DYNAMIC_MAX];			/**< TODO use it dpci token for open session */
	uint8_t  state[DPCI_DYNAMIC_MAX];			/**< TODO use it */
};

/**************************************************************************//**
@Function	dpci_mng_user_ctx_get

@Description	Read the DPCI index and FQID from user context in ADC.

@Param[out]	dpci_ind - Index to the DPCI table entry.
@Param[out]	fqid - fqid for tx
 *//***************************************************************************/
void dpci_mng_user_ctx_get(uint32_t *dpci_ind, uint32_t *fqid);


/**************************************************************************//**
@Function	dpci_mng_icid_get

@Description	Read the ICID + AMQ + BDI from DPCI entry.

@Param[in]	dpci_ind - Index to the DPCI table entry.
@Param[out]	icid - icid that belongs to this DPCI
@Param[out]	amq_bdi - AMQ + BDI that belong to this DPCI
 *//***************************************************************************/
void dpci_mng_icid_get(uint32_t dpci_ind, uint16_t *icid, uint16_t *amq_bdi);

/**************************************************************************//**
@Function	dpci_mng_tx_get

@Description	Read the ICID + AMQ + BDI from DPCI entry.

@Param[in]	dpci_ind - Index to the DPCI table entry.
@Param[out]	pr - High or Low priority
@Param[out]	fqid - The frame queue id
 *//***************************************************************************/
void dpci_mng_tx_get(uint32_t dpci_ind, int pr, uint32_t *fqid);
/*
 * Returns dpci index on success or error otherwise
 * Once dpci id is added to the table it can't be remove but only invalidated
 */
int dpci_mng_find(uint32_t dpci_id, uint32_t *ic);
int dpci_mng_peer_find(uint32_t dpci_id, uint32_t *ic);

#endif /* __FSL_DPCI_MNG_H */
