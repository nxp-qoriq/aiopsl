/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
@File		fsl_sparser_drv.h

@Description	Soft Parser Driver API
*//***************************************************************************/
#ifndef __FSL_SPARSER_DRV_H
#define __FSL_SPARSER_DRV_H

/**************************************************************************//**
@Group		sparser_drv_g SPARSER DRV

@Description	Contains APIs for :
		- Soft Parser loading
		- Get/Set Parse Cycle Limit register value
		- Dump AIOP Parser registers (debug)
		- Dump AIOP Parser instructions memory (debug)
@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		SPARSER_MEM_LIMITS AIOP/WRIOP Parser instructions memory

 @Description	Define the range of values for the Parser program counter.

 @{
*//***************************************************************************/
/** Minimum value of program counter */
#define PARSER_MIN_PC			0x20
/** Maximum value of program counter */
#define PARSER_MAX_PC			0x7FD
/** @} end of group SPARSER_MEM_LIMITS */

/**************************************************************************//**
 @Group		SPARSER_PCLIM_LIMITS Parsing Cycle Limits register

 @Description	Define the range of values for the PCLIM register.

 @{
*//***************************************************************************/
/** Maximum value of PCLIM */
#define PARSER_CYCLE_LIMIT_MAX		0x1FFF
/** Minimum value of PCLIM (Disable PCLIM verification) */
#define PARSER_CYCLE_LIMIT_DISABLE	0x0000
/** @} end of group SPARSER_PCLIM_LIMITS */

/**************************************************************************//**
@Description	 enum sparser_type - Type of a Soft Parser

*//***************************************************************************/
enum sparser_type {
	/* MC managed/loaded SP, in the WRIOP Parser, intended to parse the
	 * packets going to GPP */
	SPARSER_MC_WRIOP = 0,
	/* MC managed/loaded SP, in the WRIOP Parser, intended to parse the
	 * packets going to AIOP */
	SPARSER_MC_WRIOP_AIOP,
	/* MC loaded SP (managed by AIOP), in the AIOP Parser, intended to
	 * parse the packets from inside the AIOP. */
	SPARSER_MC_AIOP,
	/* AIOP managed/loaded SP, in the AIOP Parser, intended to parse the
	 * packets from inside the AIOP. */
	SPARSER_AIOP_AIOP
};

/**************************************************************************//**
@Description	Structure representing the information needed load a Soft
		Parser or to get information about a loaded Soft Parser.

*//***************************************************************************/
struct sparser_info {
	/* SP loading address (starting PC) */
	uint16_t		pc;
	/* Number of bytes in the SP byte-code (must be a 4 multiple) */
	uint16_t		size;
	/* Pointer to the array containing the SP byte-code (must be aligned on
	 *  a 4 bytes boundary) */
	uint8_t			*byte_code;
	/* Offset of the SP parameters (if any) in the Parameters Array
	 * (a 64 bytes array) */
	uint8_t			param_off;
	/* Size of parameters (if any) in bytes */
	uint8_t			param_size;
	/* Parse Profile ID */
	uint8_t			prpid;
	/* Type of Soft Parser */
	enum sparser_type	type;
};

/**************************************************************************//**
@Function	sparser_drv_load_parser

@Description	Load a Soft Parser into the internal instructions memory of the
		AIOP/WRIOP Parser. Driver checks the overlapping conditions on
		the SP code and on SP parameters(if any).

@Param[in]	sp : Soft Parser information.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_drv_load_parser(struct sparser_info *sp);

/**************************************************************************//**
@Function	sparser_drv_get_pclim

@Description	Get the parsing cycle limit value.

@Return		Current parsing cycle limit value.

*//***************************************************************************/
uint32_t sparser_drv_get_pclim(void);

/**************************************************************************//**
@Function	sparser_drv_set_pclim

@Description	Set PCLIM register value.

@Param[in]	limit : Limit value. In order to disable the parsing cycle limit
		verification, set it to PARSER_CYCLE_LIMIT_DISABLE. Maximum
		value of the limit is : PARSER_CYCLE_LIMIT_MAX

@Return		None

*//***************************************************************************/
void sparser_drv_set_pclim(uint32_t limit);

#ifdef SL_DEBUG
/**************************************************************************//**
@Function	sparser_drv_regs_dump

@Description	Dumps AIOP Parser registers.
		Note : If application calls this function, application and
		AIOP_SL must be build with the SL_DEBUG macro defined.

@Return		None

*//***************************************************************************/
void sparser_drv_regs_dump(void);

/**************************************************************************//**
@Function	sparser_drv_memory_dump

@Description	Dumps AIOP Parser internal instructions memory. The "from PC"
		address must be less than the "to PC" address.
		Note : If application calls this function, application and
		AIOP_SL must be build with the SL_DEBUG macro defined.

@Param[in]	from_pc : Start PC address. Should be greater than or equal to
		PARSER_MIN_PC.

@Param[in]	to_pc : End PC address. Should be less than or equal to
		PARSER_MAX_PC + 3.

@Return		None

*//***************************************************************************/
void sparser_drv_memory_dump(uint16_t from_pc, uint16_t to_pc);
#endif	/* SL_DEBUG */

/** @} */ /* end of sparser_drv_g SPARSER DRV group */
#endif		/* __FSL_SPARSER_DRV_H */
