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

#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "general.h"
#include "fsl_ldpaa_aiop.h"
#include "fsl_io.h"
#include "fsl_fdma.h"
#include "fdma.h"
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
#include "cmdif_rev.h"

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

#define OPEN_CB(M_ID, INST, DEV) \
	(cmdif_aiop_srv.srv->open_cb[M_ID](INST, &DEV))

#define CTRL_CB(AUTH_ID, CMD_ID, SIZE, DATA) \
	(cmdif_aiop_srv.srv->ctrl_cb[cmdif_aiop_srv.srv->m_id[AUTH_ID]] \
		(cmdif_aiop_srv.srv->inst_dev[AUTH_ID], CMD_ID, SIZE, DATA))

#define CLOSE_CB(AUTH_ID) \
	(cmdif_aiop_srv.srv->close_cb[cmdif_aiop_srv.srv->m_id[AUTH_ID]] \
		(cmdif_aiop_srv.srv->inst_dev[AUTH_ID]))

#define FREE_MODULE    '\0'
#define FREE_INSTANCE  (M_NUM_OF_MODULES)

#define SYNC_CMD_RESP_MAKE(ERR, ID)  (0x80000000 | \
	(((ERR) << 16) & 0x00FF0000) | (ID))

#define PR_ERR_TERMINATE(...) \
	do {                  \
		pr_err(__VA_ARGS__);  \
		fdma_terminate_task();\
		return;               \
	} while (0)


 __SHRAM static struct cmdif_srv_aiop cmdif_aiop_srv = {0};

 static inline int is_valid_auth_id(uint16_t id)
 {
	 return ((cmdif_aiop_srv.srv->inst_dev != NULL) &&
		 (id < M_NUM_OF_INSTANCES) &&
		 (cmdif_aiop_srv.srv->m_id != NULL) &&
		 (cmdif_aiop_srv.srv->m_id[id] < M_NUM_OF_MODULES));
 }

 static int module_id_find(const char *m_name)
{
	int i = 0;
	struct cmdif_srv *srv = cmdif_aiop_srv.srv;

	if (m_name[0] == FREE_MODULE)
		return -EINVAL;

	for (i = 0; i < M_NUM_OF_MODULES; i++) {
		if (strncmp(srv->m_name[i], m_name, M_NAME_CHARS) == 0)
			return i;
	}

	return -ENAVAIL;
}

static int inst_alloc(uint8_t m_id)
{
	uint32_t r = 0;
	int count = 0;
	struct cmdif_srv *srv = (struct cmdif_srv *)cmdif_aiop_srv.srv;

	if (srv == NULL)
		return -EINVAL;

	ASSERT_COND_LIGHT(is_power_of_2(M_NUM_OF_INSTANCES));
	
	lock_spinlock(&cmdif_aiop_srv.lock);

	/* randomly pick instance/authentication id*/
	r = MODULU_POWER_OF_TWO(fsl_os_rand(), M_NUM_OF_INSTANCES);
	while ((srv->m_id[r] != FREE_INSTANCE) && (count < M_NUM_OF_INSTANCES)) {
		r = MODULU_POWER_OF_TWO(fsl_os_rand(), M_NUM_OF_INSTANCES);
		count++;
	}
	/* didn't find empty space yet */
	if (srv->m_id[r] != FREE_INSTANCE) {
		count = 0;
		while ((srv->m_id[r] != FREE_INSTANCE) &&
			(count < M_NUM_OF_INSTANCES)) {
			r = MODULU_POWER_OF_TWO(r++, M_NUM_OF_INSTANCES);
			count++;
		}
	}

	/* didn't find empty space */
	if (count >= M_NUM_OF_INSTANCES) {
		unlock_spinlock(&cmdif_aiop_srv.lock);
		return -ENAVAIL;
	} else {
		srv->m_id[r] = m_id;
		srv->inst_count++;
		unlock_spinlock(&cmdif_aiop_srv.lock);
		return (int)r;
	}
}

static void inst_dealloc(int inst)
{
	struct cmdif_srv *srv = cmdif_aiop_srv.srv;

	lock_spinlock(&cmdif_aiop_srv.lock);
	srv->m_id[inst] = FREE_INSTANCE;
	srv->inst_count--;
	unlock_spinlock(&cmdif_aiop_srv.lock);
}

