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
#include <fsl_dpci.h>
#include <fsl_dpci_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd_data.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

int dpci_create(struct dpci *dpci, const struct dpci_cfg *cfg)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCI_CMD_CREATE(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpci->auth = 0;

	/* send command to mc*/
	return dplib_send(dpci->regs, &(dpci->auth), MC_DPCI_CMDID_CREATE,
				DPCI_CMDSZ_CREATE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpci_open(struct dpci *dpci, int dpci_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCI_CMD_OPEN(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dpci->auth = 0;

	/* send command to mc*/
	return dplib_send(dpci->regs, &(dpci->auth), MC_DPCI_CMDID_OPEN,
				MC_CMD_OPEN_SIZE, MC_CMD_PRI_LOW, &cmd_data);
}

int dpci_close(struct dpci *dpci)
{
	return dplib_send(dpci->regs, &(dpci->auth), MC_DPCI_CMDID_CLOSE,
				MC_CMD_CLOSE_SIZE, MC_CMD_PRI_HIGH, NULL);
}

int dpci_destroy(struct dpci *dpci)
{
	return dplib_send(dpci->regs, &(dpci->auth), MC_DPCI_CMDID_DESTROY,
				DPCI_CMDSZ_DESTROY, MC_CMD_PRI_LOW, NULL);
}

int dpci_enable(struct dpci *dpci)
{
	return dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_ENABLE,
				DPCI_CMDSZ_ENABLE, MC_CMD_PRI_LOW, NULL);
}

int dpci_disable(struct dpci *dpci)
{
	return dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_DISABLE,
				DPCI_CMDSZ_DISABLE, MC_CMD_PRI_LOW, NULL);
}

int dpci_reset(struct dpci *dpci)
{
	return dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_RESET,
				DPCI_CMDSZ_RESET, MC_CMD_PRI_LOW, NULL);
}

int dpci_get_attributes(struct dpci *dpci, struct dpci_attr *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_GET_ATTR,
			 DPCI_CMDSZ_GET_ATTR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCI_RSP_GET_ATTR(RSP_READ_STRUCT);
	return err;
}

int dpci_get_link_state(struct dpci *dpci, int *up)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* send command to mc*/
	err = dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_GET_LINK_STATE,
			 DPCI_CMDSZ_GET_LINK_STATE, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCI_RSP_GET_LINK_STATE(RSP_READ);

	return err;
}

int dpci_set_rx_queue(struct dpci *dpci,
		      uint8_t priority,
	const struct dpci_dest_cfg *dest_cfg,
	uint64_t rx_user_ctx)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCI_CMD_SET_RX_QUEUE(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_SET_RX_QUEUE,
				DPCI_CMDSZ_SET_RX_QUEUE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpci_get_irq(struct dpci *dpci,
		 uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPCI_CMD_GET_IRQ(CMD_PREP);

	err = dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_GET_IRQ,
			 DPCI_CMDSZ_GET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
	if (!err)
		DPCI_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dpci_set_irq(struct dpci *dpci,
		 uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCI_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_SET_IRQ,
				DPCI_CMDSZ_SET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
}

int dpci_get_irq_enable(struct dpci *dpci,
			uint8_t irq_index,
	uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPCI_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_GET_IRQ_ENABLE,
			 DPCI_CMDSZ_GET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCI_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dpci_set_irq_enable(struct dpci *dpci,
			uint8_t irq_index,
	uint8_t enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCI_CMD_SET_IRQ_ENABLE(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_SET_IRQ_ENABLE,
				DPCI_CMDSZ_SET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpci_get_irq_mask(struct dpci *dpci, uint8_t irq_index, uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPCI_CMD_GET_IRQ_MASK(CMD_PREP);

	err = dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_GET_IRQ_MASK,
			 DPCI_CMDSZ_GET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCI_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dpci_set_irq_mask(struct dpci *dpci, uint8_t irq_index, uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCI_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_SET_IRQ_MASK,
				DPCI_CMDSZ_SET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dpci_get_irq_status(struct dpci *dpci, uint8_t irq_index, uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPCI_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = dplib_send(dpci->regs, &(dpci->auth), DPCI_CMDID_GET_IRQ_STATUS,
			 DPCI_CMDSZ_GET_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPCI_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dpci_clear_irq_status(struct dpci *dpci,
			  uint8_t irq_index,
	uint32_t status)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPCI_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dpci->regs, &(dpci->auth),
				DPCI_CMDID_CLEAR_IRQ_STATUS,
				DPCI_CMDSZ_CLEAR_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
}
