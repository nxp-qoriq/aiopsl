/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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
#include "sys.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "fsl_sys.h"
#include "fsl_dprc.h"
#include "fsl_dpci.h"
#include "fsl_dpci_event.h"
#include "fsl_dpci_drv.h"
#include "fsl_dpci_mng.h"
#include "fsl_mc_init.h"
#include "fsl_dbg.h"
#include "cmdif_client.h"
#include "cmdif_srv.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_icontext.h"
#include "fsl_spinlock.h"
#include "fsl_malloc.h"

/*************************************************************************/
#define DPCI_LOW_PR		1

#define CMDIF_Q_OPTIONS (DPCI_QUEUE_OPT_USER_CTX | DPCI_QUEUE_OPT_DEST)

#define CMDIF_FQD_CTX_GET \
	(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fqd_ctx)

#define CMDIF_RX_CTX_GET \
	(LLLDW_SWAP((uint32_t)&CMDIF_FQD_CTX_GET, 0))

#define AMQ_BDI_SET(_offset, _width, _type, _arg) \
	(amq_bdi |= u32_enc((_offset), (_width), (_arg)))

#define AMQ_BDI_GET(_offset, _width, _type, _arg) \
	(*(_arg) = (_type)u32_dec(amq_bdi, (_offset), (_width)))

#define USER_CTX_SET(_offset, _width, _type, _arg) \
	(queue_cfg.user_ctx |= u64_enc((_offset), (_width), (_arg)))

#define USER_CTX_GET(_offset, _width, _type, _arg) \
	(*(_arg) = (_type)u64_dec(rx_ctx, (_offset), (_width)))

#define CMDIF_DPCI_FQID(_OP, DPCI, FQID) \
do { \
	_OP(32,		32,	uint32_t,	DPCI); \
	_OP(0,		32,	uint32_t,	FQID); \
} while (0)


#define CMDIF_ICID_AMQ_BDI(_OP, ICID, AMQ_BDI) \
do { \
	_OP(16,		16,	uint16_t,	ICID); \
	_OP(0,		16,	uint16_t,	AMQ_BDI); \
} while (0)

#define MEM_SET(_ADDR, _SIZE, _VAL) \
	do { \
		for (i = 0; i < (_SIZE); i++) \
			((uint8_t *)_ADDR)[i] = _VAL; \
	} while (0)

#define DPCI_DT_LOCK_R_TAKE \
	do { \
		cdma_mutex_lock_take((uint64_t)(&g_dpci_tbl), CDMA_MUTEX_READ_LOCK); \
	} while(0)

#define DPCI_DT_LOCK_W_TAKE \
	do { \
		cdma_mutex_lock_take((uint64_t)(&g_dpci_tbl), CDMA_MUTEX_WRITE_LOCK); \
	} while(0)

#define DPCI_DT_LOCK_RELEASE \
	do { \
		cdma_mutex_lock_release((uint64_t)(&g_dpci_tbl)); \
	} while(0)

#define DPCI_ENTRY_LOCK_W_TAKE(IND) \
	do { \
		cdma_mutex_lock_take((uint64_t)(&g_dpci_tbl.dpci_id[IND]), CDMA_MUTEX_WRITE_LOCK); \
	} while(0)

#define DPCI_ENTRY_LOCK_RELEASE(IND) \
	do { \
		cdma_mutex_lock_release((uint64_t)(&g_dpci_tbl.dpci_id[IND])); \
	} while(0)

int dpci_drv_init();
void dpci_drv_free();


extern struct aiop_init_info g_init_data;

struct dpci_mng_tbl g_dpci_tbl = {0};

__COLD_CODE static void dpci_tbl_dump()
{
	int i;

	fsl_os_print("----------DPCI table----------\n");
	for (i = 0; i < g_dpci_tbl.count; i++) {
		fsl_os_print("ID = 0x%x\t PEER ID = 0x%x\t IC = 0x%x\t\n",
		             g_dpci_tbl.dpci_id[i], g_dpci_tbl.dpci_id_peer[i], g_dpci_tbl.ic[i]);
	}
}

