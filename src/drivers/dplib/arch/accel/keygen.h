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
@File		keygen.h

@Description	This file contains the AIOP SW Key Generation API

*//***************************************************************************/

#ifndef __KEYGEN_H
#define __KEYGEN_H

#include "general.h"
#include "fsl_keygen.h"

/**************************************************************************//**
@Group	KEYGEN AIOP KEYGEN Internal

@Description	AIOP KEYGEN API

@{
*//***************************************************************************/

/**************************************************************************//**
@Group	KEYGEN_MACROS KEYGEN Macros

@Description	AIOP KEYGEN Macros

@{
*//***************************************************************************/
/**************************************************************************//**
@Group	KEYGEN_DEFINES HW Key Generation Defines
@{
*//***************************************************************************/
	/** Opaquein valid */
#define KEYGEN_OPAQUEIN_VALID		0x10000000

/** @} */ /* end of KEYGEN_DEFINES */

/**************************************************************************//**
@Group	KEYGEN_MTYPE HW Key Generation Message Types
@{
*//***************************************************************************/

	/** Key composition rule create or replace */
#define KEYGEN_KEY_COMPOSITION_RULE_CREATE_OR_REPLACE_MTYPE	0x003D
	/** Key composition rule query */
#define KEYGEN_KEY_COMPOSITION_RULE_QUERY_MTYPE			0x0037
	/** Key generate with explicit parse result */
#define KEYGEN_KEY_GENERATE_EPRS_MTYPE				0x0035
	/** Generate hash (Parses the frame) */
#define KEYGEN_HASH_GEN_MTYPE					0x0095
	/** Generate hash from explicit parse result*/
#define KEYGEN_HASH_GEN_EPRS_MTYPE				0x0195
	/** Generate hash from explicit key */
#define KEYGEN_HASH_GEN_KEY_MTYPE				0x009D

/** @} */ /* end of KEYGEN_MTYPE */


/**************************************************************************//**
@Group	KEYGEN_KCR KEYGEN KCR Builder defines
@{
*//***************************************************************************/

	/** Number of FEC's (first byte in the KCR) */
#define	KEYGEN_KCR_NFEC			0
	/** OP0 HET Protocol Based Extraction */
#define KEYGEN_KCR_OP0_HET_PROTOCOL	0x00
	/** OP0 HET Generic Extraction */
#define KEYGEN_KCR_OP0_HET_GEC		0x80
	/** Protocol based Extraction Header validation */
#define KEYGEN_KCR_PROTOCOL_HVT		0x40
	/** Mask extension exists */
#define KEYGEN_KCR_MASK_EXT		0x01

	/** Generic Extraction max. extract size */
#define KEYGEN_KCR_MAX_EXTRACT_SIZE	0xF
	/** Generic Extraction max. extract offset */
#define KEYGEN_KCR_MAX_EXTRACT_OFFET	0xF
	/** 16 Bytes alignment */
#define KEYGEN_KCR_16_BYTES_ALIGNMENT	0xF0
	/** Offset within 16 bytes */
#define KEYGEN_KCR_OFFSET_WITHIN_16_BYTES	0x0F


	/** Maximum KCR size */
#define KEYGEN_KCR_MAX_KCR_SIZE				64
	/** Constant FEC size */
#define KEYGEN_KCR_CONST_FEC_SIZE			3
	/** Protocol Specific FEC size (not including mask) */
#define KEYGEN_KCR_PROTOCOL_SPECIFIC_FEC_SIZE		1
	/** Protocol Based Generic FEC size (not including mask) */
#define KEYGEN_KCR_PROTOCOL_BASED_GENERIC_FEC_SIZE	4
	/** Generic FEC size (not including mask) */
#define KEYGEN_KCR_GENERIC_FEC_SIZE			4
	/** Lookup Result FEC size (not including mask) */
#define KEYGEN_KCR_LOOKUP_RES_FEC_SIZE			4
	/** Valid Field FEC size (not including mask) */
