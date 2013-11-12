/**

 @File          fsl_soc_aiop_spec.h

 @Description   LS2100A external definitions and structures.
*//***************************************************************************/
#ifndef __FSL_SOC_AIOP_SPEC_H
#define __FSL_SOC_AIOP_SPEC_H

#include "common/types.h"


/**************************************************************************//**
 @Group         ls2100a_g LS2100A Application Programming Interface

 @Description   LS2100A Chip functions,definitions and enums.

 @{
*//***************************************************************************/

#define CORE_E200

#define INTG_MAX_NUM_OF_CORES   16
#define INTG_THREADS_PER_CORE   1


/**************************************************************************//**
 @Description   Module types.
*//***************************************************************************/
enum fsl_os_module {
    FSL_OS_MOD_SOC = 0,

    FSL_OS_MOD_UART,

    FSL_OS_MOD_DPNI,

    FSL_OS_MOD_CMGW,
    FSL_OS_MOD_MC_PORTAL,

    FSL_OS_MOD_DUMMY_LAST
};

/** @} */ /* end of ls2100a_g group */


#endif /* __FSL_SOC_AIOP_SPEC_H */
