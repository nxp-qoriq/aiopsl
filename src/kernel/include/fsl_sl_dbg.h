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
 @file          fsl_sl_dbg.h

 @Description   Debug mode definitions for service layer developers
*//***************************************************************************/

#ifndef __FSL_SL_DBG_H
#define __FSL_SL_DBG_H

#include "fsl_dbg.h"
#include "fsl_string.h"
#include "fsl_core.h"
/**************************************************************************//**
 @Group		FSL_DEBUG_GROUP Debug Utilities

 @Description	FSL AIOP debug macros

 @{
 *//***************************************************************************/

/**************************************************************************//**
 @Description	Macro to use debug prints from service layer. Same as pr_debug(),
		pr_info() but only for service layer developers.
 	 	The users will not see those prints. In order to enable them
 	 	recompile the service layer library with SL_DEBUG.

 @Param[in]     _level - printing level (TRACE / MAJOR)
 @Param[in]     ... - string with arguments to print.
*//***************************************************************************/
#ifndef SL_DEBUG
#define SL_DBG(_level, ...) do {} while(0)
#define SL_PRINT(...) do {} while(0)
#else
#define SL_DBG(_level, ...) DBG(_level, __VA_ARGS__)

#define SL_PRINT(...)                                                    \
	do {                                                             \
		fsl_print(__VA_ARGS__);                               \
	} while (0)

#endif /* !defined(SL_DEBUG) */


#ifdef STACK_CHECK
#define NS_DBG(_level, ...) do {} while(0)
#else
#define NS_DBG(_level, ...) DBG(_level, __VA_ARGS__)
#endif /* defined(STACK_CHECK) */

/**************************************************************************//**
 @Function      sl_pr_debug(...)

 @Description	Same as pr_debug() but only for service layer developers.
 	 	The users will not see those prints. In order to enable them
 	 	recompile the service layer library with SL_DEBUG.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define sl_pr_debug(...) SL_DBG(REPORT_LEVEL_TRACE, __VA_ARGS__)

/**************************************************************************//**
 @Function      sl_pr_err(...)

 @Description   Same as pr_err() but only for service layer developers
  	 	The users will not see those prints. In order to enable them
 	 	recompile the service layer library with SL_DEBUG.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define sl_pr_err(...) 	SL_DBG(REPORT_LEVEL_MAJOR, __VA_ARGS__)

/**************************************************************************//**
 @Function      sl_print(...)

 @Description   Same as fsl_print() but only for service layer developers
 	 	The users will not see those prints. In order to enable them
 	 	recompile the service layer library with SL_DEBUG.
 	 	The print recovers Accelerator Hardware Context and restore
		after the print.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define sl_print(...)    SL_PRINT(__VA_ARGS__)

/**************************************************************************//**
 @Function      no_stack_pr_debug(...)

 @Description	Same as pr_debug() but only for service layer developers.
 	 	Stack estimation test with STACK_CHECK defined
 	 	will not see those prints.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define no_stack_pr_debug(...) NS_DBG(REPORT_LEVEL_TRACE, __VA_ARGS__)

/**************************************************************************//**
 @Function      no_stack_pr_err(...)

 @Description   Same as pr_err() but only for service layer developers
 	 	Stack estimation test with STACK_CHECK defined
 	 	will not see those prints.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define no_stack_pr_err(...) NS_DBG(REPORT_LEVEL_MAJOR, __VA_ARGS__)

/** @} */ /* end of Debug Utilities */

#endif /* __FSL_SL_DBG_H */
