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

#include "fsl_fdma.h"
#include "icontext.h"
#include "fsl_errors.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "fsl_icontext.h"
#include "fsl_mc_init.h"
#include "fsl_spinlock.h"
#include "cmdif_srv.h"
#include "fsl_io_ccsr.h"
#include "fsl_dpci_mng.h"

#define ICONTEXT_SET(ICID, AMQ)	\
do { \
	ic->icid = (ICID); \
	ic->dma_flags = (uint32_t)((AMQ) & ~CMDIF_BDI_BIT); \
	if ((AMQ) & CMDIF_BDI_BIT) \
		ic->bdi_flags = FDMA_ENF_BDI_BIT; \
} while(0)

extern void dpci_rx_ctx_get(uint32_t *id, uint32_t *fqid);

struct icontext icontext_aiop = {0};

void icontext_aiop_get(struct icontext *ic)
{
	ASSERT_COND_LIGHT(ic != NULL);
	*ic = icontext_aiop;
}

void icontext_cmd_get(struct icontext *ic)
{
	uint32_t ind;
	uint16_t icid;
	uint16_t amq_bdi;

	dpci_mng_user_ctx_get(&ind, NULL);
	/*
	 * TODO
	 * Lock with cdma mutex READ
	 */
	dpci_mng_icid_get(ind, &icid, &amq_bdi);
	ICONTEXT_SET(icid, amq_bdi);
#ifndef BDI_BUG_FIXED
	/* Fix for GPP BDI */
	ic->bdi_flags &= ~FDMA_ENF_BDI_BIT;
#endif
	ASSERT_COND(ic->icid != ICONTEXT_INVALID);
}

int icontext_get(uint16_t dpci_id, struct icontext *ic)
{
	int ind = 0;
	uint16_t icid;
	uint16_t amq_bdi;

	ASSERT_COND(ic);
	/*
	 * TODO
	 * Is it possible that DPCI will be removed in the middle of the task ?
	 * If yes than we need read lock on dpci_mng_find() + dpci_mng_icid_get()
	 * NOTE : only dpci_peer_id can be updated but not dpci_id.
	 * Maybe it should not update peer id at all ?? 
	 * It should be updated only in dpci_drv_added() !!!
	 */

	/* search by GPP peer id - most likely case
	 * or by AIOP dpci id  - to support both cases
	 * All DPCIs in the world have different IDs */
	ind = dpci_mng_find(dpci_id, NULL);
	if (ind < 0)
		ind = dpci_mng_peer_find(dpci_id, NULL);

	if (ind >= 0) {
		dpci_mng_icid_get((uint32_t)ind, &icid, &amq_bdi);
		if (icid == ICONTEXT_INVALID)
			return -ENAVAIL;
		ICONTEXT_SET(icid, amq_bdi);
#ifndef BDI_BUG_FIXED
		ic->bdi_flags &= ~FDMA_ENF_BDI_BIT;
#endif
		return 0;
	}
	
	return -ENOENT;
}

int icontext_dma_read(struct icontext *ic, uint16_t size,
                                 uint64_t src, void *dest)
{

#ifdef DEBUG
	if ((dest == NULL) || (src == NULL) || (ic == NULL))
		return -EINVAL;
#endif

	fdma_dma_data(size,
	              ic->icid,
	              dest,
	              src,
	              ic->dma_flags);
	return 0;
}

int icontext_dma_write(struct icontext *ic, uint16_t size,
                                  void *src, uint64_t dest)
{

#ifdef DEBUG
	if ((dest == NULL) || (src == NULL) || (ic == NULL))
		return -EINVAL;
#endif

	fdma_dma_data(size,
	              ic->icid,
	              src,
	              dest,
	              ic->dma_flags | FDMA_DMA_DA_WS_TO_SYS_BIT);
	return 0;
}

int icontext_acquire(struct icontext *ic, uint16_t bpid,
                                uint64_t *addr)
{
	int err = 0;

#ifdef DEBUG
	if (ic == NULL)
		return -EINVAL;
#endif

	err = fdma_acquire_buffer(ic->icid, ic->bdi_flags, bpid, (void *)addr);

	return err;
}

int icontext_release(struct icontext *ic, uint16_t bpid,
                                uint64_t addr)
{
	int err = 0;

#ifdef DEBUG
	if (ic == NULL)
		return -EINVAL;
#endif

	fdma_release_buffer(ic->icid, ic->bdi_flags, bpid, addr);

	return err;
}

__COLD_CODE int icontext_init()
{
	uint32_t cdma_cfg;
	struct aiop_tile_regs *ccsr = (struct aiop_tile_regs *)\
		(AIOP_PERIPHERALS_OFF +
			SOC_PERIPH_OFF_AIOP_TILE +
			SOC_PERIPH_OFF_AIOP_CMGW);

	ASSERT_COND_LIGHT(ccsr);

	cdma_cfg = ioread32_ccsr(&ccsr->cdma_regs.cfg);

	icontext_aiop.icid = (uint16_t)(cdma_cfg & CDMA_ICID_MASK);

	icontext_aiop.bdi_flags = 0;
	if (cdma_cfg & CDMA_BDI_BIT)
		icontext_aiop.bdi_flags |= FDMA_ENF_BDI_BIT;

	icontext_aiop.dma_flags = 0;
	if (cdma_cfg & CDMA_BMT_BIT)
		icontext_aiop.dma_flags |= FDMA_DMA_BMT_BIT;
	if (cdma_cfg & CDMA_PL_BIT)
		icontext_aiop.dma_flags |= FDMA_DMA_PL_BIT;
	if (cdma_cfg & CDMA_VA_BIT)
		icontext_aiop.dma_flags |= FDMA_DMA_eVA_BIT;

	ASSERT_COND_LIGHT(icontext_aiop.bdi_flags); /* BDI bit is set */
	ASSERT_COND_LIGHT(icontext_aiop.dma_flags); /* PL bit is set */

	return 0;
}

