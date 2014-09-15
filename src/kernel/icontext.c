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

int icontext_get(uint16_t dpci_id, struct icontext *ic)
{
	struct mc_dpci_obj *dt = cmdif_aiop_srv.dpci_tbl;

	ASSERT_COND((ic != NULL) && (dt != NULL));

	/* find dpci_id  */
	for (i = 0; i < dt->count; i++) {
		if (dt->attr[i].id == dpci_id) {
			/* Fill icontext */
			ic->icid = dt->icid[i];
			ic->dma_flags = dt->dma_flags[i];
			/* TODO add other flags */
			return 0;
		}
	}

	/* copy pointer from icid table */
	return -ENOENT;
}

int icontext_dma_read(struct icontext *ic, uint16_t size, uint64_t src, void *dest)
{
	ASSERT_COND(dest != NULL);
	ASSERT_COND(src != NULL);

	fdma_dma_data(size,
	              ic->icid,
	              dest,
	              src,
	              ic->dma_flags);
	return 0;
}

int icontext_dma_write(struct icontext *ic, uint16_t size, void *src, uint64_t dest)
{
	ASSERT_COND(src != NULL);
	ASSERT_COND(dest != NULL);

	fdma_dma_data(size,
	              ic->icid,
	              src,
	              dest,
	              ic->dma_flags | FDMA_DMA_DA_WS_TO_SYS_BIT);
	return 0;
}
