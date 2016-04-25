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

#include "fsl_dbg.h"
#include "fsl_system.h"
#include "fsl_general.h"
#include "fsl_sl_dprc_drv.h"
#include "fsl_string.h"
#include "fsl_aiop_common.h"
#include "fsl_dprc.h"
#include "icontext.h"
#include "system.h"



extern struct aiop_init_info g_init_data;
extern t_system sys;
extern const uint8_t AIOP_DDR_START[], AIOP_DDR_END[];
extern const uint32_t  g_boot_mem_mng_size;

int __init();
int __dprc_drv_init();
int __dpni_drv_init();
int __dpni_drv_scan();
int __get_container_id(int* container_id);

// replacement for fsl_get_mem()
static uint64_t g_curr_ptr[] = {0, 0, 0};

int __init()
{
	int err = 0;

	// sys_init()
	icontext_init();

	// sys_init() -> global_sys_init(void)
	uintptr_t   aiop_base_addr;
	struct aiop_tile_regs *tile_regs;

	aiop_base_addr = AIOP_PERIPHERALS_OFF + SOC_PERIPH_OFF_AIOP_TILE;
	err = sys_add_handle( (void *)aiop_base_addr, FSL_MOD_AIOP_TILE, 1, 0);
	if (err != 0) return err;
	tile_regs = (struct aiop_tile_regs *)aiop_base_addr;
	err = sys_add_handle( (void *)&tile_regs->cmgw_regs, FSL_MOD_CMGW, 1, 0);
	if (err != 0) return err;

	// sys_init_platform() -> pltfrm_init_mem_partitions_cb() -> build_mem_partitions_table()
	uint32_t aiop_lcf_ddr_size = (uint32_t)(AIOP_DDR_END) - (uint32_t)(AIOP_DDR_START);
	if (g_init_data.app_info.dp_ddr_size) {
		// MEM_PART_DP_DDR
		g_curr_ptr[0] = g_init_data.sl_info.dp_ddr_paddr + aiop_lcf_ddr_size + g_boot_mem_mng_size;

		// MEM_PART_SYSTEM_DDR
		g_curr_ptr[1] = g_init_data.sl_info.sys_ddr1_paddr;
	} else {
		// MEM_PART_DP_DDR
		g_curr_ptr[0] = NULL;

		// MEM_PART_SYSTEM_DDR
		g_curr_ptr[1] = g_init_data.sl_info.sys_ddr1_paddr + aiop_lcf_ddr_size + g_boot_mem_mng_size;
	}
	// MEM_PART_PEB
	g_curr_ptr[2] = g_init_data.sl_info.peb_paddr;

	return err;
}

static int fsl_get_mem__(uint64_t size, int mem_partition_id, uint64_t alignment, uint64_t* paddr)
{
	UNUSED(alignment);

	int id = 0;
	switch(mem_partition_id) {
		case MEM_PART_DP_DDR:
			id = 0;
			break;
		case MEM_PART_SYSTEM_DDR:
			id = 1;
			break;
		case MEM_PART_PEB:
			id = 2;
			break;
		default: return -EINVAL;
	}
	*paddr = g_curr_ptr[id];
	g_curr_ptr[id] += size;
	return 0;
}
#define fsl_get_mem fsl_get_mem__

int bman_fill_bpid__(uint32_t num_buffs,
                     uint16_t buff_size,
                     uint16_t alignment,
                     enum memory_partition_id  mem_partition_id,
                     uint16_t bpid,
                     uint16_t alignment_extension);
#define bman_fill_bpid bman_fill_bpid__
#include "../../../src/drivers/qbman/bman.c"

static int evmng_sl_raise_event__(uint8_t generator_id, uint8_t event_id, void *event_data)
{
	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(event_data);
	return 0;
}

