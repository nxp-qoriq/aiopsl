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

#ifndef __CMDIF_H
#define __CMDIF_H

#include <fsl_errors.h>
#include <fsl_types.h>
#include <fsl_string.h>
#include <fsl_stdlib.h>
#include <fsl_endian.h>

#ifndef CORE_IS_BIG_ENDIAN
#error "AIOP core must be big endian\n!"
#endif

//#define CMDIF_TEST_WITH_MC_SRV

#ifdef CMDIF_TEST_WITH_MC_SRV
#define CPU_TO_SRV16(val) CPU_TO_BE16(val)
#define CPU_TO_SRV32(val) CPU_TO_BE32(val)
#define CPU_TO_SRV64(val) CPU_TO_BE64(val)
#else
#define CPU_TO_SRV16(val) CPU_TO_LE16(val)
#define CPU_TO_SRV32(val) CPU_TO_LE32(val)
#define CPU_TO_SRV64(val) CPU_TO_LE64(val)
#endif

#define CMDIF_EPID         2    /*!< EPID to be used for setting by client */

#endif /* __CMDIF_H */
