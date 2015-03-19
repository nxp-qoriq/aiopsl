#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "fsl_sys.h"
#include "fsl_dpci.h"
#include "fsl_dpci_drv.h"
#include "fsl_mc_init.h"
#include "fsl_dbg.h"
#include "cmdif_client.h"
#include "cmdif_srv.h"
#include "fsl_fdma.h"
#include "fsl_icontext.h"
#include "fsl_spinlock.h"

/*************************************************************************/
#define DPCI_LOW_PR	1
#define CMDIF_Q_OPTIONS (DPCI_QUEUE_OPT_USER_CTX | DPCI_QUEUE_OPT_DEST)

#define CMDIF_FQD_CTX_GET \
	(((struct additional_dequeue_context *)HWC_ADC_ADDRESS)->fqd_ctx) 

#define CMDIF_RX_CTX_GET \
	(LLLDW_SWAP((uint32_t)&CMDIF_FQD_CTX_GET, 0))

#define AMQ_BDI_SET(_offset, _width, _type, _arg) \
	(amq_bdi |= u32_enc((_offset), (_width), (_arg)))

#define AMQ_BDI_GET(_offset, _width, _type, _arg) \
	(*(_arg) = (_type)u32_dec(dt->ic[ind], (_offset), (_width)))

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


void dpci_amq_bdi_set();
int dpci_rx_ctx_set();
int dpci_amq_bdi_init(uint32_t dpci_id);
int dpci_rx_ctx_init(uint32_t dpci_id, uint32_t id);

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
	struct mc_dpci_tbl *dt = (struct mc_dpci_tbl *)\
		sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);
	uint32_t amq_bdi = 0;
	uint16_t amq_bdi_temp = 0;
	uint16_t pl_icid = PL_ICID_GET;
	int err;
	
	ADD_AMQ_FLAGS(amq_bdi_temp, pl_icid);
	if (BDI_GET != 0)
		amq_bdi_temp |= CMDIF_BDI_BIT;

	CMDIF_ICID_AMQ_BDI(AMQ_BDI_SET, ICID_GET(pl_icid), amq_bdi_temp);

	pr_debug("Updating peer of 0x%x which is now 0x%x\n", 
	         dt->dpci_id[id], 
	         dt->dpci_id_peer[id]);
	err = dpci_get_peer_id(dt->dpci_id[id], &(dt->dpci_id_peer[id]));
	ASSERT_COND(!err);
	pr_debug("Updated peer of 0x%x to 0x%x\n", 
	         dt->dpci_id[id], 
	         dt->dpci_id_peer[id]);
	/* Must be written last */
	dt->ic[id] = amq_bdi;
	mc_dpci_tbl_dump();
}

/* To be called in runtime only */
__COLD_CODE void dpci_amq_bdi_set()
{
	uint32_t id = 0;
	
	dpci_drv_user_ctx_get(&id, NULL);

	amq_bits_update(id);
}

/* To be called upon connected event, assign even */
__COLD_CODE int dpci_amq_bdi_init(uint32_t dpci_id)
{
	struct mc_dpci_tbl *dpci_tbl = (struct mc_dpci_tbl *)\
		sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);
	int ind = -1;
	uint32_t amq_bdi = 0;
	uint32_t dpci_id_peer = 0;
	int err = 0;

	CMDIF_ICID_AMQ_BDI(AMQ_BDI_SET, ICONTEXT_INVALID, ICONTEXT_INVALID);

	ind = mc_dpci_find(dpci_id, NULL);
	if (ind >= 0) {
		/* Updated DPCI peer if possible */
		err = dpci_get_peer_id(dpci_id, &dpci_id_peer);
		if (!err)
			dpci_tbl->dpci_id_peer[ind] = dpci_id_peer;
		else
			dpci_tbl->dpci_id_peer[ind] = DPCI_FQID_NOT_VALID;
		/* Must be last */
		dpci_tbl->ic[ind] = amq_bdi;
	} else {
		/* Adding new dpci_id */
		if (dpci_tbl->count < dpci_tbl->max) {
			lock_spinlock(&dpci_tbl->lock);
			
			ind = dpci_tbl->count;
			dpci_tbl->ic[ind] = amq_bdi;
			dpci_tbl->dpci_id[ind] = dpci_id;
			/* Updated DPCI peer if possible */
			err = dpci_get_peer_id(dpci_id, &dpci_id_peer);
			if (!err)
				dpci_tbl->dpci_id_peer[ind] = dpci_id_peer;
			else
				dpci_tbl->dpci_id_peer[ind] = DPCI_FQID_NOT_VALID;			
			/* Must be last */
			atomic_incr32(&dpci_tbl->count, 1);			
			unlock_spinlock(&dpci_tbl->lock);
		} else {
			pr_err("Not enough entries\n");
			return -ENOMEM;
		}
	}
	
	return ind;
}

