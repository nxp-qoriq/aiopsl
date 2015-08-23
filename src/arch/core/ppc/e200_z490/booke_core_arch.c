/***************************************************************************//*

 @File          booke_core_arch.c

 @Description   Upper Level ionterfaces to the ASM functions
                Specific for E200 z490 core

 @Cautions

Author: Shultz Danny


*//***************************************************************************/
#include "fsl_types.h"
#include "fsl_errors.h"
#include "fsl_core_booke_regs.h"
#include "fsl_core_booke.h"
#include "booke.h"
#include "fsl_dbg.h"

/*****************************************************************************/
uint32_t booke_get_id(void)
{
    uint32_t cpu_id = 0;

    cpu_id = get_cpu_id();
    cpu_id >>= 4;

    if (cpu_id >= INTG_ACTUAL_MAX_NUM_OF_CORES) {
      pr_err("Core ID 0x%x is out of range, max = %d \n",
             cpu_id,
             INTG_ACTUAL_MAX_NUM_OF_CORES);
    }

    return cpu_id;
}

uint32_t booke_get_spr_PVR(void) 
{
  register uint32_t pvr_val;
  
  asm {mfspr pvr_val,PVR}
  
#ifdef ENGR00346193
  {
    uint32_t is_sim_bits = 0x0c000000;
    pvr_val = 0x81c00000 | (is_sim_bits & pvr_val);
  }
#endif

  return pvr_val;
}
