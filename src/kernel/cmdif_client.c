#include "common/types.h"
#include "common/dbg.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "common/fsl_cmdif.h"
#include "common/fsl_malloc.h"
#include "common/gen.h"
#include "arch/fsl_soc.h"
#include "arch/fsl_cmdif_mc.h"

#define CMDIF_MC_OPEN_SIZE	8
#define CMDIF_MC_CLOSE_SIZE	0

#define CMDIF_MC_DPNI_OPEN	0x801
#define CMDIF_MC_DPSW_OPEN	0x802
#define CMDIF_MC_DPIO_OPEN	0x803
#define CMDIF_MC_DPSP_OPEN	0x804
#define CMDIF_MC_DPRC_OPEN	0x805

#define CMDIF_MC_CLOSE		0x800

#define CMDIF_MC_READ_AUTHID(_hdr)	u64_read_field(swap_uint64(_hdr), CMDIF_MC_AUTHID_OFFSET, CMDIF_MC_AUTHID_SIZE)
#define CMDIF_MC_READ_STATUS(_hdr)	u64_read_field(swap_uint64(_hdr), CMDIF_MC_STATUS_OFFSET, CMDIF_MC_STATUS_SIZE)

#define CMDIF_MC_WRITE_HEADER(_ptr, _id, _auth, _size, _status, _pri) 					\
	do { 												\
		volatile uint64_t tmp = 0;								\
		tmp = u64_write_field(tmp, CMDIF_MC_CMDID_OFFSET, CMDIF_MC_CMDID_SIZE, (_id));		\
		tmp = u64_write_field(tmp, CMDIF_MC_AUTHID_OFFSET, CMDIF_MC_AUTHID_SIZE, (_auth));	\
		tmp = u64_write_field(tmp, CMDIF_MC_SIZE_OFFSET, CMDIF_MC_SIZE_SIZE, (_size));		\
		tmp = u64_write_field(tmp, CMDIF_MC_STATUS_OFFSET, CMDIF_MC_STATUS_SIZE, (_status));	\
		tmp = u64_write_field(tmp, CMDIF_MC_PRI_OFFSET, CMDIF_MC_PRI_SIZE, (_pri));		\
		(_ptr)->header = swap_uint64(tmp);							\
	} while (0)

static int wait_resp(struct mc_portal_regs *regs)
{
	enum cmdif_status status;

	/* Busy waiting for MC to complete command */
	do {
		status = (enum cmdif_status)CMDIF_MC_READ_STATUS(regs->header);
	} while (status == CMDIF_STATUS_READY);

	/* TODO - some errors are not defined */
	switch (status) {
	case CMDIF_STATUS_OK:
		return 0;
	case CMDIF_STATUS_AUTH_ERR:
		return -EACCES; /* unknown error */
	case CMDIF_STATUS_NO_PRIVILEGE:
		return -EPERM; /* Permission denied */
	case CMDIF_STATUS_DMA_ERR:
		return -EIO; /* unknown error */
	case CMDIF_STATUS_CONFIG_ERR:
		return -EINVAL; /* Device not configured */
	case CMDIF_STATUS_TIMEOUT:
		return -ETIMEDOUT; /* unknown error */
	case CMDIF_STATUS_NO_RESOURCE:
		return -ENAVAIL; /* Resource temporarily unavailable */
	case CMDIF_STATUS_NO_MEMORY:
		return -ENOMEM; /* Cannot allocate memory */
	case CMDIF_STATUS_BUSY:
		return -EBUSY; /* Device busy */
	case CMDIF_STATUS_UNSUPPORTED_OP:
		return -ENOTSUP; /* Operation not supported by device */
	case CMDIF_STATUS_INVALID_STATE:
		return -ENODEV; /* Unknown error */

	}
	return 99; /* default value */
}

int cmdif_open(struct cmdif_desc *cidesc,
               enum fsl_os_module mod,
               uint16_t mod_id)
{
	struct mc_portal_regs *regs = (struct mc_portal_regs *)cidesc->regs;
	int ret;
	int cmdid = 0;

	switch (mod) {
	case FSL_OS_MOD_DPNI:
		cmdid = CMDIF_MC_DPNI_OPEN;
		break;
	case FSL_OS_MOD_DPSW:
		cmdid = CMDIF_MC_DPSW_OPEN;
		break;
	case FSL_OS_MOD_DPRC:
		cmdid = CMDIF_MC_DPRC_OPEN;
		break;
	case FSL_OS_MOD_DPIO:
		cmdid = CMDIF_MC_DPIO_OPEN;
		break;
	case FSL_OS_MOD_DPSP:
		cmdid = CMDIF_MC_DPSP_OPEN;
		break;
	default:
		break;
	}

	/* Clear 'dev', which later will store the Authentication ID */
	cidesc->dev = (void*)0;

	if (cmdid != 0)
		ret = cmdif_send(cidesc, (uint16_t)cmdid, CMDIF_MC_OPEN_SIZE,
		                 CMDIF_PRI_LOW, NULL);
	if (ret != 0)
		return ret;

	/* Store the Authentication ID in 'dev' */
	cidesc->dev = (void*)CMDIF_MC_READ_AUTHID(regs->header);
	return 0;
}

int cmdif_close(struct cmdif_desc *cidesc)
{
	/* TODO - review */
	int err;
	err = cmdif_send(cidesc, CMDIF_MC_CLOSE, CMDIF_MC_CLOSE_SIZE,
	                 CMDIF_PRI_LOW, NULL);
	return err;
}

int cmdif_send(struct cmdif_desc *cidesc,
               uint16_t cmd,
               int size,
               int priority,
               struct cmdif_cmd_data *cmd_data)
{
	struct mc_portal_regs *regs = (struct mc_portal_regs *)cidesc->regs;

	if (cmd_data) {
		GPP_CMD_WRITE_PARAM(regs, 1, cmd_data->param1);
		GPP_CMD_WRITE_PARAM(regs, 2, cmd_data->param2);
		GPP_CMD_WRITE_PARAM(regs, 3, cmd_data->param3);
		GPP_CMD_WRITE_PARAM(regs, 4, cmd_data->param4);
		GPP_CMD_WRITE_PARAM(regs, 5, cmd_data->param5);
		GPP_CMD_WRITE_PARAM(regs, 6, cmd_data->param6);
		GPP_CMD_WRITE_PARAM(regs, 7, cmd_data->param7);
	}

	CMDIF_MC_WRITE_HEADER(regs, cmd, (int)cidesc->dev, size,
	                      CMDIF_STATUS_READY, priority);
	/*pr_debug("GPP sent cmd (BE) 0x%08x%08x\n",
	 (uint32_t)(swap_uint64(dev->regs->header)>>32),
	 (uint32_t)swap_uint64(dev->regs->header));
	 */
	return wait_resp(cidesc->regs); /* blocking */
}

int cmdif_get_cmd_data(struct cmdif_desc *cidesc,
                       struct cmdif_cmd_data **cmd_data)
{
	*cmd_data = (struct cmdif_cmd_data*)PTR_MOVE(cidesc->regs, 8);
	return 0;
}

