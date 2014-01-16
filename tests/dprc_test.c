#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_string.h"
#include "dplib/fsl_dprc.h"
#include "stdlib.h"
#include "kernel/platform.h"

int dprc_test_init(void);
void dprc_test_free(void);

int scan_dprc(void *portal_addr);
int find_children(void *dprc, int dev_count, struct dprc_dev_desc* dev_arr);
void print_dev_desc(struct dprc_dev_desc* dev_arr);


void print_dev_desc(struct dprc_dev_desc* dev_arr)
{
	fsl_os_print(" device %d\n");
	fsl_os_print("***********\n");
	fsl_os_print("vendor - %d\n", dev_arr->vendor);
	if ( dev_arr->type == DP_RES_TYPE_DEF_DPNI)
		fsl_os_print("type - DP_RES_TYPE_DEF_DPNI\n");
	else if (dev_arr->type == DP_RES_TYPE_DEF_DPRC)
		fsl_os_print("type - DP_RES_TYPE_DEF_DPRC\n");
	else if (dev_arr->type == DP_RES_TYPE_DEF_DPIO)
		fsl_os_print("type - DP_RES_TYPE_DEF_DPIO\n");
	fsl_os_print("id - %d\n", dev_arr->id);
	fsl_os_print("region_count - %d\n", dev_arr->region_count);
	fsl_os_print("rev_major - %d\n", dev_arr->rev_major);
	fsl_os_print("rev_minor - %d\n", dev_arr->rev_minor);
	fsl_os_print("irq_count - %d\n\n", dev_arr->irq_count);
}

int dprc_test_init(void)
{
	/* Test without layout !!*/
	
	int portal_id = 0 , err = 0;
	uint64_t child_portal_paddr = 0;
	void *portal_vaddr;
	struct dprc dprc = { 0 };
	struct dprc child_dprc = { 0 };
	struct dprc_res_req assign_res_req;	
	struct dprc_create_attributes create_attributes;
	int child_container_id, container_id;
	uint64_t irq_paddr_out;
	uint32_t tmp = 0;
	uint32_t irq_val_in, irq_val_out;
	uint64_t *irq_paddr_in;
	
	fsl_os_print("AIOP test: DPRC\n");
	
	/* This address may come from Device tree */
	/* Note: portal_addr here is already virtual */
	portal_vaddr = UINT_TO_PTR(sys_get_memory_mapped_module_base(FSL_OS_MOD_MC_PORTAL,
						      (uint32_t)0,
						      E_MAPPED_MEM_TYPE_MC_PORTAL));
	
	dprc.cidesc.regs = portal_vaddr;
	err = dprc_get_container_id(&dprc, &container_id);
	err = dprc_open(&dprc, container_id);
	
	/* Create a new child container to kernel container */
	create_attributes.icid = DPRC_GET_ICID_FROM_POOL;
	create_attributes.options = 3;
	err = dprc_create_container(&dprc, &create_attributes, &child_container_id, &child_portal_paddr);
	
	/* Set irq address and value for dprc interrupts - */ //TODO - add comment
	irq_paddr_in = (uint64_t *)malloc(sizeof(uint64_t));
	irq_val_in = 0x12345678;
	//tmp = PTR_TO_UINT(irq_paddr_in);
	dprc_set_irq(&dprc, 0, PTR_TO_UINT(irq_paddr_in), irq_val_in);
	dprc_get_irq(&dprc, 0, &irq_paddr_out, &irq_val_out);

	/* Self assignment of DPNI 10 to  container */
	assign_res_req.num = 1;
	assign_res_req.options = RES_REQ_OPT_EXPLICIT | RES_REQ_OPT_SHARED | RES_REQ_OPT_PLUGGED;
	assign_res_req.id_base_align = 10;
	assign_res_req.type = DP_RES_TYPE_DEF_DPNI;
	err = dprc_assign(&dprc, 0, &assign_res_req);
	
	/* Assign FQID to child container */
	assign_res_req.num = 100;
	assign_res_req.options = 0;
	assign_res_req.type = DP_RES_TYPE_DEF_FQID;
	err = dprc_assign(&dprc, child_container_id, &assign_res_req);
	
	
	/* Assign explicit BPID to child container */
	assign_res_req.num = 200;
	assign_res_req.options = RES_REQ_OPT_EXPLICIT;
	assign_res_req.id_base_align = 100;
	assign_res_req.type = DP_RES_TYPE_DEF_BPID;
	err = dprc_assign(&dprc, child_container_id, &assign_res_req);
	
	/* Assign explicit BPID to child container */
	assign_res_req.num = 50;
	assign_res_req.options = RES_REQ_OPT_EXPLICIT;
	assign_res_req.id_base_align = 150;
	assign_res_req.type = DP_RES_TYPE_DEF_BPID;
	err = dprc_unassign(&dprc, child_container_id, &assign_res_req);

	/* Assign explicit BPID to child container */
	assign_res_req.num = 3;
	assign_res_req.options = 0;
	assign_res_req.id_base_align = 0;
	assign_res_req.type = DP_RES_TYPE_DEF_BPID;
	err = dprc_unassign(&dprc, child_container_id, &assign_res_req);

	
//////////////////////////////////Child GPP////////////////////////////////////
	/* Phys to virt - should use ioremap() in real system */
	tmp = (uint32_t)((child_portal_paddr - (uint64_t)0x80c000000));
	tmp = tmp / 64;
	tmp = (tmp * 0x10000) + 0x8000000;
	
	child_dprc.cidesc.regs = UINT_TO_PTR(tmp);
	err = dprc_get_container_id(&child_dprc, &child_container_id);
	err = dprc_open(&child_dprc, child_container_id);
	
	/* Set irq address and value for dprc interrupts */
	irq_paddr_in = (uint64_t *)malloc(sizeof(uint64_t));
	irq_val_in = 0x87654321;
	//tmp = PTR_TO_UINT(irq_paddr_in);
	dprc_set_irq(&child_dprc, 0, PTR_TO_UINT(irq_paddr_in), irq_val_in);
	dprc_get_irq(&child_dprc, 0, &irq_paddr_out, &irq_val_out);
	err = dprc_close(&child_dprc);
///////////////////////////////////////////////////////////////////////////////	

	/* Assign DPNI 10 to child container */
	assign_res_req.num = 1;
	assign_res_req.options = RES_REQ_OPT_EXPLICIT | RES_REQ_OPT_PLUGGED;
	assign_res_req.id_base_align = 10;
	assign_res_req.type = DP_RES_TYPE_DEF_DPNI;
	err = dprc_assign(&dprc, child_container_id, &assign_res_req);

	/* Assign DPNI 11 to child container */
	assign_res_req.num = 1;
	assign_res_req.options = RES_REQ_OPT_EXPLICIT | RES_REQ_OPT_PLUGGED;
	assign_res_req.id_base_align = 11;
	assign_res_req.type = DP_RES_TYPE_DEF_DPNI;
	err = dprc_assign(&dprc, child_container_id, &assign_res_req);

	/* Assign DPNI 10 to child container */
	assign_res_req.num = 1;
	assign_res_req.options = RES_REQ_OPT_EXPLICIT;
	assign_res_req.id_base_align = 11;
	assign_res_req.type = DP_RES_TYPE_DEF_DPNI;
	err = dprc_unassign(&dprc, child_container_id, &assign_res_req);

	/* Assign DPNI to child container*/
	assign_res_req.num = 1;
	assign_res_req.options = RES_REQ_OPT_PLUGGED;
	assign_res_req.id_base_align = 0;
	assign_res_req.type = DP_RES_TYPE_DEF_DPNI;
	err = dprc_assign(&dprc, child_container_id, &assign_res_req);
	
	/* Assign DPNI 10 to child container */
	assign_res_req.num = 1;
	assign_res_req.options = 0;
	assign_res_req.type = DP_RES_TYPE_DEF_DPNI;
	err = dprc_unassign(&dprc, child_container_id, &assign_res_req);

	/* Self portal assignment to dprc */
	assign_res_req.num = 1;
	assign_res_req.options = 0;
	assign_res_req.type = DP_RES_TYPE_DEF_MCPID;
	err = dprc_assign(&dprc, 0, &assign_res_req);

	err = dprc_close(&dprc);

	/* Scan container */
	scan_dprc(portal_vaddr);
	
	return err;
}

