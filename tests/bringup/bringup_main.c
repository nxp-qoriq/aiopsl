/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_types.h"
#include "common/fsl_stdio.h"
#include "fsl_smp.h"
#include "fsl_dbg.h"
#include "cmgw.h"
#include "fsl_io_ccsr.h"
#include "sys.h"
#include "../../../tests/bringup/bringup_tests.h"
#include "../../../tests/bringup/leds.h"

extern t_system sys; /* Global System Object */

//extern int sys_init(void);
//extern void sys_free(void);
//extern int global_early_init(void);
//extern int global_init(void);
//extern int tile_init(void);
//extern int cluster_init(void);
//extern int global_post_init(void);
//extern int apps_early_init(void);
//extern int run_apps(void);
//extern void core_ready_for_tasks(void);
//extern int dpni_drv_enable_all(void);
//#if (STACK_OVERFLOW_DETECTION == 1)
//__COLD_CODE static inline void configure_stack_overflow_detection(void)
//{
//    /* DBCR2 */
//    booke_set_spr_DBCR2(booke_get_spr_DBCR2() | 0x00c00000);
//
//    /* DBCR4 */
//    asm {
//        mfspr   r6,DBCR4
//        ori r6, r6, 0x0080 /* DAC1CFG */
//        mtspr   DBCR4,r6
//        isync
//    }
////  booke_set_spr_DBCR4(booke_get_spr_DBCR4() | 0x00000080);
////  booke_instruction_sync();
//
//    /* DBCR0 */
//    booke_set_spr_DBCR0(booke_get_spr_DBCR0() | 0x400f0000);
//    booke_instruction_sync();
//
//    /* initiate DAC registers */
//    booke_set_spr_DAC1(0x400);
//    booke_set_spr_DAC2(0x8000);
//}
//#endif

static uint32_t _count_cores(uint64_t cores_mask)
{
    uint32_t count;
    for(count = 0; cores_mask > 0; cores_mask >>= 1) {
	if(cores_mask & 1 == 1)
	    count ++;
    }

    return count;
}

static void _fill_system_parameters()
{
	uintptr_t reg_base = (uintptr_t)(SOC_PERIPH_OFF_AIOP_TILE \
		+ SOC_PERIPH_OFF_AIOP_CMGW \
		+ 0x02000000);/* PLTFRM_MEM_RGN_AIOP */
	uint32_t abrr_val = ioread32_ccsr(UINT_TO_PTR(reg_base + 0x90));
	uint32_t core_id =  (get_cpu_id() >> 4);

	sys.is_tile_master[core_id] = (int)(0x1 & (1ULL << core_id));

	if(sys.is_tile_master[core_id]) {
		sys.active_cores_mask  = abrr_val;
		sys.num_of_active_cores = _count_cores(sys.active_cores_mask);
		sys_init_multi_processing();
		/* signal all other cores that global initiation is done */
		sys.boot_sync_flag = SYS_BOOT_SYNC_FLAG_DONE;
	} else {
		while(!sys.boot_sync_flag) {}
	}

/*
	pr_debug("sys_is_master_core() %d \n", sys_is_master_core());
	pr_debug("sys_is_core_active() %d \n", sys_is_core_active(core_id));
	pr_debug("sys_get_num_of_cores() %d \n", sys_get_num_of_cores());
*/
}

