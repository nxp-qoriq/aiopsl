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
@Group		ID_POOL_MACROS

@Description	AIOP ID Pool Macros

@{
*//***************************************************************************/
/**************************************************************************//**
@Group	INIT_ID_POOL_STATUS Status returned from id_pool_init function
@{
*//***************************************************************************/
/** ID pool was initialized successfully. */
#define ID_POOL_INIT_STATUS_SUCCESS				0x00000000
/** ID pool initialization failure.
 * Buffer was not acquired due to CDMA error. */
#define ID_POOL_INIT_STATUS_CDMA_ERR_NO_BUFFER_ACQUIRED		0x80000001
/** ID pool initialization failure.
 * Write to context buffer failed but buffer was not released due to
 * CDMA error.*/
#define ID_POOL_INIT_STATUS_CDMA_WR_ERR_BUF_NOT_RELEASED	0x80000002
/** ID pool initialization failure.
 * Write to context buffer failed but buffer was released */
#define ID_POOL_INIT_STATUS_CDMA_WR_ERR_BUF_RELEASED		0x80000003

/** @} */ /* end of INIT_ID_POOL_STATUS */

/**************************************************************************//**
@Group	GET_ID_STATUS Status returned from get_id function
@{
*//***************************************************************************/

/** Command successful. ID was pulled from pool */
#define GET_ID_STATUS_SUCCESS				0x00000000
/** Command failed. ID was not fetched from pool due to CDMA write error */
#define GET_ID_STATUS_CDMA_WR_MUTEX_FAILURE		0x80000001
/** Command failed. ID was not fetched from pool due to pool out of range */
#define GET_ID_STATUS_POOL_OUT_OF_RANGE			0x80000002
/** Command failed. ID was not fetched from pool due to CDMA read error */
#define GET_ID_STATUS_CDMA_RD_FAILURE			0x80000003
/** Command failed. ID was not fetched from pool due to CDMA read with
 * mutex error */
#define GET_ID_STATUS_CDMA_RD_MUTEX_FAILURE		0x80000004

/** @} */ /* end of GET_ID_STATUS */

/**************************************************************************//**
@Group	RELEASE_ID_STATUS Status returned from release_id function
@{
*//***************************************************************************/

/** Command successful. ID was returned to pool */
#define RELEASE_ID_STATUS_SUCCESS			0x00000000
/** Command failed. ID was not returned to pool due to CDMA write error */
#define RELEASE_ID_STATUS_CDMA_WR_FAILURE		0x80000001
/** Command failed. ID was not returned to pool due to pool out of range */
#define RELEASE_ID_STATUS_POOL_OUT_OF_RANGE		0x80000002
/** Command failed. ID was not returned to pool due to CDMA read with
 * mutex error */
#define RELEASE_ID_STATUS_CDMA_RD_MUTEX_FAILURE		0x80000003
/** Command failed. ID was not returned to pool due to CDMA write with
 * mutex error */
#define RELEASE_ID_STATUS_CDMA_WR_MUTEX_FAILURE		0x80000004

/** @} */ /* end of RELEASE_ID_STATUS */

/** @} */ /* end of ID_POOL_MACROS */

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

@Return		Status - Success or Failure. (\ref INIT_ID_POOL_STATUS).

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int32_t id_pool_init(uint16_t num_of_ids,
			 uint16_t buffer_pool_id,
			 uint64_t *ext_id_pool_address)
{
	int i;
	uint64_t int_id_pool_address;
	uint16_t fill_ids;
	uint16_t num_of_ids_and_index;
	uint8_t num_of_writes = 0;
	uint8_t pool[64];
	

	/* Acquire buffer for the pool */
	if (cdma_acquire_context_memory(buffer_pool_id, &int_id_pool_address)) {
		/* Todo return CDMA status with Accell ID? -*/
		return ID_POOL_INIT_STATUS_CDMA_ERR_NO_BUFFER_ACQUIRED;
	}
	/* store the address in the global parameter */
	*ext_id_pool_address = int_id_pool_address;

	num_of_ids_and_index = (num_of_ids + 1);
	
	while (num_of_ids_and_index) {
		/* Initialize pool in local memory */
		fill_ids = (num_of_ids_and_index < 64) ?
					num_of_ids_and_index : 64;
		for (i = 0; i < fill_ids; i++)
			pool[i] = (uint8_t)((num_of_writes<<6) + i - 1);
		if (num_of_writes == 0) 
			pool[0] = 0;
		num_of_writes++;
		num_of_ids_and_index = num_of_ids_and_index - fill_ids;
		/* Write pool to external memory */
		if (cdma_write((int_id_pool_address + (num_of_writes<<6)), pool,
								fill_ids)) {
			/* In case cdma_write failed, need to release
			 * the buffer */
			if (cdma_release_context_memory(int_id_pool_address)) {
				/* Todo return CDMA status with Accell ID? */
				return
			       ID_POOL_INIT_STATUS_CDMA_WR_ERR_BUF_NOT_RELEASED;
			}
			/* Todo return CDMA status with Accell ID? */
			return ID_POOL_INIT_STATUS_CDMA_WR_ERR_BUF_RELEASED;
		}
	} 
	return ID_POOL_INIT_STATUS_SUCCESS;
}
/*************************************************************************//**
@Function	get_id

@Description	General function for pulling an id out of ID pool.

@Param[in]	ext_id_pool_address - External id pool address.

@Param[in]	num_of_ids - Number of IDs to be initialized in the pool.

@Param[out]	id - id pulled from the pool.

@Return		Status - Success or Failure. (\ref GET_ID_STATUS).

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int32_t get_id(uint64_t ext_id_pool_address, uint16_t num_of_ids,
			uint8_t *id)
{
	int32_t status;
	uint64_t int_id_pool_address;
	int8_t index;

	int_id_pool_address = ext_id_pool_address;

	/* Read and lock id pool index */
	status = (cdma_read_with_mutex(int_id_pool_address,
				     CDMA_PREDMA_MUTEX_WRITE_LOCK,
				     &index,
				     1));

	if (status == CDMA_SUCCESS) {
		if (index < num_of_ids) {
			/* Pull id from the pool */
			status = (cdma_read(id,
					(uint64_t)(int_id_pool_address+index+1),
					1));
			if (status == CDMA_SUCCESS) {
				/* Update index, write it back and 
				 * release mutex */
				index++;
				if (cdma_write_with_mutex(int_id_pool_address,
					CDMA_POSTDMA_MUTEX_RM_BIT, &index, 1))
					/* In case of write error, CDMA SR will
					 * try to release mutex if needed and
					 * return status (???)
					 * TODO CDMA status */
					return 
					GET_ID_STATUS_CDMA_WR_MUTEX_FAILURE;
				else
					return GET_ID_STATUS_SUCCESS;
				} else { /* CDMA read error */
					/* Release mutex */
					cdma_mutex_lock_release
							(int_id_pool_address);
					/* TODO status */
					return GET_ID_STATUS_CDMA_RD_FAILURE;
				} 
		} else { /* Pool out of range */
			/* Release mutex */
			cdma_mutex_lock_release(int_id_pool_address);
			/* TODO status */
			return GET_ID_STATUS_POOL_OUT_OF_RANGE;
		}
	} else { /* CDMA read with mutex error */
		/* In case of read error, CDMA SR will try to release mutex
		 * if needed and return status (???)
		 * TODO CDMA status */
		return GET_ID_STATUS_CDMA_RD_MUTEX_FAILURE;	
	}
}

