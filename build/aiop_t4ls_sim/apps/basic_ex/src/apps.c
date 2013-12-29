#include "common/fsl_string.h"
#include "inc/sys.h"

extern int aiop_app_init(void); extern void aiop_app_free(void);

#define APPS                            	\
{                                       	\
	{aiop_app_init, aiop_app_free},		\
	{NULL, NULL} /* never remove! */    	\
}

void build_apps_array(struct sys_module_desc *apps);

void build_apps_array(struct sys_module_desc *apps)
{
	struct sys_module_desc apps_tmp[] = APPS;
	memcpy(apps, apps_tmp, sizeof(apps_tmp));
}
