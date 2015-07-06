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
@File		ipr.c

@Description	This file contains the AIOP SW IP Reassembly implementation.

*//***************************************************************************/


#include "general.h"
#include "system.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_osm.h"
#include "fsl_table.h"
#include "fsl_keygen.h"
#include "fsl_ste.h"
#include "fsl_net.h"
#include "fsl_spinlock.h"
#include "checksum.h"
#include "ipr.h"
#include "fsl_cdma.h"
#include "ip.h"
#include "fsl_platform.h"
#include "fsl_sl_dpni_drv.h"

/* For wrapper functions */
#include "cdma.h"
#include "table.h"
#include "tman.h"

#ifdef AIOP_VERIF
#include "slab_stub.h"
#else
#include "fsl_dbg.h"
#include "fsl_sl_slab.h"
#endif

struct  ipr_global_parameters ipr_global_parameters1;
extern struct dpni_drv *nis;
extern __TASK struct aiop_default_task_params default_task_params;


#ifndef AIOP_VERIF
int ipr_early_init(uint32_t nbr_of_instances, uint32_t nbr_of_context_buffers)
{
	uint32_t nbr_of_ipr_contexts;
	int err;
	
	nbr_of_ipr_contexts = nbr_of_context_buffers + 2*nbr_of_instances;
	/* IPR 2688 rounded up modulo 64 - 8 */
	err = slab_register_context_buffer_requirements(nbr_of_ipr_contexts,
							nbr_of_ipr_contexts,
							2744,
							64,
							MEM_PART_DP_DDR,
							0,
							0);
	if(err){
		pr_err("Failed to register IPR context buffers\n");
		return err;
	}
	return 0;
}
#endif

int ipr_init(void)
{
	struct kcr_builder kb __attribute__((aligned(16)));
	int    status;
	uint8_t  ipr_key_id;

	/* For IPv4 */
	keygen_kcr_builder_init(&kb);
	keygen_kcr_builder_add_protocol_specific_field(KEYGEN_KCR_IPSRC_1_FECID,
			NULL , &kb);
	keygen_kcr_builder_add_protocol_specific_field(KEYGEN_KCR_IPDST_1_FECID,
				NULL , &kb);
	keygen_kcr_builder_add_protocol_specific_field(KEYGEN_KCR_PTYPE_1_FECID,
					NULL , &kb);
	keygen_kcr_builder_add_protocol_specific_field(KEYGEN_KCR_IPID_1_FECID,
					NULL , &kb);
	status = keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU,
			  kb.kcr,
			  &ipr_key_id);
	if (status < 0) {
		/* todo  Fatal */
		ipr_exception_handler(IPR_INIT, __LINE__, (int32_t)status);

	}
	ipr_global_parameters1.ipr_key_id_ipv4 = ipr_key_id;
	/* For IPv6 */
	keygen_kcr_builder_init(&kb);
	keygen_kcr_builder_add_protocol_specific_field(KEYGEN_KCR_IPSRC_1_FECID,
			NULL , &kb);
	keygen_kcr_builder_add_protocol_specific_field(KEYGEN_KCR_IPDST_1_FECID,
				NULL , &kb);
	keygen_kcr_builder_add_protocol_specific_field(KEYGEN_KCR_IPID_1_FECID,
					NULL , &kb);
	status = keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU,
			  kb.kcr,
			  &ipr_key_id);
	ipr_global_parameters1.ipr_key_id_ipv6 = ipr_key_id;

	if (status < 0) {
		/* todo  Fatal */
		ipr_exception_handler(IPR_INIT, __LINE__, (int32_t)status);
	}
	return 0;
}

int ipr_create_instance(struct ipr_params *ipr_params_ptr,
			    ipr_instance_handle_t *ipr_instance_ptr)
{
	struct ipr_instance	      ipr_instance;
	struct ipr_instance_extension ipr_instance_ext;
	struct table_create_params tbl_params;
	uint32_t max_open_frames, aggregate_open_frames, table_location;
	uint16_t table_location_attr;
	uint16_t bpid;
	int table_ipv4_valid = 0;
	int sr_status;

	/* Reservation is done for 2 extra buffers: 1 one instance_params and
	 * the other in case of reaching the maximum of open reassembly */
	aggregate_open_frames = ipr_params_ptr->max_open_frames_ipv4 +
				ipr_params_ptr->max_open_frames_ipv6 + 2;
	/* call ARENA function for allocating buffers needed to IPR
	 * processing (create_slab ) */
	sr_status = slab_find_and_reserve_bpid(aggregate_open_frames,
					IPR_CONTEXT_SIZE,
					8,
					MEM_PART_DP_DDR,
					NULL,
					&bpid);

	if (sr_status < 0)
		ipr_exception_handler(IPR_CREATE_INSTANCE, __LINE__,
				      (int32_t)sr_status);

	sr_status = cdma_acquire_context_memory(bpid,
					  ipr_instance_ptr);
	if (sr_status)
		ipr_exception_handler(IPR_CREATE_INSTANCE, __LINE__,
				      (int32_t)sr_status);

	ipr_instance.bpid = bpid;
	ipr_instance.flags = ipr_params_ptr->flags;

	/* For IPv4 */
	max_open_frames = ipr_params_ptr->max_open_frames_ipv4;
	/* Initialize instance parameters */
	ipr_instance.table_id_ipv4 = 0;
	ipr_instance.table_id_ipv6 = 0;
	if (max_open_frames) {
		ipr_instance.flags |= IPV4_VALID;
		ipr_instance_ext.max_open_frames_ipv4 = max_open_frames;
		tbl_params.committed_rules = max_open_frames;
		tbl_params.max_rules = max_open_frames;
		/* IPv4 src, IPv4 dst, prot, ID */
		tbl_params.key_size = 11;
		table_location = ipr_params_ptr->flags & 0x0C000000;
		if (table_location == IPR_MODE_TABLE_LOCATION_INT)
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_INT;
		else if (table_location == IPR_MODE_TABLE_LOCATION_PEB)
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_PEB;
		else if (table_location == IPR_MODE_TABLE_LOCATION_EXT1)
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_DP_DDR;
		else if (table_location == IPR_MODE_TABLE_LOCATION_EXT2)
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_SYS_DDR;
		tbl_params.attributes = TABLE_ATTRIBUTE_TYPE_EM | \
				table_location_attr | \
				TABLE_ATTRIBUTE_MR_NO_MISS;
		sr_status = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params,
				&ipr_instance.table_id_ipv4);
		if (sr_status != TABLE_STATUS_SUCCESS) {
			/* todo SR error case */
			cdma_release_context_memory(*ipr_instance_ptr);
			ipr_exception_handler(IPR_CREATE_INSTANCE, __LINE__,
					      ENOMEM_TABLE);
		}
		table_ipv4_valid = 1;
	}
	/* For IPv6 */
	max_open_frames = ipr_params_ptr->max_open_frames_ipv6;
	if (max_open_frames) {
		ipr_instance.flags |= IPV6_VALID;
		ipr_instance_ext.max_open_frames_ipv6 = max_open_frames;
		tbl_params.committed_rules = max_open_frames;
		tbl_params.max_rules = max_open_frames;
		/* IPv6 src, IPv6 dst, ID (4 bytes) */
		tbl_params.key_size = 36;
		table_location = ipr_params_ptr->flags & 0x0C000000;
		if (table_location == IPR_MODE_TABLE_LOCATION_INT)
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_INT;
		else if (table_location == IPR_MODE_TABLE_LOCATION_PEB)
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_PEB;
		else if (table_location == IPR_MODE_TABLE_LOCATION_EXT1)
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_DP_DDR;
		else if (table_location == IPR_MODE_TABLE_LOCATION_EXT2)
			table_location_attr = TABLE_ATTRIBUTE_LOCATION_SYS_DDR;
		tbl_params.attributes = TABLE_ATTRIBUTE_TYPE_EM | \
				table_location_attr | \
				TABLE_ATTRIBUTE_MR_NO_MISS;
		sr_status = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params,
				&ipr_instance.table_id_ipv6);
		if (sr_status != TABLE_STATUS_SUCCESS) {
			/* todo SR error case */
			cdma_release_context_memory(*ipr_instance_ptr);
			if (table_ipv4_valid)
				table_delete(TABLE_ACCEL_ID_CTLU,
					     ipr_instance.table_id_ipv4);
			ipr_exception_handler(IPR_CREATE_INSTANCE, __LINE__,
					      ENOMEM_TABLE);
		}
	}

	/* Initialize instance parameters */
	ipr_instance.extended_stats_addr = ipr_params_ptr->extended_stats_addr;
	ipr_instance.max_reass_frm_size = ipr_params_ptr->max_reass_frm_size;
	ipr_instance.min_frag_size_ipv4 = ipr_params_ptr->min_frag_size_ipv4;
	ipr_instance.min_frag_size_ipv6 = ipr_params_ptr->min_frag_size_ipv6;
	ipr_instance.timeout_value_ipv4 = ipr_params_ptr->timeout_value_ipv4;
	ipr_instance.timeout_value_ipv6 = ipr_params_ptr->timeout_value_ipv6;
	ipr_instance.ipv4_timeout_cb = ipr_params_ptr->ipv4_timeout_cb;
	ipr_instance.ipv6_timeout_cb = ipr_params_ptr->ipv6_timeout_cb;
	ipr_instance.cb_timeout_ipv4_arg = ipr_params_ptr->cb_timeout_ipv4_arg;
	ipr_instance.cb_timeout_ipv6_arg = ipr_params_ptr->cb_timeout_ipv6_arg;
	ipr_instance.tmi_id = ipr_params_ptr->tmi_id;

	/* Write ipr instance data structure */
	cdma_write(*ipr_instance_ptr, &ipr_instance, IPR_INSTANCE_SIZE);

	/* Initialization of ipr instance extension parameters */
	ipr_instance_ext.confirm_delete_cb = 0;
	ipr_instance_ext.delete_arg	   = 0;
	ipr_instance_ext.num_of_open_reass_frames_ipv4 = 0;
	ipr_instance_ext.num_of_open_reass_frames_ipv6 = 0;
	ipr_instance_ext.ipv4_reass_frm_cntr = 0;
	ipr_instance_ext.ipv6_reass_frm_cntr = 0;

	/* Write ipr instance extension data structure */
	cdma_write((*ipr_instance_ptr)+sizeof(struct ipr_instance),
		   &ipr_instance_ext,
		   sizeof(struct ipr_instance_extension));

	return SUCCESS;
}

