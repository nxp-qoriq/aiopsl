#include <fsl_dplib_sys.h>
#include <fsl_cmdif_mc.h>

int dplib_status_to_error(enum mc_cmd_status status)
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

	/* not expected to reach here */
	return -EINVAL;
}

int dplib_send(void *regs,
               int auth,
               uint16_t cmd_id,
               uint16_t size,
               int pri,
               void *cmd_data)
{
	enum mc_cmd_status status;
	uint32_t flags;

	/* spin_lock_irqsave(&lock, flags); */

	mc_cmd_write((struct mc_portal *)regs, cmd_id, (uint16_t)auth,
	             (uint8_t)size, pri, (struct mc_cmd_data *)cmd_data);

	/* spin until cmd status changes */
	do {
		status = mc_cmd_read_status((struct mc_portal *)regs);
		/* ---------------- TBD call wait ------------- */
	} while (status == MC_CMD_STATUS_READY); // or timeout

	mc_cmd_read_response((struct mc_portal *)regs,
	                     (struct mc_cmd_data *)cmd_data);

	/* spin_unlock_irqrestore(&portal_lock, flags); */

	return dplib_status_to_error(status);
}