#define KEYGEN_KCR_VALID_FIELD_FEC_SIZE			2


	/** Generic Extraction offset 0x00 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x00	0x00
	/** Generic Extraction offset 0x10 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x10	0x01
	/** Generic Extraction offset 0x20 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x20	0x02
	/** Generic Extraction offset 0x30 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x30	0x03
	/** Generic Extraction offset 0x40 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x40	0x04
	/** Generic Extraction offset 0x50 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x50	0x05
	/** Generic Extraction offset 0x60 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x60	0x06
	/** Generic Extraction offset 0x70 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x70	0x07
	/** Generic Extraction offset 0x80 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x80	0x08
	/** Generic Extraction offset 0x90 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0x90	0x09
	/** Generic Extraction offset 0xA0 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0xA0	0x0A
	/** Generic Extraction offset 0xB0 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0xB0	0x0B
	/** Generic Extraction offset 0xC0 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0xC0	0x0C
	/** Generic Extraction offset 0xD0 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0xD0	0x0D
	/** Generic Extraction offset 0xE0 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0xE0	0x0E
	/** Generic Extraction offset 0xF0 from start of frame */
#define KEYGEN_KCR_EOM_FRAME_OFFSET_0xF0	0x0F
	/** Generic Extraction offset 0x00 from start of frame */
#define KEYGEN_KCR_EOM_PARSE_RES_OFFSET_0x00	0x10
	/** Generic Extraction offset 0x10 from Parse Result */
#define KEYGEN_KCR_EOM_PARSE_RES_OFFSET_0x10	0x11
	/** Generic Extraction offset 0x20 from Parse Result */
#define KEYGEN_KCR_EOM_PARSE_RES_OFFSET_0x20	0x12
	/** Generic Extraction offset 0x30 from Parse Result */
#define KEYGEN_KCR_EOM_PARSE_RES_OFFSET_0x30	0x13

	/** EOM of Opaque0 Field from FCV */
#define KEYGEN_KCR_EXT_OPAQUE0_EOM	0x18
	/** EOM of Opaque1 Field from FCV */
#define KEYGEN_KCR_EXT_OPAQUE1_EOM	0x19
	/** EOM of Opaque2 Field from FCV */
#define KEYGEN_KCR_EXT_OPAQUE2_EOM	0x18
	/** EOM of Unique ID Field from FCV */
#define KEYGEN_KCR_EXT_UNIQUE_ID_EOM	0x1A
	/** EOM of Timestamp Field from FCV */
#define KEYGEN_KCR_EXT_TIMESTAMP_EOM	0x1A
	/** EOM of OpaqueIn Field from FCV */
#define KEYGEN_KCR_EXT_OPAQUE_IN_EOM	0x14


	/** Basic Extraction Offset of Opaque0 */
#define KEYGEN_KCR_EXT_OPAQUE0_BASIC_EO		0x8
	/** Basic Extraction Offset of Opaque1 */
#define KEYGEN_KCR_EXT_OPAQUE1_BASIC_EO		0x0
	/** Basic Extraction Offset of Opaque2 */
#define KEYGEN_KCR_EXT_OPAQUE2_BASIC_EO		0x1
	/** Basic Extraction Offset of Unique */
#define KEYGEN_KCR_EXT_UNIQUE_ID_BASIC_EO	0x8
	/** Basic Extraction Offset of Timestamp */
#define KEYGEN_KCR_EXT_TIMESTAMP_BASIC_EO	0xC
	/** Basic Extraction Offset of OpaqueIn */
#define KEYGEN_KCR_EXT_OPAQUE_IN_BASIC_EO	0x8

	/** Opaque2 Field size to extract (= real size - 1) */
#define KEYGEN_KCR_EXT_OPAQUE2_SIZE	0x00
	/** Unique ID Field size to extract (= real size - 1) */
#define KEYGEN_KCR_EXT_UNIQUE_ID_SIZE	0x03
	/** Timestamp Field size to extract (= real size - 1) */
#define KEYGEN_KCR_EXT_TIMESTAMP_SIZE	0x03

/** @} */ /* end of KEYGEN_KCR */

/**************************************************************************//**
@Group	FSL_KEYGEN_HW_STATUS_GENERAL General HW status defines
@{
*//***************************************************************************/

	/** Command status success */
#define KEYGEN_HW_STATUS_SUCCESS		0x00000000
	/** Key composition error: indicates of either Invalid key composition
	 * ID (KeyID) or KeyID with 0 FECs or Key size error (key>124 bytes) */
