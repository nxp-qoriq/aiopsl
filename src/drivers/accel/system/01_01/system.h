/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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
@File		system.h

@Description	This file contains the AIOP SW system interface.

*//***************************************************************************/

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include "general.h"
#include "fsl_general.h"
#include "fsl_cdma.h"
#include "fsl_fdma.h"
#include "fsl_icontext.h"
#include "fsl_dbg.h"

extern struct icontext icontext_aiop;

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

#define SP_NUM_OF_STORAGE_PROFILES	2

/* Storage Profile 0 */

/** SP0 mode bits 1st byte - PTAR mask */
#define sp0_mode_bits1_PTAR_MASK  0x80
/** SP0 mode bits 1st byte - SGHR mask */
#define sp0_mode_bits1_SGHR_MASK  0x10
/** SP0 mode bits 1st byte - ASAR mask */
#define sp0_mode_bits1_ASAR_MASK  0x0f

/** SP0 mode bits 2nd byte - BS mask */
#define sp0_mode_bits2_BS_MASK  0x80
/** SP0 mode bits 2nd byte - FF mask */
#define sp0_mode_bits2_FF_MASK  0x30
/** SP0 mode bits 2nd byte - VA mask */
#define sp0_mode_bits2_VA_MASK  0x04
/** SP0 mode bits 2nd byte - DLC mask */
#define sp0_mode_bits2_DLC_MASK 0x01

/** SP0 mode bits 1st byte - PTAR value */
#define sp0_mode_bits1_PTAR  0x80
/** SP0 mode bits 1st byte - Reserved value */
#define sp0_mode_bits1_Rsrvd 0x00
/** SP0 mode bits 1st byte - SGHR value */
#define sp0_mode_bits1_SGHR  0x00
/** SP0 mode bits 1st byte - ASAR value */
#define sp0_mode_bits1_ASAR  0x0f

/** SP0 mode bits 2nd byte - BS value */
#define sp0_mode_bits2_BS  0x00
/** SP0 mode bits 2nd byte - FF value */
#define sp0_mode_bits2_FF  0x00
/** SP0 mode bits 2nd byte - VA value */
#define sp0_mode_bits2_VA  0x00
/** SP0 mode bits 2nd byte - DLC value */
#define sp0_mode_bits2_DLC 0x00

/* Storage Profile 1 */

/** SP1 mode bits 1st byte - PTAR mask */
#define sp1_mode_bits1_PTAR_MASK  0x80
/** SP1 mode bits 1st byte - SGHR mask */
#define sp1_mode_bits1_SGHR_MASK  0x10
/** SP1 mode bits 1st byte - ASAR mask */
#define sp1_mode_bits1_ASAR_MASK  0x0f

/** SP1 mode bits 2nd byte - BS mask */
#define sp1_mode_bits2_BS_MASK  0x80
/** SP1 mode bits 2nd byte - FF mask */
#define sp1_mode_bits2_FF_MASK  0x30
/** SP1 mode bits 2nd byte - VA mask */
#define sp1_mode_bits2_VA_MASK  0x04
/** SP1 mode bits 2nd byte - DLC mask */
#define sp1_mode_bits2_DLC_MASK 0x01

/** SP1 mode bits 1st byte - PTAR value */
#define sp1_mode_bits1_PTAR  0x00
/** SP1 mode bits 1st byte - Reserved value */
#define sp1_mode_bits1_Rsrvd 0x00
/** SP mode bits 1st byte - SGHR value */
#define sp1_mode_bits1_SGHR  0x00
/** SP1 mode bits 1st byte - ASAR value */
#define sp1_mode_bits1_ASAR  0x0f

/** SP1 mode bits 2nd byte - BS value */
#define sp1_mode_bits2_BS  0x80
/** SP1 mode bits 2nd byte - FF value */
#define sp1_mode_bits2_FF  0x00
/** SP1 mode bits 2nd byte - VA value */
#define sp1_mode_bits2_VA  0x00
/** SP1 mode bits 2nd byte - DLC value */
#define sp1_mode_bits2_DLC 0x00


/** @} */ /* end of SYS_STORAGE_PROFILE_DEFINES */

/**************************************************************************//**
@Group SYS_ID_POOL_LENGTH_DEFINES System ID Pools Length Defines
@{
*//***************************************************************************/
/** Parser Profile ID pool length */
#define SYS_NUM_OF_PRPIDS	64