int ipr_delete_instance(ipr_instance_handle_t ipr_instance_ptr,
			    ipr_del_cb_t *confirm_delete_cb,
			    ipr_del_arg_t delete_arg)
{
	struct ipr_instance	ipr_instance;
	uint16_t		timeout_value;
	uint64_t		ipr_instance_extension_ptr;
	struct ipr_instance_ext_delete delete_args;
	int			sr_status;

	cdma_read(&ipr_instance, ipr_instance_ptr, IPR_INSTANCE_SIZE);

	if (ipr_instance.timeout_value_ipv4 > ipr_instance.timeout_value_ipv6)
		timeout_value = ipr_instance.timeout_value_ipv4;
	else {
		timeout_value = ipr_instance.timeout_value_ipv6;
		/* Save biggest timeout value for potential later create timer */
		ipr_instance.timeout_value_ipv4 = timeout_value;
	}
	ipr_instance_extension_ptr = ((uint64_t)ipr_instance_ptr) +
				     sizeof(struct ipr_instance);

	delete_args.confirm_delete_cb = confirm_delete_cb;
	delete_args.delete_arg        = delete_arg;
	
	cdma_write(ipr_instance_extension_ptr,
		   &delete_args,
		   sizeof( struct ipr_instance_ext_delete));

	sr_status = tman_create_timer(ipr_instance.tmi_id,
			IPR_TIMEOUT_FLAGS,
			timeout_value,
			(tman_arg_8B_t) ipr_instance_ptr,
			(tman_arg_2B_t) NULL,
			(tman_cb_t) ipr_delete_instance_after_time_out,
			&ipr_instance.flags);
	if(sr_status)
		ipr_exception_handler(IPR_DELETE_INSTANCE,__LINE__,
				      (int32_t) sr_status);

	return SUCCESS;
}

void ipr_delete_instance_after_time_out(ipr_instance_handle_t ipr_instance_ptr)
{
	struct ipr_instance_and_extension	ipr_instance_and_extension;
	int					sr_status;

	ste_barrier();

	cdma_read(&ipr_instance_and_extension,
		  ipr_instance_ptr,
		  IPR_INSTANCE_SIZE + sizeof(struct ipr_instance_extension));

	if ((ipr_instance_and_extension.
		ipr_instance_extension.num_of_open_reass_frames_ipv4 != 0)
		||
	   (ipr_instance_and_extension.
		ipr_instance_extension.num_of_open_reass_frames_ipv6 != 0))
	{
		/* Not all the timeouts expired, give more time */
		sr_status = tman_create_timer(
		     ipr_instance_and_extension.ipr_instance.tmi_id,
		     IPR_TIMEOUT_FLAGS,
		     ipr_instance_and_extension.ipr_instance.timeout_value_ipv4,
		     (tman_arg_8B_t) ipr_instance_ptr,
		     (tman_arg_2B_t) NULL,
		     (tman_cb_t) ipr_delete_instance_after_time_out,
		     &ipr_instance_and_extension.ipr_instance.flags);
		if(sr_status)
			ipr_exception_handler(IPR_DELETE_INSTANCE,__LINE__,
					      (int32_t) sr_status);

		fdma_terminate_task();
	}
	else
		
	/* todo SR error case */
	cdma_release_context_memory(ipr_instance_ptr);
	/* error case */
	if (ipr_instance_and_extension.ipr_instance.flags & IPV4_VALID)
		table_delete(TABLE_ACCEL_ID_CTLU,
			 ipr_instance_and_extension.ipr_instance.table_id_ipv4);
	if (ipr_instance_and_extension.ipr_instance.flags & IPV6_VALID)
		table_delete(TABLE_ACCEL_ID_CTLU,
			ipr_instance_and_extension.ipr_instance.table_id_ipv6);

	ipr_instance_and_extension.ipr_instance_extension.confirm_delete_cb(
		ipr_instance_and_extension.ipr_instance_extension.delete_arg);
}