static inline uint16_t cmd_id_get()
{
	return (uint16_t)((LDPAA_FD_GET_FLC(HWC_FD_ADDRESS) & CMD_ID_MASK) \
		>> CMD_ID_OFF);
}

static inline uint32_t cmd_size_get()
{
	return LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
}

static inline void *cmd_data_get()
{
	return (void *)PRC_GET_SEGMENT_ADDRESS();
}

static void cmd_m_name_get(char *name)
{
	uint8_t * addr = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
	addr += PRC_GET_SEGMENT_OFFSET() + SYNC_BUFF_RESERVED;

/*	pr_debug("Read module name from 0x%x \n", addr);*/

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

static inline uint8_t cmd_inst_id_get()
{
	return (uint8_t)((LDPAA_FD_GET_FLC(HWC_FD_ADDRESS) & INST_ID_MASK) \
		>> INST_ID_OFF);
}

static inline uint16_t cmd_auth_id_get()
{
	return (uint16_t)((LDPAA_FD_GET_FLC(HWC_FD_ADDRESS) & AUTH_ID_MASK) \
		>> AUTH_ID_OFF);
}

int cmdif_register_module(const char *m_name, struct cmdif_module_ops *ops)
{
	/* Place here lock if required */

	return cmdif_srv_register(cmdif_aiop_srv.srv, m_name, ops);
}

int cmdif_unregister_module(const char *m_name)
{
	/* Place here lock if required */

	return cmdif_srv_unregister(cmdif_aiop_srv.srv, m_name);
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

	srv = cmdif_srv_allocate(fast_malloc, slow_malloc);

	if (srv == NULL) {
		pr_err("Not enough memory for server allocation \n");
		return -ENOMEM;
	}
	cmdif_aiop_srv.srv =srv;
	cmdif_aiop_srv.dpci_tbl = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	if (cmdif_aiop_srv.dpci_tbl == NULL)
	{
		pr_err("No DPCI table on AIOP, CMDIF is not functional \n");
		pr_info("All AIOP DPCIs should be defined in DPL\n");
		pr_info("All AIOP DPCIs should have peer before AIOP boot\n");
		return -ENODEV;
	}

	return err;
}

void cmdif_srv_free(void)
{
	cmdif_srv_deallocate(cmdif_aiop_srv.srv, srv_free);
	srv_free(cmdif_aiop_srv.dpci_tbl);
}


static int cmdif_fd_send(int cb_err)
{
	int err;
	uint64_t flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	uint32_t fqid = RESP_QID_GET;
	uint8_t  ind = 0;
	uint8_t  pr  = 0;

	flc &= ~ERROR_MASK;
	flc |= ((uint64_t)cb_err) << ERROR_OFF;
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, flc);


	ind = (uint8_t)(fqid >> 1);
	pr  = (uint8_t)(fqid & 1);
	fqid = cmdif_aiop_srv.dpci_tbl->tx_queue_attr[pr][ind].fqid;
	 /* Do it only if queue is not there yet */
	if (fqid == DPCI_FQID_NOT_VALID) {
		struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
		err = dpci_get_tx_queue(&dprc->io, 
		                        cmdif_aiop_srv.dpci_tbl->token[ind], pr, 
		                        &cmdif_aiop_srv.dpci_tbl->tx_queue_attr[pr][ind]);
		fqid = cmdif_aiop_srv.dpci_tbl->tx_queue_attr[pr][ind].fqid;
	}

	pr_debug("Response FQID = 0x%x pr = 0x%x dpci_ind = 0x%x\n", fqid, pr, ind);
	pr_debug("CB error = %d\n", cb_err);

	err = (int)fdma_store_and_enqueue_default_frame_fqid(
		fqid, CMDIF_FDMA_ENQ_TC);
	if (err)
		pr_err("Failed to send response\n");

	return err;
}

