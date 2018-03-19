/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************//**
@File          ipr.h

@Description   This file contains IPR internal functions and definitions

*//***************************************************************************/

#ifndef __AIOP_IPR_H
#define __AIOP_IPR_H

#include "fsl_types.h"
#include "fsl_ipr.h"
#include "fsl_osm.h"
#include "fsl_tman.h"
#include "fsl_fdma.h"

#include "sparser_drv.h"

/**************************************************************************//**
@addtogroup	FSL_IPR FSL_AIOP_IPR

@Description	AIOP IP reassembly functions macros and definitions

@{
*//***************************************************************************/

#define IPR_MEM_ALIGN 64

/* IPR_SW_TABLE */
#ifdef USE_IPR_SW_TABLE

/* size of the IPR_SW_TABLE */
#define FRAG_BINS		2048
#define FRAGS_PER_BIN	8

/*
 * IPR module uses software hash tables instead of CTLU ones.
 *
 * The table lines are created on demand.
 * With the default values a table has 128kB and a line 512 bytes.
 * The table lines are obtained from SLAB so a new DPBP is needed for IPR.
 *
 * The lookup key is SW extracted. From it a hash is generated
 * and reduced to a line number.
 *
 * We protect the IPR resources with OSM transitions in exclusive mode.
 * scope_id (uint32_t) has a portion (0x3F) reserved for OSM infrastructure.
 * We could use virtual addresses for scope_id since they are unique.
 * But the virtual addresses must be aligned to 64 to have the reserve
 * portion of the scope_id zero.
 *
 * Possible improvements:
 * - separate line size for ipv4 and ipv6
 * - extend the table line when it's full
 * - compact table line
 */

#define SW_IPR_OSM_MASK		(OSM_SCOPE_ID_STAGE_INCREMENT_MASK | \
				 OSM_SCOPE_ID_LEVEL_INCREMENT_MASK)

#pragma pack(push, 1)
struct ipv4_fragment_key {
	uint32_t dst;
	uint32_t src;
	uint16_t id;
	uint8_t protocol;
};	/* 11 byte */
#pragma pack(pop)

#pragma pack(push, 1)
struct ipv6_fragment_key {
	uint32_t dst[4];
	uint32_t src[4];
	uint32_t id;
};	/* 36 byte */
#pragma pack(pop)

#pragma pack(push, 1)
union ip_fragment_key {
	struct ipv4_fragment_key ipv4_fk;
	struct ipv6_fragment_key ipv6_fk;
};	/* 36 byte */
#pragma pack(pop)

#pragma pack(push, 1)
struct sw_table_entry {
	uint32_t virt_addr;
	union ip_fragment_key fk;
};	/* 40 byte */
#pragma pack(pop)

#define IPR_SW_TABLE_LINE_SIZE	(FRAGS_PER_BIN * sizeof(struct sw_table_entry))
#endif	/* USE_IPR_SW_TABLE */

/***************************************************************************/

#define	MAX_NUM_OF_FRAGS 	64
#define	FRAG_OK_REASS_NOT_COMPL	0
#define LAST_FRAG_IN_ORDER	1
#define LAST_FRAG_OUT_OF_ORDER	2

enum ipr_invalid_frag {
	MALF_MIN_SIZE_IPV4 = 3,
	MALF_MIN_SIZE_IPV6,
	MALF_SIZE_M8,
	MALF_MAX_IP_SIZE,
	MALF_PAST_END,
	MALF_LAST_FRAG,
	MALF_OVERLAP_DUPLICATE,
	MALF_ECN,
	MALF_MAX_REASS_FRM_SZ
};

/* returns one of the IPR_MALFORMED flags */
static inline int GET_IPR_ERR(uint32_t stat)
{
	return (int)(IPR_ATOMIC_FRAG + ((stat - MALF_MIN_SIZE_IPV4 + 1) << 8));
}

