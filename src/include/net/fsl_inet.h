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

/**************************************************************************//*
@File           fsl_inet.h

@Description    Contains AIOP SL Network Utilities API functions declarations.

*//***************************************************************************/

#ifndef _FSL_INET_H
#define _FSL_INET_H

#include "common/types.h"
#include "fsl_stdio.h"
#include "fsl_errors.h"

/**************************************************************************//**
@Group          net_utils_g AIOP Service Layer Network Utilities

@Description    The AIOP Service Layer Network Utilities group provides
		standard network manipulation functions

@{
*//***************************************************************************/

#define AF_INET             0               
/**< Internet IPv4 Protocol  */
#define AF_INET6            (AF_INET + 1)   
/**< Internet IPv6 Protocol  */
#define AF_MAX              (AF_INET + 2)   
/**< Maximal number of supported address families */

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
		1 on success (network address was successfully converted)
		0 is returned if src doesn't contain a character string
		representing valid network address in the specified address
		family.
		-1 if af does not contain a valid address family

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
		non-NULL pointer to destination is returned on success
		NULL is returned if size is not big enough or if af does not
		contain a valid address family

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

/** @} */ /* end of net_utils_g group */

#endif /* _FSL_INET_H */

