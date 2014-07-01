#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_io.h"
#include "fsl_fdma.h"
#include "sys.h"
#include "fsl_malloc.h"
#include "fsl_dbg.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_cdma.h"
#include "aiop_common.h"
#include "ls2085_aiop/fsl_platform.h"
#include "cmdif_srv_aiop.h"
#include "fsl_cmdif_flib_s.h"
#include "cmdif_client_aiop.h"

/** This is where rx qid should reside */
#define FQD_CTX_GET \
	(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fqd_ctx)
/** Get RX QID from dequeue context */
#define RESP_QID_GET \
	(uint32_t)(LLLDW_SWAP((uint32_t)&FQD_CTX_GET, 0) & 0xFFFFFFFF)
/** Blocking commands don't need response FD */
#define SEND_RESP(CMD)	\
	((!((CMD) & CMDIF_NORESP_CMD)) && ((CMD) != CMD_ID_NOTIFY_CLOSE) && \
		((CMD) != CMD_ID_NOTIFY_OPEN) && ((CMD) & CMDIF_ASYNC_CMD))
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
#define FREE_INSTANCE  (M_NUM_OF_MODULES)

#define SYNC_CMD_RESP_MAKE(ERR, ID)  (0x80000000 | \
	(((ERR) << 16) & 0x00FF0000) | (ID))

#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,         \
					0,                          \
					E_MAPPED_MEM_TYPE_GEN_REGS) \
					+ SOC_PERIPH_OFF_AIOP_WRKS);

#define PR_ERR_TERMINATE(...) \
	do {                  \
		pr_err(__VA_ARGS__);  \
		fdma_terminate_task();\
		return;               \
	} while (0)

#define IS_VALID_AUTH_ID(ID) \
	((srv->inst_dev != NULL) && ((ID) < M_NUM_OF_INSTANCES) && \
	(srv->m_id != NULL) && (srv->m_id[(ID)] < M_NUM_OF_MODULES))

static int module_id_find(const char *m_name, struct cmdif_srv *srv)
{
	int i = 0;

	if (m_name[0] == FREE_MODULE)
		return -EINVAL;

	for (i = 0; i < M_NUM_OF_MODULES; i++) {
		if (strncmp(srv->m_name[i], m_name, M_NAME_CHARS) == 0)
			return i;
	}

	return -ENAVAIL;
}

static int inst_alloc(struct cmdif_srv_aiop *aiop_srv, uint8_t m_id)
{
	int r = 0;
	int count = 0;
	struct cmdif_srv *srv = (struct cmdif_srv *)aiop_srv->srv;

	if (srv == NULL)
		return -EINVAL;

	lock_spinlock(&aiop_srv->lock);

	/* randomly pick instance/authentication id*/
	r = rand() % M_NUM_OF_INSTANCES;
	while ((srv->m_id[r] != FREE_INSTANCE) && (count < M_NUM_OF_INSTANCES)) {
		r = rand() % M_NUM_OF_INSTANCES;
		count++;
	}
	/* didn't find empty space yet */
	if (srv->m_id[r] != FREE_INSTANCE) {
		count = 0;
		while ((srv->m_id[r] != FREE_INSTANCE) &&
			(count < M_NUM_OF_INSTANCES)) {
			r = r++ % M_NUM_OF_INSTANCES;
			count++;
		}
	}

	/* didn't find empty space */
	if (count >= M_NUM_OF_INSTANCES) {
		unlock_spinlock(&aiop_srv->lock);
		return -ENAVAIL;
	} else {
		srv->m_id[r] = m_id;
		srv->inst_count++;
		unlock_spinlock(&aiop_srv->lock);
		return r;
	}
}

static void inst_dealloc(int inst, struct cmdif_srv_aiop *aiop_srv)
{
	struct cmdif_srv *srv = (struct cmdif_srv *)aiop_srv->srv;

	lock_spinlock(&aiop_srv->lock);
	srv->m_id[inst] = FREE_INSTANCE;
	srv->inst_count--;
	unlock_spinlock(&aiop_srv->lock);
}

__HOT_CODE static uint16_t cmd_id_get()
{
	uint64_t data = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	return (uint16_t)((data & CMD_ID_MASK) >> CMD_ID_OFF);
}

