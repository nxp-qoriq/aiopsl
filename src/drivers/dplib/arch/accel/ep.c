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
#include "fsl_cdma.h"
#include "fsl_ep.h"
#include "fsl_ep_mng.h"
#include "aiop_common.h"
#include "fsl_io_ccsr.h"

/* Supported options for initializing presentation fields*/
#define EP_SUPPORTED_INIT_PRESENTATION_OPTIONS    \
		EP_INIT_PRESENTATION_OPT_PTA   |  \
		EP_INIT_PRESENTATION_OPT_ASAPA |  \
		EP_INIT_PRESENTATION_OPT_ASAPO |  \
		EP_INIT_PRESENTATION_OPT_ASAPS |  \
		EP_INIT_PRESENTATION_OPT_SPA   |  \
		EP_INIT_PRESENTATION_OPT_SPS   |  \
		EP_INIT_PRESENTATION_OPT_SPO   |  \
		EP_INIT_PRESENTATION_OPT_SR    |  \
		EP_INIT_PRESENTATION_OPT_NDS

/* Initial Presentation settings, masks, shift */
#define FDPA_MASK                 0xFFE00000
#define FDPA_SHIFT                21
#define ADPCA_MASK                0x0000FFE0
#define ADPCA_SHIFT               5
#define PTAPA_MASK                0x0000FFC0
#define PTAPA_SHIFT               6
#define ASAPS_MASK                0x000F0000
#define ASAPS_SHIFT               16
#define ASAPA_MASK                0x0000FFC0
#define ASAPA_SHIFT               6
#define ASAPO_MASK                0x0000000F
#define SPA_MASK                  0x0000FFFF
#define SPS_MASK                  0xFFFF0000
#define SPS_SHIFT                 16
#define SR_MASK                   0x80000000
#define SR_SHIFT                  31
#define NDS_MASK                  0x02000000
#define NDS_SHIFT                 25
#define SPO_MASK                  0x0000FFFF

extern void discard_rx_cb();
extern void tman_timer_callback(void);
extern void cmdif_cl_isr(void);
extern void cmdif_srv_isr(void);

int ep_mng_init(void);
void ep_mng_free(void);


