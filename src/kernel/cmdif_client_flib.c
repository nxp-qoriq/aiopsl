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

#include <cmdif.h>
#include <fsl_cmdif_flib_c.h>
#include <cmdif_client.h>

#define IS_VLD_OPEN_SIZE(SIZE) \
	((SIZE) >= (sizeof(struct cmdif_dev) + sizeof(union cmdif_data)))

/** Server special command indication */
#define SPECIAL_CMD	0xC000

/** Blocking commands don't need response FD */
#define SYNC_CMD(CMD)	\
	(!((CMD) & (CMDIF_NORESP_CMD | CMDIF_ASYNC_CMD)) || (CMD & SPECIAL_CMD))


__HOT_CODE int cmdif_is_sync_cmd(uint16_t cmd_id)
{
	return SYNC_CMD(cmd_id);
}

int cmdif_open_cmd(struct cmdif_desc *cidesc,
			const char *m_name,
			uint8_t instance_id,
			cmdif_cb_t async_cb,
			void *async_ctx,
			uint8_t *v_data,
			uint64_t p_data,
			uint32_t size,
			struct cmdif_fd *fd)
{
	uint64_t p_addr = NULL;
	int      i = 0;
	union  cmdif_data *v_addr = NULL;
	struct cmdif_dev  *dev = NULL;

	/* if cidesc->dev != NULL it's ok,
	 * it's usefull to keep it in order to let user to free this buffer */
	if ((m_name == NULL)
		|| (cidesc == NULL)
		|| (v_data == NULL)
		|| (p_data == NULL))
		return -EINVAL;

	if (!IS_VLD_OPEN_SIZE(size))
		return -ENOMEM;

	memset(v_data, 0, size);

	p_addr = p_data + sizeof(struct cmdif_dev);
	v_addr = (union cmdif_data *)(v_data + sizeof(struct cmdif_dev));

	fd->u_flc.flc          = 0;
	fd->u_flc.open.cmid    = CPU_TO_SRV16(CMD_ID_OPEN);
	fd->u_flc.open.auth_id = CPU_TO_SRV16(OPEN_AUTH_ID);
	fd->u_flc.open.inst_id = instance_id;
	fd->u_flc.open.epid    = CPU_TO_BE16(CMDIF_EPID); /* Used by HW */
	fd->u_frc.frc          = 0;
	fd->u_addr.d_addr      = p_addr;
	fd->d_size             = sizeof(union cmdif_data);

	dev = (struct cmdif_dev *)v_data;
	dev->async_cb  = async_cb;
	dev->async_ctx = async_ctx;
	dev->sync_done = v_addr;
	cidesc->dev    = (void *)dev;

	v_addr->send.done = 0;
	/* 8 characters module name terminated by \0*/
	while ((m_name[i] != '\0') && (i < M_NAME_CHARS)) {
		v_addr->send.m_name[i] = m_name[i];
		i++;
	}
	if (i < M_NAME_CHARS)
		v_addr->send.m_name[i] = '\0';

	/* This is required because flc is a struct */
	fd->u_flc.flc = CPU_TO_BE64(fd->u_flc.flc);

	return 0;
}

__HOT_CODE int cmdif_sync_ready(struct cmdif_desc *cidesc)
{
	struct cmdif_dev *dev = NULL;

	if ((cidesc == NULL) || (cidesc->dev == NULL))
		return 0; /* Don't use POSIX on purpose */

	dev = (struct cmdif_dev *)cidesc->dev;

	if (dev->sync_done == NULL)
		return 0; /* Don't use POSIX on purpose */

	return ((union  cmdif_data *)(dev->sync_done))->resp.done;
}

__HOT_CODE int cmdif_sync_cmd_done(struct cmdif_desc *cidesc)
{
	struct cmdif_dev *dev = NULL;
	int    err = 0;

	if ((cidesc == NULL) || (cidesc->dev == NULL))
		return -EINVAL;

	dev = (struct cmdif_dev *)cidesc->dev;

	if (dev->sync_done == NULL)
		return -EINVAL;

	err = ((union  cmdif_data *)(dev->sync_done))->resp.err;
	((union  cmdif_data *)(dev->sync_done))->resp.done = 0;


	return err;
}

