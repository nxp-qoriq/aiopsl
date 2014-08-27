/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

	/* prepare command */
	DPBP_CMD_CREATE(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpbp->auth = 0;

	/* send command to mc*/
	return dplib_send(dpbp->regs, &(dpbp->auth), MC_DPBP_CMDID_CREATE,
				DPBP_CMDSZ_CREATE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpbp_open(struct dpbp *dpbp, int dpbp_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPBP_CMD_OPEN(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpbp->auth = 0;

	/* send command to mc*/
	return dplib_send(dpbp->regs, &(dpbp->auth), MC_DPBP_CMDID_OPEN,
				MC_CMD_OPEN_SIZE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpbp_close(struct dpbp *dpbp)
{
	return dplib_send(dpbp->regs, &(dpbp->auth), MC_DPBP_CMDID_CLOSE,
				MC_CMD_CLOSE_SIZE, MC_CMD_PRI_HIGH, NULL);
}

int dpbp_destroy(struct dpbp *dpbp)
{
	return dplib_send(dpbp->regs, &(dpbp->auth), MC_DPBP_CMDID_DESTROY,
				DPBP_CMDSZ_DESTROY, MC_CMD_PRI_LOW, NULL);
}

int dpbp_enable(struct dpbp *dpbp)
{
	return dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_ENABLE,
				DPBP_CMDSZ_ENABLE, MC_CMD_PRI_LOW, NULL);
}

int dpbp_disable(struct dpbp *dpbp)
{
	return dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_DISABLE,
				DPBP_CMDSZ_DISABLE, MC_CMD_PRI_LOW, NULL);
}

int dpbp_get_attributes(struct dpbp *dpbp, struct dpbp_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_GET_ATTR,
			 DPBP_CMDSZ_GET_ATTR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPBP_RSP_GET_ATTRIBUTES(RSP_READ_STRUCT);

	return err;
}

int dpbp_get_irq(struct dpbp *dpbp,
		 uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPBP_CMD_GET_IRQ(CMD_PREP);

	err = dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_GET_IRQ,
			 DPBP_CMDSZ_GET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
	if (!err)
		DPBP_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpbp_set_irq(struct dpbp *dpbp,
		 uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPBP_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_SET_IRQ,
				DPBP_CMDSZ_SET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
}

int dpbp_get_irq_enable(struct dpbp *dpbp,
			uint8_t irq_index,
	uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPBP_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_GET_IRQ_ENABLE,
			 DPBP_CMDSZ_GET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
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
	return dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_SET_IRQ_ENABLE,
				DPBP_CMDSZ_SET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpbp_get_irq_mask(struct dpbp *dpbp, uint8_t irq_index, uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPBP_CMD_GET_IRQ_MASK(CMD_PREP);

	err = dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_GET_IRQ_MASK,
			 DPBP_CMDSZ_GET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
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
	return dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_SET_IRQ_MASK,
				DPBP_CMDSZ_SET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpbp_get_irq_status(struct dpbp *dpbp, uint8_t irq_index, uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPBP_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = dplib_send(dpbp->regs, &(dpbp->auth), DPBP_CMDID_GET_IRQ_STATUS,
			 DPBP_CMDSZ_GET_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPBP_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dpbp_clear_irq_status(struct dpbp *dpbp,
			  uint8_t irq_index,
	uint32_t status)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPBP_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpbp->regs, &(dpbp->auth),
				DPBP_CMDID_CLEAR_IRQ_STATUS,
				DPBP_CMDSZ_CLEAR_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
}
