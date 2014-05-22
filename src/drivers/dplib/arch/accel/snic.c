/**************************************************************************//**
@File		snic.c

@Description	This file contains the AIOP snic source code.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#include "snic.h"
#include "system.h"
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
#include "dplib/fsl_ipr.h"

#include "general.h"
#include "osm.h"

#include "dplib/fsl_ipf.h"
#include "common/fsl_cmdif_server.h"


extern __TASK struct aiop_default_task_params default_task_params;

__SHRAM struct snic_params *snic_params;

__SHRAM ipr_instance_handle_t ipr_instance_val;

__HOT_CODE void snic_process_packet(void)
{
	
	struct parse_result *pr;

	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM, 0);

	osm_task_init();
	/* todo: spid=0?, prpid=0?, starting HXS=0?*/
	*((uint8_t *)HWC_SPID_ADDRESS) = 0;
	default_task_params.parser_profile_id = 0;
	default_task_params.parser_starting_hxs = 0;
	default_task_params.qd_priority = ((*((uint8_t *)(HWC_ADC_ADDRESS + \
			ADC_WQID_PRI_OFFSET)) & ADC_WQID_MASK) >> 4);

	
	int32_t parse_status = parse_result_generate_default(PARSER_NO_FLAGS);
	if (parse_status)
		if (fdma_discard_default_frame(FDMA_DIS_WF_TC_BIT))
					fdma_terminate_task();
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
	int status;
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;

	UNUSED(dev);
	UNUSED(size);
	
	switch(cmd)
	{
	case SNIC_IPR_CREATE_INSTANCE:
		status = (int)ipr_create_instance((struct ipr_params*)data, 
				ipr_instance_ptr);
		ipr_instance_val = ipr_instance;
		return status;
	case SNIC_SET_MTU:
		snic_params += *((uint16_t *)data);
		data += 2;
		snic_params->snic_ipf_mtu = *((uint16_t *)data);
		return 0;
	case SNIC_ENABLE_FLAGS:
		snic_params += *((uint16_t *)data);
		data += 2;
		snic_params->snic_enable_flags = *((uint16_t *)data);
		return 0;
	case SNIC_SET_QDID:
		snic_params += *((uint16_t *)data);
		data += 2;
		snic_params->qdid = *((uint16_t *)data);
		return 0;
	case SNIC_GET_EPID_PC:
		*((uint32_t *)data) = (uint32_t)snic_process_packet;
		return 0;
	default:
		return -EINVAL;
	}
	return 0;
}

int aiop_snic_init(void)
{
	int status;
	struct cmdif_module_ops snic_cmd_ops;
	
	snic_cmd_ops.open_cb = (open_cb_t *)snic_open_cb;
	snic_cmd_ops.close_cb = (close_cb_t *)snic_close_cb;
	snic_cmd_ops.ctrl_cb = (ctrl_cb_t *)snic_ctrl_cb;
	fsl_os_print("SNIC: register with cmdif module!\n");
	status = cmdif_register_module("sNIC", &snic_cmd_ops);
	if(status) {
		fsl_os_print("SNIC:Failed to register with cmdif module!\n");
		return status;
	}
	return 0;
}
