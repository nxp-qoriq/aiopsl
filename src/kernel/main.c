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

#if (STACK_OVERFLOW_DETECTION == 1)
    configure_stack_overflow_detection();
#endif

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
    
    if (is_master_core) {
    	err = run_apps();
    	if (err)
    	    return err;
    }
    

    if (is_master_core)
    	fsl_os_print("complete. freeing resources and going out ...\n");
    sys_barrier();

    /* TODO - complete - free everything here!!! */

    /* Free system */
    sys_free();

    return err;
}
