#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "cmdif_srv.h"
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
#include "fsl_cmdif_flib.h"

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

#if 0
__HOT_CODE static int cmdif_fd_send(int cb_err)
{
	int err;
	uint64_t flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);

	/** ERROR is not overridden by FDMA store */
	flc &= ~ERROR_MASK;
	flc |= ((uint64_t)cb_err) << ERROR_OFF;
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, flc);

	pr_debug("Response QID = 0x%x\n", RESP_QID_GET);
	pr_debug("CB error = %d\n", cb_err);

	err = (int)fdma_store_and_enqueue_default_frame_fqid(
					RESP_QID_GET, FDMA_EN_TC_CONDTERM_BITS);
	return err;
}

__HOT_CODE static void sync_cmd_done(uint64_t sync_done,
				int err,
				uint16_t auth_id,
				struct cmdif_srv *srv,
				char terminate)
{
	uint32_t resp = SYNC_CMD_RESP_MAKE(err, auth_id);
	uint64_t _sync_done = NULL;

	pr_debug("err = %d\n", err);
	pr_debug("auth_id = 0x%x\n", auth_id);
	pr_debug("sync_resp = 0x%x\n", resp);

	/* Delete FDMA handle and store user modified data */
	fdma_store_default_frame_data();
	if ((sync_done != NULL) || (auth_id == OPEN_AUTH_ID))
		_sync_done = sync_done;
	else
		_sync_done = srv->sync_done[auth_id];

	if (_sync_done == NULL) {
		pr_err("Can't finish sync command, no valid address\n");
		/** In this case client will fail on timeout */
	} else if (cdma_write(_sync_done, &resp, 4)) {
		pr_err("CDMA write failed, can't finish sync command\n");
		/** In this case client will fail on timeout */
	}

	if (terminate)
		fdma_terminate_task();
}

/** Save the address for polling on synchronous commands */
#define sync_done_get() LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS)
static void sync_done_set(uint16_t auth_id, struct   cmdif_srv *srv)
{
	srv->sync_done[auth_id] = sync_done_get(); /* Phys addr for cdma */
}

#endif

void cmdif_srv_cb(struct cmdif_fd *cfd)
{
	uint16_t cmd_id = cmd_id_get();
	int      err    = 0;
	uint16_t auth_id = cmd_auth_id_get();

	if (srv == NULL)
		PR_ERR_TERMINATE("Could not find CMDIF Server handle\n");

	pr_debug("cmd_id = 0x%x\n", cmd_id);
	pr_debug("auth_id = 0x%x\n", auth_id);

	if (cmd_id & CMD_ID_OPEN) {
		char     m_name[M_NAME_CHARS + 1];
		int      m_id;
		uint8_t  inst_id;
		int      new_inst;
		uint64_t sync_done = sync_done_get();

		/* OPEN will arrive with hash value 0xffff */
		if (auth_id != OPEN_AUTH_ID) {
			pr_err("No permission to open device 0x%x\n", auth_id);
			sync_cmd_done(sync_done, -EPERM, auth_id, srv, TRUE);
		}

		cmd_m_name_get(&m_name[0]);
		m_id = module_id_find(m_name, srv);
		if (m_id < 0) {
			/* Did not find module with such name */
			pr_err("No such module %s\n", m_name);
			sync_cmd_done(sync_done, -ENODEV, auth_id, srv, TRUE);
		}

		inst_id  = cmd_inst_id_get();
		new_inst = inst_alloc(srv);
		if (new_inst >= 0) {

			pr_debug("inst_id = %d\n", inst_id);
			pr_debug("new_inst = %d\n", new_inst);
			pr_debug("m_name = %s\n", m_name);

			sync_done_set((uint16_t)new_inst, srv);
			err = OPEN_CB(m_id, inst_id, new_inst);
			sync_cmd_done(sync_done, err,
					(uint16_t)new_inst, srv, FALSE);
			if (err) {
				pr_err("Open callback failed\n");
				inst_dealloc(new_inst, srv);
			}
			pr_debug("PASSED open command\n");
			fdma_terminate_task();
		} else {
			/* couldn't find free place for new device */
			sync_cmd_done(sync_done, -ENODEV, auth_id, srv, FALSE);
			PR_ERR_TERMINATE("No free entry for new device\n");
		}
	} else if (cmd_id & CMD_ID_CLOSE) {

		if (IS_VALID_AUTH_ID(auth_id)) {
			/* Don't reorder this sequence !!*/
			err = CLOSE_CB(auth_id);
			sync_cmd_done(NULL, err, auth_id, srv, FALSE);
			if (!err) {
				/* Free instance entry only if we had no error
				 * otherwise it will be impossible to retry to
				 * close the device */
				inst_dealloc(auth_id, srv);
			}
			pr_debug("PASSED close command\n");
			fdma_terminate_task();
		} else {
			sync_cmd_done(NULL, -EPERM, auth_id, srv, FALSE);
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}
	} else {
		uint32_t size	 = cmd_size_get();
		uint8_t  *data	 = cmd_data_get();

		if (IS_VALID_AUTH_ID(auth_id)) {
			/* User can ignore data and use presentation context */
			err = CTRL_CB(auth_id, cmd_id, size, data);
			if (SYNC_CMD(cmd_id)) {
				pr_debug("PASSED Synchronous Command\n");
				sync_cmd_done(NULL, err, auth_id, srv, TRUE);
			}
		} else {
			/* don't bother to send response
			 * in order not to overload response queue,
			 * it might be intentional attack
			 * */
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}
	}

	if (SEND_RESP(cmd_id)) {
		pr_debug("PASSED Asynchronous Command\n");
		err = cmdif_fd_send(err);
		if (err) {
			pr_err("Failed to send response auth_id = 0x%x\n",
			auth_id);
		}
	} else {
		/* CMDIF_NORESP_CMD store user modified data but don't send */
		pr_debug("PASSED No Response Command\n");
		fdma_store_default_frame_data();
	}
	fdma_terminate_task();
}

#pragma pop
#endif
