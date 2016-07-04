/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
@File		cwapr.c

@Description	This file contains the AIOP SW CAPWAP Reassembly implementation.

*//***************************************************************************/

#include "general.h"
#include "system.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_osm.h"
#include "fsl_table.h"
#include "fsl_keygen.h"
#include "fsl_ste.h"
#include "fsl_spinlock.h"
#include "fsl_checksum.h"
#include "cwapr.h"
#include "fsl_cdma.h"
#include "net.h"
#include "fsl_sl_dpni_drv.h"

/* For wrapper functions */
#include "cdma.h"
#include "tman.h"

#ifdef AIOP_VERIF
#include "slab_stub.h"
#else
#include "fsl_dbg.h"
#include "fsl_sl_slab.h"
#endif

/* TODO: do we actually need a structure? */
struct  cwapr_global_parameters g_cwapr_params;
extern struct dpni_drv *nis;
extern __TASK struct aiop_default_task_params default_task_params;


#ifndef AIOP_VERIF
int cwapr_early_init(uint32_t num_of_instances, uint32_t num_of_context_buffers)
{
	uint32_t num_of_cwapr_contexts;
	int err;
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_pid = MEM_PART_DP_DDR;

	num_of_cwapr_contexts = num_of_context_buffers + 2*num_of_instances;
	/* CWAPR 2688 rounded up modulo 64 - 8 */
	err = slab_register_context_buffer_requirements(num_of_cwapr_contexts,
							num_of_cwapr_contexts,
							2744,
							64,
							mem_pid,
							0,
							0);
	if (err) {
		pr_err("Failed to register CWAPR context buffers\n");
		return err;
	}
	return 0;
}
#endif

int cwapr_init(void)
{
	struct kcr_builder kb __attribute__((aligned(16)));
	int    status;
	uint8_t  cwapr_key_id;

	/* CAPWAP key: tunnel ID + fragment ID */
	keygen_kcr_builder_init(&kb);

	status = keygen_kcr_builder_add_input_value_fec(0, 8, NULL, &kb);
	if (status < 0)
		cwapr_exception_handler(CWAPR_INIT, __LINE__, (int32_t)status);

	status = keygen_kcr_builder_add_protocol_based_generic_fec(
			NXT_HDR_OFFSET_OFFSET_IN_PR,
			CAPWAP_FRAG_ID_OFFSET,
			CAPWAP_FRAG_ID_LEN,
			NULL,
			&kb);
	if (status < 0)
		cwapr_exception_handler(CWAPR_INIT, __LINE__, (int32_t)status);

	status = keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU,
			  kb.kcr,
			  &cwapr_key_id);
	if (status < 0) {
		/* todo  Fatal */
		cwapr_exception_handler(CWAPR_INIT, __LINE__, (int32_t)status);
	}
	g_cwapr_params.cwapr_key_id = cwapr_key_id;
	return 0;
}

int cwapr_create_instance(struct cwapr_params *params,
			    cwapr_instance_handle_t *cwapr_instance_handle)
{
	struct cwapr_instance	cwapr_instance;
	struct table_create_params	tbl_params;
	uint32_t tbl_location;
	uint16_t tbl_location_attr;
	uint16_t bpid;
	int status;
	enum memory_partition_id mem_pid = MEM_PART_SYSTEM_DDR;

	/* call ARENA function for allocating buffers needed to CWAPR
	 * processing (create_slab ) */
	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_pid = MEM_PART_DP_DDR;

	/* Reservation is done for 2 extra buffers: 1 cwapr_instance and
	 * the other in case of reaching the maximum of open reassembled frames */
	status = slab_find_and_reserve_bpid(params->max_open_frames + 2,
					CWAPR_CONTEXT_SIZE,
					8,
					mem_pid,
					NULL,
					&bpid);

	if (status < 0)
		cwapr_exception_handler(CWAPR_CREATE_INSTANCE, __LINE__, status);

	status = cdma_acquire_context_memory(bpid, cwapr_instance_handle);
	if (status)
		cwapr_exception_handler(CWAPR_CREATE_INSTANCE, __LINE__, status);

	cwapr_instance.bpid = bpid;
	cwapr_instance.flags = params->flags;

	/* Initialize instance parameters */
	cwapr_instance.table_id = 0;
	if (params->max_open_frames) {
		cwapr_instance.max_open_frames = params->max_open_frames;
		tbl_params.committed_rules = params->max_open_frames;
		tbl_params.max_rules = params->max_open_frames;
		/* Tunnel ID, CAPWAP Fragment ID */
		tbl_params.key_size = CWAPR_KEY_SIZE;
		tbl_location = params->flags & 0x0C000000;
		if (tbl_location == CWAPR_MODE_TABLE_LOCATION_PEB)
			tbl_location_attr = TABLE_ATTRIBUTE_LOCATION_PEB;
		else if (tbl_location == CWAPR_MODE_TABLE_LOCATION_DP_DDR)
			tbl_location_attr = TABLE_ATTRIBUTE_LOCATION_DP_DDR;
		else if (tbl_location == CWAPR_MODE_TABLE_LOCATION_SYS_DDR)
			tbl_location_attr = TABLE_ATTRIBUTE_LOCATION_SYS_DDR;
		tbl_params.attributes = TABLE_ATTRIBUTE_TYPE_EM |
					tbl_location_attr |
					TABLE_ATTRIBUTE_MR_NO_MISS;

		status = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params,
						&cwapr_instance.table_id);
		if (status != TABLE_STATUS_SUCCESS) {
			/* TODO SR error case */
			cdma_release_context_memory(*cwapr_instance_handle);
			cwapr_exception_handler(CWAPR_CREATE_INSTANCE, __LINE__,
					      ENOMEM_TABLE);
		}
	}

	/* Initialize instance parameters */
	cwapr_instance.extended_stats_addr = params->extended_stats_addr;
	cwapr_instance.max_reass_frm_size = params->max_reass_frm_size;
	cwapr_instance.timeout_value = params->timeout_value;
	cwapr_instance.timeout_cb = params->timeout_cb;
	cwapr_instance.cb_timeout_arg = params->cb_timeout_arg;
	cwapr_instance.confirm_delete_cb = 0;
	cwapr_instance.delete_arg	   = 0;
	cwapr_instance.tmi_id = params->tmi_id;
	cwapr_instance.num_of_open_reass_frames = 0;
	cwapr_instance.reass_frm_cntr = 0;

	/* Write cwapr instance data structure */
	cdma_write(*cwapr_instance_handle, &cwapr_instance, CWAPR_INSTANCE_SIZE);

	return SUCCESS;
}

int cwapr_delete_instance(cwapr_instance_handle_t cwapr_instance_handle,
			    cwapr_del_cb_t *confirm_delete_cb,
			    cwapr_del_arg_t delete_arg)
{
	struct cwapr_instance	cwapr_instance;
	uint64_t		cwapr_instance_extension_ptr;
	struct cwapr_instance_ext_delete delete_args;
	int			status;

	cdma_read(&cwapr_instance, cwapr_instance_handle, CWAPR_INSTANCE_SIZE);

	cwapr_instance_extension_ptr = ((uint64_t)cwapr_instance_handle) +
				     sizeof(struct cwapr_instance);

	delete_args.confirm_delete_cb = confirm_delete_cb;
	delete_args.delete_arg        = delete_arg;

	cdma_write(cwapr_instance_extension_ptr,
		   &delete_args,
		   sizeof(struct cwapr_instance_ext_delete));

	status = tman_create_timer(cwapr_instance.tmi_id,
			CWAPR_TIMEOUT_FLAGS,
			cwapr_instance.timeout_value,
			(tman_arg_8B_t) cwapr_instance_handle,
			(tman_arg_2B_t) NULL,
			(tman_cb_t) delete_cwapr_instance_at_timeout,
			&cwapr_instance.flags);
	if (status)
		cwapr_exception_handler(CWAPR_DELETE_INSTANCE, __LINE__, status);

	return SUCCESS;
}

void delete_cwapr_instance_at_timeout(
		cwapr_instance_handle_t cwapr_instance_handle)
{
	struct cwapr_instance	cwapr_instance;
	int	status;

	ste_barrier();

	cdma_read(&cwapr_instance, cwapr_instance_handle, CWAPR_INSTANCE_SIZE);

	if (cwapr_instance.num_of_open_reass_frames != 0) {
		/* Not all the timeouts expired, give more time */
		status = tman_create_timer(
				cwapr_instance.tmi_id,
				CWAPR_TIMEOUT_FLAGS,
				cwapr_instance.timeout_value,
				(tman_arg_8B_t) cwapr_instance_handle,
				(tman_arg_2B_t) NULL,
				(tman_cb_t) delete_cwapr_instance_at_timeout,
				&cwapr_instance.flags);
		if (status)
			cwapr_exception_handler(CWAPR_DELETE_INSTANCE, __LINE__, status);

		fdma_terminate_task();
	} else
		/* todo SR error case */
		cdma_release_context_memory(cwapr_instance_handle);

	/* error case */
	table_delete(TABLE_ACCEL_ID_CTLU, cwapr_instance.table_id);

	cwapr_instance.confirm_delete_cb(cwapr_instance.delete_arg);
}