__HOT_CODE static uint32_t cmd_size_get()
{
	return LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
}

__HOT_CODE static uint64_t cmd_data_get()
{
	return (uint64_t)PRC_GET_SEGMENT_ADDRESS();
}

static void cmd_m_name_get(char *name)
{
	uint8_t * addr = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
	addr += PRC_GET_SEGMENT_OFFSET() + SYNC_BUFF_RESERVED;

	pr_debug("Read module name from 0x%x \n", addr);

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
	uint64_t data = 0;
	data = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	return (uint8_t)((data & INST_ID_MASK) >> INST_ID_OFF);
}

__HOT_CODE static uint16_t cmd_auth_id_get()
{
	uint64_t data = 0;
	data = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	return (uint16_t)((data & AUTH_ID_MASK) >> AUTH_ID_OFF);
}

int cmdif_register_module(const char *m_name, struct cmdif_module_ops *ops)
{
	struct cmdif_srv_aiop *srv_aiop = \
		sys_get_unique_handle(FSL_OS_MOD_CMDIF_SRV);
	struct cmdif_srv *srv = NULL;

	if (srv_aiop == NULL) {
		return -ENODEV;
	}
	srv = srv_aiop->srv;

	/* Place here lock if required */

	return cmdif_srv_register(srv, m_name, ops);
}

int cmdif_unregister_module(const char *m_name)
{
	struct cmdif_srv_aiop *srv_aiop = \
		sys_get_unique_handle(FSL_OS_MOD_CMDIF_SRV);
	struct cmdif_srv *srv = NULL;

	if (srv_aiop == NULL) {
		return -ENODEV;
	}
	srv = srv_aiop->srv;

	/* Place here lock if required */

	return cmdif_srv_unregister(srv, m_name);
}

