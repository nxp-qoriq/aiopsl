/**************************************************************************//**
@File		ipsec.h

@Description	This file contains the AIOP IPSec 
		internal functions and definitions.
		
*//***************************************************************************/

#ifndef __AIOP_IPSEC_H
#define __AIOP_IPSEC_H

#include "common/types.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group	FSL_IPSEC IPSEC

@Description	Freescale AIOP IPsec API

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		IPSEC_ENUM IPsec Enumerations

 @Description	IPsec Enumerations

 @{
*//***************************************************************************/


/* @} end of IPSEC_ENUM */

/**************************************************************************//**
@Group	FSL_IPSEC_MACROS IPsec Macros

@Description	Freescale IPsec Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPSec handle Type definition

*//***************************************************************************/
//typedef uint64_t ipsec_handle_t;

/** @} */ /* end of FSL_IPSEC_STRUCTS */


/**************************************************************************//**
@Group		FSL_IPSEC_Functions IPsec Functions

@Description	Freescale AIOP IPsec Functions

@{
*//***************************************************************************/

//TODO: move ipsec_init to an internal header file

/**************************************************************************//**
@Function	ipsec_init

@Description	This function performs reservation of:
		1. Maximum number of IPsec SA buffers.
		2. Optionally maximum number of buffers needed for ASA copying.

		TODO need to finalize the buffer mechanism.
		Implicitly: 
		Updated the BPID of SA in the SRAM.
		Optionally updated the BPID of ASA copying in the SRAM.
		
@Param[in]	max_sa_no - Max. number of SAs.

@Param[in]	asa_copy - Flag indicating if SA copying is needed.

@Return		TODO

*//****************************************************************************/
int32_t ipsec_init(uint16_t max_sa_no, uint8_t asa_copy);

/**************************************************************************//**

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */
/** @} */ /* end of NETF */


#endif /* __AIOP_IPSEC_H */
