#include <fsl_cmdif_flib.h>
#include <cmdif_client.h>
#include <errno.h>

#define IS_VLD_OPEN_SIZE(SIZE) \
	((SIZE) >= (sizeof(struct cmdif_dev) + sizeof(union cmdif_data)))


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
	
	/* cidesc->dev = NULL is indication for unused cidesc 
	 * if cidesc->dev != NULL it maybe user error */
	if ((m_name == NULL) 
		|| (cidesc == NULL) 
		|| (!IS_VLD_OPEN_SIZE(size))
		|| (cidesc->regs == NULL)
		|| (cidesc->dev != NULL)) {		
		return -EINVAL;
	}
	
	p_addr = p_data + sizeof(struct cmdif_dev);
	v_addr = (union cmdif_data *)(v_data + sizeof(struct cmdif_dev));

	fd->u_flc.flc          = 0;
	fd->u_flc.open.cmid    = CMD_ID_OPEN;
	fd->u_flc.open.auth_id = OPEN_AUTH_ID;
	fd->u_flc.open.inst_id = instance_id;	
	fd->u_flc.open.epid    = CMDIF_EPID;
	fd->u_frc.frc          = 0;
	fd->d_addr             = p_addr;
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

int cmdif_sync_done(struct cmdif_desc *cidesc)
{
	struct cmdif_dev *dev = (struct cmdif_dev *)cidesc->dev;
	return ((union  cmdif_data *)(dev->sync_done))->resp.done;
}

int cmdif_sync_cmd_done(struct cmdif_desc *cidesc)
{
	/* release lock as needed */
	if (cidesc->unlock_cb)
		cidesc->unlock_cb(cidesc->lock);

	return ((union  cmdif_data *)(dev->sync_done))->resp.err;
}

int cmdif_open_done(struct cmdif_desc *cidesc)
{
	struct cmdif_dev *dev = (struct cmdif_dev *)cidesc->dev;

	((struct cmdif_dev *)(cidesc->dev))->auth_id = 
		((union  cmdif_data *)(dev->sync_done))->resp.auth_id;

	return cmdif_sync_cmd_done(cidesc);
}

int cmdif_close_cmd(struct cmdif_desc *cidesc, struct cmdif_fd *fd)
{
	struct cmdif_dev *dev = (struct cmdif_dev *)cidesc->dev;

	if ((cidesc == NULL) 
		|| (cidesc->regs == NULL)
		|| (dev == NULL)) {		
		return -EINVAL;
	}

	fd->d_addr = NULL;
	fd->d_size = 0;
	fd->u_flc.flc           = 0;
	fd->u_flc.close.cmid    = CMD_ID_CLOSE;
	fd->u_flc.close.auth_id = dev->auth_id;
	fd->u_flc.open.epid     = CMDIF_EPID;

	/* acquire lock as needed */
	if (cidesc->lock_cb)
		cidesc->lock_cb(cidesc->lock);
}


int cmdif_close_done(struct cmdif_desc *cidesc)
{	
	return cmdif_sync_cmd_done(cidesc);
}
