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
#ifndef __BUFFER_POOL_H
#define __BUFFER_POOL_H

#include "common/types.h"
#include "fsl_smp.h"
#include "fsl_icontext.h"

/**************************************************************************//*
 @Description    Buffer pool structure
*//***************************************************************************/
 struct buffer_pool {
	uint64_t  p_buffers_addr;    /* Start address for chunk of buffers */
	uint64_t  buffers_stack_addr;   /* Address of the stack of pointers to  buffers */
	uint32_t  buff_size;           /* Size of each data buffer */
	uint32_t  num_buffs;          /* Number of buffers in this pool*/
	uint32_t  current;            /* Current buffer */
	uint32_t  bf_pool_id;
};

 /* Put all function (execution code) into  dtext_vle section,aka __COLD_CODE */
 __START_COLD_CODE

/**************************************************************************//**
 *              Stack of 64 bit addresses to    	Buffers,
 * 		buffers                    		each one of buff_size
 		|--------------------------|  Address_1 |-------------------|
Current	----->	|  64b Address_1           |    	|    Buffer1        |
		|--------------------------|            |                   |
		|  64b Adress_2            |    	|                   |
		|--------------------------|  Address_2 |-------------------|
		|  64b Address_3           |            |    Buffer 2       |
		|--------------------------|            |		    |
		|			   |		|		    |
 * *//***************************************************************************/
/**************************************************************************//**
 @Function      buffer_pool_create

 @Description   Creates a buffer pool with given parameters.

 @Param[out]    bf_pool - Object of buffer pool that resides in shared ram.
                          It will be filled in this function.
 @Param[in]     bf_pool_id Id of this buffer pool.
 @Param[in]     num_buffs Number of buffers to be created.
 @Param[in]	buff_size - Size of each buffer in bytes.
 @param[in]     buff_alignment - Alignment that the buffers will be aligned to.
                                 Shouldn't be 0.
 @Param[in]     h_boot_mem_mng - Handle to boot memory manage.

 @Return        0 - if success, a  non-zero value in case of error
*//***************************************************************************/
int buff_pool_create(struct buffer_pool    *bf_pool,
                      const uint32_t 	    bf_pool_id,
                      const uint32_t        num_buffs,
                      const uint16_t        buff_size,
                      const uint16_t        buff_alignment,
                      void*           h_boot_mem_mng);

/**************************************************************************//**
 @Function      get_buff

 @Description   Returns a 64b address of a free buffer from the pool.

 @Param[in]     bf_pool - Object of buffer pool, returned by buffer_pool_create
 @Param[out]    buff_addr Address of a buffer.

 @Return        0 - if success, a  non-zero value in case of error
*//***************************************************************************/
int buff_pool_get(struct buffer_pool *bf_pool,uint64_t* buff_addr );

/**************************************************************************//**
 @Function      put_buff

 @Description   Returns a  previously returned buffer to the pool.

 @Param[in]     bf_pool - Object of buffer pool, returned by buffer_pool_create
 @Param[out]    buff_addr Address of a buffer to be returned to the pool.

 @Return        0 - if success, a  non-zero value in case of error
*//***************************************************************************/
int buff_pool_put(struct buffer_pool  *bf_pool, const uint64_t buffer_addr);

/**************************************************************************//**
 @Function      compute_num_buffers

 @Description   Returns a number of buffers that might be allocated from total_mem_size.

 @Param[in]	buff_size - Size of each buffer in bytes.
 @Param[in]     buff_alignment  Alignment that the buffers will aligned to.
                                Shouldn't be 0
 @Return        Number of buffers that fits into total_mem_size.
*//***************************************************************************/
uint32_t buff_pool_compute_num_buffers(const uint32_t  total_mem_size,
	                     const uint16_t  buff_size,
                             const uint16_t  buff_alignment);

__END_COLD_CODE
#endif