int ep_mng_get_initial_presentation(
	uint16_t epid,
	struct ep_init_presentation* const init_presentation)
{
	uint32_t ep_fdpa;
	uint32_t ep_ptapa;
	uint32_t ep_asapa;
	uint32_t ep_spa;
	uint32_t ep_spo;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
		sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

#ifdef DEBUG
	if(init_presentation == NULL)
		return -EINVAL;
#endif

	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_READ_LOCK);

	/* write epid index to epas register */
	iowrite32_ccsr((uint32_t)(epid), &wrks_addr->epas);
	/* read ep_fdpa - to get Entry Point Frame Descriptor Presentation
	 * Address */
	ep_fdpa = ioread32_ccsr(&wrks_addr->ep_fdpa);
	/* read ep_ptapa - to get Entry Point Pass Through Annotation
	 * Presentation Address */
	ep_ptapa = ioread32_ccsr(&wrks_addr->ep_ptapa);
	/* read ep_asapa - to get Entry Point Accelerator Specific
	 * Annotation Presentation Address */
	ep_asapa = ioread32_ccsr(&wrks_addr->ep_asapa);
	/* read ep_spa - to get Entry Point Segment Presentation
	 * Address */
	ep_spa = ioread32_ccsr(&wrks_addr->ep_spa);
	/* read ep_spo - to get Entry Point Segment Presentation Offset
	 * Address */
	ep_spo = ioread32_ccsr(&wrks_addr->ep_spo);

	/*Mutex unlock EPID table*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);

	init_presentation->fdpa = (uint16_t)
			((ep_fdpa & FDPA_MASK) >> FDPA_SHIFT);

	init_presentation->adpca = (uint16_t)
			((ep_fdpa & ADPCA_MASK) >> ADPCA_SHIFT);

	init_presentation->ptapa = (uint16_t)
			((ep_ptapa & PTAPA_MASK) >> PTAPA_SHIFT);

	init_presentation->asapa = (uint16_t)
			((ep_asapa & ASAPA_MASK) >> ASAPA_SHIFT);

	init_presentation->asapo = (uint8_t) (ep_asapa & ASAPO_MASK);

	init_presentation->asaps = (uint8_t)
			((ep_asapa & ASAPS_MASK) >> ASAPS_SHIFT);

	init_presentation->spa = (uint16_t) (ep_spa & SPA_MASK);

	init_presentation->sps = (uint16_t)
			((ep_spa & SPS_MASK) >> SPS_SHIFT);
	init_presentation->sr = (uint8_t)
			((ep_spo & SR_MASK) >> SR_SHIFT);

	init_presentation->nds = (uint8_t)
			((ep_spo & NDS_MASK) >> NDS_SHIFT);

	init_presentation->spo = (uint16_t) (ep_spo & SPO_MASK);

	return 0;
}

int ep_mng_set_initial_presentation(
	uint16_t epid,
	const struct ep_init_presentation* const init_presentation)
{
	uint32_t ep_ptapa = 0;
	uint32_t ep_asapa = 0;
	uint32_t ep_spa = 0;
	uint32_t ep_spo = 0;
	uint32_t ep_temp;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
		sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

#ifdef DEBUG
	if(init_presentation == NULL)
		return -EINVAL;
	if(init_presentation->options == 0)
		return -EINVAL;
#endif
	if(init_presentation->options &
		~(EP_SUPPORTED_INIT_PRESENTATION_OPTIONS))
		return -ENOTSUP;

	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_PTA)
	{
		ep_ptapa |= (((uint32_t)(init_presentation->ptapa)
			<< PTAPA_SHIFT) & PTAPA_MASK);
	}
	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_ASAPA)
	{
		ep_asapa |= (((uint32_t)(init_presentation->asapa)
			<< ASAPA_SHIFT) & ASAPA_MASK);
	}
	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_ASAPO)
	{
		ep_asapa |= ((uint32_t)(init_presentation->asapo) & ASAPO_MASK);
	}
	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_ASAPS)
	{
		ep_asapa |= (((uint32_t)(init_presentation->asaps)
			<< ASAPS_SHIFT) & ASAPS_MASK);
	}
	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_SPA)
	{
		ep_spa |= ((uint32_t)(init_presentation->spa) & SPA_MASK);
	}
	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_SPS)
	{
		ep_spa |= (((uint32_t)(init_presentation->sps)
			<< SPS_SHIFT) & SPS_MASK);
	}
	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_SR)
	{
		ep_spo |= (((uint32_t)(init_presentation->sr)
			<< SR_SHIFT) & SR_MASK);
	}
	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_NDS)
	{
		ep_spo |= (((uint32_t)(init_presentation->nds)
			<< NDS_SHIFT) & NDS_MASK);
	}
	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_SPO)
	{
		ep_spo |= ((uint32_t)(init_presentation->spo) & SPO_MASK);
	}

	/*Mutex lock to avoid race condition while writing to EPID table*/
	cdma_mutex_lock_take((uint64_t)&wrks_addr->epas, CDMA_MUTEX_WRITE_LOCK);

	/* write epid index to epas register */
	iowrite32_ccsr((uint32_t)(epid), &wrks_addr->epas);

	if(init_presentation->options & EP_INIT_PRESENTATION_OPT_PTA)
	{
		/* read ep_ptapa - to get Entry Point Pass Through
		 * Annotation Presentation Address */
		ep_temp = ioread32_ccsr(&wrks_addr->ep_ptapa);
		/* Clear PTAAPA field */
		ep_temp &= ~PTAPA_MASK;

		ep_temp |= ep_ptapa;
		/* write ep_ptapa - to set Entry Point Pass Through
		 * Annotation Presentation Address */
		iowrite32_ccsr(ep_temp, &wrks_addr->ep_ptapa);
	}

	if(init_presentation->options & (EP_INIT_PRESENTATION_OPT_ASAPA |
		EP_INIT_PRESENTATION_OPT_ASAPO |
		EP_INIT_PRESENTATION_OPT_ASAPS))
	{
		/* read ep_asapa - to get Entry Point Accelerator Specific
		 * Annotation Presentation Address */
		ep_temp = ioread32_ccsr(&wrks_addr->ep_asapa);
		if(init_presentation->options & EP_INIT_PRESENTATION_OPT_ASAPA)
		{
			/* Clear ASAPA field */
			ep_temp &= ~ASAPA_MASK;
		}
		if(init_presentation->options & EP_INIT_PRESENTATION_OPT_ASAPO)
		{
			/* Clear ASAPO field */
			ep_temp &= ~ASAPO_MASK;
		}

		if(init_presentation->options & EP_INIT_PRESENTATION_OPT_ASAPS)
		{
			/* Clear ASAPS field */
			ep_temp &= ~ASAPS_MASK;
		}

		ep_temp |= ep_asapa;
		/* write ep_asapa - to set Entry Point Accelerator Specific
		 * Annotation Presentation Address */
		iowrite32_ccsr(ep_temp, &wrks_addr->ep_asapa);
	}

	if(init_presentation->options & (EP_INIT_PRESENTATION_OPT_SPA |
		EP_INIT_PRESENTATION_OPT_SPS))
	{
		/* read ep_spa - to get Entry Point Segment Presentation
		 * Address */
		ep_temp = ioread32_ccsr(&wrks_addr->ep_spa);
		if(init_presentation->options & EP_INIT_PRESENTATION_OPT_SPA)
		{
			/* Clear SPA field */
			ep_temp &= ~SPA_MASK;
		}
		if(init_presentation->options & EP_INIT_PRESENTATION_OPT_SPS)
		{
			/* Clear SPS field */
			ep_temp &= ~SPS_MASK;
		}

		ep_temp |= ep_spa;
		/* write ep_spa - to set Entry Point Segment Presentation
		 * Address */
		iowrite32_ccsr(ep_temp, &wrks_addr->ep_spa);
	}

	if(init_presentation->options & (EP_INIT_PRESENTATION_OPT_SPO |
		EP_INIT_PRESENTATION_OPT_SR |
		EP_INIT_PRESENTATION_OPT_NDS))
	{
		/* read ep_spo - to get Entry Point Segment Presentation
		 * Offsets */
		ep_temp = ioread32_ccsr(&wrks_addr->ep_spo);
		if(init_presentation->options & EP_INIT_PRESENTATION_OPT_SR)
		{
			/* Clear SR field */
			ep_temp &= ~SR_MASK;
		}
		if(init_presentation->options & EP_INIT_PRESENTATION_OPT_NDS)
		{
			/* Clear NDS field */
			ep_temp &= ~NDS_MASK;
		}
		if(init_presentation->options & EP_INIT_PRESENTATION_OPT_SPO)
		{
			/* Clear SPO field */
			ep_temp &= ~SPO_MASK;
		}
		ep_temp |= ep_spo;
		/* write ep_spo - to set Entry Point Segment Presentation
		 * Offset */
		iowrite32_ccsr(ep_temp, &wrks_addr->ep_spo);
	}
	/*Mutex unlock EPID table*/
	cdma_mutex_lock_release((uint64_t)&wrks_addr->epas);
	return 0;
}

