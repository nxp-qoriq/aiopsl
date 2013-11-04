#include "common/types.h"
#include "common/fsl_stdio.h"
#include "kernel/smp.h"


extern int sys_init(void);
extern void sys_free(void);
extern int global_init(void);
extern int global_post_init(void);
extern int run_apps(void);

extern int sys_lo_process (void *lo);


/*****************************************************************************/
int main(int argc, char *argv[])
{
    int err = 0;
    int is_master_core;

UNUSED(argc);UNUSED(argv);

    /* Initialize system */
    err = sys_init();
    if (err)
        return err;

    is_master_core = sys_is_master_core();
    if (is_master_core &&
        ((err = global_init()) != 0))
        return err;
    sys_barrier();

/*
    if (is_master_core)
    	fsl_os_print("Processing layout\n");
    sys_barrier();
    err = sys_lo_process(NULL);
    if (err)
    	return err;
*/

    if (is_master_core)
    	fsl_os_print("Running applications\n");
    sys_barrier();
    err = run_apps();
    if (err)
    	return err;

    if (is_master_core)
    	fsl_os_print("....\n");
    sys_barrier();
    while (1) ;

    /* TODO - complete - free everything here!!! */

    /* Free system */
    sys_free();

    return err;
}
