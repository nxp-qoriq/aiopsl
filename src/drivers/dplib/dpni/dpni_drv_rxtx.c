#include "general.h"
#include "common/types.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"

#include "drv.h"


#define __ERR_MODULE__  MODULE_DPNI


extern __TASK uint8_t CURRENT_SCOPE_LEVEL;
extern __TASK uint8_t SCOPE_MODE_LEVEL1;
extern __TASK uint8_t SCOPE_MODE_LEVEL2;
extern __TASK uint8_t SCOPE_MODE_LEVEL3;
extern __TASK uint8_t SCOPE_MODE_LEVEL4;

extern __TASK struct aiop_default_task_params default_task_params;


/* TODO - get rid */
extern __SHRAM struct dpni_drv *nis;


static void osm_task_init(void)
{
	CURRENT_SCOPE_LEVEL = ((uint8_t)PRC_GET_OSM_SOURCE_VALUE());
		/**<	0- No order scope specified.\n
			1- Scope was specified for level 1 of hierarchy */
	SCOPE_MODE_LEVEL1 = ((uint8_t)PRC_GET_OSM_EXECUTION_PHASE_VALUE());
		/**<	0 = Exclusive mode.\n
			1 = Concurrent mode. */
	SCOPE_MODE_LEVEL2 = 0x00;
		/**<	Exclusive (default) Mode in level 2 of hierarchy */
	SCOPE_MODE_LEVEL3 = 0x00;
		/**<	Exclusive (default) Mode in level 3 of hierarchy */
	SCOPE_MODE_LEVEL4 = 0x00;
		/**<	Exclusive (default) Mode in level 4 of hierarchy */
}


__HOT_CODE void receive_cb (void)
{
	struct dpni_drv *dpni_drv = (struct dpni_drv *)PRC_GET_PARAMETER();
	uint8_t *fd_err = (uint8_t *)(HWC_FD_ADDRESS + FD_ERR_OFFSET);
	uint8_t *fd_flc_appidx = (uint8_t *)(HWC_FD_ADDRESS + \
			FD_FLC_APPIDX_OFFSET);
	uint8_t appidx;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* check if NI is enabled and there are no errors to discard and
	 * application call-back is not NULL */
	if (!(dpni_drv->flags & DPNI_DRV_FLG_ENABLED) ||
			(*fd_err & dpni_drv->fd_err_mask) ||
			!dpni_drv->rx_cbs[*fd_flc_appidx >> 2]) {
		/*if discard with terminate return with error then terminator*/
		if(fdma_discard_default_frame(FDMA_DIS_WF_TC_BIT))
			fdma_terminate_task();
	}
	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM);

	osm_task_init();
	*((uint8_t *)HWC_SPID_ADDRESS) = dpni_drv->spid;
	default_task_params.parser_profile_id = dpni_drv->prpid;
	default_task_params.parser_starting_hxs \
			= dpni_drv->starting_hxs;
	default_task_params.qd_priority = ((*((uint8_t *)ADC_WQID_PRI_OFFSET) \
			& ADC_WQID_MASK) >> 4);
	default_task_params.hash_value = 0;

	if (dpni_drv->flags & DPNI_DRV_FLG_PARSE) {
		int32_t parse_status = parse_result_generate_default \
				(PARSER_NO_FLAGS);
	/* TODO in future releases it may be enough to check only
	 * parse_status */	
		if (parse_status || PARSER_IS_PARSING_ERROR_DEFAULT()) {
			if (dpni_drv->flags & DPNI_DRV_FLG_PARSER_DIS) {
				/* if discard with terminate return with error \
				 * then terminator */
				if(fdma_discard_default_frame\
						(FDMA_DIS_WF_TC_BIT))
					fdma_terminate_task();
			}
		}
	}

	appidx = (*fd_flc_appidx >> 2);
	dpni_drv->rx_cbs[appidx](dpni_drv->args[appidx]);
}

__HOT_CODE int dpni_drv_send(uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	struct fdma_queueing_destination_params    enqueue_params;
	int err;

	dpni_drv = nis + ni_id; /* calculate pointer
					* to the send NI structure   */

	/* check if NI is enabled */
	if (!(dpni_drv->flags & DPNI_DRV_FLG_ENABLED))
			return(DPNI_DRV_NI_DIS);

	if (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) > dpni_drv->mtu) {
		if (dpni_drv->flags & DPNI_DRV_FLG_MTU_DIS)
			return(DPNI_DRV_MTU_ERR);
		else {
			/* TODO - mark in the FLC some error indication */
			uint32_t frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
			frc |= FD_FRC_DPNI_MTU_ERROR_CODE;
			LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, frc);
		}
	}
	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params.hash_value = default_task_params.hash_value;
	enqueue_params.qd = dpni_drv->qdid;
	enqueue_params.qd_priority = default_task_params.qd_priority;
	err = (int)fdma_store_and_enqueue_default_frame_qd(&enqueue_params, \
			FDMA_ENWF_NO_FLAGS);
	return (err);
}
