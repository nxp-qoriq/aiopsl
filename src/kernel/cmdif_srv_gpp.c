#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "general.h"
//#include "fsl_ldpaa_aiop.h"
#include "io.h"
//#include "fsl_fdma.h"
//#include "sys.h"
#include "fsl_malloc.h"
#include "dbg.h"
//#include "spinlock.h"
//#include "fsl_cdma.h"
//#include "aiop_common.h"
#include "errors.h"
#include "cmdif_srv.h"
#include "fsl_cmdif_flib_s.h"
#include "fsl_cmdif_client.h"

/** Blocking commands don't need response FD */
#define SEND_RESP(CMD)	\
	((!((CMD) & CMDIF_NORESP_CMD)) && ((CMD) & CMDIF_ASYNC_CMD))
/** Blocking commands don't need response FD */
#define SYNC_CMD(CMD)	\
	((!((CMD) & CMDIF_NORESP_CMD)) && !((CMD) & CMDIF_ASYNC_CMD))

#define OPEN_CB(M_ID, INST, DEV_ID) \
	(srv->open_cb[M_ID](INST, &srv->inst_dev[DEV_ID]))

#define CTRL_CB(AUTH_ID, CMD_ID, SIZE, DATA) \
	(srv->ctrl_cb[srv->m_id[AUTH_ID]](srv->inst_dev[AUTH_ID], \
	CMD_ID, SIZE, DATA))

#define CLOSE_CB(AUTH_ID) \
	(srv->close_cb[srv->m_id[AUTH_ID]](srv->inst_dev[AUTH_ID]))

#define FREE_MODULE    '\0'
#define TAKEN_INSTANCE (void *)0xFFFFFFFF
#define FREE_INSTANCE  NULL

#define SYNC_CMD_RESP_MAKE(ERR, ID)  (0x80000000 | ((ERR) << 16) | (ID))

#define IS_VALID_AUTH_ID(ID) \
	((srv->inst_dev != NULL) && ((ID) < M_NUM_OF_INSTANCES) && \
		(srv->inst_dev[(ID)]))

struct  cmdif_srv *srv;

#if 0

static uint16_t cmd_id_get()
{
	return 0;
}

static uint32_t cmd_size_get()
{
	return 0;
}

static uint8_t *cmd_data_get()
{
	return NULL; // TODO (uint8_t *)fsl_os_phys_to_virt(addr);
}

static void cmd_m_name_get(char *name)
{
	uint8_t * addr = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
	addr += PRC_GET_SEGMENT_OFFSET() + SYNC_BUFF_RESERVED;

	/* I expect that name will end by \0 if it has less than 8 chars */
	if (name != NULL) {
		name[0] = '\0';
		if ((PRC_GET_SEGMENT_LENGTH() >= M_NAME_CHARS) &&
			(addr != NULL)) {
			strncpy(name, (const char *)addr, M_NAME_CHARS);
		}
		name[M_NAME_CHARS] = '\0';
	}
}

static uint8_t cmd_inst_id_get()
{
	return 0;
}

static uint16_t cmd_auth_id_get()
{
	return 0;
}

static void srv_memory_free(struct  cmdif_srv *srv)
{
	if (srv->inst_dev)
		fsl_os_xfree(srv->inst_dev);
	if (srv->m_id)
		fsl_os_xfree(srv->m_id);
	if (srv->sync_done)
		fsl_os_xfree(srv->sync_done);
	if (srv->m_name)
		fsl_os_xfree(srv->m_name);
	if (srv->open_cb)
		fsl_os_xfree(srv->open_cb);
	if (srv->ctrl_cb)
		fsl_os_xfree(srv->ctrl_cb);
	if (srv->open_cb)
		fsl_os_xfree(srv->open_cb);

	fsl_os_xfree(srv);
}
#endif

static void *fast_malloc(int size)
{
	return fsl_os_xmalloc(size, MEM_PART_SH_RAM, 8);
}

static void *slow_malloc(int size)
{
	return fsl_os_xmalloc(size, MEM_PART_1ST_DDR_NON_CACHEABLE, 8);
}

static void srv_free(void *ptr)
{
	if (ptr != NULL)
		fsl_os_xfree(ptr);
}

int cmdif_srv_init(void)
{
	int     err = 0;

	srv = cmdif_srv_allocate(fast_malloc, slow_malloc);
	if (srv == NULL)
		return -ENOMEM;

	return err;
}

void cmdif_srv_free(void)
{
	cmdif_srv_deallocate(srv, srv_free);
}

int cmdif_register_module(const char *m_name, struct cmdif_module_ops *ops)
{
	/* Place here lock if required */

	return cmdif_srv_register(srv, m_name, ops);
}

int cmdif_unregister_module(const char *m_name)
{
	/* Place here lock if required */

	return cmdif_srv_unregister(srv, m_name);
}

static int send_fd(struct cmdif_fd *cfd, int pr, void *sdev)
{
	/* TODO complete */
	return 0;
}

int cmdif_srv_cb(struct cmdif_fd *cfd, int pr, void *send_dev)
{
	int    err = 0;
	struct cmdif_fd  cfd_out;

	if (srv == NULL)
		return -ENODEV;


	/* Call ctrl cb; if no perm cfd_out will be invalid */
	err = cmdif_srv_cmd(srv, auth_id, cmd_id, cfd, &cfd_out);
	/* don't bother to send response
	 * in order not to overload response queue,
	 * it might be intentional attack
	 * */
	if (err == -EPERM)
		return err;
	
	err = send_fd(&cfd_out, pr, send_dev);
	
	return err;
}

int cmdif_session_open(struct cmdif_desc *cidesc,
                       const char *m_name,
                       uint8_t inst_id,
                       uint8_t *v_data,
                       uint64_t p_data,
                       uint32_t size,
                       uint16_t *auth_id)
{
	int      err = 0;
	
	/*Call open_cb , Store dev */
	err = cmdif_srv_open(srv, m_name, inst_id, v_data, p_data, 
	                     size, auth_id);
	
	/*Send information to AIOP */
	err = cmdif_send(cidesc, CMD_ID_NOTIFY_OPEN, size, CMDIF_PRI_LOW,
	                 p_data);
	
	/*Check that AIOP received it */
	err = cmdif_srv_open_done(srv, *auth_id);
	
	return err;
	
}

int cmdif_session_close(struct cmdif_desc *cidesc, 
                        uint16_t auth_id)
{
	int      err = 0;
	uint16_t auth_id = 0;
	
	/*Call close_cb , place dpci_id, auth_id inside p_data */
	err = cmdif_srv_close(srv, auth_id, &p_data, &v_data, &size);
	
	/*Send information to AIOP */
	err = cmdif_send(cidesc, CMD_ID_NOTIFY_CLOSE, size, CMDIF_PRI_LOW, 
	                 p_data);
	
	/*Check that AIOP received it */
	err = cmdif_srv_close_done(v_data);
	
	return err;
	
}
