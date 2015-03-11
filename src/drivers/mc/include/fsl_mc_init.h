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

#ifndef __FSL_MC_INIT_H
#define __FSL_MC_INIT_H

#include "fsl_dpci.h"
#include "dplib/fsl_mc_sys.h"

struct mc_dprc {
	uint16_t		token;
	struct fsl_mc_io	io;
};

struct mc_dpci_obj {
	struct dpci_attr	*attr;
	struct dpci_peer_attr	*peer_attr;
	struct dpci_rx_queue_attr *rx_queue_attr[DPCI_PRIO_NUM];
	struct dpci_tx_queue_attr *tx_queue_attr[DPCI_PRIO_NUM];
	uint16_t		*token;
	uint16_t		*icid;		/**< ICID per DPCI */
	uint32_t		*dma_flags;	/**< FDMA dma data flags */
	uint32_t		*bdi_flags;	/**< FDMA enqueue flags */
	int    			count;
};

struct mc_dpci_tbl {
	uint32_t *ic;		/**< 0xFFFFFFFF is not valid, must be atomic*/
	uint32_t *dpci_id;	/**< dpci ids not tokens */
	int count;
	int max;
	uint8_t lock;
};

/*
 * Returns dpci index on success or error otherwise 
 * Once dpci id is added to the table it can't be remove but only invalidated
 */
int mc_dpci_find(uint32_t dpci_id, uint32_t *ic);

#endif /*__FSL_MC_INIT_H */
