/**************************************************************************//**
@File		fsl_endian.h

@Description	This file contains AIOP and MC endian macros.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __FSL_ENDIAN_H_
#define __FSL_ENDIAN_H_
#include "common/types.h"
#include "common/fsl_core.h"
#include "fsl_swab.h"



/**************************************************************************//**
 @Collection    Little-Endian Conversion Macros

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

#define CPU_TO_LE16(val)        LH_SWAP(0, &val)
#define CPU_TO_LE32(val)        LW_SWAP(0, &val)
#define CPU_TO_LE64(val)        LLLDW_SWAP(0, &val)

#define CPU_TO_BE16(val)	((uint16_t)(val))
#define CPU_TO_BE32(val)	((uint32_t)(val))
#define CPU_TO_BE64(val)	((uint64_t)(val))

#else  /* CORE_IS_LITTLE_ENDIAN */

#define CPU_TO_LE16(val)	((uint16_t)(val))
#define CPU_TO_LE32(val)	((uint32_t)(val))
#define CPU_TO_LE64(val)	((uint64_t)(val))

#define CPU_TO_BE16(val)        LH_SWAP(0, &val)
#define CPU_TO_BE32(val)        SWAP_W(val)
#define CPU_TO_BE64(val)        LLLDW_SWAP(0, &val)

#endif /* CORE_IS_LITTLE_ENDIAN */

#define LE16_TO_CPU(val)        CPU_TO_LE16(val)
#define LE32_TO_CPU(val)        CPU_TO_LE32(val)
#define LE64_TO_CPU(val)        CPU_TO_LE64(val)

#define BE16_TO_CPU(val)        CPU_TO_BE16(val)
#define BE32_TO_CPU(val)        CPU_TO_BE32(val)
#define BE64_TO_CPU(val)        CPU_TO_BE64(val)





#if 0
#ifdef CORE_IS_BIG_ENDIAN

/* write */
#define CPU_TO_LE16(val, addr)  STH_SWAP(val, 0, addr)
#define CPU_TO_LE32(val, addr)  STW_SWAP(val, 0, addr)
#define CPU_TO_LE64(val, addr)  LLSTDW_SWAP(val, 0, addr) /*done*/

#define CPU_TO_BE16(val, addr)	({ *addr = val; })
#define CPU_TO_BE32(val, addr)	({ *addr = val; })
#define CPU_TO_BE64(val, addr)	LLSTDW(val, 0, addr)

/* read */
#define LE16_TO_CPU(addr)        LH_SWAP(0, addr)
#define LE32_TO_CPU(addr)        LW_SWAP(0, addr)
#define LE64_TO_CPU(addr)        LLLDW_SWAP(0, addr) /*?*/

#define BE16_TO_CPU(addr)        ((uint16_t)(*addr))
#define BE32_TO_CPU(addr)        ((uint32_t)(*addr))
#define BE64_TO_CPU(addr)        LLLDW(0, addr)

#else  /* CORE_IS_LITTLE_ENDIAN */
/* write */
#define CPU_TO_LE16(val, addr)	({ *addr = val; })
#define CPU_TO_LE32(val, addr)	({ *addr = val; })
#define CPU_TO_LE64(val, addr)	LLSTDW(val, 0, addr)

#define CPU_TO_BE16(val, addr)  STH_SWAP(val, 0, addr)
#define CPU_TO_BE32(val, addr)  STW_SWAP(val, 0, addr)
#define CPU_TO_BE64(val, addr)  LLSTDW_SWAP(val, 0, addr)

/* read */
#define LE16_TO_CPU(addr)        ((uint16_t)(*addr))
#define LE32_TO_CPU(addr)        ((uint32_t)(*addr))
#define LE64_TO_CPU(addr)        LLLDW(0, addr)

#define BE16_TO_CPU(addr)        LH_SWAP(0, addr)
#define BE32_TO_CPU(addr)        LW_SWAP(0, addr)
#define BE64_TO_CPU(addr)        LLLDW_SWAP(0, addr)

#endif /* CORE_IS_LITTLE_ENDIAN */

#endif

/** @} */ /* end of ENDIAN */

#endif /* __FSL_ENDIAN_H_ */
