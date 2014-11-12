/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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

/**************************************************************************//**
 @File		fsl_l2.h

 @Description	This file contains the AIOP Header Modification - L2 HM API

 @Cautions	Please note that the parse results must be updated before
 calling functions defined in this file.

 *//***************************************************************************/

#ifndef __FSL_L2_H
#define __FSL_L2_H

#include "common/types.h"

/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
 *//***************************************************************************/
/**************************************************************************//**
 @Group		AIOP_L2 L2

 @Description	AIOP L2 related API

 @{
 *//***************************************************************************/
/**************************************************************************//**
 @Group		AIOP_L2_HM L2 Header Modifications

 @Description	AIOP L2 related header modifications API

 @{
 *//***************************************************************************/

/** No VLAN exists.*/
#define NO_VLAN_ERROR				(HM_MODULE_STATUS_ID + 0x0400)

/** Minimum Segment size. */
#define MIN_SEGMENT_SIZE		128

/**************************************************************************//**
 @Group		FSL_HM_L2_Functions HM L2 related functions

 @Description	L2 related Header Modification functions

 @{
 *//***************************************************************************/

/*************************************************************************//**
 @Function	l2_header_remove

 @Description	Remove Ethernet/802.3 MAC header.

 If VLAN tags are present they are also removed.
 If MPLS header exists, it is also removed.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in task defaults: frame handle,
 segment handle, parser_profile_id, parser_starting_hxs.
 Implicitly updated values in task defaults: segment length,
 segment address, parser_starting_hxs.

 @Return		None.

 @Cautions
 - This function assumes that the ethernet header exists
 in the packet.
 - The parse results must be updated before calling this
 operation.
 - In this function, the task yields.

 *//***************************************************************************/
void l2_header_remove(void);

/*************************************************************************//**
 @Function	l2_vlan_header_remove

 @Description	VLAN Header removal. Remove the stacked QTags if exists.
 (Unlimited stacked QTags).

 This function assumes an Ethernet header is present.

 The parse results are updated automatically at the end of this
 operation.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in task defaults: frame handle,
 segment handle, parser_profile_id, parser_starting_hxs.
 Implicitly updated values in task defaults: segment length,
 segment address,parser_starting_hxs.

 @Return
 0 - Success.
 HM_ERROR_NO_VLAN - There was no VLAN in the frame.
 HM_ERROR_NO_IP_HDR - There is no IP header after the stacked QTags.

 @Cautions	The parse results must be updated before calling this operation.
 In this function, the task yields

 *//***************************************************************************/
int l2_vlan_header_remove(void);

/*************************************************************************//**
 @Function	l2_set_vlan_vid

 @Description	Set the 802.1q outer VLAN id.

 @Param[in]	vlan_vid - VLAN Identifier (VID).

 @Return		Success or Failure (There was no VLAN in the frame).

 *//***************************************************************************/
int l2_set_vlan_vid(uint16_t vlan_vid);

/*************************************************************************//**
 @Function	l2_set_vlan_pcp

 @Description	Set the 802.1q priority in the outer VLAN.

 @Param[in]	vlan_pcp - VLAN Priority Code Point (PCP).

 @Return		Success or Failure (There was no VLAN in the frame).

 *//***************************************************************************/
int l2_set_vlan_pcp(uint8_t vlan_pcp);

/*************************************************************************//**
 @Function	l2_set_dl_src

 @Description	Replace the Ethernet source address

 @Param[in]	src_addr - Pointer to the new Ethernet source address.

 @Return		None.

 @Cautions	None.
 *//***************************************************************************/
void l2_set_dl_src(uint8_t *src_addr);

/*************************************************************************//**
 @Function	l2_set_dl_dst

 @Description	Replace the Ethernet destination address

 @Param[in]	dst_addr - Pointer to the new Ethernet destination address.

 @Return		None.

 @Cautions	None.
 *//***************************************************************************/
void l2_set_dl_dst(uint8_t *dst_addr);

