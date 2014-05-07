#include <errno.h>
#include <types.h>
#include <fsl_aiop_cmdif.h>
#include <cmdif_client.h>
#include <fsl_cmdif_flib.h>

#define SEND_FD(FD, PR, SEND_DEV)     (int)(PR == PR)
#define RECEIVE_FD(FD, PR, SEND_DEV)  (int)(PR == PR)

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
	
	err = SEND_FD(&fd, CMDIF_PRI_LOW, cidesc->regs);
	
	/* Wait for response from Server */
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

	err = SEND_FD(&fd, CMDIF_PRI_LOW, cidesc->regs);

	/* Wait for response from Server */
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
	
	err = SEND_FD(&fd, priority, cidesc->regs);
	
	if (cmdif_is_sync_cmd(cmd_id)) {
		/* Wait for response from Server */
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
	
	err = RECEIVE_FD(&fd, priority, cidesc->regs);
	if (err)
		return err;
	while(!err) {
		err = cmdif_async_cb(&fd);
		if (err)
			return err;
		err = RECEIVE_FD(&fd, priority, cidesc->regs);
	}
	return 0;
}

