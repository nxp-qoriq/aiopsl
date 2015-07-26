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

/**************************************************************************//*
 @File          slab_stub.h

 @Description   To Be Removed!!!

 @Cautions      This file is private for AIOP.
*//***************************************************************************/
#ifndef __SLAB_STUB_H
#define __SLAB_STUB_H

#include "fsl_types.h"
#include "fsl_errors.h"
#include "dplib/fsl_ldpaa.h"
#include "platform.h"
#include "lib/fsl_slab.h"

#define MEM_PART_1ST_DDR_NON_CACHEABLE 1


/**************************************************************************//**
 @Function      slab_find_and_reserve_bpid

 @Description   Finds and reserve buffer pool with new buffers

                This function is part of SLAB module therefore it should be called only after
                it has been initialized by slab_module_init()

 @Param[in]     num_buffs           Number of buffers in new pool.
 @Param[in]     buff_size           Size of buffers in pool.
 @Param[in]     alignment           Requested alignment for data field (in bytes).
                                    AIOP: HW pool supports up to 8 bytes alignment.
 @Param[in]     mem_partition_id    Memory partition ID for allocation.
                                    AIOP: HW pool supports only PEB and DPAA DDR.
 @Param[out]    num_filled_buffs    Number of buffers that we succeeded to fill.
 @Param[out]    bpid                Id if the buffer that was filled with new buffers.

 @Return        0       - on success,
               -ENAVAIL - could not release into bpid
               -ENOMEM  - not enough memory for mem_partition_id
 *//***************************************************************************/

int slab_find_and_reserve_bpid(
			uint32_t num_buffs,
			uint16_t buff_size,
			uint16_t alignment,
			uint8_t  mem_partition_id,
			int      *num_filled_buffs,
			uint16_t *bpid);



/**************************************************************************//**
@Function	slab_register_context_buffer_requirements

@Description	register a request for buffers requirement.

@Param[in]	committed_buffs     Number of buffers needed for the app.
@Param[in]	max_buffs           Maximal number of buffers that
		can be allocated by the app; max_buffs >= committed_buffs;
@Param[in]	buff_size           Size of buffers in pool.
@Param[in]	alignment           Requested alignment for data in bytes.
		AIOP: HW pool supports up to 8 bytes alignment.
@Param[in]	mem_pid             Memory partition ID for allocation.
		AIOP: HW pool supports only PEB and DPAA DDR.
@Param[in]	flags               Set it to 0 for default.
@Param[in]	num_ddr_pools       Number of pools needed in the future 
		(managed in ddr - slow performance).
@Cautions       Max buffer size supported - 32760 Byte (32760 + 8 Meta data = 
		32768), Max alignment supported 32768 Byte.
		Alignment <= Buffer size + Meta data.
@Return		0        - on success,
		-ENAVAIL - resource not available or not found,
		-ENOMEM  - not enough memory for requested memory partition
 *//***************************************************************************/
/*
int slab_register_context_buffer_requirements(uint32_t    committed_buffs,
                                              uint32_t    max_buffs,
                                             uint16_t    buff_size,
                                              uint16_t    alignment,
                                              //enum memory_partition_id  mem_pid,
                                              enum memory_partition_id_stub mem_pid,
                                              uint32_t    flags,
                                              uint32_t    num_ddr_pools);
*/

#endif /* __SLAB_STUB_H */