int ipr_reassemble(ipr_instance_handle_t instance_handle)
{
	/* Following struct should be aligned due to ctlu alignment request */
	struct ipr_rfdc rfdc __attribute__((aligned(16)));
	struct ipr_instance instance_params;
	struct scope_status_params scope_status;
	uint64_t rfdc_ext_addr;
	uint32_t status_insert_to_LL;
	uint32_t osm_status;
	uint32_t frame_is_ipv4;
	uint32_t status;
	void	*iphdr_ptr;
	int	 sr_status;
	uint16_t iphdr_offset;
	struct presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;

	iphdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	iphdr_ptr = (void *)(iphdr_offset + PRC_GET_SEGMENT_ADDRESS());

	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	if (scope_status.scope_mode == EXCLUSIVE) {
		if (PARSER_IS_OUTER_IP_FRAGMENT_DEFAULT()) {
			osm_status = BYPASS_OSM;
		} else {
			/* regular frame */
			return IPR_REASSEMBLY_REGULAR;
		}
	} else {
	    /* move to exclusive */
	    osm_scope_transition_to_exclusive_with_increment_scope_id_wrp();
	    if (PARSER_IS_OUTER_IP_FRAGMENT_DEFAULT()) {
		/* Fragment */
#ifdef FDMA_OSM_LIMIT		    
		    if (scope_status.scope_level <= 1) {
#else
		    if (scope_status.scope_level <= 2) {			    
#endif			    
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
			osm_scope_relinquish_exclusivity();
			return IPR_REASSEMBLY_REGULAR;
		}
	}

	/* read instance parameters */
	cdma_read_wrp(&instance_params,
		  instance_handle,
		  IPR_INSTANCE_SIZE);

	if (PARSER_IS_OUTER_IPV4_DEFAULT())
		frame_is_ipv4 = 1;
	else
		frame_is_ipv4 = 0;

	if (check_for_frag_error(&instance_params,frame_is_ipv4, iphdr_ptr) ==
								NO_ERROR) {

		sr_status = ipr_lookup(frame_is_ipv4, &instance_params,
					&rfdc_ext_addr);
		
		if (sr_status == TABLE_STATUS_SUCCESS) {
			/* Hit */
			if (osm_status == NO_BYPASS_OSM) {
				/* create nested per reassembled frame */
				osm_scope_enter_to_exclusive_with_new_scope_id(
						  (uint32_t)rfdc_ext_addr);
			}
			/* read and lock RFDC */
			cdma_read_with_mutex_wrp(rfdc_ext_addr,
					  CDMA_PREDMA_MUTEX_WRITE_LOCK,
					  &rfdc,
					  RFDC_SIZE);

			if (!(rfdc.status & RFDC_VALID)) {
				move_to_correct_ordering_scope2(osm_status);
				/* CDMA write, unlock, dec ref_cnt and release
				 * if ref_cnt=0.
				 * Error is not checked since no error
				 * can't be returned*/
				cdma_access_context_memory_wrp(
				rfdc_ext_addr,
				CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL |
				CDMA_ACCESS_CONTEXT_MEM_RM_BIT,
				NULL,
				&rfdc,
				CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE
				| RFDC_SIZE,
				(uint32_t *)REF_COUNT_ADDR_DUMMY);

				fdma_discard_fd_wrp(
					     (struct ldpaa_fd *)HWC_FD_ADDRESS,
					     FDMA_DIS_NO_FLAGS);
				/* Early Time out */
				return ETIMEDOUT;
			}
		} else if (sr_status == TABLE_STATUS_MISS) {
			sr_status = ipr_miss_handling(&instance_params,
						   frame_is_ipv4,
						   osm_status,
						   &rfdc,
						   instance_handle,
						   &rfdc_ext_addr);
			if (sr_status)
				return sr_status;

		} else {
		/* TLU lookup SR error */
		pr_err("IPR Lookup failed\n");
	}

	if (rfdc.num_of_frags == MAX_NUM_OF_FRAGS) {
		/* Release RFDC, unlock, dec ref_cnt, release if 0 */
		cdma_access_context_memory_wrp(
				  rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL |
				  CDMA_ACCESS_CONTEXT_MEM_RM_BIT,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_NO_MEM_DMA |
				  RFDC_SIZE,
				  (uint32_t *)REF_COUNT_ADDR_DUMMY);
		/* Handle ordering scope */
		move_to_correct_ordering_scope1(osm_status);

		ipr_stats_update(&instance_params,
				 offsetof(struct extended_stats_cntrs,
					  more_than_64_frags_ipv4_cntr),
				 frame_is_ipv4);

		return ENOTSUP;
	}
	status_insert_to_LL = ipr_insert_to_link_list(&rfdc, rfdc_ext_addr,
						      &instance_params,
						      iphdr_ptr, frame_is_ipv4);
	switch (status_insert_to_LL) {
	case FRAG_OK_REASS_NOT_COMPL:
		move_to_correct_ordering_scope2(osm_status);
		if (rfdc.num_of_frags != 1) {
			/* other fragments than the opening one */
			if(frame_is_ipv4) {
				if (!(instance_params.flags &
						IPR_MODE_IPV4_TO_TYPE)) {
				/* recharge timer in case of time out
				 * between fragments */
				/* Delete timer */
					sr_status = tman_delete_timer_wrp(
					  rfdc.timer_handle,
					  TMAN_TIMER_DELETE_MODE_WO_EXPIRATION);
					/* DEBUG : check ENAVAIL */
					if(sr_status == SUCCESS) {
					      sr_status = tman_create_timer_wrp(
					     instance_params.tmi_id,
					     IPR_TIMEOUT_FLAGS,
					     instance_params.timeout_value_ipv4,
					     (tman_arg_8B_t) rfdc_ext_addr,
					     (tman_arg_2B_t) NULL,
					     (tman_cb_t) ipr_time_out,
					     &rfdc.timer_handle);
						if (sr_status != SUCCESS)
					   ipr_exception_handler(IPR_REASSEMBLE,
							 __LINE__,ENOSPC_TIMER);
					}
				}
			} else if (!(instance_params.flags &
					IPR_MODE_IPV6_TO_TYPE)) {
				/* recharge timer in case of time out
				 * between fragments */
				/* Delete timer */
					sr_status = tman_delete_timer_wrp(
							rfdc.timer_handle,
					  TMAN_TIMER_DELETE_MODE_WO_EXPIRATION);
					/* DEBUG : check ENAVAIL */
					if(sr_status == SUCCESS)
						tman_create_timer_wrp(
					     instance_params.tmi_id,
					     IPR_TIMEOUT_FLAGS,
					     instance_params.timeout_value_ipv6,
					     (tman_arg_8B_t) rfdc_ext_addr,
					     (tman_arg_2B_t) NULL,
					     (tman_cb_t) ipr_time_out,
					     &rfdc.timer_handle);
					else
					   ipr_exception_handler(IPR_REASSEMBLE,
							 __LINE__,ENOSPC_TIMER);
			}
		}
		/* Write and release updated 64 first bytes of RFDC */
		cdma_write_release_lock_and_decrement(
				       rfdc_ext_addr,
				       &rfdc,
				       RFDC_SIZE);
		/* Increment no of valid fragments in extended
		 * statistics data structure*/
		ipr_stats_update(&instance_params,
				 offsetof(struct extended_stats_cntrs,
					  valid_frags_cntr_ipv4),
					  frame_is_ipv4);

		return IPR_REASSEMBLY_NOT_COMPLETED;
	case LAST_FRAG_IN_ORDER:
		closing_in_order(rfdc_ext_addr, rfdc.num_of_frags);
		break;
	case LAST_FRAG_OUT_OF_ORDER:
		closing_with_reordering(&rfdc, rfdc_ext_addr);
		break;
	case MALFORMED_FRAG:
		/* duplicate, overlap, or non-conform fragment */
		/* Write and release updated 64 first bytes of RFDC */
		/* RFDC is written for the case the opening frag is malformed */
		cdma_write_release_lock_and_decrement(
				       rfdc_ext_addr,
				       &rfdc,
				       RFDC_SIZE);

		/* Increment no of malformed frames in extended
		 * statistics data structure*/
		ipr_stats_update(&instance_params,
				 offsetof(struct extended_stats_cntrs,
					  malformed_frags_cntr_ipv4),
				 frame_is_ipv4);


		return IPR_MALFORMED_FRAG;
		break;
	}
	/* Only successfully reassembled frames continue
	   from here */
	/* Currently no default frame */

	/* Reset Valid bit of RFDC */
	rfdc.status = rfdc.status & ~RFDC_VALID;
	
	/* Increment no of valid fragments in extended statistics
	 * data structure*/
	ipr_stats_update(&instance_params,
			 offsetof(struct extended_stats_cntrs,
				  valid_frags_cntr_ipv4),
			 frame_is_ipv4);

	/* Delete timer */
	sr_status = tman_delete_timer_wrp(rfdc.timer_handle,
			  	  	 TMAN_TIMER_DELETE_MODE_WO_EXPIRATION);
	/* DEBUG : check ENAVAIL */
	if(sr_status != SUCCESS) {
		/* Write and release updated 64 first bytes of RFDC,
		 * unlock, dec ref_cnt, release if 0 */
		cdma_access_context_memory_wrp(
				  rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL |
				  CDMA_ACCESS_CONTEXT_MEM_RM_BIT,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE |
				  RFDC_SIZE,
				  (uint32_t *)REF_COUNT_ADDR_DUMMY);

		move_to_correct_ordering_scope2(osm_status);
		return ETIMEDOUT;
	}

	if (frame_is_ipv4) {
		table_rule_delete_wrp(TABLE_ACCEL_ID_CTLU,
				  instance_params.table_id_ipv4,
				  (union table_key_desc *)&rfdc.ipv4_key,
				  IPV4_KEY_SIZE,
				  NULL);
		/* DEBUG : check EIO */
	} else {
		ipv6_rule_delete(rfdc_ext_addr,&instance_params);
	}

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
	if (frame_is_ipv4)
		status = ipv4_header_update_and_l4_validation(&rfdc);
	else
		status = ipv6_header_update_and_l4_validation(&rfdc);

	/* Write and release updated 64 first bytes of RFDC */
	/* CDMA write, unlock, dec ref_cnt and release if
	 * ref_cnt=0 */
	cdma_access_context_memory_wrp(
				  rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL |
				  CDMA_ACCESS_CONTEXT_MEM_RM_BIT,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE |
				  RFDC_SIZE,
				  (uint32_t *)REF_COUNT_ADDR_DUMMY);
	
	/* Decrement ref count of acquire */
	cdma_refcount_decrement_and_release(rfdc_ext_addr);

	move_to_correct_ordering_scope2(osm_status);

	if (frame_is_ipv4) {
		/* Decrement no of IPv4 open frames in instance data structure*/
		ste_dec_counter_wrp(
				 instance_handle + sizeof(struct ipr_instance)+
					offsetof(struct ipr_instance_extension,
					num_of_open_reass_frames_ipv4),
				1,
				STE_MODE_32_BIT_CNTR_SIZE);
		if (status == SUCCESS) {
			/* L4 checksum is valid */
			/* Increment no of frames in instance data structure */
			ste_inc_counter_wrp(instance_handle +
					sizeof(struct ipr_instance)+
					offsetof(struct ipr_instance_extension,
					ipv4_reass_frm_cntr),
					1,
					STE_MODE_32_BIT_CNTR_SIZE);
			return IPR_REASSEMBLY_SUCCESS;
		}
	} else { /* IPv6 */
		/* Decrement no of IPv6 open frames in instance data structure*/
		ste_dec_counter_wrp(
				instance_handle + sizeof(struct ipr_instance)+
					offsetof(struct ipr_instance_extension,
					num_of_open_reass_frames_ipv6),
				1,
				STE_MODE_32_BIT_CNTR_SIZE);
		if (status == SUCCESS) {
			ste_inc_counter_wrp(instance_handle +
					sizeof(struct ipr_instance)+
					offsetof(struct ipr_instance_extension,
					ipv6_reass_frm_cntr),
					1,
					STE_MODE_32_BIT_CNTR_SIZE);

			return IPR_REASSEMBLY_SUCCESS;
		}
	}

	/* L4 checksum is not valid */
	return EIO;

	} else {
		/* Error fragment */
		move_to_correct_ordering_scope1(osm_status);
		/* Increment no of malformed frames in extended
		 * statistics data structure*/
		ipr_stats_update(&instance_params,
				 offsetof(struct extended_stats_cntrs,
					  malformed_frags_cntr_ipv4),
				 frame_is_ipv4);

		return IPR_MALFORMED_FRAG;
	}
}

void ipv6_rule_delete(uint64_t rfdc_ext_addr,
		      struct ipr_instance *instance_params_ptr)
{
	/* Following array should be aligned due to ctlu alignment request */
	uint8_t  ipv6_key[36] __attribute__((aligned(16)));

	cdma_read_wrp(&ipv6_key, rfdc_ext_addr+RFDC_SIZE+
			  	  offsetof(struct extended_ipr_rfdc,ipv6_key),
		      IPV6_KEY_SIZE);
	table_rule_delete_wrp(TABLE_ACCEL_ID_CTLU,
			  instance_params_ptr->table_id_ipv6,
			  (union table_key_desc *)&ipv6_key,
			  IPV6_KEY_SIZE,
			  NULL);
	/* DEBUG : check EIO */
}
int ipr_lookup(uint32_t frame_is_ipv4, struct ipr_instance *instance_params_ptr,
		uint64_t *rfdc_ext_addr_ptr)
{
	/* Following struct should be aligned due to ctlu alignment request */
	struct table_lookup_result lookup_result __attribute__((aligned(16)));
	int sr_status;
	
	/* Good fragment */
	if (frame_is_ipv4) {
		/* Error is not checked since it is assumed that
		 * IP header exists and is presented */
		sr_status = table_lookup_by_keyid_default_frame_wrp(
				TABLE_ACCEL_ID_CTLU,
				instance_params_ptr->table_id_ipv4,
				ipr_global_parameters1.ipr_key_id_ipv4,
				&lookup_result);
	} else {
		/* Error is not checked since it is assumed that
		 * IP header exists and is presented */
		sr_status = table_lookup_by_keyid_default_frame_wrp(
				TABLE_ACCEL_ID_CTLU,
				instance_params_ptr->table_id_ipv6,
				ipr_global_parameters1.ipr_key_id_ipv6,
				&lookup_result);
	}
	/* Next line is relevant only in case of Hit */
	*rfdc_ext_addr_ptr = lookup_result.opaque0_or_reference;
	
	return sr_status;
}

int ipr_miss_handling(struct ipr_instance *instance_params_ptr,
	 uint32_t frame_is_ipv4, uint32_t osm_status, struct ipr_rfdc *rfdc_ptr,
	 ipr_instance_handle_t instance_handle, uint64_t *rfdc_ext_addr_ptr)
{
	struct table_rule rule __attribute__((aligned(16)));
	int sr_status;
	uint16_t timeout_value;
	uint8_t	 keysize;
#ifdef REV2_RULEID
	uint64_t rule_id;
#endif
	
	/* Miss */
	sr_status = cdma_acquire_context_memory(
					instance_params_ptr->bpid,
					rfdc_ext_addr_ptr);
	if(sr_status)
		ipr_exception_handler(IPR_REASSEMBLE,__LINE__,
				      sr_status);
	/* Lock RFDC + increment reference count*/
	cdma_access_context_memory_wrp(
		*rfdc_ext_addr_ptr,
		CDMA_ACCESS_CONTEXT_MEM_MUTEX_WRITE_LOCK |
		CDMA_ACCESS_CONTEXT_MEM_INC_REFCOUNT,
		0,
		(void *)0,
		0,
		(uint32_t *)REF_COUNT_ADDR_DUMMY);
	
	/* Reset RFDC + Link List */
	/*cdma_ws_memory_init((void *)&rfdc,
			SIZE_TO_INIT,
			0);  */
	/* Add entry to TLU table */
	/* Generate key */
	rule.options = 0;
	rule.result.type = TABLE_RESULT_TYPE_REFERENCE;
	rule.result.op0_rptr_clp.reference_pointer = *rfdc_ext_addr_ptr;
	
	if (frame_is_ipv4) {
		/* Error is not checked since it is assumed that
		 * IP header exists and is presented */
		keygen_gen_key_wrp(
			KEYGEN_ACCEL_ID_CTLU,
			ipr_global_parameters1.ipr_key_id_ipv4,
			0,
			&rule.key_desc,
			&keysize);
		sr_status = table_rule_create_wrp(
				TABLE_ACCEL_ID_CTLU,
				instance_params_ptr->table_id_ipv4,
				&rule,
#ifdef REV2_RULEID
				keysize,&rule_id);
#else
				keysize);
#endif
		if (sr_status == -ENOMEM) {
			/* Maximum open reassembly is reached */
			ipr_stats_update(
			   instance_params_ptr,
			   offsetof(struct extended_stats_cntrs,
			   open_reass_frms_exceed_ipv4_cntr),
			   frame_is_ipv4
			   );
		    /* Release acquired buffer */
		    cdma_release_context_memory(*rfdc_ext_addr_ptr);
		    /* Handle ordering scope */
		    move_to_correct_ordering_scope1(osm_status);
		    return ENOSPC;
		}
		/* store key in RDFC */
		rfdc_ptr->ipv4_key[0] =
			      *(uint64_t *)rule.key_desc.em.key;
		rfdc_ptr->ipv4_key[1] =
			  *(uint64_t *)(rule.key_desc.em.key+8);
	
		/* Increment no of IPv4 open frames in instance
			data structure */
		ste_inc_counter_wrp(
			 instance_handle+
			 sizeof(struct ipr_instance)+
			 offsetof(struct ipr_instance_extension,
			 num_of_open_reass_frames_ipv4),
			 1,
			 STE_MODE_32_BIT_CNTR_SIZE);
	
		rfdc_ptr->status = RFDC_VALID | IPV4_FRAME;
	
	} else {
	    /* IPv6 */
	    /* Error is not checked since it is assumed that
	     * IP header exists and is presented */
	    keygen_gen_key_wrp(
			 KEYGEN_ACCEL_ID_CTLU,
			 ipr_global_parameters1.ipr_key_id_ipv6,
			 0,
			 &rule.key_desc,
			 &keysize);
	    sr_status = table_rule_create_wrp(
			      TABLE_ACCEL_ID_CTLU,
			      instance_params_ptr->table_id_ipv6,
			      &rule,
#ifdef REV2_RULEID
				keysize,&rule_id);
#else
				keysize);
#endif
	    if (sr_status == -ENOMEM) {
		    /* Maximum open reassembly is reached */
		    ipr_stats_update(
			   instance_params_ptr,
			   offsetof(struct extended_stats_cntrs,
			   open_reass_frms_exceed_ipv4_cntr),
			   frame_is_ipv4);
	    /* Release acquired buffer */
	    cdma_release_context_memory(*rfdc_ext_addr_ptr);
	    /* Handle ordering scope */
	    move_to_correct_ordering_scope1(osm_status);
	    return ENOSPC;
	}
	
	    /* write key in RDFC Extension */
	    cdma_write_wrp(*rfdc_ext_addr_ptr+RFDC_SIZE+
			    offsetof(struct extended_ipr_rfdc,
			    ipv6_key),
			&rule.key_desc.em.key,
			RFDC_EXTENSION_TRUNCATED_SIZE);
	
	    /* Increment no of IPv6 open frames in instance
	       data structure */
	     ste_inc_counter_wrp(
			 instance_handle+
			 sizeof(struct ipr_instance)+
			 offsetof(struct ipr_instance_extension,
			 num_of_open_reass_frames_ipv6),
			 1,
			 STE_MODE_32_BIT_CNTR_SIZE);
	
		rfdc_ptr->status = RFDC_VALID | IPV6_FRAME;
	
	}
	
	/* todo release struct rule  or call function for
	* gen+add rule */
	rfdc_ptr->instance_handle		= instance_handle;
	rfdc_ptr->expected_total_length	= 0;
	rfdc_ptr->index_to_out_of_order	= 0;
	rfdc_ptr->next_index			= 0;
	rfdc_ptr->current_total_length	= 0;
	rfdc_ptr->first_frag_index		= 0;
	rfdc_ptr->num_of_frags		= 0;
	/* todo check if necessary */
	rfdc_ptr->biggest_payload		= 0;
	rfdc_ptr->current_running_sum	= 0;
	rfdc_ptr->last_frag_index		= 0;
	rfdc_ptr->total_in_order_payload	= 0;
	//			get_default_amq_attributes(&rfdc.isolation_bits);
	rfdc_ptr->niid = dpni_get_receive_niid();
	
	/* create Timer in TMAN */
	
	if(frame_is_ipv4)
	    timeout_value = instance_params_ptr->timeout_value_ipv4;
	else
	    timeout_value = instance_params_ptr->timeout_value_ipv6;
	
	sr_status = tman_create_timer_wrp(instance_params_ptr->tmi_id,
				  IPR_TIMEOUT_FLAGS,
				  timeout_value,
				  (tman_arg_8B_t) *rfdc_ext_addr_ptr,
				  (tman_arg_2B_t) NULL,
				  (tman_cb_t) ipr_time_out,
				  &rfdc_ptr->timer_handle);
	if (sr_status)
	    ipr_exception_handler(IPR_REASSEMBLE, __LINE__,
				  ENOSPC_TIMER);
	
	if (osm_status == NO_BYPASS_OSM) {
	/* create nested per reassembled frame */
	osm_scope_enter_to_exclusive_with_new_scope_id(
						  (uint32_t)*rfdc_ext_addr_ptr);
	}
	return SUCCESS;
	
}
uint32_t ipr_insert_to_link_list(struct ipr_rfdc *rfdc_ptr,
				 uint64_t rfdc_ext_addr,
				 struct ipr_instance *instance_params_ptr,
				 void *iphdr_ptr,
				 uint32_t frame_is_ipv4)
{

	uint8_t			current_index;
	uint16_t		frag_offset_shifted;
	uint16_t		current_frag_size;
	uint16_t		expected_frag_offset;
	uint16_t		ip_header_size;
	uint16_t		ipv6fraghdr_offset;
	uint16_t		current_running_sum;
	uint32_t		last_fragment;
	uint32_t		return_status;
	uint64_t		ext_addr;
	/* todo reuse ext_addr for current_element_ext_addr */
	uint64_t		current_element_ext_addr;
	struct link_list_element	current_element;
	struct ipv4hdr			*ipv4hdr_ptr;
	struct ipv6hdr			*ipv6hdr_ptr;
	struct ipv6fraghdr		*ipv6fraghdr_ptr;
	struct	parse_result	*pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct	presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;

	if (frame_is_ipv4) {
		ipv4hdr_ptr = (struct ipv4hdr *) iphdr_ptr;
		frag_offset_shifted =
		    (ipv4hdr_ptr->flags_and_offset & FRAG_OFFSET_IPV4_MASK)<<3;

		ip_header_size = (uint16_t)
			  ((ipv4hdr_ptr->vsn_and_ihl & IPV4_HDR_IHL_MASK)<<2);
		current_frag_size = ipv4hdr_ptr->total_length - ip_header_size;
		last_fragment = !(ipv4hdr_ptr->flags_and_offset &
				IPV4_HDR_M_FLAG_MASK);
		/* Check ECN compliance */
		if ((ipv4hdr_ptr->tos & IPV4_ECN) == NOT_ECT) {
			if (rfdc_ptr->status & RFDC_STATUS_CE)
				return MALFORMED_FRAG;
			rfdc_ptr->status |= RFDC_STATUS_NOT_ECT;
		}
		else if ((ipv4hdr_ptr->tos & IPV4_ECN) == CE) {
			if (rfdc_ptr->status & RFDC_STATUS_NOT_ECT)
				return MALFORMED_FRAG;
			rfdc_ptr->status |= RFDC_STATUS_CE;
		}
		check_remove_padding();
	} else {
		ipv6hdr_ptr = (struct ipv6hdr *) iphdr_ptr;
		ipv6fraghdr_offset =
				PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
		ipv6fraghdr_ptr = (struct ipv6fraghdr *)
			       (PRC_GET_SEGMENT_ADDRESS() + ipv6fraghdr_offset);
		frag_offset_shifted = ipv6fraghdr_ptr->offset_and_flags &
							  FRAG_OFFSET_IPV6_MASK;

		ip_header_size = ((uint16_t)((uint32_t)ipv6fraghdr_ptr -
						(uint32_t)ipv6hdr_ptr)) + 8;
		current_frag_size = ipv6hdr_ptr->payload_length -
				ip_header_size + IPV6_FIXED_HEADER_SIZE;
		last_fragment = !(ipv6fraghdr_ptr->offset_and_flags &
				IPV6_HDR_M_FLAG_MASK);
		/* Check ECN compliance */
		if ((ipv6hdr_ptr->vsn_traffic_flow & IPV6_ECN)
				== NOT_ECT) {
			if (rfdc_ptr->status & RFDC_STATUS_CE)
				return MALFORMED_FRAG;
			rfdc_ptr->status |= RFDC_STATUS_NOT_ECT;
		}
		else if ((ipv6hdr_ptr->vsn_traffic_flow & IPV6_ECN)
				== IPV6_ECN) {
			/* CE code */
			if (rfdc_ptr->status & RFDC_STATUS_NOT_ECT)
				return MALFORMED_FRAG;
			rfdc_ptr->status |= RFDC_STATUS_CE;
		}
	}

	if (frag_offset_shifted != 0) {
		/* Not first frag */
		/* Save header to be removed in FD[FRC] */
		((struct ldpaa_fd *)HWC_FD_ADDRESS)->frc =
			    (uint32_t) (PARSER_GET_OUTER_IP_OFFSET_DEFAULT() +
					    ip_header_size);

		/* Add current frag's running sum for L4 checksum check */
		if (pr->gross_running_sum == 0) {
			/* current_running_sum is used as a temporary location
			 * for stack optimization*/
			fdma_calculate_default_frame_checksum_wrp(
				0,
				0xffff,	
				&current_running_sum);
			
			pr->gross_running_sum = current_running_sum;
			/* Run parser in order to get valid running sum */
			parse_result_generate_default(0);
		}

		current_running_sum = cksum_ones_complement_sum16(
						  rfdc_ptr->current_running_sum,
						  pr->running_sum);
	} else {
		rfdc_ptr->first_frag_hdr_length = ip_header_size;
		if (pr->gross_running_sum == 0) {
			/* current_running_sum is used as a temporary location
			 * for stack optimization*/
			fdma_calculate_default_frame_checksum_wrp(
				0,
				0xffff,
				&current_running_sum);
			pr->gross_running_sum = current_running_sum;
		}
		/* Set 1rst frag's running sum for L4 checksum check */
		current_running_sum = cksum_ones_complement_sum16(
				  	          rfdc_ptr->current_running_sum,
				  	          pr->gross_running_sum);
	}

	if (!(rfdc_ptr->status & OUT_OF_ORDER)) {
		/* In order handling */
		expected_frag_offset = rfdc_ptr->current_total_length;
		if (frag_offset_shifted == expected_frag_offset) {

			rfdc_ptr->num_of_frags++;
			rfdc_ptr->current_total_length += current_frag_size;

			if (last_fragment) {
				if((rfdc_ptr->current_total_length +
				   rfdc_ptr->first_frag_hdr_length) <=
				   instance_params_ptr->max_reass_frm_size) {
					/* Close current frame before storing FD */
					fdma_store_default_frame_data();

					/* Write FD in external buffer */
					ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
								rfdc_ptr->next_index*FD_SIZE;
					cdma_write(ext_addr,
						   (void *)HWC_FD_ADDRESS,
						   FD_SIZE);

					return_status = LAST_FRAG_IN_ORDER;
				}
				else
					return_status = MALFORMED_FRAG;
			} else {
				/* Non closing fragment */
				/* Close current frame before storing FD */
				fdma_store_default_frame_data();

				/* Write FD in external buffer */
				ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
							rfdc_ptr->next_index*FD_SIZE;
				cdma_write(ext_addr,
					   (void *)HWC_FD_ADDRESS,
					   FD_SIZE);

				rfdc_ptr->next_index++;
				return_status =  FRAG_OK_REASS_NOT_COMPL;
				}
		} else if (frag_offset_shifted < expected_frag_offset) {
				/* Malformed Error */
				return MALFORMED_FRAG;
		} else {
			/* New out of order */
			current_index = rfdc_ptr->next_index;
			if (current_index != 0) {
				rfdc_ptr->status |= OUT_OF_ORDER |
						  ORDER_AND_OOO;
				rfdc_ptr->index_to_out_of_order = current_index;
				rfdc_ptr->first_frag_index = current_index;
				rfdc_ptr->total_in_order_payload =
						rfdc_ptr->current_total_length;
		} else {
			rfdc_ptr->status |= OUT_OF_ORDER;
		}
		rfdc_ptr->last_frag_index  = current_index;
		rfdc_ptr->num_of_frags++;
		rfdc_ptr->current_total_length +=
					    current_frag_size;
		rfdc_ptr->next_index = current_index + 1;
		rfdc_ptr->biggest_payload = frag_offset_shifted +
					 current_frag_size;

		if (last_fragment)
			rfdc_ptr->expected_total_length = frag_offset_shifted +
							  current_frag_size;
		current_element.next_index  = 0;
		current_element.prev_index  = 0;
		current_element.frag_offset = frag_offset_shifted;
		current_element.frag_length = current_frag_size;
		/* Write my element of link list */
		current_element_ext_addr = rfdc_ext_addr +
			START_OF_LINK_LIST +
			current_index*LINK_LIST_ELEMENT_SIZE;
		cdma_write(current_element_ext_addr,
				   (void *)&current_element,
				   LINK_LIST_ELEMENT_SIZE);

		/* Close current frame before storing FD */
		fdma_store_default_frame_data();

		/* Write FD in external buffer */
		ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
			   current_index*FD_SIZE;
		cdma_write(ext_addr,
			       (void *)HWC_FD_ADDRESS,
			       FD_SIZE);

		return_status = FRAG_OK_REASS_NOT_COMPL;
	    }
		} else {
			/* Out of order handling */
			return_status = out_of_order(rfdc_ptr, rfdc_ext_addr,
					last_fragment, current_frag_size,
					frag_offset_shifted,
					instance_params_ptr);
			if(return_status == MALFORMED_FRAG)
				return MALFORMED_FRAG;
	}
	/* Only valid fragment runs here */
	if (frag_offset_shifted == 0) {
		/* First fragment (frag_offset == 0) */
		rfdc_ptr->status |= FIRST_ARRIVED;
		/* Save PRC params for presentation of the reassembled frame */
		rfdc_ptr->seg_addr   = prc->seg_address;
		rfdc_ptr->seg_length = prc->seg_length;
		rfdc_ptr->seg_offset = prc->seg_offset;

		/* Get IP offset */
		rfdc_ptr->iphdr_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		if (!frame_is_ipv4)
			rfdc_ptr->ipv6_fraghdr_offset = ipv6fraghdr_offset;

	}
	rfdc_ptr->current_running_sum = current_running_sum;

	return return_status;
}

