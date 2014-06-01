/**************************************************************************//**
@File		system.h

@Description	This file contains the AIOP SW system interface.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include "general.h"
#include "dplib/fsl_cdma.h"

#define __PROFILE_SRAM __declspec(section ".psram_data")
#pragma section RW ".psram_data" ".psram_bss"

/**************************************************************************//**
@Group	SYSTEM AIOP SYSTEM

@Description	AIOP System API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		SYSTEM_MACROS System Macros
@{
*//***************************************************************************/

/**************************************************************************//**
@Group SYS_STORAGE_PROFILE_DEFINES Storage Profile Defines
@{
*//***************************************************************************/
/** SP mode bits 1st byte - PTAR mask */
#define mode_bits1_PTAR_MASK  0x80
/** SP mode bits 1st byte - SGHR mask */
#define mode_bits1_SGHR_MASK  0x10
/** SP mode bits 1st byte - ASAR mask */
#define mode_bits1_ASAR_MASK  0x0f

/** SP mode bits 2nd byte - BS mask */
#define mode_bits2_BS_MASK  0x80
/** SP mode bits 2nd byte - FF mask */
#define mode_bits2_FF_MASK  0x30
/** SP mode bits 2nd byte - VA mask */
#define mode_bits2_VA_MASK  0x04
/** SP mode bits 2nd byte - DLC mask */
#define mode_bits2_DLC_MASK 0x01

/** SP mode bits 1st byte - PTAR value */
#define mode_bits1_PTAR  0x80
/** SP mode bits 1st byte - Reserved value */
#define mode_bits1_Rsrvd 0x00
/** SP mode bits 1st byte - SGHR value */
#define mode_bits1_SGHR  0x00
/** SP mode bits 1st byte - ASAR value */
#define mode_bits1_ASAR  0x0f

/** SP mode bits 2nd byte - BS value */
#define mode_bits2_BS  0x00
/** SP mode bits 2nd byte - FF value */
#define mode_bits2_FF  0x00
/** SP mode bits 2nd byte - VA value */
#define mode_bits2_VA  0x00
/** SP mode bits 2nd byte - DLC value */
#define mode_bits2_DLC 0x00

/** @} */ /* end of SYS_STORAGE_PROFILE_DEFINES */

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
@Group		SYSTEM_STRUCTS System Structures
@{
*//***************************************************************************/

/* Temporary storage profiles definition */
#define NUM_OF_SP 2

enum sp_types {
	SP_DEFAULT = 0, /* Default storage profile */
	SP_IPSEC, /* IPsec storage profile */
};

/**************************************************************************//**
@Description	Storage Profile Struct
*//***************************************************************************/

struct storage_profile {
	uint64_t ip_secific_sp_info; /**< IP-Specific SP Information */
	uint16_t dl; /**<  DataLength(correction)	*/
	uint16_t reserved; /**< reserved	*/
	uint16_t dhr; /**< DataHeadRoom(correction)	*/
	uint8_t  mode_bits1; /**< mode bits	*/
	uint8_t  mode_bits2; /**< mode bits	*/
	uint16_t pbs1; /**<  Pool Buffer Size	*/
	uint16_t bpid1; /**<  Bypass Memory Translation	*/
	uint16_t pbs2; /**<  Pool Buffer Size	*/
	uint16_t bpid2; /**<  Bypass Memory Translation	*/
	uint16_t pbs3; /**<  Pool Buffer Size	*/
	uint16_t bpid3; /**<  Bypass Memory Translation	*/
	uint16_t pbs4; /**<  Pool Buffer Size	*/
	uint16_t bpid4; /**<  Bypass Memory Translation	*/
};


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
