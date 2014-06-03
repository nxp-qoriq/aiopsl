#include <fsl_dplib_sys.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpio.h>
#include <fsl_dpio_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd_data.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

int dpio_create(struct dpio *dpio, const struct dpio_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_CREATE(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpio->auth = 0;

	/* send command to mc*/
	return dplib_send(dpio->regs, &(dpio->auth), MC_DPIO_CMDID_CREATE,
				DPIO_CMDSZ_CREATE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpio_open(struct dpio *dpio, int dpio_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_OPEN(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpio->auth = 0;

	/* send command to mc*/
	return dplib_send(dpio->regs, &(dpio->auth), MC_DPIO_CMDID_OPEN,
				MC_CMD_OPEN_SIZE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpio_close(struct dpio *dpio)
{
	return dplib_send(dpio->regs, &(dpio->auth), MC_CMDID_CLOSE,
				MC_CMD_CLOSE_SIZE, MC_CMD_PRI_HIGH, NULL);
}

int dpio_destroy(struct dpio *dpio)
{
	return dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_DESTROY,
				DPIO_CMDSZ_DESTROY, MC_CMD_PRI_LOW, NULL);
}

int dpio_enable(struct dpio *dpio)
{
	return dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_ENABLE,
				DPIO_CMDSZ_ENABLE, MC_CMD_PRI_LOW, NULL);
}

int dpio_disable(struct dpio *dpio)
{
	return dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_DISABLE,
				DPIO_CMDSZ_DISABLE, MC_CMD_PRI_LOW, NULL);
}

int dpio_get_attributes(struct dpio *dpio, struct dpio_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_GET_ATTR,
			 DPIO_CMDSZ_GET_ATTR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPIO_RSP_GET_ATTR(RSP_READ_STRUCT);

	return err;
}

int dpio_get_irq(struct dpio *dpio,
		 uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPIO_CMD_GET_IRQ(CMD_PREP);

	err = dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_GET_IRQ,
			 DPIO_CMDSZ_GET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
	if (!err)
		DPIO_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpio_set_irq(struct dpio *dpio,
		 uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_SET_IRQ,
				DPIO_CMDSZ_SET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
}

int dpio_get_irq_enable(struct dpio *dpio,
			uint8_t irq_index,
	uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPIO_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_GET_IRQ_ENABLE,
			 DPIO_CMDSZ_GET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPIO_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dpio_set_irq_enable(struct dpio *dpio,
			uint8_t irq_index,
	uint8_t enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_SET_IRQ_ENABLE(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_SET_IRQ_ENABLE,
				DPIO_CMDSZ_SET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpio_get_irq_mask(struct dpio *dpio, uint8_t irq_index, uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPIO_CMD_GET_IRQ_MASK(CMD_PREP);

	err = dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_GET_IRQ_MASK,
			 DPIO_CMDSZ_GET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPIO_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpio_set_irq_mask(struct dpio *dpio, uint8_t irq_index, uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_SET_IRQ_MASK,
				DPIO_CMDSZ_SET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpio_get_irq_status(struct dpio *dpio, uint8_t irq_index, uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPIO_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = dplib_send(dpio->regs, &(dpio->auth), DPIO_CMDID_GET_IRQ_STATUS,
			 DPIO_CMDSZ_GET_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPIO_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dpio_clear_irq_status(struct dpio *dpio,
			  uint8_t irq_index,
	uint32_t status)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpio->regs, &(dpio->auth),
				DPIO_CMDID_CLEAR_IRQ_STATUS,
				DPIO_CMDSZ_CLEAR_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
}
