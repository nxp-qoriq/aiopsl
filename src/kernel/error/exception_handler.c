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

#include "general.h"
#include "fsl_fdma.h"
#include "fsl_dbg.h"
#include "fsl_stdlib.h"
#include <string.h>

extern __TASK uint32_t exception_flag;

#pragma stackinfo_ignore on

void exception_handler(char *filename,
		       char *function_name,
		       uint32_t line,
		       char *message) __attribute__ ((noreturn))
{
	
	if (exception_flag > 0){
		exception_flag = 0;
		fdma_terminate_task();
		exit(-1);/* TODO This code is never reached and should be
		        removed once fdma_terminate_task() is declared as noreturn */
	}
	else
		exception_flag = 1;
	
	filename = strrchr(filename, '/') ?
			strrchr(filename, '/') + 1 : filename;

	pr_err("Fatal error encountered in file: %s, line: %d\n", filename, line);
	pr_err("function: %s\n", function_name);
	pr_err("error: %s\n", message);

	exception_flag = 0;
	fdma_terminate_task();
	exit(-1); /* TODO This code is never reached and should be removed once
	fdma_terminate_task() is declared as noreturn*/
}

