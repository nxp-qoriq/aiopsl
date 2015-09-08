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
#include "fsl_types.h"
#include "fsl_errors.h"
#include "fsl_gen.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_dbg.h"
#include "fsl_platform.h"
#include "fsl_mem_mng.h"
#include "slob.h"
#include "buffer_pool.h"
#include "fsl_sl_dbg.h"
#include "fsl_cdma.h"


#define NEXT_ADDR_OFFSET (PTR_TO_UINT(&((t_slob_block *)0)->next_addr))


/* Put all function (execution code) into  dtext_vle section , aka __COLD_CODE */
__START_COLD_CODE



/**********************************************************************
 *                     MM internal routines set                       *
 **********************************************************************/


/****************************************************************
 *  Routine:   create_new_block_by_boot_mng
 *
 *  Description:
 *      Initializes a new memory block of "size" bytes and started
 *      from "base" address.
 *
 *  Arguments:
 *      base    - base address of the memory block
 *      size    - size of the memory block
 *      boot_mem_mng - Memory manager to allocate from.
 *
 *  Return value:
 *      A pointer to new created structure returned on success;
 *      Otherwise, NULL.
 ****************************************************************/
static int create_new_block_by_boot_mng(const uint64_t base,
                                        const uint64_t size,
		                        struct initial_mem_mng* boot_mem_mng,
		                        uint64_t *address)
{
    t_mem_block loc_mem_block = {0};
    int rc = boot_get_mem(boot_mem_mng,sizeof(t_mem_block),address);
    if(rc)
    {
        sl_pr_err("Slob: memory allocation failed\n");
        return  rc;
    }
    loc_mem_block.base = base;
    loc_mem_block.end = base+size;
    loc_mem_block.next_addr = 0;
    cdma_write(*address,&loc_mem_block,sizeof(loc_mem_block));
    return 0;
}

/****************************************************************
 *  Routine:   create_new_block
 *
 *  Description:
 *      Initializes a new block of of "size" bytes and
 *      started from "base" address.
 *
 *  Arguments:
 *      p_MM      - slob object
 *      base      - base address of the free block
 *      size      - size of the free block
 *      block_addr[out] - physical address allocated in this function
 *
 *  Return value:
 *      A non-zero value indicates failure, zero - success.
 *
 ****************************************************************/
static  int create_new_block(t_MM *p_MM,
                          const uint64_t base,
                          const uint64_t size,
                          uint64_t *block_addr)
{
    int rc = -ENAVAIL;
    struct buffer_pool* slob_bf_pool = (struct buffer_pool*)(p_MM->h_slob_bf_pool);
    t_slob_block loc_slob_block = {0};
    rc = buff_pool_get(slob_bf_pool,block_addr);
    if (0 != rc)
    {
        sl_pr_err("Slob: memory allocation failed");
        return rc;
    }
    loc_slob_block.base = base;
    loc_slob_block.end = base + size;
    loc_slob_block.next_addr = 0;
    cdma_write(*block_addr,&loc_slob_block,sizeof(loc_slob_block));
    return 0;
}

/****************************************************************/
static int insert_free_block(uint64_t *new_b_addr,t_MM *p_MM,
                             const uint64_t curr_b_addr,const uint64_t end,
		             const uint32_t alignment,const uint64_t align_base,
		             uint64_t prev_b_addr,const int     i)
{
	 /* This is an old code line that assumes that size of an allocated memory is
	 * multiply of alignment. Replaced this by a condition that a new block
	 * is greater than the current alignment.
	 * if ( !p_curr_b && ((((uint64_t)(end-base)) & ((uint64_t)(alignment-1))) == 0) )
	 * */
	if ( curr_b_addr == 0  &&  (end-align_base) >= alignment )
	{
		if (create_new_block(p_MM,align_base, end-align_base,
		                                  new_b_addr) != 0)
			return  -ENOMEM;
		if (prev_b_addr)
		{
			cdma_write(prev_b_addr + offsetof(t_slob_block,next_addr),new_b_addr,
			           sizeof(*new_b_addr));
		}
		else
		{
			cdma_write(p_MM->head_free_blocks_addr + i*sizeof(uint64_t),
			           new_b_addr,sizeof(uint64_t));
		}
	}
	return 0;
}
/****************************************************************/