int dpci_mng_find(uint32_t dpci_id, uint32_t *ic)
{
	int i = 0;
	int count = 0;

	ASSERT_COND(dpci_id != DPCI_FQID_NOT_VALID);
	ASSERT_COND(g_dpci_tbl.count <= g_dpci_tbl.max);

	while (count < g_dpci_tbl.count) {

		if (g_dpci_tbl.dpci_id[i] != DPCI_FQID_NOT_VALID) {
			if (g_dpci_tbl.dpci_id[i] == dpci_id) {
				if (ic != NULL)
					*ic = g_dpci_tbl.ic[i];
				return i;
			}
			count++;
		}
		i++;
	}

	return -ENOENT;
}

int dpci_mng_peer_find(uint32_t dpci_id, uint32_t *ic)
{
	int i = 0;
	int count = 0;

	ASSERT_COND(dpci_id != DPCI_FQID_NOT_VALID);
	ASSERT_COND(g_dpci_tbl.count <= g_dpci_tbl.max);

	while (count < g_dpci_tbl.count) {

		if (g_dpci_tbl.dpci_id[i] != DPCI_FQID_NOT_VALID) {
			if (g_dpci_tbl.dpci_id_peer[i] == dpci_id) {
				if (ic != NULL)
					*ic = g_dpci_tbl.ic[i];
				return i;
			}
			count++;
		}
		i++;
	}

	return -ENOENT;
}

static int dpci_entry_get()
{
	int i;

	for (i = 0; i < g_dpci_tbl.max; i++)
		if (g_dpci_tbl.dpci_id[i] == DPCI_FQID_NOT_VALID) {
			atomic_incr32(&g_dpci_tbl.count, 1);
			return i;
		}
		
	return -ENOENT;
}

static void dpci_entry_delete(int ind)
{

	int i;
	
	g_dpci_tbl.ic[ind] = DPCI_FQID_NOT_VALID;
	g_dpci_tbl.dpci_id[ind] = DPCI_FQID_NOT_VALID;
	g_dpci_tbl.dpci_id_peer[ind] = DPCI_FQID_NOT_VALID;
	for (i = 0 ; i < DPCI_PRIO_NUM; i++)
		g_dpci_tbl.tx_queue[ind][i] = DPCI_FQID_NOT_VALID;
	g_dpci_tbl.token[ind] = 0xffff;
	atomic_decr32(&g_dpci_tbl.count, 1);
}

__COLD_CODE static int dpci_get_peer_id(uint32_t dpci_id, uint32_t *dpci_id_peer)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token;
	struct dpci_peer_attr peer_attr;
	uint8_t i;

	ASSERT_COND(dprc);

	/* memset */
	MEM_SET(&peer_attr, sizeof(peer_attr), 0);

	err = dpci_open(&dprc->io, (int)dpci_id, &token);
	if (err) {
		pr_err("Failed dpci_open dpci id = %d\n", dpci_id);
		return err;
	}
	err = dpci_get_peer_attributes(&dprc->io, token, &peer_attr);
	if (err) {
		pr_err("Failed to get peer_id dpci id = %d\n", dpci_id);
		dpci_close(&dprc->io, token);
		return err;
	}

	if (peer_attr.peer_id == -1) {
		err = dpci_close(&dprc->io, token);
		return -EINVAL;
	}

	*dpci_id_peer = (uint32_t)peer_attr.peer_id;

	err = dpci_close(&dprc->io, token);
	return err;
}

