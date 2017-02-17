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

/*******************************************************************************
 *                              - CAUTION -
 *
 * This code must not be distributed till we haven't a clear understanding
 * about what internals of the Parser may be exposed to customers.
 ******************************************************************************/

/**************************************************************************//**
@File		fsl_sparser_dump.h

@Description	Soft Parser information dumping API
*//***************************************************************************/
#ifndef __FSL_SPARSER_DUMP_H
#define __FSL_SPARSER_DUMP_H

#include "fsl_sparser_disa.h"

/**************************************************************************//**
@Group		sparser_dump_g SPARSER DUMP

@Description	Contains APIs for dumping :
		- Parse Error code
		- Frame Attribute Flags
		- Parse Result
@{
*//***************************************************************************/

/**************************************************************************//**
@Function	sparser_parse_error_print

@Description	Prints detailed parse error code information.

@Return		None.

*//***************************************************************************/
void sparser_parse_error_print(struct sp_parse_result *pr);

/**************************************************************************//**
@Function	sparser_frame_attributes_dump

@Description	Prints detailed Frame Attribute Flags information.

@Return		None.

*//***************************************************************************/
void sparser_frame_attributes_dump(struct sp_parse_result *pr);

/**************************************************************************//**
@Function	sparser_parse_result_dump

@Description	Prints detailed Parse Results information.

@Return		None.

*//***************************************************************************/
void sparser_parse_result_dump(struct sp_parse_result *pr);

/** @} */ /* end of sparser_dump_g SPARSER DUMP group */
#endif		/* __FSL_SPARSER_DUMP_H */