static void update_boundaries(const uint32_t alignment,
                              const uint64_t new_b_addr, const uint64_t curr_b_addr,
                              uint64_t *end,uint64_t *base)
{
    t_slob_block curr_b = {0};
    if ((alignment == 1) && new_b_addr == 0)
	{
		if(curr_b_addr)
		{
		    cdma_read(&curr_b,curr_b_addr,sizeof(curr_b));
		    if(*base > curr_b.base)
			*base = curr_b.base;
		    if( *end < curr_b.end)
			*end = curr_b.end;
		}
	}
}

/****************************************************************
 *  Routine:    AddFree
 *
 *  Description:
 *      Adds a new free block to the free lists. It updates each
 *      free list to include a new free block.
 *      Note, that all free block in each free list are ordered
 *      by their base address.
 *
 *  Arguments:
 *      p_MM  - pointer to the MM object
 *      base  - base address of a given free block
 *      end   - end address of a given free block
 *
 *  Return value:
 *
 *
 ****************************************************************/
/* This pragma is a directive for the compiler not to in-line cdma_read/cdma_write.
 * Without the pragma the function uses a big stack size.
 * These  functions require 4 consecutive registers to be available  forcing compiler
 * to store more variables on the stack. All the in-lined functions inside the
 * pragma become regular functions.
 */
#pragma dont_inline on
static int add_free(t_MM *p_MM, uint64_t base, uint64_t end)
{

    t_slob_block curr_b = {0},next_b = {0};
    uint64_t    prev_b_addr = 0,new_b_addr = 0 , next_b_addr = 0, curr_b_addr = 0;
    uint64_t    align_base = 0;
    int         i;
    struct buffer_pool *slob_bf_pool = (struct buffer_pool *)p_MM->h_slob_bf_pool;
    uint32_t    alignment = 4;

    /* Updates free lists to include  a just released block */
    for (i=0; i <= MM_MAX_ALIGNMENT; i++)
    {
        prev_b_addr = new_b_addr = 0;
        cdma_read(&curr_b_addr,p_MM->head_free_blocks_addr + i*sizeof(uint64_t),
                  sizeof(curr_b_addr));
        if(0 != curr_b_addr)
        {
            cdma_read(&curr_b,curr_b_addr,sizeof(curr_b));
        }

        alignment = (0x1 << i);
        align_base = MAKE_ALIGNED(base, alignment);

        /* Goes to the next free list if there is no block to free */
        if (align_base >= end)
            continue;

        /* Looks for a free block that should be updated */
        while ( 0 != curr_b_addr )
        {
            if ( align_base <= curr_b.end )
            {
                if ( end > curr_b.end )
                {
                    if(0 != curr_b.next_addr)
                    {
                        cdma_read(&next_b,curr_b.next_addr,sizeof(next_b));
                    }
                    while ( 0 != curr_b.next_addr && end > next_b.end )
                    {
                        next_b_addr = curr_b.next_addr;
                        curr_b.next_addr = next_b.next_addr;
                        cdma_write(curr_b_addr,&curr_b,sizeof(curr_b));
                        buff_pool_put(slob_bf_pool,next_b_addr);
                        if(0 != curr_b.next_addr)
                        {
                            cdma_read(&next_b,curr_b.next_addr,sizeof(next_b));
                        }
                    }
                    next_b_addr = curr_b.next_addr;
                    if(0 != next_b_addr)
                    {
                        cdma_read(&next_b,next_b_addr,sizeof(next_b));
                    }
                    if ( next_b_addr == 0 || ( 0 != next_b_addr  && end < next_b.base) )
                    {
                         curr_b.end = end;
                         cdma_write(curr_b_addr,&curr_b,sizeof(curr_b));
                    }
                    else
                    {
                        curr_b.end = next_b.end;
                        curr_b.next_addr = next_b.next_addr;
                        buff_pool_put(slob_bf_pool,next_b_addr);
                        cdma_write(curr_b_addr,&curr_b,sizeof(curr_b));
                    }
                }
                else if ( (end < curr_b.base) && ((end-align_base) >= alignment) )
                {
                    if (create_new_block(p_MM,align_base, end-align_base,
                                                     &new_b_addr) != 0)
                    {
                        sl_pr_err("Slob: memory allocation failed\n");
                        return -ENOMEM;
                    }
                    cdma_write(new_b_addr+offsetof(t_slob_block,next_addr),&curr_b_addr,
                               sizeof(curr_b_addr));
                    if (prev_b_addr)
                    {
                        cdma_write(prev_b_addr+offsetof(t_slob_block,next_addr),&new_b_addr,sizeof(new_b_addr));
		    }
                    else
                    {
                        cdma_write(p_MM->head_free_blocks_addr+i*sizeof(uint64_t),&new_b_addr,
                                   sizeof(new_b_addr));
                    }
                    break;
                }
                if ((align_base < curr_b.base) && (end >= curr_b.base))
                {
                    curr_b.base = align_base;
                    cdma_write(curr_b_addr,&curr_b,sizeof(curr_b));
                }

                /* if size of the free block is less then alignment
                 * deletes that free block from the free list. */
                if ( (curr_b.end - curr_b.base) < alignment)
                {
                    if ( prev_b_addr )
		    {
                        cdma_write(prev_b_addr+offsetof(t_slob_block,next_addr),&curr_b.next_addr,
                                   sizeof(curr_b.next_addr));
		    }
                    else
                    {
                        cdma_write(p_MM->head_free_blocks_addr + i*sizeof(uint64_t),
                                   &curr_b.next_addr,sizeof(curr_b.next_addr));
                    }
                    buff_pool_put(slob_bf_pool,curr_b_addr);
                    curr_b_addr = 0;
                }
                break;
            }
            else
            {
                prev_b_addr = curr_b_addr;
                cdma_read(&curr_b_addr,prev_b_addr+offsetof(t_slob_block,next_addr),sizeof(curr_b_addr));
                if(0 != curr_b_addr)
                {
                    cdma_read(&curr_b,curr_b_addr,sizeof(curr_b));
                }
            }
        } // while

        /* If no free block found to be updated, insert a new free block
         * to the end of the free list.
         */
        if(0 != insert_free_block(&new_b_addr,p_MM,curr_b_addr,end,alignment,align_base,prev_b_addr,i))
        {
            sl_pr_err("Slob: memory allocation failed\n");
            return -ENOMEM;
        }
        /* Update boundaries of the new free block */
        update_boundaries(alignment,new_b_addr,curr_b_addr,&end,&base);
    }// for
    return (0);
}
#pragma dont_inline reset

