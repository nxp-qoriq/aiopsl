#include "general.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "common/io.h"
#include "dplib/dpni_drv.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "kernel/platform.h"
#include "inc/sys.h"


#define __ERR_MODULE__  MODULE_DPNI

#define DPNI_DRV_FLG_ENABLED	0x80
#define DPNI_DRV_FLG_PARSE	0x40
#define DPNI_DRV_FLG_MTU_DIS	0x20
#define DPNI_DRV_FLG_PARSER_DIS	0x01


void receive_cb (void);

int dpni_drv_init(void);
void dpni_drv_free(void);


struct dpni_drv {
	/** network interface ID */
	uint16_t            id;
	/** Storage profile ID */
	uint8_t             spid;
	uint8_t             res0[1];
	/** Queueing destination for the enqueue. */
	uint16_t            qdid;
	/** starting HXS */
	uint16_t            starting_hxs;
	/** MTU value needed for the \ref dpni_drv_send() function */
	uint32_t            mtu;
	/** Parse Profile ID */
	uint8_t             prpid;
	/** \ref DPNI_DRV_DEFINES */
	uint8_t             flags;
	/** error mask for the \ref receive_cb() function FD
	* error check 0 - continue; 1 - discard */
	uint8_t             fd_err_mask;
	uint8_t             res[1];

	/** call back application function */
	rx_cb_t             *rx_cbs[DPNI_DRV_MAX_NUM_FLOWS];
	/** call back application argument */
	dpni_drv_app_arg_t  args[DPNI_DRV_MAX_NUM_FLOWS];
};

extern __TASK uint8_t CURRENT_SCOPE_LEVEL;
extern __TASK uint8_t SCOPE_MODE_LEVEL1;
extern __TASK uint8_t SCOPE_MODE_LEVEL2;
extern __TASK uint8_t SCOPE_MODE_LEVEL3;
extern __TASK uint8_t SCOPE_MODE_LEVEL4;

extern __TASK struct aiop_default_task_params default_task_params;


/* TODO - get rid */
__SHRAM struct dpni_drv *nis;


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

static void dflt_rx_cb(dpni_drv_app_arg_t arg)
{
	UNUSED(arg);
	/*if discard with terminate return with error then terminator*/
	if(fdma_discard_default_frame(FDMA_DIS_WF_TC_BIT))
		fdma_terminate_task();
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

	if (dpni_drv->flags & DPNI_DRV_FLG_PARSE) {
		int32_t parse_status = parse_result_generate_default \
				(PARSER_NO_FLAGS);
		if (parse_status || PARSER_IS_PARSING_ERROR_DEFAULT()) {
			if (dpni_drv->flags & DPNI_DRV_FLG_PARSER_DIS) {
				/* if discard with terminate return with error \
				 * then terminator */
				if(fdma_discard_default_frame\
						(FDMA_DIS_WF_TC_BIT))
					fdma_terminate_task();
			}
			if (parse_status)
				default_task_params.parser_status \
				= parse_status;
			else
				default_task_params.parser_status \
				= PARSER_GET_PARSE_ERROR_CODE_DEFAULT();
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
	/* for the enqueue set hash=0, an flags equal 0 meaning that the \
	 * qd_priority is taken from the TLS and that enqueue function   \
	 * always returns*/
	enqueue_params.hash_value = 0;
	enqueue_params.qd = dpni_drv->qdid;
	enqueue_params.qd_priority = default_task_params.qd_priority;
	err = (int)fdma_store_and_enqueue_default_frame_qd(&enqueue_params, \
			FDMA_ENWF_NO_FLAGS);
	return (err);
}

int dpni_drv_register_rx_cb (uint16_t     	ni_id,
                             uint16_t     	flow_id,
                             rx_cb_t      	*cb,
                             dpni_drv_app_arg_t arg)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;
	dpni_drv->rx_cbs[flow_id] = cb;
	dpni_drv->args[flow_id] = arg;

	return E_OK;
}


int dpni_drv_init(void)
{
	uintptr_t	wrks_addr;
	int		i;

	nis = fsl_os_malloc_smart(sizeof(struct dpni_drv)*SOC_MAX_NUM_OF_DPNI,
				  MEM_PART_SH_RAM,
				  64);
	if (!nis)
		RETURN_ERROR(MAJOR, E_NO_MEMORY, ("NI objs!"));
	memset(nis, 0, sizeof(struct dpni_drv)*SOC_MAX_NUM_OF_DPNI);

	wrks_addr =
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,
					   0,
					   E_MAPPED_MEM_TYPE_GEN_REGS) +
	 SOC_PERIPH_OFF_AIOP_WRKS);

	/* Write EPID-table parameters */
	for (i=255; i>=(256-SOC_MAX_NUM_OF_DPNI); i--) {
		struct dpni_drv *dpni_drv = nis + i;
		int		j;

		dpni_drv->id = (uint16_t)i;
		/* put a default RX callback - dropping the frame */
		for (j=0; j<DPNI_DRV_MAX_NUM_FLOWS; j++)
			dpni_drv->rx_cbs[j] = dflt_rx_cb;

		/* EPAS reg  - write to EPID 'i' */
		iowrite32be((uint32_t)i, UINT_TO_PTR(wrks_addr + 0x0f8));
		/* EP_PC */
		iowrite32be(PTR_TO_UINT(receive_cb), UINT_TO_PTR(wrks_addr + 0x100));
		/* EP_PM */
		iowrite32be(PTR_TO_UINT(nis + i), UINT_TO_PTR(wrks_addr + 0x100));
	}

	return 0;
}

void dpni_drv_free(void)
{
	if (nis)
		fsl_os_free_smart(nis);
	nis = NULL;
}
