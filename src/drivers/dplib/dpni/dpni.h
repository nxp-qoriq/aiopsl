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

/**************************************************************************//*
 @File          dpni.h

 @Description   DPNI FLib internal file

 @Cautions      None.
 *//***************************************************************************/

#ifndef _DPNI_H
#define _DPNI_H

#include "fsl_endian.h"
#include "common/fsl_cmdif.h"
#include "fsl_dpni.h"

struct cmdif_cmd_data {
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};

static __inline__ uint64_t u64_read_field(uint64_t reg, int start_bit, int size)
{
	if (size >= 64)
		return reg;
	return (reg >> start_bit) & ((0x0000000000000001LL << size) - 1);
}

static __inline__ uint64_t u64_write_field(uint64_t reg,
                                           int start_bit,
                                           int size,
                                           uint64_t val)
{
	if (size >= 64)
		reg = val;
	else
		reg |= (uint64_t)(
		        (val & ((0x0000000000000001LL << size) - 1))
		        << start_bit);
	return reg;
}

#define GPP_CMD_READ_PARAM(_ptr, _id) \
	swap_uint64(((struct cmdif_cmd_data *)(_ptr))->param##_id)

#define GPP_CMD_WRITE_PARAM(_ptr, _id, _val) \
	((_ptr)->param##_id = swap_uint64(_val))

#endif /* _DPNI_H */
