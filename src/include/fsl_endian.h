/**************************************************************************//**
@File		fsl_endian.h

@Description	This file contains AIOP and MC endian macros.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __FSL_ENDIAN_H_
#define __FSL_ENDIAN_H_


#include "fsl_swab.h"

/**************************************************************************//**
@Group		accessor_g Accessor API

 @Description   Little-Endian Conversion Macros

                These macros convert given parameters to or from Little-Endian
                format. Use these macros when you want to read or write a specific
                Little-Endian value in memory, without a-priori knowing the CPU
                byte order.

                These macros use the byte-swap routines. For conversion of
                constants in initialization structures, you may use the CONST
                versions of these macros (see below), which are using the
                byte-swap macros instead.
 @{
*//***************************************************************************/

#ifdef CORE_IS_BIG_ENDIAN

#define CPU_TO_LE16(val)	swap_uint16(val)
#define CPU_TO_LE32(val)	swap_uint32(val)
#define CPU_TO_LE64(val)	swap_uint64(val)

#define CPU_TO_BE16(val)	((uint16_t)(val))
#define CPU_TO_BE32(val)	((uint32_t)(val))
#define CPU_TO_BE64(val)	((uint64_t)(val))

#else  /* CORE_IS_LITTLE_ENDIAN */

#define CPU_TO_LE16(val)	((uint16_t)(val))
#define CPU_TO_LE32(val)	((uint32_t)(val))
#define CPU_TO_LE64(val)	((uint64_t)(val))

#define CPU_TO_BE16(val)        swap_uint16(val)
#define CPU_TO_BE32(val)        swap_uint32(val)
#define CPU_TO_BE64(val)        swap_uint64(val)

#endif /* CORE_IS_LITTLE_ENDIAN */

#define LE16_TO_CPU(val)        CPU_TO_LE16(val)
#define LE32_TO_CPU(val)        CPU_TO_LE32(val)
#define LE64_TO_CPU(val)        CPU_TO_LE64(val)

#define BE16_TO_CPU(val)        CPU_TO_BE16(val)
#define BE32_TO_CPU(val)        CPU_TO_BE32(val)
#define BE64_TO_CPU(val)        CPU_TO_BE64(val)


/** @} *//* end of accessor_g Accessor API group */


#endif /* __FSL_ENDIAN_H */
