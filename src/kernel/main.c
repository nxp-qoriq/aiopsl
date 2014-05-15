#include "common/types.h"
#include "common/fsl_stdio.h"
#include "kernel/smp.h"

extern int sys_init(void);
extern void sys_free(void);
extern int global_init(void);
extern int tile_init(void);
extern int cluster_init(void);
extern int global_post_init(void);
extern int run_apps(void);
extern void core_ready_for_tasks(void);


#if (STACK_OVERFLOW_DETECTION == 1)
static inline void configure_stack_overflow_detection(void)
{
    /* DBCR2 */
    booke_set_spr_DBCR2(booke_get_spr_DBCR2() | 0x00c00000);

    /* DBCR4 */
    asm {
        mfspr   r6,DBCR4
        ori r6, r6, 0x0080 /* DAC1CFG */
        mtspr   DBCR4,r6
        isync
    }
//  booke_set_spr_DBCR4(booke_get_spr_DBCR4() | 0x00000080);
//  booke_instruction_sync();

    /* DBCR0 */
    booke_set_spr_DBCR0(booke_get_spr_DBCR0() | 0x400f0000);
    booke_instruction_sync();

    /* initiate DAC registers */
    booke_set_spr_DAC1(0x400);
    booke_set_spr_DAC2(0x8000);
}
#endif

/*****************************************************************************/
int main(int argc, char *argv[])
{
    int err = 0;
    int is_master_core;
UNUSED(argc);UNUSED(argv);

    /* Initiate small data area pointers at task initialization */
    asm {
        mtdcr dcr469,r2 // INITR2
        mtdcr dcr470,r13// INITR13
    }

#if (STACK_OVERFLOW_DETECTION == 1)
    configure_stack_overflow_detection();
#endif

    /* Initialize system */
    err = sys_init();
    if (err)
        return err;

    /* Only execute if core is a cluster master */
    if(((core_get_id()) % 4) == 0)
    {
    	err = cluster_init();
    	if(err)
    		return err;
    	sys_barrier();
    }

    is_master_core = sys_is_master_core();
    if(is_master_core)
    {
    	err = tile_init();
    	if(err)
    		return err;
    	sys_barrier();

    	err = global_init();
    	if(err)
    		return err;
    	sys_barrier();
    }

    if(is_master_core)
    {
    	err = global_post_init();
    	if(err)
    		return err;

    	fsl_os_print("Running applications\n");
    	sys_barrier();

    	err = run_apps();
    	if (err)
    	    return err;
    	sys_barrier();
    }

    core_ready_for_tasks();

    if (is_master_core)
    	fsl_os_print("complete. freeing resources and going out ...\n");
    sys_barrier();

    /* TODO - complete - free everything here!!! */

    /* Free system */
    sys_free();

    return err;
}
