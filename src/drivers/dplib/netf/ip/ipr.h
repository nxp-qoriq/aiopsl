/**************************************************************************//**
@File          ipr.h

@Description   This file contains IPR internal functions and definitions

	       Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __AIOP_IPR_H
#define __AIOP_IPR_H

#include "common/types.h"
#include "dplib/fsl_ipr.h"
#include "dplib/fsl_osm.h"
#include "fdma.h"


/**************************************************************************//**
@addtogroup	FSL_IPR FSL_AIOP_IPR

@Description	AIOP IP reassembly functions macros and definitions

@{
*//***************************************************************************/

#define OUT_OF_ORDER			0x00000001
#define	MAX_NUM_OF_FRAGS 		64
#define	FRAG_OK_REASS_NOT_COMPL		0
#define LAST_FRAG_IN_ORDER		1
#define LAST_FRAG_OUT_OF_ORDER		2
#define FRAG_ERROR			3
#define NO_BYPASS_OSM			0x00000000
#define	BYPASS_OSM			0x00000001
#define START_CONCURRENT		0x00000002
#define	RESET_MF_BIT			0xDFFF
#define NO_ERROR			0
#define IPR_CONTEXT_SIZE		2624
#define LINK_LIST_ELEMENT_SIZE		sizeof(struct link_list_element)
#define LINK_LIST_SIZE			LINK_LIST_ELEMENT_SIZE*MAX_NUM_OF_FRAGS
#define SIZE_TO_INIT 			RFDC_SIZE+LINK_LIST_SIZE
#define RFDC_VALID			0x80000000
#define FRAG_OFFSET_MASK		0x1FFF
#define IPV4_FRAME			0x00000000 /* in RFDC status */
#define IPV6_FRAME			0x00000001 /* in RFDC status */
#define INSTANCE_VALID			0x0001
#define REF_COUNT_ADDR_DUMMY		HWC_ACC_OUT_ADDRESS+CDMA_REF_CNT_OFFSET
#define IPR_INSTANCE_SIZE		sizeof(struct ipr_instance)
#define RFDC_SIZE			sizeof(struct ipr_rfdc)
#define RFDC_SIZE_NO_KEY		sizeof(struct ipr_rfdc)-4
#define FD_SIZE				sizeof(struct ldpaa_fd)

/* todo should move to general or OSM include file */
#define CONCURRENT				0
#define EXCLUSIVE				1

#define IS_LAST_FRAGMENT() !(ipv4hdr_ptr->flags_and_offset & IPV4_HDR_M_FLAG_MASK)
#define LAST_FRAG_ARRIVED()	rfdc_ptr->expected_total_length

struct ipr_instance {
	uint64_t	extended_stats_addr;
	/** maximum concurrently IPv4 open frames. */
	uint16_t	table_id_ipv4;
	uint16_t	table_id_ipv6;
	uint32_t    	max_open_frames_ipv4;
	uint32_t  	max_open_frames_ipv6;
	uint16_t  	max_reass_frm_size;	/** maximum reassembled frame size */
	uint16_t  	min_frag_size;	/** minimum fragment size allowed */
	uint16_t  	timeout_value_ipv4;/** reass timeout value for ipv4 */
	uint16_t  	timeout_value_ipv6;/** reass timeout value for ipv6 */
	/** function to call upon Time Out occurrence for ipv4 */
	ipr_timeout_cb_t *ipv4_timeout_cb;
	/** function to call upon Time Out occurrence for ipv6 */
	ipr_timeout_cb_t *ipv6_timeout_cb;
	/** \link FSL_IPRInsFlags IP reassembly flags \endlink */
	uint32_t  	flags;
	/** Argument to be passed upon invocation of the IPv4 callback
	    function*/
	ipr_timeout_arg_t cb_timeout_ipv4_arg;
	/** Argument to be passed upon invocation of the IPv6 callback
	    function*/
	ipr_timeout_arg_t cb_timeout_ipv6_arg;
	/** Number of frames that started reassembly but didn't complete it yet */
	uint16_t	num_of_open_reass_frames;
	uint8_t		tmi_id;
		/** 32-bit alignment. */
	uint8_t  pad[1];
};

