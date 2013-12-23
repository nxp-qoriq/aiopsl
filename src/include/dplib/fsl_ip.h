/**************************************************************************//**
@File		fsl_ip.h

@Description	This file contains IP functions (IP API)

@Cautions	Please note that the parse results must be updated before
		calling functions defined in this file.

		Copyright 2013 Freescale Semiconductor, Inc.

*//***************************************************************************/

#ifndef __FSL_IP_H
#define __FSL_IP_H

#include "common/types.h"
#include "dplib/fsl_ipf.h"
#include "dplib/fsl_ipr.h"
#include "dplib/fsl_ipv4_checksum.h"


/**************************************************************************//**
@Group	FSL_HM FSL_AIOP_Header_Modification

@Description	Header Modification API

@{
*//***************************************************************************/

/** The frame contain only one IP header */
#define NO_IP_ENCAPSULATION_FOUND_ERROR		(HM_MODULE_STATUS_ID + 0x0100)
/** No IP header was found.*/
#define NO_IP_HDR_ERROR				(HM_MODULE_STATUS_ID + 0x0300)


/**************************************************************************//**
@Group		HM_IP_Modes HM IP Modes

@Description	IP Header Modification Modes

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		HMIPv4ModeBits IPv4 header modification mode bits

@{
*//***************************************************************************/

	/** If set, update L4 checksum (if needed) */
#define IPV4_MODIFY_MODE_L4_CHECKSUM 0x01
	/** If set, Time to Live field is decremented by 1 */
#define IPV4_MODIFY_MODE_IPTTL 0x02
	/** If set, the original IP TOS (8 bits of TOS field in the frame)
	     will be replaced */
#define IPV4_MODIFY_MODE_IPTOS 0x04
	/** If set, the original IP ID will be replaced */
#define IPV4_MODIFY_MODE_IPID 0x08
	/** If set, the original IP Src address will be replaced */
#define IPV4_MODIFY_MODE_IPSRC 0x10
	/** If set, the original IP Dst address will be replaced */
#define IPV4_MODIFY_MODE_IPDST 0x20

/* @} end of group HMIPv4ModeBits */


/**************************************************************************//**
@Group		HMIPv6ModeBits IPv6 header modification mode bits

@{
*//***************************************************************************/

	/** If set, update L4 checksum (if needed) */
#define IPV6_MODIFY_MODE_L4_CHECKSUM 0x01
	/** If set, Hop Limit field is decremented by 1 */
#define IPV6_MODIFY_MODE_IPHL 0x02
	/** If set, the original IP TC (8 bits of Traffic Class
		field in the frame) will be replaced. */
#define IPV6_MODIFY_MODE_IPTC 0x04
	/** If set, the original flow label field (20bits)
		will be replaced */
#define IPV6_MODIFY_MODE_FLOW_LABEL 0x08
	/** If set, the original IP Src address will be replaced */
#define IPV6_MODIFY_MODE_IPSRC 0x10
	/** If set, the original IP Dst address will be replaced */
#define IPV6_MODIFY_MODE_IPDST 0x20

/* @} end of group HMIPv6ModeBits */


/**************************************************************************//**
@Group		HMIPv4EncapModeBits IPv4 header encapsulation mode bits

@{
*//***************************************************************************/

	/** If set, the TTL field is copied from the inner IP header. */
#define IPV4_ENCAP_MODE_TTL 0x01
	/** If set, the TOS[DS] field (6 bits) is propagated from the
		inner IP header. */
#define IPV4_ENCAP_MODE_TOS_DS 0x02
	/** If set, the TOS[ECN] field (2 bits) is propagated from the
		inner IP header. */
#define IPV4_ENCAP_MODE_TOS_ECN 0x04
	/** If set, the DF bit is copied from the inner IP header. */
#define IPV4_ENCAP_MODE_DF 0x08

/* @} end of group HMIPv4EncapModeBits */


/**************************************************************************//**
@Group		HMIPv6EncapModeBits IPv6 header encapsulation mode bits

@{
*//***************************************************************************/

	/** If set, the Hop Limit field is copied from the inner IP header. */
