#include <errno.h>
#include <types.h>
#include <fsl_aiop_cmdif.h>
#include <cmdif_client.h>

struct nadk_buf;

/*! Frame descriptor relevant fields as should be set by cmdif */
struct cmd_fd {
	union {
		uint64_t flc;
		struct {
			uint8_t dev_h;     /*!< 7 high bits of cmdif_desc.dev */
			uint8_t err;       /*!< Reserved for error on response */
			uint16_t auth_id;  /*!< Authentication id */
			uint16_t cmid;     /*!< Command id */
			uint16_t reserved; /*!< Reserved fog EPID */
		} cmd;
		struct {
			uint8_t inst_id;    /*!< Module instance id*/
			uint8_t reserved0;
			uint16_t auth_id;   /*!< Authentication id */
			uint16_t cmid;      /*!< Command id */
			uint16_t reserved1; /*!< Reserved fog EPID */
		} open;
		/*!< Open command is always synchronous */
		struct {
			uint8_t reserved[2];
			uint16_t auth_id;   /*!< Authentication id */
			uint16_t cmid;      /*!< Command id */
			uint16_t reserved1; /*!< Reserved fog EPID */
		} close;
		/*!< Close command is always synchronous*/

	} flc_u;

	union {
		uint32_t frc;	
		struct {
			uint32_t dev_l;   /*!< 32 low bit of cmdif_desc.dev */
		} cmd;
	} frc_u;
	
	struct {
		uint8_t err;        /*!< Reserved for error on response */
		char m_name[M_NAME_CHARS]; /*!< Module name that was registered */
	} open_data;
};


static void cmd_id_set(uint16_t cmd_id, struct nadk_buf * nb) 
{
/*
	uint64_t data = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	data &= ~CMD_ID_MASK;
	data |= (((uint64_t)cmd_id) << CMD_ID_OFF);
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, data);		
*/
}

/** Module name is first 8 bytes inside data */
static void cmd_m_name_set(const char *name, struct nadk_buf * nb)
{
/*
	uint8_t * addr = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
	addr += PRC_GET_SEGMENT_OFFSET() + SYNC_BUFF_RESERVED;

	 I expect that name will end by \0 if it has less than 8 chars 
	if (name != NULL) {
		if ((PRC_GET_SEGMENT_LENGTH() >= M_NAME_CHARS) &&
			(addr != NULL)) {
			strncpy((char *)addr, name, M_NAME_CHARS);
		}
	}
*/
}

static void cmd_inst_id_set(uint8_t inst_id, struct nadk_buf * nb)
{
/*
	uint32_t frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) & ~INST_ID_MASK;
	
	frc |= inst_id;	
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, frc);	
*/
}

static void cmd_auth_id_set(uint16_t auth_id, struct nadk_buf * nb)
{
/*
	uint64_t data = 0;
	data = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS) & ~AUTH_ID_MASK;
	data |= (((uint64_t)auth_id) << AUTH_ID_OFF);
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, data);	
*/
}

static void cmd_dev_set(void *dev, struct nadk_buf * nb)
{
	/* Virtual address for linux is 39 bit, we need only virtual addr */
/*
	uint64_t data = 0;
	uint64_t dev_for_asynch = (uint64_t)dev;
	
	 Low 
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, ((uint32_t)dev_for_asynch));
	 High 
	data = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS) & ~DEV_H_MASK;
	data |=  (dev_for_asynch & 0x0000007F00000000) << DEV_H_OFF;
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, data);		
*/
}

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
		const char *module_name,
		uint8_t instance_id,
		cmdif_cb_t async_cb,
		void *async_ctx)
{
	uint16_t cmd_id = CMD_ID_OPEN;
	struct   nadk_buf *nb = NULL;
	
	cmd_id_set(cmd_id, nb); 
	/* TODO sync addr is FD[ADDR] which should be of 9 bytes size */
	cmd_m_name_set(module_name, nb);	
	cmd_inst_id_set(instance_id, nb);
	cmd_auth_id_set(OPEN_AUTH_ID, nb);
	
}


int cmdif_close(struct cmdif_desc *cidesc)
{
	
}


int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int priority,
		uint8_t *data)
{
	
}
