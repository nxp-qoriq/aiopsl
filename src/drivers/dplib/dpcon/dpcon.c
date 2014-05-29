#include <fsl_dplib_sys.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpcon.h>
#include <fsl_dpcon_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd_data.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

int dpcon_create(struct dpcon *dpcon, const struct dpcon_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_CREATE(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpcon->auth = 0;

	/* send command to mc*/
	return dplib_send(dpcon->regs, &(dpcon->auth), MC_DPCON_CMDID_CREATE,
				DPCON_CMDSZ_CREATE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpcon_open(struct dpcon *dpcon, int dpcon_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_OPEN(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpcon->auth = 0;

	/* send command to mc*/
	return dplib_send(dpcon->regs, &(dpcon->auth), MC_DPCON_CMDID_OPEN,
				MC_CMD_OPEN_SIZE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpcon_close(struct dpcon *dpcon)
{
	return dplib_send(dpcon->regs, &(dpcon->auth), MC_CMDID_CLOSE,
				MC_CMD_CLOSE_SIZE, MC_CMD_PRI_HIGH, NULL);
}

int dpcon_destroy(struct dpcon *dpcon)
{
	return dplib_send(dpcon->regs, &(dpcon->auth), DPCON_CMDID_DESTROY,
				DPCON_CMDSZ_DESTROY, MC_CMD_PRI_LOW, NULL);
}

int dpcon_enable(struct dpcon *dpcon)
{
	return dplib_send(dpcon->regs, &(dpcon->auth), DPCON_CMDID_ENABLE,
				DPCON_CMDSZ_ENABLE, MC_CMD_PRI_LOW, NULL);
}

int dpcon_disable(struct dpcon *dpcon)
{
	return dplib_send(dpcon->regs, &(dpcon->auth), DPCON_CMDID_DISABLE,
				DPCON_CMDSZ_DISABLE, MC_CMD_PRI_LOW, NULL);
}

int dpcon_get_attributes(struct dpcon *dpcon, struct dpcon_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(dpcon->regs, &(dpcon->auth), DPCON_CMDID_GET_ATTR,
			 DPCON_CMDSZ_GET_ATTR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCON_RSP_GET_ATTR(RSP_READ_STRUCT);
	return err;
}

int dpcon_get_irq(struct dpcon *dpcon,
		  uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPCON_CMD_GET_IRQ(CMD_PREP);

	err = dplib_send(dpcon->regs, &(dpcon->auth), DPCON_CMDID_GET_IRQ,
			 DPCON_CMDSZ_GET_IRQ, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCON_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpcon_set_irq(struct dpcon *dpcon,
		  uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpcon->regs, &(dpcon->auth), DPCON_CMDID_SET_IRQ,
				DPCON_CMDSZ_SET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
}

int dpcon_get_irq_enable(struct dpcon *dpcon,
			 uint8_t irq_index,
	uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPCON_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = dplib_send(dpcon->regs, &(dpcon->auth),
			 DPCON_CMDID_GET_IRQ_ENABLE,
				DPCON_CMDSZ_GET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCON_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dpcon_set_irq_enable(struct dpcon *dpcon,
			 uint8_t irq_index,
	uint8_t enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_SET_IRQ_ENABLE(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpcon->regs, &(dpcon->auth),
				DPCON_CMDID_SET_IRQ_ENABLE,
				DPCON_CMDSZ_SET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpcon_get_irq_mask(struct dpcon *dpcon, uint8_t irq_index, uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPCON_CMD_GET_IRQ_MASK(CMD_PREP);

	err = dplib_send(dpcon->regs, &(dpcon->auth), DPCON_CMDID_GET_IRQ_MASK,
			 DPCON_CMDSZ_GET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCON_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpcon_set_irq_mask(struct dpcon *dpcon, uint8_t irq_index, uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpcon->regs, &(dpcon->auth),
				DPCON_CMDID_SET_IRQ_MASK,
				DPCON_CMDSZ_SET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpcon_get_irq_status(struct dpcon *dpcon,
			 uint8_t irq_index,
	uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPCON_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = dplib_send(dpcon->regs, &(dpcon->auth),
			 DPCON_CMDID_GET_IRQ_STATUS,
				DPCON_CMDSZ_GET_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCON_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dpcon_clear_irq_status(struct dpcon *dpcon,
			   uint8_t irq_index,
	uint32_t status)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpcon->regs, &(dpcon->auth),
				DPCON_CMDID_CLEAR_IRQ_STATUS,
				DPCON_CMDSZ_CLEAR_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
}