__COLD_CODE int dpci_rx_ctx_init(uint32_t dpci_id, uint32_t id)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token;
	struct dpci_rx_queue_cfg queue_cfg;
	struct dpci_rx_queue_attr rx_attr;
	struct dpci_attr attr;
	uint8_t i;
	
	ASSERT_COND(dprc);

	/* memset */
	MEM_SET(&queue_cfg, sizeof(queue_cfg), 0);
	MEM_SET(&rx_attr, sizeof(rx_attr), 0);
	MEM_SET(&attr, sizeof(attr), 0);

	err = dpci_open(&dprc->io, (int)dpci_id, &token);
	if (err) {
		pr_err("Failed dpci_open dpci id = %d\n", dpci_id);
		return err;
	}
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
		CMDIF_DPCI_FQID(USER_CTX_SET, id, DPCI_FQID_NOT_VALID);
		err = dpci_set_rx_queue(&dprc->io, token, i,
		                         &queue_cfg);
		ASSERT_COND(!err);
	}

	err = dpci_enable(&dprc->io, token);
	if (err) {
		pr_err("DPCI enable failed\n");
	}
	err = dpci_close(&dprc->io, token);
	return err;
}

__COLD_CODE static int rx_ctx_set(uint32_t id, struct dpci_tx_queue_attr *tx)
{
	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token;
	struct dpci_rx_queue_cfg queue_cfg;
	struct dpci_attr attr;
	struct dpci_tx_queue_attr tx_attr;
	struct dpci_rx_queue_attr rx_attr;
	uint8_t i;
	struct mc_dpci_tbl *dt = (struct mc_dpci_tbl *)\
		sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	ASSERT_COND(dprc && dt);

	/* memset */
	MEM_SET(&queue_cfg, sizeof(queue_cfg), 0);
	MEM_SET(&rx_attr, sizeof(rx_attr), 0);
	MEM_SET(&tx_attr, sizeof(tx_attr), 0);
	MEM_SET(&attr, sizeof(attr), 0);
	
	err = dpci_open(&dprc->io, (int)dt->dpci_id[id], &token);
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

		err = dpci_get_tx_queue(&dprc->io, token, i, &tx_attr);
		ASSERT_COND(!err);
		ASSERT_COND(tx_attr.fqid != DPCI_FQID_NOT_VALID);
		queue_cfg.dest_cfg.priority = DPCI_LOW_PR - i;
		queue_cfg.user_ctx = 0;
		CMDIF_DPCI_FQID(USER_CTX_SET, id, tx_attr.fqid);
		err = dpci_set_rx_queue(&dprc->io, token, i,
		                         &queue_cfg);
		ASSERT_COND(!err);
		if (tx != NULL)
			tx[i].fqid = tx_attr.fqid;
	}

	err = dpci_close(&dprc->io, token);

	return err;
}

__COLD_CODE int dpci_rx_ctx_set()
{
	int err = 0;
	uint32_t id;

	dpci_drv_user_ctx_get(&id, NULL);
	err = rx_ctx_set(id, NULL);

	return err;
}


/*************************************************************************/

/*
 * New DPCI was added or the state of the DPCI has changed
 * The dpci_id must belong to AIOP side
 */
__COLD_CODE int dpci_drv_added(uint32_t dpci_id)
{
	int err = 0;
	
	err = dpci_amq_bdi_init(dpci_id);
	if (err >= 0) {
		err = dpci_rx_ctx_init(dpci_id, (uint32_t)err);
	}
	
	return err;
}

/*
 * The DPCI was removed from AIOP container
 * The dpci_id must belong to AIOP side
 */
__COLD_CODE int dpci_drv_removed(uint32_t dpci_id)
{
	
}


/*
 * The DPCI user context and AMQ bits are updated 
 * This function is to be called only inside the open command and before 
 * the AMQ bits had been changed to AIOP AMQ bits
 */
__COLD_CODE int dpci_drv_update(uint32_t ind)
{
	int err;
	
	amq_bits_update(ind);
	err = rx_ctx_set(ind, NULL);
	
	return err;	
}


__HOT_CODE void dpci_drv_user_ctx_get(uint32_t *id, uint32_t *fqid)
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

__HOT_CODE void dpci_drv_icid_get(uint32_t ind, uint16_t *icid, uint16_t *amq_bdi)
{
	struct mc_dpci_tbl *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	ASSERT_COND(dt);

	CMDIF_ICID_AMQ_BDI(AMQ_BDI_GET, icid, amq_bdi);
}

__COLD_CODE int dpci_drv_tx_get(uint32_t dpci_id, struct dpci_tx_queue_attr *tx)
{

	struct mc_dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	int err = 0;
	uint16_t token;
	struct dpci_attr attr;
	struct dpci_tx_queue_attr tx_attr;
	int i;
	struct mc_dpci_tbl *dt = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	ASSERT_COND(tx);

	/* memset */
	MEM_SET(&tx_attr, sizeof(tx_attr), 0);
	MEM_SET(&attr, sizeof(attr), 0);
		
	/* dpci id may belong to peer */
	i = mc_dpci_find(dpci_id, NULL);
	if (i < 0) {
		pr_err("Not found DPCI id or it's peer %d\n", dpci_id);
		return -ENAVAIL;
	}

	err = dpci_open(&dprc->io, dt->dpci_id[i], &token);
	if (err) 
		return err;

	err = dpci_get_attributes(&dprc->io, token, &attr);
	if (err) {
		dpci_close(&dprc->io, token);
		return err;
	}

	for (i = 0; i < attr.num_of_priorities; i++) {
		err = dpci_get_tx_queue(&dprc->io, token, i, &tx_attr);
		ASSERT_COND(!err);
		ASSERT_COND(tx_attr.fqid != DPCI_FQID_NOT_VALID);
		if (tx != NULL)
			tx[i].fqid = tx_attr.fqid;
	}

	err = dpci_close(&dprc->io, token);

	return err;	
}
