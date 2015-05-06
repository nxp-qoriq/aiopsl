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

#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "general.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "fsl_dprc.h"
#include "fsl_dpci.h"
#include "fsl_mc_init.h"
#include "fsl_platform.h"
#include "cmdif_srv.h"
#include "fsl_dpci_event.h"
#include "fsl_dpci_drv.h"
#include "fsl_spinlock.h"
#include "fsl_malloc.h"

int mc_obj_init();
void mc_obj_free();

extern struct aiop_init_info g_init_data;

__COLD_CODE static int aiop_container_init()
{
	void *p_vaddr;
	int err = 0;
	int container_id;
	/*struct mc_dprc *dprc = fsl_os_xmalloc(sizeof(struct mc_dprc),
					MEM_PART_SH_RAM,
					1);*/
	struct mc_dprc *dprc = fsl_malloc(sizeof(struct mc_dprc),
						1);
	if (dprc == NULL) {
		pr_err("No memory for AIOP Root Container \n");
		return -ENOMEM;
	}
	memset(dprc, 0, sizeof(struct mc_dprc));

	/* TODO : in this call, can 3rd argument be zero? */
	/* Get virtual address of MC portal */
	p_vaddr = UINT_TO_PTR(((uintptr_t)sys_get_handle(FSL_OS_MOD_MC_PORTAL, 0))
			+ SOC_PERIPH_OFF_PORTALS_MC(g_init_data.sl_info.mc_portal_id));

	pr_debug("MC portal ID[%d] addr = 0x%x\n", g_init_data.sl_info.mc_portal_id, (uint32_t)p_vaddr);

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

	err = sys_add_handle(dprc, FSL_OS_MOD_AIOP_RC, 1, 0);
	return err;
}

__COLD_CODE static void aiop_container_free()
{
	void *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);

	sys_remove_handle(FSL_OS_MOD_AIOP_RC, 0);

	if (dprc != NULL)
		fsl_free(dprc);
}


__COLD_CODE int mc_obj_init()
{
	int err = 0;

	err |= aiop_container_init();
	return err;

}

__COLD_CODE void mc_obj_free()
{
	aiop_container_free();
	/* TODO DPCI close ???
	 * TODO DPRC close */
}
