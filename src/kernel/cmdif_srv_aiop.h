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

#ifndef __CMDIF_SRV_AIOP_H
#define __CMDIF_SRV_AIOP_H

#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "fsl_mc_init.h"
#include "cmdif_srv.h"
#include "fdma.h"

#pragma warning_errors on
ASSERT_STRUCT_SIZE(CMDIF_SESSION_OPEN_SIZEOF, CMDIF_SESSION_OPEN_SIZE);
#pragma warning_errors off

/** Short macro for fqd_ctx, to be used in CMDIF_FQD_GET */
#define CMDIF_FQD_CTX_GET \
	(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fqd_ctx)

/** Get command dequeue context */
#define CMDIF_FQD_GET \
	(uint32_t)(LLLDW_SWAP((uint32_t)&CMDIF_FQD_CTX_GET, 0) & 0xFFFFFFFF)

struct cmdif_srv_aiop {
	struct cmdif_srv *srv;
	/**< Common Server fields */
	struct mc_dpci_obj *dpci_tbl;
	/**< DPCI table according to indexes in dequeue context */
	uint8_t lock;
	/**< cmdif spinlock used for module id allocation */
};

void cmdif_srv_isr(void);
int cmdif_srv_init(void);
void cmdif_srv_free(void);

#endif /* __CMDIF_SRV_H */
