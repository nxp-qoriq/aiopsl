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

/**************************************************************************//**
@File          fsl_doorbell.h

@Description   AIOP Doorbell API

@Cautions      None.
*//***************************************************************************/

#ifndef __FSL_DOORBELL_H
#define __FSL_DOORBELL_H

#include "fsl_types.h"
#include "fsl_errors.h"
#include "fsl_icontext.h"

enum doorbell_reg {
	DOORBELL_SRC_GENERAL,
	DOORBELL_SRC_MANAGEMENT,
	DOORBELL_SRC_LAST
};

void doorbell_status(int pr, enum doorbell_reg g_m, uint32_t *status);
void doorbell_clear(int priority, enum doorbell_reg g_m, uint32_t mask);
void doorbell_ring(int priority, enum doorbell_reg g_m, uint32_t mask);
int doorbell_setup(int priority, enum doorbell_reg g_m, uint16_t epid,
                   void (*isr_cb)(void), uint32_t scope_id);

#endif /* __FSL_DOORBELL_H */