static inline void amq_bits_update(uint32_t id)
{
	uint32_t amq_bdi = 0;
	uint16_t amq_bdi_temp = 0;
	uint16_t pl_icid = PL_ICID_GET;
	
	/* Don't update if it's already set */
	if (g_dpci_tbl.ic[id] != DPCI_FQID_NOT_VALID)
		return;
	
	ADD_AMQ_FLAGS(amq_bdi_temp, pl_icid);
	if (BDI_GET != 0)
		amq_bdi_temp |= CMDIF_BDI_BIT;

	CMDIF_ICID_AMQ_BDI(AMQ_BDI_SET, ICID_GET(pl_icid), amq_bdi_temp);

	/*
	 * TODO
	 * NOTE : only dpci_peer_id can be updated but not dpci_id.
	 * Maybe it should not update peer id at all ?? 
	 * It should be updated only in dpci_drv_added() !!!
	 * TODO
	 * Check if amq bits updated and update only if they are 0xffffffff
	 */
	//err = dpci_get_peer_id(g_dpci_tbl.dpci_id[id], &(g_dpci_tbl.dpci_id_peer[id]));
	//ASSERT_COND(!err);

	/* Must be written last */
	g_dpci_tbl.ic[id] = amq_bdi;
	/* Don't update AIOP to AIOP DPCI 2 entries because it 
	 * shouldn't change anyway */
	dpci_tbl_dump();
}

__COLD_CODE static int tx_set(uint32_t ind)
{

	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token;
	struct dpci_attr attr;
	struct dpci_tx_queue_attr tx_attr;
	int i;
	uint32_t *tx = &g_dpci_tbl.tx_queue[ind][0];
	
	ASSERT_COND(tx);
	ASSERT_COND(dprc);

	if (tx[0] != DPCI_FQID_NOT_VALID) {
		/* No need to update if it is already set */
		return 0;
	}

	/* memset */
	MEM_SET(&tx_attr, sizeof(tx_attr), 0);
	MEM_SET(&attr, sizeof(attr), 0);

	err = dpci_open(&dprc->io, (int)g_dpci_tbl.dpci_id[ind], &token);
	if (err)
		return err;

	err = dpci_get_attributes(&dprc->io, token, &attr);
	if (err) {
		dpci_close(&dprc->io, token);
		return err;
	}

	for (i = 0; i < attr.num_of_priorities; i++) {
		err = dpci_get_tx_queue(&dprc->io, token, (uint8_t)i, &tx_attr);
		ASSERT_COND(!err);
		ASSERT_COND(tx_attr.fqid != DPCI_FQID_NOT_VALID);
		tx[i] = tx_attr.fqid;
	}

	err = dpci_close(&dprc->io, token);

	return err;
}

/* To be called upon connected event, assign even */
__COLD_CODE static int dpci_entry_init(uint32_t dpci_id)
{
	int ind = -1;
	uint32_t amq_bdi = 0;
	uint32_t dpci_id_peer = DPCI_FQID_NOT_VALID;
	int err = 0;

	CMDIF_ICID_AMQ_BDI(AMQ_BDI_SET, ICONTEXT_INVALID, ICONTEXT_INVALID);

	ind = dpci_mng_find(dpci_id, NULL);
	if (ind < 0) {		
		ind = dpci_entry_get();
		if (ind < 0) {
			pr_err("Not enough entries\n");
			return -ENOMEM;
		}
	}
	
	ASSERT_COND(g_dpci_tbl.dpci_id[ind] == dpci_id);

	g_dpci_tbl.dpci_id[ind] = dpci_id;
	g_dpci_tbl.ic[ind] = amq_bdi;

	/* Updated DPCI peer if possible */
	err = dpci_get_peer_id(dpci_id, &dpci_id_peer);
	if (!err)
		g_dpci_tbl.dpci_id_peer[ind] = dpci_id_peer;
	else
		g_dpci_tbl.dpci_id_peer[ind] = DPCI_FQID_NOT_VALID;
	
	if (g_dpci_tbl.dpci_id_peer[ind] != DPCI_FQID_NOT_VALID) {
		err = tx_set((uint32_t)ind);
		ASSERT_COND(!err);
		/* AIOP DPCI to AIOP DPCI case 2 entries must be updated 
		 * 1->2 and 2->1 */
		err = dpci_mng_find(dpci_id_peer, NULL);
		if (err >= 0) {
			/* If here then 2 AIOP DPCIs are connected */
			g_dpci_tbl.dpci_id_peer[err] = dpci_id; 
			g_dpci_tbl.ic[err] = amq_bdi;
			tx_set((uint32_t)err);
			pr_debug("AIOP DPCI %d to AIOP DPCI %d\n", 
			         dpci_id, dpci_id_peer);
		}
	}
	
	return ind;
}

