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
 @File          error.c

 @Description   General errors and events reporting utilities.
*//***************************************************************************/
#if (defined(DEBUG_ERRORS) && (DEBUG_ERRORS > 0))
#include "fsl_errors.h"


const char *dbg_level_strings[] = {
     "CRITICAL"
    ,"MAJOR"
    ,"MINOR"
    ,"WARNING"
    ,"INFO"
    ,"TRACE"
};

char * err_type_strings (int err) {
    switch (err) {
        case (0):                       return "OK";
        case (EIO):                     return "write access failed";
        case (ENODEV):                  return "no device";
        case (EAGAIN):                  return "resource is unavailable";
        case (ENOMEM):                  return "memory allocation failed";
        case (EFAULT):                  return "invalid address";
        case (EBUSY):                   return "resource is busy";
        case (EEXIST):                  return "resource already exists";
        case (ENOSYS):                  return "invalid operation";
        case (EDOM):                    return "invalid value";
        case (ERANGE):                  return "value out of range";
        case (ENOTSUP):                 return "unsupported operation";
        case (ETIMEDOUT):               return "operation timed out";
        case (E_INVALID_SELECTION):     return "invalid selection";
        case (E_NOT_FOUND):             return "resource not found";
        case (E_FULL):                  return "resource is full";
        case (E_EMPTY):                 return "resource is empty";
        case (E_READ_FAILED):           return "read access failed";
        case (E_INVALID_FRAME):         return "invalid frame";
        default:
            break;
    }
    return NULL;
}
#endif /* (defined(DEBUG_ERRORS) && (DEBUG_ERRORS > 0)) */
