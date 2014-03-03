#include <fsl_dplib_sys.h>
#include <fsl_cmdif.h>
#include <fsl_cmdif_mc.h>

static int cmdif_wait_resp(struct mc_portal *portal)
{
	enum mc_cmd_status status;

	/* wait for MC to complete command */
	do {
		status = mc_cmd_read_status(portal);
	} while (status == MC_CMD_STATUS_READY);

	switch (status) {
	case MC_CMD_STATUS_OK:
		return 0;
	case MC_CMD_STATUS_AUTH_ERR:
		return -EACCES; /* Authentication error */
	case MC_CMD_STATUS_NO_PRIVILEGE:
		return -EPERM; /* Permission denied */
	case MC_CMD_STATUS_DMA_ERR:
		return -EIO; /* Input/Output error */
	case MC_CMD_STATUS_CONFIG_ERR:
		return -EINVAL; /* Device not configured */
	case MC_CMD_STATUS_TIMEOUT:
		return -ETIMEDOUT; /* Operation timed out */
	case MC_CMD_STATUS_NO_RESOURCE:
		return -ENAVAIL; /* Resource temporarily unavailable */
	case MC_CMD_STATUS_NO_MEMORY:
		return -ENOMEM; /* Cannot allocate memory */
	case MC_CMD_STATUS_BUSY:
		return -EBUSY; /* Device busy */
	case MC_CMD_STATUS_UNSUPPORTED_OP:
		return -ENOTSUP; /* Operation not supported by device */
	case MC_CMD_STATUS_INVALID_STATE:
		return -ENODEV; /* Invalid device state */
	default:
		break;
	}

	/* not expected to reach here */
	return -EINVAL;
}

struct cmdif_cmd_mapping_entry {
	enum cmdif_module module;
	uint16_t cmd_id;
};

int cmdif_open(struct cmdif_desc *cidesc,
               enum cmdif_module module,
               int instance_id)
{
	struct mc_portal *portal = (struct mc_portal *)cidesc->regs;
	struct mc_cmd_data cmd_data = {{ 0 }};
	uint16_t auth_id;
	int ret, i;

	const struct cmdif_cmd_mapping_entry cmd_map[] =
	        { { CMDIF_MOD_DPRC, MC_DPRC_CMDID_OPEN },
	          { CMDIF_MOD_DPNI, MC_DPNI_CMDID_OPEN },
	          { CMDIF_MOD_DPIO, MC_DPIO_CMDID_OPEN },
	          { CMDIF_MOD_DPBP, MC_DPBP_CMDID_OPEN },
	          { CMDIF_MOD_DPDMUX, MC_DPDMUX_CMDID_OPEN },
	          { CMDIF_MOD_DPSW, MC_DPSW_CMDID_OPEN } };

	for (i = 0; i < ARRAY_SIZE(cmd_map); i++)
		if (cmd_map[i].module == module)
			break;
	if (i == ARRAY_SIZE(cmd_map))
		return -ENOTSUP;

	/* clear 'dev' - later it will store the Authentication ID */
	cidesc->dev = (void*)0;

	/* prepare command param */
	cmd_data.params[0] = u64_enc(MC_CMD_OPEN_INST_ID_O,
	                             MC_CMD_OPEN_INST_ID_S, instance_id);

	/* acquire lock as needed */
	if (cidesc->lock_cb)
		cidesc->lock_cb(cidesc->lock);

	/* not using cmdif_send - need auth_id back before releasing the lock */
	mc_cmd_write(portal, cmd_map[i].cmd_id, 0, MC_CMD_OPEN_SIZE,
	             CMDIF_PRI_LOW, &cmd_data);

	ret = cmdif_wait_resp(portal); /* blocking */
	auth_id = mc_cmd_read_auth_id(portal);

	/* release lock as needed */
	if (cidesc->unlock_cb)
		cidesc->unlock_cb(cidesc->lock);

	if (ret == 0)
		/* all good - store the Authentication ID in 'dev' */
		cidesc->dev = (void*)auth_id;

	return ret;
}

int cmdif_close(struct cmdif_desc *cidesc)
{
	return cmdif_send(cidesc, MC_CMDID_CLOSE, MC_CMD_CLOSE_SIZE,
	                  CMDIF_PRI_HIGH, NULL);
}

int cmdif_send(struct cmdif_desc *cidesc,
               uint16_t cmd_id,
               int size,
               int priority,
               uint8_t *cmd_data)
{
	struct mc_portal *portal = (struct mc_portal *)cidesc->regs;
	struct mc_cmd_data *data = (struct mc_cmd_data *)cmd_data;
	uint16_t auth_id = (uint16_t)PTR_TO_UINT(cidesc->dev);
	int ret;

	/* acquire external lock as needed */
	if (cidesc->lock_cb)
		cidesc->lock_cb(cidesc->lock);

	mc_cmd_write(portal, cmd_id, auth_id, (uint8_t)size, priority, data);
	/*pr_debug("GPP sent cmd (BE) 0x%08x%08x\n",
	 (uint32_t)(swap_uint64(dev->regs->header)>>32),
	 (uint32_t)swap_uint64(dev->regs->header));
	 */
	ret = cmdif_wait_resp(portal); /* blocking */
	if (ret == 0)
		mc_cmd_read_response(portal, data);

	/* release lock as needed */
	if (cidesc->unlock_cb)
		cidesc->unlock_cb(cidesc->lock);

	return ret;
}

#if 1
int cmdif_get_cmd_data(struct cmdif_desc *cidesc, uint8_t **cmd_data)
{
	*cmd_data = (uint8_t*)PTR_MOVE(cidesc->regs, 8);
	return 0;
}
#endif