__COLD_CODE static int dpci_rx_ctx_init(uint32_t id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token;
	struct dpci_rx_queue_cfg queue_cfg;
	struct dpci_attr attr;
	uint8_t i;
	
	ASSERT_COND(dprc);

	/* memset */
	MEM_SET(&queue_cfg, sizeof(queue_cfg), 0);
	MEM_SET(&attr, sizeof(attr), 0);

	err = dpci_open(&dprc->io, (int)g_dpci_tbl.dpci_id[id], &token);
	if (err)
		return err;

	err = dpci_get_attributes(&dprc->io, token, &attr);
	if (err) {
		dpci_close(&dprc->io, token);
		return err;
	}

	queue_cfg.dest_cfg.dest_type = DPCI_DEST_NONE;
	queue_cfg.options = CMDIF_Q_OPTIONS;
	for (i = 0; i < attr.num_of_priorities; i++) {
		queue_cfg.dest_cfg.priority = DPCI_LOW_PR - i;
		queue_cfg.user_ctx = 0;
		CMDIF_DPCI_FQID(USER_CTX_SET, id, g_dpci_tbl.tx_queue[id][i]);
		err = dpci_set_rx_queue(&dprc->io, token, i,
		                         &queue_cfg);
		ASSERT_COND(!err);
	}

	err = dpci_close(&dprc->io, token);

	return err;
}

/*************************************************************************/

/*
 * New DPCI was added or the state of the DPCI has changed
 * The dpci_id must belong to AIOP side
 */
__COLD_CODE int dpci_event_assign(uint32_t dpci_id)
{
	int err = 0;
	int ind = 0;
	
	DPCI_DT_LOCK_W_TAKE;

	/*
	 * 1. Init DPCI entry
	 * 2. Update peer and tx queues if possible
	 * 3. Init rx_ctx
	 * 4. Update rx_ctx if possible
	 */
	ind = dpci_entry_init(dpci_id);
	if (ind >= 0) {
		err = dpci_rx_ctx_init((uint32_t)ind);
	}

	DPCI_DT_LOCK_RELEASE;

	if (g_dpci_tbl.mc_dpci_id != dpci_id) {
		/* TODO call EVM here */ 
	}
	
	dpci_tbl_dump();
	
	return err;
}

/*
 * The DPCI was removed from AIOP container
 * The dpci_id must belong to AIOP side
 */
__COLD_CODE int dpci_event_unassign(uint32_t dpci_id)
{
	int ind = -1;
	int err = 0;

	DPCI_DT_LOCK_W_TAKE;

	ind = dpci_mng_find(dpci_id, NULL);
	if (ind >= 0) {
		ASSERT_COND(g_dpci_tbl.dpci_id[ind] == dpci_id);
		dpci_entry_delete(ind);
		err = 0;
	} else {
		err = -ENOENT;
	}

	DPCI_DT_LOCK_RELEASE;
	
	if (g_dpci_tbl.mc_dpci_id != dpci_id) {
		/* TODO call EVM here */ 
	}

	dpci_tbl_dump();

	return err;
}


/*
 * The DPCI user context and AMQ bits are updated
 * This function is to be called only inside the open command and before
 * the AMQ bits had been changed to AIOP AMQ bits
 */