/** Key ID pool length */
#define SYS_NUM_OF_KEYIDS	256

/** @} */ /* end of SYS_ID_POOL_LENGTH_DEFINES */

/**************************************************************************//**
@Group SYSTEM_STATUSES_DEFINES System Statuses Defines
@{
*//***************************************************************************/
/** System initialization failed due to slab failure */
#define SYSTEM_INIT_SLAB_FAILURE	0xAA

/** @} */ /* end of SYSTEM_STATUSES_DEFINES */

/** @} */ /* end of SYSTEM_MACROS */

/* Temporary storage profiles definition */
#define NUM_OF_SP 2

enum sp_types {
	SP_DEFAULT = 0, /* Default storage profile */
	SP_IPSEC /* IPsec storage profile */
};

 /**************************************************************************//**
  @Group	SYSTEM_Enumerations PARSER Enumerations

  @Description	SYSTEM Enumerations

  @{
 *//***************************************************************************/
/**************************************************************************//**
  @enum system_functions

  @Description	AIOP SYSTEM Functions enumeration.

  @{
 *//***************************************************************************/
 enum system_function_identifier {
	 SYS_PRPID_POOL_CREATE = 0,
	 SYS_KEYID_POOL_CREATE,
	 AIOP_SL_INIT,
	 AIOP_SL_FREE
 };

 /** @} */ /* end of group SYSTEM_Enumerations */


 /**************************************************************************//**
 @Group		SYSTEM_STRUCTS System Structures
 @{
 *//***************************************************************************/

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

@Return		None.

@Cautions	Should be called only once per CTLU.
		In this function the task yields.
		This function may result in a fatal error.
*//***************************************************************************/
void sys_prpid_pool_create(void);

/*************************************************************************//**
@Function	sys_keyid_pool_create

@Description	Creation and Initialization of system Key ID pool for CTLU.

		Implicitly updated values in AIOP System global parameters:
		ext_keyid_pool_address

@Return		None.

@Cautions	Should be called only once per CTLU.
		In this function the task yields.
		This function may result in a fatal error.
*//***************************************************************************/
void sys_keyid_pool_create(void);

/*************************************************************************//**
@Function	aiop_sl_early_init

@Description	AIOP Service Layer Early Initialization function.

		Buffers requests registrations.

@Return		0 on Success, or negative value on error.

@Cautions	Should be called during system initialization.
		In this function the task yields.
		This function may result in a fatal error.
*//***************************************************************************/
int aiop_sl_early_init(void);

/*************************************************************************//**
@Function	aiop_sl_init

@Description	AIOP Service Layer Initialization function.

		Implicitly updated values in AIOP System global parameters:
		ext_keyid_pool_address, ext_prpid_pool_address.

@Return		0 on Success, or negative value on error.

@Cautions	Should be called during system initialization.
		In this function the task yields.
		This function may result in a fatal error.
*//***************************************************************************/
int aiop_sl_init(void);

/*************************************************************************//**
@Function	aiop_sl_free

@Description	AIOP Service Layer Free Resources function.

@Cautions	Should be called before system shutdown.
		In this function the task yields.
*//***************************************************************************/
void aiop_sl_free(void);

/**************************************************************************//**
@Function	system_init_exception_handler

@Description	Handler for the error status returned from the system API
		functions.

@Param[in]	file_path - The path of the file in which the error occurred.
@Param[in]	func_id - The function in which the error occurred.
@Param[in]	line - The line in which the error occurred.
@Param[in]	status - Status to be handled be this function.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void system_init_exception_handler(enum system_function_identifier func_id,
			     uint32_t line,
			     int32_t status);

/** @} */ /* end of SYSTEM_Functions */

inline void fsl_read_external_data(void *ws_addr, uint64_t ext_address,
				   uint16_t copy_size, uint32_t flags)
{
	if (flags == READ_DATA_USING_FDMA) {
		fdma_dma_data(copy_size, icontext_aiop.icid, ws_addr,
			      ext_address,
			      icontext_aiop.dma_flags |
				      FDMA_DMA_DA_SYS_TO_WS_BIT);
		return;
	}

	if (flags == READ_DATA_USING_CDMA) {
		cdma_read(ws_addr, ext_address, copy_size);
		return;
	}
}

/** @} */ /* end of SYSTEM */

#endif /* __SYSTEM_H_ */
