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
 @File          errors.h

 @Description   Error definitions.
*//***************************************************************************/

#ifndef __ERRORS_H
#define __ERRORS_H

#include <errno.h>

#include "common/types.h"
#include "common/fsl_stdio.h"
#include "common/fsl_stdlib.h"
#include "inc/fsl_core.h"
#include "fsl_soc.h"


//TODO: obsolete - do not use in new code !!!
#define E_INVALID_SELECTION   112    /* TODO: obsolete - do not use in new code*/
#define E_NOT_FOUND           114    /* TODO: obsolete - do not use in new code*/
#define E_FULL                115    /* TODO: obsolete - do not use in new code*/
#define E_EMPTY               116    /* TODO: obsolete - do not use in new code*/
#define E_READ_FAILED         118    /* TODO: obsolete - do not use in new code*/
#define E_INVALID_FRAME       119    /* TODO: obsolete - do not use in new code*/

#define NO_MSG      ("")

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
int DEBUG_DYNAMIC_LEVEL = DEBUG_GLOBAL_LEVEL;
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
int ERROR_DYNAMIC_LEVEL = ERROR_GLOBAL_LEVEL;
#endif /* ERROR_STATIC_LEVEL */
#endif /* !ERROR_DYNAMIC_LEVEL */

#define PRINT_FORMAT        "[CPU %d, %s:%d %s]"
#define PRINT_FMT_PARAMS    core_get_id(), __FILE__, __LINE__, __FUNCTION__


#if (!defined(DEBUG_ERRORS) || (DEBUG_ERRORS == 0))
/* No debug/error/event messages at all */
#define REPORT_ERROR(_level, _err, _vmsg)
#define RETURN_ERROR(_level, _err, _vmsg) \
        return (_err)
#define DEBUG_HALT
#else /* DEBUG_ERRORS > 0 */
#define DEBUG_HALT asm{se_dnh}
/*
 * se_dnh - Debug Notify Halt
 * Acts as 'se_illegal' if EDBCR0[DNH_EN] is set
 */

extern const char *dbg_level_strings[];
extern const char *module_strings[];

char * err_type_strings (int err);

#define REPORT_ERROR(_level, _err, _vmsg) \
    do { \
        if (REPORT_LEVEL_##_level <= ERROR_DYNAMIC_LEVEL) { \
            fsl_os_print("! %s %s error " PRINT_FORMAT ": %s; ", \
                     dbg_level_strings[REPORT_LEVEL_##_level - 1], \
                     module_strings[__ERR_MODULE__ >> 16], \
                     PRINT_FMT_PARAMS, \
                     err_type_strings((int)(_err))); \
            fsl_os_print _vmsg; \
            fsl_os_print("\r\n"); \
        } \
    } while (0)

#define RETURN_ERROR(_level, _err, _vmsg) \
    do { \
        REPORT_ERROR(_level, (_err), _vmsg); \
        return _err; \
    } while (0)
#endif /* (DEBUG_ERRORS > 0) */

/** @} */ /* end of gen_err_g group */
/** @} */ /* end of gen_g group */


#endif /* __ERRORS_H */
