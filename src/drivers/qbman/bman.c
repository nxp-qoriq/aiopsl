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


#include "fsl_bman.h"
#include "fsl_icontext.h"
#include "fsl_io.h"
#include "fsl_malloc.h"
#include "fsl_fdma.h"
#include "fsl_dbg.h"
#include "fsl_sl_slab.h"

/*****************************************************************************/
__COLD_CODE int bman_fill_bpid(uint32_t num_buffs,
                     uint16_t buff_size,
                     uint16_t alignment,
                     enum memory_partition_id  mem_partition_id,
                     uint16_t bpid,
                     uint16_t alignment_extension)
{
	int        i = 0;
	uint64_t addr  = 0;
	struct icontext ic;
	int err;

	/* We allocate a continuous block of memory,
	 * so alignment must be the minimum buffer size */
	if (buff_size < alignment)
	{
		buff_size = alignment;
	}

	switch(mem_partition_id){
	case MEM_PART_DP_DDR:
	case MEM_PART_SYSTEM_DDR:
	case MEM_PART_PEB:
		err = fsl_get_mem((uint64_t)buff_size * num_buffs +
		                     alignment_extension,
		                     mem_partition_id,
		                     alignment,
		                     &addr);
		if(err)
			return err;
	break;
	default:
		pr_err("Memory partition %d is not supported.\n", mem_partition_id);
		return -EINVAL;
	}


	if(addr == NULL)
		return -ENOMEM;
	/* AIOP ICID and AMQ bits are needed for filling BPID */
	icontext_aiop_get(&ic);


	/*This is to make user data to be align if called from slab,
	* otherwise 0 will be added*/
	addr += alignment_extension;

	for (i = 0; i < num_buffs; i++) {
		fdma_release_buffer(ic.icid, ic.bdi_flags, bpid, addr);
		addr += buff_size;
	}
	return 0;
}
