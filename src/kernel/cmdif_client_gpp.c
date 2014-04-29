#include <errno.h>
#include <types.h>
#include <fsl_aiop_cmdif.h>
#include <cmdif_client.h>

#define IS_VLD_OPEN_SIZE(SIZE) \
	((SIZE) >= (sizeof(struct cmdif_dev) + sizeof(union cmdif_data)))


#if 0
/** These functions define how GPP should setup the FD 
 * It does not include data_addr and data_lenght because it's setup by NADK API
 * This code is used to modify the defaults FD inside WS */
void client_open_cmd(struct cmdif_desc *client, void *sync_done); 
void client_close_cmd(struct cmdif_desc *client); 
void client_sync_cmd(struct cmdif_desc *client); 
void client_async_cmd(struct cmdif_desc *client); 
void client_no_resp_cmd(struct cmdif_desc *client); 

void client_open_cmd(struct cmdif_desc *client, void *sync_done) 
{
	uint16_t   cmd_id = CMD_ID_OPEN;
	const char *module = "ABCABC";	
	cmd_id_set(cmd_id); 
	cmd_m_name_set(module);	
	cmd_inst_id_set(3);
	cmd_auth_id_set(OPEN_AUTH_ID);
	((struct cmdif_dev *)client->dev)->sync_done = sync_done;
}

void client_close_cmd(struct cmdif_desc *client) 
{
	uint16_t cmd_id = CMD_ID_CLOSE;
	cmd_id_set(cmd_id);
	cmd_auth_id_set(((struct cmdif_dev *)client->dev)->auth_id); 
}

void client_sync_cmd(struct cmdif_desc *client) 
{
	uint16_t cmd_id = 0xCC; /* Any number */
	cmd_id_set(cmd_id); 
	cmd_auth_id_set(((struct cmdif_dev *)client->dev)->auth_id); 
}

void client_async_cmd(struct cmdif_desc *client) 
{
	uint16_t cmd_id = CMDIF_ASYNC_CMD | 0xA;
	cmd_id_set(cmd_id);
	cmd_auth_id_set(((struct cmdif_dev *)client->dev)->auth_id); 
	cmd_dev_set(client->dev);
}

void client_no_resp_cmd(struct cmdif_desc *client) 
{
	uint16_t cmd_id = CMDIF_NORESP_CMD | 0x2;
	cmd_id_set(cmd_id);
	cmd_auth_id_set(((struct cmdif_dev *)client->dev)->auth_id); 
}
#endif


int cmdif_open(struct cmdif_desc *cidesc,
		const char *m_name,
		uint8_t instance_id,
		cmdif_cb_t async_cb,
		void *async_ctx,
		uint8_t *v_data,
		uint64_t p_data,
		uint32_t size)
{			
	uint64_t p_addr = NULL;
	int      i = 0;
	union  cmdif_data *v_addr = NULL;
	union  cmdif_flc  flc;	
	struct cmdif_dev  *dev = NULL;
	
	
	if ((m_name == NULL) 
		|| (cidesc == NULL) 
		|| (!IS_VLD_OPEN_SIZE(size))
		|| (cidesc->regs == NULL)) {		
		return -EINVAL;
	}
	
	dev = (struct cmdif_dev *)v_data;	
	cidesc->dev = (void *)dev;
	p_addr = p_data + sizeof(struct cmdif_dev);
	v_addr = (union cmdif_data *)(v_data + sizeof(struct cmdif_dev));
	
	/* acquire lock as needed */
	if (cidesc->lock_cb)
		cidesc->lock_cb(cidesc->lock);

	v_addr->send.done = 0;
	/* 8 characters module name terminated by \0*/
	while ((m_name[i] != '\0') && (i < M_NAME_CHARS)) {
		v_addr->send.m_name[i] = m_name[i];
		i++;
	}
	if (i < M_NAME_CHARS)
		v_addr->send.m_name[i] = '\0';

	flc.flc = 0;
	flc.open.cmid    = CMD_ID_OPEN;
	flc.open.auth_id = OPEN_AUTH_ID;
	flc.open.inst_id = instance_id;	
	
	// TODO !!!! send(flc.flc, p_addr)
	
	/* Wait for response from Server */
	while (!v_addr->resp.done) {
		// TODO wait()
	}
	dev->auth_id = v_addr->resp.auth_id;

	/* release lock as needed */
	if (cidesc->unlock_cb)
		cidesc->unlock_cb(cidesc->lock);

	return v_addr->resp.err;
}


int cmdif_close(struct cmdif_desc *cidesc)
{
	if ((cidesc == NULL) 
		|| (cidesc->regs == NULL)
		|| (cidesc->dev == NULL)) {		
		return -EINVAL;
	}
	
	/* acquire lock as needed */
	if (cidesc->lock_cb)
		cidesc->lock_cb(cidesc->lock);
	
	/* release lock as needed */
	if (cidesc->unlock_cb)
		cidesc->unlock_cb(cidesc->lock);
}


int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int priority,
		uint64_t data)
{
	/* TODO same client can be used for MC -> AIOP */
	if ((cidesc == NULL) 
		|| (cidesc->regs == NULL)
		|| (cidesc->dev == NULL)) {		
		return -EINVAL;
	}

	/* acquire lock as needed */
	if ((cidesc->lock_cb) && !(cmd_id & CMDIF_ASYNC_CMD))
		cidesc->lock_cb(cidesc->lock);
	
	/* release lock as needed */
	if ((cidesc->unlock_cb) && !(cmd_id & CMDIF_ASYNC_CMD))
		cidesc->unlock_cb(cidesc->lock);
}
