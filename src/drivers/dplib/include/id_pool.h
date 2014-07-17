/**************************************************************************//**
@File		id_pool.h

@Description	This file contains the AIOP SW ID pool interface.

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
int id_pool_init(uint16_t num_of_ids,
			 uint16_t buffer_pool_id,
			 uint64_t *ext_id_pool_address);

/*************************************************************************//**
@Function	get_id

@Description	General function for pulling an id out of ID pool.

@Param[in]	ext_id_pool_address - External id pool address.

@Param[out]	id - id pulled from the pool.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		ENOSPC - Pool out of range, no more ID's are available.

@Cautions	In this function the task yields.
*//***************************************************************************/
int get_id(uint64_t ext_id_pool_address, uint8_t *id);


/*************************************************************************//**
@Function	release_id

@Description	General function for putting an id back to ID pool.

@Param[in]	ext_id_pool_address - External id pool address.

@Param[in]	id - id pushed into the pool.

@Return		0 on Success, or negative value on error.

@Retval		0 - Success
@Retval		ENAVAIL - All ID's already returned to pool.

@Cautions	In this function the task yields.
*//***************************************************************************/
int release_id(uint8_t id, uint64_t ext_id_pool_address);

/** @} */ /* end of ID_POOL_Functions */

/** @} */ /* end of ID_POOL */

#endif /* __ID_POOL_H_ */
