/**************************************************************************//**
@File		fsl_system.h

@Description	This file contains the AIOP SW system interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __FSL_SYSTEM_H_
#define __FSL_SYSTEM_H_

#include "general.h"
#include "fsl_cdma.h"

/**************************************************************************//**
@Group	FSL_SYSTEM FSL_AIOP_System

@Description	AIOP System API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		FSL_SYSTEM_MACROS System Macros
@{
*//***************************************************************************/
/**************************************************************************//**
@Group FSL_SYS_ID_POOL_LENGTH System ID Pools Length
@{
*//***************************************************************************/
/** Parser Profile ID pool length */
#define SYS_PRPID_POOL_LENGTH	65

/** Key ID pool length */
#define SYS_KEYID_POOL_LENGTH	257

/** @} */ /* end of SYS_ID_POOL_LENGTH */

/**************************************************************************//**
@Group FSL_SYS_ID_POOL_CREATE_STATUS System ID Pool Create Status
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

/** @} */ /* end of SYS_POOL_CREATE_STATUS */

/** @} */ /* end of SYSTEM_MACROS */


/**************************************************************************//**
@Group		FSL_SYSTEM_Functions System Functions

@Description	Freescale AIOP SYSTEM Functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	sys_ctlu_prpid_pool_create

@Description	Creation and Initialization of system Parser Profile ID pool
		for the Parser.

		Implicitly updated values in AIOP System global parameters:
		ext_prpid_pool_address

@Param[in]	buffer_pool_id - BMan pool ID used for the Acquire Context
		Memory for Parser Profile ID pool.

@Param[in]	buffer_size - Size of the Context memory buffer, must match the
		size of the BMan buffer being acquired using the
		buffer_pool_id. Must be at least 65 bytes.

@Return		Status - please refer to \ref FSL_SYS_ID_POOL_CREATE_STATUS.

@Cautions	Should be called only once per CTLU.
		In this function the task yields.
*//***************************************************************************/
int32_t sys_ctlu_prpid_pool_create(uint16_t buffer_pool_id,
						uint32_t buffer_size);

/*************************************************************************//**
@Function	sys_ctlu_keyid_pool_create

@Description	Creation and Initialization of system Key ID pool for CTLU.

		Implicitly updated values in AIOP System global parameters:
		ext_keyid_pool_address

@Param[in]	buffer_pool_id - BMan pool ID used for the Acquire Context
		Memory for Parser Profile ID pool.

@Param[in]	buffer_size - Size of the Context memory buffer, must match the
		size of the BMan buffer being acquired using the
		buffer_pool_id. Must be at least 257 bytes.

@Return		Status - please refer to \ref FSL_SYS_ID_POOL_CREATE_STATUS.

@Cautions	Should be called only once per CTLU.
		Retries to acquire CDMA context buffer until success.
		In this function the task yields.
*//***************************************************************************/
int32_t sys_ctlu_keyid_pool_create(uint16_t buffer_pool_id,
						uint32_t buffer_size);

/** @} */ /* end of SYSTEM_Functions */

/** @} */ /* end of SYSTEM */

#endif /* __FSL_SYSTEM_H_ */
