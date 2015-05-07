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

/**

 @File          fsl_soc_aiop_spec.h

 @Description   LS2085A external definitions and structures.
*//***************************************************************************/
#ifndef __FSL_SOC_AIOP_SPEC_H
#define __FSL_SOC_AIOP_SPEC_H

#include "common/types.h"


/**************************************************************************//**
 @Group         ls2085a_g LS2085A Application Programming Interface

 @Description   LS2085A Chip functions,definitions and enums.

 @{
*//***************************************************************************/

#define CORE_E200
#define CORE_E200_Z490

#define INTG_MAX_NUM_OF_CORES   16
#define INTG_THREADS_PER_CORE   1


/**************************************************************************//**
 @Description   Module types.
*//***************************************************************************/
enum fsl_os_module {
	FSL_OS_MOD_SOC = 0,

	/* FSL_OS_MOD_CMDIF_SRV, */ /**< AIOP server handle */
	FSL_OS_MOD_CMDIF_CL,  /**< AIOP client handle */
	FSL_OS_MOD_SLAB,
	FSL_OS_MOD_UART,
	FSL_OS_MOD_CMGW,
	FSL_OS_MOD_DPRC,
	FSL_OS_MOD_DPNI,
	FSL_OS_MOD_DPIO,
	FSL_OS_MOD_DPSP,
	FSL_OS_MOD_DPSW,

	FSL_OS_MOD_AIOP_TILE,

	FSL_OS_MOD_MC_PORTAL,
	FSL_OS_MOD_AIOP_RC,    /**< AIOP root container from DPL */

	FSL_OS_MOD_LAYOUT, /* TODO - review *//**< layout */

	FSL_OS_MOD_DUMMY_LAST
};

/** @} */ /* end of ls2085a_g group */

/* AIOP Peripherals Offset in AIOP memory map */
#define AIOP_PERIPHERALS_OFF            0X2000000

/* AIOP Profile SRAM offset */
#define AIOP_STORAGE_PROFILE_OFF        0x30000
/* Offsets relative to CCSR base */
#define SOC_PERIPH_OFF_AIOP_WRKS        0x1d000
#define SOC_PERIPH_OFF_AIOP_TILE        0x00080000
#define SOC_PERIPH_OFF_AIOP_CMGW        0x0

#endif /* __FSL_SOC_AIOP_SPEC_H */
