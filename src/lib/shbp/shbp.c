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

#include <fsl_shbp.h>
#include <fsl_cdma.h>
#include <fsl_spinlock.h>
#include <fsl_dbg.h>
#include <fsl_sl_dbg.h>
#include <fsl_shbp_flib.h>

/*
 * SHBP is assumed to be implemented in little endian that's why AIOP does 
 * swaps
 */

#define DUMP_SHBP_FREE() \
	do {\
		sl_pr_debug("shbp.alloc_master = 0x%x\n", shbp.alloc_master); \
		sl_pr_debug("shbp.max_num = 0x%x size = %d\n", shbp.max_num, SHBP_SIZE(&shbp)); \
		sl_pr_debug("shbp.free.base high = 0x%x\n", (uint32_t)((shbp.free.base & 0xFFFFFFFF00000000) >> 32)); \
		sl_pr_debug("shbp.free.base low = 0x%x\n", (uint32_t)(shbp.free.base & 0xFFFFFFFF)); \
		sl_pr_debug("shbp.free.deq = 0x%x\n", shbp.free.deq); \
		sl_pr_debug("shbp.free.enq = 0x%x\n\n", shbp.free.enq); \
	} while(0)

#define DUMP_SHBP_ALLOC() \
	do {\
		sl_pr_debug("shbp.alloc_master = 0x%x\n", shbp.alloc_master); \
		sl_pr_debug("shbp.max_num = 0x%x size = %d\n", shbp.max_num, SHBP_SIZE(&shbp)); \
		sl_pr_debug("shbp.alloc.base high = 0x%x\n", (uint32_t)((shbp.alloc.base & 0xFFFFFFFF00000000) >> 32)); \
		sl_pr_debug("shbp.alloc.base low = 0x%x\n", (uint32_t)(shbp.alloc.base & 0xFFFFFFFF)); \
		sl_pr_debug("shbp.alloc.deq = 0x%x\n", shbp.alloc.deq); \
		sl_pr_debug("shbp.alloc.enq = 0x%x\n\n", shbp.alloc.enq); \
	} while(0)


SHBP_CODE_PLACEMENT uint64_t shbp_acquire(uint64_t bp, struct icontext *ic)
{
	struct shbp shbp;
	uint32_t offset;
	uint64_t buf;
	int err;
	
#ifdef DEBUG
	if (bp == 0)
		return 0;
#endif
		
	cdma_mutex_lock_take(bp, CDMA_MUTEX_WRITE_LOCK);
	
	/* Read SHBP structure:
	 *  */
	err = icontext_dma_read(ic, (uint16_t)sizeof(struct shbp), 
	                  bp, &shbp);
	if (err) {
		cdma_mutex_lock_release(bp);
		return NULL;
	}
	if (shbp.alloc_master) {
		/* Pool does not belong to AIOP */
		cdma_mutex_lock_release(bp);
		return NULL;
	}

	shbp.alloc.base = CPU_TO_LE64(shbp.alloc.base); 
	shbp.alloc.deq  = CPU_TO_LE32(shbp.alloc.deq);
	shbp.alloc.enq  = CPU_TO_LE32(shbp.alloc.enq);
	
	DUMP_SHBP_ALLOC();

	if (SHBP_ALLOC_IS_EMPTY(&shbp)) {
		cdma_mutex_lock_release(bp);
		return NULL;
	}
	
	/* Read the buffer:
	 * offset in byte = index of BD * 8 which is index << 3 */
	buf = NULL;
	offset = SHBP_BD_OFF(&shbp, shbp.alloc.deq);
	err = icontext_dma_read(ic, (uint16_t)sizeof(uint64_t), 
	                  shbp.alloc.base + offset, &buf);
	if (err) {
		cdma_mutex_lock_release(bp);
		return NULL;
	}
	buf = CPU_TO_LE64(buf);
	
	/* Increase the dequeue:
	 * */
	offset = SHBP_MEM_OFF(&shbp, &(shbp.alloc.deq));
	shbp.alloc.deq++;
	shbp.alloc.deq  = CPU_TO_LE32(shbp.alloc.deq);
	err = icontext_dma_write(ic, (uint16_t)sizeof(uint32_t), &shbp.alloc.deq, 
	                   bp + offset);
	if (err) {
		cdma_mutex_lock_release(bp);
		return NULL;
	}

	cdma_mutex_lock_release(bp);
		
	sl_pr_debug("buf high = 0x%x\n", (uint32_t)((buf & 0xFFFFFFFF00000000) >> 32)); \
	sl_pr_debug("buf low = 0x%x\n", (uint32_t)(buf & 0xFFFFFFFF)); \

	return buf;
}

SHBP_CODE_PLACEMENT int shbp_release(uint64_t bp, uint64_t buf, struct icontext *ic)
{
	struct shbp shbp;
	uint32_t offset;
	int err;
	
#ifdef DEBUG
	if (bp == 0)
		return -EINVAL;
#endif
	
	buf = CPU_TO_LE64(buf);

	cdma_mutex_lock_take(bp, CDMA_MUTEX_WRITE_LOCK);
	
	/* Read SHBP structure:
	 *  */
	err = icontext_dma_read(ic, (uint16_t)sizeof(struct shbp), 
	                        bp, &shbp);
	if (err) {
		cdma_mutex_lock_release(bp);
		return err;
	}
	
	shbp.free.base = CPU_TO_LE64(shbp.free.base); 
	shbp.free.deq  = CPU_TO_LE32(shbp.free.deq);
	shbp.free.enq  = CPU_TO_LE32(shbp.free.enq);
	
	DUMP_SHBP_FREE();

	if (SHBP_FREE_IS_FULL(&shbp)) {
		cdma_mutex_lock_release(bp);
		return -ENOSPC;
	}
	
	/*
	 * Write the buffer:
	 */
	offset = SHBP_BD_OFF(&shbp, shbp.free.enq);
	err = icontext_dma_write(ic, (uint16_t)sizeof(uint64_t), &buf, 
	                   shbp.free.base + offset);
	if (err) {
		cdma_mutex_lock_release(bp);
		return err;
	}

	/*
	 * Increase enqueue: 
	 */
	offset = SHBP_MEM_OFF(&shbp, &(shbp.free.enq));
	shbp.free.enq++;
	shbp.free.enq  = CPU_TO_LE32(shbp.free.enq);
	err = icontext_dma_write(ic, (uint16_t)sizeof(uint32_t), 
	                         &shbp.free.enq, bp + offset);

	cdma_mutex_lock_release(bp);
	
	return err;
}
