#include "common/types.h"


#define __wait()	asm ("wait  \n"	)


int aiop_app_init(void);
void aiop_app_free(void);

void aiop_verification(void);


int aiop_app_init(void)
{
    int err = 0;

    /*fsl_os_print("AIOP verification suite\n");*/

	__wait();
	aiop_verification();

    return err;
}

void aiop_app_free(void)
{
    /* TODO - complete!*/
}