int scan_dprc(void *portal_vaddr)
{
	int err = 0, dev_count, i, res_count, containr_id;
	struct dprc dprc = { 0 };
	struct dprc child_dprc = { 0 };
	struct dprc_region_desc region_desc;
	struct dprc_dev_desc dev_arr;
	struct dprc_attributes attributes;
	uint64_t child_open_portal_addr = 0;
	struct dprc_res_ids_range_desc range_desc = {0};
	
	/* Get  container id and open it */
	dprc.cidesc.regs = portal_vaddr;
	err |= dprc_get_container_id(&dprc, &containr_id);
	err |= dprc_open(&dprc,containr_id);
	
	/* Gets container's attributes */
	err |= dprc_get_attributes(&dprc, &attributes);
	/* Gets the number of devices (including children containers) of kernel container */
	err |= dprc_get_device_count(&dprc, &dev_count);
	/* Fill dev_arr with all devices descriptors */
    
	/* For each one of the devices and child container */
	for(i = 0; i < dev_count ; i++)
	{
		dprc_get_device(&dprc, i, &dev_arr);
		print_dev_desc(&dev_arr);
		
		err |= dprc_get_dev_region(&dprc,
		                    dev_arr.type,
		                    (uint16_t)dev_arr.id,
		                    0,
		                    &region_desc);
		
		if (dev_arr.type == DP_RES_TYPE_DEF_DPRC)
		{
			child_dprc.cidesc.regs = portal_vaddr;
			err |= dprc_open(&child_dprc, (int)dev_arr.id);
			err |= dprc_get_attributes(&child_dprc, &attributes);
			err |= dprc_get_res_count(&child_dprc, DP_RES_TYPE_DEF_FQID, &res_count);
			err = dprc_get_res_ids(&child_dprc,
			                       DP_RES_TYPE_DEF_FQID,
			                       &range_desc);
			
			memset (&range_desc, 0, sizeof(struct dprc_res_ids_range_desc));
			err |= dprc_get_res_count(&child_dprc, DP_RES_TYPE_DEF_BPID, &res_count);
			do{
				err |= dprc_get_res_ids(&child_dprc,
				                       DP_RES_TYPE_DEF_BPID,
				                       &range_desc);
			} while (range_desc.iteration_status != DPRC_ITER_STATUS_LAST);
				                       

			err |= dprc_close(&child_dprc);
		}
	}
        
	return err;
}

void dprc_test_free(void)
{
    /* TODO - complete!*/
}
