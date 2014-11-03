/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
@File		slab_stub.c

@Description	To Be Removed !!!

*//***************************************************************************/
#include "slab_stub.h"
#include "system.h"
#include "aiop_verification_data.h"


extern __VERIF_TLS uint8_t slab_error;

int slab_find_and_reserve_bpid(uint32_t num_buffs,
                            uint16_t buff_size,
                            uint16_t alignment,
                            uint8_t  mem_partition_id,
                            int      *num_filled_buffs,
                            uint16_t *bpid)
{
	/* Assuming fixed value for IPR is 1000 buffers. */
	UNUSED(alignment);
	UNUSED(mem_partition_id);

	if ((buff_size == (SYS_NUM_OF_PRPIDS+3)) || (buff_size == 2688))
		*bpid = 1;
	else
		*bpid = 2;

	*num_filled_buffs = (int)num_buffs;

	return 0;
}