static void sync_cmd_done(uint64_t sync_done,
				int err,
				uint16_t auth_id,
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
		_sync_done = cmdif_aiop_srv.srv->sync_done[auth_id];

	if (_sync_done == NULL) {
		pr_err("Can't finish sync command, no valid address\n");
		/** In this case client will fail on timeout */
	} else {
		uint16_t pl_icid = PL_ICID_GET;
		uint32_t flags = FDMA_DMA_DA_WS_TO_SYS_BIT;

		/*
		 * It's ok to take it from current ADC and FD because this
		 * should not change between commands on the same session */
		ADD_AMQ_FLAGS(flags, pl_icid);
		pr_debug("icid = 0x%x\n", ICID_GET(pl_icid));
		pr_debug("fdma_dma_data flags = 0x%x\n", flags);
		fdma_dma_data(4, ICID_GET(pl_icid), &resp, _sync_done, flags);
	}

	pr_debug("sync_done high = 0x%x low = 0x%x \n",
		 (uint32_t)((_sync_done & 0xFF00000000) >> 32),
		 (uint32_t)(_sync_done & 0xFFFFFFFF));

	if (terminate)
		fdma_terminate_task();
}

/** Save the address for polling on synchronous commands */
#define sync_done_get() LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS)
static inline void sync_done_set(uint16_t auth_id)
{
	cmdif_aiop_srv.srv->sync_done[auth_id] = sync_done_get(); /* Phys addr for cdma */
}

/** Find dpci index and get dpci table */
static int find_dpci(uint8_t dpci_id)
{
	int i = 0;
	struct mc_dpci_obj *dt = cmdif_aiop_srv.dpci_tbl;

	for (i = 0; i < dt->count; i++) {
		if (dt->peer_attr[i].peer_id == dpci_id)
			return i;
	}
	return -1;
}

static void amq_bits_update(int ind)
{
	struct mc_dpci_obj *dpci_tbl = cmdif_aiop_srv.dpci_tbl;
	uint16_t pl_icid = PL_ICID_GET;


	dpci_tbl->icid[ind]           = ICID_GET(pl_icid);
	dpci_tbl->bdi_flags[ind]      = FDMA_EN_TC_RET_BITS; /* don't change */
	dpci_tbl->dma_flags[ind]      = FDMA_DMA_DA_SYS_TO_WS_BIT;
	ADD_AMQ_FLAGS(dpci_tbl->dma_flags[ind], pl_icid);
	if (BDI_GET != 0)
		dpci_tbl->bdi_flags[ind] |= FDMA_ENF_BDI_BIT;
}

