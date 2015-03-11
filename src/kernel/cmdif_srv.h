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

#ifndef __CMDIF_SRV_H
#define __CMDIF_SRV_H

#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "fsl_mc_init.h"
#include "cmdif_srv_flib.h"
#include "fsl_fdma.h"
#include "fsl_mc_cmd.h"

#pragma warning_errors on
ASSERT_STRUCT_SIZE(CMDIF_SESSION_OPEN_SIZEOF, CMDIF_SESSION_OPEN_SIZE);
#pragma warning_errors off

/** Short macro for fqd_ctx, to be used in CMDIF_FQD_GET */
#define CMDIF_FQD_CTX_GET \
	(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fqd_ctx)

/** Get command dequeue context */
#define CMDIF_FQD_GET \
	(uint32_t)(LLLDW_SWAP((uint32_t)&CMDIF_FQD_CTX_GET, 0) & 0xFFFFFFFF)

#define DPCI_LOW_PR	1
#define CMDIF_BDI_BIT	((uint16_t)0x1)
#define CMDIF_Q_OPTIONS (DPCI_QUEUE_OPT_USER_CTX | DPCI_QUEUE_OPT_DEST)
#define CMDIF_RX_CTX_GET \
	(LLLDW_SWAP((uint32_t)&CMDIF_FQD_CTX_GET, 0))

#define AMQ_BDI_SET(_offset, _width, _type, _arg) \
	(amq_bdi |= u32_enc((_offset), (_width), (_arg)))

#define AMQ_BDI_GET(_offset, _width, _type, _arg) \
	(*(_arg) = (_type)u32_dec(dt->ic[ind], (_offset), (_width)))

#define USER_CTX_SET(_offset, _width, _type, _arg) \
	(queue_cfg.user_ctx |= u64_enc((_offset), (_width), (_arg)))

#define USER_CTX_GET(_offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(rx_ctx, (_offset), (_width)))

#define CMDIF_DPCI_FQID(_OP, DPCI, FQID) \
do { \
	_OP(32,	32,	uint32_t,	DPCI); \
	_OP(0,	32,	uint32_t,	FQID); \
} while (0)


#define CMDIF_ICID_AMQ_BDI(_OP, ICID, AMQ_BDI) \
do { \
	_OP(16,	16,	uint16_t,	ICID); \
	_OP(0,	16,	uint16_t,	AMQ_BDI); \
} while (0)

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
