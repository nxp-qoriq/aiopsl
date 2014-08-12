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
#include <fsl_dprc.h>
#include <fsl_dprc_cmd.h>

#define CMD_PREP(_param, _offset, _width, _type, _arg) \
	(cmd_data.params[_param] |= u64_enc((_offset), (_width), (_arg)))

#define RSP_READ(_param, _offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

#define RSP_READ_STRUCT(_param, _offset, _width, _type, _arg) \
	(_arg = (_type)u64_dec(cmd_data.params[_param], (_offset), (_width)))

int dprc_get_container_id(struct dprc *dprc, int *container_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	dprc->auth = 0;

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_CONT_ID,
			 DPRC_CMDSZ_GET_CONT_ID, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_CONTAINER_ID(RSP_READ);

	return err;
}

int dprc_open(struct dprc *dprc, int container_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_OPEN(CMD_PREP);

	/* clear 'auth' - later it will store the Authentication ID */
	dprc->auth = 0;

	/* send command to mc*/
	return dplib_send(dprc->regs, &(dprc->auth), MC_DPRC_CMDID_OPEN,
				MC_CMD_OPEN_SIZE, MC_CMD_PRI_LOW, &cmd_data);
}

int dprc_close(struct dprc *dprc)
{
	return dplib_send(dprc->regs, &(dprc->auth), MC_CMDID_CLOSE,
				MC_CMD_CLOSE_SIZE, MC_CMD_PRI_HIGH, NULL);
}

int dprc_create_container(struct dprc *dprc,
			  struct dprc_cfg *cfg,
	int *child_container_id,
	uint64_t *child_portal_paddr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPRC_CMD_CREATE_CONTAINER(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_CREATE_CONT,
			 DPRC_CMDSZ_CREATE_CONT, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_CREATE_CONTAINER(RSP_READ);

	return err;
}

int dprc_destroy_container(struct dprc *dprc, int child_container_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_DESTROY_CONTAINER(CMD_PREP);

	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_DESTROY_CONT,
				DPRC_CMDSZ_DESTROY_CONT, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dprc_reset_container(struct dprc *dprc, int child_container_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_RESET_CONTAINER(CMD_PREP);

	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_RESET_CONT,
				DPRC_CMDSZ_RESET_CONT, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dprc_set_res_quota(struct dprc *dprc,
		       int child_container_id,
	char *type,
	uint16_t quota)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_SET_RES_QUOTA(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_SET_RES_QUOTA,
				DPRC_CMDSZ_SET_RES_QUOTA, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dprc_get_res_quota(struct dprc *dprc,
		       int child_container_id,
	char *type,
	uint16_t *quota)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPRC_CMD_GET_RES_QUOTA(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_RES_QUOTA,
			 DPRC_CMDSZ_GET_RES_QUOTA, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_RES_QUOTA(RSP_READ);

	return err;
}

int dprc_assign(struct dprc *dprc,
		int container_id,
	struct dprc_res_req *res_req)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPRC_CMD_ASSIGN(CMD_PREP);

	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_ASSIGN,
				DPRC_CMDSZ_ASSIGN, MC_CMD_PRI_LOW, &cmd_data);
}

int dprc_unassign(struct dprc *dprc,
		  int child_container_id,
	struct dprc_res_req *res_req)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	DPRC_CMD_UNASSIGN(CMD_PREP);

	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_UNASSIGN,
				DPRC_CMDSZ_UNASSIGN, MC_CMD_PRI_LOW, &cmd_data);
}

int dprc_get_obj_count(struct dprc *dprc, int *obj_count)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_OBJ_COUNT,
			 DPRC_CMDSZ_GET_OBJ_COUNT, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_OBJ_COUNT(RSP_READ);

	return err;
}

int dprc_get_obj(struct dprc *dprc,
		 int obj_index,
	struct dprc_obj_desc *obj_desc)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;
	DPRC_CMD_GET_OBJECT(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_OBJECT,
			 DPRC_CMDSZ_GET_OBJECT, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_OBJECT(RSP_READ_STRUCT);

	return err;
}

int dprc_get_res_count(struct dprc *dprc, char *type, int *res_count)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	*res_count = 0;

	DPRC_CMD_GET_RES_COUNT(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_RES_COUNT,
			 DPRC_CMDSZ_GET_RES_COUNT, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_RES_COUNT(RSP_READ);

	return err;
}

int dprc_get_res_ids(struct dprc *dprc,
		     char *type,
	struct dprc_res_ids_range_desc *range_desc)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPRC_CMD_GET_RES_IDS(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_RES_IDS,
			 DPRC_CMDSZ_GET_RES_IDS, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_RES_IDS(RSP_READ_STRUCT);

	return err;
}

int dprc_get_attributes(struct dprc *dprc, struct dprc_attributes *attr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_ATTR,
			 DPRC_CMDSZ_GET_ATTR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_ATTRIBUTES(RSP_READ_STRUCT);

	return err;
}

