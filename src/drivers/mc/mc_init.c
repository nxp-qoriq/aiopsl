#include "common/types.h"
#include "common/gen.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "general.h"
#include "sys.h"
#include "dbg.h"
#include "errors.h"
#include "dplib/fsl_dprc.h"

int mc_obj_init();
void mc_obj_free();

static int aiop_container_init()
{
	void *p_vaddr;
	int err = 0;
	int i = 0;
	int container_id;
	struct dprc *dprc = fsl_os_xmalloc(sizeof(struct dprc), 
	                                   MEM_PART_SH_RAM, 
	                                   1);
	if (dprc == NULL) {
		pr_err("No memory for AIOP Root Container \n");
		return -ENOMEM;		
	}
	memset(dprc, 0, sizeof(struct dprc));
	
	/* TODO: replace hard-coded portal address 1 with configured value */
	/* TODO : layout file must contain portal ID 1 in order to work. */
	/* TODO : in this call, can 3rd argument be zero? */
	/* Get virtual address of MC portal */
	p_vaddr = \
	UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
    	                             (uint32_t)1, E_MAPPED_MEM_TYPE_MC_PORTAL));

	/* Open root container in order to create and query for devices */
	dprc->regs = p_vaddr;
	if ((err = dprc_get_container_id(dprc, &container_id)) != 0) {
		pr_err("Failed to get AIOP root container ID.\n");
		return err;
	}
	if ((err = dprc_open(dprc, container_id)) != 0) {
		pr_err("Failed to open AIOP root container DP-RC%d.\n", 
		       container_id);
		return err;
	}
	
	err = sys_add_handle(dprc, FSL_OS_MOD_AIOP_RC, 1, 0);
	return err;
}

static void aiop_container_free()
{
	void *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	
	sys_remove_handle(FSL_OS_MOD_AIOP_RC, 0);
	
	if (dprc != NULL)
		fsl_os_xfree(dprc);		
}


int mc_obj_init()
{
	int err = 0;
	
	err |= aiop_container_init();
	
	return err;
	
}

void mc_obj_free()
{
	aiop_container_free();
}