int cmdif_open_done(struct cmdif_desc *cidesc)
{
	struct cmdif_dev *dev = NULL;

	if ((cidesc == NULL) || (cidesc->dev == NULL))
		return -EINVAL;

	dev = (struct cmdif_dev *)cidesc->dev;

	if (dev->sync_done == NULL)
		return -EINVAL;

	dev->auth_id = ((union  cmdif_data *)(dev->sync_done))->resp.auth_id;

	return cmdif_sync_cmd_done(cidesc);
}

int cmdif_close_cmd(struct cmdif_desc *cidesc, struct cmdif_fd *fd)
{
	struct cmdif_dev *dev = NULL;

	if ((cidesc == NULL) || (cidesc->dev == NULL))
		return -EINVAL;

	dev = (struct cmdif_dev *)cidesc->dev;

	fd->u_addr.d_addr       = NULL;
	fd->d_size              = 0;
	fd->u_flc.flc           = 0;
	fd->u_flc.close.cmid    = CPU_TO_SRV16(CMD_ID_CLOSE);
	fd->u_flc.close.auth_id = dev->auth_id;
	fd->u_flc.close.epid    = CPU_TO_BE16(CMDIF_EPID); /* Used by HW */

	/* This is required because flc is a struct */
	fd->u_flc.flc = CPU_TO_BE64(fd->u_flc.flc);

	return 0;
}


int cmdif_close_done(struct cmdif_desc *cidesc)
{
	return cmdif_sync_cmd_done(cidesc);
}

__HOT_CODE int cmdif_cmd(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		uint64_t data,
		struct cmdif_fd *fd)
{
	struct cmdif_dev *dev = NULL;

	if ((cidesc == NULL) || (cidesc->dev == NULL))
		return -EINVAL;

	dev = (struct cmdif_dev *)cidesc->dev;

	fd->u_addr.d_addr     = data;
	fd->d_size            = size;
	fd->u_flc.flc         = 0;
	fd->u_flc.cmd.auth_id = dev->auth_id;
	fd->u_flc.cmd.cmid    = CPU_TO_SRV16(cmd_id);
	fd->u_flc.cmd.epid    = CPU_TO_BE16(CMDIF_EPID); /* Used by HW */
	fd->u_flc.cmd.dev_h = (uint8_t)((((uint64_t)dev) & 0xFF00000000) >> 32);
	fd->u_frc.cmd.dev_l = ((uint32_t)dev);

	/* This is required because flc is a struct but HW treats it as
	 * 8 byte LE.
	 * Therefore if CPU is LE which means that swap is not done
	 * by QMAN driver, we need to do it here */
	fd->u_flc.flc = CPU_TO_BE64(fd->u_flc.flc);

	return 0;
}

__HOT_CODE int cmdif_async_cb(struct cmdif_fd *fd, void *v_addr)
{
	struct   cmdif_dev *dev = NULL;
	uint64_t fd_dev         = 0;

	if (fd == NULL)
		return -EINVAL;


	/* This is required because flc is a struct but HW treats it as
	 * 8 byte LE.
	 * Therefore if CPU is LE which means that swap is not done
	 * by QMAN driver, we need to do it here */
	fd->u_flc.flc = CPU_TO_BE64(fd->u_flc.flc);

	fd_dev = (uint64_t)(fd->u_frc.cmd.dev_l);
	fd_dev |= (((uint64_t)(fd->u_flc.cmd.dev_h)) << 32);
	dev    = (struct cmdif_dev *)fd_dev;

	if (dev->async_cb != NULL)
		return dev->async_cb(dev->async_ctx,
				fd->u_flc.cmd.err,
				CPU_TO_SRV16(fd->u_flc.cmd.cmid),
				fd->d_size,
				(uint64_t)((v_addr != NULL) ? v_addr : fd->u_addr.d_addr));
	else
		return -EINVAL;
}
