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

#ifndef __ICONTEXT_H
#define __ICONTEXT_H

#if 0

#define IC_MAX_NUM_SWC	64 /**< Maximal number of SW contextes */

/** BDI */
#define IC_BDI_GET \
((((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fdsrc_va_fca_bdi) \
	& ADC_BDI_MASK)
/** eVA is OR between FD[VA]  ADC[VA config] */
#define IC_VA_GET \
(((((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fdsrc_va_fca_bdi) \
	& ADC_VA_MASK) || LDPAA_FD_GET_VA(HWC_FD_ADDRESS))

/** BMT for memory accesses */
#define IC_BMT_GET \
	LDPAA_FD_GET_CBMT(HWC_FD_ADDRESS)

/** PL_ICID from Additional Dequeue Context */
#define IC_PL_ICID_GET \
	LH_SWAP(0, &(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->pl_icid))

/** Get ICID to send response */
#define IC_ICID_GET(PL_AND_ICID) ((PL_AND_ICID) & ADC_ICID_MASK)

/** Get PL to send response */
#define IC_PL_GET(PL_AND_ICID) ((PL_AND_ICID)  & ADC_PL_MASK)

#define IC_ADD_AMQ_FLAGS(FL, PL_AND_ICID)		\
	do {					\
		if (PL_GET(PL_AND_ICID))	\
			FL |= FDMA_DMA_PL_BIT;	\
		if (VA_GET)			\
			FL |= FDMA_DMA_VA_BIT;	\
		if (BMT_GET)			\
			FL |= FDMA_DMA_BMT_BIT;	\
	}while(0)

struct ic {
	uint32_t data_flags; /**< FDMA dma data flags */
	uint32_t enq_flags;  /**< FDMA enqueue flags */
	uint16_t icid;       /**< ICID of SWC */
	uint8_t	 vld;        /**< Valid bit per ICID */
};

struct ic_table {
	struct ic icontext[IC_MAX_NUM_SWC]; /**< Isolation context */
	uint8_t	 lock;
	int    	 count;
};
#endif

#endif
