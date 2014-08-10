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

/**************************************************************************//**
@File		id_pool.c

@Description	This file contains the AIOP SW ID pool implementation.

*//***************************************************************************/

#include "id_pool.h"

int id_pool_init(uint16_t num_of_ids,
			 uint16_t buffer_pool_id,
			 uint64_t *ext_id_pool_address)
{
	int i;
	uint64_t int_id_pool_address;
	uint16_t fill_ids;
	uint16_t pool_length_to_fill;
	uint8_t num_of_writes = 0;
	uint8_t last_id_in_pool = (uint8_t)(num_of_ids - 1);
	uint8_t pool[64];
	
	int32_t status;


	/* Acquire buffer for the pool */
	status = cdma_acquire_context_memory
			(buffer_pool_id, &int_id_pool_address);
	if (status < 0)
		handle_fatal_error((char *)status); /*TODO Fatal error*/

	/* store the address in the global parameter */
	*ext_id_pool_address = int_id_pool_address;

	/* pool_length_to_fill = num_of_ids + index + last_id_in_pool */
	pool_length_to_fill = (num_of_ids + 2);

	while (pool_length_to_fill) {
		/* Initialize pool in local memory */
		fill_ids = (pool_length_to_fill < 64) ?
						pool_length_to_fill : 64;
		for (i = 0; i < fill_ids; i++)
			pool[i] = (uint8_t)((num_of_writes<<6) + i - 2);
		if (num_of_writes == 0) {
			pool[0] = last_id_in_pool;
			pool[1] = 0; /* index */
		}
		pool_length_to_fill = pool_length_to_fill - fill_ids;
		/* Write pool to external memory */
		cdma_write((int_id_pool_address + (num_of_writes<<6)), pool,
								fill_ids);
		num_of_writes++;
	}
	return 0;
}


int get_id(uint64_t ext_id_pool_address, uint8_t *id)
{
	
	uint8_t last_id_and_index[2];

	/* Read and lock id pool num_of_IDs + index */
	cdma_read_with_mutex(ext_id_pool_address,
				CDMA_PREDMA_MUTEX_WRITE_LOCK,
				last_id_and_index,
				2);

	/* check if index < last_id */
	if (last_id_and_index[1] < (last_id_and_index[0])) {
		/* Pull id from the pool */
		cdma_read(id, 
			(uint64_t)(ext_id_pool_address+last_id_and_index[1]+2),
			1);
			/* Update index, write it back and release mutex */
		last_id_and_index[1]++;
		cdma_write_with_mutex(ext_id_pool_address,
				CDMA_POSTDMA_MUTEX_RM_BIT,
				last_id_and_index, 2);
		return 0;
	} else { /* Pool out of range */
		/* Release mutex */
		cdma_mutex_lock_release(ext_id_pool_address);
		return -ENOSPC;
	}
}


int release_id(uint8_t id, uint64_t ext_id_pool_address)
{

	uint8_t last_id_and_index[2];

	/* Read and lock id pool index */
	cdma_read_with_mutex(ext_id_pool_address,
				CDMA_PREDMA_MUTEX_WRITE_LOCK,
				last_id_and_index,
				2);

	if (last_id_and_index[1] == 0) { /* Pool out of range */
		cdma_mutex_lock_release(ext_id_pool_address);
		return -ENAVAIL;
	} else {
		last_id_and_index[1]--;
		/* Return id to the pool */
		cdma_write((ext_id_pool_address+last_id_and_index[1]+2),
				&id,
				1);
		/* Update index, write it back and release mutex */
		cdma_write_with_mutex(ext_id_pool_address,
					CDMA_POSTDMA_MUTEX_RM_BIT,
					last_id_and_index, 2);
		return 0;
	}
}
