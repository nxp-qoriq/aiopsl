#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "fsl_dpni.h"
#include "dplib/dpni_drv.h"
#include "dpni/drv.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_ip.h"
#include "fsl_cdma.h"
#include "common/fsl_slab.h"
#include "kernel/platform.h"
#include "io.h"
#include "aiop_common.h"

int app_init(void);
void app_free(void);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

__SHRAM struct slab *slab_peb = 0;
__SHRAM struct slab *slab_ddr = 0;

static int app_write_buff_and_release(struct slab *slab, uint64_t buff)
{
	uint64_t data1 = 0xAABBCCDD00001122;
	uint64_t data2 = 0;
	int      err = 0;

	err = cdma_write(buff, &data1, 8);
	if (err) return err;
	err = cdma_read(&data2, buff, 8);
	if (err) return err;

	if (data1 != data2) {
		slab_release(slab, buff);
		return -EPERM;
	}

	err = slab_release(slab, buff);
	if (err) return err;

	return 0;
}

static int app_test_slab(struct slab *slab, int num_times)
{
	uint64_t buff[3] = {0, 0, 0};
	int      err = 0;
	int      i = 0;

	for (i = 0; i < num_times; i++) {

		err = slab_acquire(slab, &buff[0]);
		if (err || (buff == NULL)) return -ENOMEM;
		err = slab_acquire(slab, &buff[1]);
		if (err || (buff == NULL)) return -ENOMEM;
		err = slab_acquire(slab, &buff[2]);
		if (err || (buff == NULL)) return -ENOMEM;

		err = app_write_buff_and_release(slab, buff[1]);
		if (err) return err;
		err = app_write_buff_and_release(slab, buff[2]);
		if (err) return err;
		err = app_write_buff_and_release(slab, buff[0]);
		if (err) return err;
	}

	return 0;
}

static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	int      err = 0;
	uint32_t src_addr = 0x10203040;// new ipv4 src_addr

	err = ip_set_nw_src(src_addr);
	if (err) fdma_terminate_task();

	err = app_test_slab(slab_peb, 4);
	if (err) fdma_terminate_task();

	err = app_test_slab(slab_ddr, 4);
	if (err) fdma_terminate_task();

	dpni_drv_send(APP_NI_GET(arg));
	fdma_terminate_task();
}

/* This is temporal WA for stand alone demo only */
#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,            \
	                                   0,                          \
	                                   E_MAPPED_MEM_TYPE_GEN_REGS) \
	                                   + SOC_PERIPH_OFF_AIOP_WRKS);
static int epid_setup()
{
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;

	/* EPID = 0 is saved for cmdif, need to set it for stand alone demo */
	iowrite32(0, &wrks_addr->epas); 
	iowrite32(0x00fe0000, &wrks_addr->ep_pc);
}

int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	dma_addr_t buff = 0;
	struct slab_debug_info slab_info;

	fsl_os_print("Running app_init()\n");

	/* This is temporal WA for stand alone demo only */
	epid_setup();
	
	for (ni = 0; ni < 6; ni++)
	{
		/* Every ni will have 1 flow */
		uint32_t flow_id = 0;
		err = dpni_drv_register_rx_cb((uint16_t)ni/*ni_id*/,
		                              (uint16_t)flow_id/*flow_id*/,
		                              app_process_packet_flow0, /* callback for flow_id*/
		                              (ni | (flow_id << 16)) /*arg, nic number*/);
		if (err) return err;
	}

	/* DDR SLAB creation */
	err = slab_create(10, 0, 256, 0, 0, 4, MEM_PART_1ST_DDR_NON_CACHEABLE, 0, NULL, &slab_ddr);
	if (err) return err;

	err = slab_debug_info_get(slab_ddr, &slab_info);
	if (err) {
		return err;
	} else {
		if ((slab_info.num_buffs != slab_info.max_buffs) || (slab_info.num_buffs == 0))
			return -ENODEV;
	}

	/* PEB SLAB creation */
	err = slab_create(5, 0, 100, 0, 0, 4, MEM_PART_PEB, 0, NULL, &slab_peb);
	if (err) return err;

	err = slab_debug_info_get(slab_peb, &slab_info);
	if (err) {
		return err;
	} else {
		if ((slab_info.num_buffs != slab_info.max_buffs) || (slab_info.num_buffs == 0))
			return -ENODEV;
	}

	return 0;
}

void app_free(void)
{
	int err = 0;
	/* TODO - complete!*/
	err = slab_free(slab_ddr);
	err = slab_free(slab_peb);
}
