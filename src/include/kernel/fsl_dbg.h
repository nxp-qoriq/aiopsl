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
 @file          fsl_dbg.h

 @Description   Debug mode definitions
*//***************************************************************************/

#ifndef __FSL_DBG_H
#define __FSL_DBG_H

#include "fsl_core.h"
#include "fsl_smp.h"
#include "fsl_stdio.h"
#include "fsl_errors.h"

/* Internal - Only for service layer use */

#ifdef __KLOCWORK__
#undef DEBUG_ERRORS
#endif

#ifndef DEBUG_GLOBAL_LEVEL
#define DEBUG_GLOBAL_LEVEL  REPORT_LEVEL_WARNING
#endif /* DEBUG_GLOBAL_LEVEL */

#ifndef ERROR_GLOBAL_LEVEL
#define ERROR_GLOBAL_LEVEL  DEBUG_GLOBAL_LEVEL
#endif /* ERROR_GLOBAL_LEVEL */


#ifndef DEBUG_DYNAMIC_LEVEL
#define DEBUG_USING_STATIC_LEVEL

#ifdef DEBUG_STATIC_LEVEL
#define DEBUG_DYNAMIC_LEVEL DEBUG_STATIC_LEVEL
#else
#define DEBUG_DYNAMIC_LEVEL DEBUG_GLOBAL_LEVEL
#endif /* DEBUG_STATIC_LEVEL */

#else /* DEBUG_DYNAMIC_LEVEL */
#ifdef DEBUG_STATIC_LEVEL
#error "please use either DEBUG_STATIC_LEVEL or DEBUG_DYNAMIC_LEVEL (not both)"
#else
#define DEBUG_DYNAMIC_LEVEL DEBUG_GLOBAL_LEVEL
#endif /* DEBUG_STATIC_LEVEL */
#endif /* !DEBUG_DYNAMIC_LEVEL */


#ifndef ERROR_DYNAMIC_LEVEL

#ifdef ERROR_STATIC_LEVEL
#define ERROR_DYNAMIC_LEVEL ERROR_STATIC_LEVEL
#else
#define ERROR_DYNAMIC_LEVEL ERROR_GLOBAL_LEVEL
#endif /* ERROR_STATIC_LEVEL */

#else /* ERROR_DYNAMIC_LEVEL */
#ifdef ERROR_STATIC_LEVEL
#error "please use either ERROR_STATIC_LEVEL or ERROR_DYNAMIC_LEVEL (not both)"
#else
#define ERROR_DYNAMIC_LEVEL ERROR_GLOBAL_LEVEL
#endif /* ERROR_STATIC_LEVEL */
#endif /* !ERROR_DYNAMIC_LEVEL */
/**************************************************************************//**
 @Description   Declaration of Halting the core in debug mode.

                se_dnh - Debug Notify Halt
                Acts as 'se_illegal' if EDBCR0[DNH_EN] is not set.
 *//***************************************************************************/
#if (!defined(DEBUG_ERRORS) || (DEBUG_ERRORS == 0))
#define DEBUG_HALT
#else
#ifdef __KLOCWORK__
#define DEBUG_HALT do { } while (1)
#else
#define DEBUG_HALT asm { se_dnh }
#endif /* __KLOCWORK__ */
#endif /* DEBUG_ERRORS */


#if (!defined(DEBUG_ERRORS) || (DEBUG_ERRORS == 0))
/* No debug/error/event messages at all */
#define DBG(_level, ...)

#else /* DEBUG_ERRORS > 0 */

#if ((defined(DEBUG_USING_STATIC_LEVEL)) && (DEBUG_DYNAMIC_LEVEL < REPORT_LEVEL_WARNING))
/* No need for DBG macro - debug level is higher anyway */
#define DBG(_level, ...)
#else
void enable_print_protection();
void disable_print_protection();
/*call to print but without mutex*/
extern const char *dbg_level_strings[];
void dbg_print(char *format, ...);
#define DBG(_level, ...)                                                \
		do {                                                            \
			if (_level <= DEBUG_DYNAMIC_LEVEL) {                    \
				enable_print_protection();                      \
				dbg_print("> %s " PRINT_FORMAT ": ",            \
						dbg_level_strings[_level - 1],        \
						PRINT_FMT_PARAMS);                    \
						dbg_print(__VA_ARGS__);                         \
						disable_print_protection();                     \
			}                                                       \
		} while (0)
#endif /* (defined(DEBUG_USING_STATIC_LEVEL) && (DEBUG_DYNAMIC_LEVEL < WARNING)) */
#endif /* (!defined(DEBUG_ERRORS) || (DEBUG_ERRORS == 0)) */

