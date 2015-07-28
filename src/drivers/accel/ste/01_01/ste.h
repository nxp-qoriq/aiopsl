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
@File		ste.h

@Description	This file contains the AIOP statistics API (01_01).

@note		This file is intended for internal use only (it may not be 
		included elsewhere).
*//***************************************************************************/

#ifndef __AIOP_LIB_STATISTICS_H
#define __AIOP_LIB_STATISTICS_H

#define STE_CTR_CMD_MEM_ADDR		0x2010000
#define STE_CTR_8B_SET_CMD_MEM_ADDR	0x40

#define STE_CTR_SET_CMD_TYPE		0x1181
#define STE_CTR_GET_CMD_TYPE		0x0981
#define STE_CTR_SET_WRITE_DATA_ACCESS	0x8000
#define STE_CTR_SET_READ_DATA_ACCESS	0x4000

#define STE_CTR_UPDATE_CTR_8_BYTE	0x10000000
#define STE_CTR_UPDATE_ACC_8_BYTE	0x20000000

#define STE_OSM_REQ_TYPE		0x4


/*! \enum e_ste_cmd_type defines the statistics engine CMDTYPE field.*/
enum e_ste_cmd_type {
	STE_CMDTYPE_SET = 0x1,
	STE_CMDTYPE_SYNC = 0x7,
	STE_CMDTYPE_ADD = 0x8,
	STE_CMDTYPE_SUB = 0x9,
	STE_CMDTYPE_INC_ADD = 0xC,
	STE_CMDTYPE_INC_SUB = 0xD,
	STE_CMDTYPE_DEC_ADD = 0xE,
	STE_CMDTYPE_DEC_SUB = 0xF
};

/**************************************************************************//**
@Function	ste_inc_counter_wrp

@Description	Wrapper of the function ste_inc_counter.
		See description of the function ste_inc_counter.

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
void ste_inc_counter_wrp(uint64_t counter_addr,
			 uint32_t inc_value,
			 uint32_t flags);

/**************************************************************************//**
@Function	ste_dec_counter_wrp

@Description	Wrapper of the function ste_dec_counter.
		See description of the function ste_dec_counter.

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
void ste_dec_counter_wrp(uint64_t counter_addr,
			    uint32_t dec_value,
			    uint32_t flags);

/** @} end of group STE_functions */
/** @} end of group STATS */

#endif /* __AIOP_LIB_STATISTICS_H */
