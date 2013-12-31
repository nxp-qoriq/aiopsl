#include "common/types.h"
#include "common/fsl_cmdif.h"


int cmdif_srv_init(void);
void cmdif_srv_free(void);


int cmdif_register_module (enum fsl_os_module module,
			   struct cmdif_module_ops *ops)
{
UNUSED(module);UNUSED(ops);
	/* TODO - complete here!!! */
	return 0;
}


int cmdif_srv_init(void)
{
	/* TODO - complete here!!! */
	return 0;
}

void cmdif_srv_free(void)
{
	/* TODO - complete here!!! */
}