/* Support for AIOP -> GPP */
static int notify_open()
{
	struct cmdif_session_data *data = \
		(struct cmdif_session_data *)PRC_GET_SEGMENT_ADDRESS();
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int ind = 0;
	int free_ind = 0;
	int link_up = 1;
	struct mc_dpci_obj *dpci_tbl = cmdif_aiop_srv.dpci_tbl;
	int err = 0;
	struct mc_dprc *dprc = NULL;
	uint8_t i;
	
	pr_debug("Got notify open for AIOP client \n");
	ASSERT_COND_LIGHT(dpci_tbl != NULL);
	
	if (PRC_GET_SEGMENT_LENGTH() < sizeof(struct cmdif_session_data)) {
		pr_err("Segment length is too small\n");
		return -EINVAL;
	}

	ind = find_dpci((uint8_t)data->dev_id);
	if (ind < 0) {
		pr_err("Not found DPCI peer %d\n", data->dev_id);
		return -ENAVAIL;
	}

	pr_debug("Found dpci %d peer id at index %d \n", \
	         dpci_tbl->attr[ind].id, ind);

#ifdef DEBUG
	 /* DEBUG in order not to call MC inside task */
	 dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	 ASSERT_COND_LIGHT(dprc != NULL);
	 err = dpci_get_link_state(&dprc->io, dpci_tbl->token[ind], &link_up);
	 if (err) {
		 pr_err("Failed to get dpci_get_link_state\n");
	 }
#endif
	 /* Do it only if queues are not there */
	 if ((dpci_tbl->tx_queue_attr[0][ind].fqid == DPCI_FQID_NOT_VALID) || 
		 (dpci_tbl->rx_queue_attr[0][ind].fqid == DPCI_FQID_NOT_VALID)) {
		 
		 dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
		 ASSERT_COND_LIGHT(dprc != NULL);
		 for (i = 0; i < DPCI_PRIO_NUM; i++) {
			 err |= dpci_get_tx_queue(&dprc->io, dpci_tbl->token[ind], i,
						   &dpci_tbl->tx_queue_attr[i][ind]);
			 err |= dpci_get_rx_queue(&dprc->io, dpci_tbl->token[ind], i,
						   &dpci_tbl->rx_queue_attr[i][ind]);
		 }
	 }

	if ((dpci_tbl->peer_attr[ind].peer_id == (-1)) || !link_up) {
		pr_err("DPCI is not attached or there is no link \n");
		return -EACCES; /*Invalid device state*/
	}

	/* Create descriptor for client session */
	ASSERT_COND_LIGHT(cl != NULL);
	lock_spinlock(&cl->lock);
	
#ifdef DEBUG
	/* Don't allow to open the same session twice */
	free_ind = cmdif_cl_session_get(cl, data->m_name, 
	                                data->inst_id, data->dev_id);
	if (free_ind >= 0) {
		pr_err("The session already exists\n");
		unlock_spinlock(&cl->lock);
		return -EEXIST;
	}
#endif

	free_ind = cmdif_cl_free_session_get(cl);
	if (free_ind < 0) {
		pr_err("Too many sessions\n");
		unlock_spinlock(&cl->lock);
		return -ENOSPC;
	}
	
	amq_bits_update(ind);
	cl->gpp[free_ind].ins_id           = data->inst_id;
	cl->gpp[free_ind].dev->auth_id     = data->auth_id;
	cl->gpp[free_ind].dev->p_sync_done = sync_done_get();
	cl->gpp[free_ind].dev->sync_done   = NULL; /* Not used in AIOP */
	strncpy(&cl->gpp[free_ind].m_name[0], &data->m_name[0], M_NAME_CHARS);
	cl->gpp[free_ind].m_name[M_NAME_CHARS] = '\0';
	cl->gpp[free_ind].regs->dpci_token = dpci_tbl->token[ind];
	cl->gpp[free_ind].regs->attr       = &dpci_tbl->attr[ind];
	cl->gpp[free_ind].regs->peer_attr  = &dpci_tbl->peer_attr[ind];
	cl->gpp[free_ind].regs->tx_queue_attr[0] = &dpci_tbl->tx_queue_attr[0][ind];
	cl->gpp[free_ind].regs->tx_queue_attr[1] = &dpci_tbl->tx_queue_attr[1][ind];
	cl->gpp[free_ind].regs->icid       = dpci_tbl->icid[ind];
	cl->gpp[free_ind].regs->dma_flags  = dpci_tbl->dma_flags[ind];
	cl->gpp[free_ind].regs->enq_flags  = dpci_tbl->bdi_flags[ind];

	cl->count++;
	unlock_spinlock(&cl->lock);

	pr_debug("icid = 0x%x\n", dpci_tbl->icid[ind]);
	pr_debug("enq_flags = 0x%x\n", dpci_tbl->bdi_flags[ind]);
	pr_debug("dma_flags = 0x%x\n", dpci_tbl->dma_flags[ind]);

	return 0;
}

/* Support for AIOP -> GPP */
static int notify_close()
{	
	struct cmdif_session_data *data = \
		(struct cmdif_session_data *)PRC_GET_SEGMENT_ADDRESS();
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int i = 0; 

	ASSERT_COND_LIGHT(cl != NULL);
	lock_spinlock(&cl->lock);
	
	i = cmdif_cl_auth_id_find(cl, data->auth_id, data->dev_id);
	
	/* Set this session entry as free */
	if (i >= 0) {
		cl->gpp[i].m_name[0] = CMDIF_FREE_SESSION;
		
		unlock_spinlock(&cl->lock);
		return 0;
	}
	
	unlock_spinlock(&cl->lock);	
	return -ENAVAIL;
}

