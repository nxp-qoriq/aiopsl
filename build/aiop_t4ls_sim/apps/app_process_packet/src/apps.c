#include "common/fsl_string.h"
#include "inc/sys.h"

extern int app_init(void); extern void app_free(void);


#define APPS                            	\
{                                       	\
	{app_init, app_free},	\
	{NULL, NULL} /* never remove! */    	\
}

void build_apps_array(struct sys_module_desc *apps);

void build_apps_array(struct sys_module_desc *apps)
{
	struct sys_module_desc apps_tmp[] = APPS;
	memcpy(apps, apps_tmp, sizeof(apps_tmp));
}
