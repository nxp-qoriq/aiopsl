/**************************************************************************//**
@File		snic.c

@Description	This file contains the AIOP snic source code.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "snic.h"
#include "system.h"
#include "net/fsl_net.h"
#include "common/fsl_stdio.h"
#include "common/errors.h"
#include "kernel/platform.h"
#include "io.h"
#include "aiop_common.h"
#include "fsl_parser.h"
#include "general.h"
#include "dbg.h"

#include "dplib/fsl_parser.h"
#include "dplib/fsl_l2.h"
#include "dplib/fsl_fdma.h"

#include "general.h"
#include "osm.h"

#include "dplib/fsl_ipf.h"
#include "common/fsl_cmdif_server.h"


extern __TASK struct aiop_default_task_params default_task_params;

__SHRAM struct snic_params snic_params[MAX_SNIC_NO];

__HOT_CODE void snic_process_packet(void)
{
	
	struct parse_result *pr;
	uint8_t *fd_flc_appidx;
	uint8_t appidx;
	struct snic_params *snic;
	struct fdma_queueing_destination_params enqueue_params;
	int err;
	int32_t parse_status;

	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);

	osm_task_init();
	/* todo: spid=0?, prpid=0?, starting HXS=0?*/
	*((uint8_t *)HWC_SPID_ADDRESS) = SNIC_SPID;
	default_task_params.parser_profile_id = SNIC_PRPID;
	default_task_params.parser_starting_hxs = SNIC_HXS;
	default_task_params.qd_priority = ((*((uint8_t *)(HWC_ADC_ADDRESS + \
			ADC_WQID_PRI_OFFSET)) & ADC_WQID_MASK) >> 4);

	
	parse_status = parse_result_generate_default(PARSER_NO_FLAGS);
	if (parse_status)
		if (fdma_discard_default_frame(FDMA_DIS_WF_TC_BIT))
					fdma_terminate_task();

	/* check if this is ingress or egress */
	snic = snic_params + PRC_GET_PARAMETER();
	fd_flc_appidx = (uint8_t *)(HWC_FD_ADDRESS + FD_FLC_APPIDX_OFFSET);
	appidx = (*fd_flc_appidx >> 2);
	
	if (SNIC_IS_INGRESS(appidx)) {
		/* For ingress may need to do IPR and then Remove Vlan */
		if (snic->snic_enable_flags & SNIC_IPR_EN) 
			err = snic_ipr(snic);
		/*reach here if re-assembly success or regular or IPR disabled*/
		if (snic->snic_enable_flags & SNIC_VLAN_REMOVE_EN)
			l2_pop_vlan();
		
	}
	/* Egress*/
	else {
		/* For Egress may need to do add Vlan and then IPF */
		if (snic->snic_enable_flags & SNIC_VLAN_ADD_EN)
			snic_add_vlan();
		
		if (snic->snic_enable_flags & SNIC_IPF_EN)
			err = snic_ipf(snic);
	}
	
	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params.qdbin = 0;
	enqueue_params.qd = snic->qdid;
	enqueue_params.qd_priority = default_task_params.qd_priority;
	/* todo error cases */
	err = (int)fdma_store_and_enqueue_default_frame_qd(&enqueue_params, \
			FDMA_ENWF_NO_FLAGS);
	fdma_terminate_task();
}


/* Assuming IPF is the last iteration before enqueue (IPF after encryption)*/
int snic_ipf(struct snic_params *snic)
{
	uint16_t ip_offset;
	uint32_t total_length;
	struct ipv4hdr *ipv4_hdr;
	struct ipv6hdr *ipv6_hdr;
	struct fdma_amq amq;
	ipf_ctx_t ipf_context_addr
		__attribute__((aligned(sizeof(struct ldpaa_fd))));
	uint16_t icid;
	uint32_t flags = 0;
	uint8_t va_bdi;
	int32_t ipf_status;
	int err;
	struct fdma_queueing_destination_params enqueue_params;

	ip_offset = PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	ipv4_hdr = (struct ipv4hdr *)
			(ip_offset + PRC_GET_SEGMENT_ADDRESS());
	/* need to check frame size against MTU */
	if (PARSER_IS_OUTER_IPV6_DEFAULT())
	{
		ipv6_hdr = (struct ipv6hdr *)ipv4_hdr;
		total_length =
			(uint32_t)(ipv6_hdr->payload_length 
					+ 40);
	}
	else
		total_length = (uint32_t)ipv4_hdr->total_length;

	if (total_length > snic->snic_ipf_mtu)
	{
		icid = LH_SWAP(HWC_ADC_ADDRESS +
				ADC_PL_ICID_OFFSET, 0);
		icid &= ADC_ICID_MASK;
		va_bdi = *((uint8_t *)(HWC_ADC_ADDRESS +
				ADC_FDSRC_VA_FCA_BDI_OFFSET));
		if (va_bdi & ADC_BDI_MASK)
			flags |= FDMA_ENF_BDI_BIT;
		amq.flags = (uint16_t)(flags >> 16);
		amq.icid = icid;
		
		ipf_context_init(0, snic->snic_ipf_mtu,
				ipf_context_addr);

		do {
			ipf_status =
			ipf_generate_frag(ipf_context_addr);
			if (ipf_status)
				err =
				(int)fdma_store_frame_data(1, 0,
						&amq);
			else
				err =
				(int)fdma_store_frame_data(0, 0,
						&amq);
			
			/* for the enqueue set hash from TLS,
			 * an flags equal 0 meaning that
			 * the qd_priority is taken from the
			 * TLS and that enqueue function
			 * always returns*/
			enqueue_params.qdbin = 0;
			enqueue_params.qd = snic->qdid;
			enqueue_params.qd_priority =
				default_task_params.qd_priority;
			/* todo error cases */
			
			err =
			(int)fdma_enqueue_default_fd_qd(icid,
				flags, &enqueue_params);
			
		} while (ipf_status);
		
		fdma_terminate_task();
		return 0;
	}
	else
		return 0;
}

