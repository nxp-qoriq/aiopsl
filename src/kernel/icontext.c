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

struct ic_table ic = {0};

int icontext_add()
{
	uint16_t pl_icid = PL_ICID_GET;

	/* TODO find the ind where to add it */
	ic.icid[ind]           = IC_ICID_GET(pl_icid);
	ic.enq_flags[ind]      = FDMA_EN_TC_RET_BITS; /* don't change */
	ic.dma_flags[ind]      = FDMA_DMA_DA_SYS_TO_WS_BIT;
	IC_ADD_AMQ_FLAGS(ic.dma_flags[ind], pl_icid);
	if (IC_BDI_GET != 0)
		ic.enq_flags[ind] |= FDMA_ENF_BDI_BIT;

	/* TODO locks */

}

int icontext_rm(uint16_t icid)
{
	/* TODO locks */
}

int icontext_get(uint16_t icid, void **icontext)
{

}

int icontext_table_init()
{
	memset(&ic, 0, sizeof(struct ic_table));
}

int icontext_table_free()
{
	memset(&ic, 0, sizeof(struct ic_table));
}
