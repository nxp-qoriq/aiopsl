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

 @File          fsl_soc.h

 @Description   Definitions for the part (integration) module.
*//***************************************************************************/

#ifndef __FSL_SOC_H
#define __FSL_SOC_H

#include "fsl_types.h"
#include "fsl_gen.h"
#include "fsl_soc_arch.h"

/**************************************************************************//**
 @Group         ls2_g Application Programming Interface

 @Description   Chip functions,definitions and enums.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   Module types.
*//***************************************************************************/
enum fsl_module {
	FSL_MOD_SOC = 0,

	/* FSL_MOD_CMDIF_SRV, */ /**< AIOP server handle */
	FSL_MOD_CMDIF_CL,  /**< AIOP client handle */
	FSL_MOD_SLAB,
	FSL_MOD_UART,
	FSL_MOD_CMGW,
	FSL_MOD_DPRC,
	FSL_MOD_DPNI,
	FSL_MOD_DPIO,
	FSL_MOD_DPSP,
	FSL_MOD_DPSW,

	FSL_MOD_AIOP_TILE,

	FSL_MOD_MC_PORTAL,
	FSL_MOD_AIOP_RC,    /**< AIOP root container from DPL */

	FSL_MOD_LAYOUT, /* TODO - review *//**< layout */

	FSL_MOD_DUMMY_LAST
};

/** @} */ /* end of ls2085a_g group */


/**************************************************************************//**
 @Group         ls2_g Application Programming Interface

 @Description   Chip functions,definitions and enums.

 @{
*//***************************************************************************/

#define FSL_NUM_MODULES  FSL_MOD_DUMMY_LAST

#define SOC_PERIPH_OFF_PORTALS_MC(_prtl) \
    (PERIPH_OFF_MC_PORTALS_AREA + SOC_PERIPH_MC_PORTAL_ALIGNMENT * (_prtl))

/** @} */ /* end of ls2085a_g group */



/*****************************************************************************
 INTEGRATION-SPECIFIC MODULE CODES
******************************************************************************/
#define MODULE_UNKNOWN          0x00000000
#define MODULE_SLAB             0x00010000
#define MODULE_SLOB             0x00020000
#define MODULE_SOC_PLATFORM     0x00030000
#define MODULE_DUART            0x00050000



/**************************************************************************//**
@Description   Part ID and revision number information
*//***************************************************************************/
struct fsl_soc_device_name {
   char        name[10];        /**< Chip name */
   uint8_t     rev_major;       /**< Major chip revision */
   uint8_t     rev_minor;       /**< Minor chip revision */
   int         has_sec;         /**< If the chip is with security supported */
};


#endif /* __FSL_SOC_H */