static int cmdif_epid_setup(struct aiop_ws_regs *wrks_addr,
                            uint32_t epid,
                            void (*isr_cb)(void))
{
	uint32_t data = 0;
	int      err = 0;

	iowrite32_ccsr(epid, &wrks_addr->epas); /* EPID = 2 */
	iowrite32_ccsr(PTR_TO_UINT(isr_cb), &wrks_addr->ep_pc);

	/* no PTA presentation is required (even if there is a PTA)*/
	iowrite32_ccsr(0x0000ffc0, &wrks_addr->ep_ptapa);
	/* set epid ASA presentation size to 0 */
	iowrite32_ccsr(0x00000000, &wrks_addr->ep_asapa);
	/* Set mask for hash to 16 low bits OSRM = 5 */
	iowrite32_ccsr(0x11000005, &wrks_addr->ep_osc);
	data = ioread32_ccsr(&wrks_addr->ep_osc);
	if (data != 0x11000005)
		err |= -EINVAL;

	pr_info("CMDIF is setting EPID = %d\n", epid);
	pr_info("ep_pc = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_spo));
	pr_info("ep_osc = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_osc));

	if (err) {
		pr_err("Failed to setup EPID %d\n", epid);
		/* No return err here in order to setup the rest of EPIDs */
	}
	return err;
}

int ep_mng_init(void)
{
	int i = 0;
	int err = 0;
	struct aiop_tile_regs *tile_regs = (struct aiop_tile_regs *)
			sys_get_handle(FSL_OS_MOD_AIOP_TILE, 1);
	struct aiop_ws_regs *wrks_addr = &tile_regs->ws_regs;

	/* CMDIF server epid initialization here*/
	err |= cmdif_epid_setup(wrks_addr, AIOP_EPID_CMDIF_SERVER, cmdif_srv_isr);

	/* TMAN epid initialization */
	iowrite32_ccsr(AIOP_EPID_TIMER_EVENT_IDX, &wrks_addr->epas); /* EPID = 1 */
	iowrite32_ccsr(PTR_TO_UINT(tman_timer_callback), &wrks_addr->ep_pc);
	iowrite32_ccsr(0x02000000, &wrks_addr->ep_spo); /* SET NDS bit */

	pr_info("TMAN is setting EPID = %d\n", AIOP_EPID_TIMER_EVENT_IDX);
	pr_info("ep_pc = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x\n", ioread32_ccsr(&wrks_addr->ep_spo));


	/* CMDIF interface client epid initialization here*/
	err |= cmdif_epid_setup(wrks_addr, AIOP_EPID_CMDIF_CLIENT, cmdif_cl_isr);

	/* Initialize EPID-table with discard_rx_cb for all NI's entries (EP_PC field) */
	for (i = AIOP_EPID_DPNI_START; i < AIOP_EPID_TABLE_SIZE; i++) {
		/* Prepare to write to entry i in EPID table - EPAS reg */
		iowrite32_ccsr((uint32_t)i, &wrks_addr->epas);

		iowrite32_ccsr(PTR_TO_UINT(discard_rx_cb), &wrks_addr->ep_pc);
	}

	return err;
}

void ep_mng_free(void)
{
}
