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

#ifndef __HEADER_MANIP_H
#define __HEADER_MANIP_H

#include "fsl_string.h"
#include "fsl_sys.h"
#include "fsl_parser.h"
#include "fsl_cdma.h"
#include "apps.h"

#define IPV4_HDR_LENGTH			20
#define GRE_HDR_LENGTH			4
#define IPV4_MIN_VSN_IHN		0x45
#define GRE_PROTOCOL			0x2f
#define GRE_VERSION			0
#define GRE_PTYPE_IP			0x0800	/* See RFC 1701 for full list of
						 * supported protocol types */
/* Pre-configured tunnel IP source and IP destination addresses */
#define TUNNEL_SRC_IP			0x7a7a7a7a
#define TUNNEL_DST_IP			0x8a8a8a8a

/* Functions prototype */
int app_init(void);
int app_early_init(void);
void app_free(void);

void build_apps_array(struct sys_module_desc *apps);
void app_hmanip(void);

#endif /* __HEADER_MANIP_H */

