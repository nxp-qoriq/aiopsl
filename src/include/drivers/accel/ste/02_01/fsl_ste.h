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

/**************************************************************************//**
@File		fsl_ste.h

@Description	This file contains the AIOP statistics API (02_01).

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

#include "fsl_types.h"

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

/**************************************************************************//**
 @Group		StatsErrorRegAddr Statistics error registers addresses

 @Description  Statistics error registers addresses.

 @{
 *//***************************************************************************/

	/** STE Error Registers base address */
#define STE_BASE_ADDRESS	0x02080000
	/** STECR1 - Statistics Engine Control Register address */
#define STE_STECR1_ADDRESS	(STE_BASE_ADDRESS+0x200)
	/** STESR - Statistics Engine Status Register address */
#define STE_STESR_ADDRESS	(STE_BASE_ADDRESS+0x20C)
	/** STE_ERR_CAPT1R - Statistics Engine first capture register
	* address */
#define STE_ERR_CAPT1_ADDRESS	(STE_BASE_ADDRESS+0x210)
	/** STE_ERR_CAPT2R - Statistics Engine second capture register
	* address */
#define STE_ERR_CAPT2_ADDRESS	(STE_BASE_ADDRESS+0x214)
	/** STE_ERR_CAPT3R - Statistics Engine third capture register
	* address */
#define STE_ERR_CAPT3_ADDRESS	(STE_BASE_ADDRESS+0x218)
	/** STE_ERR_CAPT4R - Statistics Engine fourth capture register
	* address */
#define STE_ERR_CAPT4_ADDRESS	(STE_BASE_ADDRESS+0x21C)

/** @} end of group StatsErrorRegAddr */


/**************************************************************************//**
 @Group		StatsErrorStatusReg Statistics status register bits definitions

 @Description  Statistics status register bits definitions.

 @{
 *//***************************************************************************/

	/** AXI interconnect returned a write error response to DDR memory
	*  access. */
#define STE_ERR_STATUS_WRITE_ACCESS		0x00000002
	/** AXI interconnect returned a read error response to DDR memory
	*  access. */
#define STE_ERR_STATUS_READ_ACCESS		0x00000004
	/** The command Descriptor contains a counter size that crosses
	*  a 16B boundary. */
#define STE_ERR_STATUS_NON_16_BYTE_ALIGN	0x00000008
	/** STE received an unrecognized command code. */
#define STE_ERR_STATUS_UNRECOGNIZED_CMD		0x00000010

	/** STE received a bad AXI Address, length or size attribute. */
#define STE_ERR_STATUS_BAD_AXI			0x00000020

/** @} end of group StatsErrorStatusReg */


/**************************************************************************//**
 @Group		StatsErrorCtrlBits Statistics control register bits definitions

 @Description  Statistics control register bits definitions.

 @{
 *//***************************************************************************/

	/** Clear Errors captured in STE_ERR_CAPT registers. */
#define STE_CLEAR_CAP_ERROR		0x00000001

/** @} end of group StatsErrorCtrlBits */


/**************************************************************************//**
@Group		StatsErrCap1 Statistics capture attributes definitions

@Description  Statistics capture attributes definitions.

@{
*//***************************************************************************/

	/** The counter on which the error occurred was 4 bytes long */
#define STE_ERR_CAP_32_BIT_CNTR_SIZE	0x00000020
	/** The counter on which the error occurred was 8 bytes long */
#define STE_ERR_CAP_64_BIT_CNTR_SIZE	0x00000030
	/** The accumulator on which the error occurred was 4 bytes long */
#define STE_ERR_CAP_32_BIT_ACC_SIZE	0x00000008
	/** The accumulator on which the error occurred was 8 bytes long */
#define STE_ERR_CAP_64_BIT_ACC_SIZE	0x0000000C

	/** The counter saturate mode was enabled when the error
	*  occurred. */
#define STE_ERR_CAP_CNTR_SATURATE	0x00000002
	/** The accumulator saturate mode was enabled when the error
	*  occurred. */
#define STE_ERR_CAP_ACC_SATURATE	0x00000001

	/** A Mask that should be applied on the STE_GET_ERR_CAP_ATTRIBUTES
	* result in order get the task id when the error occurred. 
	* The task id field is valid only on a decode type error.*/
#define STE_ERR_TASK_ID_MASK	0x0FF00000

	/** A Mask that should be applied on the STE_GET_ERR_CAP_ATTRIBUTES
	* result in order to match to the \ref e_ste_err_dcmd value. */
