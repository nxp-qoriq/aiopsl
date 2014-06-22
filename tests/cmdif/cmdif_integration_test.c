#include "common/types.h"
#include "common/fsl_stdio.h"
#include "dpni/drv.h"
#include "fsl_ip.h"
#include "kernel/platform.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"
#include "fsl_cmdif_server.h"
#include "fsl_cmdif_client.h"
#include "dplib/fsl_cdma.h"

int app_init(void);
void app_free(void);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

__SHRAM struct cmdif_desc cidesc;
uint8_t send_data[64];

__HOT_CODE static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	int      err = 0;
	fsl_os_print("Core %d received packet\n", core_get_id());
	err = dpni_drv_send(APP_NI_GET(arg));
}

static int async_cb(void *async_ctx, int err, uint16_t cmd_id,
             uint32_t size, uint64_t data)
{
	UNUSED(cmd_id);
	UNUSED(async_ctx);
	fsl_os_print("ASYNC CB data high = 0x%x low = 0x%x size = 0x%x\n",
	         (uint32_t)((data & 0xFF00000000) >> 32),
	         (uint32_t)(data & 0xFFFFFFFF), size);
	if (err != 0) {
		fsl_os_print("ERROR inside async_cb\n");
	}
	return err;
}

static int open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev);
	fsl_os_print("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	fsl_os_print("close_cb\n");
	return 0;
}

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size, uint64_t data)
{
	int err = 0;
	UNUSED(dev);
	fsl_os_print("ctrl_cb cmd = 0x%x, size = %d, data high= 0x%x data low= 0x%x\n",
	             cmd,
	             size,
	             (uint32_t)((data & 0xFF00000000) >> 32),
	             (uint32_t)(data & 0xFFFFFFFF));

	switch (cmd & ~CMDIF_NORESP_CMD) {
	case 0x100:
		cidesc.regs = 0; /* DPCI 0 is used by MC */
		err = cmdif_open(&cidesc, "IRA", 0, async_cb, cidesc.regs,
		                 NULL, NULL, 0);
		break;
	case 0x101:
		uint64_t p_data = fsl_os_virt_to_phys(&send_data[0]);
		/* Must be no response because it's on the same dpci */
		err = cmdif_send(&cidesc, 0xa | CMDIF_NORESP_CMD, 64, 0, p_data);
		break;
	default:
		break;
	}
	return 0;
}

static struct cmdif_module_ops ops = {
                               .open_cb = open_cb,
                               .close_cb = close_cb,
                               .ctrl_cb = ctrl_cb
};

int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	dma_addr_t buff = 0;

	fsl_os_print("Running app_init()\n");

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

	err = cmdif_register_module("TEST0", &ops);
	if (err)
		fsl_os_print("FAILED cmdif_register_module\n!");

	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}