/*************************************************************************//**
 @Function	l2_push_vlan

 @Description	Push a new outer VLAN tag.\n

 This function assumes the presence of an Ethernet header.

 The parse results are updated automatically at the end of
 this operation.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in Task Defaults: frame handle,
 segment handle, segment address.
 Implicit output parameters in Task Defaults: parser_starting_hxs


 @Param[in]	ethertype -  Indicates the Ethertype of the new tag.

 @Return		None.

 @Cautions	The parse results must be updated before calling this operation.
 If an ethernet header is present, it is assumed to be located at
 the beginning of the segment (offset 0 from segment address).
 If there is no ethernet header, the vlan is inserted at the
 beginning of the segment.
 *//***************************************************************************/
void l2_push_vlan(uint16_t ethertype);

/*************************************************************************//**
 @Function	l2_push_and_set_vlan

 @Description	Push and set a new outer VLAN tag.\n

 This function assumes the presence of an Ethernet header.

 The parse results are updated automatically at the end of
 this operation.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in Task Defaults: frame handle,
 segment handle, segment address.
 Implicit output parameters in Task Defaults: parser_starting_hxs


 @Param[in]	vlan_tag -  Indicates the vlan tag value.

 @Return		None.

 @Cautions	The parse results must be updated before calling this operation.
 If an ethernet header is present, it is assumed to be located at
 the beginning of the segment (offset 0 from segment address).
 If there is no ethernet header, the vlan is inserted at the
 beginning of the segment.
 *//***************************************************************************/
void l2_push_and_set_vlan(uint32_t vlan_tag);

/*************************************************************************//**
 @Function	l2_pop_vlan

 @Description	Pop the outer VLAN tag.
 The parse results are updated automatically at the end of
 this operation.

 The gross running sum of the frame becomes invalid after calling
 this function.

 Implicit input parameters in Task Defaults: frame handle,
 segment handle, segment address.

 @Return		Success or Failure (There was no VLAN in the frame).

 @Cautions	The parse results must be updated before calling this operation.
 This function assumes the presence of the ethernet header.
 *//***************************************************************************/
int l2_pop_vlan(void);

/*************************************************************************//**
 @Function	l2_push_and_set_mpls

 @Description	Push and set a new outer MPLS.\n

 This function assumes the presence of an Ethernet header.
 The parse results are updated automatically at the end of this operation.
 The gross running sum of the frame becomes invalid after calling this function.

 Implicit input parameters in Task Defaults: frame handle, segment handle, 
 segment address.
 Implicit output parameters in Task Defaults: parser_starting_hxs

 @Param[in]	mpls_hdr - indicates the mpls header value.
 @Param[in] etype - needed in case there is no MPLS header in frame, indicates
		  the EtherType of MPLS (0x8847 or 0x8848). otherwise can be NULL.

 @Return	None.
 
 @Cautions  In case there is no MPLS header in the frame, the user should set
 the S bit to 1. In addition the user should insert label according to the IP
 version (IPv4 - 0,IPv6 - 2). incorrect user inputs will cause parse error.
 
 @Cautions	The parse results must be updated before calling this operation.
 If an ethernet header is present, it is assumed to be located at
 the beginning of the segment (offset 0 from segment address).
 *//***************************************************************************/
void l2_push_and_set_mpls(uint32_t mpls_hdr, uint16_t etype);

/*************************************************************************//**
 @Function	l2_pop_mpls

 @Description	Pop the outer MPLS.
  
 This function assumes Ethernet , MPLS  and IP headers are presents.
 The parse results are updated automatically at the end of this operation.
 The gross running sum of the frame becomes invalid after calling this function.

 Implicit input parameters in Task Defaults: frame handle, segment handle, 
 segment address.

 @Cautions	The parse results must be updated before calling this operation.

 *//***************************************************************************/
void l2_pop_mpls(void);

/*************************************************************************//**
 @Function	l2_mpls_header_remove

 @Description	MPLS Header removal. Remove the stacked MPLS if exists.
 (Unlimited stacked MPLS).

 This function assumes Ethernet , MPLS  and IP headers are presents.

 The parse results are updated automatically at the end of this operation.

 The gross running sum of the frame becomes invalid after calling this function.

 Implicit input parameters in task defaults: frame handle, segment handle, 
 parser_profile_id, parser_starting_hxs.
 Implicitly updated values in task defaults: segment length, segment address,
 parser_starting_hxs.

 @Cautions	The parse results must be updated before calling this operation.

 *//***************************************************************************/
