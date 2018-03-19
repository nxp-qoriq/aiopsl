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
@File		fsl_cli.h

@Description	This file contains macro to receive command line aruments.

*//***************************************************************************/


#ifndef __FSL_CLI_H_
#define __FSL_CLI_H_

#include "fsl_aiop_common.h"

extern struct aiop_init_info g_init_data;
/**************************************************************************//**
@Group		fsl_cli_g FSL Command Line Interface

@Description	Macro to receive the command line arguments passed from MC
 	 	 when launching AIOP.


@Cautions	The macro is available during AIOP boot only.
@{
*//***************************************************************************/
/** Get AIOP command line string and it's size
 * _args - uint8_t * to the command line arguments string.
 * _args_size - uint32_t for string size. */
#define GET_AIOP_CLI_STRING(_args, _args_size)			\
	({_args = (uint8_t *) g_init_data.sl_info.args;		\
	_args_size = (uint32_t) g_init_data.sl_info.args_size;})



/** @} *//* end of fsl_cli_g FSL Command Line Interface group */


#endif /* __FSL_CLI_H */
