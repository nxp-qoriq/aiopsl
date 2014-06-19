#include <errno.h>
#include <types.h>
#include <cmdif_client.h>
#include <fsl_cmdif_flib_c.h>

int32_t nadk_send(struct nadk_dev *dev, void *vq, struct nadk_buf *buf[], uint32_t num);

/*
 * Pointer to this structure should be passed as cidesc->regs
 */
struct cmdif_reg {
	void   *nadk_dev;
	/* TODO complete */
};


static int send_fd(struct cmdif_fd *cfd, int pr, void *sdev)
{
	struct   cmdif_reg *dev = (struct cmdif_reg *)sdev;
	struct   nadk_buf nbuf;
	void     *vq = NULL;
	
	/* Copy FD fields into NASDK buff 
	 * cfd->u_frc.frc; 
	 * cfd->d_size; 
	 * cfd->u_flc.flc; 
	 * cfd->u_addr.addr;
	 * TODO complete
	 * */
	nbuf.length = cfd->d_size;
	
	/* Find queue according to priority 
	 * TODO implement */
	vq = find_queue(dev->nadk_dev, pr);
	
	nadk_send(dev->nadk_dev, vq, &nbuf, 1);
	
	return 0;
}

static int receive_fd(struct cmdif_fd *cfd, int pr, void *sdev)
{
	struct   cmdif_reg *dev = (struct cmdif_reg *)sdev;
	/* TODO complete */
	return 0;
}

int cmdif_open(struct cmdif_desc *cidesc,
		const char *m_name,
		uint8_t inst_id,
		cmdif_cb_t async_cb,
		void *async_ctx,
		uint8_t *v_data,
		uint64_t p_data,
		uint32_t size)
{
	struct cmdif_fd fd;
	int    err = 0;

	err = cmdif_open_cmd(cidesc, m_name, inst_id, async_cb, async_ctx,
	                     v_data, p_data, size, &fd);
	if (err)
		return err;

	err = send_fd(&fd, CMDIF_PRI_LOW, cidesc->regs);

	/* Wait for response from Server 
	 * TODO add timeout */
	while (!cmdif_sync_ready(cidesc)) {}

	return cmdif_open_done(cidesc);
}


int cmdif_close(struct cmdif_desc *cidesc)
{
	struct cmdif_fd fd;
	int    err = 0;

	err = cmdif_close_cmd(cidesc, &fd);
	if (err)
		return err;

	err = send_fd(&fd, CMDIF_PRI_LOW, cidesc->regs);

	/* Wait for response from Server 
	 * TODO add timeout */
	while (!cmdif_sync_ready(cidesc)) {}

	return cmdif_close_done(cidesc);
}

int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int priority,
		uint64_t data)
{
	struct cmdif_fd fd;
	int    err = 0;

	err = cmdif_cmd(cidesc, cmd_id, size, data, &fd);
	if (err)
		return err;

	err = send_fd(&fd, priority, cidesc->regs);

	if (cmdif_is_sync_cmd(cmd_id)) {
		/* Wait for response from Server 
		 * TODO add timeout */
		while (!cmdif_sync_ready(cidesc)) {}

		return cmdif_sync_cmd_done(cidesc);
	}

	return 0;
}

int cmdif_resp_read(struct cmdif_desc *cidesc, int priority)
{
	struct cmdif_fd fd;
	int    err = 0;

	if (cidesc == NULL)
		return -EINVAL;

	err = receive_fd(&fd, priority, cidesc->regs);
	if (err)
		return err;
	while(!err) {
		err = cmdif_async_cb(&fd);
		if (err)
			return err;
		err = receive_fd(&fd, priority, cidesc->regs);
	}
	return 0;
}

