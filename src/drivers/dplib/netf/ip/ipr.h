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
#include "dplib/fsl_tman.h"
#include "fdma.h"


/**************************************************************************//**
@addtogroup	FSL_IPR FSL_AIOP_IPR

@Description	AIOP IP reassembly functions macros and definitions

@{
*//***************************************************************************/

#define OUT_OF_ORDER			0x0001
#define ORDER_AND_OOO			0x0002
#define	MAX_NUM_OF_FRAGS 		64
#define	FRAG_OK_REASS_NOT_COMPL	0
#define LAST_FRAG_IN_ORDER		1
#define LAST_FRAG_OUT_OF_ORDER	2
#define FRAG_ERROR				3
#define NO_BYPASS_OSM			0x00000000
#define	BYPASS_OSM				0x00000001
#define START_CONCURRENT		0x00000002
#define	RESET_MF_BIT			0xDFFF
#define NO_ERROR				0
#define IPR_CONTEXT_SIZE		2688
#define START_OF_LINK_LIST		RFDC_SIZE+RFDC_EXTENSION_SIZE
#define START_OF_FDS_LIST		START_OF_LINK_LIST+LINK_LIST_SIZE
#define LINK_LIST_ELEMENT_SIZE	sizeof(struct link_list_element)
#define LINK_LIST_OCTET_SIZE	8*LINK_LIST_ELEMENT_SIZE
#define LINK_LIST_SIZE			LINK_LIST_ELEMENT_SIZE*MAX_NUM_OF_FRAGS
#define SIZE_TO_INIT 			RFDC_SIZE+LINK_LIST_SIZE
#define RFDC_VALID				0x8000
#define FRAG_OFFSET_IPV4_MASK	0x1FFF
#define FRAG_OFFSET_IPV6_MASK   0xFFF8
#define IPV4_FRAME				0x00000000 /* in RFDC status */
#define IPV6_FRAME				0x00000001 /* in RFDC status */
#define INSTANCE_VALID			0x0001
#define REF_COUNT_ADDR_DUMMY	HWC_ACC_OUT_ADDRESS+CDMA_REF_CNT_OFFSET
#define IPR_INSTANCE_SIZE		sizeof(struct ipr_instance)
#define RFDC_SIZE				sizeof(struct ipr_rfdc)
#define RFDC_EXTENSION_SIZE		sizeof(struct extended_ipr_rfc)
#define RFDC_EXTENSION_TRUNCATED_SIZE	40
#define FD_SIZE					sizeof(struct ldpaa_fd)
#define OCTET_LINK_LIST_MASK	0x07
#define IPV4_KEY_SIZE			11
#define IPV6_FIXED_HEADER_SIZE	40
#define IPR_TIMEOUT_FLAGS	TMAN_CREATE_TIMER_MODE_MSEC_GRANULARITY | \
				TMAN_CREATE_TIMER_MODE_TPRI | \
				TMAN_CREATE_TIMER_ONE_SHOT | \
				TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK

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
	uint32_t    max_open_frames_ipv4;
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
	uint32_t	num_of_open_reass_frames_ipv4;
	uint32_t	num_of_open_reass_frames_ipv6;
	uint32_t	ipv4_reass_frm_cntr;
	uint32_t	ipv6_reass_frm_cntr;
	/* TMAN Instance ID */
	uint8_t		tmi_id;
	uint8_t		reserved;
	uint16_t	bpid;
};

#pragma pack(push,1)
struct ipr_rfdc{
	/* 64 bytes */
	uint64_t	instance_handle;
	uint32_t	timer_handle;
	uint16_t	expected_total_length;
	uint16_t	current_total_length;
	uint16_t	first_frag_offset;
	uint16_t	biggest_payload;
	uint16_t	current_running_sum;
	uint8_t		first_frag_index;
	uint8_t		last_frag_index;
	uint8_t		next_index;
	uint8_t		index_to_out_of_order;
	uint8_t		num_of_frags;
	uint8_t		res;
	uint16_t	status;
	uint16_t	total_in_order_payload;
	struct 		fdma_amq isolation_bits; // 4 bytes
	uint64_t	ipv4_key[2];
	uint32_t	res1[3];
};
#pragma pack(pop)

#pragma pack(push,1)
struct extended_ipr_rfc{
	/* 64 bytes */
	uint32_t	ipv6_key[10];
	uint32_t	res[6];
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
@Group		IPR_Internal Internal IPR functions

@Description	Internal IP reassembly

@{
*//***************************************************************************/




/**************************************************************************//**
@Function	ipr_init

@Description	Initialize the IP Reassembly infrastructure.
		This function should be called once.
		No IPR function can be called before this function was invoked.
		This function initializes two KeyIDs (one for IPv4 and one
		for Ipv6) : IPsrc-IPdst-protocol-identification

Implicit:	max_buffers - maximum number of buffers to be used as
		context buffers for all the instances.\n
		The size of each buffer should be at least 2240 bytes.\n
		Buffers should be aligned to 64 bytes.
		flags - \link IPRInitFlags IPR init flags \endlink


@Return		reflect the return value from slab_find_and_fill_bpid().
		0       - on success,
               -ENAVAIL - could not release into bpid
               -ENOMEM  - not enough memory for mem_partition_id.

@Cautions	None.
*//***************************************************************************/
int ipr_init(void);

/**************************************************************************//**
@Function	ipr_insert_to_link_list

@Description	Insert to Link List - Save FD

@Param[in]	rfdc_ptr - pointer to RFDC in workspace (on stack)
@Param[in]	rfdc_ext_addr - pointer to RFDC in external memory.

@Return		Status - Success or Failure.

@Cautions	None.
*//***************************************************************************/

uint32_t ipr_insert_to_link_list(struct ipr_rfdc *rfdc_ptr,
							     uint64_t rfdc_ext_addr, 
							     void     *iphdr_ptr);

uint32_t closing_in_order(uint64_t rfdc_ext_addr, uint8_t num_of_frags);

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
uint32_t ipv4_header_update_and_l4_validation(struct ipr_rfdc *rfdc_ptr);
uint32_t ipv6_header_update_and_l4_validation(struct ipr_rfdc *rfdc_ptr);

uint32_t check_for_frag_error();

void ipr_time_out();

void check_remove_padding();

uint32_t out_of_order(struct ipr_rfdc *rfdc_ptr, uint64_t rfdc_ext_addr,
					  struct ipv4hdr *ipv4hdr_ptr,uint16_t current_frag_size,
					  uint16_t frag_offset_shifted); 


/**************************************************************************//**
@Description	IPR Global parameters
*//***************************************************************************/

struct ipr_global_parameters {
uint8_t  ipr_key_id_ipv4;
uint8_t  ipr_key_id_ipv6;
};

/* @} end of group FSL_IPR */

#endif /* __AIOP_IPR_H */
