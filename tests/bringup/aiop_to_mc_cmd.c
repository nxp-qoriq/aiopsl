#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "bringup_tests.h"
#include "fsl_soc.h"

#if (TEST_AIOP_MC_CMD == ON)

extern struct aiop_init_info g_init_data;
struct mc_dprc dprc = {0};

int aiop_mc_cmd_init()
{
	uint32_t mc_portal_id = g_init_data.sl_info.mc_portal_id;
	uint64_t mc_portals_vaddr = g_init_data.sl_info.mc_portals_vaddr + \
		SOC_PERIPH_OFF_PORTALS_MC(mc_portal_id);
	void *p_vaddr = UINT_TO_PTR(mc_portals_vaddr);
	int container_id = 0;

	if (sys_is_master_core()) {
		pr_debug("MC portal ID[%d] addr = 0x%x\n", 
		         mc_portal_id, 
		         (uint32_t)p_vaddr);

		/* Open root container in order to create and query for devices */
		dprc->io.regs = p_vaddr;
		if ((err = dprc_get_container_id(&dprc->io, &container_id)) != 0) {
			pr_err("Failed to get AIOP root container ID.\n");
			return err;
		}
		if ((err = dprc_open(&dprc->io, container_id, &dprc->token)) != 0) {
			pr_err("Failed to open AIOP root container DP-RC%d.\n",
			       container_id);
			return err;
		}
	}
	
	do {} while(!dprc.token);
	
	return 0;
}

int aiop_mc_cmd_test()
{
	int err = 0;
	
	err = dprc_close(&dprc.io, dprc.token);
		
	return err;
}

#endif /* (TEST_AIOP_MC_CMD == ON) */
