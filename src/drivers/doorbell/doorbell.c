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

#include "fsl_sys.h"
#include "fsl_aiop_common.h"
#include "fsl_io_ccsr.h"
#include "fsl_dbg.h"
#include "fsl_doorbell.h"
#include "fsl_icontext.h"
#include "fsl_soc.h"
#include "fsl_fdma.h"

static struct aiop_cmgw_regs *reg = NULL;
static struct aiop_portal_map_regs *pm_reg = NULL;

static void ep_set(uint16_t epid,
                   void (*isr_cb)(void))
{
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
			sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	iowrite32_ccsr(epid, &wrks_addr->epas);
	iowrite32_ccsr(PTR_TO_UINT(isr_cb), &wrks_addr->ep_pc);

	/* no PTA presentation is required (even if there is a PTA)*/
	iowrite32_ccsr(0x0000ffc0, &wrks_addr->ep_ptapa);
	/* set epid ASA presentation size to 0 */
	iowrite32_ccsr(0x00000000, &wrks_addr->ep_asapa);
	/* Set mask for hash to 16 high bits 
	 * OSRM = 2 - because 2 low bits if scope id must be cleared
	 * mask for scope_id 0xFFFF_0000 
	 * SRC = 1 
	 * Scope ID taken from the specified slice of the received 
	 * frame’s FD[FLC]. Slice is specified in the SEL field.
	 * EP = 1
	 * Executing exclusively
	 * SEL = 0 
	 * Order Scope ID is taken from FLC[63:32]
	 * */
	iowrite32_ccsr(0x11000002, &wrks_addr->ep_osc);

	pr_info("Doorbell is setting EPID = %d\n", epid);
	pr_info("ep_pc = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_spo));
	pr_info("ep_osc = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_osc));

}

int doorbell_setup(int pr, enum doorbell_reg g_m, uint16_t epid,
                   void (*isr_cb)(void), uint32_t scope_id)
{
	struct icontext ic = {0};
	uint32_t temp;

	ASSERT_COND(scope_id <= 0xffff);

	if (reg == NULL) {
		reg = sys_get_unique_handle(FSL_OS_MOD_CMGW);
		pm_reg = (struct aiop_portal_map_regs *)\
			(SOC_PERIPH_OFF_PORTAL_MAP + AIOP_PERIPHERALS_OFF);
	}
	
#if 0
	pr_debug("%ddp%dcr1  addr 0x%x\n", g_m, pr,
	         (uint32_t)&reg->init_g_m[g_m].pr[pr].dpcr[0]);
	pr_debug("%ddp%dcr2  addr 0x%x\n", g_m, pr,
	         (uint32_t)&reg->init_g_m[g_m].pr[pr].dpcr[1]);
	pr_debug("%ddp%dcr3  addr 0x%x\n", g_m, pr,
	         (uint32_t)&reg->init_g_m[g_m].pr[pr].dpcr[2]);
#endif

	icontext_aiop_get(&ic);
	ASSERT_COND(ic.bdi_flags || ic.dma_flags || ic.icid);

	temp = (uint32_t)(epid & 0x03ff);
	iowrite32_ccsr(temp, &reg->init_g_m[g_m].pr[pr].dpcr[0]);
	
	temp = ic.icid;
	if (ic.bdi_flags & FDMA_ENF_BDI_BIT)
		temp |= 0x8000;
	if (ic.dma_flags & FDMA_DMA_PL_BIT)
		temp |= 0x40000;
	if (ic.dma_flags & FDMA_DMA_eVA_BIT)
		temp |= 0x10000;
	if (ic.dma_flags & FDMA_DMA_BMT_BIT)
		temp |= 0x20000;

	iowrite32_ccsr(temp, &reg->init_g_m[g_m].pr[pr].dpcr[1]);
	/* low bits of scope_id must be unused */
	scope_id <<= 16;
	iowrite32_ccsr(scope_id, &reg->init_g_m[g_m].pr[pr].dpcr[2]);
	
	/* Set the EP entry */
	ep_set(epid, isr_cb);
	
	doorbell_clear(pr, g_m, 0xffffffff);

	return 0;
}

void doorbell_clear(int pr, enum doorbell_reg g_m, uint32_t mask)
{
#if 0
	pr_debug("%ddp%dclrr  addr 0x%x\n", g_m, pr,
	         (uint32_t)&reg->clear_g_m[g_m].pr[pr].dpclrr);
#endif
	iowrite32_ccsr(mask, &reg->clear_g_m[g_m].pr[pr].dpclrr);
}

void doorbell_ring(int pr, enum doorbell_reg g_m, uint32_t mask)
{
#if 0
	pr_debug("%ddp%drr  addr 0x%x\n", g_m, pr,
	         (uint32_t)&pm_reg->req_g_m[g_m].pr[pr].dprr);
#endif
	iowrite32_ccsr(mask, &pm_reg->req_g_m[g_m].pr[pr].dprr);
}

