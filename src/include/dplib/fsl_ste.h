/**************************************************************************//**
@File		fsl_ste.h

@Description	This file contains the AIOP statistics API.

@Cautions	The counters that are updated by the STE must not be updated
		using any other means since the STE is not synchronized with
		the CDMA.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/

#ifndef __FSL_STE_H
#define __FSL_STE_H

#include "common/types.h"

/**************************************************************************//**
 @Group		ACCEL ACCEL (Accelerator APIs)

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

/* @} end of group StatsFlags */

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

/* @} end of group StatsCompoundFlags */

/**************************************************************************//**
@Group		STE_functions Statistics functions

@Description	Statistics functions

@{
*//***************************************************************************/


/**************************************************************************//**
@Function	ste_set_32bit_counter

@Description	This routine sets a given value to a 4 bytes
		statistics counter residing in the external memory.

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	value - The value that should be stored to the counter.

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.

*//****************************************************************************/
void ste_set_32bit_counter(uint64_t counter_addr, uint32_t value);

/**************************************************************************//**
@Function	ste_set_64bit_counter

@Description	This routine sets a given value to a 8 bytes
		statistics counter residing in the external memory.

@Param[in]	counter_addr - The counter external address (DDR, PEB
		and system L3 cache configured as SRAM).
@Param[in]	value - The value that should be stored to the counter.

@Return		None.

@Cautions	This is a fire and forget function. This means that the counter
		update that was issued by this function may be executed after
		the this function returns.

*//****************************************************************************/
void ste_set_64bit_counter(uint64_t counter_addr, uint64_t value);

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
void ste_inc_counter(uint64_t counter_addr,
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
void ste_dec_counter(uint64_t counter_addr,
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
		the this function returns.

*//****************************************************************************/
void ste_inc_and_acc_counters(uint64_t counter_addr,
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
		the this function returns.

*//****************************************************************************/
void ste_inc_and_sub_counters(uint64_t counter_addr,
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
		the this function returns.

*//****************************************************************************/
void ste_dec_and_acc_counters(uint64_t counter_addr,
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
		the this function returns.

*//****************************************************************************/
void ste_dec_and_sub_counters(uint64_t counter_addr,
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


/* @} end of group STE_functions */
/* @} end of group STATS */
/** @} */ /* end of ACCEL */

#endif /* __FSL_STE_H */
