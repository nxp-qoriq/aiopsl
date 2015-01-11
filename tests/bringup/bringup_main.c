#include "common/types.h"
#include "common/fsl_stdio.h"
#include "fsl_smp.h"
#include "fsl_malloc.h"
#include "common/fsl_string.h"
#include "kernel/layout.h"
#include "dtc/dpc.h"
#include "fsl_dpmng_mc.h"
#include "fsl_dpaiop_mc.h"
#include "drivers/fsl_aiop.h"
#include "platform.h"
#include "mpu.h"
#include "dtc/dpl.h"

#include "sys.h"
#include "bringup_tests.h"

extern int sys_init(void);
extern void sys_free(void);
extern int global_init(void);
extern int global_free(void);
extern int global_post_init(void);
extern int run_apps(void);

extern int sys_lo_process(void);
extern int aiop_drv_init(void);


static void load_aiop(void)
{
	struct dpaiop *dpaiop;
	struct dpaiop_run_cfg run_cfg = {0};
	struct dpaiop_load_cfg load_cfg = {0};
	int err;

	dpaiop = sys_get_handle(FSL_OS_MOD_DPAIOP, 1, 0);

	if (dpaiop)
	{
		load_cfg.img_iova = (void *)0x1F800000;
		load_cfg.options = 0x8000000000000000 |
			DPAIOP_LOAD_OPT_DEBUG;
		err = dpaiop_load(dpaiop, &load_cfg);

		if (!err) {
			run_cfg.cores_mask = 0xffff;
			run_cfg.options = DPAIOP_RUN_OPT_DEBUG;
			err = dpaiop_run(dpaiop, &run_cfg);
#if 0
			if (!err)
			{
				do {
					dpaiop_get_status(dpaiop, &status);
				} while (status == AIOP_TILE_STATUS_BOOT_ONGOING)

				if (status == AIOP_TILE_STATUS_RUNNING)
				{
					pr_info("=================================================================\n");
					pr_info("AIOP Boot Completed!\n");
					pr_info("=================================================================\n");
				}
				else
				{
					pr_info("=================================================================\n");
					pr_info("AIOP Run Failed!\n");
					pr_info("=================================================================\n");
				}
			}
#endif
			if (err)
			{
				pr_info("=================================================================\n");
				pr_info("AIOP Run Failed!\n");
				pr_info("=================================================================\n");
			}
		}
		else
		{
			pr_info("=================================================================\n");
			pr_info("AIOP Load Failed!\n");
			pr_info("=================================================================\n");
		}
	}
	else
		pr_info("No AIOP in layout\n");

}

/*****************************************************************************/
int main(int argc, char *argv[])

{
	int err = 0;
//	int is_master_core;
//	struct mc_version mc_version;
//	uint32_t uboot_init_word;

	/* so sys_is_master_core() will work */
	extern t_system sys; /* Global System Object */
	uint32_t        core_id = core_get_id();
	sys.is_master_core[core_id]       = (int)(0x1 & (1ULL << core_id));
	
	UNUSED(argc); UNUSED(argv);
	
#if (TEST_MEM_ACCESS == ON)
	err = mem_standalone_init();
	if(err) return err;

//	sys_barrier();
	
	err =  mem_test();
	if(err) return err;
#endif

#if (TEST_CONSOLE_PRINT == ON)
	err = console_print_init();
	if(err) return err;

	err =  console_print_test();
	if(err) return err;
#endif

#if (TEST_HW_SEM == ON)
	err = hw_sem_standalone_init();
	if(err) return err;
	
//	sys_barrier();
	
	err = hw_sem_test();
	if(err) return err;
#endif
	
#if (TEST_SPINLOCK == ON)
	err = spinlock_standalone_init();
	if(err) return err;
	
//	sys_barrier();
	
	err = spinlock_test();
	if(err) return err;
#endif
	
#if (TEST_MPIC == ON)
	err = mpic_standalone_init();
	if(err) return err;
	
//	sys_barrier();
	
	err = mpic_test();
	if(err) return err;
#endif
	
#if (TEST_MPU == ON)
	err = mpu_standalone_init();
	if(err) return err;
	
//	sys_barrier();
	
	err = mpu_test();
	if(err) return err;
#endif
	
	
#if (TEST_AIOP == ON)
	dpmng_set_dpaa_sys_ddr_region();

	/* DPC process */
	dpc_process();

	/* LOG initialization */
	log_init();

	/* Initialize system */
	err = sys_init();
	if (err) {
		dpmng_set_mc_status(MC_SYS_INIT_ERR);
		return err;
	}

	/* decide if waiting for u-boot is needed before loading DPL*/
	uboot_init_word = dpmng_uboot_init_message(); //TODO - check if it needs to be before sys_init?

	is_master_core = sys_is_master_core();

	if (is_master_core)
	{
		mc_get_version(&mc_version);
		pr_info("MC Version:\nFirmware Major: "
			"%d\nFirmware Minor: %d\nFirmware revision: %d\n",mc_version.major,
			mc_version.minor, mc_version.revision);
#ifdef DEBUG
		mpu_print_tables();
#endif
	}
 	if (is_master_core && ((err = global_init()) != 0)) {
		return err;
	}
	sys_barrier();

	if (uboot_init_word == UBOOT_INIT_MAGIC_WORD){
		dpmng_set_mc_status(MC_INIT_DONE);
		pr_info("MC waits for U-boot to finish initialization\n");
		while(dpmng_uboot_init_message() && MC_INIT_DONE) ;
	}

	if (is_master_core ) {
		pr_info("Processing layout\n");
		err = dpl_process();
		if (err)
		{
			dpmng_set_mc_status(MC_LO_PROCESS_ERR);
			return err;
		}
	}
	sys_barrier();

	/* Load image to AIOP core#0;
	 Read out-of-reset address from the provided image and load the image to DP-DDR */
	if (is_master_core)
		load_aiop();

#endif



//	sys_barrier();
//
//	if (is_master_core && ((err = global_post_init()) != 0)) {
//		return err;
//	}
//	sys_barrier();
//
//	if (is_master_core)
//		pr_info("Running applications\n");
//
//	run_apps();
//	sys_barrier();
//
//	if (is_master_core)
//	{
//		pr_info("complete. Freeing resources and going out ...\n");
//		global_free();
//	}
//
//	sys_barrier();
//	/* TODO - complete - free everything here!!! */
//
//	if (is_master_core)
//		pr_info("MC status = 0x%08x.\n", dpmng_get_mc_status());
//
//	/* Free system */
//	sys_free();

	return err;
}

