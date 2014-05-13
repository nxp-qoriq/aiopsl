#include "common/fsl_string.h"
#include "inc/fsl_sys.h"

extern int aiop_pton_init(void); extern void aiop_pton_free(void);
extern int aiop_ntop_init(void); extern void aiop_ntop_free(void);

#define APPS                            	\
{                                       	\
	{aiop_ntop_init, aiop_ntop_free},       \
    {aiop_pton_init, aiop_pton_free},       \
	{NULL, NULL} /* never remove! */    	\
}

void build_apps_array(struct sys_module_desc *apps);

void build_apps_array(struct sys_module_desc *apps)
{
	struct sys_module_desc apps_tmp[] = APPS;
	memcpy(apps, apps_tmp, sizeof(apps_tmp));
}
