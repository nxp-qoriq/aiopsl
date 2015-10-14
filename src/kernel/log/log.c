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
/*This struct holds the log header values in little endian */
uint64_t g_log_buf_phys_address;
uint32_t g_log_buf_size; /* Buffer size is not including the log header*/
uint32_t g_log_buf_start;
uint32_t g_log_last_byte;

extern struct aiop_init_info g_init_data;
extern struct icontext icontext_aiop;
/*******************************************************************
 	 	 	 	 	 Static Functions
 *******************************************************************/

int log_init()
{
	uint8_t magic_word[8] = {
	 (uint8_t)'A', (uint8_t)'I', (uint8_t)'O', (uint8_t)'P',
	 (uint8_t)LOG_VERSION_MAJOR, (uint8_t)LOG_VERSION_MINOR, 0, 0
	};
	struct icontext ic;
	struct log_header log_h = {0};

	g_log_last_byte = sizeof(struct log_header);
	g_log_buf_start = g_log_last_byte;
	if(g_init_data.sl_info.log_buf_size <= sizeof(struct log_header)){
		return -EINVAL;
	}
	/* Buffer length without the log header*/
	g_log_buf_size = g_init_data.sl_info.log_buf_size - sizeof(struct log_header);
	g_log_buf_phys_address = g_init_data.sl_info.log_buf_paddr;

	/* offset 0x00 holds 32-bit little-endian magic word 'AIOP<version>'*/

	ASSERT_COND_LIGHT(g_log_buf_size > 1 * KILOBYTE);
	icontext_aiop_get(&ic);
	ASSERT_COND_LIGHT(ic.dma_flags);

	log_h.buf_length = CPU_TO_LE32(g_log_buf_size);
	log_h.buf_start = CPU_TO_LE32(g_log_buf_start);
	log_h.last_byte = CPU_TO_LE32(g_log_last_byte);
	*((uint32_t *) &log_h.magic_word[0]) = CPU_TO_LE32(*(uint32_t *)(&magic_word[0]));
	*((uint32_t *) &log_h.magic_word[4]) = CPU_TO_LE32(*(uint32_t *)(&magic_word[4]));

	icontext_dma_write(&ic, (uint16_t)sizeof(struct log_header),
	                   &log_h, g_log_buf_phys_address);
	return 0;
}
/*
 * @Cautions - Calling this function from not protected by mutex print is dangerous.
 * */
void log_print_to_buffer(char *str, uint16_t str_length)
{
	uint32_t local_counter;
	uint16_t second_write_len;
	uint32_t log_last_byte;

	if(str_length > g_log_buf_size) /*in case the length is too big */
	{
		return;
	}
	/*The call to function must be protected*/
	/* If the flag for cyclic write is on, don't turn it off */
	local_counter = g_log_last_byte & ~(LOG_HEADER_FLAG_BUFFER_WRAPAROUND);


	/*Fast phase*/
	if(local_counter + str_length <= g_log_buf_size + g_log_buf_start /*last position + header length*/)
	{
		/* Enough buffer*/
		g_log_last_byte += str_length;
		log_last_byte = CPU_TO_LE32(g_log_last_byte);
		icontext_dma_write(&icontext_aiop,
		                   sizeof(log_last_byte),
		                   &(log_last_byte),
		                   g_log_buf_phys_address +
		                   LOG_HEADER_LAST_BYTE_OFFSET);
		icontext_dma_write(&icontext_aiop,
		                   str_length,
		                   str,
		                   g_log_buf_phys_address + local_counter);

	}
	else /*cyclic write needed*/
	{
		/*Two writes needed - calculate the second write*/
		second_write_len = str_length - (uint16_t)(g_log_buf_size + g_log_buf_start - local_counter);
		str_length -= second_write_len; /*str_length is now "first_write" length*/

		/*The counter will point to the end of the string from the beginning of buffer*/
		g_log_last_byte = second_write_len + g_log_buf_start;
		g_log_last_byte |= LOG_HEADER_FLAG_BUFFER_WRAPAROUND;
		log_last_byte = CPU_TO_LE32(g_log_last_byte);
		icontext_dma_write(&icontext_aiop,
		                   sizeof(log_last_byte),
		                   &(log_last_byte),
		                   g_log_buf_phys_address +
		                   LOG_HEADER_LAST_BYTE_OFFSET);

		icontext_dma_write(&icontext_aiop,
		                   str_length,
		                   str,
		                   g_log_buf_phys_address + local_counter);

		icontext_dma_write(&icontext_aiop,
		                   second_write_len,
		                   &str[str_length],
		                   g_log_buf_phys_address + g_log_buf_start);

	}
}

