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

#include "fsl_errors.h"
#include "fsl_io.h"
#include "fsl_malloc.h"
#include "common/types.h"
#include "common/fsl_string.h"
#include "fsl_soc.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_dprc.h"
#include "fsl_aiop_common.h"

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
		err = dprc_get_container_id(&dprc.io, 0, &container_id);
		err |= err;
		if (container_id == -1)
			err |= -EINVAL;
	}

	return err;
}