CWAPR_CODE_PLACEMENT int cwapr_reassemble(
		cwapr_instance_handle_t instance_handle, uint64_t tunnel_id)
{
	/* Following struct should be aligned due to ctlu alignment request */
	struct cwapr_rfdc rfdc __attribute__((aligned(16)));
	struct cwapr_instance cwapr_instance;
	struct scope_status_params scope_status;
	uint64_t rfdc_ext_addr;
	uint32_t status_insert_to_LL;
	uint32_t osm_status;
	uint32_t status;
	int	 sr_status;
	uint16_t capwap_offset;
	uint8_t is_frag = 0;
	struct capwaphdr *capwap_hdr;
	struct presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;

	capwap_offset = (uint16_t)PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT();
	capwap_hdr = (void *)(capwap_offset + PRC_GET_SEGMENT_ADDRESS());
	is_frag = capwap_hdr->bits_flags & NET_HDR_FLD_CAPWAP_F;

	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	if (scope_status.scope_mode == EXCLUSIVE) {
		osm_status = BYPASS_OSM;
		if (is_frag) {
			/* Fragment */
			osm_status = BYPASS_OSM;
		} else {
			/* regular frame */
			return CWAPR_REASSEMBLY_REGULAR;
		}
	} else {
	    if (is_frag) {
		/* Fragment */
		/* move to exclusive */
		osm_scope_transition_to_exclusive_with_increment_scope_id_wrp();

		/* scope level should be bigger than 1 and not than 2
		 * because of TKT260685 that requires an additional
		 * scope level */
		if (scope_status.scope_level <= 1) {
			 osm_status = NO_BYPASS_OSM;
			/* create nested exclusive for the fragments of
			 * the same flow*/
			 osm_scope_enter_to_exclusive_with_increment_scope_id();
		} else {
			/* can't create 2 nested */
			osm_status = BYPASS_OSM | START_CONCURRENT;
			}
		} else {
			/* regular frame */
			/* transition in order to have the same scope id
			 * as closing fragment */
		   osm_scope_transition_to_concurrent_with_increment_scope_id();
		   return CWAPR_REASSEMBLY_REGULAR;
		}
	}

	/* read instance parameters */
	cdma_read_wrp(&cwapr_instance, instance_handle, CWAPR_INSTANCE_SIZE);

	if (check_for_capwap_frag_error(capwap_hdr) == NO_ERROR) {

		sr_status = lookup_cwapr_flow(&cwapr_instance,
				tunnel_id, &rfdc_ext_addr);
		if (sr_status == TABLE_STATUS_SUCCESS) {
			/* Hit */

			/* create nested per reassembled frame
			 * Also serve as mutex for Timeout */
			if ((osm_status == NO_BYPASS_OSM) ||
				(osm_status == START_CONCURRENT)) {
				/* release parent to concurrent */
				osm_scope_enter_to_exclusive_with_new_scope_id(
						  (uint32_t)rfdc_ext_addr);
			} else {
			      /* Next step is needed only for mutex with
			       * Timeout.
			       * Doesn't release parent to concurrent */
			     osm_scope_enter(OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE,
					     (uint32_t)rfdc_ext_addr);
			}

			/* read RFDC */
			cdma_read_wrp(&rfdc, rfdc_ext_addr, CWAPR_RFDC_SIZE);

			if (!(rfdc.status & RFDC_VALID)) {
				return_to_correct_ordering_scope(osm_status);
				/* CDMA write, dec ref_cnt and release
				 * if ref_cnt=0.
				 * Error is not checked since no error
				 * can't be returned*/
				cdma_access_context_memory_wrp(
					rfdc_ext_addr,
					CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL,
					NULL,
					&rfdc,
					CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE
					| CWAPR_RFDC_SIZE,
					(uint32_t *)CWAPR_REF_COUNT_ADDR);

				fdma_discard_fd_wrp(
					     (struct ldpaa_fd *)HWC_FD_ADDRESS,
					     FDMA_DIS_NO_FLAGS);
				/* Early Time out */
				return -ETIMEDOUT;
			}
		} else if (sr_status == TABLE_STATUS_MISS) {
			sr_status = miss_cwapr_flow(&cwapr_instance,
						   osm_status,
						   tunnel_id,
						   &rfdc,
						   instance_handle,
						   &rfdc_ext_addr);
			if (sr_status)
				return sr_status;

		} else {
		/* TLU lookup SR error */
		pr_err("CWAPR Lookup failed\n");
	}

	if (rfdc.num_of_frags == MAX_NUM_OF_FRAGS) {
		/* Release RFDC, dec ref_cnt, release if 0 */
		cdma_access_context_memory_wrp(
				  rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_NO_MEM_DMA |
				  CWAPR_RFDC_SIZE,
				  (uint32_t *)CWAPR_REF_COUNT_ADDR);
		/* Handle ordering scope */
		move_to_correct_cwapr_ordering_scope(osm_status);

		update_stats_cwapr(&cwapr_instance,
				 offsetof(struct cwapr_stats_cntrs, more_than_64_frags_cntr));

		return -ENOTSUP;
	}
	status_insert_to_LL = insert_to_cwapr_link_list(&rfdc, rfdc_ext_addr,
						      &cwapr_instance,
						      capwap_hdr);
	switch (status_insert_to_LL) {
	case FRAG_OK_REASS_NOT_COMPL:
		if (rfdc.num_of_frags != 1) {
			/* other fragments than the opening one */
			if (!(cwapr_instance.flags & CWAPR_MODE_TIMEOUT_TYPE)) {
				/* recharge timer in case of time out
				 * between fragments */
				/* Delete timer */
				sr_status = tman_delete_timer_wrp(
				  rfdc.timer_handle,
				  TMAN_TIMER_DELETE_MODE_WO_EXPIRATION);
				/* DEBUG : check ENAVAIL */
				if (sr_status == SUCCESS) {
					sr_status = tman_create_timer_wrp(
					 cwapr_instance.tmi_id,
					 CWAPR_TIMEOUT_FLAGS,
					 cwapr_instance.timeout_value,
					 (tman_arg_8B_t) rfdc_ext_addr,
					 (tman_arg_2B_t) NULL,
					 (tman_cb_t) timeout_cwapr_reassemble,
					 &rfdc.timer_handle);
					if (sr_status != SUCCESS)
						cwapr_exception_handler(CWAPR_REASSEMBLE,
								__LINE__, ENOSPC_TIMER);
				}
			}
		}
		/* Write updated 64 first bytes of RFDC */
		cdma_access_context_memory_wrp(
				       rfdc_ext_addr,
				       CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL,
				       NULL,
				       &rfdc,
				       CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE | CWAPR_RFDC_SIZE,
				       (uint32_t *)CWAPR_REF_COUNT_ADDR);

		return_to_correct_ordering_scope(osm_status);

		/* Increment no of valid fragments in extended
		 * statistics data structure*/
		update_stats_cwapr(&cwapr_instance,
				 offsetof(struct cwapr_stats_cntrs,
					  valid_frags_cntr));

		return CWAPR_REASSEMBLY_NOT_COMPLETED;
	case LAST_FRAG_IN_ORDER:
		close_in_order_capwap_frags(rfdc_ext_addr, rfdc.num_of_frags);
		break;
	case LAST_FRAG_OUT_OF_ORDER:
		close_with_reorder_capwap_frags(&rfdc, rfdc_ext_addr);
		break;
	case MALFORMED_FRAG:
		/* duplicate, overlap, or non-conform fragment */
		/* Write updated 64 first bytes of RFDC */
		/* RFDC is written in case the opening frag is malformed */
		if (rfdc.num_of_frags == 0)
			cdma_access_context_memory_wrp(
				       rfdc_ext_addr,
				       CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL,
				       NULL,
				       &rfdc,
				       CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE | CWAPR_RFDC_SIZE,
				       (uint32_t *)CWAPR_REF_COUNT_ADDR);

		/* Increment no of malformed frames in extended
		 * statistics data structure*/
		update_stats_cwapr(&cwapr_instance,
				 offsetof(struct cwapr_stats_cntrs,
					  malformed_frags_cntr));


		return CWAPR_MALFORMED_FRAG;
	}
	/* Only successfully reassembled frames continue
	   from here */
	/* Currently no default frame */

	/* Reset Valid bit of RFDC */
	rfdc.status = rfdc.status & ~RFDC_VALID;

	/* Increment no of valid fragments in extended statistics
	 * data structure*/
	update_stats_cwapr(&cwapr_instance,
			 offsetof(struct cwapr_stats_cntrs,
				  valid_frags_cntr));

	/* Delete timer */
	sr_status = tman_delete_timer_wrp(rfdc.timer_handle,
					TMAN_TIMER_DELETE_MODE_WO_EXPIRATION);
	/* DEBUG : check ENAVAIL */
	if (sr_status != SUCCESS) {
		/* Write and release updated 64 first bytes of RFDC,
		 * dec ref_cnt, release if 0 */
		cdma_access_context_memory_wrp(
				  rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE |
				  CWAPR_RFDC_SIZE,
				  (uint32_t *)CWAPR_REF_COUNT_ADDR);

		return_to_correct_ordering_scope(osm_status);
		return -ETIMEDOUT;
	}

	table_rule_delete_wrp(TABLE_ACCEL_ID_CTLU,
			  cwapr_instance.table_id,
			  (union table_key_desc *)&rfdc.table_key,
			  CWAPR_KEY_SIZE,
			  NULL);
	/* DEBUG : check EIO */

	/* Open segment for reassembled frame */
	/* Retrieve original seg length,seg addr and seg offset from RFDC */
	prc->seg_address = rfdc.seg_addr;
	prc->seg_length  = rfdc.seg_length;
	prc->seg_offset  = rfdc.seg_offset;
	/* Default frame is now the full reassembled frame */
	fdma_present_default_frame_default_segment();
	/* FD length is still not updated */

	/* Parser is not re-run here, and iphdr offset will be retrieved
	   from RFDC*/

	/* todo check if write context and then exit from OSM can be done here */
	status = l3_l4_l5_headers_update(&rfdc);

	/* Write and release updated 64 first bytes of RFDC */
	/* CDMA write, dec ref_cnt and release if
	 * ref_cnt=0 */
	cdma_access_context_memory_wrp(
				  rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE |
				  CWAPR_RFDC_SIZE,
				  (uint32_t *)CWAPR_REF_COUNT_ADDR);

	/* Decrement ref count of acquire */
	cdma_refcount_decrement_and_release(rfdc_ext_addr);

	return_to_correct_ordering_scope(osm_status);

	/* Decrement no of IPv4 open frames in instance data structure*/
	ste_dec_counter_wrp(instance_handle +
		offsetof(struct cwapr_instance, num_of_open_reass_frames),
		1,
		STE_MODE_32_BIT_CNTR_SIZE);
	if (status == SUCCESS) {
		/* L4 checksum is valid */
		/* Increment no of frames in instance data structure */
		ste_inc_counter_wrp(instance_handle +
				offsetof(struct cwapr_instance, reass_frm_cntr),
				1,
				STE_MODE_32_BIT_CNTR_SIZE);
		return CWAPR_REASSEMBLY_SUCCESS;
	}

	/* L4 checksum is not valid */
	return -EIO;

	} else {
		/* Error fragment */
		move_to_correct_cwapr_ordering_scope(osm_status);
		/* Increment no of malformed frames in extended
		 * statistics data structure*/
		update_stats_cwapr(&cwapr_instance,
				 offsetof(struct cwapr_stats_cntrs,
					  malformed_frags_cntr));

		return CWAPR_MALFORMED_FRAG;
	}
}