uint32_t closing_in_order(uint64_t rfdc_ext_addr, uint8_t num_of_frags)
{
	struct		ldpaa_fd fds_to_concatenate[2] \
			     __attribute__((aligned(sizeof(struct ldpaa_fd))));
	uint64_t	fds_to_fetch_addr;
	struct		fdma_concatenate_frames_params concatenate_params;

	/* Bring into workspace 2 FDs to be concatenated */
	fds_to_fetch_addr = rfdc_ext_addr + START_OF_FDS_LIST;
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

uint32_t ipv4_header_update_and_l4_validation(struct ipr_rfdc *rfdc_ptr)
{
	uint8_t		new_tos;
	uint16_t	ipv4hdr_offset;
	uint16_t	new_total_length;
	uint16_t	ip_hdr_cksum;
	uint16_t	old_ip_checksum;
	uint16_t	new_flags_and_offset;
	uint16_t	ip_header_size;
	struct ipv4hdr  *ipv4hdr_ptr;
	struct parse_result *pr;
	struct	presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;

	ipv4hdr_offset = rfdc_ptr->iphdr_offset;
	ipv4hdr_ptr = (struct ipv4hdr *)
			(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());

	/* update IP checksum */
	ip_header_size = (uint16_t)
			((ipv4hdr_ptr->vsn_and_ihl & IPV4_HDR_IHL_MASK)<<2);
	new_total_length = rfdc_ptr->current_total_length + ip_header_size;
	old_ip_checksum = ipv4hdr_ptr->hdr_cksum;
	ip_hdr_cksum = old_ip_checksum;
	ip_hdr_cksum = cksum_accumulative_update_uint32(ip_hdr_cksum,
					 ipv4hdr_ptr->total_length,
					 new_total_length);
	ipv4hdr_ptr->total_length = new_total_length;

	/* Set CE code in ECN field if required */
	if ((rfdc_ptr->status & RFDC_STATUS_CE) &&
			((ipv4hdr_ptr->tos & IPV4_ECN) != CE)) {
		new_tos = ipv4hdr_ptr->tos | CE;
		ip_hdr_cksum = cksum_accumulative_update_uint32(
					 ip_hdr_cksum,
					 ipv4hdr_ptr->tos,
					 new_tos);
		ipv4hdr_ptr->tos = new_tos;
	}		
	
	new_flags_and_offset = ipv4hdr_ptr->flags_and_offset & RESET_MF_BIT;
	ip_hdr_cksum = cksum_accumulative_update_uint32(
				 ip_hdr_cksum,
				 ipv4hdr_ptr->flags_and_offset,
				 new_flags_and_offset);
	ipv4hdr_ptr->flags_and_offset = new_flags_and_offset;

	ipv4hdr_ptr->hdr_cksum = ip_hdr_cksum;

	/* update FDMA with tos, total length and IP header checksum*/
	fdma_modify_default_segment_data(ipv4hdr_offset, 12);

	/* Updated FD[length] */
	LDPAA_FD_SET_LENGTH(HWC_FD_ADDRESS, new_total_length + ipv4hdr_offset+
					   prc->seg_offset);

	/* Update Gross running sum of the reassembled frame */
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	pr->gross_running_sum = rfdc_ptr->current_running_sum;

	/* Run Parser and check L4 checksum if needed */
	if (parse_result_generate_default(PARSER_VALIDATE_L4_CHECKSUM)){
		/* error in L4 checksum */
		return EIO;
	}
	return SUCCESS;
}

uint32_t ipv6_header_update_and_l4_validation(struct ipr_rfdc *rfdc_ptr)
{
	uint16_t		ipv6hdr_offset;
	uint16_t		ipv6fraghdr_offset;
	uint16_t		ipv6_frag_extension_size;
	uint16_t		checksum;
	uint16_t		gross_running_sum;
	uint16_t		size;
	uint16_t		payload_length;
	struct ipv6hdr		*ipv6hdr_ptr;
	struct ipv6fraghdr	*ipv6fraghdr_ptr;
	struct	parse_result 	*pr =
				   (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct	presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;

	ipv6hdr_offset = rfdc_ptr->iphdr_offset;
	ipv6hdr_ptr = (struct ipv6hdr *)
				(ipv6hdr_offset + PRC_GET_SEGMENT_ADDRESS());

	ipv6fraghdr_offset = rfdc_ptr->ipv6_fraghdr_offset;

	ipv6fraghdr_ptr = (struct ipv6fraghdr *) (PRC_GET_SEGMENT_ADDRESS() +
						ipv6fraghdr_offset);

	ipv6_frag_extension_size = (uint16_t)((uint32_t)ipv6fraghdr_ptr -
				(uint32_t)ipv6hdr_ptr) - IPV6_FIXED_HEADER_SIZE;

	fdma_calculate_default_frame_checksum_wrp(
				ipv6hdr_offset,
				ipv6fraghdr_offset-ipv6hdr_offset+8,
				&checksum);

	/* payload length doesn't include nor any frag extension header neither the fixed IPv6 header */
	payload_length = rfdc_ptr->current_total_length;
	
	ipv6hdr_ptr->payload_length = payload_length + ipv6_frag_extension_size;
	/* Set CE code in ECN field if required, even if it was already CE */
	if (rfdc_ptr->status & RFDC_STATUS_CE)
		ipv6hdr_ptr->vsn_traffic_flow |= IPV6_ECN;

	/* Move next header of fragment header to previous extension header */
	*(uint8_t *)(ipv6_last_header(ipv6hdr_ptr, LAST_HEADER_BEFORE_FRAG) &
			0x7FFFFFFF) = ipv6fraghdr_ptr->next_header;

	/* Remove fragment header extension and update FDMA cache */
	size = ipv6fraghdr_offset-ipv6hdr_offset;

	fdma_replace_default_segment_data(ipv6hdr_offset,
					  size+8,
					  (void *)ipv6hdr_ptr,
					  size,
					  (void *)prc->seg_address,
					  prc->seg_length,
					  FDMA_REPLACE_SA_REPRESENT_BIT);

	/* Updated FD[length] */
	/* pay attention that if seg_offset != 0 , parser may be unable to calculate checksum */
	LDPAA_FD_SET_LENGTH(HWC_FD_ADDRESS, payload_length +
					    ipv6fraghdr_offset +
					    prc->seg_offset);

	/* Update Gross running sum of the reassembled frame */
	/* Subtract old IPv6 header (fixed until frag header) */
	gross_running_sum = cksum_ones_complement_sum16(
					rfdc_ptr->current_running_sum,
					(uint16_t)~checksum);

	/* Calculate checksum of new IPv6 header */
	fdma_calculate_default_frame_checksum_wrp(
			ipv6hdr_offset,
			size,
			&checksum);

	/* Add new fields */
	gross_running_sum = cksum_ones_complement_sum16(gross_running_sum,
							checksum);

	pr->gross_running_sum = gross_running_sum;

	if(parse_result_generate_default(PARSER_VALIDATE_L4_CHECKSUM)) {
			/* error in L4 checksum */
			return EIO;
		}
	return SUCCESS;
}


uint32_t closing_with_reordering(struct ipr_rfdc *rfdc_ptr,
				 uint64_t rfdc_ext_addr)
{
	uint8_t				num_of_frags;
	uint8_t				current_index;
	uint8_t				octet_index;
	uint64_t			temp_ext_addr;
	struct link_list_element	link_list[8];
	struct				ldpaa_fd fds_to_concatenate[2] \
			      __attribute__((aligned(sizeof(struct ldpaa_fd))));
	struct		fdma_concatenate_frames_params concatenate_params;

	concatenate_params.flags = FDMA_CONCAT_SF_BIT;

	if (rfdc_ptr->status & ORDER_AND_OOO) {
		if (rfdc_ptr->index_to_out_of_order == 1) {
			temp_ext_addr = rfdc_ext_addr + START_OF_FDS_LIST;
			cdma_read((void *)HWC_FD_ADDRESS,
					  temp_ext_addr,
					  FD_SIZE);
			/* Copy 1rst FD to default frame FD's place */
			/* *((struct ldpaa_fd *)(HWC_FD_ADDRESS)) =
			 * fds_to_concatenate[0];*/

			/* Open 1rst frame and get frame handle */
			fdma_present_default_frame_without_segments();
			concatenate_params.frame1 =
					    (uint16_t) PRC_GET_FRAME_HANDLE();
			current_index = rfdc_ptr->first_frag_index;
			temp_ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
							current_index*FD_SIZE;

			cdma_read((void *)fds_to_concatenate,
					  temp_ext_addr,
					  FD_SIZE);
			/* Open frame and get frame handle */
			/* reset frame2 field because handle is 2 bytes in
			 * concatenate vs 1 byte in present*/
			concatenate_params.frame2 = 0;

			fdma_present_frame_without_segments(
				    fds_to_concatenate,
				    FDMA_INIT_NO_FLAGS,
				    0,
				    (uint8_t *)(&(concatenate_params.frame2)) +
				    sizeof(uint8_t));

			/* Take header size to be removed from FD[FRC] */
			concatenate_params.trim  =
					(uint8_t)fds_to_concatenate[0].frc;
			fdma_concatenate_frames(&concatenate_params);

			num_of_frags = rfdc_ptr->num_of_frags - 2;
			octet_index = 255; /* invalid value */

		} else {
		current_index = rfdc_ptr->index_to_out_of_order;
		closing_in_order(rfdc_ext_addr, current_index);
		num_of_frags = rfdc_ptr->num_of_frags - current_index - 1;
		current_index = rfdc_ptr->first_frag_index;

		temp_ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
						current_index*FD_SIZE;

		cdma_read((void *)fds_to_concatenate,
				  temp_ext_addr,
				  FD_SIZE);
		/* Open frame and get frame handle */
		/* reset frame2 field because handle is 2 bytes in concatenate
		   vs 1 byte in present*/
		concatenate_params.frame2 = 0;

		concatenate_params.frame1 = (uint16_t) PRC_GET_FRAME_HANDLE();

		fdma_present_frame_without_segments(
				fds_to_concatenate,
				FDMA_INIT_NO_FLAGS,
				0,
				(uint8_t *)(&(concatenate_params.frame2)) +
				sizeof(uint8_t));

		/* Take header size to be removed from FD[FRC] */
		concatenate_params.trim  = (uint8_t)fds_to_concatenate[0].frc;
		fdma_concatenate_frames(&concatenate_params);
		octet_index = 255; /* invalid value */
		}
	} else {
		num_of_frags  = rfdc_ptr->num_of_frags;
		current_index = rfdc_ptr->first_frag_index;

		/* Bring 8 elements of LL */
		octet_index = current_index >> 3;
		temp_ext_addr = rfdc_ext_addr + START_OF_LINK_LIST +
				octet_index * 8 * LINK_LIST_ELEMENT_SIZE;
		cdma_read(link_list,
			  temp_ext_addr,
			  8*LINK_LIST_ELEMENT_SIZE);
		temp_ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
						current_index*FD_SIZE;
		cdma_read(fds_to_concatenate,
				  temp_ext_addr,
				  FD_SIZE);
		current_index =
		      link_list[current_index&OCTET_LINK_LIST_MASK].next_index;
		temp_ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
						current_index*FD_SIZE;
		cdma_read(fds_to_concatenate+1,
				  temp_ext_addr,
				  FD_SIZE);
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
			   (uint8_t *)(&(concatenate_params.frame2)) +
			   sizeof(uint8_t));

		concatenate_params.spid   = *((uint8_t *) HWC_SPID_ADDRESS);
		concatenate_params.frame1 = (uint16_t) PRC_GET_FRAME_HANDLE();
		/* Take header size to be removed from 2nd FD[FRC] */
		concatenate_params.trim   = (uint8_t)fds_to_concatenate[1].frc;

		fdma_concatenate_frames(&concatenate_params);

		num_of_frags -= 2;
	}
	while (num_of_frags != 0) {
		if ((current_index >> 3) == octet_index)
			current_index =
		       link_list[current_index&OCTET_LINK_LIST_MASK].next_index;
		else {
			/* Bring 8 elements of LL */
			octet_index = current_index >> 3;
			temp_ext_addr = rfdc_ext_addr + START_OF_LINK_LIST +
					octet_index * 8 *
					LINK_LIST_ELEMENT_SIZE;
			cdma_read(link_list,
				  temp_ext_addr,
				  8*LINK_LIST_ELEMENT_SIZE);

			current_index =
		       link_list[current_index&OCTET_LINK_LIST_MASK].next_index;
		}

		temp_ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
						current_index*FD_SIZE;

		cdma_read((void *)fds_to_concatenate,
				  temp_ext_addr,
				  FD_SIZE);
		/* Open frame and get frame handle */
		/* reset frame2 field because handle is 2 bytes in concatenate
		   vs 1 byte in present*/
		/* todo move this reset to be done once and not each
		 * iteration */
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

		num_of_frags -= 1;
	}

	return SUCCESS;
}

uint32_t check_for_frag_error (struct ipr_instance *instance_params,
				uint32_t frame_is_ipv4, void *iphdr_ptr)
{
	uint16_t length, ip_header_size, current_frag_size, ipv6fraghdr_offset;
	uint16_t frag_offset_shifted;
	uint32_t last_fragment;
	struct ipv4hdr *ipv4hdr_ptr;
	struct ipv6hdr *ipv6hdr_ptr;
	struct ipv6fraghdr *ipv6fraghdr_ptr;


	length = (uint16_t) (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
	if (frame_is_ipv4) {
		if (length < instance_params->min_frag_size_ipv4)
			return MALFORMED_FRAG;
		ipv4hdr_ptr = (struct ipv4hdr *) iphdr_ptr;
		frag_offset_shifted =
		    (ipv4hdr_ptr->flags_and_offset & FRAG_OFFSET_IPV4_MASK)<<3;
		ip_header_size = (uint16_t)
			  ((ipv4hdr_ptr->vsn_and_ihl & IPV4_HDR_IHL_MASK)<<2);
		current_frag_size = ipv4hdr_ptr->total_length - ip_header_size;
		last_fragment = !(ipv4hdr_ptr->flags_and_offset &
				IPV4_HDR_M_FLAG_MASK);
	} else {
		if (length < instance_params->min_frag_size_ipv6)
			return MALFORMED_FRAG;
		ipv6hdr_ptr = (struct ipv6hdr *) iphdr_ptr;
		ipv6fraghdr_offset =
				PARSER_GET_IPV6_FRAG_HEADER_OFFSET_DEFAULT();
		ipv6fraghdr_ptr = (struct ipv6fraghdr *)
			       (PRC_GET_SEGMENT_ADDRESS() + ipv6fraghdr_offset);
		frag_offset_shifted = ipv6fraghdr_ptr->offset_and_flags &
							  FRAG_OFFSET_IPV6_MASK;
		ip_header_size = ((uint16_t)((uint32_t)ipv6fraghdr_ptr -
						(uint32_t)ipv6hdr_ptr)) + 8;
		current_frag_size = ipv6hdr_ptr->payload_length -
				ip_header_size + IPV6_FIXED_HEADER_SIZE;
		last_fragment = !(ipv6fraghdr_ptr->offset_and_flags &
				IPV6_HDR_M_FLAG_MASK);

	}
	/* Check IP size is multiple of 8 for First or middle fragment*/
	if (!last_fragment && (current_frag_size % 8 != 0))
		return MALFORMED_FRAG;
	/* Check 64K limit */
	/* todo check if WRIOP checks this */
	if ((current_frag_size + frag_offset_shifted) > MAX_IP_SIZE)
		return MALFORMED_FRAG;

	return NO_ERROR;
}

void ipr_time_out(uint64_t rfdc_ext_addr, uint16_t opaque_not_used)
{
	struct	 ipr_rfdc rfdc __attribute__((aligned(16)));
	struct	 ipr_instance instance_params;
	struct   dpni_drv *dpni_drv;
	uint16_t rfdc_status;
	uint32_t flags;
	uint8_t  num_of_frags;
	uint8_t  first_frag_index;
	uint8_t  index = 0;

	UNUSED(opaque_not_used);

	/* read and lock RFDC */
	cdma_read_with_mutex(rfdc_ext_addr,
			     CDMA_PREDMA_MUTEX_WRITE_LOCK,
			     &rfdc,
			     RFDC_SIZE);
	
	/* confirm timer expiration */
	tman_timer_completion_confirmation(rfdc.timer_handle);

	/* read instance parameters */
	cdma_read(&instance_params,
		  rfdc.instance_handle,
		  IPR_INSTANCE_SIZE);

	rfdc_status = rfdc.status;
	
	if (rfdc_status & IPV6_FRAME) {
		ipv6_rule_delete(rfdc_ext_addr,&instance_params);
	} else {
		/* IPV4 */
		table_rule_delete(TABLE_ACCEL_ID_CTLU,
				  instance_params.table_id_ipv4,
				  (union table_key_desc *)&rfdc.ipv4_key,
				  IPV4_KEY_SIZE,
				  NULL);
		/* DEBUG: check EIO */
	}

	if (rfdc_status & IPV6_FRAME) {
		/* Decrement no of IPv6 open frames in instance data structure*/
		ste_dec_counter(rfdc.instance_handle + \
				sizeof(struct ipr_instance)+ \
				offsetof(struct ipr_instance_extension,
				num_of_open_reass_frames_ipv6),
				1,
				STE_MODE_32_BIT_CNTR_SIZE);
		/* Increment no of frames in IPv6 TO stats */
		ipr_stats_update(&instance_params,
				 offsetof(struct extended_stats_cntrs,
						 time_out_ipv4_cntr),
				 0);
	} else { /* IPv4 */
		/* Decrement no of IPv4 open frames in instance data structure*/
		ste_dec_counter(rfdc.instance_handle + \
				sizeof(struct ipr_instance)+ \
				offsetof(struct ipr_instance_extension,
				num_of_open_reass_frames_ipv4),
				1,
				STE_MODE_32_BIT_CNTR_SIZE);
		/* Increment no of frames in TO stats */
		ipr_stats_update(&instance_params,
				 offsetof(struct extended_stats_cntrs,
						 time_out_ipv4_cntr),
				 1);
	}

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
		/* CDMA write, unlock, dec ref_cnt and release if ref_cnt=0 */
		cdma_access_context_memory(rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL |
				  CDMA_ACCESS_CONTEXT_MEM_RM_BIT,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE
				  | RFDC_SIZE,
				  (uint32_t *)REF_COUNT_ADDR_DUMMY);
		fdma_terminate_task();
	}
	if ((rfdc_status & OUT_OF_ORDER) && !(rfdc_status & ORDER_AND_OOO))
		first_frag_index = rfdc.first_frag_index;
	else
		first_frag_index = 0;
	while (num_of_frags != 0) {

		if(index != first_frag_index) {
			cdma_read((void *) HWC_FD_ADDRESS,
				  rfdc_ext_addr+START_OF_FDS_LIST+index*32,
				  FD_SIZE);
			fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS,
					FDMA_DIS_NO_FLAGS);
		}
		num_of_frags --;
		index++;
	}
	/* Fetch FD's fragment to be returned */
	cdma_read((void *) HWC_FD_ADDRESS,
		   rfdc_ext_addr+START_OF_FDS_LIST+first_frag_index*32,
		   FD_SIZE);
	/* CDMA write, unlock, dec ref_cnt and release if ref_cnt=0 */
	cdma_access_context_memory(rfdc_ext_addr,
				  CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL |
				  CDMA_ACCESS_CONTEXT_MEM_RM_BIT,
				  NULL,
				  &rfdc,
				  CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE
				  | RFDC_SIZE,
				  (uint32_t *)REF_COUNT_ADDR_DUMMY);

	if(rfdc_status & FIRST_ARRIVED) 
		 flags = IPR_TO_CB_FIRST_FRAG;
	else
		 flags = 0;
	/* Present the fragment to be returned to the user */
	/* todo check if spid should be stored in rfdc and restored here */
	fdma_present_default_frame_without_segments();

	if (rfdc_status & IPV6_FRAME)
		instance_params.ipv6_timeout_cb(
				  instance_params.cb_timeout_ipv6_arg,
				  flags);

	else
		instance_params.ipv4_timeout_cb(
					 instance_params.cb_timeout_ipv4_arg,
					 flags);
}

