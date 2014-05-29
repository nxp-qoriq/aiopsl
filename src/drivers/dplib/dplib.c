#include <fsl_dplib_sys.h>
#include <fsl_cmdif_mc.h>

static int dplib_status_to_error(enum mc_cmd_status status)
{
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

	/* Not expected to reach here */
	return -EINVAL;
}

int dplib_send(void *regs,
	       int *auth,
	       uint16_t cmd_id,
	       uint16_t size,
	       int pri,
	       void *cmd_data)
{
	struct mc_portal *portal = (struct mc_portal *)regs;
	enum mc_cmd_status status;

	if (!regs || !auth)
		return -EACCES;

	/* --- Call lock function here in case portal is shared --- */

	mc_cmd_write(portal, cmd_id, (uint16_t)(*auth),
		     (uint8_t)size, pri, (struct mc_cmd_data *)cmd_data);

	/* Spin until status changes */
	do {
		status = mc_cmd_read_status(portal);

		/*
		 * --- Call wait function here to prevent blocking ---
		 * Change the loop condition accordingly to exit on timeout.
		 */
	} while (status == MC_CMD_STATUS_READY);

	/* Read the response back into the command buffer */
	if (cmd_data)
		mc_cmd_read_response(portal, (struct mc_cmd_data *)cmd_data);

	/*
	 * The authentication id is read in create() or open() commands,
	 * to setup the control session.
	 */
	if (0 == (*auth))
		*auth = (int)mc_cmd_read_auth_id(portal);

	/* --- Call unlock function here in case portal is shared --- */

	return dplib_status_to_error(status);
}