CWAPR_CODE_PLACEMENT int lookup_cwapr_flow(
		struct cwapr_instance *cwapr_instance,
		uint64_t tunnel_id, uint64_t *rfdc_ext_addr)
{
	/* Following struct should be aligned due to ctlu alignment request */
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	uint8_t	key[CWAPR_KEY_SIZE] __attribute__((aligned(16)));
	union table_lookup_key_desc key_desc;
	struct capwaphdr *capwap_hdr = NULL;
	int status;

	capwap_hdr = (struct capwaphdr *)(
		PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() +
		PRC_GET_SEGMENT_ADDRESS());

	key_desc.em_key = (union table_lookup_key_desc *)&key;
	*(uint64_t *)key_desc.em_key = tunnel_id;
	*(uint16_t *)((uint8_t *)key_desc.em_key + sizeof(uint64_t)) =
			capwap_hdr->frag_id;

	status = table_lookup_by_key(TABLE_ACCEL_ID_CTLU,
			cwapr_instance->table_id,
			key_desc,
			CWAPR_KEY_SIZE,
		        &lookup_result);
	if (status < 0)
		cwapr_exception_handler(CWAPR_INIT, __LINE__, (int32_t)status);

	/* Next line is relevant only in case of Hit */
	*rfdc_ext_addr = lookup_result.opaque0_or_reference;

	return status;
}

CWAPR_CODE_PLACEMENT int miss_cwapr_flow(struct cwapr_instance *cwapr_instance,
	 uint32_t osm_status, uint64_t tunnel_id, struct cwapr_rfdc *rfdc,
	 cwapr_instance_handle_t cwapr_instance_handle, uint64_t *rfdc_ext_addr)
{
	struct capwaphdr *capwap_hdr = NULL;
	struct table_rule rule __attribute__((aligned(16)));
	int	status;

	capwap_hdr = (struct capwaphdr *)(
		PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() +
		PRC_GET_SEGMENT_ADDRESS());

	/* Miss */
	status = cdma_acquire_context_memory(cwapr_instance->bpid, rfdc_ext_addr);
	if (status)
		cwapr_exception_handler(CWAPR_REASSEMBLE, __LINE__, status);

	/* increment reference count */
	cdma_access_context_memory_wrp(
		*rfdc_ext_addr,
		CDMA_ACCESS_CONTEXT_MEM_INC_REFCOUNT,
		0,
		(void *)0,
		0,
		(uint32_t *)CWAPR_REF_COUNT_ADDR);

	/* Reset RFDC + Link List */
	/*cdma_ws_memory_init((void *)&rfdc,
			SIZE_TO_INIT,
			0);  */
	/* Add entry to TLU table */
	/* Generate key */
	rule.options = 0;
	rule.result.type = TABLE_RESULT_TYPE_REFERENCE;
	rule.result.op0_rptr_clp.reference_pointer = *rfdc_ext_addr;
	*(uint64_t *)rule.key_desc.em.key = tunnel_id;
	*(uint16_t *)((uint8_t *)rule.key_desc.em.key + sizeof(uint64_t)) =
			capwap_hdr->frag_id;

	status = table_rule_create_wrp(
			TABLE_ACCEL_ID_CTLU,
			cwapr_instance->table_id,
			&rule,
			CWAPR_KEY_SIZE);

	if (status == -ENOMEM) {
		/* Maximum open reassembly is reached */
		update_stats_cwapr(cwapr_instance,
				offsetof(struct cwapr_stats_cntrs,
				open_reass_frms_exceed_cntr));
		/* Release acquired buffer */
		cdma_release_context_memory(*rfdc_ext_addr);
		/* Handle ordering scope */
		move_to_correct_cwapr_ordering_scope(osm_status);
		return -ENOSPC;
	}

	/* store key in RDFC */
	rfdc->table_key[0] = *(uint64_t *)rule.key_desc.em.key;
	rfdc->table_key[1] = *(uint64_t *)(rule.key_desc.em.key + 8);

	/* Increment number of open frames in instance data structure */
	ste_inc_counter_wrp(cwapr_instance_handle +
		 offsetof(struct cwapr_instance, num_of_open_reass_frames),
		 1,
		 STE_MODE_32_BIT_CNTR_SIZE);

	rfdc->status = RFDC_VALID |
		 (uint16_t)(default_task_params.current_scope_level << 4);

	rfdc->instance_handle	= cwapr_instance_handle;
	rfdc->exp_total_len		= 0;
	rfdc->idx_out_of_order	= 0;
	rfdc->next_idx			= 0;
	rfdc->curr_total_len	= 0;
	rfdc->first_frag_idx 	= 0;
	rfdc->num_of_frags		= 0;
	/* todo check if necessary */
	rfdc->biggest_payload	= 0;
	rfdc->last_frag_idx		= 0;
	rfdc->total_in_order_payload = 0;
	//			get_default_amq_attributes(&rfdc.isolation_bits);
	rfdc->niid = task_get_receive_niid();

	/* create Timer in TMAN */
	status = tman_create_timer_wrp(cwapr_instance->tmi_id,
				  CWAPR_TIMEOUT_FLAGS,
				  cwapr_instance->timeout_value,
				  (tman_arg_8B_t)*rfdc_ext_addr,
				  (tman_arg_2B_t) NULL,
				  (tman_cb_t) timeout_cwapr_reassemble,
				  &rfdc->timer_handle);
	if (status)
		cwapr_exception_handler(CWAPR_REASSEMBLE, __LINE__, ENOSPC_TIMER);

	/* create nested per reassembled frame
	 * Also serve as mutex for Timeout */
	if ((osm_status == NO_BYPASS_OSM) || (osm_status == START_CONCURRENT)) {
		/* release parent to concurrent */
		osm_scope_enter_to_exclusive_with_new_scope_id(
						 (uint32_t)*rfdc_ext_addr);
	} else {
		/* Next step is needed only for mutex with Timeout */
		/* doesn't release parent to concurrent */
		osm_scope_enter(OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE,
				(uint32_t)*rfdc_ext_addr);
	}

	return SUCCESS;

}
CWAPR_CODE_PLACEMENT uint32_t insert_to_cwapr_link_list(struct cwapr_rfdc *rfdc,
				 uint64_t rfdc_ext_addr,
				 struct cwapr_instance *cwapr_instance,
				 struct capwaphdr *capwap_hdr)
{