static int epid_setup()
{
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;
	uint32_t data = 0;

	iowrite32(0, &wrks_addr->epas); /* EPID = 0 reserved for server */
	iowrite32(PTR_TO_UINT(cmdif_srv_isr), &wrks_addr->ep_pc);

#ifdef AIOP_STANDALONE
	/* Default settings */
	iowrite32(0x00600040, &wrks_addr->ep_fdpa);
	iowrite32(0x000002c0, &wrks_addr->ep_ptapa);
	iowrite32(0x00020300, &wrks_addr->ep_asapa);
	iowrite32(0x010001c0, &wrks_addr->ep_spa);
	iowrite32(0x00000000, &wrks_addr->ep_spo);
#endif
	/* Set mask for hash to 16 low bits OSRM = 5 */
	iowrite32(0x11000005, &wrks_addr->ep_osc);
	data = ioread32(&wrks_addr->ep_osc);
	if (data != 0x11000005)
		return -EINVAL;

	pr_info("CMDIF Server is setting EPID = 0\n");
	pr_info("ep_pc = 0x%x \n", ioread32(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x \n", ioread32(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x \n", ioread32(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x \n", ioread32(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x \n", ioread32(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x \n", ioread32(&wrks_addr->ep_spo));
	pr_info("ep_osc = 0x%x \n", ioread32(&wrks_addr->ep_osc));

	return 0;
}

static void *fast_malloc(int size)
{
	return fsl_os_xmalloc((size_t)size, MEM_PART_SH_RAM, 8);
}

static void *slow_malloc(int size)
{
	return fsl_os_xmalloc((size_t)size, MEM_PART_DP_DDR, 8);
}

static void srv_free(void *ptr)
{
	if (ptr != NULL)
		fsl_os_xfree(ptr);
}

int cmdif_srv_init(void)
{
	int  err = 0;
	void *srv = NULL;
	struct cmdif_srv_aiop *srv_aiop = NULL;

	if (sys_get_unique_handle(FSL_OS_MOD_CMDIF_SRV))
		return -ENODEV;

	err = epid_setup();
	if (err) {
		pr_err("EPID 0 is not setup correctly \n");
		return err;
	}

	srv_aiop = fast_malloc(sizeof(struct cmdif_srv_aiop));
	srv = cmdif_srv_allocate(fast_malloc, slow_malloc);

	if ((srv == NULL) || (srv_aiop == NULL)) {
		pr_err("Not enough memory for server allocation \n");
		return -ENOMEM;
	}
	srv_aiop->srv =srv;
	srv_aiop->dpci_tbl = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	err = sys_add_handle(srv_aiop, FSL_OS_MOD_CMDIF_SRV, 1, 0);

#ifndef OLD_DPCI
	if (srv_aiop->dpci_tbl == NULL)
	{
		pr_err("No DPCI table on AIOP, CMDIF is not functional \n");
		pr_info("All AIOP DPCIs should be defined in DPL\n");
		pr_info("All AIOP DPCIs should have peer before AIOP boot\n");
		return -ENODEV;
	}
#endif

	return err;
}

void cmdif_srv_free(void)
{
	void *srv = sys_get_unique_handle(FSL_OS_MOD_CMDIF_SRV);

	sys_remove_handle(FSL_OS_MOD_CMDIF_SRV, 0);

	cmdif_srv_deallocate(srv, srv_free);
}


__HOT_CODE static int cmdif_fd_send(int cb_err, struct cmdif_srv_aiop *aiop_srv)
{
	int err;
	uint64_t flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	uint32_t fqid = RESP_QID_GET;
#ifndef OLD_DPCI
	uint8_t  ind = 0;
	uint8_t  pr  = 0;
#endif
	/** ERROR is not overridden by FDMA store */
	flc &= ~ERROR_MASK;
	flc |= ((uint64_t)cb_err) << ERROR_OFF;
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, flc);


#ifndef OLD_DPCI
	ind = (uint8_t)(fqid >> 1);
	pr  = (uint8_t)(fqid & 1);
	fqid = aiop_srv->dpci_tbl->attr[ind].dpci_prio_attr[pr].tx_qid;
	 /* Do it only if queue is not there yet */
	if (fqid == 0xFFFFFFFF) {
		err = dpci_get_attributes(&aiop_srv->dpci_tbl->dpci[ind],
		                          &aiop_srv->dpci_tbl->attr[ind]);
		fqid = aiop_srv->dpci_tbl->attr[ind].dpci_prio_attr[pr].tx_qid;
	}
#endif

	pr_debug("Response ID = 0x%x\n", fqid);
	pr_debug("CB error = %d\n", cb_err);

	err = (int)fdma_store_and_enqueue_default_frame_fqid(
		fqid, FDMA_EN_TC_RET_BITS);
	if (err)
		pr_err("Failed to send response\n");

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
	} else {
		cdma_write(_sync_done, &resp, 4);
	}

	pr_debug("sync_done high = 0x%x low = 0x%x \n",
		 (uint32_t)((_sync_done & 0xFF00000000) >> 32),
		 (uint32_t)(_sync_done & 0xFFFFFFFF));

	if (terminate)
		fdma_terminate_task();
}

/** Save the address for polling on synchronous commands */
#define sync_done_get() LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS)
static void sync_done_set(uint16_t auth_id, struct   cmdif_srv *srv)
{
	srv->sync_done[auth_id] = sync_done_get(); /* Phys addr for cdma */
}
/** Find dpci index and get dpci table */
static int find_dpci(uint8_t dpci_id, struct dpci_obj **dpci_tbl)
{
	int i = 0;
	struct dpci_obj *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);
	*dpci_tbl = dt;

	if (dt == NULL)
		return -1;

	for (i = 0; i < dt->count; i++) {
		if (dt->attr[i].peer_id == dpci_id)
			return i;
	}
	return -1;
}

static int notify_open(struct cmdif_srv_aiop *aiop_srv)
{
	struct cmdif_srv *srv = aiop_srv->srv;
	struct cmdif_session_data *data = \
		(struct cmdif_session_data *)PRC_GET_SEGMENT_ADDRESS();
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int ind = 0;
	int link_up = 1;
	struct dpci_obj *dpci_tbl = NULL;
	int err = 0;

	pr_debug("Got notify open for AIOP client \n");
	if (PRC_GET_SEGMENT_LENGTH() < sizeof(struct cmdif_session_data)) {
		pr_err("Segment length is too small\n");
		return -EINVAL;
	}

	if (cl->count >= CMDIF_MN_SESSIONS) {
		pr_err("Too many sessions\n");
		return -ENOSPC;
	}

	ind = find_dpci((uint8_t)data->dev_id, &dpci_tbl);
	if (ind < 0) {
		pr_err("Not found DPCI peer %d\n", data->dev_id);
		return -ENAVAIL;
	}

	pr_debug("Found dpci peer id at index %d \n", ind);

	cl->gpp[cl->count].ins_id           = data->inst_id;
	cl->gpp[cl->count].dev->auth_id     = data->auth_id;
	cl->gpp[cl->count].dev->p_sync_done = cmd_data_get();
	cl->gpp[cl->count].dev->sync_done   = NULL; /* Not used in AIOP */
	strncpy(&cl->gpp[cl->count].m_name[0], &data->m_name[0], M_NAME_CHARS);
	cl->gpp[cl->count].m_name[M_NAME_CHARS] = '\0';
	cl->gpp[cl->count].regs->dpci_dev = &dpci_tbl->dpci[ind];
	cl->gpp[cl->count].regs->attr     = &dpci_tbl->attr[ind];

	/* TODO check with Ehud about  dpci_get_attributes() */
#if 0 //#ifdef DEBUG //TODO debug why it fails on MC when MC is server
	 /* DEBUG in order not to call MC inside task */
	 err = dpci_get_link_state(&dpci_tbl->dpci[ind], &link_up);
	 if (err) {
		 pr_err("Failed to get dpci_get_link_state\n");
	 }
#endif
	 /* Do it only if queues are not there */
	 if (dpci_tbl->attr[ind].dpci_prio_attr[0].tx_qid == 0xFFFFFFFF) {
		 err = dpci_get_attributes(&dpci_tbl->dpci[ind],
		                           &dpci_tbl->attr[ind]);
	 }

	if (!dpci_tbl->attr[ind].peer_attached ||
		!link_up) {
		pr_err("DPCI is not attached or there is no link \n");
		return -EACCES; /*Invalid device state*/
	}

	cl->count++;

	return 0;
}

static int notify_close(struct cmdif_srv_aiop *aiop_srv)
{
	struct cmdif_srv *srv = aiop_srv->srv;

	/* Support for AIOP -> GPP */

	return -ENOTSUP;
}

#pragma push
#pragma force_active on

__HOT_CODE void cmdif_srv_isr(void)
{
	uint16_t cmd_id = cmd_id_get();
	struct cmdif_srv_aiop *aiop_srv = \
		sys_get_unique_handle(FSL_OS_MOD_CMDIF_SRV);
	struct cmdif_srv *srv = NULL;
	int      err    = 0;
	uint16_t auth_id = cmd_auth_id_get();

	if ((aiop_srv == NULL) || (aiop_srv->srv == NULL))
		PR_ERR_TERMINATE("Could not find CMDIF Server handle\n");

	srv = aiop_srv->srv;

	pr_debug("cmd_id = 0x%x\n", cmd_id);
	pr_debug("auth_id = 0x%x\n", auth_id);

#ifdef DEBUG
	{
		uint32_t len = MIN(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS),\
				   PRC_GET_SEGMENT_LENGTH());
		uint8_t  *p = (uint8_t  *)PRC_GET_SEGMENT_ADDRESS();

		pr_debug("----- Dump of SEGMENT_ADDRESS 0x%x size %d -----\n",
			 p, len);
		while (len > 15)
		{
			fsl_os_print("0x%x: %x %x %x %x\r\n",
				     p, *(uint32_t *)p, *(uint32_t *)(p + 4),
				     *(uint32_t *)(p + 8), *(uint32_t *)(p + 12));
			len -= 16;
			p += 16;
		}
		while (len > 3)
		{
			fsl_os_print("0x%x: %x\r\n",
				     p, *(uint32_t *)p);
			len -= 4;
			p += 4;
		}

	}
#endif

	if (cmd_id == CMD_ID_NOTIFY_OPEN) {
		/* Support for AIOP -> GPP */
		if (IS_VALID_AUTH_ID(auth_id)) {
			err = notify_open(aiop_srv);
			sync_cmd_done(NULL, err, auth_id, srv, TRUE);
		} else {
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}

	} else if (cmd_id == CMD_ID_NOTIFY_CLOSE) {
		if (IS_VALID_AUTH_ID(auth_id)) {
			err = notify_close(aiop_srv);
			sync_cmd_done(NULL, err, auth_id, srv, TRUE);
		} else {
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}

	} else if (cmd_id == CMD_ID_OPEN) {
		char     m_name[M_NAME_CHARS + 1];
		int      m_id;
		uint8_t  inst_id;
		int      new_inst;
		uint64_t sync_done = sync_done_get();

		pr_debug("sync_done high = 0x%x low = 0x%x \n",
			 (uint32_t)((sync_done & 0xFF00000000) >> 32),
			 (uint32_t)(sync_done & 0xFFFFFFFF));

		/* OPEN will arrive with hash value 0xffff */
		if (auth_id != OPEN_AUTH_ID) {
			pr_err("No permission to open device 0x%x\n", auth_id);
			sync_cmd_done(sync_done, -EPERM, auth_id, srv, TRUE);
		}

		cmd_m_name_get(&m_name[0]);
		pr_debug("m_name = %s\n", m_name);

		m_id = module_id_find(m_name, srv);
		pr_debug("m_id = %d\n", m_id);

		if (m_id < 0) {
			/* Did not find module with such name */
			pr_err("No such module %s\n", m_name);
			sync_cmd_done(sync_done, -ENODEV, auth_id, srv, TRUE);
		}

		inst_id  = cmd_inst_id_get();
		new_inst = inst_alloc(aiop_srv, (uint8_t)m_id);
		if (new_inst >= 0) {

			pr_debug("inst_id = %d\n", inst_id);
			pr_debug("new_inst = %d\n", new_inst);

			sync_done_set((uint16_t)new_inst, srv);
			err = OPEN_CB(m_id, inst_id, new_inst);
			sync_cmd_done(sync_done, err,
					(uint16_t)new_inst, srv, FALSE);
			if (err) {
				pr_err("Open callback failed\n");
				inst_dealloc(new_inst, aiop_srv);
			}
			pr_debug("PASSED open command\n");
			fdma_terminate_task();
		} else {
			/* couldn't find free place for new device */
			sync_cmd_done(sync_done, -ENODEV, auth_id, srv, FALSE);
			PR_ERR_TERMINATE("No free entry for new device\n");
		}
	} else if (cmd_id == CMD_ID_CLOSE) {

		if (IS_VALID_AUTH_ID(auth_id)) {
			/* Don't reorder this sequence !!*/
			err = CLOSE_CB(auth_id);
			sync_cmd_done(NULL, err, auth_id, srv, FALSE);
			if (!err) {
				/* Free instance entry only if we had no error
				 * otherwise it will be impossible to retry to
				 * close the device */
				inst_dealloc(auth_id, aiop_srv);
			}
			pr_debug("PASSED close command\n");
			fdma_terminate_task();
		} else {
			/* don't bother to send response
			 * in order not to overload response queue,
			 * don't set done bit for invalid auth_id
			 * it might be intentional attack
			 * */
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}
	} else {
		if (IS_VALID_AUTH_ID(auth_id)) {
			/* User can ignore data and use presentation context */
			err = CTRL_CB(auth_id, cmd_id, cmd_size_get(), \
			              cmd_data_get());
			if (SYNC_CMD(cmd_id)) {
				pr_debug("PASSED Synchronous Command\n");
				sync_cmd_done(NULL, err, auth_id, srv, TRUE);
			}
		} else {
			/* don't bother to send response
			 * auth_id is not valid
			 * it might be intentional attack
			 * */
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}
	}

	if (SEND_RESP(cmd_id)) {
		pr_debug("PASSED Asynchronous Command\n");
		err = cmdif_fd_send(err, aiop_srv);
	} else {
		/* CMDIF_NORESP_CMD store user modified data but don't send */
		pr_debug("PASSED No Response Command\n");
		fdma_store_default_frame_data();
	}

	fdma_terminate_task();
}

#pragma pop
