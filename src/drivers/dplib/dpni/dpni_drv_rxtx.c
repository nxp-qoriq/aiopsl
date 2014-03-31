#include "general.h"
#include "common/types.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "osm.h"

#include "drv.h"


#define __ERR_MODULE__  MODULE_DPNI


extern __TASK struct aiop_default_task_params default_task_params;

/* TODO - get rid */
extern __SHRAM struct dpni_drv *nis;


__HOT_CODE void receive_cb(void)
{
	struct dpni_drv *dpni_drv;
	uint8_t *fd_flc_appidx;
	uint8_t appidx;
	struct parse_result *pr;

	dpni_drv = nis + PRC_GET_PARAMETER(); /* calculate pointer
						* to the send NI structure   */
	fd_flc_appidx = (uint8_t *)(HWC_FD_ADDRESS + FD_FLC_APPIDX_OFFSET);
	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	/* Need to save running-sum in parse-results LE-> BE */
	pr->gross_running_sum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_RUNNING_SUM);

	osm_task_init();
	*((uint8_t *)HWC_SPID_ADDRESS) = dpni_drv->spid;
	default_task_params.parser_profile_id = dpni_drv->prpid;
	default_task_params.parser_starting_hxs \
			= dpni_drv->starting_hxs;
	default_task_params.qd_priority = ((*((uint8_t *)(HWC_ADC_ADDRESS + \
			ADC_WQID_PRI_OFFSET)) & ADC_WQID_MASK) >> 4);

	if (dpni_drv->flags & DPNI_DRV_FLG_PARSE) {
		int32_t parse_status = parse_result_generate_default \
				(PARSER_NO_FLAGS);
		if (parse_status) {
			if (dpni_drv->flags & DPNI_DRV_FLG_PARSER_DIS) {
				/* if discard with terminate return with error \
				 * then terminator */
				if (fdma_discard_default_frame\
						(FDMA_DIS_WF_TC_BIT))
					fdma_terminate_task();
			}
		}
	}

	appidx = (*fd_flc_appidx >> 2);
	dpni_drv->rx_cbs[appidx](dpni_drv->args[appidx]);
	fdma_terminate_task();
}

__HOT_CODE int dpni_drv_send(uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	struct fdma_queueing_destination_params    enqueue_params;
	int err;

	dpni_drv = nis + ni_id; /* calculate pointer
					* to the send NI structure   */

	if ((dpni_drv->flags & DPNI_DRV_FLG_MTU_ENABLE) &&
		(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) > dpni_drv->mtu))
			return DPNI_DRV_MTU_ERR;
	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params.qdbin = 0;
	enqueue_params.qd = dpni_drv->qdid;
	enqueue_params.qd_priority = default_task_params.qd_priority;
	err = (int)fdma_store_and_enqueue_default_frame_qd(&enqueue_params, \
			FDMA_ENWF_NO_FLAGS);
	return err;
}

__HOT_CODE int dpni_drv_explicit_send(uint16_t ni_id, struct ldpaa_fd *fd)
{
	struct dpni_drv *dpni_drv;
	struct fdma_queueing_destination_params    enqueue_params;
	int err;
	uint32_t flags = 0;
	uint16_t icid;
	uint8_t va_bdi;

	dpni_drv = nis + ni_id; /* calculate pointer
					* to the send NI structure   */

	if ((dpni_drv->flags & DPNI_DRV_FLG_MTU_ENABLE) &&
		(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) > dpni_drv->mtu))
			return DPNI_DRV_MTU_ERR;
	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params.qdbin = 0;
	enqueue_params.qd = dpni_drv->qdid;
	enqueue_params.qd_priority = default_task_params.qd_priority;
	/* Assuming user already called fdma_create_frame() and saved fd in the
	 *  TLS */
	/* TODO maybe in future HW the fdma_enqueue_fd_qd command will support
	 * taking ICID and relevant bits from default values */
	va_bdi = *((uint8_t *)(HWC_ADC_ADDRESS + ADC_FDSRC_VA_FCA_BDI_OFFSET));
	if (va_bdi & ADC_BDI_MASK)
		flags |= FDMA_ENF_BDI_BIT;
	if (va_bdi & ADC_VA_MASK)
		flags |= FDMA_ENF_VA_BIT;
	icid = LH_SWAP(HWC_ADC_ADDRESS + ADC_PL_ICID_OFFSET);
	if (icid & ADC_PL_MASK)
		flags |= FDMA_ENF_PL_BIT;
	icid &= ADC_ICID_MASK;
	err = (int)fdma_enqueue_fd_qd(fd, flags, &enqueue_params, icid);
	return err;
}

/* TODO : replace by macros/inline funcs */
__HOT_CODE int dpni_get_receive_niid(void)
{
	return((int)PRC_GET_PARAMETER());
}


/* TODO : replace by macros/inline funcs */
__HOT_CODE int dpni_set_send_niid(uint16_t niid)
{
	default_task_params.send_niid = niid;
	return 0;
}


/* TODO : replace by macros/inline funcs */
__HOT_CODE int dpni_get_send_niid(void)
{
	return((int)default_task_params.send_niid);
}