/*************************************************************************//**
@Function	release_id

@Description	General function for putting an id back to ID pool.

@Param[in]	ext_id_pool_address - External id pool address.

@Param[in]	id - id pushed into the pool.

@Return		Status - Success or Failure. (\ref RELEASE_ID_STATUS).

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int32_t release_id(uint8_t id, uint64_t ext_id_pool_address)
{
	int32_t status;
	uint64_t int_id_pool_address;
	int8_t index;
	
	int_id_pool_address = ext_id_pool_address;

	/* Read and lock id pool index */
	status = (cdma_read_with_mutex(int_id_pool_address,
				     CDMA_PREDMA_MUTEX_WRITE_LOCK,
				     &index,
				     1));

	if (status == CDMA_SUCCESS) {
		index--;
		if (index >= 0) {
			/* Return id to the pool */
			status = (cdma_write((int_id_pool_address+index+1),
					      &id,
					      1));
			if (status == CDMA_SUCCESS) {
				/* Update index, write it back and 
				 * release mutex */
				if (cdma_write_with_mutex(int_id_pool_address,
					CDMA_POSTDMA_MUTEX_RM_BIT, &index, 1))
					/* In case of write error, CDMA SR will
					 * try to release mutex if needed and
					 * return status (???)
					 * TODO CDMA status */
					return
					RELEASE_ID_STATUS_CDMA_WR_MUTEX_FAILURE;
				else
					return RELEASE_ID_STATUS_SUCCESS;
			} else { /* CDMA write error */
				/* Release mutex */
				if (cdma_mutex_lock_release
						(int_id_pool_address))
					return status; /* TODO */
				return RELEASE_ID_STATUS_CDMA_WR_FAILURE;
			} 
		} else { /* Pool out of range */
			if (cdma_mutex_lock_release(int_id_pool_address))
				return status; /* TODO */
			else
				return RELEASE_ID_STATUS_POOL_OUT_OF_RANGE;
		}
	} else { /* CDMA read with mutex error */
		/* In case of read error, CDMA SR will try to release mutex
		 * if needed and return status (???)
		 * TODO CDMA status */
		return RELEASE_ID_STATUS_CDMA_RD_MUTEX_FAILURE;	
	}
}	

/** @} */ /* end of ID_POOL_Functions */

/** @} */ /* end of ID_POOL */

#endif /* __ID_POOL_H_ */
