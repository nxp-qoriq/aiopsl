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

#include "common/fsl_string.h"
#include "fsl_errors.h"
#include "common/types.h"
#include "fsl_io.h"
#include "inc/fsl_gen.h"
#include "fsl_dbg.h"
#include "common/fsl_cmdif_flib.h"

#include "arch/fsl_soc.h"
#include "kernel/platform.h"
#include "kernel/device.h"
#include "kernel/layout.h"
#include "kernel/smp.h"

#include "dplib/fsl_dpci.h"
#include "fsl_dpci_mc.h"
#include "fsl_resman.h"
#include "cmdif_client.h"

#include "drivers/fsl_qbman_ctrl.h"
#include "drivers/fsl_qbman_portal.h"
#include "drivers/fsl_qbman_portal_ex.h"


/*
 * Pointer to this structure should be passed as cidesc->regs
 */
struct cmdif_reg {
	struct dpci_attr   dpci_attr;
	struct qbman_swp   *swp;
	struct dpci        *dpci;
};

#define MC_C1BL_START_ADDR  0x21010000
#define MC_C2BL_START_ADDR  0x21011000
#define QBMAN_PORTAL_IDX    20
#define QBMAN_ICID          0
#define QBMAN_IOBYPASS      1
#define QBMAN_VA            0

struct cmdif_reg   regs  = {0};

int aiop_cmdif_init(void** ci_regs);

static void config_memory()
{
	iowrite32(0x00000000, UINT_TO_PTR(MC_C1BL_START_ADDR));
	iowrite32(0x00000060,UINT_TO_PTR(MC_C1BL_START_ADDR+4)); // DP-DDR
	//iowrite32(0x0000004c,UINT_TO_PTR(MC_C1BL_START_ADDR+4)); // PEB
	//iowrite32(0x00000000,UINT_TO_PTR(MC_C1BL_START_ADDR+4)); // DP-SYSTEM

}

static void reset_config_memory()
{
	iowrite32(0x00000000, UINT_TO_PTR(MC_C1BL_START_ADDR));
	iowrite32(0x00000000,UINT_TO_PTR(MC_C1BL_START_ADDR+4));

}

static int init_dpci(void)
{
	struct dpci_cfg dpci_cfg;
	struct dpci_dest_cfg dest_cfg;
	struct dpci_mc_amq amq;
	int err = 0;
	struct dpci_mc_internal_info info;
	fsl_handle_t resman = NULL;
	void *device = NULL;

	if (regs.dpci == NULL) {
		memset(&regs.dpci_attr, 0, sizeof(struct dpci_attr));
		memset(&dpci_cfg, 0, sizeof(struct dpci_cfg));
		memset(&amq, 0, sizeof(struct dpci_mc_amq));

		resman = (struct resman *)sys_get_unique_handle(FSL_OS_MOD_RESMAN);		
		if (resman == NULL)
			return -ENODEV;

		device = resman_open_dev(resman, DP_RES_TYPE_DPCI, (uint16_t)-1,
		                         0, 0);
		if (device == NULL)
			return -ENODEV;
			

		regs.dpci = dpci_allocate();
		info.id = device_get_id(device);
		info.device = device;
		err = dpci_set_mc_info(regs.dpci, &info);

		dpci_cfg.aiop_ref_id = 1; // TODO 1 or 0 ???
		/* Must be set in low, high order */
		dpci_cfg.priorities[0] = 1;
		dpci_cfg.priorities[1] = 2;
		err |= dpci_init(regs.dpci, &dpci_cfg);

		dest_cfg.type = DPCI_DEST_NONE;
		dest_cfg.priority = 1;
		err |= dpci_set_rx_queue(regs.dpci, 0, &dest_cfg,0xAAAAAAAABBBBBBBB);
		dest_cfg.priority = 2;
		err |= dpci_set_rx_queue(regs.dpci, 1, &dest_cfg,0xABCDABCDABCDABCD);
		amq.bdi = 1;
		err |= dpci_set_amq(regs.dpci, &amq);

		err |= dpci_enable(regs.dpci);
		err |= dpci_get_attributes(regs.dpci, &regs.dpci_attr);
	}
	
	return err;
}