/****************************************************************
 *  Routine:      CutFree
 *
 *  Description:
 *      Cuts a free block from holdBase to holdEnd from the free lists.
 *      That is, it updates all free lists of the MM object do
 *      not include a block of memory from holdBase to holdEnd.
 *      For each free lists it seek for a free block that holds
 *      either holdBase or holdEnd. If such block is found it updates it.
 *
 *  Arguments:
 *      p_MM            - pointer to the MM object
 *      holdBase        - base address of the allocated block
 *      holdEnd         - end address of the allocated block
 *
 *  Return value:
 *      0 is returned on success,
 *      otherwise returns an error code.
 *
 ****************************************************************/
/* This pragma is a directive for the compiler not to in-line cdma_read/cdma_write.
 * Without the pragma the function uses a big stack size.
 * These  functions require 4 consecutive registers to be available  forcing compiler
 * to store more variables on the stack. All the in-lined functions inside the
 * pragma become regular functions.
 */
#pragma dont_inline on
static int cut_free(t_MM *p_MM, const uint64_t hold_base, const uint64_t hold_end)
{

    t_slob_block curr_b = {0};
    uint64_t new_b_addr = 0,prev_b_addr = 0,curr_b_addr = 0;
    uint64_t    align_base = 0, base = 0, end = 0;
    uint32_t    alignment = 4;
    int         i;

    struct buffer_pool* slob_bf_pool = (struct buffer_pool*)p_MM->h_slob_bf_pool;

    for (i=0; i <= MM_MAX_ALIGNMENT; i++)
    {
        prev_b_addr = 0;
        cdma_read(&curr_b_addr,p_MM->head_free_blocks_addr+ i*sizeof(uint64_t),
                  sizeof(curr_b_addr));
        if(0 != curr_b_addr)
        {
            cdma_read(&curr_b,curr_b_addr,sizeof(curr_b));
        }

        alignment = (0x1 << i);
        align_base = MAKE_ALIGNED(hold_end, alignment);

        while( curr_b_addr )
        {

            base = curr_b.base;
            end = curr_b.end;

            if ( (hold_base <= base) && (hold_end <= end) && (hold_end > base) )
            {
                if ( align_base >= end ||
                     (align_base < end && ((end-align_base) < alignment)) )
                {
                    if (0 != prev_b_addr)
                    {
                        cdma_write(prev_b_addr+offsetof(t_slob_block,next_addr),&curr_b.next_addr,sizeof(curr_b.next_addr));
                    }
                    else
                    {
                        cdma_write(p_MM->head_free_blocks_addr+ i*sizeof(uint64_t),
                                   &curr_b.next_addr,sizeof(curr_b.next_addr));
                    }
                    buff_pool_put(slob_bf_pool,curr_b_addr);
                }
                else
                {
                    curr_b.base = align_base;
                }
                cdma_write(curr_b_addr,&curr_b,sizeof(curr_b));
                break;
            }
            else if ( (hold_base > base) && (hold_end <= end) )
            {
                if ( (hold_base-base) >= alignment )
                {
                    if ( (align_base < end) && ((end-align_base) >= alignment) )
                    {
                        if (create_new_block(p_MM,align_base,
                                                         end-align_base,
                                                         &new_b_addr) != 0)
                        {
                            sl_pr_err("Slob: memory allocation failed\n");
                            return -ENOMEM;
                        }
                        cdma_write(new_b_addr + offsetof(t_slob_block,next_addr),&curr_b.next_addr,
                                   sizeof(curr_b.next_addr));
                        curr_b.next_addr = new_b_addr;
                    }
                    curr_b.end = hold_base;
                }
                else if ( (align_base < end) && ((end-align_base) >= alignment) )
                {
                    curr_b.base = align_base;
                }
                else
                {
                    if (0 != prev_b_addr)
                    {
                        cdma_write(prev_b_addr+offsetof(t_slob_block,next_addr),&curr_b.next_addr,
                            sizeof(curr_b.next_addr));
                    }
                    else
                    {
                        cdma_write(p_MM->head_free_blocks_addr + i*sizeof(uint64_t),
                                  &curr_b.next_addr,sizeof(curr_b.next_addr));
                    }
                    buff_pool_put(slob_bf_pool,curr_b_addr);
                }
                cdma_write(curr_b_addr,&curr_b,sizeof(curr_b));
                break;
            }
            else
            {
                prev_b_addr = curr_b_addr;
                cdma_write(curr_b_addr,&curr_b,sizeof(curr_b));
                curr_b_addr = curr_b.next_addr;
                if(0 != curr_b_addr)
                {
                    cdma_read(&curr_b,curr_b_addr,sizeof(curr_b));
                }
            }
        }
    }
    return (0);
}
#pragma dont_inline reset
/****************************************************************
 *  Routine:     AddBusy
 *
 *  Description:
 *      Adds a new busy block to the list of busy blocks. Note,
 *      that all busy blocks are ordered by their base address in
 *      the busy list.
 *
 *  Arguments:
 *      MM              - handler to the MM object
 *      p_NewBusyB      - pointer to the a busy block
 *
 *  Return value:
 *      None.
 *
 ****************************************************************/
