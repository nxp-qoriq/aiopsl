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

#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_malloc.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"
#include "fsl_icontext.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_io_ccsr.h"
#include "aiop_common.h"

#define TEST_DDR_SIZE	64
#define TEST_DDR_VAL	0xbe

struct icontext ic_aiop = {0};

int gpp_ddr_check(struct icontext *ic, uint64_t iova, uint16_t size);
int gpp_sys_ddr_init();
int gpp_sys_ddr_test(uint64_t iova, uint16_t size);

int gpp_sys_ddr_init()
{
	uint32_t cdma_cfg;
	struct aiop_tile_regs *ccsr = \
		(struct aiop_tile_regs *)
		(AIOP_PERIPHERALS_OFF +
		 SOC_PERIPH_OFF_AIOP_TILE +
		 SOC_PERIPH_OFF_AIOP_CMGW);

	cdma_cfg = ioread32_ccsr(&ccsr->cdma_regs.cfg);

	ic_aiop.icid = (uint16_t)(cdma_cfg & CDMA_ICID_MASK);

	ic_aiop.bdi_flags = 0;
	if (cdma_cfg & CDMA_BDI_BIT)
		ic_aiop.bdi_flags |= FDMA_ENF_BDI_BIT;

	ic_aiop.dma_flags = 0;
	if (cdma_cfg & CDMA_BMT_BIT)
		ic_aiop.dma_flags |= FDMA_DMA_BMT_BIT;
	if (cdma_cfg & CDMA_PL_BIT)
		ic_aiop.dma_flags |= FDMA_DMA_PL_BIT;
	if (cdma_cfg & CDMA_VA_BIT)
		ic_aiop.dma_flags |= FDMA_DMA_eVA_BIT;

	if (!ic_aiop.bdi_flags) return 1;
	if (!ic_aiop.dma_flags) return 2;

	return 0;
}

int gpp_ddr_check(struct icontext *ic, uint64_t iova, uint16_t size)
{
	int i = 0;
	uint8_t  buf[TEST_DDR_SIZE];

	if ((size > TEST_DDR_SIZE) || (size == 0))
		return 4;

	/* Clean */
	for (i = 0; i < size; i++) {
		buf[i] = 0;
	}
	icontext_dma_write(ic, size, &buf[0], iova);

	/* Write */
	for (i = 0; i < size; i++) {
		buf[i] = TEST_DDR_VAL;
	}
	icontext_dma_write(ic, size, &buf[0], iova);

	/* Check */
	for (i = 0; i < size; i++) {
		buf[i] = 0;
	}
	icontext_dma_read(ic, size, iova, &buf[0]);
	for (i = 0; i < size; i++) {
		if (buf[i] != TEST_DDR_VAL)
			return 8;
	}

	return 0;
}

int gpp_sys_ddr_test(uint64_t iova, uint16_t size)
{
	int err = 0;
	struct icontext ic;

	/* Each SW context has different icid:
	 * icid 0 - goes to MC
	 * icid 1 and up - goes to AIOP and GPP */
	ic.icid = (ic_aiop.icid == 1 ? 2 : 1);
	ic.dma_flags = 0; /* Should not change */
	ic.bdi_flags = 0; /* Should not change */

	/* Virtual addr of GPP may change */
	err |= gpp_ddr_check(&ic, iova, size);
	err |= gpp_ddr_check(&ic, iova, size);

	return err;
}
