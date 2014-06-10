#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "general.h"
#include "sys.h"
#include "dbg.h"
#include "errors.h"
#include "cmdif_client_aiop.h"
#include "fsl_cmdif_fd.h"
#include "fsl_cmdif_flib_c.h"
#include "dplib/fsl_dprc.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"


#define CMDIF_TIMEOUT  0x100000

void cmdif_client_free();
int cmdif_client_init();

static int send_fd(struct cmdif_fd *fd, int pr, void *_sdev)
{
	int      err = 0;
	struct cmdif_reg *sdev = (struct cmdif_reg *)_sdev;
	struct ldpaa_fd _fd __attribute__((aligned(sizeof(struct ldpaa_fd))));
	uint32_t fqid = sdev->fqid[pr];
	uint16_t icid = 0;
	
	fdma_enqueue_fd_fqid(&_fd, FDMA_EN_TC_CONDTERM_BITS | FDMA_ENF_BDI_BIT, 
	                     fqid, icid);
	/* TODO FDMA_ENF_BDI_BIT ??? */
	
	if ((sdev == NULL) || (sdev->num_of_pr <= pr) || (fd == NULL))
		return -EINVAL;
	
	/* TODO copy fields from FD */
	
	fqid = ((struct cmdif_reg *)sdev)->fqid[pr];	
	err = (int)fdma_store_and_enqueue_default_frame_fqid(
					fqid, FDMA_EN_TC_CONDTERM_BITS);
	if (err) {
		pr_err("Failed to send response\n");
		fdma_terminate_task();
	}
	return err;
}

static int session_get(const char *m_name, 
                       uint8_t ins_id, 
                       uint32_t dpci_id, 
                       struct cmdif_desc *cidesc)
{
	struct cmdif_cl *cl = sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL);
	int i = 0;
	
	for (i = 0; i < CMDIF_MN_SESSIONS; i++) {
		if (	(cl->gpp[i].ins_id == ins_id) && 
			(cl->gpp[i].regs->id == dpci_id) && 
			(strncmp((const char *)&(cl->gpp[i].m_name[0]), 
			         m_name, 
			         M_NAME_CHARS) == 0)) {
			cidesc->regs = (void *)cl->gpp[i].regs;
			cidesc->dev  = (void *)cl->gpp[i].dev;
			return 0;
		}
	}

	return -ENAVAIL;
}

static int dpci_discovery()
{
	int dev_count;
	struct dprc_obj_desc dev_desc;
	int err = 0;
	int i = 0;
	struct dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
			
	if ((err = dprc_get_obj_count(dprc, &dev_count)) != 0) {
	    pr_err("Failed to get device count for RC auth_d = %d\n", 
	           dprc->auth);
	    return err;
	}

	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(dprc, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {			
			pr_debug(" device %d\n");
			pr_debug("***********\n");
			pr_debug("vendor - %x\n", dev_desc.vendor);
			pr_debug("type - %s\n", dev_desc.type);
			pr_debug("id - %d\n", dev_desc.id);
			pr_debug("region_count - %d\n", dev_desc.region_count);
			pr_debug("state - %d\n", dev_desc.state);
			pr_debug("ver_major - %d\n", dev_desc.ver_major);
			pr_debug("ver_minor - %d\n", dev_desc.ver_minor);
			pr_debug("irq_count - %d\n\n", dev_desc.irq_count);
			
			/* TODO query about DPCI*/
		}
	}

	return err;
}

int cmdif_client_init()
{
	int err = 0;
	int i   = 0;
	struct cmdif_cl *cl = fsl_os_xmalloc(sizeof(struct cmdif_cl), 
	                                     MEM_PART_1ST_DDR_NON_CACHEABLE, 
	                                     8);
	if (cl == NULL) {
		pr_err("No memory for client handle\n");
		return -ENOMEM;
	}
	
	memset(cl, 0, sizeof(struct cmdif_cl));
	
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
	
	if (sys_get_unique_handle(FSL_OS_MOD_CMDIF_CL))
		return -ENODEV;
	
	
#ifndef AIOP_STANDALONE
	dpci_discovery();
#endif
	
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
	
	if (cl != NULL)
		fsl_os_xfree(cl);
	
	for (i = 0; i < CMDIF_MN_SESSIONS; i++) {
		if (cl->gpp[i].regs != NULL) 
			fsl_os_xfree(cl->gpp[i].regs);
		if (cl->gpp[i].dev != NULL) 
			fsl_os_xfree(cl->gpp[i].dev);
	}
	
}

int cmdif_open(struct cmdif_desc *cidesc,
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
	if (err != 0)
		return err;
	
	dev = (struct cmdif_dev *)cidesc->dev;
	dev->async_cb  = async_cb;
	dev->async_ctx = async_ctx;
	
	return err;
}

int cmdif_send(struct cmdif_desc *cidesc,
		uint16_t cmd_id,
		uint32_t size,
		int pr,
		uint64_t data)
{
	struct   cmdif_fd fd;
	int      err = 0;
	int      t   = 0;
	union cmdif_data done;
	
	err = cmdif_cmd(cidesc, cmd_id, size, data, &fd);

	err = send_fd(&fd, pr, cidesc->regs); /* TODO handle error ?*/
	
	if (cmdif_is_sync_cmd(cmd_id)) {
		do {
			cdma_read(&done, 
			          ((struct cmdif_dev *)cidesc->dev)->p_sync_done, 
			          4);
			t++;
		} while ((done.resp.done == 0) && (t < CMDIF_TIMEOUT));
		
		if (done.resp.done == 0)
			return -ETIMEDOUT;
		else
			return done.resp.err;
	}
	
	return 0;
}