	struct	parse_result	*pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct	presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;
	struct cwapr_rfdc_list_node	node;
	/* todo reuse ext_addr for node_ext_addr */
	uint64_t		node_ext_addr;
	uint8_t			index;
	uint16_t		frag_size;
	uint16_t		frag_offset;
	uint16_t		exp_frag_offset; /* expected fragment offset */
	uint8_t			last_frag;
	uint32_t		status;
	uint64_t		ext_addr;
	uint16_t		capwap_hdr_len;

	capwap_hdr_len = (uint16_t)
		  (((capwap_hdr->hlen_rid_wbid_t & NET_HDR_FLD_CAPWAP_HLEN_MASK) >>
				  NET_HDR_FLD_CAPWAP_HLEN_OFFSET) << 2);

	frag_offset =
		(((capwap_hdr->offset_rsvd & NET_HDR_FLD_CAPWAP_FRAG_OFFSET_MASK) >>
			NET_HDR_FLD_CAPWAP_FRAG_OFFSET_OFFSET) << 3);

	/* Get size of CAPWAP payload for current fragment */
	frag_size = (uint16_t)(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() - capwap_hdr_len);

	last_frag = capwap_hdr->bits_flags & NET_HDR_FLD_CAPWAP_L;

	if (frag_offset != 0) {
		/* Not first frag */
		/* Save header to be removed in FD[FRC] */
		((struct ldpaa_fd *)HWC_FD_ADDRESS)->frc =
			    (uint32_t) (PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() +
					    capwap_hdr_len);
	} else
		rfdc->first_frag_hdr_length = capwap_hdr_len;

	if (!(rfdc->status & OUT_OF_ORDER)) {
		/* In order handling */
		exp_frag_offset = rfdc->curr_total_len;
		if (frag_offset == exp_frag_offset) {

			rfdc->num_of_frags++;
			rfdc->curr_total_len += frag_size;

			if (last_frag) {
				if ((rfdc->curr_total_len +
					rfdc->first_frag_hdr_length) <=
					cwapr_instance->max_reass_frm_size) {
					/* Close current frame before storing FD */
					fdma_store_default_frame_data();

					/* Write FD in external buffer */
					ext_addr = rfdc_ext_addr +
						CWAPR_START_OF_FDS_LIST +
						rfdc->next_idx * FD_SIZE;
					cdma_write(ext_addr,
						   (void *)HWC_FD_ADDRESS,
						   FD_SIZE);

					status = LAST_FRAG_IN_ORDER;
				} else
					status = MALFORMED_FRAG;
			} else {
				/* Non closing fragment */
				/* Close current frame before storing FD */
				fdma_store_default_frame_data();

				/* Write FD in external buffer */
				ext_addr = rfdc_ext_addr +
					CWAPR_START_OF_FDS_LIST +
					rfdc->next_idx * FD_SIZE;
				cdma_write(ext_addr,
					(void *)HWC_FD_ADDRESS, FD_SIZE);
				rfdc->next_idx++;
				status =  FRAG_OK_REASS_NOT_COMPL;
			}
		} else if (frag_offset < exp_frag_offset) {
				/* Malformed Error */
				return MALFORMED_FRAG;
		} else {
			/* New out of order */
			index = rfdc->next_idx;
			if (index != 0) {
				rfdc->status |= OUT_OF_ORDER | ORDER_AND_OOO;
				rfdc->idx_out_of_order = index;
				rfdc->first_frag_idx = index;
				rfdc->total_in_order_payload =
						rfdc->curr_total_len;
			} else
				rfdc->status |= OUT_OF_ORDER;

			rfdc->last_frag_idx = index;
			rfdc->num_of_frags++;
			rfdc->curr_total_len += frag_size;
			rfdc->next_idx = index + 1;
			rfdc->biggest_payload = frag_offset + frag_size;

			if (last_frag)
				rfdc->exp_total_len = frag_offset + frag_size;
			node.next_idx  = 0;
			node.prev_idx  = 0;
			node.frag_offset = frag_offset;
			node.frag_len = frag_size;
			/* Write my node of link list */
			node_ext_addr = rfdc_ext_addr +
						CWAPR_START_OF_LINK_LIST +
						index * CWAPR_RFDC_LIST_NODE_SIZE;
			cdma_write(node_ext_addr,
				(void *)&node,
				CWAPR_RFDC_LIST_NODE_SIZE);

			/* Close current frame before storing FD */
			fdma_store_default_frame_data();

			/* Write FD in external buffer */
			ext_addr = rfdc_ext_addr +
					CWAPR_START_OF_FDS_LIST + index * FD_SIZE;
			cdma_write(ext_addr, (void *)HWC_FD_ADDRESS, FD_SIZE);

			status = FRAG_OK_REASS_NOT_COMPL;
		}
	} else {
		/* Out of order handling */
		status = out_of_order_capwap_frags(rfdc, rfdc_ext_addr,
				last_frag, frag_size,
				frag_offset,
				cwapr_instance);
		if (status == MALFORMED_FRAG)
			return MALFORMED_FRAG;
	}

	/* Only valid fragment runs here */
	if (frag_offset == 0) {
		/* First fragment (frag_offset == 0) */
		rfdc->status |= FIRST_ARRIVED;
		/* Save PRC params for presentation of the reassembled frame */
		rfdc->seg_addr   = prc->seg_address;
		rfdc->seg_length = prc->seg_length;
		rfdc->seg_offset = prc->seg_offset;

		/* Get IP offset */
		rfdc->iphdr_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	}
	return status;
}

CWAPR_CODE_PLACEMENT uint32_t close_in_order_capwap_frags(
				uint64_t rfdc_ext_addr, uint8_t num_of_frags)
{
	struct		ldpaa_fd fds_to_concatenate[2]
			     __attribute__((aligned(sizeof(struct ldpaa_fd))));
	uint64_t	fds_to_fetch_addr;
	struct		fdma_concatenate_frames_params concatenate_params;

	/* Bring into workspace 2 FDs to be concatenated */
	fds_to_fetch_addr = rfdc_ext_addr + CWAPR_START_OF_FDS_LIST;
	cdma_read((void *)fds_to_concatenate,
			  fds_to_fetch_addr,
			  FD_SIZE*2);
	/* Copy 1rst FD to default frame FD's place */
	*((struct ldpaa_fd *)(HWC_FD_ADDRESS)) = fds_to_concatenate[0];

	/* Open 1rst frame and get frame handle */
	fdma_present_default_frame_without_segments();

	/* Open 2nd frame and get frame handle */
	/* reset frame2 field because handle is 2 bytes in concatenate
	   vs 1 byte in present*/
	concatenate_params.frame2 = 0;
	fdma_present_frame_without_segments(
		   fds_to_concatenate+1,
		   FDMA_INIT_NO_FLAGS,
		   0,
		   (uint8_t *)(&(concatenate_params.frame2)) + sizeof(uint8_t));

	concatenate_params.flags  = FDMA_CONCAT_SF_BIT;
	concatenate_params.spid   = *((uint8_t *) HWC_SPID_ADDRESS);
	concatenate_params.frame1 = (uint16_t) PRC_GET_FRAME_HANDLE();
	/* Take header size to be removed from 2nd FD[FRC] */
	concatenate_params.trim   = (uint8_t)fds_to_concatenate[1].frc;

	fdma_concatenate_frames(&concatenate_params);

	num_of_frags -= 2;
	while (num_of_frags >= 2) {
		/* Bring into workspace 2 FDs to be concatenated */
		fds_to_fetch_addr += 2*FD_SIZE;
		cdma_read((void *)fds_to_concatenate,
			  fds_to_fetch_addr,
			  2*FD_SIZE);
		/* Open frame and get frame handle */
		/* reset frame2 field because handle is 2 bytes in concatenate
		   vs 1 byte in present*/
		concatenate_params.frame2 = 0;
		fdma_present_frame_without_segments(
				fds_to_concatenate,
				FDMA_INIT_NO_FLAGS,
				0,
				(uint8_t *)(&(concatenate_params.frame2)) +
				sizeof(uint8_t));

		/* Take header size to be removed from FD[FRC] */
		concatenate_params.trim  = (uint8_t)fds_to_concatenate[0].frc;

		fdma_concatenate_frames(&concatenate_params);

		/* Open frame and get frame handle */
		fdma_present_frame_without_segments(
				fds_to_concatenate+1,
				FDMA_INIT_NO_FLAGS,
				0,
				(uint8_t *)(&(concatenate_params.frame2)) +
				sizeof(uint8_t));

		/* Take header size to be removed from FD[FRC] */
		concatenate_params.trim  = (uint8_t)fds_to_concatenate[1].frc;

		fdma_concatenate_frames(&concatenate_params);

		num_of_frags -= 2;
	}
	if (num_of_frags == 1) {
		/* Handle last even fragment */
		/* Bring into workspace last FD to be concatenated */
		fds_to_fetch_addr += 2*FD_SIZE;
		cdma_read((void *)fds_to_concatenate,
			  fds_to_fetch_addr,
			  FD_SIZE);

		/* reset frame2 field because handle is 2 bytes in concatenate
		   vs 1 byte in present*/
		concatenate_params.frame2 = 0;

		fdma_present_frame_without_segments(
				fds_to_concatenate,
				FDMA_INIT_NO_FLAGS,
				0,
				(uint8_t *)(&(concatenate_params.frame2)) +
				sizeof(uint8_t));

		/* Take header size to be removed from FD[FRC] */
		concatenate_params.trim = (uint8_t)fds_to_concatenate[0].frc;

		fdma_concatenate_frames(&concatenate_params);

	}
	return SUCCESS;
}

