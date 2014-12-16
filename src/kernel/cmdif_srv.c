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
#include "fsl_sl_cmd.h"
#include "fsl_icontext.h"

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


#define SAVE_GPP_ICID	\
do {\
	gpp_icid = PL_ICID_GET; 		\
	gpp_dma = 0; 				\
	ADD_AMQ_FLAGS(gpp_dma, gpp_icid); 	\
	gpp_icid = ICID_GET(gpp_icid); 		\
\
} while(0)

#define SET_AIOP_ICID	\
	do { \
		/* Set AIOP ICID and AMQ bits */			\
		uint16_t pl_icid = icontext_aiop.icid;			\
		uint8_t flags = 0;					\
		struct additional_dequeue_context *adc = 		\
		((struct additional_dequeue_context *)HWC_ADC_ADDRESS);	\
		/* SHRAM optimization */				\
		uint64_t dma_bdi_flags = 				\
				(*(uint64_t *)(&icontext_aiop.dma_flags));\
		if (((uint32_t)dma_bdi_flags) & FDMA_ENF_BDI_BIT) {	\
			flags |= ADC_BDI_MASK;				\
		}							\
		dma_bdi_flags >>= 32;					\
		if (((uint32_t)dma_bdi_flags) & FDMA_DMA_eVA_BIT) {	\
			flags |= ADC_VA_MASK;				\
		}							\
		if (((uint32_t)dma_bdi_flags) & FDMA_DMA_PL_BIT) {	\
			pl_icid |= ADC_PL_MASK;				\
		}							\
		adc->fdsrc_va_fca_bdi = (adc->fdsrc_va_fca_bdi &	\
			~(ADC_BDI_MASK | ADC_VA_MASK)) | flags;		\
		STH_SWAP(pl_icid, 0, &(adc->pl_icid));			\
	} while (0)

#define OPEN_CB(M_ID, INST, DEV) \
	do {\
		SET_AIOP_ICID;					\
		err = cmdif_aiop_srv.srv->open_cb[M_ID](INST, &DEV);	\
	} while (0)

#define CTRL_CB(AUTH_ID, CMD_ID, SIZE, DATA) \
	do {\
		SET_AIOP_ICID;\
		err = cmdif_aiop_srv.srv->ctrl_cb[cmdif_aiop_srv.srv->m_id[AUTH_ID]] \
		(cmdif_aiop_srv.srv->inst_dev[AUTH_ID], CMD_ID, SIZE, DATA);   \
	} while(0)

#define CLOSE_CB(AUTH_ID) \
	do {\
		SET_AIOP_ICID;\
		err = cmdif_aiop_srv.srv->close_cb[cmdif_aiop_srv.srv->m_id[AUTH_ID]]\
		(cmdif_aiop_srv.srv->inst_dev[AUTH_ID]); \
	} while(0);


#define FREE_MODULE    '\0'
#define FREE_INSTANCE  (M_NUM_OF_MODULES)

#define SYNC_CMD_RESP_MAKE(ERR, ID)  (0x80000000 | \
	(((ERR) << 16) & 0x00FF0000) | (ID))

#define PR_ERR_TERMINATE(...) \
	do {                  \
		sl_pr_debug(__VA_ARGS__);  \
		fdma_terminate_task();\
		return;               \
	} while (0)


static struct cmdif_srv_aiop cmdif_aiop_srv = {0};

extern int sl_cmd_ctrl_cb(void *dev, uint16_t cmd, uint32_t size, void *data);
extern int sl_cmd_open_cb(uint8_t instance_id, void **dev);
extern int sl_cmd_close_cb_t(void *dev);

extern struct icontext icontext_aiop;

static inline int is_valid_auth_id(uint16_t id)
{
	return ((cmdif_aiop_srv.srv->inst_dev != NULL) &&
		(id < M_NUM_OF_INSTANCES) &&
		(cmdif_aiop_srv.srv->m_id != NULL) &&
		(cmdif_aiop_srv.srv->m_id[id] < M_NUM_OF_MODULES));
}

static inline int module_id_find(const char *m_name)
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

