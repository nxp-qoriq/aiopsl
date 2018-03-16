/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*

 @File          booke.c

 @Description   Upper Level ionterfaces to the ASM functions

 @Cautions

Author: Donio Ron


*//***************************************************************************/
#define __ERR_MODULE__  MODULE_UNKNOWN

#include "fsl_types.h"
#include "fsl_errors.h"
#include "fsl_core_booke_regs.h"
#include "fsl_core_booke.h"
#include "booke.h"
#include "fsl_dbg.h"

/*----------------------------------*/
/*  Is Instruction Cache Enabled    */
/*----------------------------------*/
int booke_is_icache_enabled(void)
{
    return !!l1icache_is_enabled();
}

/*----------------------------------*/
/*  Instruction Cache Enable        */
/*----------------------------------*/
void booke_icache_enable(void)
{
    l1icache_invalidate();
    l1icache_enable();
}

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

/*----------------------------------*/
/*  Instruction Cache Flush         */
/*----------------------------------*/
void booke_icache_flush(void)
{
    l1icache_flush();
}

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