CWAPR_CODE_PLACEMENT uint32_t l3_l4_l5_headers_update(struct cwapr_rfdc *rfdc)
{
	uint16_t	ipv4hdr_offset;
	uint16_t	total_len;
	uint16_t	ip_hdr_cksum;
	uint16_t	ip_hdr_len;
	uint16_t	capwap_hdr_len;
	struct ipv4hdr  *ipv4_hdr;
	struct udphdr	*udp_hdr;
	struct capwaphdr *capwap_hdr;
	struct parse_result *pr;
	struct	presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;

	ipv4hdr_offset = rfdc->iphdr_offset;
	ipv4_hdr = (struct ipv4hdr *)
			(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());
	capwap_hdr = (struct capwaphdr *)
			(PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() +
				PRC_GET_SEGMENT_ADDRESS());

	ip_hdr_len = (uint16_t)((ipv4_hdr->vsn_and_ihl & IPV4_HDR_IHL_MASK)<<2);

	capwap_hdr_len = (uint16_t)
		  (((capwap_hdr->hlen_rid_wbid_t & NET_HDR_FLD_CAPWAP_HLEN_MASK) >>
				  NET_HDR_FLD_CAPWAP_HLEN_OFFSET) << 2);

	/* IP header length + UDP header length + CAPWAP header length + payload */
	total_len = ip_hdr_len + sizeof(struct udphdr) +
			capwap_hdr_len + rfdc->curr_total_len;

	/* update IP checksum */
	ip_hdr_cksum = cksum_accumulative_update_uint32(ipv4_hdr->hdr_cksum,
					ipv4_hdr->total_length, total_len);
	ipv4_hdr->hdr_cksum = ip_hdr_cksum;

	/* update IP total length */
	ipv4_hdr->total_length = total_len;

	/* update UDP length and reset checksum */
	udp_hdr = (struct udphdr *)
			(PARSER_GET_L4_OFFSET_DEFAULT() + PRC_GET_SEGMENT_ADDRESS());
	udp_hdr->length = sizeof(struct udphdr) +
				capwap_hdr_len + rfdc->curr_total_len;
	udp_hdr->checksum = 0;

	/* reset CAPWAP 'F' flag */
	capwap_hdr->bits_flags = capwap_hdr->bits_flags & ~NET_HDR_FLD_CAPWAP_F;

	/* update FDMA with new IP header, new UDP header, new CAPWAP header */
	fdma_modify_default_segment_data(ipv4hdr_offset,
			ip_hdr_len + sizeof(struct udphdr) + capwap_hdr_len);

	/* Updated FD[length] */
	LDPAA_FD_SET_LENGTH(HWC_FD_ADDRESS, total_len + ipv4hdr_offset +
					   prc->seg_offset);

	/* Invalidate Gross running sum of the reassembled frame */
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	pr->gross_running_sum = 0;

	/* Run Parser */
	if (parse_result_generate_default(PARSER_VALIDATE_L3_CHECKSUM))
		return EIO;
	return SUCCESS;
}

CWAPR_CODE_PLACEMENT uint32_t close_with_reorder_capwap_frags(
			struct cwapr_rfdc *rfdc, uint64_t rfdc_ext_addr)
{
	uint8_t				num_of_frags;
	uint8_t				frag_index;
	uint8_t				byte_idx;
	uint64_t			temp_ext_addr;
	struct cwapr_rfdc_list_node	link_list[8];
	struct				ldpaa_fd fds_to_concatenate[2]
			      __attribute__((aligned(sizeof(struct ldpaa_fd))));
	struct		fdma_concatenate_frames_params concatenate_prm;

	concatenate_prm.flags = FDMA_CONCAT_SF_BIT;

