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

/**************************************************************************//**
@File		fsl_endian.h

@Description	This file contains AIOP and MC endian macros.

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

#define CPU_TO_LE16(val)	swap_uint16(val) /** <CPU to Little Endian conversion  */
#define CPU_TO_LE32(val)	swap_uint32(val) /** <CPU to Little Endian conversion  */
#define CPU_TO_LE64(val)	swap_uint64(val) /** <CPU to Little Endian conversion  */

#define CPU_TO_BE16(val)	((uint16_t)(val)) /** <CPU to Big Endian conversion  */
#define CPU_TO_BE32(val)	((uint32_t)(val)) /** <CPU to Big Endian conversion  */
#define CPU_TO_BE64(val)	((uint64_t)(val)) /** <CPU to Big Endian conversion  */

#else  /* CORE_IS_LITTLE_ENDIAN */

#define CPU_TO_LE16(val)	((uint16_t)(val))
#define CPU_TO_LE32(val)	((uint32_t)(val))
#define CPU_TO_LE64(val)	((uint64_t)(val))

#define CPU_TO_BE16(val)        swap_uint16(val)
#define CPU_TO_BE32(val)        swap_uint32(val)
#define CPU_TO_BE64(val)        swap_uint64(val)

#endif /* CORE_IS_LITTLE_ENDIAN */

#define LE16_TO_CPU(val)        CPU_TO_LE16(val) /** <Little Endian to CPU conversion  */
#define LE32_TO_CPU(val)        CPU_TO_LE32(val) /** <Little Endian to CPU conversion  */
#define LE64_TO_CPU(val)        CPU_TO_LE64(val) /** <Little Endian to CPU conversion  */

#define BE16_TO_CPU(val)        CPU_TO_BE16(val) /** <Big Endian to CPU conversion  */
#define BE32_TO_CPU(val)        CPU_TO_BE32(val) /** <Big Endian to CPU conversion  */
#define BE64_TO_CPU(val)        CPU_TO_BE64(val) /** <Big Endian to CPU conversion  */


/** @} *//* end of accessor_g Accessor API group */


#endif /* __FSL_ENDIAN_H */
