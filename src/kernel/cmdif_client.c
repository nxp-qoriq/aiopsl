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
#include "dplib/fsl_dprc.h"
#include "fsl_fdma.h"

void cmdif_client_free();
int cmdif_client_init();


static int send_fd(int pr, void *_sdev)
{
	int      err = 0;
	uint32_t fqid = 0;
	struct cmdif_sdev *sdev = (struct cmdif_sdev *)_sdev;
	
	if ((sdev == NULL) || (sdev->num_of_pr <= pr))
		return -EINVAL;
	fqid = ((struct cmdif_sdev *)sdev)->fqid[pr];	
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

	if (	(cl->gpp[0].ins_id == ins_id) && 
		(cl->gpp[0].sdev->id == dpci_id) && 
		(strncmp((const char *)&(cl->gpp[0].m_name[0]), 
		         m_name, 
		         M_NAME_CHARS) == 0)) {
		/* TODO AIOP need physical address !!!! cidesc->dev = */ 
		cidesc->regs = (void *)dpci_id; /* TODO change it to GPP */
		
	}

	return 0;
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
		cl->gpp[i].sdev = fsl_os_xmalloc(sizeof(struct cmdif_sdev), 
		                                 MEM_PART_SH_RAM, 
		                                 8);
		if (cl->gpp[i].sdev == NULL) {
			pr_err("No memory for client handle\n");
			return -ENOMEM;
		}		
		memset(cl->gpp[i].sdev, 0, sizeof(struct cmdif_sdev));
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
		if (cl->gpp[i].sdev != NULL) 
			fsl_os_xfree(cl->gpp[i].sdev);
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
		return -EINVAL; /* Buffer allocated by GPP */
	
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
	UNUSED(cmd_id);
	UNUSED(size);
	UNUSED(data);

	send_fd(pr, cidesc->regs);
	return -ENOTSUP;
}