int aiop_cmdif_init(void** ci_regs)
{

	struct qbman_swp_desc pd;
	struct qbman_block *qb;
	int    err = 0;
	
	*ci_regs =  NULL;
	
	if (regs.swp == NULL) {
		/******************************************/
		/* Allocate a QBMan portal and prepare it */
		/******************************************/
		qb = sys_get_handle(FSL_OS_MOD_QBMAN, 0);
		pd.block = qbman_block_get_desc(qb);
		pd.cena_bar = 0xdeadbeef; /* Should be ignored */
		pd.cinh_bar = 0xdeadbeef; /* Should be ignored */
		pd.irq = -1;
		pd.idx = QBMAN_PORTAL_IDX; /* <-- this is the value we care about */

		/* This is how MC sets up a portal prior to giving it to a user. NB, the
		 * "_enabled" would be set to 1 in order to allow GPP/AIOP access to
		 * QBMan portal, whereas this is unnecessary if the MC is accessing the
		 * portal via its CCSR "back door". */
		qbman_block_set_swp_icid(qb, &pd, QBMAN_ICID);
		qbman_block_set_swp_iobypass(qb, &pd, QBMAN_IOBYPASS);
		qbman_block_set_swp_sdest(qb, &pd, 0);
		qbman_block_set_swp_va(qb, &pd, QBMAN_VA);
		qbman_block_set_swp_isolated(qb, &pd, 0);
		/* TODO: for MC access only, enabled==0 should be fine. The sim doesn't
		 * yet understand this... */
		qbman_block_set_swp_enabled(qb, &pd, 1);

		/**********************/
		/* Init the SW portal */
		/**********************/
		regs.swp = qbman_swp_init(&pd);

		if (!regs.swp)
			return -1;
	}

	err = init_dpci();
	
	if (!err)
		*ci_regs = &regs;
	
	return err;
}

static int send_fd(struct cmdif_fd *cfd, int pr, void *sdev)
{
	struct   cmdif_reg *dev = (struct cmdif_reg *)sdev;
	struct   qbman_eq_desc eqdesc;
	struct qbman_eq_response response;
	uint16_t fqid = 0;
	int      ret  = 0;
	struct   qbman_fd fd;
	struct   qbman_swp *swp = NULL;

	if ((pr < 0) || (pr > CMDIF_PRI_HIGH) || (dev == NULL))
		return -EINVAL;

	fqid = dev->dpci_attr.dpci_prio_attr[pr].tx_qid;
	swp = dev->swp;

	/*********************************/
	/* Prepare a enqueue descriptor */
	/*********************************/
	qbman_eq_desc_clear(&eqdesc);
	qbman_eq_desc_set_no_orp(&eqdesc, 1);
	qbman_eq_desc_set_response(&eqdesc, fsl_os_virt_to_phys((void*)&response), 0);
	qbman_eq_desc_set_token(&eqdesc, 0x99);
	qbman_eq_desc_set_fq(&eqdesc, fqid);
	
	/******************/
	/* Copy FD        */
	/******************/
	memset(&fd, 0, sizeof(struct qbman_fd));
	fd.simple.frc     = cfd->u_frc.frc;
	fd.simple.len     = cfd->d_size;
	fd.simple.flc_hi  = cfd->u_flc.word[0];
	fd.simple.flc_lo  = cfd->u_flc.word[1];
	fd.simple.addr_hi = cfd->u_addr.word[0];
	fd.simple.addr_lo = cfd->u_addr.word[1];
	
	/******************/
	/* Try an enqueue */
	/******************/
	ret = qbman_swp_enqueue(swp, &eqdesc, (const struct qbman_fd *)&fd);
	return ret;
}

