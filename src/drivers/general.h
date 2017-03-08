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

/**************************************************************************//**
@File		general.h

@Description	This file contains AIOP SW internal general definitions.

*//***************************************************************************/


#ifndef __GENERAL_H_
#define __GENERAL_H_


#include "fsl_general.h"
#include "fsl_stdio.h"

#pragma section RW ".exception_data" ".exception_data_bss"


/**************************************************************************//**
 @Group		AIOP_GENERAL AIOP General

 @Description	AIOP General macros and functions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_General_Definitions AIOP SL General Definitions

 @Description	AIOP General Definitions

 @{
*//***************************************************************************/


/**************************************************************************//**
 @Group		AIOP_General_Macros

 @Description	AIOP General Macros

 @{
*//***************************************************************************/

	/** Null buffer pool ID */
#define BPID_NULL	0x00FF
	/** Logical false */
#define FALSE	0
	/** Logical true */
#define TRUE	1

/* Address of end of TLS section */
extern const uint16_t TLS_SECTION_END_ADDR[];

/* Address of start of SRAM section */
extern const uint8_t _ssram_addr[];


/** @} */ /* end of AIOP_General_Macros */


/**************************************************************************//**
 @Group		AIOP_Accelerators_IDS

 @Description	AIOP Accelerators IDs

 @{
*//***************************************************************************/

/* Accelerators IDs (from AIOP Source IDs section in ArchDef) */
	/** Accelerator ID for yield command*/
#define YIELD_ACCEL_ID			0x00
	/** TMAN accelerator ID */
#define TMAN_ACCEL_ID			0x01
	/** MFLU accelerator ID */
#define MFLU_ACCEL_ID			0x02
	/** AAP SEC accelerator ID */
#define AAP_SEC_ACCEL_ID		0x03
	/** PARSER & CLASSIFIER accelerator ID */
#define CTLU_PARSE_CLASSIFY_ACCEL_ID	0x04
	/** CTLU accelerator ID */
#define CTLU_ACCEL_ID			0x05
	/** Frame Presentation DMA accelerator ID */
#define FPDMA_ACCEL_ID			0x0C
	/** Frame Output DMA accelerator ID */
#define FODMA_ACCEL_ID			0x0E
	/** SCDMA accelerator ID */
#define CDMA_ACCEL_ID			0x0D

/** @} */ /* end of AIOP_Accelerators_IDS */

/**************************************************************************//**
 @Group		AIOP_General_Protocols

 @Description	AIOP general protocols definitions and structures.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		FSL_General_Protocols_Macros General Protocols Macros

@Description	Freescale AIOP General Protocols Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		AIOP_General_Protocols_IP_Definitions Internet Protocol (IP) \
		 definitions
@{
*//***************************************************************************/


#define IPV6_FRAGMENT_HEADER_LENGTH   8    /*!< IPv6 fragment header length */

/** @} */ /* end of AIOP_General_Protocols_IP_Definitions */

/**************************************************************************//**
@Group		AIOP_General_Protocols_IPV4_HDR_Masks IPv4 Header Masks
@{
*//***************************************************************************/
#define IPV4_HDR_VERSION_MASK	  0xF0   /*!< IPv4 Version mask */
#define IPV4_HDR_IHL_MASK	  0x0F   /*!< IPv4 IHL mask */
#define IPV4_HDR_X_FLAG_MASK	  0x8000 /*!< IPv4 evil bit mask */
#define IPV4_HDR_D_FLAG_MASK	  0x4000 /*!< IPv4 don't fragment bit mask */
#define IPV4_HDR_M_FLAG_MASK	  0x2000 /*!< IPv4 more fragments bit mask */
#define IPV4_HDR_FRAG_OFFSET_MASK 0x1FFF /*!< IPv4 fragment offset mask */

/** @} */ /* end of AIOP_General_Protocols_IPV4_HDR_Masks */

/**************************************************************************//**
@Group		AIOP_General_Protocols_IPV4_HDR_Offsets IPv4 Header Offsets
@{
*//***************************************************************************/
#define IPV4_HDR_VERSION_OFFSET     4 /*!< IPv4 header Version field offset*/
#define IPV4_HDR_IHL_OFFSET	    0 /*!< IPv4 header IHL field offset*/
#define IPV4_HDR_X_FLAG_OFFSET      15/*!< IPv4 evil bit offset */
#define IPV4_HDR_D_FLAG_OFFSET      14/*!< IPv4 don't fragment bit offset */
#define IPV4_HDR_M_FLAG_OFFSET      13/*!< IPv4 more fragments bit offset */
#define IPV4_HDR_FRAG_OFFSET_OFFSET 0 /*!< IPv4 fragment offset field offset */

