/**************************************************************************//*
@File           fsl_inet.h

@Brief          AIOP Service Layer Network Utilities header file

@Description    Contains AIOP SL Network Utilities API functions declarations.

@@internal
@requirements   CR:ENGR00272890 CR:ENGR00272889
@implements     See CR's Analysis information
@warning        POSIX used for the API, however, no full POSIX implementation
		is guaranteed


		Copyright 2013-2014 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef _FSL_INET_H
#define _FSL_INET_H

#include "common/types.h"
#include "common/errors.h"

/**************************************************************************//**
 @Group		UTILITIES  UTILITIES

 @Description	ARENA Utilities APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group          net_utils AIOP Service Layer Network Utilities

@Description    The AIOP Service Layer Network Utilities group provides
		standard network manipulation functions

@{
*//***************************************************************************/

#define AF_INET             0               /**< Internet IPv4 Protocol  */
#define AF_INET6            (AF_INET + 1)   /**< Internet IPv6 Protocol  */
#define AF_MAX              (AF_INET + 2)   /**< Maximal number of supported
																								 adress families */

/**************************************************************************//**
@Function       inet_pton

@Description    This function converts the character string src into
		a network address structure in the af address family,
		then copies the network address structure to dst.
		The af argument must be either AF_INET or AF_INET6.

@Param[in]      af - The following address families are currently supported
		AF_INET - src points to a character string containing an IPv4
		network address in dotted-decimal format,
		AF_INET6 - src points to a character string containing an
		IPv6 network address.
@Param[in]      src - char string cotaing a network address
@Param[out]     dst - pointer to network address structure

@Return         error or success value as defined below
@Retval         1 on success (network address was successfully converted)
@Retval         0 is returned if src doesn't contain a character string
		representing valid network address in the specified address
		family.
@Retval        -1 if af does not contain a valid address family

@internal
@requirements   CR:ENGR00272889
@implements     See CR's Analysis information
@warning        POSIX used for the API, however, no full POSIX implementation is
		guaranteed; Groups of zeros replaced by :: and dotted-quad
		notation for AF_INET6 are not supported

*//***************************************************************************/
int inet_pton(int af, const char *src, void *dst);

/**************************************************************************//**
@Function       inet_ntop

@Description    This function converts the network address structure src in the
		af address family into a character string. The resulting string
		is copied to the buffer pointed to by dst, which must be a
		non-NULL pointer. The caller specifies the number of bytes
		available in this buffer in the argument size.

@Param[in]      af - The following address families are currently supported
		AF_INET - src points to a character string containing an IPv4
		network address in dotted-decimal format,
		AF_INET - src points to a character string containing an IPv6
		network address.
@Param[in]      src  - network address structure
@Param[out]     dst  - char string address
@Param[in]      size - number of buffers available in dst buffer

@Return         error or success value as defined below
@Retval         non-NULL pointer to destination is returned on success
@Retval         NULL is returned if size is not big enough or if af does not
		contain a valid address family

@internal
@requirements   CR:ENGR00272890
@implements     See CR's Analysis information
@warning        POSIX used for the API, however, no full POSIX implementation is
		guaranteed. Groups of zeros replaced by :: and dotted-quad
		notation for AF_INET6 are not supported.
*//***************************************************************************/
const char *inet_ntop(int af, const void *src, char *dst, size_t size);


/**************************************************************************//**
@Function       htons

@Description    This function converts the unsigned short integer hostshort
		from host byte order to network byte order.

@Param[in]      hostshort - Host short value to be converted.

@Return         Converted unsigned short value between host and network

*//***************************************************************************/
uint16_t htons(uint16_t hostshort);

/**************************************************************************//**
@Function       ntohs

@Description    This function converts the unsigned short integer netshort
		from network byte order to host byte order.

@Param[in]      netshort - Network short value to be converted.

@Return         Converted unsigned short value between network and host

*//***************************************************************************/
uint16_t ntohs(uint16_t netshort);

/**************************************************************************//**
@Function       htonl

@Description    This function converts the unsigned integer hostlong from host
		byte order to network byte order.

@Param[in]      hostlong - Host long value to be converted.

@Return         Converted unsigned long value between host and network

*//***************************************************************************/
uint32_t htonl(uint32_t hostlong);

/**************************************************************************//**
@Function       ntohl

@Description    This function converts the unsigned integer netlong from
		network byte order to host byte order.

@Param[in]      netlong - Network long value to be converted.

@Return         Converted unsigned long value between network and host

*//***************************************************************************/
uint32_t ntohl(uint32_t netlong);

/** @} */ /* end of net_utils group */
/** @} *//* end of ARENA Utilities APIs */


#endif /* _FSL_INET_H */