static inline int inst_alloc(uint8_t m_id)
{
	uint32_t r = 0;
	int count = 0;

#ifdef DEBUG
	if (cmdif_aiop_srv.srv == NULL)
		return -EINVAL;
#endif

	ASSERT_COND_LIGHT(is_power_of_2(M_NUM_OF_INSTANCES));

	lock_spinlock(&cmdif_aiop_srv.lock);

	/* randomly pick instance/authentication id*/
	r = MODULU_POWER_OF_TWO(fsl_os_rand(), M_NUM_OF_INSTANCES);
	while ((cmdif_aiop_srv.srv->m_id[r] != FREE_INSTANCE) &&
		(count < M_NUM_OF_INSTANCES)) {
		r = MODULU_POWER_OF_TWO(fsl_os_rand(), M_NUM_OF_INSTANCES);
		count++;
	}
	/* didn't find empty space yet */
	if (cmdif_aiop_srv.srv->m_id[r] != FREE_INSTANCE) {
		count = 0;
		while ((cmdif_aiop_srv.srv->m_id[r] != FREE_INSTANCE) &&
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
		cmdif_aiop_srv.srv->m_id[r] = m_id;
		cmdif_aiop_srv.srv->inst_count++;
		unlock_spinlock(&cmdif_aiop_srv.lock);
		return (int)r;
	}
}

static inline void inst_dealloc(int inst)
{
	lock_spinlock(&cmdif_aiop_srv.lock);
	cmdif_aiop_srv.srv->m_id[inst] = FREE_INSTANCE;
	cmdif_aiop_srv.srv->inst_count--;
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

static inline void cmd_m_name_get(char *name)
{
	uint8_t * addr = (uint8_t *)PRC_GET_SEGMENT_ADDRESS();
	addr += PRC_GET_SEGMENT_OFFSET() + SYNC_BUFF_RESERVED;

	sl_pr_debug("Read module name from 0x%x \n", addr);

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

__COLD_CODE int cmdif_register_module(const char *m_name, struct cmdif_module_ops *ops)
{
	/* Place here lock if required */

	return cmdif_srv_register(cmdif_aiop_srv.srv, m_name, ops);
}

__COLD_CODE int cmdif_unregister_module(const char *m_name)
{
	/* Place here lock if required */

	return cmdif_srv_unregister(cmdif_aiop_srv.srv, m_name);
}

static void *fast_malloc(int size)
{
	/*return fsl_os_xmalloc((size_t)size, MEM_PART_SH_RAM, 8);*/
	return fsl_malloc((size_t)size,8);
}

static void *slow_malloc(int size)
{
	/*return fsl_os_xmalloc((size_t)size, MEM_PART_SH_RAM, 8);*/
	return fsl_malloc((size_t)size,8);
}

static void srv_free(void *ptr)
{
	if (ptr != NULL)
		fsl_free(ptr);
}

__COLD_CODE int cmdif_srv_init(void)
{
	int  err = 0;
	void *srv = NULL;
	struct cmdif_module_ops ops = {sl_cmd_open_cb,
				       sl_cmd_close_cb_t,
				       sl_cmd_ctrl_cb};

	srv = cmdif_srv_allocate(fast_malloc, slow_malloc);

	if (srv == NULL) {
		pr_err("Not enough memory for server allocation \n");
		return -ENOMEM;
	}
	cmdif_aiop_srv.srv = srv;
	cmdif_aiop_srv.dpci_tbl = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	if (cmdif_aiop_srv.dpci_tbl == NULL)
	{
		pr_err("No DPCI table on AIOP, CMDIF is not functional \n");
		pr_info("All AIOP DPCIs should be defined in DPL\n");
		pr_info("All AIOP DPCIs should have peer before AIOP boot\n");
		return -ENODEV;
	}

	/* Register ARENA SL module */
	err = cmdif_register_module(SL_CMD_MODULE, &ops);
	if (err) {
		pr_err("Failed registration of %s module\n", SL_CMD_MODULE);
		return err;
	}

	return err;
}

__COLD_CODE void cmdif_srv_free(void)
{
	cmdif_srv_deallocate(cmdif_aiop_srv.srv, srv_free);
	srv_free(cmdif_aiop_srv.dpci_tbl);
}


void cmdif_fd_send(int cb_err);
/*static*/ void cmdif_fd_send(int cb_err)
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

	sl_pr_debug("Response FQID = 0x%x pr = 0x%x dpci_ind = 0x%x\n", fqid, pr, ind);
	sl_pr_debug("CB error = %d\n", cb_err);

	err = (int)fdma_store_and_enqueue_default_frame_fqid(
		fqid, CMDIF_FDMA_ENQ_TC);
	if (err) {
		sl_pr_err("Failed to send response\n");
	}
}

void sync_cmd_done(uint64_t sync_done,
			  int err,
			  uint16_t auth_id,
			  char terminate,
			  uint16_t icid,
			  uint32_t dma_flags);
/* static */ void sync_cmd_done(uint64_t sync_done,
			  int err,
			  uint16_t auth_id,
			  char terminate,
			  uint16_t icid,
			  uint32_t dma_flags)
{
	uint32_t resp = SYNC_CMD_RESP_MAKE(err, auth_id);
	uint64_t _sync_done = NULL;

	sl_pr_debug("err = %d\n", err);
	sl_pr_debug("auth_id = 0x%x\n", auth_id);
	sl_pr_debug("sync_resp = 0x%x\n", resp);

	/* Delete FDMA handle and store user modified data */
	fdma_store_default_frame_data();
	if ((sync_done != NULL) || (auth_id == OPEN_AUTH_ID))
		_sync_done = sync_done;
	else
		_sync_done = cmdif_aiop_srv.srv->sync_done[auth_id];

	if (_sync_done == NULL) {
		sl_pr_err("Can't finish sync command, no valid address\n");
		/** In this case client will fail on timeout */
	} else {
		/*
		 * Use previously saved AMQ bits and ICID */
		sl_pr_debug("icid = 0x%x\n", icid);
		sl_pr_debug("fdma_dma_data flags = 0x%x\n", dma_flags);
		fdma_dma_data(4, icid, &resp, _sync_done,
		              dma_flags | FDMA_DMA_DA_WS_TO_SYS_BIT);
	}

	sl_pr_debug("sync_done high = 0x%x low = 0x%x \n",
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
__COLD_CODE static inline int find_dpci(uint32_t dpci_id)
{
	int i = 0;
	struct mc_dpci_obj *dt = cmdif_aiop_srv.dpci_tbl;

	for (i = 0; i < dt->count; i++) {
		if (dt->peer_attr[i].peer_id == dpci_id)
			return i;
	}
	return -1;
}

__COLD_CODE static inline void amq_bits_update(int ind)
{
	uint16_t pl_icid = PL_ICID_GET;

	if (cmdif_aiop_srv.dpci_tbl->icid[ind] == ICONTEXT_INVALID) {
		cmdif_aiop_srv.dpci_tbl->bdi_flags[ind]  = FDMA_EN_TC_RET_BITS; /* don't change */
		cmdif_aiop_srv.dpci_tbl->dma_flags[ind]  = FDMA_DMA_DA_SYS_TO_WS_BIT;
		ADD_AMQ_FLAGS(cmdif_aiop_srv.dpci_tbl->dma_flags[ind], pl_icid);
		if (BDI_GET != 0)
			cmdif_aiop_srv.dpci_tbl->bdi_flags[ind] |= FDMA_ENF_BDI_BIT;
		/* Must be last line */
		cmdif_aiop_srv.dpci_tbl->icid[ind]       = ICID_GET(pl_icid);
	}
}

/* Support for AIOP -> GPP */
/* int mc_dpci_check(int ind);*/
__COLD_CODE static inline int mc_dpci_check(int ind)
{
	uint8_t i;
	struct mc_dprc *dprc = NULL;
	int link_up = 1;
	int err = 0;
	
	dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	ASSERT_COND_LIGHT(dprc != NULL);

	err = dpci_get_link_state(&dprc->io, cmdif_aiop_srv.dpci_tbl->token[ind], &link_up);
	if (err) {
		pr_err("Failed to get dpci_get_link_state\n");
	}

	if ((cmdif_aiop_srv.dpci_tbl->peer_attr[ind].peer_id == (-1)) || !link_up) {
		pr_err("DPCI is not attached or there is no link \n");
		return -EACCES; /*Invalid device state*/
	}

	for (i = 0; i < DPCI_PRIO_NUM; i++) {
		err |= dpci_get_tx_queue(&dprc->io, cmdif_aiop_srv.dpci_tbl->token[ind], i,
		                         &cmdif_aiop_srv.dpci_tbl->tx_queue_attr[i][ind]);
		err |= dpci_get_rx_queue(&dprc->io, cmdif_aiop_srv.dpci_tbl->token[ind], i,
		                         &cmdif_aiop_srv.dpci_tbl->rx_queue_attr[i][ind]);
	}
	return err;
}

__COLD_CODE int notify_open();
/* static */ int notify_open()
{
#ifndef STACK_CHECK /* No user callback */

	struct cmdif_session_data *data = \
		(struct cmdif_session_data *)PRC_GET_SEGMENT_ADDRESS();
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int ind = 0;
	int link_up = 1;
	int err = 0;

	/* Create descriptor for client session */
	ASSERT_COND_LIGHT((cl != NULL) && (cmdif_aiop_srv.dpci_tbl != NULL));

	if (PRC_GET_SEGMENT_LENGTH() < sizeof(struct cmdif_session_data)) {
		pr_err("Segment length is too small\n");
		return -EINVAL;
	}

	ind = find_dpci(data->dev_id); /* dev_id is swapped by GPP */
	if (ind < 0) {
		pr_err("Not found DPCI peer %d\n", data->dev_id);
		return -ENAVAIL;
	}

	pr_debug("Found dpci %d peer id at index %d \n", \
		    cmdif_aiop_srv.dpci_tbl->attr[ind].id, ind);

	amq_bits_update(ind);

	/* Locking here for updating dpci table in runtime in case if queues 
	 * are not available yet. This case should not happen. 
	 * After cmdif_open() on AIOP those DPCI queues are not updated anymore 
	 * no lock in runtime. 
	 * TODO consider to add lock per DPCI entry */
	lock_spinlock(&cl->lock);

	/* Do it only if queues are not there, it should not happen */
	if ((cmdif_aiop_srv.dpci_tbl->tx_queue_attr[0][ind].fqid == DPCI_FQID_NOT_VALID) ||
		(cmdif_aiop_srv.dpci_tbl->rx_queue_attr[0][ind].fqid == DPCI_FQID_NOT_VALID)) {
		pr_err("DPCI queues are not known to AIOP, will try again\n");
		err = mc_dpci_check(ind);
		if (err) {
			unlock_spinlock(&cl->lock);
			return err;
		}
		if (cmdif_aiop_srv.dpci_tbl->rx_queue_attr[0][ind].fqid != DPCI_FQID_NOT_VALID) {
			pr_debug("DPCI queues are now set\n");	
		}		
	}

#ifdef DEBUG
	/* Don't allow to open the same session twice */
	link_up = cmdif_cl_session_get(cl, data->m_name,
					data->inst_id, data->dev_id);
	if (link_up >= 0) {
		pr_err("The session already exists\n");
		unlock_spinlock(&cl->lock);
		return -EEXIST;
	}
#endif

	/*  REUSING link_up as free_ind */
	link_up = cmdif_cl_free_session_get(cl);
	if (link_up < 0) {
		pr_err("Too many sessions\n");
		unlock_spinlock(&cl->lock);
		return -ENOSPC;
	}

	cl->gpp[link_up].ins_id           = data->inst_id;
	cl->gpp[link_up].dev->auth_id     = data->auth_id;
	cl->gpp[link_up].dev->p_sync_done = sync_done_get();
	cl->gpp[link_up].dev->sync_done   = NULL; /* Not used in AIOP */
	strncpy(&cl->gpp[link_up].m_name[0], &data->m_name[0], M_NAME_CHARS);
	cl->gpp[link_up].m_name[M_NAME_CHARS] = '\0';
	cl->gpp[link_up].regs->dpci_token = cmdif_aiop_srv.dpci_tbl->token[ind];
	cl->gpp[link_up].regs->attr       = &cmdif_aiop_srv.dpci_tbl->attr[ind];
	cl->gpp[link_up].regs->peer_attr  = &cmdif_aiop_srv.dpci_tbl->peer_attr[ind];
	cl->gpp[link_up].regs->tx_queue_attr[0] = &cmdif_aiop_srv.dpci_tbl->tx_queue_attr[0][ind];
	cl->gpp[link_up].regs->tx_queue_attr[1] = &cmdif_aiop_srv.dpci_tbl->tx_queue_attr[1][ind];
	cl->gpp[link_up].regs->icid       = cmdif_aiop_srv.dpci_tbl->icid[ind];
	cl->gpp[link_up].regs->dma_flags  = cmdif_aiop_srv.dpci_tbl->dma_flags[ind];
	cl->gpp[link_up].regs->enq_flags  = cmdif_aiop_srv.dpci_tbl->bdi_flags[ind];

	cl->count++;
	unlock_spinlock(&cl->lock);

	pr_debug("icid = 0x%x\n", cmdif_aiop_srv.dpci_tbl->icid[ind]);
	pr_debug("enq_flags = 0x%x\n", cmdif_aiop_srv.dpci_tbl->bdi_flags[ind]);
	pr_debug("dma_flags = 0x%x\n", cmdif_aiop_srv.dpci_tbl->dma_flags[ind]);

#endif /* STACK_CHECK */
	return 0;
}

/* Support for AIOP -> GPP */
__COLD_CODE int notify_close();
/* static */ int notify_close()
{
#ifndef STACK_CHECK /* No user callabck here */	
	
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
#endif /* STACK_CHECK */
	return -ENAVAIL;
}

void dump_param_get(uint32_t *len, uint8_t **p, uint64_t *addr);
void dump_param_get(uint32_t *len, uint8_t **p, uint64_t *addr)
{
	*len = MIN(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS),\
	                   PRC_GET_SEGMENT_LENGTH());
	*p = (uint8_t  *)PRC_GET_SEGMENT_ADDRESS();
	*addr = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
}

void dump_memory();
/* static inline */ void dump_memory()
{
#ifndef STACK_CHECK /* dump memory happens before user callabck */
	uint32_t len;
	uint8_t  *p;
	uint64_t addr;
	
	dump_param_get(&len, &p, &addr);
	
	pr_debug("----- Dump of SEGMENT_ADDRESS 0x%x size %d -----\n",
	         p, len);
	pr_debug("Virtual addr high = 0x%x low = 0x%x \n",
	         (uint32_t)(( addr & 0xFF00000000) >> 32),
	         (uint32_t)(addr & 0xFFFFFFFF));

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
#endif /* STACK_CHECK */
}

__COLD_CODE static void open_cmd_print()
{
#ifdef DEBUG
	char  m_name[M_NAME_CHARS + 1];
	cmd_m_name_get(&m_name[0]);
	pr_debug("Module name is %s\n", m_name);
#endif
}

__COLD_CODE static void dpci_icontext_update()
{
	uint32_t fqid = RESP_QID_GET;
	uint8_t  ind = (uint8_t)(fqid >> 1);	

	amq_bits_update(ind);
}

__COLD_CODE int session_open();
/* static */ int session_open()
{
	char     m_name[M_NAME_CHARS + 1];
	int      m_id;
	uint8_t  inst_id;
	void     *dev;
	int      err;
	
	cmd_m_name_get(&m_name[0]);
	sl_pr_debug("m_name = %s\n", m_name);

	m_id = module_id_find(m_name);
	sl_pr_debug("m_id = %d\n", m_id);

	if (m_id < 0) {
		/* Did not find module with such name */
		sl_pr_err("No such module %s\n", m_name);
		return -ENODEV; 
	}

	inst_id  = cmd_inst_id_get();
	sl_pr_debug("inst_id = %d\n", inst_id);
	
	dpci_icontext_update(); /* Must be before open callback */
	
	OPEN_CB(m_id, inst_id, dev);
	if (!err) {
		int  new_inst = inst_alloc((uint8_t)m_id);
		if (new_inst >= 0) {
			sl_pr_debug("New auth_id = %d module name = %s\n", new_inst, m_name);
			sync_done_set((uint16_t)new_inst);
			cmdif_aiop_srv.srv->inst_dev[new_inst] = dev;			
			return new_inst;
		} else {
			/* couldn't find free place for new device */
			sl_pr_err("No free entry for new device\n");
			return -ENOMEM; 			
		}
	} else {
		return err; /* User error */
	}
}

void cmdif_srv_isr(void) /*__attribute__ ((noreturn))*/
{
	uint16_t gpp_icid;
	uint32_t gpp_dma;
	int err;
	uint16_t cmd_id;
	uint16_t auth_id;

	ASSERT_COND_LIGHT(cmdif_aiop_srv.srv != NULL);

#ifdef DEBUG
	dump_memory();
#endif

	SAVE_GPP_ICID;
	
	cmd_id = cmd_id_get();
	auth_id = cmd_auth_id_get();

	pr_debug("cmd_id = 0x%x\n", cmd_id);
	pr_debug("auth_id = 0x%x\n", auth_id);
	pr_debug("gpp_icid = 0x%x\n", gpp_icid);
	pr_debug("gpp_dma flags = 0x%x\n", gpp_dma);
	
	if (cmd_id == CMD_ID_NOTIFY_OPEN) {
		/* Support for AIOP -> GPP */
		if (is_valid_auth_id(auth_id)) {
			pr_debug("Got notify open for AIOP client \n");
			err = notify_open();
			if (err) {
				pr_err("notify_open failed\n");
			}
			sync_cmd_done(NULL, err, auth_id, TRUE, gpp_icid, gpp_dma);
		} else {
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}
	} else if (cmd_id == CMD_ID_NOTIFY_CLOSE) {
		if (is_valid_auth_id(auth_id)) {
			pr_debug("Got notify close for AIOP client \n");
			err = notify_close();
			if (err) {
				pr_err("notify_close failed\n");
			}
			sync_cmd_done(NULL, err, auth_id, TRUE, gpp_icid, gpp_dma);
		} else {
			fdma_store_default_frame_data(); /* Close FDMA */
			PR_ERR_TERMINATE("Invalid authentication id\n");
		}
	} else if (cmd_id == CMD_ID_OPEN) {

		/* OPEN will arrive with hash value 0xffff */
		if (auth_id != OPEN_AUTH_ID) {
			pr_err("No permission to open device 0x%x\n", auth_id);
			sync_cmd_done(sync_done_get(), -EPERM, auth_id,
				      TRUE, gpp_icid, gpp_dma);
		}

		open_cmd_print();
		
		err = session_open();
		if (err < 0) {
			pr_err("Open session FAILED err = %d\n", err);
			sync_cmd_done(sync_done_get(), err, auth_id,
				      TRUE, gpp_icid, gpp_dma);
		} else {
			pr_debug("Open session PASSED auth_id = 0x%x\n", 
			       (uint16_t)err);
			sync_cmd_done(sync_done_get(), 0, (uint16_t)err,
				      TRUE, gpp_icid, gpp_dma);
		}
		
	} else if (cmd_id == CMD_ID_CLOSE) {

		if (is_valid_auth_id(auth_id)) {
			/* Don't reorder this sequence !!*/
			CLOSE_CB(auth_id);
			sync_cmd_done(NULL, err, auth_id, FALSE, 
			              gpp_icid, gpp_dma);
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
			CTRL_CB(auth_id, cmd_id, cmd_size_get(), \
				      cmd_data_get());
			if (SYNC_CMD(cmd_id)) {
				pr_debug("PASSED Synchronous Command\n");
				sync_cmd_done(NULL, err, auth_id,
					      TRUE, gpp_icid, gpp_dma);
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
		cmdif_fd_send(err);
	} else {
		/* CMDIF_NORESP_CMD store user modified data but don't send */
		pr_debug("PASSED No Response Command\n");
		fdma_store_default_frame_data();
	}

	fdma_terminate_task();
}
