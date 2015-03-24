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
@File		ste_inline.h

@Description	This file contains the AIOP SW Statistics inline API 
		implementation.

*//***************************************************************************/

#ifndef __STE_INLINE_H
#define __STE_INLINE_H

#include "general.h"
#include "ste.h"
#include "fsl_ste.h"

inline void ste_set_32bit_counter(uint64_t counter_addr, uint32_t value)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_4B);
#endif
	
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_SET + STE_MODE_32_BIT_CNTR_SIZE;

	__stqw(cmd_type, (uint32_t)(value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);
}

inline void ste_set_64bit_counter(uint64_t counter_addr, uint64_t value)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_8B);
#endif
	
	uint32_t mem_ptr;

	mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR + STE_CTR_8B_SET_CMD_MEM_ADDR;

	__stqw((uint32_t)(value>>32), (uint32_t)(value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);
}

inline void ste_inc_counter(uint64_t counter_addr,
				      uint32_t inc_value, uint32_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	if ((flags & ~STE_MODE_SATURATE) == STE_MODE_32_BIT_CNTR_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_4B);
	if ((flags & ~STE_MODE_SATURATE) == STE_MODE_64_BIT_CNTR_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_8B);
#endif
	
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_ADD + flags;

	__stqw(cmd_type, (uint32_t)(inc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

inline void ste_dec_counter(uint64_t counter_addr,
				      uint32_t dec_value, uint32_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	if ((flags & ~STE_MODE_SATURATE) == STE_MODE_32_BIT_CNTR_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_4B);
	if ((flags & ~STE_MODE_SATURATE) == STE_MODE_64_BIT_CNTR_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_8B);
#endif
	
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_SUB + flags;

	__stqw(cmd_type, (uint32_t)(dec_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

inline void ste_inc_and_acc_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	if ((flags & ~(STE_MODE_COMPOUND_ACC_SATURATE | 
			STE_MODE_COMPOUND_CNTR_SATURATE)) == 
					STE_MODE_COMPOUND_64_BIT_CNTR_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_16B);
	else if ((flags & ~(STE_MODE_COMPOUND_ACC_SATURATE | 
			STE_MODE_COMPOUND_CNTR_SATURATE)) == 
					STE_MODE_COMPOUND_64_BIT_ACC_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_16B);
	else
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_8B);
#endif
	
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;
	
	cmd_type = (uint32_t)STE_CMDTYPE_INC_ADD + flags;

	__stqw(cmd_type, (uint32_t)(acc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

inline void ste_inc_and_sub_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	if ((flags & ~(STE_MODE_COMPOUND_ACC_SATURATE | 
			STE_MODE_COMPOUND_CNTR_SATURATE)) == 
					STE_MODE_COMPOUND_64_BIT_CNTR_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_16B);
	else if ((flags & ~(STE_MODE_COMPOUND_ACC_SATURATE | 
			STE_MODE_COMPOUND_CNTR_SATURATE)) == 
					STE_MODE_COMPOUND_64_BIT_ACC_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_16B);
	else
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_8B);
#endif
	
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_INC_SUB + flags;

	__stqw(cmd_type, (uint32_t)(acc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

inline void ste_dec_and_acc_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	if ((flags & ~(STE_MODE_COMPOUND_ACC_SATURATE | 
			STE_MODE_COMPOUND_CNTR_SATURATE)) == 
					STE_MODE_COMPOUND_64_BIT_CNTR_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_16B);
	else if ((flags & ~(STE_MODE_COMPOUND_ACC_SATURATE | 
			STE_MODE_COMPOUND_CNTR_SATURATE)) == 
					STE_MODE_COMPOUND_64_BIT_ACC_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_16B);
	else
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_8B);
#endif
	
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_DEC_ADD + flags;

	__stqw(cmd_type, (uint32_t)(acc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

inline void ste_dec_and_sub_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags)
{
	
#ifdef CHECK_ALIGNMENT 	
	if ((flags & ~(STE_MODE_COMPOUND_ACC_SATURATE | 
			STE_MODE_COMPOUND_CNTR_SATURATE)) == 
					STE_MODE_COMPOUND_64_BIT_CNTR_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_16B);
	else if ((flags & ~(STE_MODE_COMPOUND_ACC_SATURATE | 
			STE_MODE_COMPOUND_CNTR_SATURATE)) == 
					STE_MODE_COMPOUND_64_BIT_ACC_SIZE)
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_16B);
	else
		DEBUG_ALIGN("ste_inline.c", (uint32_t)counter_addr, ALIGNMENT_8B);
#endif
	
	uint32_t mem_ptr = (uint32_t)STE_CTR_CMD_MEM_ADDR;
	uint32_t cmd_type;

	cmd_type = (uint32_t)STE_CMDTYPE_DEC_SUB + flags;

	__stqw(cmd_type, (uint32_t)(acc_value),
		(uint32_t)(counter_addr>>32), (uint32_t)(counter_addr),
		(uint32_t)0, (uint32_t *)mem_ptr);

}

#endif /* __STE_INLINE_H */
