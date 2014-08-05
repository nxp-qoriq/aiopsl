#include "common/types.h"
#include "inc/fsl_gen.h"
#include "fsl_errors.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "general.h"
#include "sys.h"
#include "fsl_dbg.h"
#include "dplib/fsl_dprc.h"
#include "dplib/fsl_dpci.h"
#include "fsl_mc_init.h"
#include "ls2085_aiop/fsl_platform.h"

int mc_obj_init();
void mc_obj_free();

#define DPCI_LOW_PR  1
#define MC_DPCI_NUM 1
#define MC_DPCI_ID  0

static int aiop_container_init()
{
	void *p_vaddr;
	int err = 0;
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

static int dpci_tbl_create(struct dpci_obj **_dpci_tbl, int dpci_count)
{
	uint32_t size = 0;
	struct   dpci_obj *dpci_tbl = NULL;
	int      err = 0;

	size = sizeof(struct dpci_obj); /* use for size */
	dpci_tbl = fsl_os_xmalloc(size, MEM_PART_SH_RAM, 1);
	*_dpci_tbl = dpci_tbl;
	if (dpci_tbl == NULL) {
		pr_err("No memory for %d DPCIs\n", dpci_count);
		return -ENOMEM;
	}
	memset(dpci_tbl, 0, size);

	size = sizeof(struct dpci_attr) * dpci_count; /* use for size */
	dpci_tbl->attr = fsl_os_xmalloc(size, MEM_PART_SH_RAM, 1);
	if (dpci_tbl->attr == NULL) {
		pr_err("No memory for %d DPCIs\n", dpci_count);
		return -ENOMEM;
	}
	memset(dpci_tbl->attr, 0, size);

	size = sizeof(struct dpci) * dpci_count; /* use for size */
	dpci_tbl->dpci = fsl_os_xmalloc(size, MEM_PART_SH_RAM, 1);
	if (dpci_tbl->dpci == NULL) {
		pr_err("No memory for %d DPCIs\n", dpci_count);
		return -ENOMEM;
	}
	memset(dpci_tbl->dpci, 0, size);

	err = sys_add_handle(dpci_tbl,
	                     FSL_OS_MOD_DPCI_TBL,
	                     1,
	                     0);
	if (err != 0) {
		pr_err("FSL_OS_MOD_DPCI_TBL sys_add_handle failed\n");
		return err;
	}

	return err;
}

static int dpci_tbl_add(struct dprc_obj_desc *dev_desc, int ind,
                        struct dpci_obj *dpci_tbl, struct dprc *dprc)
{
	struct  dpci *dpci;
	struct  dpci_dest_cfg dest_cfg;
	int     err = 0;
	uint8_t p   = 0;

	if (dev_desc == NULL)
		return -EINVAL;

	pr_debug(" Found DPCI device\n");
	pr_debug("***********\n");
	pr_debug("vendor - %x\n", dev_desc->vendor);
	pr_debug("type - %s\n", dev_desc->type);
	pr_debug("id - %d\n", dev_desc->id);
	pr_debug("region_count - %d\n", dev_desc->region_count);
	pr_debug("state - %d\n", dev_desc->state);
	pr_debug("ver_major - %d\n", dev_desc->ver_major);
	pr_debug("ver_minor - %d\n", dev_desc->ver_minor);
	pr_debug("irq_count - %d\n\n", dev_desc->irq_count);

	memset(&dest_cfg, 0, sizeof(struct dpci_dest_cfg));

	dpci = &dpci_tbl->dpci[ind];
	dpci->regs = dprc->regs;
	err |= dpci_open(dpci, dev_desc->id);
	/* Set priorities 0 and 1
	 * 0 is high priority
	 * 1 is low priority
	 * Making sure that low priority is at index 0*/
	dest_cfg.type = DPCI_DEST_NONE;
	for (p = 0; p <= DPCI_LOW_PR; p++) {
		dest_cfg.priority = DPCI_LOW_PR - p;
		err |= dpci_set_rx_queue(dpci,
		                         p,
		                         &dest_cfg,
		                         (ind << 1) | p);
	}
	err |= dpci_enable(dpci);
	err |= dpci_get_attributes(dpci,
	                           &dpci_tbl->attr[ind]);
	if (err) {
		pr_err("Failed dpci initialization \n");
		dpci_tbl->count = ind;
		return -ENODEV;
	}

	return 0;
}

static int dpci_for_mc_add(struct dpci_obj *dpci_tbl, struct dprc *dprc, int ind)
{
	struct dpci_cfg dpci_cfg;
	struct dpci *dpci;
	struct dpci_dest_cfg dest_cfg;
	struct dprc_endpoint endpoint1 ;
	struct dprc_endpoint endpoint2;
	uint8_t p = 0;
	int     err = 0;
	int     link_up = 0;

	dpci_cfg.num_of_priorities = 2;

	dpci = &dpci_tbl->dpci[ind];
	/* Create DPCI on container 0 with portal 0*/
	dpci->regs = dprc->regs;

	err |= dpci_create(dpci, &dpci_cfg);
	/* Set priorities 0 and 1
	 * 0 is high priority
	 * 1 is low priority
	 * Making sure that low priority is at index 0*/
	dest_cfg.type = DPCI_DEST_NONE;
	for (p = 0; p <= DPCI_LOW_PR; p++) {
		dest_cfg.priority = DPCI_LOW_PR - p;
		err |= dpci_set_rx_queue(dpci,
		                         p,
		                         &dest_cfg,
		                         (ind << 1) | p);
	}

	/* Get attributes just for dpci id,
	 * fqids are not there yet */
	err |= dpci_get_attributes(dpci,
	                           &dpci_tbl->attr[ind]);

	/* Connect to dpci 0 that belongs to MC */
	memset(&endpoint1, 0, sizeof(struct dprc_endpoint));
        memset(&endpoint2, 0, sizeof(struct dprc_endpoint));
        endpoint1.id = MC_DPCI_ID;
        endpoint1.interface_id = 0;
        strcpy(endpoint1.type, "dpci");

        endpoint2.id = dpci_tbl->attr[ind].id;
        endpoint2.interface_id = 0;
        strcpy(endpoint2.type, "dpci");

	err |= dpci_enable(dpci);
        err |= dprc_connect(dprc, &endpoint1, &endpoint2);
	err |= dpci_get_attributes(dpci, &dpci_tbl->attr[ind]);
	err |= dpci_get_link_state(dpci, &link_up);
	if (!link_up) {
		pr_err("MC<->AIOP DPCI link is down !\n");
	}

	return err;
}

static int dpci_tbl_fill(struct dpci_obj *dpci_tbl, struct dprc *dprc,
                         int dpci_count, int dev_count)
{
	int ind = 0;
	int i   = 0;
	int err = 0;
	struct dprc_obj_desc dev_desc;

	if (dpci_tbl == NULL) {
		pr_err("No DPCI objects in AIOP root container \n");
		return -EINVAL;
	}

	while ((i < dev_count) && (ind < dpci_count)) {
		dprc_get_obj(dprc, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {
			err = dpci_tbl_add(&dev_desc, ind, dpci_tbl, dprc);
			if (err) {
				pr_err("Failed dpci_tbl_add \n");
				dpci_tbl->count = ind;
				return -ENODEV;
			}
			ind++;
		}
		i++;
	}

	err = dpci_for_mc_add(dpci_tbl, dprc, ind);
	if (err) {
		pr_err("Failed to create and link AIOP<->MC DPCI \n");
		dpci_tbl->count = ind;
	} else {
		dpci_tbl->count = ind + MC_DPCI_NUM;
	}
	return err;
}

static int dpci_discovery()
{
	struct dprc_obj_desc dev_desc;
	struct dprc *dprc = sys_get_unique_handle(FSL_OS_MOD_AIOP_RC);
	struct dpci_obj *dpci_tbl = NULL;
	int dev_count  = 0;
	int dpci_count = 0;
	int err        = 0;
	int i          = 0;

	if (dprc == NULL) {
		pr_err("No AIOP root container \n");
		return -ENODEV;
	}

	if ((err = dprc_get_obj_count(dprc, &dev_count)) != 0) {
	    pr_err("Failed to get device count for RC auth_d = %d\n",
	           dprc->auth);
	    return err;
	}

	/* First count how many DPCI objects we have */
	for (i = 0; i < dev_count; i++) {
		dprc_get_obj(dprc, i, &dev_desc);
		if (strcmp(dev_desc.type, "dpci") == 0) {
			dpci_count++;
		}
	}

	if (dpci_count > 0) {
		err = dpci_tbl_create(&dpci_tbl, dpci_count + MC_DPCI_NUM);
		if (err != 0) {
			pr_err("Failed dpci_tbl_create() \n");
			return err;
		}
	}

	err = dpci_tbl_fill(dpci_tbl, dprc, dpci_count, dev_count);
	return err;
}

static void dpci_discovery_free()
{
	void *dpci_tbl = sys_get_unique_handle(FSL_OS_MOD_DPCI_TBL);

	sys_remove_handle(FSL_OS_MOD_DPCI_TBL, 0);

	if (dpci_tbl != NULL)
		fsl_os_xfree(dpci_tbl);
}

int mc_obj_init()
{
	int err = 0;

#ifndef AIOP_STANDALONE
	err |= aiop_container_init();
	err |= dpci_discovery(); /* must be after aiop_container_init */
#endif
	return err;

}

void mc_obj_free()
{
#ifndef AIOP_STANDALONE
	aiop_container_free();
	dpci_discovery_free();
	/* TODO DPCI close ??? */
#endif
}
