/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "common/fsl_string.h"
#include "inc/fsl_sys.h"
#include "fsl_dbg.h"
#include "apps.h"
#include "fsl_dprc.h"
#include "fsl_dpci_drv.h"
#include "fsl_dpci_event.h"
#include "fsl_dpci_event.h"
#include "fsl_mc_init.h"

int dpci_scan_and_enable();

extern int app_init(void); extern void app_free(void);


#define APPS                            	\
{                                       	\
	{NULL, app_init, app_free},		\
	{NULL, NULL, NULL} /* never remove! */	\
}

void build_apps_array(struct sys_module_desc *apps);

void build_apps_array(struct sys_module_desc *apps)
{
	struct sys_module_desc apps_tmp[] = APPS;
	
	ASSERT_COND(ARRAY_SIZE(apps_tmp) <= APP_INIT_APP_MAX_NUM);
	memcpy(apps, apps_tmp, sizeof(apps_tmp));
}

/*
 * TODO this is needed only until EVM will be implemented
 * dpci_event_assign - will be triggered by AIOP SL EVM
 * dpci_drv_enable - will be triggered by AIOP APP
 */
int dpci_scan_and_enable()
{
	int ind = 0;
	int i   = 0;
	int err = 0;
	int dev_count = 0;
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	if (dprc == NULL) {
		pr_err("No AIOP root container \n");
		return -ENODEV;
	}

	if ((err = dprc_get_obj_count(&dprc->io, dprc->token, &dev_count)) != 0) {
		pr_err("Failed to get device count for RC auth_d = %d\n",
		       dprc->token);
		return err;
	}

	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {
			pr_debug(" Found DPCI device\n");
			pr_debug("***********\n");
			pr_debug("vendor - %x\n", dev_desc.vendor);
			pr_debug("type - %s\n", dev_desc.type);
			pr_debug("id - %d\n", dev_desc.id);
			pr_debug("region_count - %d\n", dev_desc.region_count);
			pr_debug("state - %d\n", dev_desc.state);
			pr_debug("ver_major - %d\n", dev_desc.ver_major);
			pr_debug("ver_minor - %d\n", dev_desc.ver_minor);
			pr_debug("irq_count - %d\n\n", dev_desc.irq_count);
			err |= dpci_event_assign((uint32_t)dev_desc.id);
			err |= dpci_drv_enable((uint32_t)dev_desc.id);
		}
	}
		
	return err;
}