/*****************************************************************************/
int main(int argc, char *argv[])
{
    int err = 0;
    uint32_t core_id =  (get_cpu_id() >> 4);
    volatile uint32_t wait = 1; /* Don't change */

    UNUSED(argc); UNUSED(argv);

//    int is_master_core;

    /* Initiate small data area pointers at task initialization */
    asm {
        mtdcr dcr469,r2 // INITR2
        mtdcr dcr470,r13// INITR13
    }
    
	/* so
	 * sys_is_master_core()
	 * sys_is_core_active()
	 * sys_get_num_of_cores()
	 * _sys_barrier() - without prints
	 * will work
	 * Use get_cpu_id() and not core_id_get() as it uses prints */
    booke_generic_irq_init();
    _fill_system_parameters();

#if (TEST_MEM_ACCESS == ON)
	/* memory access test */
	err |= mem_standalone_init();
	err |= mem_test();
#endif /* TEST_MEM_ACCESS */


#if (TEST_CONSOLE_PRINT == ON)
	if(sys.is_tile_master[core_id]){
		err |= console_print_init();
	}
	sys_barrier();
	err |=  console_print_test();
#endif

#if (TEST_SPINLOCKS == ON)
	err |= spinlock_standalone_init();
	err |= spinlock_test();
#endif
	
/* Those 2 tests can't be tested together */
#if (TEST_EXCEPTIONS == ON)
	err |= exceptions_test();
#elif (TEST_STACK_OVF == ON)
	err |= stack_ovf_test();
#endif

#if (TEST_SINGLE_CLUSTER == ON)
	err |= single_cluster_test();
#endif

#if (TEST_MULTI_CLUSTER == ON)
	err |= multi_cluster_test();
#endif

#if (TEST_AIOP_MC_CMD == ON)
	err |= aiop_mc_cmd_test();
#endif

#if (TEST_DPBP == ON) /*DPBP must be off for DPNI test*/
	/* memory access test */
	if(sys.is_tile_master[core_id]){
	err |= dpbp_init();
	err |= dpbp_test();
	}
#endif /* TEST_DPBP */

#if (TEST_DPNI == ON) /*DPNI must be off for DPBP test*/
	/* memory access test */
	if(sys.is_tile_master[core_id]){
	err |= dpni_init();
	err |= dpni_test();
	}
#endif /* TEST_DPNI */

#if (TEST_BUFFER_POOLS == ON)
	/* memory access test */
	if(sys.is_tile_master[core_id]){
	err |= buffer_pool_init();
	err |= buffer_pool_test();
	}
#endif /* TEST_BUFFER_POOLS */

#if (TEST_GPP_DDR == ON)
	if(sys.is_tile_master[core_id]){
		err |= gpp_sys_ddr_init();
		/* change the address if needed
		 * ICID is set inside to 1 or 2 */
		err |= gpp_sys_ddr_test(0x2000000400, 16);
		err |= gpp_sys_ddr_test(0x2000000300, 32);
	}
#endif

//
//#if (STACK_OVERFLOW_DETECTION == 1)
//    configure_stack_overflow_detection();
//#endif
//
//    /* Initialize system */
//    err = sys_init();
//    if (err) {
//    	cmgw_report_boot_failure();
//        return err;
//    }
//    sys_barrier();
//
//    is_master_core = sys_is_master_core();
//
//    if(is_master_core) {
//        if(cmgw_get_ntasks() > 2 /*4-tasks*/) {
//        	pr_warn("More then 4 AIOP tasks/core.\n");
//        }
//    }
//
//    /* Only execute if core is a cluster master */
//    if(sys_is_cluster_master())
//    {
//    	err = cluster_init();
//    	if(err) {
//    		cmgw_report_boot_failure();
//    		return err;
//    	}
//    }
//
//    if(is_master_core)
//    {
//    	err = tile_init();
//    	if(err) {
//    		cmgw_report_boot_failure();
//    		return err;
//    	}
//
//    	err = global_early_init();
//    	if(err) {
//    	    cmgw_report_boot_failure();
//    	    return err;
//    	}
//
//    	err = apps_early_init();
//    	if(err) {
//    	    cmgw_report_boot_failure();
//    	    return err;
//    	}
//
//    	err = global_init();
//    	if(err) {
//    		cmgw_report_boot_failure();
//    		return err;
//    	}
//    }
//
//    if(is_master_core)
//    {
//    	err = global_post_init();
//    	if(err) {
//    		cmgw_report_boot_failure();
//    		return err;
//    	}
//
//    	fsl_print("Running applications\n");
//
//    	err = run_apps();
//    	if (err) {
//    		cmgw_report_boot_failure();
//    	    return err;
//    	}
//    }
//
//    core_ready_for_tasks();
//
//    if (is_master_core)
//    	fsl_print("complete. freeing resources and going out ...\n");
//    sys_barrier();
//
//    /* TODO - complete - free everything here!!! */
//
//    /* Free system */
//    sys_free();
//
//    //TODO should never get here !!
//    cmgw_report_boot_failure();
	if (err) {
#if (TEST_CONSOLE_PRINT == ON)
		fsl_print("Core %d TEST FAILED\n", core_id);
#endif
		do {} while(wait); /* TEST failed */
	} else {
#if (TEST_CONSOLE_PRINT == ON)
		fsl_print("Core %d TEST PASSED\n", core_id);
#endif
		do {} while(wait); /* TEST passed */
	}

    return err;
}
