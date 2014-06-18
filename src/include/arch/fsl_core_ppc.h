/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_core_ppc.h

 @Description   Core API for PowerPC cores

                These routines must be implemented by each specific PowerPC
                core driver.
*//***************************************************************************/
#ifndef __FSL_CORE_PPC_H
#define __FSL_CORE_PPC_H

#include "soc_db/fsl_soc.h"


#define CORE_IS_BIG_ENDIAN

#if defined(CORE_E200)
#include "arch/cores/fsl_core_booke.h"
#define CORE_CACHELINE_SIZE     32
#elif defined(CORE_E6500)
#include "arch/cores/fsl_core_booke.h"
#define CORE_CACHELINE_SIZE     64
#else
#error "core not defined"
#endif /* defined(CORE_E300) || ... */



#endif /* __FSL_CORE_PPC_H */