__COLD_CODE int dpci_event_update(uint32_t ind, uint8_t flags)
{
	int err = 0;
		
	/*
	 * TODO
	 * Is it possible that DPCI will be removed in the middle of the task ?
	 * Answer : NO
	 * AIOP SL will wait for all the tasks to finish and only then it will 
	 * delete the entry. Before the waiting AIOP SL will change the dpci 
	 * table or just entry for the new tasks only. 
	 */

	DPCI_ENTRY_LOCK_W_TAKE(ind);
	
	if (flags & DPCI_EVENT_UPDATE_ICID) {
		amq_bits_update(ind);
		/* TODO err = rx_ctx_set(ind);
		 * rx_ctx_set(ind) moved to added event
		 * Need to check if this is the right thing
		 * Can't get tx fqid from GPP inside command, 
		 * maybe for AIOP it is authorized
		 * with a different ICID
		 */
	}
	
	if (flags & DPCI_EVENT_UPDATE_TX) {
		err = tx_set(ind);
	}
	
	DPCI_ENTRY_LOCK_RELEASE(ind);
	return err;
}


__HOT_CODE void dpci_mng_user_ctx_get(uint32_t *id, uint32_t *fqid)
{
	uint64_t rx_ctx = CMDIF_RX_CTX_GET;
	uint32_t _id;
	uint32_t _fqid;

	CMDIF_DPCI_FQID(USER_CTX_GET, (&_id), (&_fqid));

	if (id)
		*id = _id;
	if (fqid)
		*fqid = _fqid;
}

__HOT_CODE void dpci_mng_icid_get(uint32_t ind, uint16_t *icid, uint16_t *amq_bdi_out)
{
	uint32_t amq_bdi = g_dpci_tbl.ic[ind];

	/* 1. Atomic read of 4 bytes so that icid and amq are always in sync 
	 * 2. Retrieve the icid and amq & bdi */
	
	CMDIF_ICID_AMQ_BDI(AMQ_BDI_GET, icid, amq_bdi_out);
}

__HOT_CODE void dpci_mng_tx_get(uint32_t ind, int pr, uint32_t *fqid)
{
	*fqid = g_dpci_tbl.tx_queue[ind][pr];
}

__COLD_CODE int dpci_drv_enable(uint32_t dpci_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token = 0xffff;
	
	ASSERT_COND(dprc);
	
	err = dpci_open(&dprc->io, (int)dpci_id, &token);
	if (err) {
		dpci_close(&dprc->io, token);
		return err;
	}
	
	err = dpci_enable(&dprc->io, token);
	if (err) {
		pr_err("DPCI enable failed\n");
		dpci_close(&dprc->io, token);
		return err;
	}
	
	/*
	 * TODO maybe need to keep enable bit in dt table ???
	 */
	dpci_close(&dprc->io, token);
	return err;
}

__COLD_CODE int dpci_drv_disable(uint32_t dpci_id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token = 0xffff;
	
	ASSERT_COND(dprc);
	
	err = dpci_open(&dprc->io, (int)dpci_id, &token);
	if (err) {
		dpci_close(&dprc->io, token);
		return err;
	}
	
	err = dpci_disable(&dprc->io, token);
	if (err) {
		pr_err("DPCI disable failed\n");
		dpci_close(&dprc->io, token);
		return err;
	}
	
	/*
	 * TODO maybe need to keep enable bit in dt table ???
	 * TODO draining !!!!!
	 */
	dpci_close(&dprc->io, token);
	return err;
}