void move_to_correct_ordering_scope2(uint32_t osm_status)
{
	if (osm_status == 0) {
		/* return to original ordering scope that entered
		 * the ipr_reassemble function */
		osm_scope_exit();
		osm_scope_exit();
	} else if (osm_status & START_CONCURRENT) {
		osm_scope_relinquish_exclusivity();
	}
}

void check_remove_padding()
{
	uint8_t			delta;
	uint16_t		ipv4hdr_offset;
	uint16_t		start_padding;
	uint16_t		seg_length;
	struct ipv4hdr		*ipv4hdr_ptr;
	struct	parse_result	*pr =
				  (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct presentation_context *prc =
		(struct presentation_context *) HWC_PRC_ADDRESS;



	ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv4hdr_ptr = (struct ipv4hdr *)
		  (ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());

	start_padding = ipv4hdr_ptr->total_length+ipv4hdr_offset;
	delta = (uint8_t) (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) - start_padding);

	if (delta != 0) {
		/* Save seg_length before delete
		 * because seg_length can't be overwritten with wrong value by FDMA */ 
		seg_length = prc->seg_length;
		fdma_delete_default_segment_data(start_padding,
		                                 delta,
		                                 FDMA_REPLACE_NO_FLAGS);
		/* update prc length because represent wasn't done */
		prc->seg_length = seg_length - delta;
		/* For recalculating running sum */
		/* Updated FD[length] */
		LDPAA_FD_SET_LENGTH(HWC_FD_ADDRESS, start_padding);
		pr->gross_running_sum = 0;
	}
}


