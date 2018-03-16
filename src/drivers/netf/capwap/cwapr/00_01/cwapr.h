/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
@File          cwapr.h

@Description   This file contains CAPWAP Reassembly internal functions and definitions

*//***************************************************************************/

#ifndef __AIOP_CWAPR_H
#define __AIOP_CWAPR_H

#include "fsl_types.h"
#include "fsl_cwapr.h"
#include "fsl_osm.h"
#include "fsl_tman.h"
#include "fsl_fdma.h"


/**************************************************************************//**
@addtogroup	FSL_CWAPR FSL_AIOP_CWAPR

@Description	AIOP CAPWAP reassembly functions macros and definitions

@{
*//***************************************************************************/

#define	MAX_NUM_OF_FRAGS	64
#define	FRAG_OK_REASS_NOT_COMPL	0
#define LAST_FRAG_IN_ORDER	1
#define LAST_FRAG_OUT_OF_ORDER	2
#define MALFORMED_FRAG		3
#define NO_BYPASS_OSM		0x00000000 /* in osm_status */
#define	BYPASS_OSM		0x00000001 /* in osm_status */
#define START_CONCURRENT	0x00000002 /* in osm_status */
#define NO_ERROR		0
#define CWAPR_CONTEXT_SIZE		2688
#define CWAPR_START_OF_LINK_LIST	CWAPR_RFDC_SIZE
#define CWAPR_START_OF_FDS_LIST		(CWAPR_START_OF_LINK_LIST + CWAPR_RFDC_LIST_SIZE)
#define CWAPR_RFDC_LIST_NODE_SIZE	sizeof(struct cwapr_rfdc_list_node)
#define CWAPR_RFDC_LIST_SIZE		(CWAPR_RFDC_LIST_NODE_SIZE * MAX_NUM_OF_FRAGS)
#define RFDC_VALID		0x8000 /* in RFDC status */
#define FIRST_ARRIVED		0x2000 /* in RFDC status */
#define SCOPE_LEVEL		0x0030 /* in RFDC status */
/*following define includes both cases: pure OOO and OOO_and_IN_ORDER */
#define OUT_OF_ORDER		0x0001 /* in RFDC status */
#define ORDER_AND_OOO		0x0002 /* in RFDC status */
#define CAPWAP_FRAG_ID_LEN	2      /* 2 bytes*/
#define CAPWAP_FRAG_ID_OFFSET	4      /* fragment ID offset relative to header start */
#define INSTANCE_VALID		0x0001
#define CWAPR_REF_COUNT_ADDR	(HWC_ACC_OUT_ADDRESS + CDMA_REF_CNT_OFFSET)
#define CWAPR_INSTANCE_SIZE	sizeof(struct cwapr_instance)
#define CWAPR_RFDC_SIZE		sizeof(struct cwapr_rfdc)
#define FD_SIZE			sizeof(struct ldpaa_fd)
#define BYTE_RFDC_LIST_MASK	0x07
#define CWAPR_KEY_SIZE		10 /* 8-byte tunnel id + 2-byte fragment id */
#define CWAPR_TIMEOUT_FLAGS	(TMAN_CREATE_TIMER_MODE_10_MSEC_GRANULARITY | \
				TMAN_CREATE_TIMER_MODE_TPRI | \
				TMAN_CREATE_TIMER_ONE_SHOT | \
				TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK)
#define ENOSPC_TIMER		2
#define ENOMEM_TABLE		3
#define CWAPR_ERROR		1

/* todo should move to general or OSM include file */
#define CONCURRENT		0
#define EXCLUSIVE		1

