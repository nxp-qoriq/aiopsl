/* Copyright 2013-2016 Freescale Semiconductor Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <fsl_mc_sys.h>
#include <fsl_mc_cmd.h>
#include <fsl_dpbp.h>
#include <fsl_dpbp_cmd.h>

int dpbp_open(struct fsl_mc_io *mc_io,
	      uint32_t cmd_flags,
	      int dpbp_id,
	      uint16_t *token)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_OPEN,
					  cmd_flags,
					  0);
	DPBP_CMD_OPEN(cmd, dpbp_id);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	*token = MC_CMD_HDR_READ_TOKEN(cmd.header);

	return err;
}

int dpbp_close(struct fsl_mc_io *mc_io,
	       uint32_t cmd_flags,
	       uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_CLOSE, cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_create(struct fsl_mc_io *mc_io,
		uint16_t dprc_token,
		uint32_t cmd_flags,
		const struct dpbp_cfg *cfg,
		uint32_t *obj_id)
{
	struct mc_command cmd = { 0 };
	int err;

	(void)(cfg); /* unused */

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_CREATE,
					  cmd_flags,
					  dprc_token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	CMD_CREATE_RSP_GET_OBJ_ID_PARAM0(cmd, *obj_id);

	return 0;
}

int dpbp_destroy(struct fsl_mc_io *mc_io,
		 uint16_t dprc_token,
		uint32_t cmd_flags,
		uint32_t object_id)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_DESTROY,
					  cmd_flags,
					  dprc_token);
	/* set object id to destroy */
	CMD_DESTROY_SET_OBJ_ID_PARAM0(cmd, object_id);
	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_enable(struct fsl_mc_io *mc_io,
		uint32_t cmd_flags,
		uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_ENABLE, cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_disable(struct fsl_mc_io *mc_io,
		 uint32_t cmd_flags,
		 uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_DISABLE,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_is_enabled(struct fsl_mc_io *mc_io,
		    uint32_t cmd_flags,
		    uint16_t token,
		    int *en)
{
	struct mc_command cmd = { 0 };
	int err;
	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_IS_ENABLED, cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPBP_RSP_IS_ENABLED(cmd, *en);

	return 0;
}

int dpbp_reset(struct fsl_mc_io *mc_io,
	       uint32_t cmd_flags,
	       uint16_t token)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_RESET,
					  cmd_flags,
					  token);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_set_irq(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		 uint16_t		token,
		 uint8_t		irq_index,
		 struct dpbp_irq_cfg	*irq_cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_SET_IRQ,
					  cmd_flags,
					  token);

	DPBP_CMD_SET_IRQ(cmd, irq_index, irq_cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_get_irq(struct fsl_mc_io	*mc_io,
		 uint32_t		cmd_flags,
		 uint16_t		token,
		 uint8_t		irq_index,
		 int			*type,
		 struct dpbp_irq_cfg	*irq_cfg)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_GET_IRQ,
					  cmd_flags,
					  token);

	DPBP_CMD_GET_IRQ(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPBP_RSP_GET_IRQ(cmd, *type, irq_cfg);

	return 0;
}

int dpbp_set_irq_enable(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint8_t en)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_SET_IRQ_ENABLE,
					  cmd_flags,
					  token);

	DPBP_CMD_SET_IRQ_ENABLE(cmd, irq_index, en);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_get_irq_enable(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint8_t *en)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_GET_IRQ_ENABLE,
					  cmd_flags,
					  token);

	DPBP_CMD_GET_IRQ_ENABLE(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPBP_RSP_GET_IRQ_ENABLE(cmd, *en);

	return 0;
}

int dpbp_set_irq_mask(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
		      uint8_t irq_index,
		      uint32_t mask)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_SET_IRQ_MASK,
					  cmd_flags,
					  token);

	DPBP_CMD_SET_IRQ_MASK(cmd, irq_index, mask);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_get_irq_mask(struct fsl_mc_io *mc_io,
		      uint32_t cmd_flags,
		      uint16_t token,
		      uint8_t irq_index,
		      uint32_t *mask)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_GET_IRQ_MASK,
					  cmd_flags,
					  token);

	DPBP_CMD_GET_IRQ_MASK(cmd, irq_index);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPBP_RSP_GET_IRQ_MASK(cmd, *mask);

	return 0;
}

int dpbp_get_irq_status(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			uint8_t irq_index,
			uint32_t *status)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_GET_IRQ_STATUS,
					  cmd_flags,
					  token);

	DPBP_CMD_GET_IRQ_STATUS(cmd, irq_index, *status);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPBP_RSP_GET_IRQ_STATUS(cmd, *status);

	return 0;
}

int dpbp_clear_irq_status(struct fsl_mc_io *mc_io,
			  uint32_t cmd_flags,
			  uint16_t token,
			  uint8_t irq_index,
			  uint32_t status)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_CLEAR_IRQ_STATUS,
					  cmd_flags,
					  token);

	DPBP_CMD_CLEAR_IRQ_STATUS(cmd, irq_index, status);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_get_attributes(struct fsl_mc_io *mc_io,
			uint32_t cmd_flags,
			uint16_t token,
			struct dpbp_attr *attr)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_GET_ATTR,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPBP_RSP_GET_ATTRIBUTES(cmd, attr);

	return 0;
}

int dpbp_set_notifications(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		token,
			   struct dpbp_notification_cfg	*cfg)
{
	struct mc_command cmd = { 0 };

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_SET_NOTIFICATIONS,
					  cmd_flags,
					  token);

	DPBP_CMD_SET_NOTIFICATIONS(cmd, cfg);

	/* send command to mc*/
	return mc_send_command(mc_io, &cmd);
}

int dpbp_get_notifications(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			      uint16_t		token,
			      struct dpbp_notification_cfg	*cfg)
{
	struct mc_command cmd = { 0 };
	int err;

	/* prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_GET_NOTIFICATIONS,
					  cmd_flags,
					  token);

	/* send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	/* retrieve response parameters */
	DPBP_CMD_GET_NOTIFICATIONS(cmd, cfg);

	return 0;
}

int dpbp_get_api_version(struct fsl_mc_io *mc_io,
			 uint32_t cmd_flags,
			   uint16_t *major_ver,
			   uint16_t *minor_ver)
{
	struct mc_command cmd = { 0 };
	int err;

	cmd.header = mc_encode_cmd_header(DPBP_CMDID_GET_API_VERSION,
					cmd_flags,
					0);

	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;

	DPBP_RSP_GET_API_VERSION(cmd, *major_ver, *minor_ver);

	return 0;
}

int dpbp_get_num_free_bufs(struct fsl_mc_io *mc_io, uint32_t cmd_flags,
			   uint16_t token, uint32_t *num_free_bufs)
{
	struct mc_command	cmd = { 0 };
	int			err;

	/* Prepare command */
	cmd.header = mc_encode_cmd_header(DPBP_CMDID_GET_NUM_FREE_BUFS,
					  cmd_flags, token);
	/* Send command to mc*/
	err = mc_send_command(mc_io, &cmd);
	if (err)
		return err;
	/* Retrieve response */
	DPBP_CMD_GET_NUM_FREE_BUFS(cmd, *num_free_bufs);

	return 0;
}
