/**************************************************************************//**
@File		ipr.c

@Description	This file contains the AIOP SW IP Reassembly implementation.

		Copyright 2014 Freescale Semiconductor, Inc.
*//***************************************************************************/


#include "general.h"
#include "dplib/fsl_parser.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_tman.h"
#include "dplib/fsl_osm.h"
#include "dplib/fsl_ctlu.h"
#include "net/fsl_net.h"
#include "fdma.h"
#include "checksum.h"
#include "ipr.h"

struct ipr_global_parameters ipr_global_parameters1;

int32_t ipr_reassemble(ipr_instance_handle_t instance_handle)
{
	uint16_t ipv4hdr_offset;
	uint32_t status_insert_to_LL;
	uint32_t osm_status;
	uint64_t rfdc_ext_addr;
	uint64_t context_memory;
	int32_t  sr_status;
	uint8_t	 keysize;
	struct ipr_rfdc rfdc;
	struct	ipv4hdr	*ipv4hdr_ptr;
	struct ipr_instance instance_params;
	struct scope_status_params scope_status;
	struct ctlu_lookup_result lookup_result;
	struct ctlu_table_rule rule;

	ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv4hdr_ptr = (struct ipv4hdr *)
			(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());
		
	/* Get OSM status (ordering scope mode and levels) */	
	osm_get_scope(&scope_status);

	if(scope_status.scope_mode == EXCLUSIVE) {
		if(PARSER_IS_OUTER_IP_FRAGMENT_DEFAULT()) {
			osm_status = BYPASS_OSM;				
		} else {
			/* regular frame */
			return IPR_REASSEMBLY_SUCCESS;
		}
	} else {
	    /* move to exclusive */
	    osm_scope_transition_to_exclusive_with_increment_scope_id();
	    if(PARSER_IS_OUTER_IP_FRAGMENT_DEFAULT()) {
		/* Fragment */
		if(scope_status.scope_level <= 2) {
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
			return IPR_REASSEMBLY_SUCCESS;
		}
	}
	
	/* read and lock instance handle parameters */
	cdma_read_with_mutex(instance_handle,
			     CDMA_PREDMA_MUTEX_WRITE_LOCK,
			     &instance_params,
			     sizeof(struct ipr_instance));
	
	if(check_for_frag_error() == NO_ERROR) {
		/* Good fragment */
		if(instance_params.flags & INSTANCE_VALID) {
		sr_status = ctlu_table_lookup_by_keyid(
				instance_params.table_id_ipv4,
				ipr_global_parameters1.ipr_key_id_ipv4,
				&lookup_result);
		if(sr_status == CTLU_LOOKUP_STATUS_MATCH_FOUND) {
		     /* Hit */
		     rfdc_ext_addr = lookup_result.opaque0_or_reference;
		     /* Unlock instance handle parameters*/
		     cdma_mutex_lock_release(instance_handle);
		     if(osm_status == NO_BYPASS_OSM) {
			/* create nested per reassembled frame */
			osm_scope_enter_to_exclusive_with_new_scope_id(
					       (uint32_t)rfdc_ext_addr);
			}
		     /* read and lock RFDC */
		     cdma_read_with_mutex( rfdc_ext_addr,
					  CDMA_PREDMA_MUTEX_WRITE_LOCK,
					  &rfdc,
					  sizeof(struct ipr_rfdc));

			if(!(rfdc.status & RFDC_VALID)) {
			   move_to_correct_ordering_scope2(osm_status);
			   cdma_write_release_lock_and_decrement(
					       rfdc_ext_addr,
					       (void *)&rfdc,
					       sizeof(struct ipr_rfdc));
			   /* Early Time out */
			   return IPR_ERROR;
			}
			
		} else if(sr_status ==
				CTLU_LOOKUP_STATUS_MATCH_NOT_FOUND) {
			/* Miss */
		    cdma_acquire_context_memory(
				    IPR_CONTEXT_SIZE,
				    ipr_global_parameters1.ipr_pool_id,
				    &context_memory);
		    rfdc_ext_addr = context_memory;
		    /* Lock RFDC */
		    cdma_mutex_lock_take(rfdc_ext_addr, CDMA_MUTEX_WRITE_LOCK);
		    /* Reset RFDC + Link List */
		    cdma_ws_memory_init((void *)&rfdc,
					SIZE_TO_INIT,
					0);
		    /* Add entry to TLU table */
		    /* Generate key */
		    ctlu_gen_key(ipr_global_parameters1.ipr_key_id_ipv4,
				 (union ctlu_key *)&rule.key.key_em,
				 &keysize);
		//	rule.key.key_em = key_em;
		    rule.options = 0;
		    rule.result.type = CTLU_RULE_RESULT_TYPE_REFERENCE;
		    rule.result.op_rptr_clp.reference_pointer = rfdc_ext_addr;
		    ctlu_table_rule_create(instance_params.table_id_ipv4,
					   &rule,
					   keysize);
		    /* store key in RDFC */
		    rfdc.key[0] = *(uint64_t *)rule.key.key_em.key;
		    rfdc.key[1] = *(uint64_t *)(rule.key.key_em.key+8);
		    /* todo release struct rule  or call function for gen+add rule */
		    rfdc.status = RFDC_VALID;
		    /* todo : unify this cdma command with previous lock into
		     * cdma_access_context_memory() */
		    /* Increment ref. count to balance with later decrement */
		    cdma_refcount_increment(rfdc_ext_addr);
			
		    /* create Timer in TMAN */
		    tman_create_timer(
			       instance_params.tmi_id,
			       ipr_global_parameters1.ipr_timeout_flags,
			       instance_params.timeout_value_ipv4,
			       rfdc_ext_addr,
			       NULL,
			       ipr_global_parameters1.ipr_timeout_epid,
			       (uint32_t)instance_params.ipv4_timeout_cb,
			       &rfdc.timer_handle);
		    instance_params.num_of_open_reass_frames += 1;
		    /* Write and unlock instance handle parameters*/
		    cdma_access_context_memory(
				   instance_handle,
				   CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT,
				   0,
				   (void *)&instance_params,
				   CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE | 
				   sizeof(struct ipr_instance),
				   (uint16_t *)REF_COUNT_ADDR_DUMMY);

		     if(osm_status == NO_BYPASS_OSM) {
			/* create nested per reassembled frame */
			osm_scope_enter_to_exclusive_with_new_scope_id(
					       (uint32_t)rfdc_ext_addr);
			}
			
		} else {
			/* TLU lookup SR error */
		}
		
		status_insert_to_LL = 
			  ipr_insert_to_link_list(&rfdc,rfdc_ext_addr);
		switch(status_insert_to_LL) {
		case 0:
			/* Fragment was successfully added to LL */
			move_to_correct_ordering_scope2(osm_status);
			if(instance_params.flags &
					IPR_MODE_IPV4_TO_TYPE) {
			/* recharge timer in case of time out 
			 * between fragments */
				tman_recharge_timer(rfdc.timer_handle);
			}
			/* Write and release updated 64 first bytes
			 * of RFDC */
			cdma_write_release_lock_and_decrement(
					       rfdc_ext_addr,
					       &rfdc,
					       sizeof(struct ipr_rfdc));
			return IPR_REASSEMBLY_NOT_COMPLETED;
		case 1:
			/* Last fragment, need reassembly in order */
			closing_in_order(&rfdc,rfdc_ext_addr);
			break;
		case 2:
			/* Last fragment, need re-ordering */
			closing_with_reordering(&rfdc,rfdc_ext_addr);
			break;
		case 3:
			/* duplicate or overlap fragment */
			return IPR_MALFORMED_FRAG;
			break;
		}
		if(ip_header_update_and_l4_validation(&rfdc) ==
							~SUCCESS) {
			/* L4 checksum is not valid */
		} else {
		/* L4 checksum is valid */			
		/* Write and release updated 64 first bytes of RFDC */
		cdma_write_release_lock_and_decrement(
				       rfdc_ext_addr,
				       &rfdc,
				       sizeof(struct ipr_rfdc));
		
		move_to_correct_ordering_scope2(osm_status);

		/* read and lock instance handle parameters */
		cdma_read_with_mutex( instance_handle,
				CDMA_PREDMA_MUTEX_WRITE_LOCK,
				&instance_params,
				sizeof(struct ipr_instance));

		instance_params.num_of_open_reass_frames --;
		if(!(instance_params.flags & INSTANCE_VALID)) {
			/* instance no more valid */
		}		
		/* Write and unlock instance params */
		/* todo no decrement ref. count */
		cdma_write_release_lock_and_decrement(
				instance_handle,
				(void *)&instance_params,
				sizeof(struct ipr_instance));
		cdma_access_context_memory(
				instance_handle,
				CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT,
				0,
				(void *)&instance_params,
				CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE | 
				sizeof(struct ipr_instance),
				(uint16_t *)REF_COUNT_ADDR_DUMMY);

		
		return IPR_REASSEMBLY_SUCCESS;
		} /* L4 checksum is valid */
		} 
		else {
			/* instance not valid */
			cdma_mutex_lock_release(instance_handle);
			move_to_correct_ordering_scope1(osm_status);
			return IPR_ERROR;
		}

	} else {
		/* Error fragment */
		cdma_mutex_lock_release(instance_handle);
		move_to_correct_ordering_scope1(osm_status);
		return IPR_MALFORMED_FRAG;
	}	
	/* todo remove the following return */
	return SUCCESS;
}

uint32_t ipr_insert_to_link_list(struct ipr_rfdc *rfdc_ptr,
				 uint64_t rfdc_ext_addr)
{

	struct	ipv4hdr			*ipv4hdr_ptr;
	uint16_t 			frag_offset;
	uint16_t			ipv4hdr_offset;
	uint16_t			current_frag_size;
	uint16_t			expected_frag_offset;
	uint64_t			ext_addr;
	uint64_t			my_element_ext_addr;
	uint64_t			link_list_ext_addr;
	struct link_list_element	my_element;
	struct link_list_element	*my_element_ptr;
	struct link_list_element	*last_frag_element_ptr;
	struct link_list_element	*first_frag_element_ptr;
	uint8_t				current_index;
	uint16_t			size;
	uint16_t			ip_header_size;
	struct link_list_element	link_list[33];
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	
	ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv4hdr_ptr = (struct ipv4hdr *)
			(ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());
		
	frag_offset = ipv4hdr_ptr->flags_and_offset & FRAG_OFFSET_MASK;
	
	if (frag_offset == 0)
	   current_frag_size = ipv4hdr_ptr->total_length;
	else {
	    ip_header_size = (uint16_t)
	    		((ipv4hdr_ptr->vsn_and_ihl & IPV4_HDR_IHL_MASK)>>2);
		current_frag_size = ipv4hdr_ptr->total_length - ip_header_size;
	}

	if(!(rfdc_ptr->status & OUT_OF_ORDER)) {
		/* In order handling */
		expected_frag_offset = rfdc_ptr->current_total_length>>3;
        if (frag_offset == expected_frag_offset) {
        	        	   
                rfdc_ptr->current_total_length += current_frag_size;
                if(frag_offset != 0) {
                /* Save header to be removed in FD[FRC] */
                	((struct ldpaa_fd *)HWC_FD_ADDRESS)->frc = 
                			PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	
            	/* Add current frag's running sum for L4 checksum check */
                rfdc_ptr->current_running_sum = cksum_ones_complement_sum16(
                				rfdc_ptr->current_running_sum,
                				pr->running_sum);
                } else {
                /* Set 1rst frag's running sum for L4 checksum check */
                    rfdc_ptr->current_running_sum = pr->gross_running_sum;
                }
                /* Write FD in external buffer */
                ext_addr = rfdc_ext_addr + sizeof(struct ipr_rfdc) + 
                	   MAX_NUM_OF_FRAGS*sizeof(struct link_list_element) +
                	   rfdc_ptr->next_index*sizeof(struct ldpaa_fd);
                cdma_write(ext_addr,
                	   (void *)HWC_FD_ADDRESS,
                	   sizeof(struct ldpaa_fd));

                rfdc_ptr->next_index++;
               	if(IS_LAST_FRAGMENT()) {
               		return LAST_FRAG_IN_ORDER;
               	} else {
               		/* Non closing fragment */
                    return FRAG_IN_ORDER_OK;
               	}
            }
            else if (frag_offset < rfdc_ptr->current_total_length) {
            /* Malformed Error */
            	return FRAG_ERROR;
            } else {
            /* New out of order */
            	rfdc_ptr->status |= OUT_OF_ORDER;
           //     p_IntRfd->outOfOrderTotalFragmentsPayload = p_IntRfd->totalFragmentsPayload;
                rfdc_ptr->current_total_length += current_frag_size;
            	current_index = rfdc_ptr->next_index;
            	rfdc_ptr->index_to_out_of_order = current_index;
            	rfdc_ptr->index_to_out_of_order = current_index;
                rfdc_ptr->first_frag_index = current_index;
                rfdc_ptr->last_frag_index = current_index;
                my_element.next_index = 0;
                my_element.prev_index = 0;
                my_element.frag_offset = frag_offset;
                /* Write my element of link list */
                my_element_ext_addr = rfdc_ext_addr + sizeof(struct ipr_rfdc) +
                				current_index*sizeof(struct link_list_element);
                cdma_write(my_element_ext_addr,
                		   (void *)&my_element,
                		   sizeof(struct link_list_element));
                /* Save header size to be removed upon concatenation */
          //      *(uint16_t *)(HWC_FD_ADDRESS+16) = ipv4hdr_offset;
                ((struct ldpaa_fd *)(HWC_FD_ADDRESS))->frc = ipv4hdr_offset;
                
                /* Write FD in RFDC */
                ext_addr = rfdc_ext_addr + sizeof(struct ipr_rfdc) +
                			MAX_NUM_OF_FRAGS*sizeof(struct link_list_element) + 
                			rfdc_ptr->next_index*sizeof(struct ldpaa_fd);
                cdma_write(ext_addr,
                		   (void *)HWC_FD_ADDRESS,
                		   sizeof(struct ldpaa_fd));

                rfdc_ptr->next_index++;
            	
        		if(!IS_LAST_FRAGMENT()) {        
            		rfdc_ptr->expected_total_length = frag_offset<<3 +
            										  	  	  current_frag_size;
        		}
                /* Write updated RFDC */
                cdma_write(rfdc_ext_addr,
                		   (void *)rfdc_ptr,
                		   sizeof(struct ipr_rfdc));
                return FRAG_OUT_OF_ORDER_OK;
            	}
            } else {
        		/* Out of order handling */
            	current_index = rfdc_ptr->next_index;
            	if(current_index <= 31) {
            		/* all the link list will be in the workspace */
                    size = (uint16_t)(current_index*8);
                    link_list_ext_addr = rfdc_ext_addr+64;
                    cdma_read( link_list,
            				   rfdc_ext_addr+64,  
            				   size);
            	  last_frag_element_ptr = &link_list[rfdc_ptr->last_frag_index];
            	  if(frag_offset > last_frag_element_ptr->frag_offset) {
            			/* bigger than biggest arrived fragment */
            			if(!LAST_FRAG_ARRIVED()) {
            				last_frag_element_ptr->next_index = current_index;
            				my_element_ptr = (struct link_list_element *)
            								&link_list[current_index];
            				my_element_ptr->next_index = 0;
                            my_element_ptr->prev_index = 
                            						 rfdc_ptr->last_frag_index;
                            my_element_ptr->frag_offset = frag_offset;
                            /* Save header size to be removed upon
                               concatenation */
                            *(uint16_t *)(HWC_FD_ADDRESS+16) = ipv4hdr_offset;

                            /* Write back updated link list */                
                            cdma_write(rfdc_ext_addr+64,
                            		   link_list,
                            		   size+8);
                            /* Write FD in RFDC */                
                            ext_addr = rfdc_ext_addr + 576 + current_index*32;
                            cdma_write(rfdc_ext_addr+64,
                            		   (void *)HWC_FD_ADDRESS,
                            		   32);

                            return FRAG_OUT_OF_ORDER_OK;
            				
            			} else {
            				/* Malformed fragment */
            			}
            			
            		} else {
            			/* smaller than biggest arrived fragment */
            			if(frag_offset < rfdc_ptr->first_frag_offset) {
            				/* smaller than smallest arrived fragment */
                      	    first_frag_element_ptr = 
                      	    		     &link_list[rfdc_ptr->first_frag_index];
                      	  first_frag_element_ptr->prev_index = current_index;
                      	  my_element_ptr = (struct link_list_element *)
          								&link_list[current_index];
                      	  my_element_ptr->next_index = 
                      			  	  	  	  	  rfdc_ptr->first_frag_index;
                          my_element_ptr->prev_index = 0;
                          my_element_ptr->frag_offset = frag_offset;
                          /* Save header size to be removed upon
                             concatenation */
                          *(uint16_t *)(HWC_FD_ADDRESS+16) = ipv4hdr_offset;

                          /* Write back updated link list */                
                          cdma_write(rfdc_ext_addr+64,
                          		   link_list,
                          		   size+8);
                          /* Write FD in RFDC */                
                          ext_addr = rfdc_ext_addr + 576 + current_index*32;
                          cdma_write(rfdc_ext_addr+64,
                          		   (void *)HWC_FD_ADDRESS,
                          		   32);
                      //    return;
            			} else {
            				/* current frag is between smallest and biggest
            				   arrived fragment */
            //				checked_element = (struct link_list_element *)
              //    						 &link_list[rfdc_ptr->first_frag_index];
            //				if(frag_offset > checked_element_ptr->frag_offset)
            //				{
            					
            //				}
            			}
            		}
            	}
            	else {
            		/* more than 31 fragments arrived */
            		return FRAG_OUT_OF_ORDER_OK;
            	}
            }
	/* to be removed */
	return SUCCESS;
}

uint32_t closing_in_order(struct ipr_rfdc *rfdc_ptr, uint64_t rfdc_ext_addr)
{

	struct 		ldpaa_fd fds_to_concatenate[2];
    	uint64_t	fds_to_fetch_addr;
    	uint8_t		frame_handle1;
    	uint8_t		frame_handle2;
    	uint8_t		num_of_frags;
//    	uint16_t	ipv4hdr_offset;
//    	uint16_t	new_total_length;
//    	uint16_t	old_ip_checksum;
//    	uint16_t	gross_running_sum;
 //   	uint16_t	ip_hdr_cksum;
 //   	uint16_t	new_flags_and_offset;
    	struct		fdma_concatenate_frames_params concatenate_frame_params;
    	struct		fdma_present_frame_params present_frame_params;
//    	struct 		ipv4hdr *ipv4hdr_ptr;
    	struct 		parse_result *pr = 
    				   (struct parse_result *)HWC_PARSE_RES_ADDRESS;


    	/* Close current fragment */
    	fdma_store_default_frame_data();

    	/* Bring into workspace 2 FDs to be concatenated */
    	fds_to_fetch_addr = rfdc_ext_addr+ sizeof(struct ipr_rfdc) + 
    			     MAX_NUM_OF_FRAGS* sizeof(struct link_list_element);
		cdma_read( (void *)fds_to_concatenate,
			   fds_to_fetch_addr,  
    			   64);
		fds_to_fetch_addr += MAX_NUM_OF_FRAGS;
		/* Copy 1rst FD to default frame FD's place */	
		*((struct ldpaa_fd *)(HWC_FD_ADDRESS)) = fds_to_concatenate[0];
		
		/* set default task parameters */
		PRC_SET_SEGMENT_ADDRESS((uint32_t)TLS_SECTION_END_ADDR + 
		                     DEFAULT_SEGMENT_HEADOOM_SIZE);
		PRC_SET_SEGMENT_LENGTH(DEFAULT_SEGMENT_SIZE);
		PRC_SET_SEGMENT_OFFSET(0);
		PRC_RESET_SR_BIT();
		PRC_SET_ASA_SIZE(0);
		PRC_SET_PTA_ADDRESS(PRC_PTA_NOT_LOADED_ADDRESS);
		
		/* Open 1rst frame and get frame handle */
		present_frame_params.flags = FDMA_INIT_NDS_BIT;
		present_frame_params.asa_size = 0;
		present_frame_params.fd_src = (void *)HWC_FD_ADDRESS;
		present_frame_params.pta_dst =
					   (void *)PRC_PTA_NOT_LOADED_ADDRESS;

		fdma_present_frame(&present_frame_params);
		frame_handle1 = present_frame_params.frame_handle;
		
		/* Open 2nd frame and get frame handle */
		present_frame_params.fd_src = fds_to_concatenate+1;

		fdma_present_frame(&present_frame_params);
		frame_handle2 = present_frame_params.frame_handle;
		
		if(0)//SF_MODE
			concatenate_frame_params.flags = FDMA_CONCAT_SF_BIT;
		else
			concatenate_frame_params.flags = FDMA_CONCAT_NO_FLAGS;

		concatenate_frame_params.spid = *((uint8_t *) HWC_SPID_ADDRESS);
		concatenate_frame_params.frame1 = (uint16_t) frame_handle1;
		concatenate_frame_params.frame2 = (uint16_t) frame_handle2;
		/* Take header size to be removed from 2nd FD[FRC] */
		concatenate_frame_params.trim =
					    (uint8_t)fds_to_concatenate[1].frc;
			
		fdma_concatenate_frames(&concatenate_frame_params);
		num_of_frags = rfdc_ptr->num_of_frags - 2;
		while(num_of_frags != 0) {
	    	/* Bring into workspace 2 FDs to be concatenated */
			cdma_read( (void *)fds_to_concatenate,
						fds_to_fetch_addr,  
	    				sizeof(fds_to_concatenate));
			/* Open frame and get frame handle */
			present_frame_params.fd_src = fds_to_concatenate;
			frame_handle2 = present_frame_params.frame_handle;
			concatenate_frame_params.frame2 =
						     (uint16_t) frame_handle2;
			/* Take header size to be removed from FD[FRC] */
			concatenate_frame_params.trim  =
					    (uint8_t)fds_to_concatenate[0].frc;

			fdma_concatenate_frames(&concatenate_frame_params);
			
			num_of_frags --;
			if(num_of_frags != 0) {
			present_frame_params.fd_src = fds_to_concatenate+1;
			frame_handle2 = present_frame_params.frame_handle;
			concatenate_frame_params.frame2 =
						      (uint16_t) frame_handle2;
			/* Take header size to be removed from FD[FRC] */
			concatenate_frame_params.trim =
					    (uint8_t)fds_to_concatenate[1].frc;

			fdma_concatenate_frames(&concatenate_frame_params);
			}
		}
			/* default frame is now the full reassembled frame */
			fdma_present_default_frame();
			/* run parser */
			parse_result_generate_default(0);
			return SUCCESS;
}

uint32_t ip_header_update_and_l4_validation(struct ipr_rfdc *rfdc_ptr)
{
	uint16_t	ipv4hdr_offset;
	uint16_t	new_total_length;
	uint16_t	ip_hdr_cksum;
	uint16_t	old_ip_checksum;
	uint16_t	new_flags_and_offset;
	uint16_t	gross_running_sum;
	struct ipv4hdr  *ipv4hdr_ptr;
    	struct 		parse_result *pr = 
    				   (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	
	ipv4hdr_offset =
		 (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv4hdr_ptr = (struct ipv4hdr *)
		  (ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS());
	/* update IP checksum */
	new_total_length = rfdc_ptr->current_total_length;
	old_ip_checksum = ipv4hdr_ptr->hdr_cksum;
	ip_hdr_cksum = old_ip_checksum;
	cksum_accumulative_update_uint32(ip_hdr_cksum,
			    		ipv4hdr_ptr->total_length,
			    		new_total_length);
	ipv4hdr_ptr->total_length = new_total_length;
	
	new_flags_and_offset = ipv4hdr_ptr->flags_and_offset & RESET_MF_BIT;
	cksum_accumulative_update_uint32(
				 ip_hdr_cksum,
				 ipv4hdr_ptr->flags_and_offset,
				 new_flags_and_offset);
	ipv4hdr_ptr->flags_and_offset = new_flags_and_offset;

	ipv4hdr_ptr->hdr_cksum = ip_hdr_cksum;
	
	/* L4 checksum Validation */
	/* prepare gross running sum for L4 checksum validation by parser */
	gross_running_sum = rfdc_ptr->current_running_sum;
	cksum_accumulative_update_uint32(
					gross_running_sum,
				 	old_ip_checksum,
					ipv4hdr_ptr->hdr_cksum);

	pr->gross_running_sum = gross_running_sum;
	
	/* update FDMA with total length and IP header checksum*/
	fdma_modify_default_segment_data(ipv4hdr_offset+2,10);

	/* Call parser for L4 validation */
	parse_result_generate_default(PARSER_VALIDATE_L4_CHECKSUM);
	if ((pr->parse_error_code == PARSER_UDP_CHECKSUM_ERROR)
			|| 
	   (pr->parse_error_code == PARSER_TCP_CHECKSUM_ERROR))
	{
		/* error in L4 checksum */
	}
	return SUCCESS;
}

uint32_t closing_with_reordering(struct ipr_rfdc *rfdc_ptr,
				 uint64_t rfdc_ext_addr)
{
	rfdc_ext_addr = 0;
	rfdc_ptr->first_frag_index = 0;
	return SUCCESS;
}

uint32_t check_for_frag_error()
{
	return SUCCESS;
}