static void add_busy(t_MM *p_MM, uint64_t new_busy_addr)
{
    t_slob_block new_busy_b = {0},curr_busy_b = {0};
    uint64_t curr_busy_b_addr = 0, prev_busy_b_addr = 0;

    cdma_read(&new_busy_b,new_busy_addr,sizeof(new_busy_b));
    /* finds a place of a new busy block in the list of busy blocks */
    curr_busy_b_addr = p_MM->head_busy_blocks_addr;
    if(0 != curr_busy_b_addr)
    {
        cdma_read(&curr_busy_b,curr_busy_b_addr,sizeof(curr_busy_b));
    }

    while ( curr_busy_b_addr && new_busy_b.base > curr_busy_b.base )
    {
	prev_busy_b_addr = curr_busy_b_addr;
	curr_busy_b_addr = curr_busy_b.next_addr;
        if(0 != curr_busy_b_addr)
        {
            cdma_read(&curr_busy_b,curr_busy_b_addr,sizeof(curr_busy_b));
        }
    }
    /* insert the new busy block into the list of busy blocks */
    if ( curr_busy_b_addr )
    {
	new_busy_b.next_addr =  curr_busy_b_addr;
        cdma_write(new_busy_addr,&new_busy_b,sizeof(new_busy_b));
    }
    if ( prev_busy_b_addr )
    {
        cdma_write(prev_busy_b_addr + offsetof(t_slob_block,next_addr),&new_busy_addr,sizeof(new_busy_addr));
    }
    else
    {
        p_MM->head_busy_blocks_addr = new_busy_addr;
    }
}

