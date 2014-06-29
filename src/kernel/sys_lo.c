#ifdef ARENA_LEGACY_CODE
#include "common/types.h"
//#include "fsl_errors.h"
#include "kernel/layout.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"

#include "sys.h"
#include "dpl/dtc.h"


//#define __ERR_MODULE__  MODULE_SYS_LO
#define COMPATIBLE	"pname" 	/*TODO - property name - compatible */


struct dpl {
	struct sys_lo_mod_params *module;
	struct dpl *next;
};


static struct dpl *sys_get_dpl(void)
{
	return (struct dpl *)sys_get_handle(FSL_OS_MOD_LAYOUT, 0);
}


struct node *sys_build_dpl (unsigned int *dpl_bin)
{
	/*TODO - review */
	struct dpl *dpl = (struct dpl *)fsl_os_malloc(sizeof(struct dpl));
	struct boot_info *bi;
	bi = dtc_from_blob(dpl_bin);
	UNUSED(bi);
	sys_add_handle(dpl, FSL_OS_MOD_LAYOUT, 1, 0);
	return bi->dt;
}

int sys_lo_register_module (struct sys_lo_mod_params *lo_mod_params)
{
	struct dpl *dpl = sys_get_dpl();
	if (!dpl)
		return 1; /*TODO - error */
	/* check if already registered */
	while( strcmp(*(dpl->module->compatibles),*(lo_mod_params->compatibles)) 	/*TODO - simple condition !*/
		&& dpl->module && dpl->next && dpl->module->compatibles ){
		dpl = dpl->next;
	}
	if (!strcmp(*(dpl->module->compatibles),*(lo_mod_params->compatibles)))
		return 0; /*already registered*/
	else if (!dpl->module) /* register new module */
	{
		dpl->module = (struct sys_lo_mod_params *)
				fsl_os_malloc(sizeof(struct sys_lo_mod_params));
		if (!dpl->module)
			return 1; /* TODO - error */
	}
	else if (!dpl->next)
	{
		dpl->next = (struct dpl *)fsl_os_malloc(sizeof(struct dpl));
		if (!dpl->next)
		        return 1; /* TODO - error RETURN_ERROR(MAJOR, E_NOT_AVAILABLE, ("dpl object"));*/
		dpl = dpl->next;
		dpl->module = (struct sys_lo_mod_params *)
				fsl_os_malloc(sizeof(struct sys_lo_mod_params));
		if (!dpl->module)
			return 1; /* TODO - error */
	}
	/* register module */
	dpl->module->compatibles = (char **)fsl_os_malloc(sizeof(char));
	if (!dpl->module->compatibles)
		return 1; /* TODO - error */
	*(dpl->module->compatibles) = (char *)fsl_os_malloc(sizeof((lo_mod_params->compatibles)));
	if (!(*(dpl->module->compatibles)))
		return 1; /* TODO - error */
	strcpy(*(dpl->module->compatibles), *(lo_mod_params->compatibles));
	dpl->module->f_prob_module = lo_mod_params->f_prob_module;
	dpl->module->f_remove_module = lo_mod_params->f_remove_module;
	dpl->module->num_compats = lo_mod_params->num_compats;
	return 0;
}

int sys_lo_process (void *lo)	/* TODO - lo is the device tree (struct node) or dtb (binary)? assumed device tree */
{
    /* TODO - review */
	struct dpl *dpl = sys_get_dpl();
	struct node *node = (struct node *)lo;
	struct node *child;
	struct property *compatible;
	char *comp_name;
	int err = 0;
	int len;
	if (!dpl)
		return 1; /* TODO - error */
	if (!node) /* stop recursion */
		return 0;
	compatible = get_property(node, COMPATIBLE);
	if (compatible)
	{
		len = compatible->val.len;
		while (len > 0)
		{
			/* search for module in registry */
			comp_name = PTR_MOVE(compatible->val.val, compatible->val.len - len);
			while (strcmp(comp_name, *(dpl->module->compatibles))
				&& dpl->next ) {
				dpl = dpl->next;
			}
			if (!strcmp(comp_name, *(dpl->module->compatibles)))
			{
				dpl->module->f_prob_module(node); /* call driver */
				len -= (strlen(compatible->val.val) + 1);

				err = 0;
			}
			else if (!dpl->next)
				err = 1; /* TODO - error - module not registered */
		}
	}
	else
		err = 0;		 //root - TODO- check if it possible that "compatible" property doesn't exist
	for_each_child(node, child){
		err = sys_lo_process(child);
	}
    return err; /* TODO - ERROR */
}
#endif