#define IPV6_ENCAP_MODE_HL 0x01
	/** If set, the TC[DS] field (6 bits) is propagated from the
		inner IP header. */
#define IPV6_ENCAP_MODE_TC_DSCP 0x02
	/** If set, the TC[ECN] field (2 bits) is propagated from the
		inner IP header. */
#define IPV6_ENCAP_MODE_TC_ECN 0x04

/* @} end of group HMIPv6EncapModeBits */


/**************************************************************************//**
@Group		HMIPDecapModeBits IP header decapsulation mode bits

@{
*//***************************************************************************/

	/** If set, the Hop Limit / TTL field is copied from the outer IP
		header.*/
#define IP_DECAP_MODE_TTL_HL 0x01
	/** If set, the TOS/TC[DS] field is propagated from the outer
		IP header.*/
#define IP_DECAP_MODE_TOS_TC_DS 0x02
	/** If set, the TOS/TC[ECN] field is propagated from the outer
		IP header.*/
#define IP_DECAP_MODE_TOS_TC_ECN 0x04

/* @} end of group HMIPDecapModeBits */

/* @} end of group HM_IP_Modes */

/**************************************************************************//**
@Group		FSL_HM_IP_Functions HM IP related functions

@Description	IP related Header Modification functions

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	ipv4_header_modification

@Description	Replace/update fields in the outer IPv4 header (if exists).

		It automatically generates the IP checksum and optionally
		can update the UDP/TCP checksum.
		The function assumes the original IP checksum to be valid.

		If the incoming frame contains TCP/UDP (the original UDP
		checksum!= 0), the following occur:
		 1- the UDP/TCP checksum is recalculated based on the original
		    checksum and the change in the relevant header fields.
		 2-The gross running sum of the frame becomes invalid after
		   calling this function.

		Implicit input parameters in task defaults: frame handle,
			segment handle, parser_profile_id, parser_starting_hxs.
		Implicitly updated values in task defaults: segment length,
								segment address.


@Param[in]	flags - \link HMIPv4ModeBits IPv4 modification mode bits
			\endlink
@Param[in]	tos - TOS header to be replaced.
@Param[in]	id  - Identification header to be replaced.
@Param[in]	ip_src_addr - Src address header to be replaced.
@Param[in]	ip_dst_addr - Dst address header to be replaced.

@Return		Success or Failure (There was no IPv4 header in the frame).

@Cautions	The parse results must be updated before
		calling this operation.
		In this function, the task yields.

*//***************************************************************************/
int32_t ipv4_header_modification(uint8_t flags, uint8_t tos, uint16_t id,
		uint32_t ip_src_addr, uint32_t ip_dst_addr);


/*************************************************************************//**
@Function	ipv6_header_modification

@Description	Replace/update fields in the outer IPv6 header (if exists).

		This function supports only IPv6 frames without routing header.

		If the incoming frame contains TCP/UDP, the following occur:
		 1- the UDP/TCP checksum is recalculated based on the original
		    checksum and the change in the relevant header fields.
		 2-The gross running sum of the frame becomes invalid after
		   calling this function.

		Implicit input parameters in task defaults: frame handle,
			segment handle, parser_profile_id, parser_starting_hxs.
		Implicitly updated values in task defaults: segment length,
								segment address.

@Param[in]	flags - \link HMIPv6ModeBits IPv6 modification mode bits
			\endlink
@Param[in]	tc - TC header to be replaced.
@Param[in]	flow_label - flow label header to be replaced.
@Param[in]	ip_src_addr - Points to the IP Src address header to
		be replaced (in internal memory).
@Param[in]	ip_dst_addr - Points to the IP Dst address header to
		be replaced (in internal memory).

@Return		Success or Failure (There was no IPv6 header in the frame).

@Cautions	The parse results must be updated before
		calling this operation.\n
		In this function, the task yields.

*//***************************************************************************/
int32_t ipv6_header_modification(uint8_t flags, uint8_t tc,
				    uint32_t flow_label, uint8_t *ip_src_addr,
				    uint8_t *ip_dst_addr);


