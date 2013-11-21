/**************************************************************************//**
@File          ipr.h

@Description   This file contains IPR internal functions and definitions

	       Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_IPR_H
#define __AIOP_IPR_H


/**************************************************************************//**
@Group		IPR_Functions Internal IPR functions

@Description	Internal IP reassembly functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	ipr_init

@Description	Initialize the IP Reassembly infrastructure.
		This function should be called prior to calling any
		IPR function.
		This function will initialize two KeyIDs (one for IPv4 and one
		for Ipv6).

@Param[in]	epid - The epid configured for the use of IPR Time Out.
		Upon IPR Time Out expiration, a task is created and uses this
		epid for EPID lookup table.\n
		todo - if a timer registration will be available, this epid
		     will be changed to a timer_handle
@Param[in]	pool_id - Pool id for context buffers allocation.\n
		The size of each buffer should be at least 2240 bytes.\n
		Buffers should be aligned to 64 bytes.

@Return		None.

@Cautions	None.
*//***************************************************************************/
void ipr_init(uint8_t epid, uint8_t pool_id);

/* @} end of group IPR_Functions */

/**************************************************************************//**
@Description	IPR Global parameters
*//***************************************************************************/

struct ipr_global_parameters {
uint8_t ipr_timeout_epid;
uint8_t ipr_key_id_ipv4;
uint8_t ipr_key_id_ipv6;
uint8_t ipr_pool_id;
};

struct ipr_global_parameters ipr_global_params;

#endif /* __AIOP_IPR_H */