	if (rfdc->status & ORDER_AND_OOO) {
		if (rfdc->idx_out_of_order == 1) {
			temp_ext_addr = rfdc_ext_addr + CWAPR_START_OF_FDS_LIST;
			cdma_read((void *)HWC_FD_ADDRESS, temp_ext_addr, FD_SIZE);

			/* Copy 1rst FD to default frame FD's place */
			/* *((struct ldpaa_fd *)(HWC_FD_ADDRESS)) =
			 * fds_to_concatenate[0];*/

			/* Open 1rst frame and get frame handle */
			fdma_present_default_frame_without_segments();
			concatenate_prm.frame1 = (uint16_t) PRC_GET_FRAME_HANDLE();
			frag_index = rfdc->first_frag_idx;

			temp_ext_addr = rfdc_ext_addr + CWAPR_START_OF_FDS_LIST +
						frag_index * FD_SIZE;
			cdma_read((void *)fds_to_concatenate, temp_ext_addr,  FD_SIZE);

			/* Open frame and get frame handle */
			/* reset frame2 field because handle is 2 bytes in
			 * concatenate vs 1 byte in present*/
			concatenate_prm.frame2 = 0;

			fdma_present_frame_without_segments(
				    fds_to_concatenate,
				    FDMA_INIT_NO_FLAGS,
				    0,
				    (uint8_t *)(&(concatenate_prm.frame2)) +
				    sizeof(uint8_t));

			/* Take header size to be removed from FD[FRC] */
			concatenate_prm.trim = (uint8_t)fds_to_concatenate[0].frc;
			fdma_concatenate_frames(&concatenate_prm);

			num_of_frags = rfdc->num_of_frags - 2;
			byte_idx = 255; /* invalid value */

		} else {
			frag_index = rfdc->idx_out_of_order;
			close_in_order_capwap_frags(rfdc_ext_addr, frag_index);
			num_of_frags = rfdc->num_of_frags - frag_index - 1;
			frag_index = rfdc->first_frag_idx;

			temp_ext_addr = rfdc_ext_addr + CWAPR_START_OF_FDS_LIST +
								frag_index * FD_SIZE;
			cdma_read((void *)fds_to_concatenate,
					  temp_ext_addr,
					  FD_SIZE);

			/* Open frame and get frame handle */
			/* reset frame2 field because handle is 2 bytes in concatenate
			   vs 1 byte in present*/
			concatenate_prm.frame2 = 0;
			concatenate_prm.frame1 = (uint16_t) PRC_GET_FRAME_HANDLE();

			fdma_present_frame_without_segments(
					fds_to_concatenate,
					FDMA_INIT_NO_FLAGS,
					0,
					(uint8_t *)(&(concatenate_prm.frame2)) +
					sizeof(uint8_t));

			/* Take header size to be removed from FD[FRC] */
			concatenate_prm.trim  = (uint8_t)fds_to_concatenate[0].frc;
			fdma_concatenate_frames(&concatenate_prm);
			byte_idx = 255; /* invalid value */
		}
	} else {
		num_of_frags  = rfdc->num_of_frags;
		frag_index = rfdc->first_frag_idx;

		/* Bring 8 nodes of LL */
		byte_idx = frag_index >> 3;
		temp_ext_addr = rfdc_ext_addr + CWAPR_START_OF_LINK_LIST +
				byte_idx * 8 * CWAPR_RFDC_LIST_NODE_SIZE;
		cdma_read(link_list,
			  temp_ext_addr,
			  8 * CWAPR_RFDC_LIST_NODE_SIZE);
		temp_ext_addr = rfdc_ext_addr + CWAPR_START_OF_FDS_LIST +
						frag_index * FD_SIZE;
		cdma_read(fds_to_concatenate,
				  temp_ext_addr,
				  FD_SIZE);
		frag_index = link_list[frag_index & BYTE_RFDC_LIST_MASK].next_idx;
		temp_ext_addr = rfdc_ext_addr + CWAPR_START_OF_FDS_LIST +
							frag_index * FD_SIZE;
		cdma_read(fds_to_concatenate + 1,
				  temp_ext_addr,
				  FD_SIZE);
		/* Copy 1rst FD to default frame FD's place */
		*((struct ldpaa_fd *)(HWC_FD_ADDRESS)) = fds_to_concatenate[0];

		/* Open 1rst frame and get frame handle */
		fdma_present_default_frame_without_segments();

		/* Open 2nd frame and get frame handle */
		/* reset frame2 field because handle is 2 bytes in concatenate
		   vs 1 byte in present*/
		concatenate_prm.frame2 = 0;
		fdma_present_frame_without_segments(
			   fds_to_concatenate+1,
			   FDMA_INIT_NO_FLAGS,
			   0,
			   (uint8_t *)(&(concatenate_prm.frame2)) +
			   sizeof(uint8_t));

		concatenate_prm.spid   = *((uint8_t *) HWC_SPID_ADDRESS);
		concatenate_prm.frame1 = (uint16_t) PRC_GET_FRAME_HANDLE();
		/* Take header size to be removed from 2nd FD[FRC] */
		concatenate_prm.trim   = (uint8_t)fds_to_concatenate[1].frc;

		fdma_concatenate_frames(&concatenate_prm);

		num_of_frags -= 2;
	}
	while (num_of_frags != 0) {
		if ((frag_index >> 3) == byte_idx)
			frag_index =
					link_list[frag_index & BYTE_RFDC_LIST_MASK].next_idx;
		else {
			/* Bring 8 nodes of LL */
			byte_idx = frag_index >> 3;
			temp_ext_addr = rfdc_ext_addr + CWAPR_START_OF_LINK_LIST +
					byte_idx * 8 *
					CWAPR_RFDC_LIST_NODE_SIZE;
			cdma_read(link_list,
				  temp_ext_addr,
				  8 * CWAPR_RFDC_LIST_NODE_SIZE);

			frag_index =
					link_list[frag_index & BYTE_RFDC_LIST_MASK].next_idx;
		}

		temp_ext_addr = rfdc_ext_addr + CWAPR_START_OF_FDS_LIST +
							frag_index * FD_SIZE;

		cdma_read((void *)fds_to_concatenate,
				  temp_ext_addr,
				  FD_SIZE);
		/* Open frame and get frame handle */
		/* reset frame2 field because handle is 2 bytes in concatenate
		   vs 1 byte in present*/
		/* todo move this reset to be done once and not each
		 * iteration */
		concatenate_prm.frame2 = 0;
		fdma_present_frame_without_segments(
				fds_to_concatenate,
				FDMA_INIT_NO_FLAGS,
				0,
				(uint8_t *)(&(concatenate_prm.frame2)) +
				sizeof(uint8_t));

		/* Take header size to be removed from FD[FRC] */
		concatenate_prm.trim  = (uint8_t)fds_to_concatenate[0].frc;

		fdma_concatenate_frames(&concatenate_prm);

		num_of_frags -= 1;
	}

	return SUCCESS;
}

CWAPR_CODE_PLACEMENT uint32_t check_for_capwap_frag_error(
					struct capwaphdr *capwap_hdr)
{
	uint16_t	hdr_len, frag_size;
	uint8_t		last_frag;

	hdr_len = (uint16_t)(((capwap_hdr->hlen_rid_wbid_t &
			NET_HDR_FLD_CAPWAP_HLEN_MASK) >>
			NET_HDR_FLD_CAPWAP_HLEN_OFFSET) << 2);

	/* Get size of CAPWAP payload for current fragment */
	frag_size = (uint16_t)(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() - hdr_len);

	last_frag = capwap_hdr->bits_flags & NET_HDR_FLD_CAPWAP_L;

	/* Check CAPWAP size is multiple of 8 for First or middle fragment */
	if (!last_frag && (frag_size % 8 != 0))
		return MALFORMED_FRAG;

	return NO_ERROR;
}

void timeout_cwapr_reassemble(uint64_t rfdc_ext_addr, uint16_t opaque)
{
	struct	 cwapr_rfdc rfdc __attribute__((aligned(16)));
	struct	 cwapr_instance cwapr_instance;
	struct   dpni_drv *dpni_drv;
	uint16_t rfdc_status;
	uint32_t flags;
	uint32_t i;
	uint8_t  enter_number;
	uint8_t  num_of_frags;
	uint8_t  first_frag_idx;
	uint8_t  index = 0;

	UNUSED(opaque);

	/* read RFDC */
	cdma_read(&rfdc, rfdc_ext_addr, CWAPR_RFDC_SIZE);

	/* read instance parameters */
	cdma_read(&cwapr_instance, rfdc.instance_handle, CWAPR_INSTANCE_SIZE);

	/* Recover OSM scope */
	enter_number = (uint8_t)((rfdc.status & SCOPE_LEVEL) >> 4) -
					default_task_params.current_scope_level;
	for (i = 0; i < enter_number; i++) {
		/* Intentionally doesn't relinquish parent automatically */
		osm_scope_enter(OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE,
						(uint32_t)rfdc_ext_addr);
	}
	osm_scope_enter(OSM_SCOPE_ENTER_CHILD_TO_EXCLUSIVE, (uint32_t)rfdc_ext_addr);

	/* confirm timer expiration */
	tman_timer_completion_confirmation(rfdc.timer_handle);

	/* Re-read RFDC since it could have changed */
	cdma_read(&rfdc, rfdc_ext_addr, CWAPR_RFDC_SIZE);

	rfdc_status = rfdc.status;

	table_rule_delete(TABLE_ACCEL_ID_CTLU,
			  cwapr_instance.table_id,
			  (union table_key_desc *)&rfdc.table_key,
			  CWAPR_KEY_SIZE,
			  NULL);
	/* DEBUG: check EIO */

	/* Decrement no of IPv4 open frames in instance data structure*/
	ste_dec_counter(rfdc.instance_handle +
			offsetof(struct cwapr_instance, num_of_open_reass_frames),
			1,
			STE_MODE_32_BIT_CNTR_SIZE);
	/* Increment no of frames in TO stats */
	update_stats_cwapr(&cwapr_instance,
			 offsetof(struct cwapr_stats_cntrs, time_out_cntr));

	/* Reset valid indication in RFDC */
	rfdc.status = rfdc_status & ~RFDC_VALID;

	//set_default_amq_attributes(&rfdc.isolation_bits);
	/* Update task default params */
	dpni_drv = nis + rfdc.niid;
	sl_tman_expiration_task_prolog(dpni_drv->dpni_drv_params_var.spid);
	default_task_params.parser_starting_hxs =
				   dpni_drv->dpni_drv_params_var.starting_hxs;
	default_task_params.parser_profile_id =
				   dpni_drv->dpni_drv_params_var.prpid;

	/* Discard all the fragments except the first one or one of them */
	num_of_frags = rfdc.num_of_frags;
	if (num_of_frags == 0) {
		/* Non-conform frame opened the entry */
		/* CDMA write, dec ref_cnt and release if ref_cnt=0 */
		cdma_access_context_memory(rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE
				  | CWAPR_RFDC_SIZE,
				  (uint32_t *)CWAPR_REF_COUNT_ADDR);
		/* No need to handle OSM level since task will be terminated */
		fdma_terminate_task();
	}
	if ((rfdc_status & OUT_OF_ORDER) && !(rfdc_status & ORDER_AND_OOO))
		first_frag_idx = rfdc.first_frag_idx;
	else
		first_frag_idx = 0;
	while (num_of_frags != 0) {

		if (index != first_frag_idx) {
			cdma_read((void *) HWC_FD_ADDRESS,
				  rfdc_ext_addr+CWAPR_START_OF_FDS_LIST+index*32,
				  FD_SIZE);
			fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS,
					FDMA_DIS_NO_FLAGS);
		}
		num_of_frags--;
		index++;
	}
	/* Fetch FD's fragment to be returned */
	cdma_read((void *) HWC_FD_ADDRESS,
		   rfdc_ext_addr+CWAPR_START_OF_FDS_LIST+first_frag_idx*32,
		   FD_SIZE);
	/* CDMA write, dec ref_cnt and release if ref_cnt=0 */
	cdma_access_context_memory(rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE
				  | CWAPR_RFDC_SIZE,
				  (uint32_t *)CWAPR_REF_COUNT_ADDR);

	/* Return to original scope level of timer task */
	for (i = 0; i <= enter_number; i++)
		osm_scope_exit();

	if (rfdc_status & FIRST_ARRIVED)
		flags = CWAPR_TO_CB_FIRST_FRAG;
	else
		flags = 0;
	/* Present the fragment to be returned to the user */
	/* todo check if spid should be stored in rfdc and restored here */
	fdma_present_default_frame_without_segments();

	cwapr_instance.timeout_cb(cwapr_instance.cb_timeout_arg, flags);
}

