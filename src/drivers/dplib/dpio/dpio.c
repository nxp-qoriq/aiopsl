#include <fsl_dplib_sys.h>
#include <fsl_cmdif.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpio.h>
#include <fsl_dpio_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
do { \
	cmd_data.params[_param] |= u64_enc(_offset, _width, _arg); \
}while (0);

#define RSP_READ(_param, _offset, _width, _type, _arg) \
do { \
	*(_arg) = (_type)u64_dec(cmd_data.params[_param], _offset, _width);\
}while (0);	

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
do{\
	_arg = (_type)u64_dec(cmd_data.params[_param], _offset, _width);\
}while (0);

int dpio_open(struct dpio *dpio, int dpio_id)
{
	return cmdif_open(&(dpio->cidesc), CMDIF_MOD_DPIO, (uint16_t)dpio_id);
}

int dpio_close(struct dpio *dpio)
{

	return cmdif_close(&(dpio->cidesc));
}

int dpio_init(struct dpio *dpio, const struct dpio_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_INIT(CMD_PREP);

	return cmdif_send(&(dpio->cidesc), DPIO_CMDID_INIT, DPIO_CMDSZ_INIT,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpio_done(struct dpio *dpio)
{
	/* send command to mc*/
	return cmdif_send(&(dpio->cidesc), DPIO_CMDID_DONE, DPIO_CMDSZ_DONE,
				CMDIF_PRI_LOW, NULL);
}

int dpio_enable(struct dpio *dpio)
{
	/* send command to mc*/
	return cmdif_send(&(dpio->cidesc), DPIO_CMDID_ENABLE,
				DPIO_CMDSZ_ENABLE, CMDIF_PRI_LOW, NULL);
}

int dpio_disable(struct dpio *dpio)
{
	/* send command to mc*/
	return cmdif_send(&(dpio->cidesc), DPIO_CMDID_DISABLE,
				DPIO_CMDSZ_DISABLE, CMDIF_PRI_LOW, NULL);
}

int dpio_get_attributes(struct dpio *dpio, struct dpio_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpio->cidesc), DPIO_CMDID_GET_ATTR,
				DPIO_CMDSZ_GET_ATTR, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPIO_RSP_GET_ATTR(RSP_READ_STRUCT);
	return err;
}

int dpio_get_irq(struct dpio *dpio,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpio->cidesc), DPIO_CMDID_GET_IRQ,
				DPIO_CMDSZ_GET_IRQ, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPIO_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpio_set_irq(struct dpio *dpio,
                 uint8_t irq_index,
                 uint64_t irq_paddr,
                 uint32_t irq_val)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return cmdif_send(&(dpio->cidesc), DPIO_CMDID_SET_IRQ, DPIO_CMDSZ_SET_IRQ,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpio_get_irq_enable(struct dpio *dpio,
                          uint8_t irq_index,
                          uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpio->cidesc), DPIO_CMDID_GET_IRQ_ENABLE,
				DPIO_CMDSZ_GET_IRQ_ENABLE, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
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
	return cmdif_send(&(dpio->cidesc), DPIO_CMDID_SET_IRQ_ENABLE, DPIO_CMDSZ_SET_IRQ_ENABLE,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpio_get_irq_mask(struct dpio *dpio,
                          uint8_t irq_index,
                          uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpio->cidesc), DPIO_CMDID_GET_IRQ_MASK,
				DPIO_CMDSZ_GET_IRQ_MASK, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPIO_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpio_set_irq_mask(struct dpio *dpio,
                          uint8_t irq_index,
                          uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPIO_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return cmdif_send(&(dpio->cidesc), DPIO_CMDID_SET_IRQ_MASK, DPIO_CMDSZ_SET_IRQ_MASK,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpio_get_irq_status(struct dpio *dpio,
                          uint8_t irq_index,
                          uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpio->cidesc), DPIO_CMDID_GET_IRQ_STATUS,
				DPIO_CMDSZ_GET_IRQ_STATUS, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
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
	return cmdif_send(&(dpio->cidesc), DPIO_CMDID_CLEAR_IRQ_STATUS, DPIO_CMDSZ_CLEAR_IRQ_STATUS,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

