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

#ifndef __CMDIF_H
#define __CMDIF_H

#include <errno.h>
#include <nadk_types.h>
#include <string.h>
#include <stdlib.h>
#include <nadk_types.h>
#include <nadk_byteorder.h>
#include <fsl_cmdif_fd.h>

#define CPU_TO_SRV16(val) nadk_bswap16(val)
#define CPU_TO_SRV32(val) nadk_bswap32(val)
#define CPU_TO_BE64(val)  nadk_bswap64(val)
#define CPU_TO_BE16(val)  nadk_bswap16(val)

#define CMDIF_EPID         0     
/*!< EPID to be used for setting by client */

#ifdef NADK_DEBUG
#ifndef DEBUG
#define DEBUG
#endif
#endif /* NADK_DEBUG */

#ifndef __HOT_CODE
#define __HOT_CODE
#endif

#ifndef __COLD_CODE
#define __COLD_CODE
#endif /* COLD_CODE*/

#ifndef CPU_TO_LE64
#define CPU_TO_LE64(val) (val)
#endif
#ifndef CPU_TO_LE32
#define CPU_TO_LE32(val) (val)
#endif

#define SHBP_BUF_TO_PTR(BUF) (BUF)
#define SHBP_PTR_TO_BUF(BUF) (BUF)

int send_fd(struct cmdif_fd *cfd, int pr, void *nadk_dev);
int receive_fd(struct cmdif_fd *cfd, int pr, void *nadk_dev);

#endif /* __CMDIF_H */
