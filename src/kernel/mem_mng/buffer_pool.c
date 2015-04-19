#include "buffer_pool.h"
#include "errors.h"
#include "mem_mng_util.h"
#include "fsl_spinlock.h"
#include "buffer_pool.h"
#include "fsl_dbg.h"
#include "fsl_sl_dbg.h"



const uint32_t STACK_ENTRY_BYTE_SIZE = 8;


__COLD_CODE int buffer_pool_create(struct buffer_pool *p_bf_pool,
                      const uint32_t        bf_pool_id,
                      const uint32_t        num_buffs,
                      const uint16_t        buff_size,
                      void 	    	    *h_boot_mem_mng)
{
	int rc = 0;
	uint64_t phys_addr = 0;
	uint64_t curr_buffer_addr = 0;
	uint64_t *curr_buff_stack = NULL;
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
	/* Allocate blocks */
	rc =  boot_get_mem(boot_mem_mng,num_buffs*buff_size,&phys_addr);
	if(rc){
		pr_err("Memory allocation failed in buffer_pool_create, "
			"bf_pool_id %d\n",bf_pool_id);
		return -ENOMEM;
	}
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
		curr_buffer_addr += buff_size;
	}
	return 0;
}

__COLD_CODE int get_buff(struct buffer_pool *bf_pool, uint64_t* buffer_addr)
{
	struct icontext ic = {0};
#ifdef DEBUG
	uint64_t zero_value = 0;
#endif
	ASSERT_COND(bf_pool);
	icontext_aiop_get(&ic);
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
	icontext_dma_read(&ic,
	                  STACK_ENTRY_BYTE_SIZE,
	                  bf_pool->buffers_stack_addr +
		               STACK_ENTRY_BYTE_SIZE*bf_pool->current,
		          buffer_addr);
#ifdef DEBUG
	/* bf_pool->p_buffers_stack[bf_pool->current] = 0; */
	icontext_dma_write(&ic,
	                   STACK_ENTRY_BYTE_SIZE,
	                   &zero_value,
	                   bf_pool->buffers_stack_addr +
		                STACK_ENTRY_BYTE_SIZE*bf_pool->current);
#endif /* DEBUG */
	/* advance current index */
	bf_pool->current++;
	cdma_mutex_lock_release(bf_pool->p_buffers_addr);
	return 0;
}

__COLD_CODE int put_buff(struct buffer_pool  *bf_pool, uint64_t buffer_addr)
{
	struct icontext ic = {0};
	ASSERT_COND(bf_pool);
	icontext_aiop_get(&ic);
	cdma_mutex_lock_take(bf_pool->p_buffers_addr, CDMA_MUTEX_WRITE_LOCK);
	/* check if blocks stack is full */
	if (bf_pool->current > 0)
	{
		/* decrease current index */
		bf_pool->current--;
		/* put the block */
		/*bf_pool->p_buffers_stack[bf_pool->current] = PTR_TO_UINT(p_block);*/
		icontext_dma_write(&ic,
		                   STACK_ENTRY_BYTE_SIZE,
		                   &buffer_addr,
		                   bf_pool->buffers_stack_addr+
		                      +STACK_ENTRY_BYTE_SIZE*bf_pool->current);
		cdma_mutex_lock_release(bf_pool->p_buffers_addr);
		return 0;
	}
	sl_pr_err("Couldn't put buffer 0x%x%08x into buffer pool id %d\n",
	       (uint32_t)(buffer_addr >> 32),
	       (uint32_t)(buffer_addr),bf_pool->bf_pool_id);
	cdma_mutex_lock_release(bf_pool->p_buffers_addr);
	return -ENOSPC;
}