/** @} */ /* end of AIOP_General_Protocols_IPV4_HDR_Offsets */

/**************************************************************************//**
@Group		AIOP_General_Protocols_IPV6_Extensions IPv6 Extensions
@{
*//***************************************************************************/
#define  IPV6_EXT_HOP_BY_HOP	0 /*!< IPv6 Hop By Hop extension number*/
#define  IPV6_EXT_ROUTING	43 /*!< IPv6 Routing extension number*/
#define  IPV6_EXT_DESTINATION	60 /*!< IPv6 Destination extension number*/
#define  IPV6_EXT_AH		51 /*!< IPv6 Authentication extension number*/
#define  IPV6_EXT_FRAGMENT	44 /*!< IPv6 Fragment extension number*/

/** @} */ /* end of AIOP_General_Protocols_IPV6_Extensions */

/**************************************************************************//**
@Group		AIOP_General_Protocols_IPV6_FRAG_HDR_Offsets IPv6 Frag Header
		Offsets
@{
*//***************************************************************************/
#define IPV6_HDR_M_FLAG_MASK 0x0001 /** Mask for IPv6 Fragment Header M flag */

/** @} */ /* end of AIOP_General_Protocols_IPV6_FRAG_HDR_Offsets */

/**************************************************************************//**
@Group		AIOP_General_Protocols_TCP_Definitions Transmission Control
		Protocol (TCP) Definitions
@{
*//***************************************************************************/
#define TCP_HDR_LENGTH		      20   /**< TCP header length */
#define TCP_PROTOCOL		      6   /**< TCP Protocol number */

/** @} */ /* end of AIOP_General_Protocols_TCP_Definitions */

/**************************************************************************//**
@Group		AIOP_General_Protocols_UDP_Definitions
@{
*//***************************************************************************/
#define UDP_HDR_LENGTH		      8   /**< UDP header length */
#define UDP_PROTOCOL		      17   /**< UDP Protocol number */

/** @} */ /* end of AIOP_General_Protocols_UDP_Definitions */

/** @} */ /* end of FSL_General_Protocols_Macros */
/** @} */ /* end of AIOP_General_Protocols */
/** @} */ /* end of AIOP_General_Definitions */

/**************************************************************************//**
@Group		FSL_General_Macros General Macros

@Description	Freescale AIOP General Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		AIOP_Alignment_Macro
@{
*//***************************************************************************/
															
#define  ALIGNMENT_4B	0x03 /*!< 4B Alignment*/
#define  ALIGNMENT_8B	0x07 /*!< 8B Alignment*/
#define  ALIGNMENT_16B	0xf  /*!< 16B Alignment*/
#define  ALIGNMENT_32B	0x1f /*!< 32B Alignment*/
#define  ALIGNMENT_64B	0x3f /*!< 32B Alignment*/

#define DEBUG_ALIGN(filename, var_addr, alignment)							\
	({if (((uint32_t)var_addr & alignment) != 0)							\
			exception_handler("filename", "", __LINE__, "Alignment Error"); })	

/** @} */ /* end of AIOP_Alignment_Macro */
/** @} */ /* end of FSL_General_Macros */

/**************************************************************************//**
 @Group		General_Functions General Functions

 @Description	General Functions

 @{
*//***************************************************************************/

/**************************************************************************//**
@Function	update_random_64bit

@Description	This function updates the random_64bit parameter in the TLS.

@Return		None.
*//***************************************************************************/
void update_random_64bit(void);

/* TEMP function (TODO Remove!) */
/* Allow exception_handler() to be redefined */
__declspec(weak) void exception_handler(char *filename,
		       char *function_name,
		       uint32_t line,
		       char *message) __attribute__ ((noreturn));

#ifdef AIOP_VERIF
/**************************************************************************//**
@Function	trim_path_prefix

@Description	Trims the prefix of a given file path.
		It removes all the path but the file name (i.e. all the path up
		to the last '/'). If the pointer to the file path string is
		NULL or points to an empty string the returned string will be
		identical to the given string.

@Param[in]	filepath - The file path to be trimmed.

@Return		The trimmed file name.
*//***************************************************************************/
char * trim_path_prefix(char *filepath);
#endif /*AIOP_VERIF*/

/** @} */ /* end of General_Functions */
/** @} */ /* end of AIOP_GENERAL */

#endif /* __GENERAL_H_ */
