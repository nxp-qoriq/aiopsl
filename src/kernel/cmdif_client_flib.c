#include <fsl_cmdif_flib.h>
#include <cmdif_client.h>
#include <errno.h>
#include <types.h>

#define IS_VLD_OPEN_SIZE(SIZE) \
	((SIZE) >= (sizeof(struct cmdif_dev) + sizeof(union cmdif_data)))

/** Blocking commands don't need response FD */
#define SYNC_CMD(CMD)	\
	(!((CMD) & (CMDIF_NORESP_CMD | CMDIF_ASYNC_CMD)))

int cmdif_is_sync_cmd(uint16_t cmd_id)
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
	if ((m_name == NULL) || (cidesc == NULL))
		return -EINVAL;

	if (!IS_VLD_OPEN_SIZE(size))
		return -ENOMEM;

	p_addr = p_data + sizeof(struct cmdif_dev);
	v_addr = (union cmdif_data *)(v_data + sizeof(struct cmdif_dev));

	fd->u_flc.flc          = 0;
	fd->u_flc.open.cmid    = CMD_ID_OPEN;
	fd->u_flc.open.auth_id = OPEN_AUTH_ID;
	fd->u_flc.open.inst_id = instance_id;
	fd->u_flc.open.epid    = CMDIF_EPID;
	fd->u_frc.frc          = 0;
	fd->u_addr.d_addr      = p_addr;
	fd->d_size             = sizeof(union cmdif_data);

	/* acquire lock as needed
	 * acquire lock before modifying any user data in order to avoid
	 * garbage data in case of race condition */
	if (cidesc->lock_cb)
		cidesc->lock_cb(cidesc->lock);

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

	return 0;
}

int cmdif_sync_ready(struct cmdif_desc *cidesc)
{
	struct cmdif_dev *dev = NULL;

	if ((cidesc == NULL) || (cidesc->dev == NULL))
		return -EINVAL;

	dev = (struct cmdif_dev *)cidesc->dev;

	if (dev->sync_done == NULL)
		return -EINVAL;

	return ((union  cmdif_data *)(dev->sync_done))->resp.done;
}

int cmdif_sync_cmd_done(struct cmdif_desc *cidesc)
{
	struct cmdif_dev *dev = NULL;

	if ((cidesc == NULL) || (cidesc->dev == NULL)) {
		/* prevent deadlocks */
		if (cidesc->unlock_cb)
			cidesc->unlock_cb(cidesc->lock);

		return -EINVAL;
	}

	dev = (struct cmdif_dev *)cidesc->dev;

	if (dev->sync_done == NULL) {		
		/* prevent deadlocks */
		if (cidesc->unlock_cb)
			cidesc->unlock_cb(cidesc->lock);
		
		return -EINVAL;
	}

	((union  cmdif_data *)(dev->sync_done))->resp.done = 0;

	/* release lock as needed */
	if (cidesc->unlock_cb)
		cidesc->unlock_cb(cidesc->lock);

	return ((union  cmdif_data *)(dev->sync_done))->resp.err;
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

	fd->u_addr.d_addr = NULL;
	fd->d_size = 0;
	fd->u_flc.flc           = 0;
	fd->u_flc.close.cmid    = CMD_ID_CLOSE;
	fd->u_flc.close.auth_id = dev->auth_id;
	fd->u_flc.open.epid     = CMDIF_EPID;

	/* acquire lock as needed */
	if (cidesc->lock_cb)
		cidesc->lock_cb(cidesc->lock);

	return 0;
}


int cmdif_close_done(struct cmdif_desc *cidesc)
{
	return cmdif_sync_cmd_done(cidesc);
}

int cmdif_cmd(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		uint64_t data,
		struct cmdif_fd *fd)
{
	struct cmdif_dev *dev = NULL;

	if ((cidesc == NULL) || (cidesc->dev == NULL))
		return -EINVAL;

	dev = (struct cmdif_dev *)cidesc->dev;

	fd->u_addr.d_addr = data;
	fd->d_size = size;
	fd->u_flc.flc = 0;
	fd->u_flc.cmd.auth_id = dev->auth_id;
	fd->u_flc.cmd.cmid = cmd_id;
	fd->u_flc.cmd.epid = CMDIF_EPID;
	fd->u_flc.cmd.dev_h = (uint8_t)((((uint64_t)dev) & 0xFF00000000) >> 32);
	fd->u_frc.cmd.dev_l = ((uint32_t)dev);

	/* acquire lock as needed */
	if ((cidesc->lock_cb) &&
		(!(cmd_id & (CMDIF_ASYNC_CMD | CMDIF_NORESP_CMD))))
		cidesc->lock_cb(cidesc->lock);

	return 0;
}

int cmdif_async_cb(struct cmdif_fd *fd)
{
	struct   cmdif_dev *dev = NULL;
	uint64_t fd_dev         = 0;

	if (fd == NULL)
		return -EINVAL;

	fd_dev = (uint64_t)(fd->u_frc.cmd.dev_l);
	fd_dev |= (((uint64_t)(fd->u_flc.cmd.dev_h)) << 32);
	dev    = (struct cmdif_dev *)fd_dev;

	if (dev->async_cb != NULL)
		return dev->async_cb(dev->async_ctx,
		                     fd->u_flc.cmd.err,
		                     fd->u_flc.cmd.cmid,
		                     fd->d_size,
		                     fd->u_addr.d_addr);
	else
		return -EINVAL;
}
