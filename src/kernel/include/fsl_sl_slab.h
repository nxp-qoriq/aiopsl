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
@File          fsl_sl_slab.h

@Description   This is slab internal header file which includes all the API
               exposed to AIOPSL.

@Cautions      This file is private for AIOP.
*//***************************************************************************/
#ifndef __FSL_SL_SLAB_H
#define __FSL_SL_SLAB_H

#include "fsl_slab.h"

#define IS_POWER_VALID_ALLIGN(_val, _max_size) \
    (((((uint32_t)_val) <= (_max_size)) && ((((uint32_t)_val) & (~((uint32_t)_val) + 1)) == ((uint32_t)_val))))
/**************************************************************************//**
@Function      slab_module_init

@Description   Initialize SLAB module

		In AIOP during slab_module_init() we will call MC API in order
		to get all BPIDs

@Return        0 on success, error code otherwise.
 *//***************************************************************************/
int slab_module_init(void);

/**************************************************************************//**
@Function      slab_module_free

@Description   Frees SLAB module

		In addition to memory de-allocation it will return BPIDs to MC

@Return        None
 *//***************************************************************************/
void slab_module_free(void);

/**************************************************************************//**
@Function      slab_find_and_reserve_bpid

@Description   Finds and reserve buffers from buffer pool.

		This function is part of SLAB module therefore it should be
		called only after it has been initialized by slab_module_init()

@Param[in]     num_buffs         Number of buffers in new pool.
@Param[in]     buff_size         Size of buffers in pool.
@Param[in]     alignment         Requested alignment for data field (in bytes).
@Param[in]     mem_partition_id  Memory partition ID for buffer type.
				 AIOP: HW pool supports only PEB and DDR.
@Param[out]    bpid_array_index  Index for bman pool array which reserved the
				 buffers.
@Param[out]    bpid              Id of pool that supply the requested buffers.

@Return        0       - on success,
	       -EINVAL - slab module handle is null
	       -ENOMEM - not enough memory for mem_partition_id
 *//***************************************************************************/
int slab_find_and_reserve_bpid(uint32_t num_buffs,
			uint16_t buff_size,
			uint16_t alignment,
			enum memory_partition_id  mem_partition_id,
			uint16_t *bpid_array_index,
			uint16_t *bpid);

/**************************************************************************//**
@Function      slab_find_and_unreserve_bpid

@Description   Finds and unreserved buffers in buffer pool.

		This function is part of SLAB module therefore it should be
		called only after it has been initialized by slab_module_init()
		the function is for service layer to return buffers to bman pool.

@Param[in]    num_buffs        Number of buffers in new pool.
@Param[in]    bpid              Id of pool that was filled with new buffers.

@Return        0       - on success,
	       -ENAVAIL - bman pool not found
 *//***************************************************************************/
int slab_find_and_unreserve_bpid(int32_t num_buffs,
                            uint16_t bpid);

#endif /* __FSL_SL_SLAB_H */