#define nis nis__
#define num_of_nis num_of_nis__
#define pools_params pools_params__
#define g_dpni_early_init_data g_dpni_early_init_data__
#define dpni_drv_init_ni_table dpni_drv_init_ni_table__
#define configure_bpids_for_dpni configure_bpids_for_dpni__
#define dpni_drv_update_obj dpni_drv_update_obj__
#define dpni_drv_is_dpni_exist dpni_drv_is_dpni_exist__
#define dpni_drv_probe dpni_drv_probe__
#define initialize_dpni initialize_dpni__
#define configure_dpni_params configure_dpni_params__
#define evmng_sl_raise_event evmng_sl_raise_event__
#undef __ERR_MODULE__
#include "../../../src/drivers/dplib/dpni/dpni_drv.c"


static struct mc_dprc g_dprc = {0};
int __get_container_id(int* container_id)
{
	// global_init() -> dprc_drv_init() -> aiop_container_init()
	int err = 0;
	uint32_t mc_portal_id = g_init_data.sl_info.mc_portal_id;
	uint64_t mc_portals_vaddr = g_init_data.sl_info.mc_portals_vaddr + SOC_PERIPH_OFF_PORTALS_MC(mc_portal_id);
	void *p_vaddr = UINT_TO_PTR(mc_portals_vaddr);
	struct mc_dprc *dprc = &g_dprc;

	*container_id = -1;

	/* Get container ID from MC */
	dprc->io.regs = p_vaddr;
	err = dprc_get_container_id(&dprc->io, 0, container_id);
	if (err || (*container_id == -1)) {
		pr_err("Failed to get AIOP container ID err = %d\n", err);
		return err;
	}
	return err;
}

int __dprc_drv_init()
{
	// global_init() -> dprc_drv_init() -> aiop_container_init()
	int err = 0;
	struct mc_dprc *dprc = &g_dprc;
	int container_id = -1;

	err = __get_container_id(&container_id);
	if (err) {
		return err;
	}
	err = dprc_open(&dprc->io, 0, container_id, &dprc->token);
	if (err) {
		pr_err("Failed to open AIOP root container DP-RC%d.\n",
		       container_id);
		return err;
	}
	err = sys_add_handle(dprc, FSL_MOD_AIOP_RC, 1, 0);
	return err;
}

struct dpni_drv g_nis_table[SOC_MAX_NUM_OF_DPNI];

int __dpni_drv_init()
{
	// global_init() -> dpni_drv_init()
	int err = 0;

	memset(&pools_params__, 0, sizeof(struct dpni_pools_cfg));
	num_of_nis__ = 0;
	/* Allocate internal AIOP NI table */
	nis__ = &g_nis_table[0];
	if (!nis__) {
		return -ENOMEM;
	}

	/* Initialize internal AIOP NI table */
	dpni_drv_init_ni_table__(1);

	err = configure_bpids_for_dpni__();
	if (err) {
		pr_err("configure_bpids_for_dpni failed %d\n",err);
		return err;
	}
	return err;
}

int __dpni_drv_scan()
{
	// global_post_init() -> dprc_drv_scan() -> dpni_drv_update_obj();
	int i, err, dev_count = 0;
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_MOD_AIOP_RC);

	err = dprc_get_obj_count(&dprc->io, 0, dprc->token, &dev_count);
	if (err) {
		sl_pr_err("Failed to get device count for AIOP RC auth_id = %d.\n",
		          dprc->token);
		return err;
	}
	sl_pr_debug("Num of objects found: %d\n",dev_count);
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, 0, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpni") == 0) {
			sl_pr_debug("DPNI %d found in the container\n",dev_desc.id);

			err = dpni_drv_update_obj__(dprc,
			                          (uint16_t)dev_desc.id);
			if (err) {
				sl_pr_err("Failed to update DPNI %d.\n",
				          dev_desc.id);
				return err;
			}
		}
	}
	return 0;
}

int __configure_bpids();
int __configure_bpids()
{
	// global_init() -> dpni_drv_init() -> configure_bpids_for_dpni() (static function)
	return configure_bpids_for_dpni__();
}


/*****************************************************************************/
int dpbp_init();
int dpbp_test();

int dpbp_init()
{
	int err = 0;

	err = __init();
	if (err)
	{
		return err;
	}
	return __dprc_drv_init();
}

int dpbp_test()
{
	return __configure_bpids();
}

