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
 @Param[in]     h_boot_mem_mng - Handle to boot memory manage.

 @Return        0 - if success, a  non-zero value in case of error
*//***************************************************************************/
int buffer_pool_create(struct buffer_pool    *bf_pool,
                      const uint32_t 	    bf_pool_id,
                      const uint32_t        num_buffs,
                      const uint16_t        buff_size,
                      void*           h_boot_mem_mng);

/**************************************************************************//**
 @Function      get_buff

 @Description   Returns a 64b address of a free buffer from the pool.

 @Param[in]     bf_pool - Object of buffer pool, returned by buffer_pool_create
 @Param[out]    buff_addr Address of a buffer.

 @Return        0 - if success, a  non-zero value in case of error
*//***************************************************************************/
int get_buff(struct buffer_pool *bf_pool,uint64_t* buff_addr );

/**************************************************************************//**
 @Function      put_buff

 @Description   Returns a  previously returned buffer to the pool.

 @Param[in]     bf_pool - Object of buffer pool, returned by buffer_pool_create
 @Param[out]    buff_addr Address of a buffer to be returned to the pool.

 @Return        0 - if success, a  non-zero value in case of error
*//***************************************************************************/
int put_buff(struct buffer_pool  *bf_pool, const uint64_t buffer_addr);

#endif
