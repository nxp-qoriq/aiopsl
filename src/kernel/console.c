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

#include "common/types.h"
#include "inc/fsl_gen.h"
#include "kernel/fsl_spinlock.h"
#include "common/fsl_string.h"
#include "fsl_malloc.h"

#include "inc/console.h"
#include "sys.h"

#ifdef AIOP
#include "fsl_icontext.h"
#define PRINT_TO_BUFFER     1
uint64_t g_print_buffer_phys_address = 0xe6000000;
uint32_t g_print_buffer_size = 0x1000000;
uint32_t g_print_buffer_count = 0;
#endif

#ifdef SIMULATOR
static int system_call(int num, int arg0, int arg1, int arg2, int arg3)
{
       int out;

       __asm__ volatile (
             "mr 0, %5"  "\n"
             "mr 3, %1" "\n"
             "mr 4, %2" "\n"
             "mr 5, %3" "\n"
             "mr 6, %4" "\n"
             "sc" "\n"
             "mr %0, 3" "\n"
             : "=r"(out)
             : "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3), "r"(num)
             : "r0", "r3", "r4", "r5", "r6", "r7"
       );

       return out;
}
#endif /* SIMULATOR */

/*****************************************************************************/
static int sys_debugger_print(fsl_handle_t unused,
                              uint8_t *p_data,
                              uint32_t size)
{
	int ret;
#ifdef AIOP
	lock_spinlock(&(sys.console_lock));
#else
	uint32_t int_flags;
	/* Disable interrupts to work-around CW bug */
	int_flags = spin_lock_irqsave(&(sys.console_lock));
#endif

#ifdef SIMULATOR
	ret = system_call(4, 1, (int)PTR_TO_UINT(p_data), (int)size, 0);
#else
	ret = printf((char *)p_data);
	fflush(stdout);
#endif /* SIMULATOR */

#ifdef AIOP
	unlock_spinlock(&(sys.console_lock));
#else
	spin_unlock_irqrestore(&(sys.console_lock), int_flags);
#endif

	UNUSED(unused);
	UNUSED(size);
	return ret;
}

/*****************************************************************************/
__COLD_CODE int sys_register_console(fsl_handle_t h_console_dev,
                               int(*f_console_print)(fsl_handle_t h_console_dev,
                                                     uint8_t *p_data,
                                                     uint32_t size),
                               int(*f_console_get)(fsl_handle_t h_console_dev,
                                                   uint8_t *p_data,
                                                   uint32_t size))
{
	if (sys.console)
		RETURN_ERROR(MINOR, EEXIST, ("system console"));

	/* Must have Print routine (Get routine is not mandatory) */
	ASSERT_COND(f_console_print);

	sys.console = h_console_dev;

	sys.f_console_print = f_console_print;

	sys.f_console_get = f_console_get;

#ifdef AIOP
	sys.console_lock = 0; /* spinlock init */
#else
    spin_lock_init(&(sys.console_lock));
#endif

	/* Flush pre-console printouts as necessary */
	if (h_console_dev) {
		sys_print(sys.p_pre_console_buf);
		sys.p_pre_console_buf = NULL;
		sys.pre_console_buf_pos = 0;
	}

	return 0;
}

/*****************************************************************************/
int sys_unregister_console(void)
{
	if (!sys.console)
		RETURN_ERROR(MINOR, EAGAIN, ("console"));

	sys.console = NULL;

	return 0;
}

static void sys_print_to_buffer(char *str, uint16_t str_length)
{
	uint16_t temp_count;
	uint32_t local_counter = g_print_buffer_count;
	struct icontext ic;
	icontext_aiop_get(&ic);

	if(str_length > g_print_buffer_size) /*in case the length is to big */
		return;

	/*Fast phase*/
	if(local_counter + str_length <= g_print_buffer_size)
	{
		/* Enough buffer*/
		g_print_buffer_count += str_length;
		/*unlock spinlock*/
		unlock_spinlock(&(sys.console_lock));
		icontext_dma_write(&ic, str_length, str, g_print_buffer_phys_address + local_counter);
	}
	else
	{
		temp_count = str_length - (uint16_t)(g_print_buffer_size - local_counter);
		str_length -= temp_count;
		g_print_buffer_count = str_length; /*The counter will point to the end of the string from the beginning of buffer*/
		unlock_spinlock(&(sys.console_lock));
		icontext_dma_write(&ic, temp_count, str, g_print_buffer_phys_address + local_counter);
		str += temp_count;
		icontext_dma_write(&ic, str_length, str, g_print_buffer_phys_address );
	}
	/*lock spinlock*/
	lock_spinlock(&(sys.console_lock));
}

/*****************************************************************************/
void sys_print(char *str)
{
	uint32_t count;

#ifdef AIOP
	lock_spinlock(&(sys.console_lock));
#else
	uint32_t int_flags;
	int_flags = spin_lock_irqsave(&(sys.console_lock));
#endif
	count = (uint32_t)strlen(str);


#ifdef AIOP
	if(PRINT_TO_BUFFER)
	{
		/*print using fdma - inside spinlock - makes it safe*/
		sys_print_to_buffer(str, (uint16_t)count);
	}
#endif

	/* Print to the registered console, if exists */
	if (sys.console)
		sys.f_console_print(sys.console, (uint8_t *)str, count);
	else {
		if (!sys.p_pre_console_buf) {
				/* Cannot print error message - print called before entering to sys_init */
#ifdef AIOP
				unlock_spinlock(&(sys.console_lock));
#else
				spin_unlock_irqrestore(&(sys.console_lock), int_flags);
#endif
				return;
		}
		else
		{
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

#ifdef AIOP
				unlock_spinlock(&(sys.console_lock));
#else
				spin_unlock_irqrestore(&(sys.console_lock), int_flags);
#endif
}

#ifdef MC
/*****************************************************************************/
char sys_get_char(void)
{
	uint8_t temp_char;
	int read_chars;
	uint32_t int_flags;

	if (sys.console && sys.f_console_get) {
		int_flags = spin_lock_irqsave(&(sys.console_lock));

		read_chars = sys.f_console_get(sys.console, &temp_char, 1);

		spin_unlock_irqrestore(&(sys.console_lock), int_flags);

		if (read_chars == 1)
			return temp_char;
	}

	return ('\0');
}
#endif

/*****************************************************************************/
void sys_register_debugger_console(void)
{
	int err_code = 0;

	UNUSED(err_code);

	/* Register system console */
	err_code = sys_register_console(&sys, sys_debugger_print, NULL);
	ASSERT_COND(err_code == 0);
}

