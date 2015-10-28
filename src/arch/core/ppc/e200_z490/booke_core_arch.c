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
