/**************************************************************************//**
 @File          ip.h

 @Description   This file contains the AIOP SW IP Internal API
*//***************************************************************************/
#ifndef __IP_H
#define __IP_H

/**************************************************************************//**
@Group		IP_INTERNAL  IP Internal

@Description	IP Internal

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		IP_INTERNAL_Functions IP Internal Functions

@Description	IP Internal Functions

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	ipv6_last_header

@Description	Return the fragmnetation pointer or the last extension
		pointer of IPv6 header.

@Param[in]	ipv6_hdr - pointer to IPv6 header
@Param[in]	flag - fragmentation request = 1, encaspulation request = 0.

@Return		Fragmentation pointer or last IPv6 extension pointer

@Cautions	The function assume a valid IPv6 frame
*//***************************************************************************/
uint32_t ipv6_last_header(struct ipv6hdr *ipv6_hdr, uint8_t flag);

/** @} */ /* end of IP_INTERNAL_Functions */

/** @} */ /* end of IP_INTERNAL */

#endif /* __IP_H */
