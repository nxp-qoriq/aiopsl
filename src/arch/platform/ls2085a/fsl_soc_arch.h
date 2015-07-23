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

/**************************************************************************//**

 @File          fsl_soc_arch.h

 @Description   Definitions for the part (integration) module.
*//***************************************************************************/

#ifndef __FSL_SOC_ARCH_H
#define __FSL_SOC_ARCH_H

#include "common/types.h"
#include "fsl_gen.h"


/**************************************************************************//**
 @Group         ls2085a_g LS2085A Application Programming Interface

 @Description   LS2085A Chip functions,definitions and enums.

 @{
*//***************************************************************************/

#define CORE_E200
#define CORE_E200_Z490

#define INTG_MAX_NUM_OF_CORES   16
#define INTG_THREADS_PER_CORE   1
#define INTG_MAX_NUM_OF_CLUSTR  4


/** @} */ /* end of ls2085a_g group */

/* AIOP Peripherals Offset in AIOP memory map */
#define AIOP_PERIPHERALS_OFF            0X2000000

/* AIOP Profile SRAM offset */
#define AIOP_STORAGE_PROFILE_OFF        0x30000
/* Offsets relative to CCSR base */
#define SOC_PERIPH_OFF_AIOP_WRKS        0x1d000
#define SOC_PERIPH_OFF_AIOP_TILE        0x00080000
#define SOC_PERIPH_OFF_AIOP_CMGW        0x0
#define SOC_PERIPH_OFF_DCSR             0x0100000

#define ENGR00346193 /* A-008681: AIOP e200 PVR is wrong */

/**************************************************************************//**
 @Group         ls2085a_g LS2085A Application Programming Interface

 @Description   LS2085A Chip functions,definitions and enums.

 @{
*//***************************************************************************/

/* Offsets relative to CCSR base */

#define SOC_PERIPH_OFF_DUART1           0x021c0500
#define SOC_PERIPH_OFF_DUART2           0x021c0600
#define SOC_PERIPH_OFF_DUART3           0x021d0500
#define SOC_PERIPH_OFF_DUART4           0x021d0600
#define SOC_PERIPH_OFF_QBMAN            0x08180000
#define SOC_PERIPH_OFF_EIOP             0x08b90000
#define SOC_PERIPH_OFF_MC               0x08340000
#define SOC_PERIPH_OFF_SEC_GEN          0x08000000
#define SOC_PERIPH_OFF_EIOP_IFPS        0x08800000

#define SHARED_RAM_SIZE (256*KILOBYTE)

/* Offset of MC portals  relative to MC area base */
#define PERIPH_OFF_MC_PORTALS_AREA  0x0000000
#define SOC_PERIPH_MC_PORTAL_ALIGNMENT  0x10000 /* Alignment of a MC portal in SoC */

/** @} */ /* end of ls2085a_g group */


#endif /* __FSL_SOC_H */

