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

#include <fsl_shbp_flib.h>
#include <fsl_shbp_host.h>

#ifndef IS_ALIGNED
#define IS_ALIGNED(ADDRESS, ALIGNMENT)              \
        (!(((uint32_t)(ADDRESS)) & (((uint32_t)(ALIGNMENT)) - 1)))
#endif /*!< check only 4 last bytes */

#define COPY_AND_SWAP(LOCAL, SHBP) \
	do {\
		(LOCAL) = *(SHBP);\
		(LOCAL).alloc.base = CPU_TO_LE64((LOCAL).alloc.base);\
		(LOCAL).alloc.enq = CPU_TO_LE32((LOCAL).alloc.enq);\
		(LOCAL).alloc.deq = CPU_TO_LE32((LOCAL).alloc.deq);\
		(LOCAL).free.base = CPU_TO_LE64((LOCAL).free.base);\
		(LOCAL).free.enq = CPU_TO_LE32((LOCAL).free.enq);\
		(LOCAL).free.deq = CPU_TO_LE32((LOCAL).free.deq);\
	} while(0)
/*!< It is more efficient to copy to cached stack and then process */

uint32_t shbp_flib_mem_ptr_size(uint32_t num_bufs)
{
	return SHBP_TOTAL_BYTES + (16 * (num_bufs));
}

static uint8_t get_num_of_first_bit(uint32_t num)
{
	int i;
	for (i = 0; i < 32; i++) {
		if (num & (0x80000000 >> i))
			return (uint8_t)(31 - i);
	}
	return 0xff;
}

static void *acquire(struct shbp *bp, struct shbp_q *lq, struct shbp_q *q)
{
	uint32_t deq = SHBP_BD_IND(bp, lq->deq); /* mod 2^x */
	uint64_t buf = (((uint64_t *)SHBP_BUF_TO_PTR(lq->base))[deq]);
	
	lq->deq++;
	q->deq = CPU_TO_LE32(lq->deq); /* Must be last */

	return SHBP_BUF_TO_PTR(CPU_TO_LE64(buf));
}

static void release(struct shbp *bp, struct shbp_q *lq, 
                    void *buf, struct shbp_q *q)
{
	uint32_t enq = SHBP_BD_IND(bp, lq->enq); /* mod 2^x */
	((uint64_t *)SHBP_BUF_TO_PTR(lq->base))[enq] = \
		CPU_TO_LE64(SHBP_PTR_TO_BUF(buf));
	
	lq->enq++;
	q->enq = CPU_TO_LE32(lq->enq); /* Must be last */
}

int shbp_flib_create(void *mem_ptr, uint32_t size, uint32_t flags, struct shbp **_bp)
{
	struct shbp *bp;
	uint32_t ring_size;
	
#ifdef DEBUG
	if ((mem_ptr == NULL) || (size == 0))
		return -EINVAL;
#endif	
	/* Better to have mem_ptr aligned to cache line */
	if (!IS_ALIGNED(mem_ptr, 64))
		return -EINVAL;
	
	bp = (struct shbp *)mem_ptr;
	memset(bp, 0, size);

	/* 8 bytes for each BD, 2 rings = 2 ^ 4 
	 * bp->size is size = 8 * bp->size because 8 BDs = cache line 
	 * size = 2^bp->size */
	ring_size = (size - SHBP_TOTAL_BYTES);
	ring_size = ring_size >> 4;
	/* Minimum 8 BDs = 64 bytes */
	if (ring_size < 8)
		return -EINVAL;
	
	bp->max_num = get_num_of_first_bit(ring_size);
	
	bp->alloc_master = (uint8_t)(flags & SHBP_GPP_MASTER);
		
	bp->alloc.base = (uint64_t)(((uint8_t *)bp) + SHBP_TOTAL_BYTES);
	/* Each BD is 8 bytes */
	bp->free.base  = (uint64_t)(((uint8_t *)bp->alloc.base) + 
		SHBP_SIZE_BYTES(bp));
		
	bp->alloc.base = SHBP_PTR_TO_BUF((uint8_t *)bp->alloc.base);
	bp->free.base  = SHBP_PTR_TO_BUF((uint8_t *)bp->free.base);
	bp->alloc.base = CPU_TO_LE64(bp->alloc.base);
	bp->free.base  = CPU_TO_LE64(bp->free.base);
	
#ifdef DEBUG
	if ((bp->alloc.base == 0) || (bp->free.base == 0))
		return -EINVAL;
#endif
	
	*_bp = bp;
	
	return 0;
}
		
void *shbp_flib_acquire(struct shbp *bp)
{
	void *buf;
	struct shbp lbp;
	
#ifdef DEBUG
	if (bp == NULL)
		return NULL;
#endif
	
	COPY_AND_SWAP(lbp, bp);
	
	if (!(lbp.alloc_master & SHBP_GPP_MASTER))
		return NULL;
	
	if (SHBP_ALLOC_IS_EMPTY(&lbp))
		return NULL;
	
	buf = acquire(&lbp, &lbp.alloc, &bp->alloc);
	
	return buf;
}

int shbp_flib_release(struct shbp *bp, void *buf)
{
	struct shbp lbp;

#ifdef DEBUG
	if ((buf == NULL) || (bp == NULL))
		return -EINVAL;
#endif
	
	COPY_AND_SWAP(lbp, bp);

	if (SHBP_ALLOC_IS_FULL(&lbp))
		return -ENOSPC;
	
	release(&lbp, &lbp.alloc, buf, &bp->alloc);	
	
	return 0;
}

int shbp_flib_refill(struct shbp *bp)
{
	void *buf;
	int count = 0;
	struct shbp lbp;

#ifdef DEBUG
	if (bp == NULL)
		return -EINVAL;
#endif
	COPY_AND_SWAP(lbp, bp);

	while(!SHBP_FREE_IS_EMPTY(&lbp) && !SHBP_ALLOC_IS_FULL(&lbp)) {
		buf = acquire(&lbp, &lbp.free, &bp->free);
#ifdef DEBUG
		if (buf == 0)
			return -EACCES;
#endif
		release(&lbp, &lbp.alloc, buf, &bp->alloc);
		count++;
	}
	return count;
}

int shbp_flib_destroy(struct shbp *bp, void **ptr)
{
	struct shbp lbp;

#ifdef DEBUG
	if ((bp == NULL) || (ptr == NULL))
		return -EINVAL;
#endif
	
	COPY_AND_SWAP(lbp, bp);

	/* take all from free */
	if (!SHBP_FREE_IS_EMPTY(&lbp)) {
		*ptr = acquire(&lbp, &lbp.free, &bp->free);
		return -EACCES;
	}	
	
	/* take all from alloc */
	if (!SHBP_ALLOC_IS_EMPTY(&lbp)) {
		*ptr = acquire(&lbp, &lbp.alloc, &bp->alloc);
		return -EACCES;
	}
	
	*ptr = NULL;
	return 0;
}