uint32_t out_of_order(struct ipr_rfdc *rfdc_ptr, uint64_t rfdc_ext_addr,
		  uint32_t last_fragment, uint16_t current_frag_size,
		  uint16_t frag_offset_shifted,
		  struct ipr_instance *instance_params_ptr)
{
	uint8_t				current_index;
	uint8_t				temp_frag_index;
	uint8_t				first_frag_index;
	uint8_t				octet_index;
	uint8_t				new_frag_index;
	uint8_t				current_index_in_octet;
	uint8_t				temp_index_in_octet;
	uint8_t				new_frag_index_in_octet;
	uint16_t			temp_total_payload;
	uint64_t			current_element_ext_addr;
	uint64_t			temp_element_ext_addr;
	uint64_t			new_frag_ext_addr;
	uint64_t			octet_ext_addr;
	struct link_list_element	*temp_element_ptr;
	struct link_list_element	*current_element_ptr;
	struct link_list_element	link_list[8];


	if (frag_offset_shifted < rfdc_ptr->total_in_order_payload) {
		/* overlap or duplicate */
		return MALFORMED_FRAG;
	}
	current_index = rfdc_ptr->next_index;
	current_element_ext_addr =  rfdc_ext_addr + START_OF_LINK_LIST +
					current_index*LINK_LIST_ELEMENT_SIZE;
	first_frag_index = rfdc_ptr->first_frag_index;
	temp_frag_index = rfdc_ptr->last_frag_index;
	temp_element_ext_addr = rfdc_ext_addr + START_OF_LINK_LIST +
				LINK_LIST_ELEMENT_SIZE*temp_frag_index;
	temp_total_payload = rfdc_ptr->biggest_payload;
	if (frag_offset_shifted >= temp_total_payload) {
		/* Bigger than last */
		temp_element_ptr = link_list;
		cdma_read_wrp((void *)temp_element_ptr,
			  temp_element_ext_addr,
			  LINK_LIST_ELEMENT_SIZE);
		if (LAST_FRAG_ARRIVED()) {
			/* Error */
			return MALFORMED_FRAG;
		}
		if(last_fragment)
			rfdc_ptr->expected_total_length = frag_offset_shifted +
							  current_frag_size;

		rfdc_ptr->biggest_payload = frag_offset_shifted +
							current_frag_size;
		current_element_ptr = link_list + 1;
		rfdc_ptr->last_frag_index	 = current_index;
		temp_element_ptr->next_index	 = current_index;
		current_element_ptr->prev_index  = temp_frag_index;
		current_element_ptr->frag_offset = frag_offset_shifted;
		current_element_ptr->frag_length = current_frag_size;
		/* not required */
		current_element_ptr->next_index	  = 0;
		cdma_write_wrp(temp_element_ext_addr,
				   temp_element_ptr,
				   LINK_LIST_ELEMENT_SIZE);
		cdma_write_wrp(current_element_ext_addr,
				   current_element_ptr,
				   LINK_LIST_ELEMENT_SIZE);
	} else {
		/* Smaller than last */
		if(last_fragment) {
			/* Current fragment is smaller than last but is marked
			 * as last */
			return MALFORMED_FRAG;
		}
		/* Bring 8 elements of the Link List */
		octet_index = temp_frag_index >> 3;
		octet_ext_addr = rfdc_ext_addr + START_OF_LINK_LIST +
			LINK_LIST_ELEMENT_SIZE * 8 * octet_index;
		cdma_read_wrp(link_list,
			  octet_ext_addr,
			  8*LINK_LIST_ELEMENT_SIZE);
		temp_index_in_octet = temp_frag_index & OCTET_LINK_LIST_MASK;
		temp_element_ptr = link_list + temp_index_in_octet;
		if ((frag_offset_shifted + current_frag_size) >
					temp_element_ptr->frag_offset) {
			/* Overlap */
			return MALFORMED_FRAG;
		}
		do {
			if (temp_frag_index == first_frag_index) {
				rfdc_ptr->first_frag_index	= current_index;
				temp_element_ptr->prev_index	= current_index;
				current_index_in_octet = current_index &
							 OCTET_LINK_LIST_MASK;
				if ((current_index >> 3) == octet_index) {
				 link_list[current_index_in_octet].frag_length =
							    current_frag_size;
				 link_list[current_index_in_octet].frag_offset =
							frag_offset_shifted;
				 link_list[current_index_in_octet].next_index =
								temp_frag_index;
					/* not required */
				  link_list[current_index_in_octet].prev_index =
									    0;

				cdma_write_wrp(octet_ext_addr,
					   link_list,
					   8*LINK_LIST_ELEMENT_SIZE);
				} else {
					if (temp_index_in_octet == 0)
						current_element_ptr =
							   temp_element_ptr+1;
					else
						current_element_ptr =
							  temp_element_ptr-1;

					current_element_ptr->frag_length =
							     current_frag_size;
					current_element_ptr->frag_offset =
							frag_offset_shifted;
					current_element_ptr->next_index =
							      temp_frag_index;
				    /* not required */
					current_element_ptr->prev_index  = 0;

				    cdma_write_wrp(current_element_ext_addr,
						   current_element_ptr,
						   LINK_LIST_ELEMENT_SIZE);
				    temp_element_ext_addr = rfdc_ext_addr +
						    START_OF_LINK_LIST +
						    LINK_LIST_ELEMENT_SIZE *
						    temp_frag_index;
				    cdma_write_wrp(temp_element_ext_addr,
						   temp_element_ptr,
						   LINK_LIST_ELEMENT_SIZE);
				}
				rfdc_ptr->current_total_length +=
							     current_frag_size;
				rfdc_ptr->num_of_frags++;
				
				if (rfdc_ptr->current_total_length ==
					rfdc_ptr->expected_total_length) {
					/* Reassembly is completed */
					/* Check max reassembly size */
					if((rfdc_ptr->current_total_length +
					    rfdc_ptr->first_frag_hdr_length) <=
				     instance_params_ptr->max_reass_frm_size) {
						
						/* Close current frame before storing FD */
						fdma_store_default_frame_data_wrp();

						/* Write FD in external buffer */
						current_element_ext_addr = rfdc_ext_addr +
									   START_OF_FDS_LIST +
									  current_index*FD_SIZE;
						cdma_write_wrp(current_element_ext_addr,
							       (void *)HWC_FD_ADDRESS,
							       FD_SIZE);

						return LAST_FRAG_OUT_OF_ORDER;
					}
					else 
						return MALFORMED_FRAG;
				} else {
					/* reassembly is not completed */

					/* Close current frame before storing FD */
					fdma_store_default_frame_data_wrp();

					/* Write FD in external buffer */
					current_element_ext_addr = rfdc_ext_addr +
								   START_OF_FDS_LIST +
								  current_index*FD_SIZE;
					cdma_write_wrp(current_element_ext_addr,
						       (void *)HWC_FD_ADDRESS,
						       FD_SIZE);

				       rfdc_ptr->next_index = current_index + 1;
				       return FRAG_OK_REASS_NOT_COMPL;
				}

			}
			temp_frag_index = temp_element_ptr->prev_index;
			if ((temp_frag_index>>3) != octet_index) {
				/* Bring 8 elements of the Link List */
				/* todo check if compiler add a clock
				   for next line */
				octet_index = temp_frag_index >> 3;
				octet_ext_addr = rfdc_ext_addr +
						START_OF_LINK_LIST +
						LINK_LIST_ELEMENT_SIZE * 8 *
						octet_index;
				cdma_read_wrp(link_list,
					  octet_ext_addr,
					  8*LINK_LIST_ELEMENT_SIZE);
			}
			temp_index_in_octet = temp_frag_index &
					      OCTET_LINK_LIST_MASK;
			temp_element_ptr = link_list + temp_index_in_octet;
		} while ((frag_offset_shifted + current_frag_size) <=
					temp_element_ptr->frag_offset);
		temp_total_payload = temp_element_ptr->frag_offset +
					temp_element_ptr->frag_length;
		if (frag_offset_shifted >= temp_total_payload) {
			current_index_in_octet = current_index &
						OCTET_LINK_LIST_MASK;
			if ((current_index >> 3) == octet_index) {
			new_frag_index = temp_element_ptr->next_index;
			link_list[current_index_in_octet].frag_length =
							current_frag_size;
			link_list[current_index_in_octet].frag_offset =
							frag_offset_shifted;
			link_list[current_index_in_octet].next_index =
								new_frag_index;
			link_list[current_index_in_octet].prev_index =
								temp_frag_index;

			link_list[temp_index_in_octet].next_index =
								current_index;

			if ((new_frag_index>>3) == octet_index) {
				new_frag_index_in_octet =
					new_frag_index&OCTET_LINK_LIST_MASK;
				link_list[new_frag_index_in_octet].prev_index =
								  current_index;
				cdma_write_wrp(octet_ext_addr,
					   link_list,
					   8*LINK_LIST_ELEMENT_SIZE);

			} else {
				cdma_write_wrp(octet_ext_addr,
					   link_list,
					   8*LINK_LIST_ELEMENT_SIZE);
			    new_frag_ext_addr = rfdc_ext_addr +
					    START_OF_LINK_LIST +
					LINK_LIST_ELEMENT_SIZE * new_frag_index;
			    cdma_read_wrp(link_list,
					  new_frag_ext_addr,
					  LINK_LIST_ELEMENT_SIZE);
			    link_list[0].prev_index = current_index;
			    cdma_write_wrp(new_frag_ext_addr,
					  link_list,
					  LINK_LIST_ELEMENT_SIZE);
			  }
			   } else {
			new_frag_index = temp_element_ptr->next_index;
			link_list[temp_index_in_octet].next_index =
								current_index;

			if ((new_frag_index>>3) == octet_index) {

				new_frag_index_in_octet = new_frag_index &
							OCTET_LINK_LIST_MASK;
				link_list[new_frag_index_in_octet].prev_index =
								  current_index;
				/* update temp and new_frag elements */
				cdma_write_wrp(octet_ext_addr,
					   link_list,
					   8*LINK_LIST_ELEMENT_SIZE);

			} else {
				/* update temp element */
				cdma_write_wrp(octet_ext_addr,
					   link_list,
					   8*LINK_LIST_ELEMENT_SIZE);
			    new_frag_ext_addr = rfdc_ext_addr +
					    START_OF_LINK_LIST +
					    LINK_LIST_ELEMENT_SIZE *
					    new_frag_index;
			    cdma_read_wrp(link_list,
				     new_frag_ext_addr,
				     LINK_LIST_ELEMENT_SIZE);
			    link_list[0].prev_index = current_index;
			    cdma_write_wrp(new_frag_ext_addr,
					  link_list,
					  LINK_LIST_ELEMENT_SIZE);
			}

			link_list[0].frag_length = current_frag_size;
			link_list[0].frag_offset = frag_offset_shifted;
			link_list[0].next_index  = new_frag_index;
			link_list[0].prev_index  = temp_frag_index;

			cdma_write_wrp(current_element_ext_addr,
					   link_list,
					   LINK_LIST_ELEMENT_SIZE);
			   }
		} else {
			/* Error */
			return MALFORMED_FRAG;
		}
	}