#define NO_BYPASS_OSM		0x00000000 /* in osm_status */
#define	BYPASS_OSM		0x00000001 /* in osm_status */
#define START_CONCURRENT	0x00000002 /* in osm_status */
#define	RESET_MF_BIT		0xDFFF
#define NO_ERROR		0
#define IPR_CONTEXT_SIZE	2688
#define START_OF_LINK_LIST	RFDC_SIZE+RFDC_EXTENSION_SIZE
#define START_OF_FDS_LIST	START_OF_LINK_LIST+LINK_LIST_SIZE
#define LINK_LIST_ELEMENT_SIZE	sizeof(struct link_list_element)
#define LINK_LIST_OCTET_SIZE	8*LINK_LIST_ELEMENT_SIZE
#define LINK_LIST_SIZE		LINK_LIST_ELEMENT_SIZE*MAX_NUM_OF_FRAGS
#define SIZE_TO_INIT		RFDC_SIZE+LINK_LIST_SIZE
#define RFDC_VALID		0x8000 /* in RFDC status */
#define IPV4_FRAME		0x0000 /* in RFDC status */
#define IPV6_FRAME		0x4000 /* in RFDC status */
#define FIRST_ARRIVED		0x2000 /* in RFDC status */
#define RFDC_STATUS_NOT_ECT	0x0004 /* in RFDC status */
#define RFDC_STATUS_CE		0x0008 /* in RFDC status */
#define SCOPE_LEVEL		0x0030 /* in RFDC status */
/*following define includes both cases: pure OOO and OOO_and_IN_ORDER */
#define OUT_OF_ORDER		0x0001 /* in RFDC status */
#define ORDER_AND_OOO		0x0002 /* in RFDC status */
#define FRAG_OFFSET_IPV4_MASK	0x1FFF
#define FRAG_OFFSET_IPV6_MASK	0xFFF8
#define INSTANCE_VALID		0x0001
#define REF_COUNT_ADDR_DUMMY	(HWC_ACC_OUT_ADDRESS + CDMA_REF_CNT_OFFSET)
#define IPR_INSTANCE_SIZE	sizeof(struct ipr_instance)
#define RFDC_SIZE		sizeof(struct ipr_rfdc)
#define RFDC_EXTENSION_SIZE	sizeof(struct extended_ipr_rfdc)
#define RFDC_EXTENSION_TRUNCATED_SIZE	40
#define FD_SIZE			sizeof(struct ldpaa_fd)
#define OCTET_LINK_LIST_MASK	0x07
#define IPV4_KEY_SIZE		11
#define IPV6_KEY_SIZE		36
#define IPV6_FIXED_HEADER_SIZE	40
#define IPR_TIMEOUT_FLAGS	TMAN_CREATE_TIMER_MODE_10_MSEC_GRANULARITY | \
				TMAN_CREATE_TIMER_MODE_TPRI | \
				TMAN_CREATE_TIMER_ONE_SHOT | \
				TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK
#define IPV4_VALID		0x00000001	/* In IPR instance */
#define IPV6_VALID		0x00000002	/* In IPR instance */
#define MAX_IP_SIZE		65536 /* 64K */
#define NOT_ECT			0
#define CE			0x3
#define IPV4_ECN		0x3
#define IPV6_ECN		0x00300000
#define ENOSPC_TIMER		2
#define ENOMEM_TABLE		3
#define IPR_ERROR		1

/* todo should move to general or OSM include file */
#define CONCURRENT		0
#define EXCLUSIVE		1

#define LAST_FRAG_ARRIVED()	rfdc_ptr->expected_total_length