/*************************************************************************//**
@Function	ipv4_header_encapsulation

@Description	This HM operation encapsulates an IP header with an outer IPv4
		header for tunneling.
		It can be applied to frames with or without ETH header.
		The Ethernet type (or MPLS label) is updated if exists.
		The IPv4 length field of the inserted header is updated.
		The IPv4 checksum is calculated and inserted by this operation.

		The checksum field in the inserted IPv4 header should be
		cleared by the user.

		Few fields can be imported from the inner IP (v4 or v6) to the
		outer IPv4 header.

		The parse results are updated (the parser is rerun)
		automatically at the end of this operation.

		Implicit input parameters in task defaults: frame handle,
			segment handle, parser_profile_id, parser_starting_hxs.
		Implicitly updated values in task defaults: segment length,
							segment address.


@Param[in]	ipv4header - Points to the new outer IPv4 header be
		inserted (in workspace memory).
@Param[in]	ipv4_header_size - Size (in bytes) of the new outer IPv4 header.

@Param[in]	flags - \link HMIPv4EncapModeBits IPv4 encapsulation mode bits
			\endlink


@Return		Success or Failure (There was no inner IP header in the frame).

@Cautions	The parse results must be updated before calling this operation.
		In this function, the task yields.

*//***************************************************************************/
int32_t ipv4_header_encapsulation(uint8_t flags,
								  void *ipv4header, uint8_t ipv4_header_size);


/*************************************************************************//**
@Function	ipv6_header_encapsulation

@Description	This HM operation encapsulates an IP header with an outer IPv6
		header for tunneling. It can be applied to frames with or
		without ETH header.

		The Ethernet type (or MPLS label) is updated if exists.
		The IPv6 payload length field of the inserted header is updated.

		Few fields can be imported from the inner IP (v4 or v6) to the
		outer IPv6 header.

		The parse results are updated (the parser is rerun)
		automatically at the end of this operation.

		Implicit input parameters in task defaults: frame handle,
			segment handle, parser_profile_id, parser_starting_hxs.
		Implicitly updated values in task defaults: segment length,
							segment address.


@Param[in]	ipv6header - Points to the new outer IPv6 header to be
		inserted (in workspace).
@Param[in]	ipv6_header_size - Size (in bytes) of the new outer IPv6 header.

@Param[in]	flags - \link  HMIPv6EncapModeBits IPv6 encapsulation mode bits
			\endlink

@Return		Success or Failure (There was no inner IP header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.
		In this function, the task yields.

*//***************************************************************************/
int32_t ipv6_header_encapsulation(uint8_t flags,
								  void *ipv6header, uint8_t ipv6_header_size);

/*************************************************************************//**
@Function	ip_header_decapsulation

@Description	This HM operation de-capsulates an outer IP header (if exist).
		It can be applied to frames with or without ETH header.

		Few fields can be imported from the outer IP to the inner IP
		header.

		The parse results are updated (the parser is rerun)
		automatically at the end of this operation.

@Param[in]	flags - \link HMIPDecapModeBits IP decapsulation mode bits
			\endlink

@Return		Success or Failure (There was no outer IP header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.
*//***************************************************************************/
int32_t ip_header_decapsulation(uint8_t flags);



/*************************************************************************//**
@Function	ip_set_nw_src

@Description	Replace the outer IPv4 source address. The IP and UDP/TCP
		checksums are updated automatically.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	src_addr - the new IPv4 source address.

@Return		Success or Failure (There was no IPv4 header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.\n
		This function assumes the original IP checksum is valid.

*//***************************************************************************/
int32_t ip_set_nw_src(uint32_t src_addr);


/*************************************************************************//**
@Function	ip_set_nw_dst

@Description	Replace the outer IPv4 destination address. The IP and UDP/TCP
		checksums are updated automatically.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	dst_addr - the new IPv4 destination address.

@Return		Success or Failure (There was no IPv4 header in the frame).

@Cautions	The parse results must be updated before calling this
		operation.\n
		This function assumes the original IP checksum is valid.

*//***************************************************************************/
int32_t ip_set_nw_dst(uint32_t dst_addr);

/* @} end of group FSL_HM_IP_Functions */
/* @} end of group FSL_HM */


#endif /* __FSL_IP_H */
