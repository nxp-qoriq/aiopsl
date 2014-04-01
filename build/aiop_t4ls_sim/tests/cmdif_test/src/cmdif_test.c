#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "fsl_fdma.h"
#include "general.h"
#include "fsl_cdma.h"
#include "common/fsl_slab.h"
#include "kernel/platform.h"
#include "common/fsl_aiop_cmdif.h"
#include "cmdif_srv.h"

int app_init(void);
void app_free(void);

__SHRAM uint32_t sync_done = 0;

/* Client STUB */
int cmdif_open(struct cmdif_desc *cidesc,
		const char *module_name,
		uint8_t instance_id,
		cmdif_cb_t async_cb,
		void *async_ctx)
{
	struct   cmdif_srv *srv = sys_get_handle(FSL_OS_MOD_CMDIF_SRV, 0);
	UNUSED(cidesc);
	UNUSED(module_name);
	UNUSED(instance_id);
	UNUSED(async_cb);
	UNUSED(async_ctx);
	srv->instance_handle[0] = (void *)0x22222222;
	srv->sync_done[0] = &sync_done;
	return 0;
}

static int open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(instance_id);
	UNUSED(dev);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	return 0;
}

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size, uint8_t *data)
{
	UNUSED(dev);
	UNUSED(cmd);
	UNUSED(size);
	UNUSED(data);

	return 0;
}

int app_init(void)
{
	const char * module = "ABCABC";
	struct cmdif_module_ops ops;
	int err = 0;

	ops.open_cb  = open_cb;
	ops.close_cb = close_cb;
	ops.ctrl_cb  = ctrl_cb;

	err = cmdif_register_module(module, &ops);
	if (err) return err;
	err = cmdif_register_module(module, &ops);
	if (!err) return err;
	err = cmdif_register_module(module, &ops);
	if (!err) return err;
	err = cmdif_register_module("ABCDEF", &ops);
	if (err) return err;
	err = cmdif_register_module("TTTTTTTTTTT", &ops);
	if (err) return err;
	err = cmdif_unregister_module("ABCDEF");
	if (err) return err;
	err = cmdif_unregister_module("TTTTTTTTTTT");
	if (err) return err;

	err = cmdif_open(NULL, module, 0, NULL, NULL);

	return err;
}

void app_free(void)
{

}
