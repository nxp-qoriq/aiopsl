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
 @File          fsl_types.h

 @Description   TODO
 *//***************************************************************************/
#ifndef __FSL_TYPES_H
#define __FSL_TYPES_H

#include <stdint.h>
#include <stddef.h>

#define _prepacked
#define _packed

/* temporary, due to include issues */
typedef uint32_t uintptr_t;
typedef int32_t intptr_t;


#ifndef NULL
#define NULL ((0L))
#endif /* NULL */

#define STRINGTYPE(a) #a
#define DEFINE_CODE_SECTION(x) section code_type x data_mode=far_abs code_mode=pc_rel
#define DEFINE_DATA_SECTION(x) section data_type x
#define PUSH _Pragma("push")
#define START_CODE_SECTION(s) _Pragma(STRINGTYPE(DEFINE_CODE_SECTION(s)))
#define START_DATA_SECTION(s) _Pragma(STRINGTYPE(DEFINE_DATA_SECTION(s)))
#define START_CODE_IN_SECTION(s) PUSH START_CODE_SECTION(s)
#define START_DATA_IN_SECTION(s) PUSH START_DATA_SECTION(s)
#define POP _Pragma("pop")
/**declare function as entry point*/
#define ENTRY_POINT   __declspec(entry_point)
/** Task global variables area */
#define __TASK __declspec(section ".tdata")

/** Shared-SRAM code location */
#pragma section RX ".stext_vle"
#define __WARM_CODE __declspec(section ".stext_vle")
#define __START_WARM_CODE START_CODE_IN_SECTION(".stext_vle")
#define __END_WARM_CODE POP

/** DDR code location */
#pragma section RX ".dtext_vle"
#define __COLD_CODE __declspec(section ".dtext_vle")
#define __START_COLD_CODE START_CODE_IN_SECTION(".dtext_vle")
#define __END_COLD_CODE POP

/** i-RAM code location */
#pragma section RX ".itext_vle"
#define __HOT_CODE __declspec(section ".itext_vle")
#define __START_HOT_CODE START_CODE_IN_SECTION(".itext_vle")
#define __END_HOT_CODE POP

/** DDR data location */
#define __START_DDR_DATA START_DATA_IN_SECTION(".exception_data")
#define __END_DDR_DATA POP

#endif /* __FSL_TYPES_H */
