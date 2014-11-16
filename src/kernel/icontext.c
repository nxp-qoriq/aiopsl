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

#include "fsl_fdma.h"
#include "icontext.h"
#include "fsl_errors.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_fdma.h"
#include "fdma.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "fsl_icontext.h"
#include "fsl_mc_init.h"
#include "fsl_spinlock.h"
#include "cmdif_client_aiop.h" /* TODO remove it once you have lock per dpci table !!! */

struct icontext icontext_aiop = {0};

void icontext_aiop_get(struct icontext *ic)
{
	ASSERT_COND(ic != NULL);
	*ic = icontext_aiop;
}

int icontext_get(uint16_t dpci_id, struct icontext *ic)
{
	int i = 0;
	struct mc_dpci_obj *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);

#ifdef DEBUG
	if ((ic == NULL) || (dt == NULL) || (cl == NULL))
		return -EINVAL;
#endif
	lock_spinlock(&cl->lock); /* TODO make it lock per dpci table not client ! */
	/* search by GPP peer id - most likely case
	 * or by AIOP dpci id  - to support both cases
	 * All DPCIs in the world have different IDs */
	for (i = 0; i < dt->count; i++) {
		if ((dt->peer_attr[i].peer_id == dpci_id) ||
			(dt->attr[i].id == dpci_id)) {
			/* Fill icontext */
			ic->icid = dt->icid[i];
			ic->dma_flags = dt->dma_flags[i];
			ic->bdi_flags = dt->bdi_flags[i];
			unlock_spinlock(&cl->lock);
			return 0;
		}
	}

	unlock_spinlock(&cl->lock);
	return -ENAVAIL;
}

int icontext_dma_read(struct icontext *ic, uint16_t size, 
                                 uint64_t src, void *dest)
{

#ifdef DEBUG
	if ((dest == NULL) || (src == NULL))
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
	if ((dest == NULL) || (src == NULL))
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