CWAPR_CODE_PLACEMENT void return_to_correct_ordering_scope(uint32_t osm_status)
{
	/* return to original ordering scope that entered
	 * the cwapr_reassemble function */
	osm_scope_exit();
	if (osm_status == 0) {
		/* Tasks which started in concurrent and have 2 free levels */
		osm_scope_exit();
	}
}

CWAPR_CODE_PLACEMENT uint32_t out_of_order_capwap_frags(struct cwapr_rfdc *rfdc,
		uint64_t rfdc_ext_addr, uint32_t last_frag,
		uint16_t frag_size, uint16_t frag_offset,
		struct cwapr_instance *cwapr_instance)
{
	uint8_t				curr_idx;
	uint8_t				tmp_frag_idx;
	uint8_t				first_frag_idx;
	uint8_t				byte_idx;
	uint8_t				new_frag_idx;
	uint8_t				curr_idx_in_byte;
	uint8_t				tmp_idx_in_byte;
	uint8_t				new_frag_idx_in_byte;
	uint16_t			tmp_total_payload;
	uint64_t			curr_node_ext_addr;
	uint64_t			tmp_node_ext_addr;
	uint64_t			new_frag_ext_addr;
	uint64_t			byte_ext_addr;
	struct cwapr_rfdc_list_node	*tmp_node;
	struct cwapr_rfdc_list_node	*curr_node;
	struct cwapr_rfdc_list_node	link_list[8];


	if (frag_offset < rfdc->total_in_order_payload) {
		/* overlap or duplicate */
		return MALFORMED_FRAG;
	}
	curr_idx = rfdc->next_idx;
	curr_node_ext_addr = rfdc_ext_addr + CWAPR_START_OF_LINK_LIST +
				curr_idx * CWAPR_RFDC_LIST_NODE_SIZE;
	first_frag_idx = rfdc->first_frag_idx;
	tmp_frag_idx = rfdc->last_frag_idx;
	tmp_node_ext_addr = rfdc_ext_addr + CWAPR_START_OF_LINK_LIST +
				CWAPR_RFDC_LIST_NODE_SIZE * tmp_frag_idx;
	tmp_total_payload = rfdc->biggest_payload;
	if (frag_offset >= tmp_total_payload) {
		/* Bigger than last */
		tmp_node = link_list;
		cdma_read_wrp((void *)tmp_node,
			tmp_node_ext_addr,
			CWAPR_RFDC_LIST_NODE_SIZE);
		if (rfdc->exp_total_len) {
			/* Error */
			return MALFORMED_FRAG;
		}
		if (last_frag)
			rfdc->exp_total_len = frag_offset + frag_size;

		rfdc->biggest_payload = frag_offset + frag_size;
		curr_node = link_list + 1;
		rfdc->last_frag_idx = curr_idx;
		tmp_node->next_idx = curr_idx;
		curr_node->prev_idx  = tmp_frag_idx;
		curr_node->frag_offset = frag_offset;
		curr_node->frag_len = frag_size;
		/* not required */
		curr_node->next_idx = 0;
		cdma_write_wrp(tmp_node_ext_addr,
				tmp_node,
				CWAPR_RFDC_LIST_NODE_SIZE);
		cdma_write_wrp(curr_node_ext_addr,
				curr_node,
				CWAPR_RFDC_LIST_NODE_SIZE);
	} else {
		/* Smaller than last */
		if (last_frag) {
			/* Current fragment is smaller than last but is marked
			 * as last */
			return MALFORMED_FRAG;
		}
		/* Bring 8 nodes of the Link List */
		byte_idx = tmp_frag_idx >> 3;
		byte_ext_addr = rfdc_ext_addr + CWAPR_START_OF_LINK_LIST +
			CWAPR_RFDC_LIST_NODE_SIZE * 8 * byte_idx;
		cdma_read_wrp(link_list,
			  byte_ext_addr,
			  8*CWAPR_RFDC_LIST_NODE_SIZE);
		tmp_idx_in_byte = tmp_frag_idx & BYTE_RFDC_LIST_MASK;
		tmp_node = link_list + tmp_idx_in_byte;
		if ((frag_offset + frag_size) > tmp_node->frag_offset) {
			/* Overlap */
			return MALFORMED_FRAG;
		}
		do {
			if (tmp_frag_idx == first_frag_idx) {
				rfdc->first_frag_idx = curr_idx;
				tmp_node->prev_idx = curr_idx;
				curr_idx_in_byte = curr_idx & BYTE_RFDC_LIST_MASK;
				if ((curr_idx >> 3) == byte_idx) {
					link_list[curr_idx_in_byte].frag_len = frag_size;
					link_list[curr_idx_in_byte].frag_offset = frag_offset;
					link_list[curr_idx_in_byte].next_idx = tmp_frag_idx;
						/* not required */
					link_list[curr_idx_in_byte].prev_idx = 0;

					cdma_write_wrp(byte_ext_addr,
						   link_list,
						   8*CWAPR_RFDC_LIST_NODE_SIZE);
				} else {
					if (tmp_idx_in_byte == 0)
						curr_node = tmp_node + 1;
					else
						curr_node = tmp_node - 1;

					curr_node->frag_len = frag_size;
					curr_node->frag_offset = frag_offset;
					curr_node->next_idx = tmp_frag_idx;
					/* not required */
					curr_node->prev_idx  = 0;

				    cdma_write_wrp(curr_node_ext_addr,
						   curr_node,
						   CWAPR_RFDC_LIST_NODE_SIZE);
				    tmp_node_ext_addr = rfdc_ext_addr +
						    CWAPR_START_OF_LINK_LIST +
						    CWAPR_RFDC_LIST_NODE_SIZE *
						    tmp_frag_idx;
				    cdma_write_wrp(tmp_node_ext_addr,
						   tmp_node,
						   CWAPR_RFDC_LIST_NODE_SIZE);
				}
				rfdc->curr_total_len += frag_size;
				rfdc->num_of_frags++;

				if (rfdc->curr_total_len == rfdc->exp_total_len) {
					/* Reassembly is completed */
					/* Check max reassembly size */
					if ((rfdc->curr_total_len +
						rfdc->first_frag_hdr_length) <=
						cwapr_instance->max_reass_frm_size) {

						/* Close current frame before storing FD */
						fdma_store_default_frame_data_wrp();

						/* Write FD in external buffer */
						curr_node_ext_addr = rfdc_ext_addr +
									   CWAPR_START_OF_FDS_LIST +
									   curr_idx * FD_SIZE;
						cdma_write_wrp(curr_node_ext_addr,
							       (void *)HWC_FD_ADDRESS,
							       FD_SIZE);

						return LAST_FRAG_OUT_OF_ORDER;
					} else
						return MALFORMED_FRAG;
				} else {
					/* reassembly is not completed */

					/* Close current frame before storing FD */
					fdma_store_default_frame_data_wrp();

					/* Write FD in external buffer */
					curr_node_ext_addr = rfdc_ext_addr +
						CWAPR_START_OF_FDS_LIST +
						curr_idx * FD_SIZE;
					cdma_write_wrp(curr_node_ext_addr,
						       (void *)HWC_FD_ADDRESS,
						       FD_SIZE);

					rfdc->next_idx = curr_idx + 1;
				    return FRAG_OK_REASS_NOT_COMPL;
				}

			}
			tmp_frag_idx = tmp_node->prev_idx;
			if ((tmp_frag_idx>>3) != byte_idx) {
				/* Bring 8 nodes of the Link List */
				/* todo check if compiler add a clock
				   for next line */
				byte_idx = tmp_frag_idx >> 3;
				byte_ext_addr = rfdc_ext_addr +
						CWAPR_START_OF_LINK_LIST +
						CWAPR_RFDC_LIST_NODE_SIZE * 8 *
						byte_idx;
				cdma_read_wrp(link_list,
					  byte_ext_addr,
					  8*CWAPR_RFDC_LIST_NODE_SIZE);
			}
			tmp_idx_in_byte = tmp_frag_idx &
					      BYTE_RFDC_LIST_MASK;
			tmp_node = link_list + tmp_idx_in_byte;
		} while ((frag_offset + frag_size) <= tmp_node->frag_offset);

		tmp_total_payload = tmp_node->frag_offset + tmp_node->frag_len;
		if (frag_offset >= tmp_total_payload) {
			curr_idx_in_byte =
					curr_idx & BYTE_RFDC_LIST_MASK;
			if ((curr_idx >> 3) == byte_idx) {
				new_frag_idx = tmp_node->next_idx;
				link_list[curr_idx_in_byte].frag_len = frag_size;
				link_list[curr_idx_in_byte].frag_offset = frag_offset;
				link_list[curr_idx_in_byte].next_idx = new_frag_idx;
				link_list[curr_idx_in_byte].prev_idx = tmp_frag_idx;

				link_list[tmp_idx_in_byte].next_idx = curr_idx;

				if ((new_frag_idx >> 3) == byte_idx) {
					new_frag_idx_in_byte = new_frag_idx & BYTE_RFDC_LIST_MASK;
					link_list[new_frag_idx_in_byte].prev_idx = curr_idx;
					cdma_write_wrp(byte_ext_addr,
						   link_list,
						   8*CWAPR_RFDC_LIST_NODE_SIZE);

				} else {
					cdma_write_wrp(byte_ext_addr,
						   link_list,
						   8*CWAPR_RFDC_LIST_NODE_SIZE);
					new_frag_ext_addr = rfdc_ext_addr +
							CWAPR_START_OF_LINK_LIST +
						CWAPR_RFDC_LIST_NODE_SIZE * new_frag_idx;
					cdma_read_wrp(link_list,
						  new_frag_ext_addr,
						  CWAPR_RFDC_LIST_NODE_SIZE);
					link_list[0].prev_idx = curr_idx;
					cdma_write_wrp(new_frag_ext_addr,
						  link_list,
						  CWAPR_RFDC_LIST_NODE_SIZE);
				  }
			} else {
				new_frag_idx = tmp_node->next_idx;
				link_list[tmp_idx_in_byte].next_idx =
									curr_idx;

				if ((new_frag_idx >> 3) == byte_idx) {
					new_frag_idx_in_byte = new_frag_idx &
								BYTE_RFDC_LIST_MASK;
					link_list[new_frag_idx_in_byte].prev_idx =
									  curr_idx;
					/* update temp and new_frag nodes */
					cdma_write_wrp(byte_ext_addr,
						   link_list,
						   8*CWAPR_RFDC_LIST_NODE_SIZE);
				} else {
					/* update temp nodes */
					cdma_write_wrp(byte_ext_addr,
						   link_list,
						   8*CWAPR_RFDC_LIST_NODE_SIZE);
					new_frag_ext_addr = rfdc_ext_addr +
							CWAPR_START_OF_LINK_LIST +
							CWAPR_RFDC_LIST_NODE_SIZE *
							new_frag_idx;
					cdma_read_wrp(link_list,
						 new_frag_ext_addr,
						 CWAPR_RFDC_LIST_NODE_SIZE);
					link_list[0].prev_idx = curr_idx;
					cdma_write_wrp(new_frag_ext_addr,
						  link_list,
						  CWAPR_RFDC_LIST_NODE_SIZE);
				}

				link_list[0].frag_len = frag_size;
				link_list[0].frag_offset = frag_offset;
				link_list[0].next_idx  = new_frag_idx;
				link_list[0].prev_idx  = tmp_frag_idx;

				cdma_write_wrp(curr_node_ext_addr,
						link_list,
						CWAPR_RFDC_LIST_NODE_SIZE);
			 }
		} else {
			/* Error */
			return MALFORMED_FRAG;
		}
	}