/****************************************************************
 *  Routine:    CutBusy
 *
 *  Description:
 *      Cuts a block from base to end from the list of busy blocks.
 *      This is done by updating the list of busy blocks do not
 *      include a given block, that block is going to be free. If a
 *      given block is a part of some other busy block, so that
 *      busy block is updated. If there are number of busy blocks
 *      included in the given block, so all that blocks are removed
 *      from the busy list and the end blocks are updated.
 *      If the given block devides some block into two parts, a new
 *      busy block is added to the busy list.
 *
 *  Arguments:
 *      p_MM  - pointer to the MM object
 *      base  - base address of a given busy block
 *      end   - end address of a given busy block
 *
 *  Return value:
 *      0 on success, E_NOMEMORY otherwise.
 *
 ****************************************************************/

/****************************************************************
 *  Routine:     slob_get_greater_alignment
 *
 *  Description:
 *      Allocates a block of memory according to the given size
 *      and the alignment. That routine is called from the MM_Get
 *      routine if the required alignment is greater then MM_MAX_ALIGNMENT.
 *      In that case, it goes over free blocks of 64 byte align list
 *      and checks if it has the required size of bytes of the required
 *      alignment. If no blocks found returns 0.
 *      After the block is found and data is allocated, it calls
 *      the internal CutFree routine to update all free lists
 *      do not include a just allocated block. Of course, each
 *      free list contains a free blocks with the same alignment.
 *      It is also creates a busy block that holds
 *      information about an allocated block.
 *
 *  Arguments:
 *      MM              - handle to the MM object
 *      size            - size of the MM
 *      alignment       - index as a power of two defines
 *                        a required alignment that is greater then 64.
 *      name            - the name that specifies an allocated block.
 *
 *  Return value:
 *      base address of an allocated block.
 *      0 if can't allocate a block
 *
 ****************************************************************/
/* This pragma is a directive for the compiler not to in-line cdma_read/cdma_write.
 * Without the pragma the function uses a big stack size.
 * These  functions require 4 consecutive registers to be available  forcing compiler
 * to store more variables on the stack. All the in-lined functions inside the
 * pragma become regular functions.
 */
#pragma dont_inline on
static uint64_t slob_get_greater_alignment(t_MM *p_MM,
                                           const uint64_t size,
                                           const uint32_t alignment)
{
    t_slob_block free_b = {0};
    uint64_t new_busy_addr = 0;
    uint64_t  free_addr = 0;
    uint64_t    hold_base = 0, hold_end = 0, align_base = 0;
    struct buffer_pool* slob_bf_pool = (struct buffer_pool*)p_MM->h_slob_bf_pool;



    /* goes over free blocks of the 64 byte alignment list
       and look for a block of the suitable size and
       base address according to the alignment. */
    cdma_read(&free_addr,p_MM->head_free_blocks_addr + MM_MAX_ALIGNMENT*sizeof(uint64_t),
              sizeof(free_addr));
    if(0 != free_addr )
    {
        cdma_read(&free_b,free_addr,sizeof(free_b));
    }

    while(free_addr)
    {
	  align_base = MAKE_ALIGNED(free_b.base, alignment);

        /* the block is found if the aligned base inside the block
         * and has the enough size. */
	  if ( align_base >= free_b.base &&
	       align_base < free_b.end &&
	       size <= (free_b.end - align_base) )
	      break;
	  else
	  {
	      free_addr = free_b.next_addr;
	      cdma_read(&free_b,free_addr,sizeof(free_b));
	  }
    }

    /* If such block isn't found */
    if ( 0 == free_addr  )
    {
        return 0LL;
    }

    hold_base = align_base;
    hold_end = align_base + size;

    /* init a new busy block */
    if (create_new_block(p_MM,hold_base, size,&new_busy_addr) != 0)
    {
        return 0LL;
    }

    /* calls Update routine to update a lists of free blocks */
    if ( cut_free ( p_MM, hold_base, hold_end ) != 0 ) {
            buff_pool_put( slob_bf_pool,new_busy_addr);
	    return 0LL;
    }

    /* insert the new busy block into the list of busy blocks */
    add_busy ( p_MM,new_busy_addr);
    return (hold_base);
}
#pragma dont_inline reset

/**********************************************************************
 *                     MM API routines set                            *
 **********************************************************************/

/*****************************************************************************/

