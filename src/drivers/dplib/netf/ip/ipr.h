/**************************************************************************//**
@File          ipr.h

@Description   This file contains IPR internal functions and definitions

	       Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_IPR_H
#define __AIOP_IPR_H

/**************************************************************************//**
@addtogroup	FSL_IPR FSL_AIOP_IPR

@Description	AIOP IP reassembly functions macros and definitions

@{
*//***************************************************************************/


/**************************************************************************//**
@Group		IPR_Internal Internal IPR functions & Definitions

@Description	Internal IP reassembly

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		IPRInitFlags IPR init flags

@Description	IP reassembly init flags.


|   0   |   1   |  2-5 |      6-7      |
|-------|-------|------|---------------|
|IPv4_EN|IPv6_EN|      | Table_location|
\n
| 8 | 9 |     10-11    | 12 |   13 - 15   | 16 - 31  |
|---|---|--------------|----|-------------|----------|
|   |   |AIOP_priority |TPRI| Granularity |          |

Recommended default values: Granularity:IPR_MODE_100_USEC_TO_GRANULARITY
			    TPRI : not set (low priority)
			    AIOP task priority: low
@{
*//***************************************************************************/

/** If set, IPR supports reassembly of IPv4 frames*/
#define IPR_IPV4_EN				0x80000000
/** If set, IPR supports reassembly of IPv6 frames*/
#define IPR_IPV6_EN				0x40000000

/* The following defines will be used to set the timeout timer tick size.*/
/** 1 uSec timeout timer ticks*/
#define IPR_MODE_USEC_TO_GRANULARITY		0x00000000
/** 10 uSec timeout timer ticks*/
#define IPR_MODE_10_USEC_TO_GRANULARITY		0x00010000
/** 100 uSec timeout timer ticks*/
#define IPR_MODE_100_USEC_TO_GRANULARITY	0x00020000
/** 1 mSec timeout timer ticks*/
#define IPR_MODE_MSEC_TO_GRANULARITY		0x00030000
/** 10 mSec timeout timer ticks*/
#define IPR_MODE_10_MSEC_TO_GRANULARITY		0x00040000
/** 100 mSec timeout timer ticks*/
#define IPR_MODE_100_MSEC_TO_GRANULARITY	0x00050000
/** 1 Sec timeout timer ticks*/
#define IPR_MODE_SEC_TO_GRANULARITY		0x00060000

/** If set, timeout priority task is high. */
#define IPR_MODE_TPRI				0x00080000

/* The following defines will be used to set the AIOP task priority
	of the created timeout task.*/
/** Low priority AIOP task*/
#define IPR_MODE_LOW_PRIORITY_TASK		0x00000000
/** Middle priority AIOP task*/
#define IPR_MODE_MID_PRIORITY_TASK		0x00100000
/** High priority AIOP task*/
#define IPR_MODE_HIGH_PRIORITY_TASK		0x00200000

/** Tables are located in dedicated RAM */
#define IPR_MODE_TABLE_LOCATION_INT		0x00000000
/** Tables are located in Packet Express Buffer table */
#define IPR_MODE_TABLE_LOCATION_PEB		0x10000000
/** Tables are located in DDR */
#define IPR_MODE_LOCATION_EXT			0x30000000

/* @} end of group IPRInitFlags */

/**************************************************************************//**
@Function	ipr_init

@Description	Initialize the IP Reassembly infrastructure.
		This function should be called once.
		No IPR function can be called before this function was invoked.
		This function initializes two KeyIDs (one for IPv4 and one
		for Ipv6) : IPsrc-IPdst-protocol-identification

@Param[in]	max_buffers - maximum number of buffers to be used as
		context buffers for all the instances.\n
		The size of each buffer should be at least 2240 bytes.\n
		Buffers should be aligned to 64 bytes.
@Param[in]	flags - \link IPRInitFlags IPR init flags \endlink
@Param[in]	tmi_id - TMAN instance ID to be used for IPR process.


@Return		None.

@Cautions	None.
*//***************************************************************************/
void ipr_init(uint32_t max_buffers, uint32_t flags, uint8_t tmi_id);

/**************************************************************************//**
@Description	IPR Global parameters
*//***************************************************************************/

struct ipr_global_parameters {
/** Initialized to max_buffers and will be decremented upon each
 * create instance */
uint32_t ipr_avail_buffers_cntr;
/** Pool id returned by the ARENA allocator to be used as context buffer pool */
uint8_t  ipr_pool_id;
/** Size of the allocated buffers by the ARENA. These buffers are associated to
    the ipr_pool_id */
uint16_t ipr_buffer_size;
/** Should got either as a global define or as a return parameter from
    a dedicated  ARENA function (epid = get_tmi_epid(tmi_id)).*/
uint8_t  ipr_timeout_epid;
uint8_t  ipr_key_id_ipv4;
uint8_t  ipr_key_id_ipv6;
uint8_t  ipr_tmi_id;
};

/* @} end of group IPR_Internal */
/* @} end of group FSL_IPR */

#endif /* __AIOP_IPR_H */
