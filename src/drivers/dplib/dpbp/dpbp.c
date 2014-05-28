#include <fsl_dplib_sys.h>
#include <fsl_cmdif_mc.h>
#include <fsl_dpbp.h>
#include <fsl_dpbp_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd_data.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

int dpbp_create(struct dpbp *dpbp, const struct dpbp_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPBP_CMD_CREATE(CMD_PREP);

	/* send command to mc*/

	/* clear 'dev' - later it will store the Authentication ID */
	dpbp->auth = 0;

	/* ----------TBD lock------------------ */
	err = dplib_send(dpbp->regs, 0, MC_DPBP_CMDID_CREATE, DPBP_CMDSZ_CREATE,
	                 CMDIF_PRI_LOW, &cmd_data);
	if (!err)
		dpbp->auth = (int)mc_cmd_read_auth_id(dpbp->regs);
	/* ----------TBD unlock------------------ */

	return err;
}

int dpbp_open(struct dpbp *dpbp, int dpbp_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	
	/* prepare command */
	DPBP_CMD_OPEN(CMD_PREP);

	/* send command to mc*/
	dpbp->auth = 0;

	/* ----------TBD lock------------------ */
	err = dplib_send(dpbp->regs, 0, MC_DPBP_CMDID_OPEN, MC_CMD_OPEN_SIZE,
	                 CMDIF_PRI_LOW, &cmd_data);
	if (!err)
		dpbp->auth = (int)mc_cmd_read_auth_id(dpbp->regs);
	/* ----------TBD unlock------------------ */

	return err;
}

int dpbp_close(struct dpbp *dpbp)
{
	return dplib_send(dpbp->regs, 0, MC_CMDID_CLOSE, MC_CMD_CLOSE_SIZE,
		                 CMDIF_PRI_LOW, NULL);
}

int dpbp_destroy(struct dpbp *dpbp)
{
	/* send command to mc*/
	return dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_DESTROY,
			  DPBP_CMDSZ_DESTROY, CMDIF_PRI_LOW, NULL);
}

int dpbp_enable(struct dpbp *dpbp)
{
	/* send command to mc*/
	return dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_ENABLE, DPBP_CMDSZ_ENABLE,
			  CMDIF_PRI_LOW, NULL);
}

int dpbp_disable(struct dpbp *dpbp)
{
	/* send command to mc*/
	return dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_DISABLE,
			  DPBP_CMDSZ_DISABLE, CMDIF_PRI_LOW, NULL);
}

int dpbp_get_attributes(struct dpbp *dpbp, struct dpbp_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_GET_ATTR,
			 DPBP_CMDSZ_GET_ATTR, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPBP_RSP_GET_ATTRIBUTES(RSP_READ_STRUCT);

	return err;
}

int dpbp_get_irq(struct dpbp *dpbp,
		 uint8_t irq_index,
		 int *type,
		 uint64_t *irq_paddr,
		 uint32_t *irq_val,
		 int *irq_virt_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPBP_CMD_GET_IRQ(CMD_PREP);

	err = dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_GET_IRQ,
			 DPBP_CMDSZ_GET_IRQ, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPBP_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpbp_set_irq(struct dpbp *dpbp,
		 uint8_t irq_index,
		 uint64_t irq_paddr,
		 uint32_t irq_val,
		 int irq_virt_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPBP_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_SET_IRQ,
			  DPBP_CMDSZ_SET_IRQ, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpbp_get_irq_enable(struct dpbp *dpbp,
			uint8_t irq_index,
			uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPBP_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_GET_IRQ_ENABLE,
			 DPBP_CMDSZ_GET_IRQ_ENABLE, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPBP_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dpbp_set_irq_enable(struct dpbp *dpbp,
			uint8_t irq_index,
			uint8_t enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPBP_CMD_SET_IRQ_ENABLE(CMD_PREP);

	/* send command to mc*/
	return dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_SET_IRQ_ENABLE,
			  DPBP_CMDSZ_SET_IRQ_ENABLE, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpbp_get_irq_mask(struct dpbp *dpbp, uint8_t irq_index, uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPBP_CMD_GET_IRQ_MASK(CMD_PREP);

	err = dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_GET_IRQ_MASK,
			 DPBP_CMDSZ_GET_IRQ_MASK, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPBP_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpbp_set_irq_mask(struct dpbp *dpbp, uint8_t irq_index, uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPBP_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_SET_IRQ_MASK,
			  DPBP_CMDSZ_SET_IRQ_MASK, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}

int dpbp_get_irq_status(struct dpbp *dpbp, uint8_t irq_index, uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPBP_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_GET_IRQ_STATUS,
			 DPBP_CMDSZ_GET_IRQ_STATUS, CMDIF_PRI_LOW,
			 (uint8_t *)&cmd_data);
	if (!err)
		DPBP_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dpbp_clear_irq_status(struct dpbp *dpbp, uint8_t irq_index, uint32_t status)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPBP_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	/* send command to mc*/
	return dplib_send(&(dpbp->regs), (uint16_t)dpbp->auth, DPBP_CMDID_CLEAR_IRQ_STATUS,
			  DPBP_CMDSZ_CLEAR_IRQ_STATUS, CMDIF_PRI_LOW,
			  (uint8_t *)&cmd_data);
}
