/**************************************************************************//**
@File		system.h

@Description	This file contains the AIOP SW system interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include "general.h"
#include "dplib/fsl_cdma.h"

/**************************************************************************//**
@Group	SYSTEM AIOP_System

@Description	AIOP System API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		SYSTEM_MACROS System Macros
@{
*//***************************************************************************/
/**************************************************************************//**
@Group SYS_ID_POOL_LENGTH System ID Pools Length
@{
*//***************************************************************************/
/** Parser Profile ID pool length */
#define SYS_NUM_OF_PRPIDS	64

/** Key ID pool length */
#define SYS_NUM_OF_KEYIDS	256

/** @} */ /* end of SYS_ID_POOL_LENGTH */

/**************************************************************************//**
@Group SYS_ID_POOL_CREATE_STATUS System ID Pool Create Status
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
@Group		SYSTEM_Functions System Functions

@Description	AIOP SYSTEM Functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	sys_prpid_pool_create

@Description	Creation and Initialization of system Parser Profile ID pool
		for the Parser.

		Implicitly updated values in AIOP System global parameters:
		ext_prpid_pool_address

@Return		Status - please refer to \ref SYS_ID_POOL_CREATE_STATUS.

@Cautions	Should be called only once per CTLU.
		In this function the task yields.
*//***************************************************************************/
int32_t sys_prpid_pool_create(void);

/*************************************************************************//**
@Function	sys_keyid_pool_create

@Description	Creation and Initialization of system Key ID pool for CTLU.

		Implicitly updated values in AIOP System global parameters:
		ext_keyid_pool_address

@Return		Status - please refer to \ref SYS_ID_POOL_CREATE_STATUS.

@Cautions	Should be called only once per CTLU.
		In this function the task yields.
*//***************************************************************************/
int32_t sys_keyid_pool_create(void);

/*************************************************************************//**
@Function	aiop_sl_init

@Description	AIOP Service Layer Initialization function.

		Implicitly updated values in AIOP System global parameters:
		ext_keyid_pool_address, ext_prpid_pool_address.

@Return		Status - please refer to \ref SYS_ID_POOL_CREATE_STATUS.

@Cautions	Should be called during system initialization.
		In this function the task yields.
*//***************************************************************************/
int32_t aiop_sl_init(void);

/*************************************************************************//**
@Function	aiop_sl_free

@Description	AIOP Service Layer Free Resources function.

@Cautions	Should be called before system shutdown.
		In this function the task yields.
*//***************************************************************************/
void aiop_sl_free(void);


/** @} */ /* end of SYSTEM_Functions */

/** @} */ /* end of SYSTEM */

#endif /* __SYSTEM_H_ */
