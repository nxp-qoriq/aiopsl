/**************************************************************************//**
@File		id_pool.h

@Description	This file contains the AIOP SW ID pool interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __ID_POOL_H_
#define __ID_POOL_H_

#include "general.h"
#include "fsl_cdma.h"

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
#define GET_ID_STATUS_CDMA_WR_FAILURE			0x80000001
/** Command failed. ID was not fetched from pool due to pool out of range */
#define GET_ID_STATUS_POOL_OUT_OF_RANGE			0x80000002
/** Command failed. ID was not fetched from pool due to CDMA read error */
#define GET_ID_STATUS_CDMA_RD_FAILURE			0x80000003

/** @} */ /* end of GET_ID_STATUS */

/**************************************************************************//**
@Group	RELEASE_ID_STATUS Status returned from release_id function
@{
*//***************************************************************************/

/** Command successful. ID was returned to pool */
#define RELEASE_ID_STATUS_SUCCESS			0x80000000
/** Command failed. ID was not returned to pool due to CDMA write error */
#define RELEASE_ID_STATUS_CDMA_WR_FAILURE		0x80000001
/** Command failed. ID was not returned to pool due to pool out of range */
#define RELEASE_ID_STATUS_POOL_OUT_OF_RANGE		0x80000002
/** Command failed. ID was not returned to pool due to CDMA read error */
#define RELEASE_ID_STATUS_CDMA_RD_FAILURE		0x80000003

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

@Param[in]	pool - local array to be initialized.

@Param[in]	length - id pool length.

@Param[in]	buffer_pool_id - BMan pool ID used for the Acquire Context
		Memory for Parser Profile ID pool.

@Param[in]	buffer_size - Size of the Context memory buffer, must match the
		size of the BMan buffer being acquired using the
		buffer_pool_id.

@Param[out]	ext_id_pool_address - External id pool address.

@Return		Status - Success or Failure. (\ref INIT_ID_POOL_STATUS).

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int32_t id_pool_init(int8_t *pool,
			 int8_t length,
			 uint16_t buffer_pool_id,
			 uint32_t buffer_size,
			 uint64_t *ext_id_pool_address)
{
	int i;

	/* Acquire buffer for the pool */
	if (cdma_acquire_context_memory(buffer_size, buffer_pool_id,
					ext_id_pool_address)) {
		/* Todo return CDMA status with Accell ID? -*/
		return ID_POOL_INIT_STATUS_CDMA_ERR_NO_BUFFER_ACQUIRED;
	}

	/* Initialize pool in local memory */
	pool[0] = length - 1;
	for (i = length; i > 1; i--)
		pool[i-1] = (int8_t)(length - i);

	/* Write pool to external memory */
	if (cdma_write(*ext_id_pool_address, pool, length)) {
		/* In case cdma_write failed, need to release the buffer */
		if (cdma_release_context_memory(*ext_id_pool_address)) {
			/* Todo return CDMA status with Accell ID? */
			return ID_POOL_INIT_STATUS_CDMA_WR_ERR_BUF_NOT_RELEASED;
		}
		/* Todo return CDMA status with Accell ID? */
		return ID_POOL_INIT_STATUS_CDMA_WR_ERR_BUF_RELEASED;
	}
	return ID_POOL_INIT_STATUS_SUCCESS;
}

/*************************************************************************//**
@Function	get_id

@Description	General function for pulling an id out of ID pool.

@Param[in]	pool - local array to hold a copy of the external pool.

@Param[in]	length - id pool length.

@Param[in]	ext_id_pool_address - External id pool address.

@Param[out]	id - id pulled from the pool.

@Return		Status - Success or Failure. (\ref GET_ID_STATUS).

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int32_t get_id(uint8_t *pool, uint16_t length,
			uint64_t ext_id_pool_address, uint8_t *id)
{
	int index;
	int32_t status;

	/* Read id pool to local memory */
	status = (cdma_read_with_mutex(ext_id_pool_address,
				     CDMA_PREDMA_MUTEX_WRITE_LOCK,
				     pool,
				     length));

	if (status == CDMA_SUCCESS) {
		index = pool[0];
		if (index > 1) {
			/* Pull id from the pool and update the pointer */
			*id = pool[index];
			(pool[0])--;
			status = GET_ID_STATUS_SUCCESS;
		} else {
			status = GET_ID_STATUS_POOL_OUT_OF_RANGE;
		}
		/* Write id pool from local memory, release mutex */
		if (cdma_write_with_mutex(ext_id_pool_address,
			CDMA_POSTDMA_MUTEX_RM_BIT, pool, 1))
			/* In case of write error, CDMA SR will try to
			 * release mutex if needed and return status.
			 * TODO CDMA status */
			return GET_ID_STATUS_CDMA_WR_FAILURE;
		else
			return status;
	} else {	/* CDMA read error */
		/* In case of read error, CDMA SR will try to release mutex
		 * if needed and return status.
		 * TODO CDMA status */
		return GET_ID_STATUS_CDMA_RD_FAILURE;
	}
}


/*************************************************************************//**
@Function	release_id

@Description	General function for pulling an id out of ID pool.

@Param[in]	pool - local array to hold a copy of the external pool.

@Param[in]	length - id pool length.

@Param[in]	ext_id_pool_address - External id pool address.

@Param[out]	id - id pulled from the pool.

@Return		Status - Success or Failure. (\ref RELEASE_ID_STATUS).

@Cautions	In this function the task yields.
*//***************************************************************************/
inline int32_t release_id(uint8_t id, uint8_t *pool,
			  uint16_t length, uint64_t ext_id_pool_address)
{
	int index;
	int32_t status;

	status = cdma_read_with_mutex(ext_id_pool_address,
				     CDMA_PREDMA_MUTEX_WRITE_LOCK,
				     pool,
				     length);
	if (status == CDMA_SUCCESS) {
		index = pool[0];
		if (index < (length - 1)) {
			pool[0]++;
			pool[index] = id;
			status = RELEASE_ID_STATUS_SUCCESS;
		} else {
			/* Todo return CDMA status with Accell ID? */
			status = RELEASE_ID_STATUS_POOL_OUT_OF_RANGE;
		}

		if (cdma_write_with_mutex(ext_id_pool_address,
			CDMA_POSTDMA_MUTEX_RM_BIT, pool, length))
			/* In case of write error, CDMA SR will try to
			 * release mutex if needed and return status.
			 * TODO CDMA status */
			return GET_ID_STATUS_CDMA_WR_FAILURE;
		else
			return status;;
	} else {	/* CDMA read error */
		/* In case of read error, CDMA SR will try to release mutex
		 * if needed and return status.
		 * TODO CDMA status */
		return GET_ID_STATUS_CDMA_RD_FAILURE;
	}
}

/** @} */ /* end of ID_POOL_Functions */

/** @} */ /* end of ID_POOL */

#endif /* __ID_POOL_H_ */
