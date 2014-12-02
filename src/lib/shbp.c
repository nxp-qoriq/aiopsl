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

#include <fsl_shbp.h>

#ifndef MODULU_POWER_OF_TWO
#define MODULU_POWER_OF_TWO(NUM, MOD) \
	((uint32_t)(NUM) & ((uint32_t)(MOD) - 1))
#endif

struct shbp *shbp_create(void *mem_ptr, uint32_t size, 
                         uint32_t buf_size, uint32_t flags)
{
#ifdef DEBUG
	if ((mem_ptr == NULL) || (size == 0) || (buf_size == 0) || (flags != 0))
		return NULL;
#endif
	
	return NULL;
}

void *shbp_acquire(struct shbp *bp)
{
	uint32_t deq;
	void *buf;
	
	if (SHBP_ALLOC_IS_EMPTY(bp))
		return NULL;
	
	deq = MODULU_POWER_OF_TWO(bp->alloc.deq, (1 << bp->size));
	buf = (void *)((uint64_t *)bp->alloc.base)[deq];
	bp->alloc.deq++;
	
	return buf;
}

int shbp_release(struct shbp *bp, void *buf)
{
	uint32_t enq;
	
#ifdef DEBUG
	if (buf == NULL)
		return -EINVAL;
#endif
	
	if (SHBP_ALLOC_IS_FULL(bp))
		return -ENOSPC;
	
	enq = MODULU_POWER_OF_TWO(bp->alloc.enq, (1 << bp->size));
	((uint64_t *)bp->alloc.base)[enq] = (uint64_t)buf;
	bp->alloc.enq++;
	
	return 0;
}


int shbp_refill(struct shbp *bp)
{
#ifdef DEBUG
	if (bp == NULL)
		return -EINVAL;
#endif
	return 0;
}
