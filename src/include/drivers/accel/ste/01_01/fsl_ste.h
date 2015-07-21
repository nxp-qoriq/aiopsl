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
@File		fsl_ste.h

@Description	This file contains the AIOP statistics API (01_01).

@Cautions	The counters that are updated by the STE must not be updated
		or initialized using any other means other than the STE API
		functions since the STE block is not synchronized with the 
		CDMA/FDMA/e200 cores hardware.\n
		All counters memory addresses should be aligned at minimum to
		their size. i.e. 4 byte counter should be 4 byte aligned and 8
		byte counter should be 8 byte aligned.
		
*//***************************************************************************/

#ifndef __FSL_STE_H
#define __FSL_STE_H

#include "types.h"

/**************************************************************************//**
 @Group		ACCEL Accelerators APIs

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		STATS STE (Statistics)

@Description	AIOP Statistics functions macros and definitions

@{
*//***************************************************************************/

/**************************************************************************//**
@Group		StatsFlags Statistics flags

@Description  Statistics flags.

@{
*//***************************************************************************/

	/** Saturation - If overflow occurs, the counter doesn't perform
	     a wrap and saturates to its maximum/minimum value. */
#define STE_MODE_SATURATE		0x08000000
	/** Size - If set, the counter is 4 bytes long */
#define STE_MODE_32_BIT_CNTR_SIZE	0x20000000
	/** Size - If set, the counter is 8 bytes long */
#define STE_MODE_64_BIT_CNTR_SIZE	0x30000000

/** @} end of group StatsFlags */

/**************************************************************************//**
@Group		StatsCompoundFlags Statistics Compound commands flags

@Description  Statistics Compound commands flags.

@{
*//***************************************************************************/

	/** Size - If set, the counter is 4 bytes long */
#define STE_MODE_COMPOUND_32_BIT_CNTR_SIZE	0x20000000
	/** Size - If set, the counter is 8 bytes long */
#define STE_MODE_COMPOUND_64_BIT_CNTR_SIZE	0x30000000

	/** Size - If set, the accumulator is 4 bytes long */
#define STE_MODE_COMPOUND_32_BIT_ACC_SIZE	0x80000000
	/** Size - If set, the accumulator is 8 bytes long */
#define STE_MODE_COMPOUND_64_BIT_ACC_SIZE	0xC0000000

	/** Saturation - If overflow occurs, the counter doesn't
		perform a wrap and saturates to its maximum/minimum value. */
#define STE_MODE_COMPOUND_CNTR_SATURATE		0x04000000
	/** Saturation - If overflow occurs, the accumulator doesn't
		perform a wrap and saturates to its maximum/minimum value. */
#define STE_MODE_COMPOUND_ACC_SATURATE		0x08000000

/** @} end of group StatsCompoundFlags */

#include "ste_inline.h"

/**************************************************************************//**
@Group		STE_functions Statistics functions

@Description	Statistics functions

@{
*//***************************************************************************/


/**************************************************************************//**
@Function	ste_set_32bit_counter

@Description	This routine sets a given value to a 4 bytes
		statistics counter residing in the external memory.
		When initializing an STE counter value this function must be
		used. CDMA HW/FDMA HW/AIOP core stores should not be used to
		initialize counter values that are handled by the STE.

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	value - The value that should be stored to the counter.

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.

*//****************************************************************************/
inline void ste_set_32bit_counter(uint64_t counter_addr, uint32_t value);

/**************************************************************************//**
@Function	ste_set_64bit_counter

@Description	This routine sets a given value to a 8 bytes
		statistics counter residing in the external memory.
		When initializing an STE counter value this function must be
		used. CDMA HW/FDMA HW/AIOP core stores should not be used to
		initialize counter values that are handled by the STE.

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	value - The value that should be stored to the counter.

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.

*//****************************************************************************/
inline void ste_set_64bit_counter(uint64_t counter_addr, uint64_t value);