	rfdc_ptr->current_total_length += current_frag_size;
	rfdc_ptr->num_of_frags++;

	if (rfdc_ptr->current_total_length == rfdc_ptr->expected_total_length) {
		/* Reassembly is completed */
		/* Check max reassembly size */
		if((rfdc_ptr->current_total_length +
		    rfdc_ptr->first_frag_hdr_length) <=
		    instance_params_ptr->max_reass_frm_size) {
			/* Close current frame before storing FD */
			fdma_store_default_frame_data_wrp();

			/* Write FD in external buffer */
			current_element_ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
							   current_index*FD_SIZE;
			cdma_write_wrp(current_element_ext_addr,
					   (void *)HWC_FD_ADDRESS,
					   FD_SIZE);

			return LAST_FRAG_OUT_OF_ORDER;
		}
		else
			return MALFORMED_FRAG;
	} else {
		/* reassembly is not completed */
		/* Close current frame before storing FD */
		fdma_store_default_frame_data_wrp();

		/* Write FD in external buffer */
		current_element_ext_addr = rfdc_ext_addr + START_OF_FDS_LIST +
						   current_index*FD_SIZE;
		cdma_write_wrp(current_element_ext_addr,
				   (void *)HWC_FD_ADDRESS,
				   FD_SIZE);

		rfdc_ptr->next_index++;
		return FRAG_OK_REASS_NOT_COMPL;
	}
}

