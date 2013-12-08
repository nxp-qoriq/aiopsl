#include "common/types.h"
#include "common/errors.h"
#include "common/fsl_cmdif.h"
#include "common/fsl_malloc.h"
#include "common/gen.h"
#include "arch/fsl_soc.h"


#define CMDIF_MC_OPEN_SIZE	8
#define CMDIF_MC_CLOSE_SIZE	0

#define CMDIF_MC_DPNI_OPEN	0x201
#define CMDIF_MC_DPSW_OPEN	0x202
#define CMDIF_MC_DPIO_OPEN	0x203
#define CMDIF_MC_DPSP_OPEN	0x204
#define CMDIF_MC_DPRC_OPEN	0x205

#define CMDIF_MC_CLOSE		0x200

#define CMDIF_MC_READ_CMDID(_hdr)		u64_read_field((_hdr), CMDIF_MC_CMDID_OFFSET, CMDIF_MC_CMDID_SIZE)
#define CMDIF_MC_READ_AUTHID(_hdr)	u64_read_field((_hdr), CMDIF_MC_AUTHID_OFFSET, CMDIF_MC_AUTHID_SIZE)
#define CMDIF_MC_READ_SIZE(_hdr)		u64_read_field((_hdr), CMDIF_MC_SIZE_OFFSET, CMDIF_MC_SIZE_SIZE)
#define CMDIF_MC_READ_STATUS(_hdr)	u64_read_field((_hdr), CMDIF_MC_STATUS_OFFSET, CMDIF_MC_STATUS_SIZE)
#define CMDIF_MC_READ_PRI(_hdr)		u64_read_field((_hdr), CMDIF_MC_PRI_OFFSET, CMDIF_MC_PRI_SIZE)

#define CMDIF_MC_READ_HEADER(_ptr)	swap_uint64((_ptr)->header)

#define CMDIF_MC_WRITE_HEADER(_ptr, _id, _auth, _size, _status, _pri) 			\
	do { 										\
		volatile uint64_t tmp = 0;							\
		u64_write_field(tmp, CMDIF_MC_CMDID_OFFSET, CMDIF_MC_CMDID_SIZE, (_id));	\
		u64_write_field(tmp, CMDIF_MC_AUTHID_OFFSET, CMDIF_MC_AUTHID_SIZE, (_auth));	\
		u64_write_field(tmp, CMDIF_MC_SIZE_OFFSET, CMDIF_MC_SIZE_SIZE, (_size));	\
		u64_write_field(tmp, CMDIF_MC_STATUS_OFFSET, CMDIF_MC_STATUS_SIZE, (_status));	\
		u64_write_field(tmp, CMDIF_MC_PRI_OFFSET, CMDIF_MC_PRI_SIZE, (_pri));		\
		(_ptr)->header = swap_uint64(tmp);					\
	} while (0)


struct mc_portal_regs {
	uint64_t header;
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
	uint64_t param6;
	uint64_t param7;
};

struct cmdif_dev {
	struct mc_portal_regs *regs;
	int auth_id;
};


static int wait_resp(struct mc_portal_regs *regs)
{
	enum cmdif_status status;

	/* Busy waiting for MC to complete command */
	do {
		status = (enum cmdif_status)CMDIF_MC_READ_STATUS(regs->header);
	} while (status == CMDIF_STATUS_READY);

	
	/* TODO - some errors are not defined */
	switch (status)
	{
	case CMDIF_STATUS_OK:
		return E_OK;
	case CMDIF_STATUS_READY:
		return ENOSYS; 			/* function not implemented */
	case CMDIF_STATUS_AUTH_ERR:
		return 99;	//EUNKNOWN;															 /* unknown error */
	case CMDIF_STATUS_NO_PRIVILEGE:
		return EACCES;  		/* Permission denied */
	case CMDIF_STATUS_DMA_ERR:
		return 99;//EUNKNOWN;															 /* unknown error */
	case CMDIF_STATUS_CONFIG_ERR:	
		return ENXIO;			/* Device not configured */
	case  CMDIF_STATUS_TIMEOUT:
		return 99;//EUNKNOWN;															 /* unknown error */
	case CMDIF_STATUS_NO_RESOURCE:
		return EAGAIN;			/* Resource temporarily unavailable */
	case CMDIF_STATUS_NO_MEMORY:
		return ENOMEM; 			/* Cannot allocate memory */
	case CMDIF_STATUS_BUSY:
		return EBUSY; 			/* Device busy */
	case CMDIF_STATUS_INVALID_OP:
		return EPERM;			/* Operation not permitted */
	case CMDIF_STATUS_UNSUPPORTED_OP:
		return ENODEV;			/* Operation not supported by device */
	case CMDIF_STATUS_INVALID_STATE:
		return 99;	//EUNKNOWN;															/* Unknown error */

		
	}
	return 99; /* default value */
}

