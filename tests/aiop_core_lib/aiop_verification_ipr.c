/**************************************************************************//**
@File          aiop_verification_ipr.c

@Description   This file contains the AIOP IPR FMs SW Verification
*//***************************************************************************/

#include "aiop_verification.h"
#include "aiop_verification_ipr.h"
#include "aiop_verification_data.h"

extern __VERIF_GLOBAL uint64_t verif_ipr_instance_handle;

uint16_t aiop_verification_ipr(uint32_t asa_seg_addr)
{
	
	uint16_t str_size = STR_SIZE_ERR;
	uint32_t opcode;
	uint64_t ipr_instance_handle; 

#ifdef CLOSE_MODEL
	ipr_instance_handle_t ipr_instance;
	ipr_instance_handle_t *ipr_instance_ptr;
#endif
	opcode  = *((uint32_t *) asa_seg_addr);


	switch (opcode) {
		/* IPR Init Command Verification */
		case IPR_VERIF_INIT_CMD_STR:
		{
			struct ipr_init_verif_command *str =
				(struct ipr_init_verif_command *) asa_seg_addr;
			ipr_init(str->max_buffers, str->flags);
			str_size = sizeof(struct ipr_init_verif_command);
			break;
		}
		/* IPR create instance Command Verification */
		case IPR_CREATE_INSTANCE_CMD_STR:
		{
			struct ipr_create_instance_command *str =
				(struct ipr_create_instance_command *) asa_seg_addr;
		/*	str->status = ipr_create_instance(
					&(str->ipr_params),
					&verif_ipr_instance_handle);*/
			str->status = ipr_create_instance(
					&(str->ipr_params),
					&ipr_instance_handle);

			verif_ipr_instance_handle = ipr_instance_handle;
			str_size = sizeof(struct ipr_create_instance_command);
			break;
		}
		/* IPR reassemble Command Verification */
		case IPR_REASSEMBLE_CMD_STR:
		{
			struct ipr_reassemble_command *str =
				(struct ipr_reassemble_command *) asa_seg_addr;
			str->status = ipr_reassemble(verif_ipr_instance_handle);
			str_size = sizeof(struct ipr_reassemble_command);
			break;
		}
#ifdef CLOSE_MODEL
		case IPR_CREATE_INSTANCE_CMD_STR:
		{
			struct ipr_create_instance_command *str =
				(struct ipr_create_instance_command *) asa_seg_addr;
			if(str->ipr_instance_ref)
				ipr_instance_ptr = 
				(ipr_instance_handle_t*) str->ipr_instance;
			else 
				ipr_instance_ptr = &(str->ipr_instance);

			
			
			str->status = ipr_create_instance(
					&(str->ipr_params),
					ipr_instance_ptr);
			str_size = sizeof(struct ipr_create_instance_command);
			break;
		}
		/* IPR reassemble Command Verification */
		case IPR_REASSEMBLE_CMD_STR:
		{
			struct ipr_reassemble_command *str =
				(struct ipr_reassemble_command *) asa_seg_addr;
			if(str->ipr_instance_ref)
				ipr_instance = 
		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
			else 
				ipr_instance = str->ipr_instance;

			str->status = ipr_reassemble(ipr_instance);
			str_size = sizeof(struct ipr_reassemble_command);
			break;
		}
#endif
		
#if 0
		/* IPR delete instance Command Verification */
		case IPR_DELETE_INSTANCE_CMD_STR:
		{
			struct ipr_delete_instance_command *str =
				(struct ipr_delete_instance_command *) asa_seg_addr;
			if(str->ipr_instance_ref)
				ipr_instance = 
		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
			else 
				ipr_instance = str->ipr_instance;

			str->status = ipr_delete_instance(
					ipr_instance,
					str->confirm_delete_cb,
					str->delete_arg);
			str_size = sizeof(struct ipr_delete_instance_command);
			break;
		}
		/* IPR max reassembled frame size Command Verification */
		case IPR_MODIFY_REASS_FRM_SIZE_CMD_STR:
		{
			struct ipr_modify_max_reass_frm_size_command *str =
				(struct ipr_modify_max_reass_frm_size_command *) asa_seg_addr;
            if(str->ipr_instance_ref)
            	ipr_instance = 
            		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
            else 
            	ipr_instance = str->ipr_instance;

			str->status = ipr_modify_max_reass_frm_size(
					ipr_instance,
					str->max_reass_frm_size);
			str_size = sizeof(struct ipr_modify_max_reass_frm_size_command);
			break;
		}
		/* IPR modify min fragment size Command Verification */
		case IPR_MODIFY_MODIFY_MIN_FRAG_SIZE_CMD_STR:
		{
			struct ipr_modify_min_frag_size_command *str =
				(struct ipr_modify_min_frag_size_command *) asa_seg_addr;
            if(str->ipr_instance_ref)
            	ipr_instance = 
            		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
            else 
            	ipr_instance = str->ipr_instance;

			str->status = ipr_modify_min_frag_size(
					ipr_instance,
					str->min_frag_size);
			str_size = sizeof(struct ipr_modify_min_frag_size_command);
			break;
		}
		/* IPR modify IPv4 TO value Command Verification */
		case IPR_MODIFY_TO_VALUE_IPV4_CMD_STR:
		{
			struct ipr_modify_timeout_value_ipv4_command *str =
				(struct ipr_modify_timeout_value_ipv4_command *) asa_seg_addr;
            if(str->ipr_instance_ref)
            	ipr_instance = 
            		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
            else 
            	ipr_instance = str->ipr_instance;

			str->status = ipr_modify_timeout_value_ipv4(
					ipr_instance,
					str->reasm_timeout_value_ipv4);
			str_size = sizeof(struct ipr_modify_timeout_value_ipv4_command);
			break;
		}
		/* IPR modify IPv6 TO value Command Verification */
		case IPR_MODIFY_TO_VALUE_IPV6_CMD_STR:
		{
			struct ipr_modify_timeout_value_ipv6_command *str =
				(struct ipr_modify_timeout_value_ipv6_command *) asa_seg_addr;
            if(str->ipr_instance_ref)
            	ipr_instance = 
            		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
            else 
            	ipr_instance = str->ipr_instance;

			str->status = ipr_modify_timeout_value_ipv6(
					ipr_instance,
					str->reasm_timeout_value_ipv6);
			str_size = sizeof(struct ipr_modify_timeout_value_ipv6_command);
			break;
		}
		/* IPR get reassembled frame counter Command Verification */
		case IPR_GET_REASS_FRM_CNTR_CMD_STR:
		{
			struct ipr_get_reass_frm_cntr_command *str =
				(struct ipr_get_reass_frm_cntr_command *) asa_seg_addr;
            if(str->ipr_instance_ref)
            	ipr_instance = 
            		(ipr_instance_handle_t)*((uint32_t *) str->ipr_instance);
            else 
            	ipr_instance = str->ipr_instance;

			str->status = ipr_get_reass_frm_cntr(
					ipr_instance,
					str->flags,
					str->reass_frm_cntr);
			str_size = sizeof(struct ipr_get_reass_frm_cntr_command);
			break;
		}
#endif
		default:
		{
			return STR_SIZE_ERR;
		}
	}


	return str_size;
}

void ipr_verif_update_frame(uint16_t iteration)
{
	struct ipv4hdr *iphdr;
	
	iphdr = ((struct ipv4hdr *)PARSER_GET_OUTER_IP_POINTER_DEFAULT());
	if(iteration > 2) {
		fdma_present_default_frame();
	
	/* calculate segment size + data offset + headers size*/
/*	seg_size = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	data_offset = (tcp->data_offset_reserved & 
				NET_HDR_FLD_TCP_DATA_OFFSET_MASK) >> 
				(NET_HDR_FLD_TCP_DATA_OFFSET_OFFSET - 
				 NET_HDR_FLD_TCP_DATA_OFFSET_SHIFT_VALUE);
	headers_size = (uint16_t)(PARSER_GET_L4_OFFSET_DEFAULT() + data_offset);
	tcp->sequence_number = sequence_number + seg_size - headers_size;
*/
	iphdr->flags_and_offset = (iphdr->flags_and_offset & 0xffC0) | 0x60;
	}
}
