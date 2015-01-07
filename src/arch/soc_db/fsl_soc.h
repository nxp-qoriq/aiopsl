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

#include "common/types.h"
#include "inc/fsl_gen.h"


#if defined(LS2085A)
#include "fsl_soc_spec_ls2085a.h"
#elif defined(LS1080A)
#include "fsl_soc_spec_ls1080a.h"
#else
#error "unable to proceed without chip-definition"
#endif /* chip select */


/**************************************************************************//**
@Description   Part ID and revision number information
*//***************************************************************************/
struct fsl_soc_device_name {
   char        name[10];        /**< Chip name */
   uint8_t     rev_major;       /**< Major chip revision */
   uint8_t     rev_minor;       /**< Minor chip revision */
   int         has_sec;         /**< If the chip is with security supported */
};

/**************************************************************************//*
 @Description   Part data structure - must be contained in any integration
                data structure.
*//***************************************************************************/
#if 0
struct fsl_soc {
    uintptr_t           (* f_get_module_base)(fsl_handle_t soc, enum fsl_os_module module_id);
        /**< Returns the address of the module's memory map base. */
    enum fsl_os_module  (* f_get_module_id_by_base)(fsl_handle_t soc, uintptr_t base_address);
        /**< Returns the module's ID according to its memory map base. */
};
#endif

#endif /* __FSL_SOC_H */

