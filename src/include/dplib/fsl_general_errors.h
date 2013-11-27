/**************************************************************************//**
@File		fsl_header_modification_errors.h

@Description	This file contains the AIOP Header Modification Errors

		Copyright 2013 Freescale Semiconductor, Inc.

*//***************************************************************************/
#ifndef __FSL_HEADER_MODIFICATION_ERRORS_H
#define __FSL_HEADER_MODIFICATION_ERRORS_H

/**************************************************************************//**
@addtogroup	FSL_HM FSL_AIOP_Header_Modification

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		HMReturnStatus Header Modification functions return status

@{
*//***************************************************************************/


/** The frame contain only one IP header */
#define NO_IP_ENCAPSULATION_FOUND_ERROR		(HM_MODULE_STATUS_ID + 0x0100)
/** The frame not contain a UDP header */
#define NO_UDP_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0200)
/** No IP header was found.*/
#define NO_IP_HDR_ERROR				(HM_MODULE_STATUS_ID + 0x0300)
/** No VLAN exists.*/
#define NO_VLAN_ERROR				(HM_MODULE_STATUS_ID + 0x0400)
/** No TCP header was found */
#define NO_TCP_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0500)
/** No TCP MSS Option was found */
#define NO_TCP_MSS_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0600)
/** No TCP or UDP headers were found */
#define NO_L4_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0700)
/** No IP or L4 (TCP/UDP) headers were found */
#define NO_L4_IP_FOUND_ERROR			(HM_MODULE_STATUS_ID + 0x0800)


/* @} end of group HMReturnStatus */
/* @} end of group FSL_HM */

#endif /* __FSL_HEADER_MODIFICATION_ERRORS_H */