static int receive_fd(struct cmdif_fd *cfd, int pr, void *sdev)
{
	const struct           qbman_dq_entry *dq_storage;
	struct cmdif_reg       *dev = (struct cmdif_reg *)sdev;
	const struct qbman_fd  *fd;
	struct qbman_pull_desc pulldesc;
	struct qbman_swp       *swp = NULL;
	uint16_t               fqid = 0;
	int                    ret  = 0;

	if ((pr < 0) || (pr > CMDIF_PRI_HIGH) || (dev == NULL))
		return -EINVAL;

	fqid = dev->dpci_attr.dpci_prio_attr[pr].rx_qid;
	swp = dev->swp;

	do {
		qbman_pull_desc_clear(&pulldesc);
		qbman_pull_desc_set_storage(&pulldesc, NULL, NULL, 0);
		qbman_pull_desc_set_numframes(&pulldesc, 1);
		qbman_pull_desc_set_token(&pulldesc, 0xab);
		qbman_pull_desc_set_fq(&pulldesc, fqid);
		ret = qbman_swp_pull(swp, &pulldesc);
		if (ret)
			pr_err("Failed qbman_swp_pull\n");
		dq_storage = qbman_swp_dqrr_next(swp);

		/* Pull dequeues always produce at least one
		 * result, but if there is nothing to dequeue, the "VALIDFRAME"
		 * flag won't be set */
	} while (!dq_storage
			&& !(qbman_dq_entry_DQ_flags(dq_storage)
				& QBMAN_DQ_STAT_VALIDFRAME));

	fd = qbman_dq_entry_DQ_fd(dq_storage);
	qbman_swp_dqrr_consume(swp, dq_storage);

	/******************/
	/* Copy FD        */
	/******************/
	cfd->u_frc.frc      = fd->simple.frc;
	cfd->d_size         = fd->simple.len;
	cfd->u_flc.word[0]  = fd->simple.flc_hi;
	cfd->u_flc.word[1]  = fd->simple.flc_lo;
	cfd->u_addr.word[0] = fd->simple.addr_hi;
	cfd->u_addr.word[1] = fd->simple.addr_lo;
	
	/******************/
	/* Endianess     */
	/******************/
	//endian_fix(cfd); // TODO check if needed

	return 0;
}

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
	
	config_memory();	
	err = cmdif_open_cmd(cidesc, m_name, inst_id, async_cb, async_ctx,
	                     v_data, p_data, size, &fd);	
	reset_config_memory();

	if (err)
		return err;

	err = send_fd(&fd, CMDIF_PRI_LOW, cidesc->regs);

	/* Wait for response from Server
	 * TODO add timeout */
	config_memory();	
	
	while (!cmdif_sync_ready(cidesc)) {}	
	err = cmdif_open_done(cidesc);
	
	reset_config_memory();
	
	return err; 
}


int cmdif_close(struct cmdif_desc *cidesc)
{
	struct cmdif_fd fd;
	int    err = 0;

	config_memory();	

	err = cmdif_close_cmd(cidesc, &fd);
	
	reset_config_memory();

	if (err)
		return err;

	err = send_fd(&fd, CMDIF_PRI_LOW, cidesc->regs);

	/* Wait for response from Server
	 * TODO add timeout */
	config_memory();

	while (!cmdif_sync_ready(cidesc)) {}
	err = cmdif_close_done(cidesc);
	
	reset_config_memory();

	return err;
}

int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int priority,
		uint64_t data)
{
	struct cmdif_fd fd;
	int    err = 0;

	config_memory();
	
	err = cmdif_cmd(cidesc, cmd_id, size, data, &fd);
	
	reset_config_memory();

	if (err)
		return err;

	err = send_fd(&fd, priority, cidesc->regs);

	if (cmdif_is_sync_cmd(cmd_id)) {
		/* Wait for response from Server
		 * TODO add timeout */
		config_memory();

		while (!cmdif_sync_ready(cidesc)) {}		
		err = cmdif_sync_cmd_done(cidesc);
		
		reset_config_memory();
		return err;
	}

	return 0;
}

int cmdif_resp_read(struct cmdif_desc *cidesc, int priority)
{
	struct cmdif_fd fd;
	int    err = 0;

	if (cidesc == NULL)
		return -EINVAL;

	err = receive_fd(&fd, priority, cidesc->regs);
	if (err)
		return err;
	while(!err) {
		
		config_memory();
		
		err = cmdif_async_cb(&fd);
		
		reset_config_memory();

		if (err)
			return err;
		err = receive_fd(&fd, priority, cidesc->regs);
	}
	return 0;
}

