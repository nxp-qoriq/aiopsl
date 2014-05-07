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
@Group		AIOP_General_Protocols_IPV6_HDR_Offsets IPv6 Header Offsets
@{
*//***************************************************************************/
#define  IPV6_NEXT_HDR_OFFSET 6 /*!< IPv6 next header offset*/

/** @} */ /* end of AIOP_General_Protocols_IPV6_HDR_Offsets */


/**************************************************************************//**
@Group		AIOP_General_MASKS IPv6 Last Header MASKS
@{
*//***************************************************************************/
#define  IPV6_NO_EXTENSION 0x8000 /*!< IPv6 no extension mask*/

/** @} */ /* end of AIOP_General_MASKS */

/**************************************************************************//**
@Group		IPV6_LAST_HEADER_Definitions ipv6_last_header flags defines 
@{
*//***************************************************************************/
#define  LAST_HEADER_OFFSET_BEFORE_FRAG 0 /*!< last header offset before frag*/
#define  LAST_HEADER_OFFSET 1 /*!< last header offset*/

/** @} */ /* end of IPV6_LAST_HEADER_Definitions */

/**************************************************************************//**
@Group		IP_INTERNAL_Functions IP Internal Functions

@Description	IP Internal Functions

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	ipv6_last_header

@Description	Return the pointer to fragment or last extension IPv6 header.

@Param[in]	ipv6_hdr - pointer to IPv6 header
@Param[in]	flag - (\ref IPV6_LAST_HEADER_Definitions).

@Return		Pointer to next header of last extension before fragment or last 
			IPv6 extension pointer (depend on flag).
  	  	  	The MSB bit indicate whether there is a extension (1=no extension)

@Cautions	The function assume a valid IPv6 frame
*//***************************************************************************/
uint32_t ipv6_last_header(struct ipv6hdr *ipv6_hdr, uint8_t flag);

/** @} */ /* end of IP_INTERNAL_Functions */

/** @} */ /* end of IP_INTERNAL */

#endif /* __IP_H */
