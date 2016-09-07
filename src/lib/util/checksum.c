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
@File		checksum.c

@Description	This file contains the AIOP SW general checksum functions.

*//***************************************************************************/
#include "fsl_checksum.h"
#include "general.h"

CHECKSUM_CODE_PLACEMENT uint16_t cksum_ones_complement_sum16(uint16_t arg1, uint16_t arg2)
{
	int32_t tmp = arg1 + arg2;
	return (uint16_t)(tmp + (tmp >> 16));
}

uint16_t cksum_ones_complement_dec16(
		register uint16_t arg1, register uint16_t arg2)
{
	asm{
	   se_not   arg2		/* Get the negative value of arg2 */
	   e_rlwinm arg2, arg2, 0, 16, 31/* Make sure only the lowest 16bit are
	   	   	   	   	  taken */
	   se_add   arg2, arg1		/* One's complement of the new value.*/
	   srawi    arg1, arg2, 16	/* Calculate carry */
	   se_add   arg1, arg2		/* Add carry */
	}
	return arg1;
}

CHECKSUM_CODE_PLACEMENT void cksum_update_uint32(register uint16_t *cs_ptr,
		register uint32_t old_val,
		register uint32_t new_val)
{
	register temp1;
	register temp2;
	asm{
		se_lhz	temp1, 0(cs_ptr)	/* Load CS */
		nor	new_val, new_val, new_val/* One's complement of the new
						value. Pipeline optimization */
		addc	temp1, temp1, old_val	/* Adding old value to CS and
						generating carry */
		adde	temp1, new_val, temp1	/* Adding new value and carry of
						the previous addition to CS */
		addze	temp1, temp1		/* Add carry of the previous
						addition to CS */
		e_rlwinm temp2, temp1, 16, 0, 31/* Replace two half words
						location */
		se_add	temp2, temp1		/* Adding two half words and
						first half word carry */
		se_srwi	temp2, 16		/* Isolate only the high 2B of
						the previous addition */
		se_sth	temp2, 0(cs_ptr)	/* Store CS */
	}
}

CHECKSUM_CODE_PLACEMENT uint16_t cksum_accumulative_update_uint32(register uint16_t cksum,
		register uint32_t old_val,
		register uint32_t new_val)
{
	register temp2;
	asm{
		nor	new_val, new_val, new_val/* One's complement of the new
						value. Pipeline optimization */
		addc	cksum, cksum, old_val	/* Adding old value to CS and
						generating carry */
		adde	cksum, new_val, cksum	/* Adding new value and carry of
						the previous addition to CS */
		addze	cksum, cksum		/* Add carry of the previous
						addition to CS */
		e_rlwinm  temp2, cksum, 16, 0, 31/* Replace two half words
						location */
		se_add	temp2, cksum		/* Adding two half words and
						first half word carry */
		se_srwi	temp2, 16		/* Isolate only the high 2B of
						the previous addition */
	}
	return (uint16_t) temp2;
}


