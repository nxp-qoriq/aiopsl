/**************************************************************************//*
 @file          inet.h

 @brief         AIOP Service Layer Network Utilities header file

 @details       Contains AIOP SL Network Utilities API functions declarations.

 @@internal
 @requirements CR:ENGR00272890 CR:ENGR00272889
 @implements   See CR's Analysis information
 @warning      POSIX used for the API, however, no full POSIX implementation is 
               guaranteed
 *//***************************************************************************/

#ifndef _INET_H
#define _INET_H

#include "common/types.h" 
#include "common/errors.h"

/**************************************************************************//**
 @ingroup         fsl_lib_g   Utility Library 
 @{
 *//***************************************************************************/
/**************************************************************************//**
 @ingroup       net_utils AIOP Service Layer Network Utilities

 @Description   The AIOP Service Layer Network Utilities group provides 
                standard network manipulation functions 

 @{
 *//***************************************************************************/ 

#define AF_INET             0               /**< Internet IPv4 Protocol  */
#define AF_INET6            (AF_INET + 1)   /**< Internet IPv6 Protocol  */
#define AF_MAX              (AF_INET + 2)   /**< Maximal number of supported 
                                                 adress families */

/**************************************************************************//**
 @brief      inet_pton 

 @details   This function converts the character string src into
            a network address structure in the af address family,
	    then copies the network address structure to dst.
            The af argument must be either AF_INET or AF_INET6

 @param    [in] af - The following address families are currently supported
                AF_INET - src points to a character string containing an IPv4
	                  network address in dotted-decimal format, 
                AF_INET6 - src points to a character string containing an 
                           IPv6 network address. 
 @param    [in]     src - char string cotaing a network address
 @param    [out]    dst - pointer to network address structure 

 @returns  error or success value as defined below
 @retval   1 on success (network address was successfully converted).
 @retval   0 is returned if src doesn't contain a character string representing 
             valid network address in the specified address family. 
 @retval  -1 if af does not contain a valid address family

 @internal
 @requirements CR:ENGR00272889
 @implements   See CR's Analysis information
 @warning      POSIX used for the API, however, no full POSIX implementation is 
               guaranteed;
               Groups of zeros replaced by :: and dotted-quad notation for 
               AF_INET6 are not supported

 *//***************************************************************************/
int inet_pton(int af, const char *src, void *dst);

/**************************************************************************//**
 @brief    inet_ntop 

 @details  This function converts the network address structure src in the 
           af address family into a character string. The resulting string
	   is copied to the buffer pointed to by dst, which must be a non-NULL
	   pointer. The caller specifies the number of bytes available in this
	   buffer in the argument size. 

 @param    [in] af - The following address families are currently supported
                AF_INET - src points to a character string containing an IPv4
	                  network address in dotted-decimal format, 
                AF_INET - src points to a character string containing an 
                          IPv6 network address. 
 @param    [in]  src  - network address structure
 @param    [out] dst  - char string address
 @param    [in]  size - number of buffers available in dst buffer

 @returns  error or success value as defined below
 @retval   non-NULL pointer to destination is returned on sucess
 @retval   NULL is returned if size is not big enough or if af does not contain 
           a valid address family 

 @internal
 @requirements CR:ENGR00272890
 @implements   See CR's Analysis information
 @warning      POSIX used for the API, however, no full POSIX implementation is 
               guaranteed. Groups of zeros replaced by :: and dotted-quad 
               notation for AF_INET6 are not supported.               
 *//***************************************************************************/
const char * inet_ntop(int af, const void *src, char *dst, size_t size);


/**************************************************************************//**
 @brief    htons 

 @details  This function converts the unsigned short integer hostshort
           from host byte order to network byte order.

 @param    [in] uint16_t hostshort

 @returns  Converted unsigned short value between host and network 

 *//***************************************************************************/
uint16_t htons(uint16_t hostshort);

/**************************************************************************//**
 @brief    ntohs 

 @details  This function converts the unsigned short integer netshort
           from network byte order to host byte order.

 @param    [in] uint16_t netshort

 @returns  Converted unsigned short value between network and host

 *//***************************************************************************/
uint16_t ntohs(uint16_t netshort);

/**************************************************************************//**
 @brief    htonl 

 @details  This function converts the unsigned integer hostlong from host
           byte order to network byte order.

 @param    [in] uint32_t hostlong

 @returns  Converted unsigned long value between host and network

 *//***************************************************************************/
uint32_t htonl(uint32_t hostlong);

/**************************************************************************//**
 @brief    ntohl 

 @details  This function converts the unsigned integer netlong from
           network byte order to host byte order.

 @param    [in] uint32_t netlong

 @returns  Converted unsigned long value between network and host

 *//***************************************************************************/
uint32_t ntohl(uint32_t netlong);

/** @} */ /* end of net_utils group */
/** @} *//* end of fsl_lib_g group */


#endif /* _INET_H */

