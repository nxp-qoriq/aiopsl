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

#include "fsl_types.h"
#include "fsl_gen.h"
#include "kernel/fsl_spinlock.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"
#include "console.h"
#include "fsl_log.h"
#include "sys.h"

/*****************************************************************************/
__COLD_CODE int sys_register_console(fsl_handle_t h_console_dev,
                        int(*f_console_print)(fsl_handle_t h_console_dev,
                        	uint8_t *p_data,
                        	uint32_t size),
                        int(*f_console_get)(fsl_handle_t h_console_dev,
                        	uint8_t *p_data,
                        	uint32_t size))
{
	uint8_t print_to_buffer_state;

	if (sys.console){
		pr_err("system console");
		return -EEXIST;
	}
	/* Must have Print routine (Get routine is not mandatory) */
	ASSERT_COND(f_console_print);
	sys.console = h_console_dev;
	sys.f_console_print = f_console_print;
	sys.f_console_get = f_console_get;
	/* Flush pre-console printouts as necessary */
	if (h_console_dev) {
		print_to_buffer_state = sys.print_to_buffer;
		sys.print_to_buffer = FALSE;
		sys_print(sys.p_pre_console_buf);
		sys.print_to_buffer = print_to_buffer_state;
		sys.p_pre_console_buf = NULL;
		sys.pre_console_buf_pos = 0;
	}
	return 0;
}

/*****************************************************************************/
int sys_unregister_console(void)
{
	if (!sys.console){
		pr_err("console");
		return -ENODEV;
	}
	sys.console = NULL;
	return 0;
}

/*****************************************************************************/
/*pragma used to ignore stack check for the function below*/
#pragma stackinfo_ignore on
static void sys_print_to_pre_console_buf(char *str, uint32_t count)
{
	if (sys.p_pre_console_buf) {
		if (count >= (PRE_CONSOLE_BUF_SIZE - sys.pre_console_buf_pos)) {
			/* Reached buffer end - overwrite from buffer start */
			sys.pre_console_buf_pos = 0;
			sys.pre_console_buf_pos += sprintf(
				sys.p_pre_console_buf, "[TRUNCATED]...\n");
		}
		memcpy(&(sys.p_pre_console_buf[sys.pre_console_buf_pos]),
		       str/*sys.print_buf*/, count);
		sys.pre_console_buf_pos += count;
	}
}

/*****************************************************************************/
void sys_print(char *str)
{
	uint32_t count = (uint32_t)strlen(str);
	if(sys.print_to_buffer){
		/*print to buffer using fdma*/
		/*Don't call this function if you under spinlock*/
		log_print_to_buffer(str, (uint16_t)count);
	}
	/* Print to the registered console, if exists */
#pragma fn_ptr_candidates(console_print_cb)
	if (sys.console){
		sys.f_console_print(sys.console, (uint8_t *)str, count);
	}
	else{
		sys_print_to_pre_console_buf(str, count);
	}
}

