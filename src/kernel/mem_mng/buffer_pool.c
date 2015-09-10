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

#include "buffer_pool.h"
#include "fsl_spinlock.h"
#include "buffer_pool.h"
#include "fsl_dbg.h"
#include "fsl_sl_dbg.h"
#include "fsl_cdma.h"
#include "fsl_mem_mng.h"


/* Put all function (execution code) into  dtext_vle section , aka __COLD_CODE */
__START_COLD_CODE

const uint32_t STACK_ENTRY_BYTE_SIZE = 8;


static uint32_t compute_block_size(const uint16_t     buff_size,
                                   const uint16_t     buff_alignment);
/*
 *  Computes an aligned buffer size based on provided buffer size and an
 *  buff_alignment.
 */
static uint32_t compute_block_size(const uint16_t  buff_size,
                                   const uint16_t  buff_alignment)
{
	uint32_t  block_size = buff_size;
	if(block_size < buff_alignment)
	    block_size = buff_alignment;
	else{
		if(buff_size%buff_alignment)
			block_size += buff_alignment - buff_size%buff_alignment;
	}
	return block_size;
}

/*
 * Computes maximal number of buffers that might be allocated from provided
 * total  memory size.
 */
uint32_t buff_pool_compute_num_buffers(const uint32_t  total_mem_size,
	                        const uint16_t  buff_size,
                                const uint16_t  buff_alignment)
{
    uint32_t block_size = compute_block_size(buff_size,buff_alignment);
    if(total_mem_size < block_size || total_mem_size < buff_alignment)
	    return 0;
    return (total_mem_size - buff_alignment)/block_size;
}

int buff_pool_create(struct buffer_pool *p_bf_pool,
                      const uint32_t        bf_pool_id,
                      const uint32_t        num_buffs,
                      const uint16_t        buff_size,
                      const uint16_t        buff_alignment,
                      void 	    	    *h_boot_mem_mng)
{
	int rc = 0;
	uint64_t phys_addr = 0;
	uint64_t curr_buffer_addr = 0;
	uint64_t *curr_buff_stack = NULL;
	uint32_t aligned_buffer_size = 0;
	struct icontext ic = {0};
	//struct buffer_pool *p_bf_pool = NULL;
	ASSERT_COND_LIGHT(h_boot_mem_mng);
	struct initial_mem_mng* boot_mem_mng = (struct initial_mem_mng*)h_boot_mem_mng;
	/* store info about this buffer pool */
	p_bf_pool->num_buffs = num_buffs;
	p_bf_pool->current = 0;
	p_bf_pool->buff_size = buff_size;
	p_bf_pool->bf_pool_id = bf_pool_id;
	icontext_aiop_get(&ic);
	/* Allocate stack of pointers to blocks */
	rc =  boot_get_mem(boot_mem_mng,num_buffs*STACK_ENTRY_BYTE_SIZE,&phys_addr);
	if(rc){
		pr_err("Memory allocation failed for buffer_pool id %d \n",
		       bf_pool_id);
		return -ENOMEM;
	}
	p_bf_pool->buffers_stack_addr = phys_addr;
	aligned_buffer_size = compute_block_size(buff_size,buff_alignment);
	/* Allocate blocks */
	rc =  boot_get_mem(boot_mem_mng,num_buffs*aligned_buffer_size,&phys_addr);
	if(rc){
		pr_err("Memory allocation failed in buffer_pool_create, "
			"bf_pool_id %d\n",bf_pool_id);
		return -ENOMEM;
	}
	//  Set the first phys_addr to be aligned to buff_alignment
	phys_addr= ALIGN_UP_64(phys_addr,buff_alignment);
	p_bf_pool->p_buffers_addr = phys_addr;
	curr_buffer_addr = p_bf_pool->p_buffers_addr;
	/* initialize pointers to buffers */
	for (int i=0; i < num_buffs; i++)
	{
		icontext_dma_write(&ic,
		                   STACK_ENTRY_BYTE_SIZE,
				   &curr_buffer_addr,
				   p_bf_pool->buffers_stack_addr+i*STACK_ENTRY_BYTE_SIZE);
		/*p_bf_pool->p_buffers_stack[i] = curr_buffer_addr;*/
		curr_buffer_addr += aligned_buffer_size;
	}
	return 0;
}
int buff_pool_get(struct buffer_pool *bf_pool, uint64_t* buffer_addr)
{
#ifdef DEBUG
	uint64_t zero_value = 0;
#endif
	ASSERT_COND(bf_pool);
	cdma_mutex_lock_take(bf_pool->p_buffers_addr, CDMA_MUTEX_WRITE_LOCK);
	/* check if there is an available block */
	if (bf_pool->current == bf_pool->num_buffs)
	{
		sl_pr_err("Buffer pool memory depletion for id = %d, num_buffs = %d\n",
		bf_pool->bf_pool_id, bf_pool->num_buffs);
		cdma_mutex_lock_release(bf_pool->p_buffers_addr);
		ASSERT_COND(0);
		return -ENAVAIL;
	}
	/* get the  address of a buffer */
	cdma_read(buffer_addr,bf_pool->buffers_stack_addr +
	               STACK_ENTRY_BYTE_SIZE*bf_pool->current,
	               STACK_ENTRY_BYTE_SIZE);
#ifdef DEBUG
	/* bf_pool->p_buffers_stack[bf_pool->current] = 0; */
	cdma_write(bf_pool->buffers_stack_addr +
	                STACK_ENTRY_BYTE_SIZE*bf_pool->current,
	                &zero_value,
	                STACK_ENTRY_BYTE_SIZE);
#endif /* DEBUG */
	/* advance current index */
	bf_pool->current++;
	cdma_mutex_lock_release(bf_pool->p_buffers_addr);
	return 0;
}

int buff_pool_put(struct buffer_pool  *bf_pool, uint64_t buffer_addr)
{
	ASSERT_COND(bf_pool);
	cdma_mutex_lock_take(bf_pool->p_buffers_addr, CDMA_MUTEX_WRITE_LOCK);
	/* check if blocks stack is full */
	if (bf_pool->current > 0)
	{
		/* decrease current index */
		bf_pool->current--;
		/* put the block */
		/*bf_pool->p_buffers_stack[bf_pool->current] = PTR_TO_UINT(p_block);*/
		cdma_write(bf_pool->buffers_stack_addr+
	                      +STACK_ENTRY_BYTE_SIZE*bf_pool->current,
	                      &buffer_addr,
	                      STACK_ENTRY_BYTE_SIZE);
		cdma_mutex_lock_release(bf_pool->p_buffers_addr);
		return 0;
	}
	sl_pr_err("Couldn't put buffer 0x%x%08x into buffer pool id %d\n",
	       (uint32_t)(buffer_addr >> 32),
	       (uint32_t)(buffer_addr),bf_pool->bf_pool_id);
	cdma_mutex_lock_release(bf_pool->p_buffers_addr);
	return -ENOSPC;
}

__END_COLD_CODE