__COLD_CODE static int dpci_tbl_create(int dpci_count)
{
	uint32_t size = 0;

	size = sizeof(struct dpci_mng_tbl);
	memset(&g_dpci_tbl, 0, size);

	size = sizeof(g_dpci_tbl.dpci_id);
	memset(&g_dpci_tbl.dpci_id, 0xff, size);

	size = sizeof(g_dpci_tbl.dpci_id_peer);
	memset(&g_dpci_tbl.dpci_id_peer, 0xff, size);

	size = sizeof(g_dpci_tbl.ic) ;
	memset(&g_dpci_tbl.ic, 0xff, size);

	size = sizeof(g_dpci_tbl.token);
	memset(&g_dpci_tbl.token, 0xff, size);

	size = sizeof(g_dpci_tbl.tx_queue);
	memset(&g_dpci_tbl.tx_queue, 0xff, size);

	size = sizeof(g_dpci_tbl.state);
	memset(&g_dpci_tbl.state, 0, size);

	g_dpci_tbl.count = 0;
	g_dpci_tbl.max = dpci_count;
	g_dpci_tbl.mc_dpci_id = 0xff;

	mem_disp((void *)&g_dpci_tbl, sizeof(g_dpci_tbl));
	
	return 0;
}

__COLD_CODE static int dpci_for_mc_add(struct mc_dprc *dprc)
{
	struct dpci_cfg dpci_cfg;
	uint16_t dpci = 0;
	struct dpci_rx_queue_cfg queue_cfg;
	struct dprc_endpoint endpoint1 ;
	struct dprc_endpoint endpoint2;
	struct dpci_attr attr;
	uint8_t p = 0;
	int     err = 0;
	int     link_up = 0;

	memset(&queue_cfg, 0, sizeof(struct dpci_rx_queue_cfg));
	memset(&attr, 0, sizeof(attr));

	dpci_cfg.num_of_priorities = 2;

	err |= dpci_create(&dprc->io, &dpci_cfg, &dpci);

	/* Get attributes just for dpci id fqids are not there yet */
	err |= dpci_get_attributes(&dprc->io, dpci, &attr);

	/* Connect to dpci that belongs to MC */
	g_dpci_tbl.mc_dpci_id = g_init_data.sl_info.mc_dpci_id;
	pr_debug("MC dpci ID[%d] \n", g_dpci_tbl.mc_dpci_id);
	
	memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
	memset(&endpoint2, 0, sizeof(struct dprc_endpoint));
	endpoint1.id = (int)g_init_data.sl_info.mc_dpci_id;
	endpoint1.interface_id = 0;
	strcpy(endpoint1.type, "dpci");

	endpoint2.id = attr.id;
	endpoint2.interface_id = 0;
	strcpy(endpoint2.type, "dpci");

	err = dprc_connect(&dprc->io, dprc->token, &endpoint1, &endpoint2);
	if (err) {
		pr_err("dprc_connect failed\n");
	}
	err = dpci_close(&dprc->io, dpci);
	ASSERT_COND(!err);
	
	err = dpci_event_assign((uint32_t)attr.id);
	ASSERT_COND(!err);

	err = dpci_drv_enable((uint32_t)attr.id);
	return err;
}

__COLD_CODE int dpci_drv_init()
{
	struct dprc_obj_desc dev_desc;
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpci_mng_tbl *dpci_tbl = NULL;
	int dev_count  = 0;
	int dpci_count = 0;
	int err        = 0;
	int i          = 0;

	if (dprc == NULL) {
		pr_err("No AIOP root container \n");
		return -ENODEV;
	}

	if ((err = dprc_get_obj_count(&dprc->io, dprc->token, &dev_count)) != 0) {
		pr_err("Failed to get device count for RC auth_d = %d\n",
		       dprc->token);
		return err;
	}

	/* First count how many DPCI objects we have */
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(&dprc->io, dprc->token, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {
			dpci_count++;
		}
	}

	err = dpci_tbl_create(DPCI_DYNAMIC_MAX);
	if (err != 0) {
		pr_err("Failed dpci_tbl_create() \n");
		return err;
	}

	err = dpci_for_mc_add(dprc);
	if (err) {
		pr_err("Failed to create and link AIOP<->MC DPCI \n");
	}
	
	dpci_tbl_dump();

	return err;
}

__COLD_CODE void dpci_drv_free()
{
	/*
	 * TODO do I need to free anything ? 
	 */
}
