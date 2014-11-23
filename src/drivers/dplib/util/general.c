/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
@File		general.c

@Description	This file contains the AIOP SW task default params.

*//***************************************************************************/
#include "general.h"
#include "fsl_fdma.h"
#include "fsl_cdma.h"

#ifndef STACK_CHECK
#include "fsl_dbg.h" 
#endif

/** Global task params */
__TASK struct aiop_default_task_params default_task_params;

/* TODO - cleanup once the error handling below is moved to verification code.*/
#ifdef AIOP_VERIF
#include "aiop_verification_data.h"
#include "aiop_verification.h"
#include <string.h>
extern __VERIF_TLS uint64_t initial_ext_address;
#endif /*AIOP_VERIF*/

/* TODO - once the ARENA implementation is ready move this (verification)
 * implementation (the chosen implementation between the following 2) to
 * aiop_verification_data.c. keep the declaration in place*/
void handle_fatal_error(char *message)
{
       uint32_t status;
       status = -1 + (uint32_t)message;
       fdma_terminate_task();
}

void exception_handler(char *filename,
		       char *function_name,
		       uint32_t line,
		       char *message) __attribute__ ((noreturn))
{
	uint32_t status;
#ifdef AIOP_VERIF

	struct fatal_error_command fatal_cmd_str;
	struct fatal_error_command *fatal_cmd;

	/* Read command from external buffer in DDR */
	fatal_cmd = &fatal_cmd_str;
	cdma_read((void *)fatal_cmd, initial_ext_address,
			sizeof(struct fatal_error_command));

	filename = strrchr(filename, '/') ?
			strrchr(filename, '/') + 1 : filename;

	strcpy(fatal_cmd->file_name, filename);
	strcpy(fatal_cmd->function_name, function_name);
	strcpy(fatal_cmd->err_msg, message);

	/* write command results back to DDR */
	cdma_write(initial_ext_address,
		   (void *)fatal_cmd,
		   sizeof(struct fatal_error_command));


	fdma_store_and_enqueue_default_frame_fqid(fatal_cmd->fqid,
						  FDMA_EN_TC_TERM_BITS);
#endif /*AIOP_VERIF*/
	status = -1 + (uint32_t)message + (uint32_t)filename + line +
			(uint32_t)function_name;

#ifndef STACK_CHECK
	pr_err("Fatal error encountered!\n");
#endif

	fdma_terminate_task();
	exit(-1); /* TODO This code is never reached and should be removed once
	fdma_terminate_task() is declared as noreturn*/
}