struct cmdif_dev *cmdif_open(void *regs,
                             enum fsl_os_module mod,
                             uint16_t mod_id)
{
	UNUSED (mod_id);

	int err;
	int cmdid = 0;
	struct cmdif_dev *dev = fsl_os_malloc(sizeof(struct cmdif_dev));

	dev->regs = (struct mc_portal_regs *)regs;

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

	if (cmdid != 0)
		err = cmdif_send(dev, (uint16_t)cmdid, CMDIF_MC_OPEN_SIZE,
		                    CMDIF_PRI_LOW, NULL);

	if (err != E_OK)
		return NULL;

	/* Save Authentication ID */
	dev->auth_id = (int)CMDIF_MC_READ_AUTHID(dev->regs->header);
	return dev;
}

int cmdif_close(struct cmdif_dev *dev)
{
	/* TODO - review */
	int err;
	err = cmdif_send(dev, CMDIF_MC_CLOSE, CMDIF_MC_CLOSE_SIZE,
			                    CMDIF_PRI_LOW, NULL);
	fsl_os_free(dev);
	return err;
}

int cmdif_send(struct cmdif_dev *dev,
               uint16_t cmd,
               int size,
               int priority,
               struct cmdif_cmd_desc *desc)
{
	GPP_CMD_WRITE_PARAM(dev->regs, 1, desc->param1);
	GPP_CMD_WRITE_PARAM(dev->regs, 2, desc->param2);
	GPP_CMD_WRITE_PARAM(dev->regs, 3, desc->param3);
	GPP_CMD_WRITE_PARAM(dev->regs, 4, desc->param4);
	GPP_CMD_WRITE_PARAM(dev->regs, 5, desc->param5);
	GPP_CMD_WRITE_PARAM(dev->regs, 6, desc->param6);
	GPP_CMD_WRITE_PARAM(dev->regs, 7, desc->param7);

	CMDIF_MC_WRITE_HEADER(dev->regs, cmd, dev->auth_id, size,
	                    CMDIF_STATUS_READY, priority);

	return wait_resp(dev->regs); /* blocking */
}

#if 0
int status2errno(enum cmdif_status status)
{
	int ret;

	switch (status) {
		case CMDIF_STATUS_READY: /**< [0] not handled (only GPP clears this field) */
		ret = EOPNOTSUPP; /* Operation not supported on transport endpoint */
		break;
		case CMDIF_STATUS_OK: /**< [2]Passed */
		ret = 0;
		break;
		case CMDIF_STATUS_AUTH_ERR: /**< [3]Authentication error */
		ret = EACCES; /* Permission denied */
		break;
		case CMDIF_STATUS_NO_PRIVILEGE: /**< [4]Privilege error */
		ret = PRIV_ERROR;
		break;
		case CMDIF_STATUS_DMA_ERR: /**< [5]DMA failure */
		ret = DMA_ERROR;
		break;
		case CMDIF_STATUS_CONFIG_ERR: /**< [6]Configuration failure */
		ret = CONF_ERROR;
		break;
		case CMDIF_STATUS_TIMEOUT: /**< [7]command timed out */
		ret = ETIME; /* Timer expired */
		break;
		case CMDIF_STATUS_NO_RESOURCE: /**< [8]no resources */
		ret = ENOSR;
		break;
		case CMDIF_STATUS_NO_MEMORY: /**< [9]no_mem */
		ret = NO_MEM;
		break;
		case CMDIF_STATUS_BUSY: /**< [A]busy */
		ret = EBUSY;
		break;
		case CMDIF_STATUS_INVALID_OP: /**< [B]Invalid operation */
		ret = INVALID_OPERATION;
		break;
		case CMDIF_STATUS_UNSUPPORTED_OP:/**< [C]Unsupported operation */
		ret = UNSUPPORTED_OPERATION;
		break;
		case CMDIF_STATUS_INVALID_STATE: /**< [D]Invalid state */
		ret = ENOTRECOVERABLE;
		break;
		default:
		ret = UNKNOWN_ERROR;
		break;
	}
	return ret;
}
#endif

struct cmdif_cmd_desc *cmdif_get_desc(struct cmdif_dev *dev)
{
	return PTR_MOVE(dev->regs, 8);
}