#pragma pack(push,1)
struct ipr_instance {
	uint64_t	extended_stats_addr;
	/** Argument to be passed upon invocation of the IPv4 callback
	    function*/
	ipr_timeout_arg_t cb_timeout_ipv4_arg;
	/** Argument to be passed upon invocation of the IPv6 callback
	    function*/
	ipr_timeout_arg_t cb_timeout_ipv6_arg;
	/** function to call upon Time Out occurrence for ipv4 */
	ipr_timeout_cb_t *ipv4_timeout_cb;
	/** function to call upon Time Out occurrence for ipv6 */
	ipr_timeout_cb_t *ipv6_timeout_cb;
	/** \link FSL_IPRInsFlags IP reassembly flags \endlink */
	uint32_t  	flags;
#ifdef USE_IPR_SW_TABLE
	/* virtual address IPR_SW_TABLE for IPv4 frames */
	uint32_t	table_id_ipv4;
	/* virtual address IPR_SW_TABLE for IPv6 frames */
	uint32_t	table_id_ipv6;
#else
	/* CTLU table ID for IPv4 frames */
	uint16_t	table_id_ipv4;
	/* CTLU table ID for IPv6 frames */
	uint16_t	table_id_ipv6;
#endif	/* USE_IPR_SW_TABLE */
	/** maximum reassembled frame size */
	uint16_t  	max_reass_frm_size;
	/* BPID to fetch buffers from */
	uint16_t	bpid;
	/** minimum fragment size allowed for IPv4 frames*/
	uint16_t  	min_frag_size_ipv4;
	/** minimum fragment size allowed for IPv4 frames*/
	uint16_t  	min_frag_size_ipv6;
	/** reass timeout value for ipv4 */
	uint16_t  	timeout_value_ipv4;
	/** reass timeout value for ipv6 */
	uint16_t  	timeout_value_ipv6;
	/* TMAN Instance ID */
	uint8_t		tmi_id;
#ifdef USE_IPR_SW_TABLE
	/* BPID to fetch buffers for ip fragment key */
	uint16_t	bpid_fk;
	uint8_t		res[5];
#else
	uint8_t		res[11];
#endif	/* USE_IPR_SW_TABLE */
};
#pragma pack(pop)

#pragma pack(push,1)
struct ipr_instance_extension{
	uint64_t	delete_arg;
	ipr_del_cb_t 	*confirm_delete_cb;
	uint32_t	ipv4_reass_frm_cntr;
	uint32_t	ipv6_reass_frm_cntr;
	/** Number of frames that IPv4 started reassembly
	    but didn't complete it yet */
	uint32_t	num_of_open_reass_frames_ipv4;
	/** Number of frames that IPv6 started reassembly
	    but didn't complete it yet */
	uint32_t	num_of_open_reass_frames_ipv6;
	/* Address to the memory used for the timers
	 * associated with the TMI */
	uint64_t	tmi_mem_base_addr;
};
#pragma pack(pop)

#pragma pack(push,1)
struct ipr_rfdc{
	/* 64 bytes */
	uint64_t	instance_handle;
	uint32_t	timer_handle;
	uint16_t	expected_total_length;
	uint16_t	current_total_length;
	uint16_t	first_frag_hdr_length;
	uint16_t	biggest_payload;
	uint16_t	current_running_sum;
	uint8_t		first_frag_index;
	uint8_t		last_frag_index;
	uint8_t		next_index;
	uint8_t		index_to_out_of_order;
	uint8_t		num_of_frags;
	uint8_t		res1;
	uint16_t	status;
	uint16_t	total_in_order_payload;
	uint64_t	ipv4_key[2]; /* this field should stay aligned to 16 */
	/* next 2 bytes (niid) can move to extension */
	uint16_t	niid;
	uint16_t 	res2;
	uint16_t	iphdr_offset;
	uint16_t	ipv6_fraghdr_offset;
	uint16_t	seg_addr;
	uint16_t	seg_length;
	uint16_t	seg_offset;
	uint8_t		res3[2];
};
#pragma pack(pop)

#pragma pack(push,1)
struct extended_ipr_rfdc{
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
 @enum ipr_functions

 @Description	AIOP IPR Functions enumeration.