int slob_init(uint64_t*slob, const uint64_t base, const uint64_t size,
		      void * h_mem_mng,void* h_slob_bf_pool)
{
    t_MM        MM =  {0};
    int         i;
    uint64_t    paddr = 0;
    uint64_t    free_blocks_addr = 0;
    uint64_t    new_base, new_size;

    if (0 == size)
    {
        sl_pr_err("Slob: invalid value slob size (should be positive)\n");
    }
    struct initial_mem_mng* boot_mem_mng = (struct initial_mem_mng*)h_mem_mng;
    ASSERT_COND_LIGHT(boot_mem_mng);
    if(NULL == h_mem_mng)
        return -EINVAL;
    /* Initializes a new MM object */
    if (boot_get_mem(boot_mem_mng,sizeof(t_MM),&paddr) != 0)
    {
        sl_pr_err("Slob: memory allocation failed\n");
        return -ENOMEM;
    }
    *slob = paddr;
    MM.h_mem_mng = h_mem_mng;
    MM.h_slob_bf_pool = h_slob_bf_pool;


    /* Initializes counter of free memory to total size */
    MM.free_mem_size = size;

    /* A busy list is empty */
    MM.head_busy_blocks_addr = 0;

    /* Initializes a new memory block */
    if (create_new_block_by_boot_mng(base,size,boot_mem_mng,&MM.head_mem_blocks) !=0 ) {
	    //slob_free( *slob_addr); // No need to release anything at this point
	    sl_pr_err("Slob: memory allocation failed\n");
	    return -ENOMEM;
    }
    if(boot_get_mem(boot_mem_mng,sizeof(uint64_t)*(MM_MAX_ALIGNMENT + 1),
                    &MM.head_free_blocks_addr) != 0)
    {
	sl_pr_err("Slob: memory allocation failed\n");
        return -ENOMEM;
    }
    if(0 == size)
    {// allow a slob of size 0, should return 0 on any slob_get
	free_blocks_addr = 0;
        for (i=0; i <= MM_MAX_ALIGNMENT; i++){
            cdma_write(MM.head_free_blocks_addr + i*sizeof(uint64_t),&free_blocks_addr
                       ,sizeof(free_blocks_addr));
        }
        cdma_write(paddr,&MM,sizeof(MM));
        return 0;
    }

    /* Initializes a new free block for each free list*/
    for (i=0; i <= MM_MAX_ALIGNMENT; i++)
    {
        new_base = MAKE_ALIGNED( base, (0x1 << i) );
        new_size = size - (new_base - base);

        if (create_new_block(&MM,
                                     new_base,
                                     new_size,
                                     &free_blocks_addr) != 0) {
            cdma_write(paddr,&MM,sizeof(MM));
            slob_free(&paddr);
            sl_pr_err("Slob: memory allocation failed");
            return -ENOMEM;
        }
        cdma_write(MM.head_free_blocks_addr + i*sizeof(uint64_t),&free_blocks_addr,
                   sizeof(uint64_t));
    }
    cdma_write(paddr,&MM,sizeof(MM));
    return (0);
}

/*****************************************************************************/
void slob_free(uint64_t* slob)
{
    t_MM        MM = {0};
    t_slob_block busy_block = {0}, free_block = {0};
    uint64_t    busy_block_addr = 0, free_block_addr = 0, block_adr = 0;

    void        *p_block = NULL;
    int         i;

    ASSERT_COND(slob);
    cdma_read(&MM,*slob,sizeof(slob));

    struct buffer_pool* slob_bf_pool = (struct buffer_pool* )MM.h_slob_bf_pool;

    /* release memory allocated for busy blocks */
    busy_block_addr = MM.head_busy_blocks_addr;
    if( 0 != busy_block_addr)
    {
        cdma_read(&busy_block,busy_block_addr,sizeof(busy_block_addr));
    }

    while ( busy_block_addr )
    {
        block_adr = busy_block_addr;
        busy_block_addr = busy_block.next_addr;
        if( 0 != busy_block_addr)
        {
            cdma_read(&busy_block,busy_block_addr,sizeof(busy_block_addr));
        }
        //fsl_free(p_block);
        buff_pool_put(slob_bf_pool,block_adr);
    }

    /* release memory allocated for free blocks */
    for (i=0; i <= MM_MAX_ALIGNMENT; i++)
    {
        cdma_read(&free_block_addr,MM.head_free_blocks_addr + i*sizeof(uint64_t),
                  sizeof(free_block_addr));
        if(0 != free_block_addr)
        {
            cdma_read(&free_block,free_block_addr,sizeof(free_block));
        }
        while ( free_block_addr )
        {
            block_adr = free_block_addr;
            free_block_addr =  free_block.next_addr;
            if(0 != free_block_addr)
            {
                cdma_read(&free_block,free_block_addr,sizeof(free_block));
            }
            buff_pool_put(slob_bf_pool,block_adr);
        }
    }
}

