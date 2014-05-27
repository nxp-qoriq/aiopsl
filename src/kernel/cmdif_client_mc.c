#include "common/fsl_string.h"
#include "common/errors.h"
#include "common/types.h"
#include "common/io.h"
#include "common/gen.h"
#include "arch/fsl_soc.h"
#include "kernel/platform.h"
#include "common/dbg.h"

#include <cmdif_client.h>
#include <fsl_cmdif_flib.h>
#include <dplib/fsl_dpci.h>
#include <drivers/fsl_qbman_ctrl.h>
#include <drivers/fsl_qbman_portal.h>
#include <drivers/fsl_qbman_portal_ex.h>

/*
 * Pointer to this structure should be passed as cidesc->regs
 */
struct cmdif_reg {
	struct dpci_attr   dpci_attr;
	struct qbman_swp   *swp;
};

#define MC_C1BL_START_ADDR  0x21010000
#define MC_C2BL_START_ADDR  0x21011000

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

	err = cmdif_close_cmd(cidesc, &fd);
	
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

	err = cmdif_cmd(cidesc, cmd_id, size, data, &fd);

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
		err = cmdif_async_cb(&fd);
		if (err)
			return err;
		err = receive_fd(&fd, priority, cidesc->regs);
	}
	return 0;
}