 @{
*//***************************************************************************/
enum ipr_function_identifier {
	IPR_INIT = 0,
	IPR_CREATE_INSTANCE,
	IPR_DELETE_INSTANCE,
	IPR_REASSEMBLE
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


@Return		reflect the return value from slab_find_and_reserve_bpid().
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
@Param[in]	iphdr_ptr - pointer to IP header.
@Param[in]	frame_is_ipv4 - frame is Ipv4 or Ipv6.

@Return		Status.

@Cautions	None.
*//***************************************************************************/

uint32_t ipr_insert_to_link_list(struct ipr_rfdc *rfdc_ptr,
				 uint64_t rfdc_ext_addr,
				 struct ipr_instance *instance_params,
				 void *iphdr_ptr,
				 uint32_t frame_is_ipv4);

#ifndef USE_IPR_SW_TABLE
void ipv6_rule_delete(uint64_t rfdc_ext_addr,
		      struct ipr_instance *instance_params_ptr);

int ipr_lookup(uint32_t frame_is_ipv4,
	       struct ipr_instance *instance_params_ptr,
	       uint64_t *rfdc_ext_addr_ptr);
#endif	/* USE_IPR_SW_TABLE */

int ipr_miss_handling(struct ipr_instance *instance_params_ptr,
	 uint32_t frame_is_ipv4, uint32_t osm_status, struct ipr_rfdc *rfdc_ptr,
	 ipr_instance_handle_t instance_handle, uint64_t *rfdc_ext_addr_ptr);


uint32_t closing_in_order(uint64_t rfdc_ext_addr, uint8_t num_of_frags,
			  uint8_t preserve_fragments);

uint32_t closing_with_reordering(struct ipr_rfdc *rfdc_ptr,
				 uint64_t rfdc_ext_addr,
				 uint8_t preserve_fragments);

inline void move_to_correct_ordering_scope1(uint32_t osm_status)
{
	/* return to original ordering scope that entered
	 * the ipr_reassemble function */
		if(osm_status == 0) {
			/* Tasks which started in concurrent and have 2 free levels */
			osm_scope_exit();
		} else if(osm_status & START_CONCURRENT) {
			/* Tasks which started in concurrent and hadn't 2 free levels */
			osm_scope_relinquish_exclusivity();
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
			osm_scope_relinquish_exclusivity();
		} else if(osm_status & START_CONCURRENT) {
			osm_scope_relinquish_exclusivity();
		}
}*/
uint32_t ipv4_header_update_and_l4_validation(struct ipr_rfdc *rfdc_ptr);
uint32_t ipv6_header_update_and_l4_validation(struct ipr_rfdc *rfdc_ptr);

uint32_t check_for_frag_error(struct ipr_instance *instance_params_ptr,
			      uint32_t frame_is_ipv4, void *iphdr_ptr
#ifdef USE_IPR_SW_TABLE
			      , union ip_fragment_key *fk
#endif	/* USE_IPR_SW_TABLE */
			      );

void ipr_time_out(uint64_t rfdc_ext_addr, uint16_t dummy);

void check_remove_padding();

uint32_t out_of_order(struct ipr_rfdc *rfdc_ptr, uint64_t rfdc_ext_addr,
		      uint32_t last_fragment,uint16_t current_frag_size,
		      uint16_t frag_offset_shifted,
		      struct ipr_instance *instance_params_ptr);

void ipr_delete_instance_after_time_out(ipr_instance_handle_t ipr_instance_ptr);

void ipr_stats_update(struct ipr_instance *instance_params_ptr,
		      uint32_t counter_offset, uint32_t frame_is_ipv4);

int ipr_early_init(uint32_t nbr_of_instances, uint32_t nbr_of_context_buffers);

void ipr_exception_handler(enum ipr_function_identifier func_id,
		     	   uint32_t line,
		     	   int32_t status);

inline uint8_t is_atomic_fragment(void)
{
	struct ipv6fraghdr	*ipv6fraghdr_ptr;
	uint16_t		ipv6fraghdr_offset;

	if (!parser_is_atomic_frag_detection_enabled())
		return 0;

	ipv6fraghdr_offset = PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
	ipv6fraghdr_ptr = (struct ipv6fraghdr *)
			(PRC_GET_SEGMENT_ADDRESS() + ipv6fraghdr_offset);

	if ((ipv6fraghdr_ptr->offset_and_flags & IPV6_HDR_OFFSET_MASK) ||
	    (ipv6fraghdr_ptr->offset_and_flags & IPV6_HDR_M_FLAG_MASK))
		return 0;

	return 1;
}

/**************************************************************************//**
@Description	IPR Global parameters
*//***************************************************************************/

struct ipr_global_parameters {
uint8_t  ipr_key_id_ipv4;
uint8_t  ipr_key_id_ipv6;
};

struct ipr_instance_ext_delete{
	uint64_t	delete_arg;
	ipr_del_cb_t 	*confirm_delete_cb;
};

#pragma pack(push,1)
struct ipr_instance_and_extension{
	struct ipr_instance		ipr_instance;
	struct ipr_instance_extension	ipr_instance_extension;
};
#pragma pack(pop)


/* @} end of group FSL_IPR */

#endif /* __AIOP_IPR_H */
