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
/**************************************************************************//*
 @File          log.c

 @Description   library implementation

 more_detailed_description

 @Cautions      None.
 *//***************************************************************************/

#include "log.h"
#include "fsl_dbg.h"

/*******************************************************************
 	 	 	 	 	 Global Variables
 *******************************************************************/
uint64_t g_log_buf_phys_address;
uint32_t g_log_buf_size;
uint32_t g_log_buf_count;


extern struct aiop_init_info g_init_data;
extern struct icontext icontext_aiop;
/*******************************************************************
 	 	 	 	 	 Static Functions
 *******************************************************************/

int log_init()
{
	char str[] = LOG_INIT_SIGN;
	struct icontext ic;

	g_log_buf_phys_address = g_init_data.sl_info.log_buf_paddr;
	g_log_buf_size = g_init_data.sl_info.log_buf_size;
	g_log_buf_count = sizeof(str) - 1;

	ASSERT_COND_LIGHT(g_log_buf_size > 1 * KILOBYTE);
	icontext_aiop_get(&ic);
	ASSERT_COND_LIGHT(ic.dma_flags);

	icontext_dma_write(&ic, (uint16_t)g_log_buf_count, str, g_log_buf_phys_address);
	return 0;
}
/*
 * @Cautions - Calling this function from not protected by mutex print is dangerous.
 * */
void log_print_to_buffer(char *str, uint16_t str_length)
{
	uint32_t local_counter;
	uint16_t second_write_len;
	/*
	 * Adding END\n delimiter.
	 * The full str buffer size was given with 4 bytes extra for the end delimiter
	 * */

	str[str_length++] = 'E';
	str[str_length++] = 'N';
	str[str_length++] = 'D';
	str[str_length++] = '\n';


	if(str_length > g_log_buf_size) /*in case the length is too big */
	{
		return;
	}
	/*The call to function must be protected*/
	local_counter = g_log_buf_count;
	/*Move the pointer 4 bytes back */
	if(local_counter >= LOG_END_SIGN_LENGTH){
		/*Start to write from the last end of write without END delimiter*/
		local_counter -= LOG_END_SIGN_LENGTH;
	}
	else{/*Write from the end of buffer because the last write was cyclic*/
		/*write from the end of the buffer (END\n delimiter is in the end of buffer)*/
		local_counter = g_log_buf_size + (local_counter - LOG_END_SIGN_LENGTH);
	}


	/*Fast phase*/
	if(local_counter + str_length <= g_log_buf_size )/*last position + string length + END delimiter*/
	{
		/* Enough buffer*/
		g_log_buf_count = local_counter + str_length;

		icontext_dma_write(&icontext_aiop,
		                   str_length - LOG_END_SIGN_LENGTH,
		                   &str[LOG_END_SIGN_LENGTH],
		                   g_log_buf_phys_address + local_counter + LOG_END_SIGN_LENGTH);

		icontext_dma_write(&icontext_aiop,
		                   LOG_END_SIGN_LENGTH,
		                   str,
		                   g_log_buf_phys_address + local_counter);
	}
	else /*cyclic write needed*/
	{
		/*Two writes needed (Not including the overwrite of END delimiter) - calculate the second write*/
		second_write_len = str_length - (uint16_t)(g_log_buf_size - local_counter);
		str_length -= second_write_len; /*str_length is now "first_write" length*/
		if(second_write_len >= LOG_END_SIGN_LENGTH)/*The END\n delimiter will be written in the second write*/
		{
			g_log_buf_count = second_write_len; /*The counter will point to the end of the string from the beginning of buffer*/

			icontext_dma_write(&icontext_aiop,
			                   second_write_len,
			                   &str[str_length],
			                   g_log_buf_phys_address);

			icontext_dma_write(&icontext_aiop,
			                   str_length,
			                   str,
			                   g_log_buf_phys_address + local_counter);
		}
		else /*if(second_write_len < LOG_END_SIGN_LENGTH) must be bigger than 0, otherwise no need for cyclic write*/
		{
			g_log_buf_count = LOG_END_SIGN_LENGTH;
			/*First write END\n delimiter to indicate that cyclic write was made*/
			icontext_dma_write(&icontext_aiop,
			                   LOG_END_SIGN_LENGTH,
			                   &str[str_length + second_write_len - LOG_END_SIGN_LENGTH],
			                   g_log_buf_phys_address);
			str[str_length + second_write_len - 1] = ' ';
			str[str_length + second_write_len - 2] = ' ';
			str[str_length + second_write_len - 3] = ' ';
			str[str_length + second_write_len - 4] = ' ';

			icontext_dma_write(&icontext_aiop,
			                   str_length,
			                   str,
			                   g_log_buf_phys_address + local_counter);
		}
	}
}

