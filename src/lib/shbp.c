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


#ifndef IS_ALIGNED
#define IS_ALIGNED(ADDRESS, ALIGNMENT)              \
        (!(((uint32_t)(ADDRESS)) & (((uint32_t)(ALIGNMENT)) - 1)))
#endif /*!< check only 4 last bytes */

#ifndef ALIGN_UP64
#define ALIGN_UP64(ADDRESS, ALIGNMENT)           \
        ((((uint64_t)(ADDRESS)) + ((uint64_t)(ALIGNMENT)) - 1) & (~(((uint64_t)(ALIGNMENT)) - 1)))
        /**< Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
#endif /* ALIGN_UP */

static uint8_t get_num_of_first_bit(uint32_t num)
{
	int i;
	for (i = 0; i < 32; i++) {
		if (num & (0x80000000 >> i))
			return (uint8_t)(31 - i);
	}
	return 0xff;
}

struct shbp *shbp_create(void *mem_ptr, uint32_t size, uint32_t flags)
{
	struct shbp *bp;
	uint32_t ring_size;
#ifdef DEBUG
	if ((mem_ptr == NULL) || (size == 0) || (flags != 0))
		return NULL;
	/* Better to have mem_ptr aligned to cache line */
	if (!IS_ALIGNED(mem_ptr, 64))
		return NULL;
#endif
	
	bp = (struct shbp *)ALIGN_UP64(mem_ptr, 64);
	size -= (uint32_t)((uint8_t *)bp - (uint8_t *)mem_ptr);
	memset(bp, 0, size);

	/* 8 bytes for each BD, 2 rings = 2 ^ 4 
	 * bp->size is size = 8 * bp->size because 8 BDs = cache line 
	 * size = 2^bp->size */
	ring_size = (size - sizeof(struct shbp));
	ring_size = ring_size >> 4;
	/* Minimum 8 BDs = 64 bytes */
	if (ring_size < 8)
		return NULL;
	bp->size = get_num_of_first_bit(ring_size);
	
	bp->alloc_master = (uint8_t)(flags & SHBP_GPP_MASTER);
		
	bp->alloc.base = (uint64_t)(((uint8_t *)bp) + sizeof(struct shbp));
	/* Each BD is 8 bytes */
	bp->free.base  = (uint64_t)(((uint8_t *)bp->alloc.base) + 
		SHBP_SIZE_BYTES(bp));
	
	return bp;
}

void *shbp_acquire(struct shbp *bp)
{
	uint32_t deq;
	void *buf;
	
#ifdef DEBUG
	if (bp == NULL)
		return NULL;
#endif
	if (!(bp->alloc_master & SHBP_GPP_MASTER))
		return NULL;
	
	if (SHBP_ALLOC_IS_EMPTY(bp))
		return NULL;
	
	deq = bp->alloc.deq % SHBP_SIZE(bp); /* mod 2^x */
	buf = (void *)SHBP_ALLOC_BD(bp ,deq);
	bp->alloc.deq++;
	
	return buf;
}

int shbp_release(struct shbp *bp, void *buf)
{
	uint32_t enq;
	
#ifdef DEBUG
	if ((buf == NULL) || (bp == NULL))
		return -EINVAL;
#endif
	
	if (SHBP_ALLOC_IS_FULL(bp))
		return -ENOSPC;
	
	enq = bp->alloc.enq % SHBP_SIZE(bp); /* mod 2^x */
	SHBP_ALLOC_BD(bp ,enq) = (uint64_t)buf;
	bp->alloc.enq++;
	
	return 0;
}


int shbp_refill(struct shbp *bp)
{
	uint32_t deq;
	void *buf;
#ifdef DEBUG
	if (bp == NULL)
		return -EINVAL;
#endif
	while(!SHBP_FREE_IS_EMPTY(bp)) {
		deq = bp->free.deq % SHBP_SIZE(bp);
		buf = (void *)SHBP_FREE_BD(bp ,deq);
		bp->free.deq++;
		shbp_release(bp, buf);
	}
	return 0;
}
