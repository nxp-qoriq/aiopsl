#include <fsl_dplib_sys.h>
#include <fsl_cmdif.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpcon.h>
#include <fsl_dpcon_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	cmd_data.params[_param] |= u64_enc(_offset, _width, _arg); 

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	*(_arg) = (_type)u64_dec(cmd_data.params[_param], _offset, _width);

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	_arg = (_type)u64_dec(cmd_data.params[_param], _offset, _width);

int dpcon_open(struct dpcon *dpcon, int dpcon_id)
{
	return cmdif_open(&(dpcon->cidesc), CMDIF_MOD_DPCON,
				(uint16_t)dpcon_id);
}

int dpcon_close(struct dpcon *dpcon)
{
	return cmdif_close(&(dpcon->cidesc));
}

int dpcon_init(struct dpcon *dpcon, const struct dpcon_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_INIT(CMD_PREP);

	return cmdif_send(&(dpcon->cidesc), DPCON_CMDID_INIT, DPCON_CMDSZ_INIT,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpcon_done(struct dpcon *dpcon)
{
	/* send command to mc*/
	return cmdif_send(&(dpcon->cidesc), DPCON_CMDID_DONE, DPCON_CMDSZ_DONE,
				CMDIF_PRI_LOW, NULL);
}

int dpcon_enable(struct dpcon *dpcon)
{
	/* send command to mc*/
	return cmdif_send(&(dpcon->cidesc), DPCON_CMDID_ENABLE,
				DPCON_CMDSZ_ENABLE, CMDIF_PRI_LOW, NULL);
}

int dpcon_disable(struct dpcon *dpcon)
{
	/* send command to mc*/
	return cmdif_send(&(dpcon->cidesc), DPCON_CMDID_DISABLE,
				DPCON_CMDSZ_DISABLE, CMDIF_PRI_LOW, NULL);
}

int dpcon_get_attributes(struct dpcon *dpcon, struct dpcon_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpcon->cidesc), DPCON_CMDID_GET_ATTR,
				DPCON_CMDSZ_GET_ATTR, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPCON_RSP_GET_ATTR(RSP_READ_STRUCT);
	return err;
}

int dpcon_get_irq(struct dpcon *dpcon,
                 uint8_t irq_index,
                 uint64_t *irq_paddr,
                 uint32_t *irq_val)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpcon->cidesc), DPCON_CMDID_GET_IRQ,
				DPCON_CMDSZ_GET_IRQ, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPCON_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpcon_set_irq(struct dpcon *dpcon,
                 uint8_t irq_index,
                 uint64_t irq_paddr,
                 uint32_t irq_val)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return cmdif_send(&(dpcon->cidesc), DPCON_CMDID_SET_IRQ, DPCON_CMDSZ_SET_IRQ,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpcon_get_irq_enable(struct dpcon *dpcon,
                          uint8_t irq_index,
                          uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpcon->cidesc), DPCON_CMDID_GET_IRQ_ENABLE,
				DPCON_CMDSZ_GET_IRQ_ENABLE, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
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
	return cmdif_send(&(dpcon->cidesc), DPCON_CMDID_SET_IRQ_ENABLE, DPCON_CMDSZ_SET_IRQ_ENABLE,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpcon_get_irq_mask(struct dpcon *dpcon,
                          uint8_t irq_index,
                          uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpcon->cidesc), DPCON_CMDID_GET_IRQ_MASK,
				DPCON_CMDSZ_GET_IRQ_MASK, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
	if (!err)
		DPCON_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpcon_set_irq_mask(struct dpcon *dpcon,
                          uint8_t irq_index,
                          uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCON_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return cmdif_send(&(dpcon->cidesc), DPCON_CMDID_SET_IRQ_MASK, DPCON_CMDSZ_SET_IRQ_MASK,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

int dpcon_get_irq_status(struct dpcon *dpcon,
                          uint8_t irq_index,
                          uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = cmdif_send(&(dpcon->cidesc), DPCON_CMDID_GET_IRQ_STATUS,
				DPCON_CMDSZ_GET_IRQ_STATUS, CMDIF_PRI_LOW,
				(uint8_t *)&cmd_data);
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
	return cmdif_send(&(dpcon->cidesc), DPCON_CMDID_CLEAR_IRQ_STATUS, DPCON_CMDSZ_CLEAR_IRQ_STATUS,
				CMDIF_PRI_LOW, (uint8_t *)&cmd_data);
}

