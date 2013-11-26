/**************************************************************************//**
@File          ipsec.h

@Description   This file contains IPSec internal functions and definitions

	       Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __IPSEC_H
#define __IPSEC_H

/**************************************************************************//**
@Group		AIOP_IPSEC_INTERNAL AIOP IPSEC Internal

@Description	AIOP IPSEC Internal

@{
*//***************************************************************************/
/**************************************************************************//**
@Description	IPSEC Global parameters
*//***************************************************************************/
uint8_t ipsec_pool_id;

/**************************************************************************//**
@Group		IPSEC_Functions Internal IPSec functions

@Description	Internal IPSec functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	ipsec_init

@Description	This function initializes the IPsec structure.
		Should be called at initialization time.
		
		Implicitly updated value in AIOP IPSec global parameters:
		ipsec_pool_id.


@Param[in]	pool_id - Used for IPsec functions buffer allocation for
		internal use. The number of buffers this pool contains must be
		at least equal to the number of tasks in the AIOP (256) and each
		buffer size must be at least equal to the maximum Accelerator
		Specific Annotation (ASA) size (960 Bytes).
		
@Param[in]	buffer_size - Size of the Context memory buffer, must match the
		size of the BMan buffer being acquired using the pool_id.
		Must be at least equal to the maximum Accelerator Specific
		Annotation (ASA) size (960 Bytes).
		
@Cautions	In this function the task yields.
*//****************************************************************************/
void ipsec_init(uint16_t pool_id, uint32_t buffer_size);

/* @} end of group IPSEC_Functions */
/* @} end of group AIOP_IPSEC_INTERNAL */

#endif /* __IPSEC_H */
