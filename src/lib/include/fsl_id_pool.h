/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File		fsl_id_pool.h

@Description	This file contains the AIOP SW ID pool interface.

*//***************************************************************************/

#ifndef __FSL_ID_POOL_H_
#define __FSL_ID_POOL_H_

#include "general.h"
#include "fsl_cdma.h"


/**************************************************************************//**
@Group	ID_POOL ID Pool (Internal)

@Description	AIOP ID Pool API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group ID_POOL_STATUSES_DEFINES ID_POOL Statuses Defines
@{
*//***************************************************************************/
/** ID POOL initialization failed due to buffer depletion failure */
#define SYSTEM_INIT_BUFFER_DEPLETION_FAILURE	0x55

/** @} */ /* end of ID_POOL_STATUSES_DEFINES */

 /**************************************************************************//**
  @Group	ID_POOL_Enumerations ID_POOL Enumerations

  @Description	ID_POOL Enumerations

  @{
 *//***************************************************************************/

/**************************************************************************//**
  @enum id_pool_functions

  @Description	AIOP ID_POOL Functions enumeration.

  @{
 *//***************************************************************************/
 enum id_pool_function_identifier {
	 ID_POOL_INIT = 0,
	 GET_ID,
	 RELEASE_ID
 };

 /** @} */ /* end of group ID_POOL_Enumerations */

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

@Return		None.

@Cautions	In this function the task yields.
 	 	This function may result in a fatal error.
*//***************************************************************************/
void id_pool_init(uint16_t num_of_ids,
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

/**************************************************************************//**
@Function	id_pool_exception_handler

@Description	Handler for the error status returned from the id_pool API
		functions.

@Param[in]	file_path - The path of the file in which the error occurred.
@Param[in]	func_id - The function in which the error occurred.
@Param[in]	line - The line in which the error occurred.
@Param[in]	status - Status to be handled be this function.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void id_pool_exception_handler(enum id_pool_function_identifier func_id,
			     uint32_t line,
			     int32_t status);


/** @} */ /* end of ID_POOL_Functions */

/** @} */ /* end of ID_POOL */

#endif /* __FSL_ID_POOL_H_ */