void cmdif_srv_isr(void)
{
	uint16_t cmd_id = cmd_id_get();
	int err = 0;
	uint16_t auth_id = cmd_auth_id_get();

	pr_debug("cmd_id = 0x%x\n", cmd_id);
	pr_debug("auth_id = 0x%x\n", auth_id);
	
	if (cmdif_aiop_srv.srv == NULL)
		PR_ERR_TERMINATE("Could not find CMDIF Server handle\n");

#ifdef DEBUG
	{
		uint32_t len = MIN(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS),\
				   PRC_GET_SEGMENT_LENGTH());
		uint8_t  *p = (uint8_t  *)PRC_GET_SEGMENT_ADDRESS();

		pr_debug("----- Dump of SEGMENT_ADDRESS 0x%x size %d -----\n",
			 p, len);
		pr_debug("Virtual addr high = 0x%x low = 0x%x \n",
			 (uint32_t)((LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS) & 0xFF00000000) >> 32),
			 (uint32_t)(LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS) & 0xFFFFFFFF));

		while (len > 15)
		{
			fsl_os_print("0x%x: %x %x %x %x\r\n",
				     p,
				     *(uint32_t *)p,
				     *(uint32_t *)(p + 4),
				     *(uint32_t *)(p + 8),
				     *(uint32_t *)(p + 12));
			len -= 16;
			p += 16;
		}
		while (len > 3)
		{
			fsl_os_print("0x%x: %x\r\n", p, *(uint32_t *)p);
			len -= 4;
			p += 4;
		}

	}
#endif
	
	if (cmd_id == CMD_ID_NOTIFY_OPEN) {
		/* Support for AIOP -> GPP */
		if (is_valid_auth_id(auth_id)) {
			err = notify_open();
			sync_cmd_done(NULL, err, auth_id, TRUE);
		} else {
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}

	} else if (cmd_id == CMD_ID_NOTIFY_CLOSE) {
		if (is_valid_auth_id(auth_id)) {
			err = notify_close();
			sync_cmd_done(NULL, err, auth_id, TRUE);
		} else {
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}

	} else if (cmd_id == CMD_ID_OPEN) {			
		char     m_name[M_NAME_CHARS + 1];
		int      m_id      = 0;
		uint8_t  inst_id   = 0;
		int      new_inst  = 0;
		uint64_t sync_done = sync_done_get();
		void     *dev;

		/* OPEN will arrive with hash value 0xffff */
		if (auth_id != OPEN_AUTH_ID) {
			pr_err("No permission to open device 0x%x\n", auth_id);
			sync_cmd_done(sync_done, -EPERM, auth_id, TRUE);
		}

		cmd_m_name_get(&m_name[0]);
		pr_debug("m_name = %s\n", m_name);

		m_id = module_id_find(m_name);
		pr_debug("m_id = %d\n", m_id);

		if (m_id < 0) {
			/* Did not find module with such name */
			pr_err("No such module %s\n", m_name);
			sync_cmd_done(sync_done, -ENODEV, auth_id, TRUE);
		}

		inst_id  = cmd_inst_id_get();
		pr_debug("inst_id = %d\n", inst_id);

		err = OPEN_CB(m_id, inst_id, dev);
		if (!err) {
			new_inst = inst_alloc((uint8_t)m_id);
			if (new_inst >= 0) {
				pr_debug("new auth_id = %d\n", new_inst);
				sync_done_set((uint16_t)new_inst);
				cmdif_aiop_srv.srv->inst_dev[new_inst] = dev;
				sync_cmd_done(sync_done, 0,
						(uint16_t)new_inst, TRUE);
			} else {
				/* couldn't find free place for new device */
				sync_cmd_done(sync_done, -ENODEV, auth_id, FALSE);
				PR_ERR_TERMINATE("No free entry for new device\n");
			}
		} else {
			sync_cmd_done(sync_done, err, auth_id, FALSE);
			PR_ERR_TERMINATE("Open callback failed\n");
		}
	} else if (cmd_id == CMD_ID_CLOSE) {

		if (is_valid_auth_id(auth_id)) {
			/* Don't reorder this sequence !!*/
			err = CLOSE_CB(auth_id);
			sync_cmd_done(NULL, err, auth_id, FALSE);
			if (!err) {
				/* Free instance entry only if we had no error
				 * otherwise it will be impossible to retry to
				 * close the device */
				inst_dealloc(auth_id);
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
		if (is_valid_auth_id(auth_id)) {
			/* User can ignore data and use presentation context */
			err = CTRL_CB(auth_id, cmd_id, cmd_size_get(), \
			              cmd_data_get());
			if (SYNC_CMD(cmd_id)) {
				pr_debug("PASSED Synchronous Command\n");
				sync_cmd_done(NULL, err, auth_id, TRUE);
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
		err = cmdif_fd_send(err);
	} else {
		/* CMDIF_NORESP_CMD store user modified data but don't send */
		pr_debug("PASSED No Response Command\n");
		fdma_store_default_frame_data();
	}

	fdma_terminate_task();
}