#define KEYGEN_HW_STATUS_KSE			0x00000400
	/** Extract Out Of Frame Header Error for Key Generation.
	 This bit is set if key composition attempts to extract a field which
	 is not in the frame header. */
#define KEYGEN_HW_STATUS_EOFH			0x00000200

/** @} */ /* end of FSL_KEYGEN_HW_STATUS_GENERAL */

/** @} */ /* end of KEYGEN_MACROS */

/** \addtogroup KEYGEN_Enumerations
 *  @{
 */

/**************************************************************************//**
 @enum keygen_functions

 @Description	AIOP KEYGEN Functions enumeration.

 @{
*//***************************************************************************/
enum keygen_function_identifier {
	KEYGEN_KCR_BUILDER_INIT = 0,
	KEYGEN_KCR_BUILDER_ADD_CONSTANT_FEC,
	KEYGEN_KCR_BUILDER_ADD_INPUT_VALUE_FEC,
	KEYGEN_KCR_BUILDER_ADD_PROTOCOL_SPECIFIC_FIELD,
	KEYGEN_KCR_BUILDER_ADD_PROTOCOL_BASED_GENERIC_FEC,
	KEYGEN_KCR_BUILDER_ADD_GENERIC_EXTRACT_FEC,
#ifdef REV2
	KEYGEN_KCR_BUILDER_ADD_LOOKUP_RESULT_FIELD_FEC,
#endif
	KEYGEN_KCR_BUILDER_ADD_VALID_FIELD_FEC,
	KEYGEN_KCR_CREATE,
	KEYGEN_KCR_REPLACE,
	KEYGEN_KCR_DELETE,
	KEYGEN_KCR_QUERY,
	KEYGEN_GEN_KEY,
	KEYGEN_GEN_HASH
};

/** @}*/ /* end of group KEYGEN_Enumerations */


/**************************************************************************//**
@Group		KEYGEN_STRUCTS KEYGEN Structures

@Description	AIOP KEYGEN Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	Input message Structure
*//***************************************************************************/
#pragma pack(push, 1)
struct keygen_input_message_params {
	uint16_t fha;
	uint16_t frs;
	uint16_t pra;
	uint16_t reserved1;
	uint16_t fda;
	uint16_t reserved2;
	uint16_t falugrp;
	uint16_t gross_running_sum;
	uint64_t opaquein;
};
#pragma pack(pop)

/**************************************************************************//**
@Description	KEYGEN KCR Builder FEC Mask
*//***************************************************************************/
#pragma pack(push, 1)
struct	keygen_hw_fec_mask {
	/** Number of mask bytes & Mask offset 0.
	First Nibble: Number of mask bytes, determines the size of the FEC mask
	extension.
	Second Nibble: Mask offset 0. */
	uint8_t  nmsk_moff0;

	/** Mask0.
	Bit-wise-mask which is applied to the extracted header at offset MOFF0
	from its beginning */
	uint8_t  mask0;

	/** Mask offset 1 & Mask offset 2.
	First Nibble: Mask offset 1.
	Second Nibble: Mask offset 2. */
	uint8_t  moff1_moff2;

	/** Mask 1.
	Bit-wise-mask which is applied to the extracted header at offset MOFF1
	from its beginning */
	uint8_t  mask1;

	/** Mask 2.
	Bit-wise-mask which is applied to the extracted header at offset MOFF2
	from its beginning */
	uint8_t  mask2;

	/** Mask offset 3.
	First Nibble: Reserved.
	Second Nibble: Mask offset 3. */
	uint8_t  res_moff3;

	/** Mask 3.
	Bit-wise-mask which is applied to the extracted header at offset MOFF3
	from its beginning */
	uint8_t  mask3;
};
#pragma pack(pop)

/** @} */ /* end of KEYGEN_STRUCTS */
/**************************************************************************//**
@Function	keygen_exception_handler

@Description	Handler for the error status returned from the KEYGEN API
		functions.

@Param[in]	file_path - The path of the file in which the error occurred.
@Param[in]	func_id - The function in which the error occurred.
@Param[in]	line - The line in which the error occurred.
@Param[in]	status - Status to be handled be this function.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void keygen_exception_handler(enum keygen_function_identifier func_id,
			     uint32_t line,
			     int32_t status);

/** @} */ /* end of KEYGEN */

#endif /* __KEYGEN_H */