#pragma pack(push, 1)
struct cwapr_instance {
	uint64_t	extended_stats_addr;
	uint64_t	delete_arg;
	/** Argument to be passed upon invocation of the CAPWAP callback
	    function*/
	cwapr_timeout_arg_t cb_timeout_arg;
	/** function to call upon Time Out occurrence */
	cwapr_timeout_cb_t *timeout_cb;
	cwapr_del_cb_t *confirm_delete_cb;
	/** \link FSL_CWAPRInsFlags CAPWAP reassembly flags \endlink */
	uint32_t	flags;
	uint32_t	reass_frm_cntr;
	/** Number of frames that CAPWAP started reassembly
	    but didn't complete it yet */
	uint32_t	num_of_open_reass_frames;
	/* todo remove 2 following parameters */
	uint32_t	max_open_frames;
	/* CTLU table ID  */
	uint16_t	table_id;
	/** maximum reassembled frame size */
	uint16_t	max_reass_frm_size;
	/* BPID to fetch buffers from */
	uint16_t	bpid;
	/** reassembly timeout value */
	uint16_t	timeout_value;
	/* TMAN Instance ID */
	uint8_t		tmi_id;
	uint8_t		res[7];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct cwapr_rfdc {
	uint64_t	table_key[2]; /* this field should stay aligned to 16 */
	uint64_t	instance_handle;
	uint32_t	timer_handle;
	uint16_t	exp_total_len; /* Expected total length */
	uint16_t	curr_total_len; /* CAPWAP payload length */
	uint16_t	first_frag_hdr_length;
	uint16_t	biggest_payload;
	uint16_t	status;
	uint16_t	total_in_order_payload;
	/* next 2 bytes (niid) can move to extension */
	uint16_t	niid;
	uint16_t	iphdr_offset;
	uint16_t	seg_addr;
	uint16_t	seg_length;
	uint16_t	seg_offset;
	uint8_t		first_frag_idx;
	uint8_t		last_frag_idx;
	uint8_t		next_idx;
	uint8_t		idx_out_of_order;
	uint8_t		num_of_frags;
	uint8_t		res[9];
};
#pragma pack(pop)

struct cwapr_rfdc_list_node {
	uint16_t	frag_offset;
	uint16_t	frag_len;
	uint16_t	res;
	uint8_t		prev_idx;
	uint8_t		next_idx;
};

/**************************************************************************//**
 @enum cwapr_functions

 @Description	AIOP CWAPR Functions enumeration.

 @{
*//***************************************************************************/
enum cwapr_function_identifier {
	CWAPR_INIT = 0,
	CWAPR_CREATE_INSTANCE,
	CWAPR_DELETE_INSTANCE,
	CWAPR_REASSEMBLE
};


/**************************************************************************//**
@Group		CWAPR_Internal Internal CWAPR functions

@Description	Internal CAPWAP reassembly

@{
*//***************************************************************************/




/**************************************************************************//**
@Function	cwapr_init

@Description	Initialize the CAPWAP Reassembly infrastructure.
		This function should be called once.
		No CWAPR function can be called before this function was invoked.

Implicit:	max_buffers - maximum number of buffers to be used as
		context buffers for all the instances.\n
		The size of each buffer should be at least 2240 bytes.\n
		Buffers should be aligned to 64 bytes.
		flags - \link CWAPRInitFlags CWAPR init flags \endlink

@Return		reflect the return value from slab_find_and_reserve_bpid().
		0       - on success,
		-ENAVAIL - could not release into bpid
		-ENOMEM  - not enough memory for mem_partition_id.

@Cautions	None.
*//***************************************************************************/
int cwapr_init(void);

/**************************************************************************//**
@Function	cwapr_insert_to_link_list

@Description	Insert to Link List - Save FD

@Param[in]	rfdc - pointer to RFDC in workspace (on stack)
@Param[in]	rfdc_ext_addr - pointer to RFDC in external memory.
@Param[in]	capwap_ptr - pointer to CAPWAP header.

@Return		Status.

@Cautions	None.
*//***************************************************************************/
uint32_t insert_to_cwapr_link_list(struct cwapr_rfdc *rfdc,
				 uint64_t rfdc_ext_addr,
				 struct cwapr_instance *cwapr_instance,
				 struct capwaphdr *capwap_hdr);

int lookup_cwapr_flow(struct cwapr_instance *cwapr_instance,
		uint64_t tunnel_id, uint64_t *rfdc_ext_addr);

int miss_cwapr_flow(struct cwapr_instance *cwapr_instance,
	 uint32_t osm_status, uint64_t tunnel_id, struct cwapr_rfdc *rfdc,
	 cwapr_instance_handle_t cwapr_instance_handle, uint64_t *rfdc_ext_addr);

uint32_t close_in_order_capwap_frags(
		uint64_t rfdc_ext_addr, uint8_t num_of_frags);

uint32_t close_with_reorder_capwap_frags(
		struct cwapr_rfdc *rfdc, uint64_t rfdc_ext_addr);

uint32_t l3_l4_l5_headers_update(struct cwapr_rfdc *rfdc);

uint32_t check_for_capwap_frag_error(struct capwaphdr *capwap_hdr);

void timeout_cwapr_reassemble(uint64_t rfdc_ext_addr, uint16_t opaque);

uint32_t out_of_order_capwap_frags(struct cwapr_rfdc *rfdc,
		uint64_t rfdc_ext_addr, uint32_t last_frag,
		uint16_t frag_size, uint16_t frag_offset,
		struct cwapr_instance *cwapr_instance);

void delete_cwapr_instance_at_timeout(
		cwapr_instance_handle_t cwapr_instance_handle);

void update_stats_cwapr(struct cwapr_instance *cwapr_instance,
		      uint32_t counter_offset);

int cwapr_early_init(uint32_t num_of_instances, uint32_t num_of_context_buffers);

void cwapr_exception_handler(enum cwapr_function_identifier func_id,
			uint32_t line,
			int32_t status);

inline void move_to_correct_cwapr_ordering_scope(uint32_t osm_status)
{
	/* return to original ordering scope that entered
	 * the cwapr_reassemble function */
		if (osm_status == 0) {
			/* Tasks which started in concurrent and have 2 free levels */
			osm_scope_exit();
		} else if (osm_status & START_CONCURRENT) {
			/* Tasks which started in concurrent and hadn't 2 free levels */
			osm_scope_relinquish_exclusivity();
		}
}

void return_to_correct_ordering_scope(uint32_t osm_status);
/* todo restore this inline after compiler fix.
inline void move_to_correct_ordering_scope2(uint32_t osm_status)
{
		if(osm_status == 0) { */
			/* return to original ordering scope that entered
			 * the cwapr_reassemble function */
	/*		osm_scope_exit();
			osm_scope_exit();
			osm_scope_relinquish_exclusivity();
		} else if(osm_status & START_CONCURRENT) {
			osm_scope_relinquish_exclusivity();
		}
}*/

/**************************************************************************//**
@Description	CWAPR Global parameters
*//***************************************************************************/

struct cwapr_global_parameters {
uint8_t  cwapr_key_id;
};

struct cwapr_instance_ext_delete {
	uint64_t	delete_arg;
	cwapr_del_cb_t	*confirm_delete_cb;
};

/* @} end of group FSL_CWAPR */

#endif /* __AIOP_CWAPR_H */
