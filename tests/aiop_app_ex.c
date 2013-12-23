#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "kernel/platform.h"
#include "dplib/fsl_dpni.h"


int aiop_app_init(void);
void aiop_app_free(void);


static int init_nic(int portal_id)
{
	struct dpni_cfg			cfg;
	struct dpni_init_params	params;
	fsl_handle_t			dpni;
	int 					err;

	/* obtain default configuration of the NIC */
	dpni_defconfig(&cfg);

	dpni = dpni_open(UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
								       (uint32_t)portal_id,
								       E_MAPPED_MEM_TYPE_MC_PORTAL)),
			 10);

	memset(&params, 0, sizeof(params));
	params.type = DPNI_TYPE_NIC;
	err = dpni_init(dpni, &cfg, &params);
	if (err)
		return err;

	return 0;
}


int aiop_app_init(void)
{
    int err = 0;

    fsl_os_print("AIOP test: NIC\n");

    err = init_nic(2);

    return err;
}

void aiop_app_free(void)
{
    /* TODO - complete!*/
}
