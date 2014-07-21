#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "general.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "cmdif_client_aiop.h"
#include "cmdif.h"
#include "fsl_cmdif_fd.h"
#include "fsl_cmdif_flib_c.h"
#include "dplib/fsl_dprc.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"
#include "fsl_endian.h"
#include "fsl_general.h"
#include "ls2085_aiop/fsl_platform.h"
#include "fsl_spinlock.h"
#include "fsl_io_ccsr.h"

#define CMDIF_TIMEOUT     0x10000000

#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,         \
					0,                          \
					E_MAPPED_MEM_TYPE_GEN_REGS) \
					+ SOC_PERIPH_OFF_AIOP_WRKS);

/* TODO get rid of it !
 * Should move to stack */
__TASK static struct ldpaa_fd _fd \
__attribute__((aligned(sizeof(struct ldpaa_fd))));


void cmdif_client_free();
int cmdif_client_init();
void cmdif_cl_isr();

static int epid_setup()
{
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;
	uint32_t data = 0;

	iowrite32_ccsr(CMDIF_EPID, &wrks_addr->epas); /* EPID = 2 */
	iowrite32_ccsr(PTR_TO_UINT(cmdif_cl_isr), &wrks_addr->ep_pc);

#ifdef AIOP_STANDALONE
	/* Default settings */
	iowrite32_ccsr(0x00600040, &wrks_addr->ep_fdpa);
	iowrite32_ccsr(0x010001c0, &wrks_addr->ep_spa);
	iowrite32_ccsr(0x00000000, &wrks_addr->ep_spo);
#endif
	/* no PTA presentation is required (even if there is a PTA)*/
	iowrite32_ccsr(0x0000ffc0, &wrks_addr->ep_ptapa);
	/* set epid ASA presentation size to 0 */
	iowrite32_ccsr(0x00000000, &wrks_addr->ep_asapa);
	/* Set mask for hash to 16 low bits OSRM = 5 */
	iowrite32_ccsr(0x11000005, &wrks_addr->ep_osc);
	data = ioread32_ccsr(&wrks_addr->ep_osc);
	if (data != 0x11000005)
		return -EINVAL;

	pr_info("CMDIF Client is setting EPID = %d\n", CMDIF_EPID);
	pr_info("ep_pc = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_pc));
	pr_info("ep_fdpa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_fdpa));
	pr_info("ep_ptapa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_ptapa));
	pr_info("ep_asapa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_asapa));
	pr_info("ep_spa = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_spa));
	pr_info("ep_spo = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_spo));
	pr_info("ep_osc = 0x%x \n", ioread32_ccsr(&wrks_addr->ep_osc));

	return 0;
}

__HOT_CODE static int send_fd(struct cmdif_fd *fd, int pr, void *_sdev)
{
	int    err = 0;
	struct cmdif_reg *sdev = (struct cmdif_reg *)_sdev;
	uint32_t fqid = 0;
	uint32_t flags = FDMA_EN_TC_RET_BITS;

	if ((sdev == NULL) 				||
		(sdev->attr->num_of_priorities <= pr)	||
		(fd == NULL))
		return -EINVAL;

	/* Copy fields from FD  */
	_fd.addr   = CPU_TO_LE64(fd->u_addr.d_addr);
	_fd.flc    = CPU_TO_LE64(fd->u_flc.flc);
	_fd.frc    = CPU_TO_LE32(fd->u_frc.frc);
	_fd.length = CPU_TO_LE32(fd->d_size);
	_fd.control = 0;
	_fd.offset  = 0;

	fqid = sdev->attr->dpci_prio_attr[pr].tx_qid;

	if (BDI_GET != 0)
		flags |= FDMA_ENF_BDI_BIT;

	pr_debug("Sending to fqid 0x%x fdma flags = 0x%x\n", fqid, flags);

	err = fdma_enqueue_fd_fqid(&_fd, flags , fqid, ICID_GET);
	if (err) {
		pr_err("Failed to send response\n");
		return -EIO;
	}

	return err;
}

__HOT_CODE static int session_get(const char *m_name,
                       uint8_t ins_id,
                       uint32_t dpci_id,
                       struct cmdif_desc *cidesc)
{
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int i = 0;

	/* TODO place it under debug ? */
	if (cl == NULL) {
		return -ENODEV;
	}

	/* TODO if sync mode is supported
	 * Sharing the same auth_id will require management of opened or not
	 * there won't be 2 cidesc with same auth_id because
	 * the same sync buffer is going to be used for 2 cidesc
	 * but as for today we don't support sync on AIOP client
	 * that's why it is working */
	lock_spinlock(&cl->lock);
	for (i = 0; i < cl->count; i++) {
		if ((cl->gpp[i].ins_id == ins_id) &&
			(cl->gpp[i].regs->attr->peer_id == dpci_id) &&
			(strncmp((const char *)&(cl->gpp[i].m_name[0]),
			         m_name,
			         M_NAME_CHARS) == 0)) {
			cidesc->regs = (void *)cl->gpp[i].regs;
			cidesc->dev  = (void *)cl->gpp[i].dev;
			unlock_spinlock(&cl->lock);
			return 0;
		}
	}
	unlock_spinlock(&cl->lock);
	return -ENAVAIL;
}

int cmdif_client_init()
{
	int err = 0;
	int i   = 0;
	struct cmdif_cl *cl = NULL;

	if (sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL)) {
		pr_err("Client had been already setup\n");
		return -ENODEV;
	}

	cl = fsl_os_xmalloc(sizeof(struct cmdif_cl),
	                    MEM_PART_SH_RAM,
	                    8);
	if (cl == NULL) {
		pr_err("No memory for client handle\n");
		return -ENOMEM;
	}

	memset(cl, 0, sizeof(struct cmdif_cl));

	err = epid_setup();
	if (err) {
		pr_err("Failed to setup EPID 2 for AIOP client\n");
		return -ENODEV;
	}

	for (i = 0; i < CMDIF_MN_SESSIONS; i++) {
		cl->gpp[i].regs = fsl_os_xmalloc(sizeof(struct cmdif_reg),
		                                 MEM_PART_SH_RAM,
		                                 8);
		cl->gpp[i].dev = fsl_os_xmalloc(sizeof(struct cmdif_dev),
		                                 MEM_PART_SH_RAM,
		                                 8);
		if ((cl->gpp[i].regs == NULL) || (cl->gpp[i].dev == NULL)) {
			pr_err("No memory for client handle\n");
			return -ENOMEM;
		}
		memset(cl->gpp[i].regs, 0, sizeof(struct cmdif_reg));
		memset(cl->gpp[i].dev, 0, sizeof(struct cmdif_dev));
	}


	err = sys_add_handle(cl, FSL_OS_MOD_CMDIF_CL, 1, 0);
	if (err != 0) {
		pr_err("Can't add client handle\n");
		return err;
	}
	return 0;
}

void cmdif_client_free()
{
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int i = 0;

	if (cl != NULL) {
		for (i = 0; i < CMDIF_MN_SESSIONS; i++) {
			if (cl->gpp[i].regs != NULL)
				fsl_os_xfree(cl->gpp[i].regs);
			if (cl->gpp[i].dev != NULL)
				fsl_os_xfree(cl->gpp[i].dev);
		}
		fsl_os_xfree(cl);
	}

}

__HOT_CODE int cmdif_open(struct cmdif_desc *cidesc,
		const char *module_name,
		uint8_t ins_id,
		cmdif_cb_t async_cb,
		void *async_ctx,
		uint8_t *v_data,
		uint64_t p_data,
		uint32_t size)
{
	struct cmdif_dev *dev = NULL;
	int    err = 0;

	if ((v_data != NULL) || (p_data != NULL) || (size > 0))
		return -EINVAL; /* Buffer are allocated by GPP */

	err = session_get(module_name, ins_id, (uint32_t)cidesc->regs, cidesc);
	if (err != 0) {
		pr_err("Session not found\n");
		return err;
	}

	dev = (struct cmdif_dev *)cidesc->dev;
	dev->async_cb  = async_cb;
	dev->async_ctx = async_ctx;

	return err;
}

__HOT_CODE int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int pr,
		uint64_t data)
{
	struct   cmdif_fd fd;
	int      err = 0;
	int      t   = 0;
	union cmdif_data done;

	if (cmdif_is_sync_cmd(cmd_id))
		return -ENOTSUP;

	err = cmdif_cmd(cidesc, cmd_id, size, data, &fd);
	if (err)
		return -EINVAL;

	err = send_fd(&fd, pr, cidesc->regs);
	if (err)
		return -EINVAL;

	if (cmdif_is_sync_cmd(cmd_id)) {

		uint64_t p_sync = \
			((struct cmdif_dev *)cidesc->dev)->p_sync_done;

		do {
			/* Same as
			cdma_read(&done,
			          ((struct cmdif_dev *)cidesc->dev)->p_sync_done,
			          4); */
			fdma_dma_data(4, ICID_GET, &done,
			              p_sync, FDMA_DMA_DA_SYS_TO_WS_BIT);

			t++;
		} while ((done.resp.done == 0) && (t < CMDIF_TIMEOUT));

		pr_debug("PASSED sync cmd 0x%x \n", cmd_id);
		done.resp.done = 0;
		if (t >= CMDIF_TIMEOUT)
			return -ETIMEDOUT;
		else
			return done.resp.err;
	}

	pr_debug("PASSED sent async or no response cmd 0x%x \n", cmd_id);
	return 0;
}

#pragma push
#pragma force_active on
__HOT_CODE void cmdif_cl_isr(void)
{
	int err = 0;
	struct cmdif_fd fd;

	fd.d_size        = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	fd.u_addr.d_addr = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
	fd.u_flc.flc     = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	fd.u_frc.frc     = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);

	err = cmdif_async_cb(&fd);
	if (err) {
		pr_debug("Async callback cmd 0x%x returned error %d", \
		         fd.u_flc.cmd.cmid, err);
	}

	pr_debug("PASSED got async response for cmd 0x%x\n", \
	         CPU_TO_SRV16(fd.u_flc.cmd.cmid));

	fdma_store_default_frame_data();
	fdma_terminate_task();
}
#pragma pop

