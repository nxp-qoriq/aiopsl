/**************************************************************************//**
@File		id_pool.h

@Description	This file contains the AIOP SW ID pool interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __ID_POOL_H_
#define __ID_POOL_H_

#include "general.h"
#include "dplib/fsl_cdma.h"
#include "cdma.h"

/**************************************************************************//**
@Group	ID_POOL ID Pool (Internal)

@Description	AIOP ID Pool API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		ID_POOL_Functions

@Description	AIOP ID Pool Functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	id_pool_init

@Description	General function for Creation and Initialization of ID pool.

@Param[in]	num_of_ids - Number of IDs to be initialized in the pool.

@Param[in]	buffer_pool_id - BMan pool ID used for the Acquire Context
		Memory for Parser Profile ID pool.

@Param[out]	ext_id_pool_address - External id pool address.

@Return		0 on Success.

@Cautions	In this function the task yields.
 	 	This function may result in a fatal error.
*//***************************************************************************/
inline int32_t id_pool_init(uint16_t num_of_ids,
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
/*************************************************************************//**
@Function	get_id

@Description	General function for pulling an id out of ID pool.

@Param[in]	ext_id_pool_address - External id pool address.

@Param[in]	num_of_ids - Number of IDs to be initialized in the pool.

@Param[out]	id - id pulled from the pool.

@Return		0 on Success, or negative value on error.

@Retval		0 – Success
@Retval		ENOSPC - Pool out of range, no more ID's are available.

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int32_t get_id(uint64_t ext_id_pool_address, uint8_t *id)
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

/*************************************************************************//**
@Function	release_id

@Description	General function for putting an id back to ID pool.

@Param[in]	ext_id_pool_address - External id pool address.

@Param[in]	id - id pushed into the pool.

@Return		0 on Success, or negative value on error.

@Retval		0 – Success
@Retval		ENAVAIL - All ID's already returned to pool.

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int32_t release_id(uint8_t id, uint64_t ext_id_pool_address)
{

	uint8_t last_id_and_index[2];

	/* Read and lock id pool index */
	cdma_read_with_mutex(ext_id_pool_address,
				CDMA_PREDMA_MUTEX_WRITE_LOCK,
				last_id_and_index,
				2);

	last_id_and_index[1]--;
	if (last_id_and_index[1] >= 0) {
		/* Return id to the pool */
		cdma_write((ext_id_pool_address+last_id_and_index[1]+2),
				&id,
				1);
		/* Update index, write it back and release mutex */
		cdma_write_with_mutex(ext_id_pool_address,
					CDMA_POSTDMA_MUTEX_RM_BIT,
					last_id_and_index, 2);
		return 0;
	} else { /* Pool out of range */
		cdma_mutex_lock_release(ext_id_pool_address);
		return -ENAVAIL;
	}
}

/** @} */ /* end of ID_POOL_Functions */

/** @} */ /* end of ID_POOL */

#endif /* __ID_POOL_H_ */
