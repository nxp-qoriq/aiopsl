#include "common/types.h"
#include "common/fsl_stdio.h"
#include "kernel/smp.h"


extern int sys_init(void);
extern void sys_free(void);
extern int global_init(void);
extern int global_post_init(void);
extern int run_apps(void);

#ifdef ARENA_LEGACY_CODE
extern int sys_lo_process (void *lo);
#endif

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

#ifdef ARENA_LEGACY_CODE
    if (is_master_core)
    	fsl_os_print("Processing layout\n");
    sys_barrier();

    /* TODO - get the DPL from somewhere .... */
    err = sys_lo_process(NULL);
    err = 0; /* TODO - keep this until we have a DPL */
    if (err)
    	return err;
#endif

    if (is_master_core &&
        ((err = global_post_init()) != 0))
        return err;
    sys_barrier();

    if (is_master_core)
    	fsl_os_print("Running applications\n");
    sys_barrier();
    err = run_apps();
    if (err)
    	return err;

    if (is_master_core)
    	fsl_os_print("complete. freeing resources and going out ...\n");
    sys_barrier();

    /* TODO - complete - free everything here!!! */

    /* Free system */
    sys_free();

    return err;
}
