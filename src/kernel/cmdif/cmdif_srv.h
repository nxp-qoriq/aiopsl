/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CMDIF_SRV_H
#define __CMDIF_SRV_H

#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "fsl_sl_dprc_drv.h"
#include "cmdif_srv_flib.h"
#include "fsl_fdma.h"
#include "fsl_mc_cmd.h"
#include "fsl_dpci_mng.h"

#pragma warning_errors on
ASSERT_STRUCT_SIZE(CMDIF_SESSION_OPEN_SIZEOF, CMDIF_SESSION_OPEN_SIZE);
#pragma warning_errors off


#define CMDIF_BDI_BIT	((uint16_t)0x1)

struct cmdif_srv_aiop {
	struct cmdif_srv *srv;
	/**< Common Server fields */
};

void cmdif_srv_isr(void);
int cmdif_srv_init(void);
void cmdif_srv_free(void);

#endif /* __CMDIF_SRV_H */
