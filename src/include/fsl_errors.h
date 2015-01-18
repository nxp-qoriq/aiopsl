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
 @File          fsl_errors.h

 @Description   Error definitions.
*//***************************************************************************/

#ifndef __FSL_ERRORS_H
#define __FSL_ERRORS_H

#include <errno.h>
#include "inc/errors.h"


/**************************************************************************//**
 @Group         error_g Error Handling

 @Description   Error return defines.

 @{
*//***************************************************************************/

/*
 * Even though this file includes <errno.h>, developers are guided to use only
 * the following subset, for reasons of consistency and clarity.
 */

#ifndef EPERM
#define EPERM		1	/*!< Permission denied, no privileges */
#endif

#ifndef EIO
#define EIO		5	/*!< Input/output error */
#endif

#ifndef ENOMEM
#define ENOMEM		12	/*!< Out of memory */
#endif

#ifndef EACCES
#define EACCES		13	/*!< Illegal access, or invalid device state */
#endif

#ifndef EFAULT
#define EFAULT		14	/*!< Bad address detected */
#endif

#ifndef EBUSY
#define EBUSY		16	/*!< Device or resource is busy */
#endif

#ifndef EEXIST
#define EEXIST		17	/*!< Resource already exists */
#endif

#ifndef ENODEV
#define ENODEV		19	/*!< No such device, or device not configured */
#endif

#ifndef EINVAL
#define EINVAL		22	/*!< Invalid argument, or conflicting arguments */
#endif

#ifndef ENOSPC
#define ENOSPC		28	/*!< No space left (resource is full) */
#endif

#ifndef ENOTSUP
#define ENOTSUP		95	/*!< Operation not supported */
#endif

#ifndef ETIMEDOUT
#define ETIMEDOUT	110	/*!< Operation timed out */
#endif

#ifndef ENAVAIL
#define ENAVAIL		119	/*!< Resource not available, or not found */
#endif

#undef ENOERR			/* Prevent usage */

/** @} */ /* end of error_g Error Handling group */

#endif /* __FSL_ERRORS_H */