#pragma pack(push,1) 
struct ipr_rfdc{
	uint64_t	instance_handle;
	uint32_t	timer_handle;
	uint16_t	expected_total_length;
	uint16_t	current_total_length;
	uint16_t	first_frag_offset;
	uint16_t	last_frag_offset;
	uint16_t	current_running_sum;
	uint8_t		first_frag_index;
	uint8_t		last_frag_index;
	uint8_t		next_index;
	uint8_t		index_to_out_of_order;
	uint8_t		num_of_frags;
	uint8_t		res;
	uint32_t	status;
	struct 		fdma_isolation_attributes isolation_bits; // 4 bytes
	uint64_t	ipv4_key[2];
	uint32_t	res1[3];
};
#pragma pack(pop)

#pragma pack(push,1) 
struct extended_ipr_rfc{
	uint64_t	ipv6_key[4];
	uint32_t	ipv6_key_cont;
};
#pragma pack(pop)


struct link_list_element{
	uint16_t	frag_offset;
	uint16_t	frag_length;
	uint16_t	res;
	uint8_t		prev_index;
	uint8_t 	next_index;
};


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
#define IPR_MODE_TABLE_LOCATION_PEB		0x02000000
/** Tables are located in DDR */
#define IPR_MODE_TABLE_LOCATION_EXT1		0x03000000 /* Doron */

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


@Return		None.

@Cautions	None.
*//***************************************************************************/
void ipr_init(uint32_t max_buffers, uint32_t flags);

/**************************************************************************//**
@Function	ipr_insert_to_link_list

@Description	Insert to Link List - Save FD

@Param[in]	rfdc_ptr - pointer to RFDC in workspace (on stack)
@Param[in]	rfdc_ext_addr - pointer to RFDC in external memory.

@Return		Status - Success or Failure.

@Cautions	None.
*//***************************************************************************/

uint32_t ipr_insert_to_link_list(struct ipr_rfdc *rfdc_ptr,
				 uint64_t rfdc_ext_addr);

uint32_t closing_in_order(struct ipr_rfdc *rfdc_ptr, uint64_t rfdc_ext_addr);
uint32_t closing_with_reordering(struct ipr_rfdc *rfdc_ptr,
				 uint64_t rfdc_ext_addr);


inline void move_to_correct_ordering_scope1(uint32_t osm_status)
{
		if(osm_status == 0) {
			/* return to original ordering scope that entered
			 * the ipr_reassemble function */
			osm_scope_exit();
		} else if(osm_status & START_CONCURRENT) {
		   osm_scope_transition_to_concurrent_with_increment_scope_id();
		}
}
void move_to_correct_ordering_scope2(uint32_t osm_status);
/* todo restore this inline after compiler fix.
inline void move_to_correct_ordering_scope2(uint32_t osm_status)
{
		if(osm_status == 0) { */
			/* return to original ordering scope that entered
			 * the ipr_reassemble function */
	/*		osm_scope_exit();
			osm_scope_exit();	
		} else if(osm_status & START_CONCURRENT) {
		  osm_scope_transition_to_concurrent_with_increment_scope_id();
		}
}*/
uint32_t ip_header_update_and_l4_validation(struct ipr_rfdc *rfdc_ptr);

uint32_t check_for_frag_error();

void ipr_time_out();

/**************************************************************************//**
@Description	IPR Global parameters
*//***************************************************************************/

struct ipr_global_parameters {
/** Initialized to max_buffers and will be decremented upon each
 * create instance */
uint32_t ipr_avail_buffers_cntr;
/** Size of the allocated buffers by the ARENA. These buffers are associated to
    the ipr_pool_id */
uint16_t ipr_buffer_size;
/** save ipr table location */
uint8_t  ipr_table_location;
uint8_t  ipr_timeout_flags;
uint8_t  ipr_key_id_ipv4;
uint8_t  ipr_key_id_ipv6;
/** Pool id returned by the ARENA allocator to be used as context buffer pool */
uint8_t  ipr_pool_id;
uint8_t  ipr_instance_spin_lock;
uint8_t res;
};

/* @} end of group IPR_Internal */
/* @} end of group FSL_IPR */

#endif /* __AIOP_IPR_H */
