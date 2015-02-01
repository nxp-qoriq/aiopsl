#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_platform.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"
#include "fsl_mc_init.h"
#include "fsl_dprc.h"
#include "aiop_common.h"
#include "sys.h"

#define CORE_ID_GET		(get_cpu_id() >> 4)

extern struct aiop_init_info g_init_data;

int aiop_mc_cmd_test();

int aiop_mc_cmd_test()
{
	uint32_t mc_portal_id = g_init_data.sl_info.mc_portal_id;
	uint64_t mc_portals_vaddr = g_init_data.sl_info.mc_portals_vaddr + \
		SOC_PERIPH_OFF_PORTALS_MC(mc_portal_id);
	void *p_vaddr = UINT_TO_PTR(mc_portals_vaddr);
	int container_id = -1;
	int err = 0;
	struct mc_dprc dprc;
	int i;

	/* Get container ID from MC */
	dprc.io.regs = p_vaddr;
	for (i = 0; i < 10; i++) {
		err = dprc_get_container_id(&dprc.io, &container_id);
		err |= err;
		if (container_id == -1)
			err |= -EINVAL;
	}

	return err;
}