int dprc_get_obj_region(struct dprc *dprc,
			char *obj_type,
	int obj_id,
	uint8_t region_index,
	struct dprc_region_desc *region_desc)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPRC_CMD_GET_OBJ_REGION(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_OBJ_REG,
			 DPRC_CMDSZ_GET_OBJ_REG, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_OBJ_REGION(RSP_READ_STRUCT);

	return err;
}

int dprc_get_irq(struct dprc *dprc,
		 uint8_t irq_index,
	int *type,
	uint64_t *irq_paddr,
	uint32_t *irq_val,
	int *user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPRC_CMD_GET_IRQ(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_IRQ,
			 DPRC_CMDSZ_GET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
	if (!err)
		DPRC_RSP_GET_IRQ(RSP_READ);

	return err;
}

int dprc_set_irq(struct dprc *dprc,
		 uint8_t irq_index,
	uint64_t irq_paddr,
	uint32_t irq_val,
	int user_irq_id)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_SET_IRQ(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_SET_IRQ,
				DPRC_CMDSZ_SET_IRQ, MC_CMD_PRI_LOW, &cmd_data);
}

int dprc_get_irq_enable(struct dprc *dprc,
			uint8_t irq_index,
	uint8_t *enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPRC_CMD_GET_IRQ_ENABLE(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_IRQ_ENABLE,
			 DPRC_CMDSZ_GET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_IRQ_ENABLE(RSP_READ);

	return err;
}

int dprc_set_irq_enable(struct dprc *dprc,
			uint8_t irq_index,
	uint8_t enable_state)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_SET_IRQ_ENABLE(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_SET_IRQ_ENABLE,
				DPRC_CMDSZ_SET_IRQ_ENABLE, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dprc_get_irq_mask(struct dprc *dprc, uint8_t irq_index, uint32_t *mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPRC_CMD_GET_IRQ_MASK(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_IRQ_MASK,
			 DPRC_CMDSZ_GET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_IRQ_MASK(RSP_READ);

	return err;
}

int dprc_set_irq_mask(struct dprc *dprc, uint8_t irq_index, uint32_t mask)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_SET_IRQ_MASK(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_SET_IRQ_MASK,
				DPRC_CMDSZ_SET_IRQ_MASK, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dprc_get_irq_status(struct dprc *dprc, uint8_t irq_index, uint32_t *status)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	DPRC_CMD_GET_IRQ_STATUS(CMD_PREP);

	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_IRQ_STATUS,
			 DPRC_CMDSZ_GET_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_IRQ_STATUS(RSP_READ);

	return err;
}

int dprc_clear_irq_status(struct dprc *dprc,
			  uint8_t irq_index,
	uint32_t status)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_CLEAR_IRQ_STATUS(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dprc->regs, &(dprc->auth),
				DPRC_CMDID_CLEAR_IRQ_STATUS,
				DPRC_CMDSZ_CLEAR_IRQ_STATUS, MC_CMD_PRI_LOW,
				&cmd_data);
}

int dprc_get_pool_count(struct dprc *dprc, int *pool_count)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* send command to mc*/
	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_POOL_COUNT,
			 DPRC_CMDSZ_GET_POOL_COUNT, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_POOL_COUNT(RSP_READ);

	return err;
}

int dprc_get_pool(struct dprc *dprc, int pool_index, char *type)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPRC_CMD_GET_POOL(CMD_PREP);

	/* send command to mc*/
	err = dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_GET_POOL,
			 DPRC_CMDSZ_GET_POOL, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_POOL(RSP_READ_STRUCT);

	return err;
}

int dprc_get_portal_paddr(struct dprc *dprc,
			  int portal_id,
	uint64_t *portal_addr)
{
	struct mc_cmd_data cmd_data = { { 0 } };
	int err;

	/* prepare command */
	DPRC_CMD_GET_PORTAL_PADDR(CMD_PREP);

	/* send command to mc*/
	err = dplib_send(dprc->regs, &(dprc->auth),
			 DPRC_CMDID_GET_PORTAL_PADDR,
				DPRC_CMDSZ_GET_PORTAL_PADDR, MC_CMD_PRI_LOW,
				&cmd_data);
	if (!err)
		DPRC_RSP_GET_PORTAL_PADDR(RSP_READ);

	return err;
}

int dprc_connect(struct dprc *dprc,
		 struct dprc_endpoint *endpoint1,
	struct dprc_endpoint *endpoint2)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_CONNECT(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_CONNECT,
				DPRC_CMDSZ_CONNECT, MC_CMD_PRI_LOW, &cmd_data);
}

int dprc_disconnect(struct dprc *dprc, struct dprc_endpoint *endpoint)
{
	struct mc_cmd_data cmd_data = { { 0 } };

	/* prepare command */
	DPRC_CMD_DISCONNECT(CMD_PREP);

	/* send command to mc*/
	return dplib_send(dprc->regs, &(dprc->auth), DPRC_CMDID_DISCONNECT,
				DPRC_CMDSZ_DISCONNECT, MC_CMD_PRI_LOW,
				&cmd_data);
}
