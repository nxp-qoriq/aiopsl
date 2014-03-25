#include "common/types.h"
#include "common/gen.h"
#include "cmdif_srv.h"
#include "common/fsl_aiop_cmdif.h"
#include "general.h"
#include "common/errors.h"
#include "io.h"
#include "fsl_fdma.h"

extern __TASK struct aiop_default_task_params default_task_params; /* TODO remove it */

int cmdif_srv_init(void);
void cmdif_srv_free(void);
void cmdif_srv_isr(void);


int cmdif_register_module(const char *module_name, struct cmdif_module_ops *ops)
{
	if ((module_name == NULL) || (ops == NULL))
		return -EINVAL;

	return 0;
}

int cmdif_unregister_module(const char *module_name)
{
	if (module_name == NULL)
		return -EINVAL;

	return 0;
}

int cmdif_srv_init(void)
{
	return 0;
}

void cmdif_srv_free(void)
{
}

static uint16_t cmd_id_get()
{
	uint64_t data = 0;
	/* FD[FLC] */
	data = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	return (uint16_t)((data & CMD_ID_MASK) >> CMD_ID_OFF);
}

static uint32_t cmd_size_get()
{
	return 0;
}

static uint8_t * cmd_data_get()
{
	return (uint8_t *)fsl_os_phys_to_virt(LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS));
}

static int cmdif_fd_send()
{
	struct fdma_queueing_destination_params    enqueue_params;
	int err;

	/* for the enqueue set hash from TLS, an flags equal 0 meaning that \
	 * the qd_priority is taken from the TLS and that enqueue function \
	 * always returns*/
	enqueue_params.qdbin = 0;
	enqueue_params.qd = 0; /* TODO take it from tx context */
	enqueue_params.qd_priority = default_task_params.qd_priority;
	err = (int)fdma_store_and_enqueue_default_frame_qd(&enqueue_params,
	                                                   FDMA_ENWF_NO_FLAGS);
	return err;
}

static int sync_cmd_done(uint16_t cmd_id, uint32_t size, uint8_t *data)
{
	if (!(cmd_id & CMDIF_ASYNC_CMD) && (size > 0)) {
		*data = 0x1;
		return 0;
	} else {
		return -EINVAL;
	}
}

#pragma push
#pragma force_active on

void cmdif_srv_isr(void)
{
	uint16_t cmd_id = 0;
	int      err    = 0;
	uint32_t size	= 0;
	uint8_t  *data	= 0;

	cmd_id	= cmd_id_get();
	data	= cmd_data_get();
	size	= cmd_size_get();

	if (cmd_id & CMD_ID_OPEN) {
		/* Call open_cb */
		sync_cmd_done(cmd_id, size, data);
	} else {
		/* Call ctrl_cb */
		if (cmd_id & CMDIF_ASYNC_CMD) {
			err = cmdif_fd_send();
		} else {
			sync_cmd_done(cmd_id, size, data);
		}
	}

	if (!(cmd_id & CMDIF_NORESP_CMD)) {
		err = cmdif_fd_send();
	}

	fdma_terminate_task();
}

#pragma pop