/*****************************************************************************/
uint64_t slob_get(uint64_t* slob, const uint64_t size, uint32_t alignment)
{
    t_MM        MM = {0};
    uint64_t    hold_base, hold_end;
    uint64_t    new_busy_b_addr = 0, free_b_addr = 0;
    uint64_t slob_addr = 0;
    t_slob_block free_b = {0};
    uint32_t j, i = 0;
    struct buffer_pool* slob_bf_pool = NULL;

    ASSERT_COND(slob);
    slob_addr = *slob;

    if (size == 0)
    {
        sl_pr_err("Slob invalid value: allocation size must be positive\n");
    }
    /* checks that alignment value is greater then zero */
    if (alignment == 0)
    {
        alignment = 1;
    }

    j = alignment;

    /* checks if alignment is a power of two, if it correct and if the
       required size is multiple of the given alignment. */
    while ((j & 0x1) == 0)
    {
        i++;
        j = j >> 1;
    }

    /* if the given alignment isn't power of two, returns an error */
    if (j != 1)
    {
        sl_pr_err("Slob invalid value: alignment (should be power of 2)\n");
        return 0LL;
    }

    if (i > MM_MAX_ALIGNMENT)
    {
        cdma_mutex_lock_take(slob_addr, CDMA_MUTEX_WRITE_LOCK);
        cdma_read(&MM,slob_addr,sizeof(MM));
        hold_base = slob_get_greater_alignment(&MM, size, alignment);
        cdma_write(slob_addr,&MM,sizeof(MM));
        cdma_mutex_lock_release(slob_addr);
        return hold_base ;
    }

    cdma_mutex_lock_take(slob_addr, CDMA_MUTEX_WRITE_LOCK);

    cdma_read(&MM,slob_addr,sizeof(MM));
    slob_bf_pool = (struct buffer_pool* )MM.h_slob_bf_pool;

    /* look for a block of the size greater or equal to the required size. */
    cdma_read(&free_b_addr,MM.head_free_blocks_addr + i*sizeof(uint64_t),
              sizeof(free_b_addr));
    if(0 != free_b_addr)
    {
        cdma_read(&free_b,free_b_addr,sizeof(free_b));
    }
    while ( free_b_addr && (free_b.end - free_b.base) < size )
    {
	free_b_addr = free_b.next_addr;
	if(0 != free_b_addr)
	{
	    cdma_read(&free_b,free_b_addr,sizeof(free_b));
	}
    }

    /* If such block is not found */
    if ( free_b_addr == 0)
    {
	cdma_mutex_lock_release(slob_addr);
        return 0LL;
    }

    hold_base =  free_b.base;
    hold_end = hold_base + size;

    /* init a new busy block */

    if (create_new_block(&MM,hold_base, size,
                                 &new_busy_b_addr) != 0)
    {
	cdma_mutex_lock_release(slob_addr);
        return 0LL;
    }
    /* calls Update routine to update a lists of free blocks */
    if ( cut_free (&MM, hold_base, hold_end ) != 0 )
    {
        //fsl_free(p_new_busy_b);
        buff_pool_put(slob_bf_pool,new_busy_b_addr);
        cdma_mutex_lock_release(slob_addr);
        return 0LL;
    }

    /* Decreasing the allocated memory size from free memory size */
    MM.free_mem_size -= size;
    /* insert the new busy block into the list of busy blocks */
    add_busy (&MM,new_busy_b_addr);
    cdma_write(slob_addr,&MM,sizeof(MM));
    cdma_mutex_lock_release(slob_addr);
    return (hold_base);
}