	rfdc->curr_total_len += frag_size;
	rfdc->num_of_frags++;

	if (rfdc->curr_total_len == rfdc->exp_total_len) {
		/* Reassembly is completed */
		/* Check max reassembly size */
		if ((rfdc->curr_total_len + rfdc->first_frag_hdr_length) <=
			cwapr_instance->max_reass_frm_size) {
			/* Close current frame before storing FD */
			fdma_store_default_frame_data_wrp();

			/* Write FD in external buffer */
			curr_node_ext_addr = rfdc_ext_addr +
				CWAPR_START_OF_FDS_LIST + curr_idx * FD_SIZE;
			cdma_write_wrp(curr_node_ext_addr,
					(void *)HWC_FD_ADDRESS,
					FD_SIZE);

			return LAST_FRAG_OUT_OF_ORDER;
		} else
			return MALFORMED_FRAG;
	} else {
		/* reassembly is not completed */
		/* Close current frame before storing FD */
		fdma_store_default_frame_data_wrp();

		/* Write FD in external buffer */
		curr_node_ext_addr = rfdc_ext_addr + CWAPR_START_OF_FDS_LIST +
					curr_idx * FD_SIZE;
		cdma_write_wrp(curr_node_ext_addr,
				   (void *)HWC_FD_ADDRESS,
				   FD_SIZE);

		rfdc->next_idx++;
		return FRAG_OK_REASS_NOT_COMPL;
	}
}

void update_stats_cwapr(struct cwapr_instance *cwapr_instance,
		      uint32_t counter_offset)
{
	if (cwapr_instance->flags & CWAPR_MODE_EXTENDED_STATS_EN) {
		ste_inc_counter(cwapr_instance->extended_stats_addr +
				counter_offset,
				1,
				STE_MODE_32_BIT_CNTR_SIZE);
	}
}

void cwapr_modify_max_reass_frm_size(
		cwapr_instance_handle_t cwapr_instance_handle,
		uint16_t max_reass_frm_size)
{
	cdma_write(cwapr_instance_handle +
			offsetof(struct cwapr_instance, max_reass_frm_size),
		   &max_reass_frm_size,
		   sizeof(max_reass_frm_size));
}

void cwapr_modify_timeout_value(cwapr_instance_handle_t cwapr_instance_handle,
				uint16_t reasm_timeout_value)
{
	cdma_write(cwapr_instance_handle +
		   offsetof(struct cwapr_instance, timeout_value),
		   &reasm_timeout_value,
		   sizeof(reasm_timeout_value));
}

void cwapr_get_reass_frm_cntr(cwapr_instance_handle_t cwapr_instance_handle,
				uint32_t *reass_frm_cntr)
{
	cdma_read(reass_frm_cntr,
		  cwapr_instance_handle +
		  offsetof(struct cwapr_instance, reass_frm_cntr),
		  sizeof(*reass_frm_cntr));
}

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"
#pragma stackinfo_ignore on
void cwapr_exception_handler(enum cwapr_function_identifier func_id,
		     uint32_t line,
		     int32_t status)
{
	char *func_name;
	char *err_msg;

	status = status & 0xFF;

	/* Translate function ID to function name string */
	switch (func_id) {
	case CWAPR_INIT:
		func_name = "cwapr_init";
		switch (status) {
		case EINVAL:
			err_msg = "KCR exceeds maximum KCR size\n";
			break;
		case ENOSPC:
			err_msg = "No more KCRs are available\n";
			break;
		default:
			err_msg = "Unknown or Invalid status Error.\n";
		}
		break;
	case CWAPR_CREATE_INSTANCE:
		func_name = "cwapr_create_instance";
		switch (status) {
		case EINVAL:
			err_msg = "Could not release into BPID, "
				   "BPID is full\n";
			break;
		case ENOMEM:
			   err_msg = "Not enough memory for partition id\n";
			   break;
		case ENOSPC:
			err_msg = "Buffer Pool Depletion\n";
			break;
		case ENOSPC_TIMER:
			err_msg = "No free timer\n";
			break;
		case ENOMEM_TABLE:
			err_msg = "Not enough memory available to create "
				   "table\n";
			break;
		default:
			err_msg = "Unknown or Invalid status Error.\n";
		}
		break;
	case CWAPR_DELETE_INSTANCE:
		func_name = "cwapr_delete_instance";
		err_msg = "No free timer for this tmi id\n";
		break;
	case CWAPR_REASSEMBLE:
		func_name = "cwapr_reassemble";
		switch (status) {
		case ENOSPC:
			 err_msg = "Buffer Pool Depletion\n";
			 break;
		default:
			err_msg = "Unknown or Invalid status Error.\n";
		}
		break;
	default:
		/* create own exception */
		func_name = "Unknown Function";
	}

	exception_handler(__FILE__, func_name, line, err_msg);
}

#pragma pop

