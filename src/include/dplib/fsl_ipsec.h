/**************************************************************************//**
@File		fsl_ipsec.h

@Description	This file contains the AIOP IPSec API.
*//***************************************************************************/

#ifndef __FSL_IPSEC_H
#define __FSL_IPSEC_H

#include "general.h"

/**************************************************************************//**
@Group	FSL_IPSEC FSL_AIOP_IPSEC

@Description	Freescale AIOP IPsec API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	FSL_IPSEC_MACROS IPsec Macros

@Description	Freescale AIOP IPsec Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		IPSEC_FLAGS IPsec Flags

@Description	IPsec Flags

@{
*//***************************************************************************/
/** IPsec Tunnel Mode Flag */
#define IPSEC_FLAG_TUNNEL_MODE		0x00000000

/** IPsec Transport Mode Flag */
#define IPSEC_FLAG_TRANSPORT_MODE	0x00000001

/** @} */ /* end of IPSEC_FLAGS */

/** @} */ /* end of FSL_IPSEC_MACROS */

/**************************************************************************//**
@Group		FSL_IPSEC_STRUCTS IPsec Structures

@Description	Freescale AIOP IPsec Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPSec Parameters
*//***************************************************************************/
struct ipsec_params{
	/** SEC Flow Context Address in external memory.
	 * A pointer to a 64-byte memory structure followed by a SEC shared
	 * descriptor. This structure must be configured before calling this
	 * function. Documentation of this structure is out of the scope of
	 * this API. */
	uint64_t sec_flc_addr;

	/** Flags. Please refer to \ref IPSEC_FLAGS for more details. */
	uint32_t flags;

	/** Storage Profile ID of the output frame.
	 * This Storage Profile(SP) ID should fit the SP configured in the SEC
	 * descriptor which is located at \ref sec_flc_addr. */
	uint16_t spid;

	/** Padding */
	uint8_t  pad[2];
};

/** @} */ /* end of FSL_IPSEC_STRUCTS */

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

@Param[in]	ipsec_params - Function's parameters.
@Param[out]	byte_cnt - Encrypted byte count (As supplied by SEC engine).

@Return		TODO TBD

@Cautions	In this function the task yields.
		This function preserves the Order Scope mode of the task. If
		the Order Scope is of mode concurrent, the Order Scope ID is
		incremented by 1.
		It is assumed that IPv6 ESP extension is the last IPv6
		extension in the packet.
		This function does not support Storage Profile with BS == 1
		(buffer reuse).
		This function does not support input frames which are IPv6
		jumbograms.
*//****************************************************************************/
int32_t ipsec_frame_decrypt(struct ipsec_params *ipsec_params,
				 uint32_t *byte_cnt);

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

@Param[in]	ipsec_params - Function's parameters.
@Param[out]	byte_cnt - Encrypted byte count (As supplied by SEC engine).

@Return		TODO TBD

@Cautions	In this function the task yields.
		This function preserves the Order Scope mode of the task. If
		the Order Scope is of mode concurrent, the Order Scope ID is
		incremented by 1.
		This function does not support Storage Profile with BS == 1
		(buffer reuse).
		This function does not support encrypted frames which are IPv6
		jumbograms.
*//****************************************************************************/
int32_t ipsec_frame_encrypt(struct ipsec_params *ipsec_params,
				 uint32_t *byte_cnt);

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */

#endif /* __FSL_IPSEC_H */