/*****************************************************************************/
uint64_t slob_put(uint64_t* slob, const uint64_t base)
{
    t_MM        MM = {0};
    t_slob_block busy_b = {0};
    uint64_t    size;
    uint64_t   busy_b_addr = 0, prev_busy_b_addr = 0;
    uint64_t slob_addr = 0;
    struct buffer_pool *slob_bf_pool = NULL;



    ASSERT_COND(slob);
    slob_addr = *slob;



    /* Look for a busy block that have the given base value.
     * That block will be returned back to the memory.
     */
    cdma_mutex_lock_take(slob_addr, CDMA_MUTEX_WRITE_LOCK);

    cdma_read(&MM,slob_addr,sizeof(MM));
    slob_bf_pool  = (struct buffer_pool *)MM.h_slob_bf_pool;

    busy_b_addr = MM.head_busy_blocks_addr;
    if(0 != busy_b_addr)
    {
        cdma_read(&busy_b,busy_b_addr, sizeof(busy_b));
    }
    while ( busy_b_addr && base != busy_b.base )
    {
        prev_busy_b_addr = busy_b_addr;
        busy_b_addr = busy_b.next_addr;
        if(0 != busy_b_addr)
        {
            cdma_read(&busy_b,busy_b_addr, sizeof(busy_b));
        }
    }

    if ( 0 == busy_b_addr)
    {
	cdma_mutex_lock_release(slob_addr);
        return 0LL;
    }

    if ( add_free(&MM, busy_b.base, busy_b.end ) != 0 )
    {
	cdma_mutex_lock_release(slob_addr);
        return 0LL;
    }

    /* removes a busy block form the list of busy blocks */
    if(prev_busy_b_addr)
    {
	cdma_write(prev_busy_b_addr+offsetof(t_slob_block,next_addr),&busy_b.next_addr,
	           sizeof(busy_b.next_addr));
    }
    else
    {
        MM.head_busy_blocks_addr = busy_b.next_addr;
    }

    size = busy_b.end - busy_b.base;

    /* Adding the deallocated memory size to free memory size */
    MM.free_mem_size += size;
    //fsl_free(p_busy_b);
    buff_pool_put(slob_bf_pool,busy_b_addr);
    cdma_write(slob_addr,&MM,sizeof(MM));
    cdma_mutex_lock_release(slob_addr);
    return (size);
}
#if 0
/*****************************************************************************/
uint64_t slob_get_base(uint64_t* slob)
{
    t_MM        MM = {0};
    t_mem_block mem_block = {0};

    ASSERT_COND(*slob);

    cdma_mutex_lock_take(*slob, CDMA_MUTEX_READ_LOCK);
    cdma_read(&MM,*slob,sizeof(MM));
    cdma_read(&mem_block,MM.head_mem_blocks,sizeof(mem_block));
    cdma_mutex_lock_release(*slob);
    return  mem_block.base;
}

/*****************************************************************************/
int slob_in_range(uint64_t* slob, const uint64_t addr)
{
    t_MM      MM = {0};
    t_mem_block mem_block = {0};

    ASSERT_COND(*slob);

    cdma_mutex_lock_take(*slob, CDMA_MUTEX_READ_LOCK);
    cdma_read(&MM,*slob,sizeof(MM));
    cdma_read(&mem_block,MM.head_mem_blocks,sizeof(mem_block));
    cdma_mutex_lock_release(*slob);

    if ((addr >= mem_block.base) && (addr < mem_block.end))
        return 1;
    else
        return 0;
}

/*****************************************************************************/
uint64_t slob_get_free_mem_size(uint64_t* slob)
{
    t_MM       MM = {0};

    ASSERT_COND(*slob);

    cdma_mutex_lock_take(*slob, CDMA_MUTEX_READ_LOCK);
    cdma_read(&MM,*slob,sizeof(MM));
    cdma_mutex_lock_release(*slob);
    return MM.free_mem_size;
}

/*****************************************************************************/
void slob_dump(uint64_t* slob)
{
    t_MM        MM = {0};
    t_slob_block block = {0};
    uint64_t busy_b_addr = 0,free_b_addr = 0;
    int          i;

    ASSERT_COND(*slob);

    cdma_read(&MM,*slob,sizeof(MM));

    busy_b_addr = MM.head_busy_blocks_addr;

    pr_debug("list of busy blocks:\n");
    while (busy_b_addr)
    {
	cdma_read(&block,busy_b_addr,sizeof(block));
        pr_debug("\t0x%llx: ( b=0x%llx, e=0x%llx)\n", busy_b_addr, block.base, block.end );
        busy_b_addr = block.next_addr;
    }

    pr_debug("\n_lists of free blocks according to alignment:\n");
    for (i=0; i <= MM_MAX_ALIGNMENT; i++)
    {
        pr_debug("%d alignment:\n", (0x1 << i));
        cdma_read(&free_b_addr,MM.head_free_blocks_addr + i*sizeof(uint64_t),
                  sizeof(free_b_addr));
        while (free_b_addr)
        {
            cdma_read(&block,free_b_addr,sizeof(block));
            pr_debug("\t0x%llx: (b=0x%llx, e=0x%llx)\n", free_b_addr, block.base, block.end);
            free_b_addr = block.next_addr;
        }
        pr_debug("\n");
    }
}
#endif

__END_COLD_CODE