#define STE_ERR_CAP_DCMD_MASK	0x000003C0

	/** A Mask that should be applied on the STE_GET_ERR_CAP_ATTRIBUTES
	* result in order to match to the \ref e_ste_err_dcmd value. */
#define STE_ERR_CAP_EC_MASK	0x000003C0

	/** \enum e_ste_err_dcmd defines the statistics Descriptor Command Code
	*  of the captured error.*/
enum e_ste_err_dcmd {
	STE_ERR_CAP_CMDTYPE_SET = 0x40,
	STE_ERR_CAP_CMDTYPE_SYNC = 0x1C0,
	STE_ERR_CAP_CMDTYPE_ADD = 0x200,
	STE_ERR_CAP_CMDTYPE_SUB = 0x240,
	STE_ERR_CAP_CMDTYPE_INC_ADD = 0x300,
	STE_ERR_CAP_CMDTYPE_INC_SUB = 0x340,
	STE_ERR_CAP_CMDTYPE_DEC_ADD = 0x380,
	STE_ERR_CAP_CMDTYPE_DEC_SUB = 0x3C0
};

	/** \enum e_ste_err_ec defines the type of error (Error Code)
	*  of the captured error.*/
enum e_ste_err_ec {
	/** AXI Command Decode Error, Bad Address/AWSIZ/AWLEN.
	* This is a decode type error. */
	STE_ERR_CAP_AXI_CMD_DECODE = 0x2000000,
	/** Command Descriptor Error, Bad Command Code.
	* This is a decode type error. */
	STE_ERR_CAP_CMD_DESC = 0x40000000,
	/** AXI Outbound Address Error, Bad Alignment.
	* This is a processing type error. */
	STE_ERR_CAP_AXI_BAD_ALIGNMENT = 0x60000000,
	/** AXI Outbound Read Transfer Error.
	* This is a processing type error. */
	STE_ERR_CAP_AXI_READ = 0x80000000,
	/** AXI Outbound Write Transfer Error.
	* This is a processing type error. */
	STE_ERR_CAP_AXI_WRITE = 0xA0000000
};

/** @} end of group StatsErrCap1 */


/**************************************************************************//**
@Group		StatsErrMacroes Statistics error macros

@Description  Statistics error macros.

@{
*//***************************************************************************/

	/** Macro to get the STE status register value. The status register
	* bits definitions can be found at \ref StatsErrorStatusReg */
#define STE_GET_STATUS_REGISTER()			\
	(uint32_t)({register uint32_t __rR = 0;		\
	__rR = *((uint32_t *) STE_STESR_ADDRESS); })

	/** Macro to get the attributes of the STE command that caused the
	* error. The attribute definitions can be found at \ref StatsErrCap1 */
#define STE_GET_ERR_CAP_ATTRIBUTES()			\
	(uint32_t)({register uint32_t __rR = 0;		\
	__rR = *((uint32_t *) STE_ERR_CAPT1_ADDRESS); })

	/** Macro to get the accumulate value of the STE command that caused
	*  the error. */
#define STE_GET_ERR_ACC_VALUE()				\
	(uint32_t)({register uint32_t __rR = 0;		\
	__rR = *((uint32_t *) STE_ERR_CAPT2_ADDRESS); })

	/** Macro to get the counter address high portion of the STE command
	* that caused the error. */
#define STE_GET_ERR_MSB_COUNTER_ADDRESS()		\
	(uint32_t)({register uint32_t __rR = 0;		\
	__rR = *((uint32_t *) STE_ERR_CAPT3_ADDRESS); })

	/** Macro to get the counter address low portion of the STE command
	*  that caused the error. */
#define STE_GET_ERR_LSB_COUNTER_ADDRESS()		\
	(uint32_t)({register uint32_t __rR = 0;		\
	__rR = *((uint32_t *) STE_ERR_CAPT4_ADDRESS); })

	/** Macro to clear the errors captured in STE_ERR_CAPTnR registers. */
#define STE_CLEAR_CAPTURED_ERROR()				\
	({uint32_t STECR = *((uint32_t *) STE_STECR1_ADDRESS);	\
	STECR |= STE_CLEAR_CAP_ERROR;				\
	*((uint32_t *) STE_STECR1_ADDRESS) = STECR; })

/** @} end of group StatsErrMacroes */
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
