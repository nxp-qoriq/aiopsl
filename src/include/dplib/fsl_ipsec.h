/**************************************************************************//**
@File		fsl_ipsec.h

@Description	This file contains the AIOP IPSec API.
*//***************************************************************************/

#ifndef __FSL_IPSEC_H
#define __FSL_IPSEC_H

#include "common/types.h"


/**************************************************************************//**
@Group	FSL_IPSEC FSL_AIOP_IPSEC

@Description	Freescale AIOP IPsec API

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPSec handle Type definition

 *//***************************************************************************/
typedef uint64_t ipsec_handle_t;

/**************************************************************************//**
@Group		FSL_IPSEC_Functions IPsec Functions

@Description	Freescale AIOP IPsec Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	ipsec_frame_decrypt

@Description	This function performs the decryption and the required IPsec
		protocol changes (according to RFC4303). This function also
		handles UDP encapsulated IPsec packets according to RFC3948.
		Both Tunnel and Transport modes are supported.
		Tunneling of IPv4/IPv6 packets (either with (up to 64 Bytes) or
		without Ethernet L2) within IPv4/IPv6 is supported.
		The function also updates the decrypted frame parser result and
		checks the inner UDP checksum (if available).

@Param[in]	ipsec_handle - IPsec handle.

@Return		todo (implementation dependent)

@Cautions	User should note the following:
		 - In this function the task yields.
		 - This function preserves the Order Scope mode of the task. If
		the Order Scope is of mode concurrent, the Order Scope ID is
		incremented by 1.
		 - It is assumed that IPv6 ESP extension is the last IPv6
		extension in the packet.
		 - This function does not support input frames which are IPv6
		jumbograms.
*//****************************************************************************/
int32_t ipsec_frame_decrypt(ipsec_handle_t ipsec_handle);

/**************************************************************************//**
@Function	ipsec_frame_encrypt

@Description	This function performs the encryption and the required IPsec
		protocol changes (according to RFC4303).  This function
		enables UDP encapsulation of the IPsec packets according
		to RFC3948.
		Both Tunnel and Transport modes are supported.
		Tunneling of IPv4/IPv6 packets (either with (up to 64 Bytes) or
		without Ethernet L2) within IPv4/IPv6 is supported.
		The function also updates the encrypted frame parser result.

@Param[in]	ipsec_handle - IPsec handle.

@Return		todo (implementation dependent)

@Cautions	User should note the following:
		 - In this function the task yields.
		 - This function preserves the Order Scope mode of the task. If
		the Order Scope is of mode concurrent, the Order Scope ID is
		incremented by 1.
		 - This function does not support encrypted frames which are
		IPv6 jumbograms.
*//****************************************************************************/
int32_t ipsec_frame_encrypt(ipsec_handle_t ipsec_handle);

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */


#endif /* __FSL_IPSEC_H */
