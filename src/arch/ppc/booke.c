/*

 @File          booke.c

 @Description   Upper Level ionterfaces to the ASM functions

 @Cautions

Author: Donio Ron


*//***************************************************************************/
#define __ERR_MODULE__  MODULE_UNKNOWN

#include "common/types.h"
#include "common/errors.h"
#include "ppc/include/fsl_core_booke_regs.h"
#include "arch/cores/fsl_core_booke.h"
#include "booke.h"
#include "dbg.h"

/*----------------------------------*/
/*  Is Instruction Cache Enabled    */
/*----------------------------------*/
int booke_is_icache_enabled(void)
{
    return !!l1icache_is_enabled();
}

#ifndef CORE_E200_Z490
/*----------------------------------*/
/*  Is Data Cache Enabled           */
/*----------------------------------*/
int  booke_is_dcache_enabled(void)
{
    return !!l1dcache_is_enabled();
}

/*----------------------------------*/
/*  Data Cache Enable               */
/*----------------------------------*/
void booke_dcache_enable(void)
{
    l1dcache_invalidate();
    l1dcache_enable();
}
#endif /* CORE_E200_Z490 */

/*----------------------------------*/
/*  Instruction Cache Enable        */
/*----------------------------------*/
void booke_icache_enable(void)
{
    l1icache_invalidate();
    l1icache_enable();
}

#ifndef CORE_E200_Z490
/*----------------------------------*/
/*  Data Cache Disable              */
/*----------------------------------*/
void booke_dcache_disable(void)
{
   uint32_t       int_flags;

   int_flags = core_local_irq_save();
   l1dcache_flush();
   l1dcache_disable();
   core_local_irq_restore(int_flags);
}
#endif /* CORE_E200_Z490 */

/*----------------------------------*/
/*  Instruction Cache Disable       */
/*----------------------------------*/
void booke_icache_disable(void)
{
    uint32_t       int_flags;

    int_flags = core_local_irq_save();
    l1icache_flush();
    l1icache_disable();
    core_local_irq_restore(int_flags);
}

#ifndef CORE_E200_Z490
/*----------------------------------*/
/*  Data Cache Flush                */
/*----------------------------------*/
void booke_dcache_flush(void)
{
    l1dcache_flush();
}
#endif /* CORE_E200_Z490 */

/*----------------------------------*/
/*  Instruction Cache Flush         */
/*----------------------------------*/
void booke_icache_flush(void)
{
    l1icache_flush();
}

#ifndef CORE_E200_Z490
/*----------------------------------*/
/*  Data Cache Set Stash Id         */
/*----------------------------------*/
void booke_dcache_set_stash_id(uint8_t stash_id)
{
    uint32_t    reg = (booke_get_spr_L1CSR2() & not_L1CSR2_STASHID);

    reg |= stash_id;

    booke_set_spr_L1CSR2(reg);
}

/*----------------------------------*/
/*  Data Cache Set Stash Id         */
/*----------------------------------*/
uint8_t booke_dcache_get_stash_id(void)
{
    uint32_t   reg = booke_get_spr_L1CSR2 ();

    reg &= L1CSR2_STASHID;

    return (uint8_t)reg;
}
#endif /* CORE_E200_Z490 */

/*****************************************************************************/
void booke_enable_time_base(void)
{
    uint32_t t_hid0;

    t_hid0 = booke_get_spr_HID0();
    t_hid0 |= HID0_TBEN;
    booke_set_spr_HID0(t_hid0);
}

/*****************************************************************************/
void booke_disable_time_base(void)
{
    uint32_t hid0;

    hid0 = booke_get_spr_HID0();
    hid0 &= ~(HID0_TBEN);
    booke_set_spr_HID0(hid0);
}

/*****************************************************************************/
uint64_t booke_get_time_base_time(void)
{
    uint64_t time;
    uint32_t timel, timeh1, timeh2;

    do
    {
        timeh1 = booke_get_spr_TBU();
        timel = booke_get_spr_TBL();
        timeh2 = booke_get_spr_TBU();
    } while (timeh1 != timeh2); /* Verify that no carry took place between
                                   reads of high and low*/

    time = ((uint64_t)timeh1 << 32) | timel;

    return time;
}

/*****************************************************************************/
uint32_t booke_get_id(void)
{
    uint32_t cpu_id = 0;

    cpu_id = get_cpu_id();
#ifdef CORE_E200_Z490
    cpu_id >>= 4;
#endif

    if (cpu_id >= INTG_MAX_NUM_OF_CORES) {
	    pr_err("Core ID 0x%x is out of range, max = %d \n",
	           cpu_id,
	           INTG_MAX_NUM_OF_CORES);
    }

    return cpu_id;
}
