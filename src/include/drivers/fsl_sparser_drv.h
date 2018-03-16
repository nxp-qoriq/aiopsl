/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
	/* Size of parameters in bytes. Must be 0 if SP has no parameter. */
	uint8_t			param_size;
	/* Offset of the SP parameters (if any) in the Parameters Array
	 * (a 64 bytes array) */
	uint8_t			param_off;
};

/**************************************************************************//**
@Description	enum sparser_preloaded - Pre-loaded Soft Parsers

		This enumeration contains the soft parsers defined by AIOP SL
		and loaded into the AIOP Parser instructions memory in the
		early initialization phase of the soft parser driver.
		Applications may call a soft parser if it is a routine or jump
		to a soft parser if it is a soft HXS.

		The pre-loaded soft parsers are loaded to the bottom of the
		instructions memory. Application defined soft parsers may be
		loaded into the memory staring with the 0x020 PC address.
		The size of all application defined soft parsers can't exceed :
			2 * (0x73c - 0x20) = 3640 bytes

*//***************************************************************************/
enum sparser_preloaded {
	/* Routine computing the "Running Sum" on a header of a given length.
	 * The length of the header must be set by the calling soft parser into
	 * the GPRV0 register (RA[0:1]) of the Parse Array.
	 * The routine reserves, for internal computations, the GPRV1 (RA[2:3])
	 * and the GPRV2 (RA[4:5]) registers. The working registers are not
	 * preserved.
	 * The "Running Sum" value in the Parse Array is updated with the
	 * computed value. The "Window Offset" register is set to point to the
	 * beginning of the next header. Calling soft parser may advance to the
	 * next header just after the routine returns.
	 *
	 * Attributes:
	 *	Type			: Routine
	 *	Loading Address (PC)	: 0x73c
	 *	Byte-code size		: 388 bytes
	 *	Parameters		: None
	 *
	 * ===================================================================
	 *						Computation example
	 * -------------------------------------------------------------------
	 * Operation				Cycles	L=46	L=18	L=14
	 * ===================================================================
	 * Store custom header length (L)	1	1	1	1
	 * -------------------------------------------------------------------
	 * Routine call				1	1	1	1
	 * ===================================================================
	 * # of 16 bytes windows (N) and
	 * # of the remaining bytes		8	8	8	8
	 * -------------------------------------------------------------------
	 * For each 16 bytes window		15 * N	30	15
	 * -------------------------------------------------------------------
	 * Remained more than 8 bytes ?		3	3	3	3
	 * -------------------------------------------------------------------
	 * Remained >= 8 bytes			9	8		8
	 * -------------------------------------------------------------------
	 * Remained 0 bytes			4
	 * Remained 1 or 3 bytes		12
	 * Remained 2 bytes			11		11
	 * Remained 4 or 6 bytes		13	13		13
	 * Remained 5 or 7 bytes		14
	 * -------------------------------------------------------------------
	 * Total					64	39	34
	 * -------------------------------------------------------------------
	 */
	sp_compute_running_sum = 0x73c
};

/**************************************************************************//**
@Function	sparser_drv_load_ingress_parser

@Description	Load a soft parser into the internal instructions memory of the
		AIOP Parser. The driver checks the overlapping conditions of the
		soft parser code. The driver checks the overlapping conditions
		of the soft parser parameters, if any, in the Parameter Array of
		the "ingress" parse profile.

@Param[in]	sp : Soft parser information.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_drv_load_ingress_parser(struct sparser_info *sp);

/**************************************************************************//**
@Function	sparser_drv_load_egress_parser

@Description	Load a soft parser into the internal instructions memory of the
		AIOP Parser. The driver checks the overlapping conditions of the
		soft parser code. The driver checks the overlapping conditions
		of the soft parser parameters, if any, in the Parameter Array of
		the "egress" parse profile.

@Param[in]	sp : Soft parser information.

@Return		0 on success, -1 on failure. Prints error messages, showing
		what error occurred.

*//***************************************************************************/
int sparser_drv_load_egress_parser(struct sparser_info *sp);

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

/*************************************************************************//**
@Function	parser_enable_ipv6_atomic_frag_detection

@Description	Enables ipv6 atomic fragment detection.

@Return		None.
*//***************************************************************************/
void parser_enable_ipv6_atomic_frag_detection(void);

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
