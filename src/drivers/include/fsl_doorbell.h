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

/**************************************************************************//**
@File          fsl_doorbell.h

@Description   AIOP Doorbell API

@Cautions      None.
*//***************************************************************************/

#ifndef __FSL_DOORBELL_H
#define __FSL_DOORBELL_H

#include "fsl_types.h"
#include "fsl_errors.h"


/**************************************************************************//**
@Function      bman_fill_bpid

@Description   fill the HW pool with buffers.


@Param[in]     num_buffs         Number of buffers to fill in HW pool.
@Param[in]     buff_size         Size of buffers in pool.
@Param[in]     alignment         Requested alignment for data field (in bytes).
@Param[in]     mem_partition_id  Memory partition ID for buffer type.
				 AIOP: HW pool supports only PEB and DDR.
@Param[in]     bpid              Id of pool to fill with buffers.
@Param[in]     alignment_extension Used to shift first buffer if 8 bytes of HW
				metadata exist and make user field aligned.
				(Default 0 must be entered if not used for slab)


@Return        0       - on success,
               -EINVAL - slab module handle is null
               -ENOMEM - not enough memory for mem_partition_id
 *//***************************************************************************/
int bman_fill_bpid(uint32_t num_buffs,
                     uint16_t buff_size,
                     uint16_t alignment,
                     enum memory_partition_id  mem_partition_id,
                     uint16_t bpid,
                     uint16_t alignment_extension);


#endif /* __FSL_DOORBELL_H */
