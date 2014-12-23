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

#include <fsl_shbp_aiop.h>
#include <fsl_icontext.h>
#include <fsl_cdma.h>
#include <fsl_spinlock.h>
#include <fsl_dbg.h>
/*
 * SHBP is assumed to be implemented in little endian that's why AIOP does 
 * swaps
 */
#define DUMP_SHBP() \
	do {\
		pr_debug("shbp.alloc_master = 0x%x\n", shbp.alloc_master); \
		pr_debug("shbp.max_num = 0x%x size = %d\n", shbp.max_num, SHBP_SIZE(&shbp)); \
		pr_debug("shbp.alloc.base high = 0x%x\n", (uint32_t)((shbp.alloc.base & 0xFFFFFFFF00000000) >> 32)); \
		pr_debug("shbp.alloc.base low = 0x%x\n", (uint32_t)(shbp.alloc.base & 0xFFFFFFFF)); \
		pr_debug("shbp.alloc.deq = 0x%x\n", shbp.alloc.deq); \
		pr_debug("shbp.alloc.enq = 0x%x\n", shbp.alloc.enq); \
		pr_debug("shbp.free.base high = 0x%x\n", (uint32_t)((shbp.free.base & 0xFFFFFFFF00000000) >> 32)); \
		pr_debug("shbp.free.base low = 0x%x\n", (uint32_t)(shbp.free.base & 0xFFFFFFFF)); \
		pr_debug("shbp.free.deq = 0x%x\n", shbp.free.deq); \
		pr_debug("shbp.free.enq = 0x%x\n\n", shbp.free.enq); \
	} while(0)

#define DUMP_AIOP_BP() \
	do {\
		pr_debug("bp->shbp high = 0x%x\n", (uint32_t)((bp->shbp & 0xFFFFFFFF00000000) >> 32)); \
		pr_debug("bp->shbp low = 0x%x\n", (uint32_t)(bp->shbp & 0xFFFFFFFF)); \
		pr_debug("bp->ic.dma_flags = 0x%x\n", bp->ic.dma_flags); \
		pr_debug("bp->ic.bdi_flags = 0x%x\n", bp->ic.bdi_flags); \
		pr_debug("bp->ic.icid = 0x%x\n", bp->ic.icid); \
	}while(0)


int shbp_read(struct shbp_aiop *bp, uint16_t size, uint64_t src, void *dest)
{
#ifdef DEBUG
	if (bp == NULL)
		return -EINVAL;
#endif	
	return icontext_dma_read(&bp->ic, size, src, dest);
}

int shbp_write(struct shbp_aiop *bp, uint16_t size, void *src, uint64_t dest)
{
#ifdef DEBUG
	if (bp == NULL)
		return -EINVAL;
#endif
	return icontext_dma_write(&bp->ic, size, src, dest);
}

uint64_t shbp_acquire(struct shbp_aiop *bp)
{
	struct shbp shbp;
	uint32_t offset;
	uint64_t buf;
	
	DUMP_AIOP_BP();
	
	cdma_mutex_lock_take(bp->shbp, CDMA_MUTEX_WRITE_LOCK);
	
	/* Read SHBP structure:
	 *  */
	icontext_dma_read(&bp->ic, sizeof(struct shbp), bp->shbp, &shbp);
	if (shbp.alloc_master) {
		/* Pool does not belong to AIOP */
		cdma_mutex_lock_release(bp->shbp);
		return NULL;
	}
		
	shbp.alloc.base = CPU_TO_LE64(shbp.alloc.base); 
	shbp.alloc.deq  = CPU_TO_LE32(shbp.alloc.deq);
	shbp.alloc.enq  = CPU_TO_LE32(shbp.alloc.enq);
	
	DUMP_SHBP();

	if (SHBP_ALLOC_IS_EMPTY(&shbp)) {
		cdma_mutex_lock_release(bp->shbp);
		return NULL;
	}
	
	/* Read the buffer:
	 * offset in byte = index of BD * 8 which is index << 3 */
	buf = NULL;
	offset = SHBP_BD_OFF(&shbp, shbp.alloc.deq);
	icontext_dma_read(&bp->ic, sizeof(uint64_t), 
	                  shbp.alloc.base + offset, &buf);
	buf = CPU_TO_LE64(buf);
	
	/* Increase the dequeue:
	 * */
	offset = SHBP_MEM_OFF(&shbp, &(shbp.alloc.deq));
	shbp.alloc.deq++;
	shbp.alloc.deq  = CPU_TO_LE32(shbp.alloc.deq);
	icontext_dma_write(&bp->ic, sizeof(uint32_t), &shbp.alloc.deq, 
	                   bp->shbp + offset);
	
	cdma_mutex_lock_release(bp->shbp);
		
	pr_debug("buf high = 0x%x\n", (uint32_t)((buf & 0xFFFFFFFF00000000) >> 32)); \
	pr_debug("buf low = 0x%x\n", (uint32_t)(buf & 0xFFFFFFFF)); \

	return buf;
}

int shbp_release(struct shbp_aiop *bp, uint64_t buf)
{
	struct shbp shbp;
	uint32_t offset;
	
	buf = CPU_TO_LE64(buf);

	DUMP_AIOP_BP();

	cdma_mutex_lock_take(bp->shbp, CDMA_MUTEX_WRITE_LOCK);
	
	/* Read SHBP structure:
	 *  */
	icontext_dma_read(&bp->ic, sizeof(struct shbp), bp->shbp, &shbp);

	shbp.free.base = CPU_TO_LE64(shbp.free.base); 
	shbp.free.deq  = CPU_TO_LE32(shbp.free.deq);
	shbp.free.enq  = CPU_TO_LE32(shbp.free.enq);

	DUMP_SHBP();

	if (SHBP_FREE_IS_FULL(&shbp)) {
		cdma_mutex_lock_release(bp->shbp);
		return -ENOSPC;
	}
	
	/*
	 * Write the buffer:
	 */
	offset = SHBP_BD_OFF(&shbp, shbp.free.enq);
	icontext_dma_write(&bp->ic, sizeof(uint64_t), &buf, 
	                   shbp.free.base + offset);
	/*
	 * Increase enqueue: 
	 */
	offset = SHBP_MEM_OFF(&shbp, &(shbp.free.enq));
	shbp.free.enq++;
	shbp.free.enq  = CPU_TO_LE32(shbp.free.enq);
	icontext_dma_write(&bp->ic, sizeof(uint32_t), &shbp.free.enq, 
	                   bp->shbp + offset);

	cdma_mutex_lock_release(bp->shbp);
	
	return 0;
}

int shbp_enable(uint16_t swc_id, uint64_t shbp_iova, struct shbp_aiop *bp)
{	
	int err;
	
	if ((shbp_iova == NULL) || (bp == NULL))
		return -EINVAL;
		
	lock_spinlock(&bp->lock);
	
	bp->shbp = shbp_iova;
	err = icontext_get(swc_id, &bp->ic);
	if (err) {
		unlock_spinlock(&bp->lock);
		pr_err("No such isolation context 0x%x\n", swc_id);
		return -EINVAL;
	}
	
	unlock_spinlock(&bp->lock);
	
	DUMP_AIOP_BP();
	
	return 0;
}
