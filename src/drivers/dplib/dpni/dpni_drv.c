#include "common/types.h"
#include "common/dbg.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "common/io.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"
#include "kernel/platform.h"
#include "inc/sys.h"

#include "drv.h"


#define __ERR_MODULE__  MODULE_DPNI


int init_nic_stub(int portal_id, int ni_id);

int dpni_drv_probe(uint16_t	ni_id,
                   uint16_t	mc_portal_id,
                   fsl_handle_t	dpio,
                   fsl_handle_t	dpsp);

int dpni_drv_init(void);
void dpni_drv_free(void);


/* TODO - get rid */
__SHRAM struct dpni_drv *nis;


static void dflt_rx_cb(dpni_drv_app_arg_t arg)
{
	UNUSED(arg);
	/*if discard with terminate return with error then terminator*/
	if (fdma_discard_default_frame(FDMA_DIS_WF_TC_BIT))
		fdma_terminate_task();
}


int init_nic_stub(int portal_id, int ni_id)
{
	struct dpni_cfg			cfg;
	struct dpni_init_params		params;
	struct dpni			dpni;
	int 				err;
	uint8_t				eth_addr[] = {0x00, 0x04, 0x9f, 0x0, 0x0, 0x1};
#if 0
	dpni = dpni_open(UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
								       (uint32_t)portal_id,
								       E_MAPPED_MEM_TYPE_MC_PORTAL)),
			 ni_id);
	if (!dpni) {
		pr_err("failed to open DPNI!\n");
		return -ENODEV;
	}
#endif
	dpni.cidesc.regs = UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
									       (uint32_t)portal_id,
									       E_MAPPED_MEM_TYPE_MC_PORTAL));
	err = dpni_open(&dpni, ni_id);
	if (err) {
		pr_err("failed to open DPNI!\n");
		return -ENODEV;
	}
	/* obtain default configuration of the NIC */
	dpni_defconfig(&cfg);

	memset(&params, 0, sizeof(params));
	params.type = DPNI_TYPE_NIC;
	params.max_dpio_objs = 8; /* TODO - ??? */
	memcpy(params.mac_addr, eth_addr, sizeof(eth_addr));
	err = dpni_init(&dpni, &cfg, &params);
	if (err)
		return err;
	dpni_close(&dpni);

	return 0;
}


int dpni_drv_register_rx_cb (uint16_t		ni_id,
                             uint16_t		flow_id,
                             fsl_handle_t	dpio,
                             fsl_handle_t	dpsp,
                             rx_cb_t      	*cb,
                             dpni_drv_app_arg_t arg)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;

	if (dpio || dpsp) {
		pr_err("DPIO and\\or DPSP not supported yet\n");
		return -E_NOT_SUPPORTED;
	}
	dpni_drv->args[flow_id] = arg;
	dpni_drv->rx_cbs[flow_id] = cb;

	/* TODO - if passed DP-IO or DP-SP,
	 * call 'dpni_attach' with new args.
	 * also note to update the 'spid' if changed.
	 */

	return 0;
}

int dpni_drv_enable (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;
	int		err;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;

	if ((err = dpni_enable(dpni_drv->dpni)) != 0)
		return err;
	dpni_drv->flags |= DPNI_DRV_FLG_ENABLED;
	return 0;
}

int dpni_drv_disable (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;

	dpni_drv->flags &= ~DPNI_DRV_FLG_ENABLED;
	return dpni_disable(dpni_drv->dpni);
}

int dpni_drv_is_up (uint16_t ni_id)
{
	struct dpni_drv *dpni_drv;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;

	return !!(dpni_drv->flags & DPNI_DRV_FLG_ENABLED);
}


int dpni_drv_probe(uint16_t	ni_id,
                   uint16_t	mc_portal_id,
                   fsl_handle_t	dpio,
                   fsl_handle_t	dpsp)
{
	struct dpni_drv 		*dpni_drv;
	struct dpni_attach_params	params;
	int				err;

	/* calculate pointer to the send NI structure */
	dpni_drv = nis + ni_id;

#if 0
	dpni = dpni_open(UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
								       (uint32_t)portal_id,
								       E_MAPPED_MEM_TYPE_MC_PORTAL)),
			 ni_id);
	if (!dpni_drv->dpni) {
		pr_err("can't open DP-NI%d\n", ni_id);
		return -ENODEV;
	}

	dpni.cidesc.regs = UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
									       (uint32_t)portal_id,
									       E_MAPPED_MEM_TYPE_MC_PORTAL));
#endif
	err = dpni_open(dpni_drv->dpni, ni_id);
	if (err) {
		pr_err("can't open DP-NI%d\n", ni_id);
		return -ENODEV;
	}

	memset(&params, 0, sizeof(params));
	/* TODO - how to retrieve the ID here??? */
	params.dpio_id = (uint16_t)PTR_TO_UINT(dpio);
	/* TODO - how to retrieve the ID here??? */
	params.dpsp_id = (uint16_t)PTR_TO_UINT(dpsp);
	if ((err = dpni_attach(dpni_drv->dpni,
	                       (const struct dpni_attach_params *)&params)) != 0)
		return err;

	return 0;
}

int dpni_get_num_of_ni (void)
{
	/* TODO - complete here. should count the "real" number of NIs */
	return 1;
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