/**************************************************************************//**
@Function	ste_inc_counter

@Description	This routine increments atomically a counter residing
		in the external memory by a given value.

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	inc_value - The value to be added to the counter.
@Param[in]	flags -  \link StatsFlags Statistics flags
		\endlink.\n

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.

*//****************************************************************************/
inline void ste_inc_counter(uint64_t counter_addr,
				      uint32_t inc_value, uint32_t flags);

/**************************************************************************//**
@Function	ste_dec_counter

@Description	This routine decrements atomically a counter residing
		in the external memory by a given value.

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	dec_value - The value to be subtracted from the counter.
@Param[in]	flags -  \link StatsFlags Statistics flags
		\endlink.\n


@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.

*//****************************************************************************/
inline void ste_dec_counter(uint64_t counter_addr,
				      uint32_t dec_value, uint32_t flags);

/**************************************************************************//**
@Function	ste_inc_and_acc_counters

@Description	This routine updates atomically a counter and an accumulator.\n
		This function performs the next actions: \n
		counter++; \n
		accumulator+= acc_value; \n
		The accumulator memory address should be: \n
		counter_addr + sizeof(counter);

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	acc_value - The value to be added to the accumulator.
@Param[in]	flags - \link StatsCompoundFlags Statistics Compound
		flags \endlink.\n

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.\n
		If the counter or the accumulator size is 8 byte the counter
		memory address should be 16 byte aligned. If both counter and
		accumulator are 4 byte size than the counter memory should be 8
		byte aligned.

*//****************************************************************************/
inline void ste_inc_and_acc_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags);

/**************************************************************************//**
@Function	ste_inc_and_sub_counters

@Description	This routine updates atomically a counter and an accumulator. \n
		This function performs the next actions: \n
		counter++; \n
		accumulator-= acc_value; \n
		The accumulator memory address should be: \n
		counter_addr + sizeof(counter);

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	acc_value - The value to be subtracted from the accumulator.
@Param[in]	flags - \link StatsCompoundFlags Statistics Compound
		flags \endlink.\n

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.\n
		If the counter or the accumulator size is 8 byte the counter
		memory address should be 16 byte aligned. If both counter and
		accumulator are 4 byte size than the counter memory should be 8
		byte aligned.

*//****************************************************************************/
inline void ste_inc_and_sub_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags);

/**************************************************************************//**
@Function	ste_dec_and_acc_counters

@Description	This routine updates atomically a counter and an accumulator. \n
		This function performs the next actions: \n
		counter--; \n
		accumulator+= acc_value; \n
		The accumulator memory address should be: \n
		counter_addr + sizeof(counter);

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	acc_value - The value to be added to the accumulator.
@Param[in]	flags - \link StatsCompoundFlags Statistics Compound
		flags \endlink.\n

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.\n
		If the counter or the accumulator size is 8 byte the counter
		memory address should be 16 byte aligned. If both counter and
		accumulator are 4 byte size than the counter memory should be 8
		byte aligned.

*//****************************************************************************/
inline void ste_dec_and_acc_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags);

/**************************************************************************//**
@Function	ste_dec_and_sub_counters

@Description	This routine updates atomically a counter and an accumulator. \n
		This function performs the next actions: \n
		counter--; \n
		accumulator-= acc_value; \n
		The accumulator memory address should be: \n
		counter_addr + sizeof(counter);

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	acc_value - The value to be subtracted from the accumulator.
@Param[in]	flags - \link StatsCompoundFlags Statistics Compound
		flags \endlink.\n

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.\n
		If the counter or the accumulator size is 8 byte the counter
		memory address should be 16 byte aligned. If both counter and
		accumulator are 4 byte size than the counter memory should be 8
		byte aligned.

*//****************************************************************************/
inline void ste_dec_and_sub_counters(uint64_t counter_addr,
				  uint32_t acc_value, uint32_t flags);

/**************************************************************************//**
@Function	ste_barrier

@Description	This service routine will flush all the counter updates that
		are pending in the statistics engine request queue. \n
		This function will act as a barrier command and return only
		after all the pending update requests have completed.

@Return		None.

*//****************************************************************************/
void ste_barrier();


/** @} end of group STE_functions */
/** @} end of group STATS */
/** @} */ /* end of ACCEL */

#endif /* __FSL_STE_H */
