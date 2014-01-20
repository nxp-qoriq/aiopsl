#include "common/fsl_string.h"
#include "inc/sys.h"

extern int aiop_app_init(void); extern void aiop_app_free(void);
extern int dprc_test_init(void); extern void dprc_test_free(void);


#define APPS                            	\
{                                       	\
	{dprc_test_init, dprc_test_free},	\
	{NULL, NULL} /* never remove! */    	\
}

void build_apps_array(struct sys_module_desc *apps);

void build_apps_array(struct sys_module_desc *apps)
{
	struct sys_module_desc apps_tmp[] = APPS;
	memcpy(apps, apps_tmp, sizeof(apps_tmp));
}
