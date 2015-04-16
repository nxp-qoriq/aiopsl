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

#define NO_MSG      ("")

#define PRINT_FORMAT        "[CPU %d, %s:%d %s]"
#define PRINT_FMT_PARAMS    core_get_id(), __FILE__, __LINE__, __FUNCTION__

extern const char *dbg_level_strings[];
extern const char *module_strings[];

/** @} */ /* end of gen_err_g group */
/** @} */ /* end of gen_g group */


#endif /* __ERRORS_H */