/* End of internal - Only for service layer use */
/**************************************************************************//**
 @Group			FSL_DEBUG_GROUP Debug Utilities

 @Description	FSL AIOP debug macros

 @{
*//***************************************************************************/
#ifdef __KLOCWORK__
#define pr_crit(...)
#define pr_err(...)
#define pr_warn(...)
#define pr_debug(...)
#define pr_info(...)
#else

/**************************************************************************//**
 @Collection    Debug Levels for Errors and Events

                The level description refers to errors only.
                For events, classification is done by the user.
 @{
*//***************************************************************************/
#define REPORT_LEVEL_CRITICAL   1       /**< Crasher: Incorrect flow, NULL pointers/handles. */
#define REPORT_LEVEL_MAJOR      2       /**< Cannot proceed: Invalid operation, parameters or
                                             configuration. */
#define REPORT_LEVEL_MINOR      3       /**< Recoverable problem: a repeating call with the same
                                             parameters may be successful. */
#define REPORT_LEVEL_WARNING    4       /**< Something is not exactly right, yet it is not an error. */
#define REPORT_LEVEL_INFO       5       /**< Messages which may be of interest to user/programmer. */
#define REPORT_LEVEL_TRACE      6       /**< Program flow messages. */

#define EVENT_DISABLED          0xff    /**< Disabled event (not reported at all) */

/* @} */

#define PRINT_FORMAT        "[CPU %d, %s:%d %s]"
#define PRINT_FMT_PARAMS    core_get_id(), __FILE__, __LINE__, __FUNCTION__

/**************************************************************************//**
 @Function      ASSERT_COND

 @Description   Assertion macro.

 @Param[in]     _cond - The condition being checked, in positive form;
                        Failure of the condition triggers the assert.
*//***************************************************************************/
#ifdef DISABLE_ASSERTIONS
#define ASSERT_COND(_cond)
#else
#define ASSERT_COND(_cond) \
    do { \
        if (!(_cond)) { \
            fsl_print("*** ASSERT_COND failed " PRINT_FORMAT "\r\n", \
                    PRINT_FMT_PARAMS); \
            DEBUG_HALT; \
        } \
    } while (0)
#endif /* DISABLE_ASSERTIONS */

/**************************************************************************//**
 @Function      ASSERT_COND_LIGHT

 @Description   Assertion macro, without printing an error message.

 @Param[in]     _cond - The condition being checked, in positive form;
                        Failure of the condition triggers the assert.
*//***************************************************************************/
#ifdef DISABLE_ASSERTIONS
#define ASSERT_COND_LIGHT(_cond)
#else
#define ASSERT_COND_LIGHT(_cond) \
    do { \
        if (!(_cond)) { \
            DEBUG_HALT; \
        } \
    } while (0)
#endif /* DISABLE_ASSERTIONS */

/**************************************************************************//**
 @Function      pr_debug(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print debug info.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define pr_debug(...) 	DBG(REPORT_LEVEL_TRACE, __VA_ARGS__)

/**************************************************************************//**
 @Function      pr_info(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print info messages.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define pr_info(...) 	DBG(REPORT_LEVEL_INFO, __VA_ARGS__)

/**************************************************************************//**
 @Function      pr_warn(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print warnings info.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define pr_warn(...) 	DBG(REPORT_LEVEL_WARNING, __VA_ARGS__)

/**************************************************************************//**
 @Function      pr_err(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print errors info.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define pr_err(...) 	DBG(REPORT_LEVEL_MAJOR, __VA_ARGS__)

/**************************************************************************//**
 @Function      pr_crit(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print critical debug info.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     ... string with arguments to print.
*//***************************************************************************/
#define pr_crit(...)	DBG(REPORT_LEVEL_CRITICAL, __VA_ARGS__)
#endif
/**************************************************************************//**
 @Function      dbg_get_core_id

 @Description   Returns the core ID in the system.

 @Return        Core ID.
*//***************************************************************************/
#define dbg_get_core_id()    core_get_id()

/**************************************************************************//**
 @Function      dbg_get_num_of_cores

 @Description   Returns the number of active cores in the system.

 @Return        number of active cores.
*//***************************************************************************/
#define dbg_get_num_of_cores()    sys_get_num_of_cores()

/**************************************************************************//**
 @Function      dbg_get_max_num_of_cores

 @Description   Returns the number of existing cores in the system.

 @Return        number of existing cores.
*//***************************************************************************/
#define dbg_get_max_num_of_cores()    sys_get_max_num_of_cores()

/** @} */ /* end of Debug Utilities */

#endif /* __FSL_DBG_H */