void l2_mpls_header_remove(void);

/*************************************************************************//**
 @Function	l2_push_and_set_vxlan

 @Description	Push and set a VxLAN headers.\n

 This function assumes the presence of an Ethernet header.

 The parse results are updated automatically at the end of this operation.

 The gross running sum of the frame becomes invalid after calling this function.

 Implicit input parameters in Task Defaults: frame handle, segment handle, 
 segment address.
 Implicit output parameters in Task Defaults: parser_starting_hxs


 @Param[in]	vxlan_hdr -  Pointer to VxLAN encapsulate headers: Ethernet, IP,
 	 	 	 UDP and VxLan.
 @Param[in]	size -  Indicate the size of VxLAN encapsulate headers.
 
 @Return		None.

 @Cautions	The parse results must be updated before calling this operation.

 *//***************************************************************************/
void l2_push_and_set_vxlan(uint8_t *vxlan_hdr, uint16_t size);

/*************************************************************************//**
 @Function	l2_pop_vxlan

 @Description	Pop the VxLAN headers.
 The parse results are updated automatically at the end of
 this operation.

 This function assumes Ethernet, IP, UDP and VxLAN headers are presents.

 The gross running sum of the frame becomes invalid after calling this function.

 Implicit input parameters in Task Defaults: frame handle, segment handle, 
 segment address.

 @Return		None.

 @Cautions	The parse results must be updated before calling this operation.
 
 *//***************************************************************************/
void l2_pop_vxlan(void);

/*************************************************************************//**
 @Function	l2_set_vxlan_vid

 @Description	Set the VxLAN id.

 This function assumes VxLAN header is present.

 The gross running sum of the frame becomes invalid after calling this function.

 Implicit input parameters in Task Defaults: frame handle, segment handle,
 segment address.

 @Param[in]	vxlan_vid - VxLAN Network identifier (3 LSB bytes).

 @Return		None.

 @Cautions	The parse results must be updated before calling this operation.
 
 *//***************************************************************************/
void l2_set_vxlan_vid(uint32_t vxlan_vid);

/*************************************************************************//**
 @Function	l2_set_vxlan_flags

 @Description	Set the VxLAN flags.

 This function assumes VxLAN header is present.

 The gross running sum of the frame becomes invalid after calling this function.

 Implicit input parameters in Task Defaults: frame handle, segment handle,
 segment address.

 @Param[in]	flags - VxLAN flags.

 @Return		None.

 @Cautions	The parse results must be updated before calling this operation.

 *//***************************************************************************/
void l2_set_vxlan_flags(uint8_t flags);

/*************************************************************************//**
 @Function	l2_arp_response

 @Description	Creates an ARP Response frame from an ARP Request frame. This
		function Updates both ETH and ARP header fields to create an ARP
		Response frame.

		The function assumes the ARP Request source frame is the default
		frame, and that the parse results are updated.

		The gross running sum of the frame becomes invalid after calling
		this function.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Return		None.

@Cautions	The parse results must be updated before calling this
		operation.
@Cautions	This function assumes the presence of the ETH header and ARP
		Request header.
 *//***************************************************************************/
void l2_arp_response();

/*************************************************************************//**
 @Function	l2_set_hw_src_dst

 @Description	Sets ETH HW source and destination addresses. The function
		assumes the ETH source frame is the default frame, and that the
		parse results are updated.

		The gross running sum of the frame becomes invalid after calling
		this function.

		Implicit input parameters in Task Defaults: frame handle,
		segment handle, segment address.

@Param[in]	target_hw_addr -  Target MAC address.

@Return		None.

@Cautions	The parse results must be updated before calling this
		operation.
@Cautions	This function assumes the presence of the ETH header.
 *//***************************************************************************/
void l2_set_hw_src_dst(uint8_t *target_hw_addr);

/** @} end of group FSL_HM_L2_Functions */
/** @} end of group AIOP_L2_HM */
/** @} end of group AIOP_L2 */
/** @} end of group NETF */

#endif /* __FSL_L2_H */
