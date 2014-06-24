/**************************************************************************//**

 @File          fsl_soc.h

 @Description   Definitions for the part (integration) module.
*//***************************************************************************/

#ifndef __FSL_SOC_H
#define __FSL_SOC_H

#include "common/types.h"
#include "inc/fsl_gen.h"


#if defined(LS2100A)
#include "fsl_soc_spec_ls2100a.h"
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