void ipr_stats_update(struct ipr_instance *instance_params_ptr,
		      uint32_t counter_offset, uint32_t frame_is_ipv4)
{
	if (instance_params_ptr->flags & IPR_MODE_EXTENDED_STATS_EN) {
		if (frame_is_ipv4)
			ste_inc_counter(instance_params_ptr->extended_stats_addr +
					counter_offset,
					1,
					STE_MODE_32_BIT_CNTR_SIZE);
		else
			ste_inc_counter(instance_params_ptr->extended_stats_addr +
					counter_offset+4,
					1,
					STE_MODE_32_BIT_CNTR_SIZE);

	return;
	}
}


void ipr_modify_max_reass_frm_size(ipr_instance_handle_t ipr_instance,
					  uint16_t max_reass_frm_size)
{
	cdma_write(ipr_instance+offsetof(struct ipr_instance,
		    max_reass_frm_size),
		   &max_reass_frm_size,
		   sizeof(max_reass_frm_size));
	return;
}

void ipr_modify_min_frag_size_ipv4(ipr_instance_handle_t ipr_instance,
				      uint16_t min_frag_size)
{
	cdma_write(ipr_instance+
		   offsetof(struct ipr_instance,min_frag_size_ipv4),
		   &min_frag_size,
		   sizeof(min_frag_size));
	return;
}

void ipr_modify_min_frag_size_ipv6(ipr_instance_handle_t ipr_instance,
				      uint16_t min_frag_size)
{
	cdma_write(ipr_instance+
		   offsetof(struct ipr_instance,min_frag_size_ipv6),
		   &min_frag_size,
		   sizeof(min_frag_size));
	return;
}

void ipr_modify_timeout_value_ipv4(ipr_instance_handle_t ipr_instance,
				      uint16_t reasm_timeout_value_ipv4)
{
	cdma_write(ipr_instance+
		   offsetof(struct ipr_instance, timeout_value_ipv4),
		   &reasm_timeout_value_ipv4,
		   sizeof(reasm_timeout_value_ipv4));
	return;
}

void ipr_modify_timeout_value_ipv6(ipr_instance_handle_t ipr_instance,
				      uint16_t reasm_timeout_value_ipv6)
{

	cdma_write(ipr_instance+
		   offsetof(struct ipr_instance, timeout_value_ipv6),
		   &reasm_timeout_value_ipv6,
		   sizeof(reasm_timeout_value_ipv6));
	return;
}

void ipr_get_reass_frm_cntr(ipr_instance_handle_t ipr_instance,
				uint32_t flags, uint32_t *reass_frm_cntr)
{

	if (flags & IPR_STATS_IP_VERSION)
		cdma_read(reass_frm_cntr,
			  ipr_instance+sizeof(struct ipr_instance)+
			  offsetof(struct ipr_instance_extension,
			  ipv4_reass_frm_cntr),
			  sizeof(*reass_frm_cntr));
	else
		cdma_read(reass_frm_cntr,
			  ipr_instance+sizeof(struct ipr_instance)+
			  offsetof(struct ipr_instance_extension,
				   ipv6_reass_frm_cntr),
			  sizeof(*reass_frm_cntr));
	return;
}

#pragma push
	/* make all following data go into .exception_data */
#pragma section data_type ".exception_data"
#pragma stackinfo_ignore on
void ipr_exception_handler(enum ipr_function_identifier func_id,
		     uint32_t line,
		     int32_t status)
{
	char *func_name;
	char *err_msg;
	
	status = status & 0xFF;
	
	/* Translate function ID to function name string */
	switch(func_id) {
	case IPR_INIT:
		func_name = "ipr_init";
		switch(status) {
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
	case IPR_CREATE_INSTANCE:
		func_name = "ipr_create_instance";
		switch(status) {
			case EINVAL:
				err_msg = "Could not release into BPID, \
					   BPID is full\n";
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
				err_msg = "Not enough memory available to create\
					   table\n";
				break;
			default:
				err_msg = "Unknown or Invalid status Error.\n";

		}
		break;
	case IPR_DELETE_INSTANCE:
		func_name = "ipr_delete_instance";
		     err_msg = "No free timer for this tmi id\n";
		break;
	case IPR_REASSEMBLE:
		func_name = "ipr_reassemble";
		switch(status) {
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