int snic_ipr(struct snic_params *snic)
{
	int32_t reassemble_status;

	reassemble_status = ipr_reassemble(snic->ipr_instance_val);
	if (reassemble_status != IPR_REASSEMBLY_REGULAR &&
		reassemble_status != IPR_REASSEMBLY_SUCCESS)
	{
		/* todo: error cases*/
		fdma_terminate_task();
		return 0;
	}
		
	else
		return 0;
}

int snic_add_vlan(void)
{
	uint32_t vlan;
	struct presentation_context *presentation_context;
	uint32_t asa_seg_addr;	/* ASA Segment Address */

	/* Get ASA pointer */
	presentation_context =
		(struct presentation_context *) HWC_PRC_ADDRESS;
	asa_seg_addr = (uint32_t)(presentation_context->
			asapa_asaps & PRC_ASAPA_MASK);
	vlan = *((uint32_t *)(PTR_MOVE(asa_seg_addr, 0x50)));
	l2_push_vlan((uint16_t)(vlan>>16));
	l2_set_vlan_vid((uint16_t)(vlan & VLAN_VID_MASK));
	l2_set_vlan_pcp((uint8_t)((vlan & VLAN_PCP_MASK) >>
			VLAN_PCP_SHIFT));
	return 0;
}

int snic_open_cb(void *dev)
{
	/* TODO: */
	UNUSED(dev);
	return 0;
}


int snic_close_cb(void *dev)
{
	/* TODO: */
	UNUSED(dev);
	return 0;
}

int snic_ctrl_cb(void *dev, uint16_t cmd, uint16_t size, uint8_t *data)
{
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;
	uint16_t snic_id;
	int i;

	UNUSED(dev);
	UNUSED(size);
	
	switch(cmd)
	{
	case SNIC_IPR_CREATE_INSTANCE:
		snic_id = *((uint16_t *)data);
		ipr_create_instance((struct ipr_params*)data, 
				ipr_instance_ptr);
		snic_params[snic_id].ipr_instance_val = ipr_instance;
		return 0;
	case SNIC_IPR_DELETE_INSTANCE:
		/* todo: parameters to ipr_delete_instance */
		snic_id = *((uint16_t *)data);
		ipr_delete_instance(snic_params[snic_id].ipr_instance_val,
				NULL, NULL);
		return 0;
	case SNIC_SET_MTU:
		snic_id = *((uint16_t *)data);
		data += 2;
		snic_params[snic_id].snic_ipf_mtu = *((uint16_t *)data);
		return 0;
	case SNIC_ENABLE_FLAGS:
		snic_id = *((uint16_t *)data);
		data += 2;
		snic_params[snic_id].snic_enable_flags = *((uint16_t *)data);
		return 0;
	case SNIC_SET_QDID:
		snic_id = *((uint16_t *)data);
		data += 2;
		snic_params[snic_id].qdid = *((uint16_t *)data);
		return 0;
	case SNIC_REGISTER:
		*((uint32_t *)data) = (uint32_t)snic_process_packet;
		data +=4;
		for (i=0; i < MAX_SNIC_NO; i++)
		{
			if (snic_params[i].valid)
			{
				snic_params[i].valid = FALSE;
				snic_id = (uint16_t)i;
				break;
			}
		}
		if (i== MAX_SNIC_NO)
			*((uint16_t *)data) = 0xFFFF;
		else
			*((uint16_t *)data) = snic_id;
		return 0;
	case SNIC_UNREGISTER:
		snic_id = *((uint16_t *)data);
		snic_params[snic_id].valid = TRUE;
		return 0;
	default:
		return -EINVAL;
	}
	return 0;
}

int aiop_snic_init(void)
{
	int status, i;
	struct cmdif_module_ops snic_cmd_ops;
	
	snic_cmd_ops.open_cb = (open_cb_t *)snic_open_cb;
	snic_cmd_ops.close_cb = (close_cb_t *)snic_close_cb;
	snic_cmd_ops.ctrl_cb = (ctrl_cb_t *)snic_ctrl_cb;
	pr_info("sNIC: register with cmdif module!\n");
	status = cmdif_register_module("sNIC", &snic_cmd_ops);
	if(status) {
		pr_info("sNIC:Failed to register with cmdif module!\n");
		return status;
	}
	for (i=0; i < MAX_SNIC_NO; i++)
		snic_params[i].valid = TRUE;
	return 0;
}
