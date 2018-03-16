/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
@File		sec.h

@Description	This header contains AIOP SEC glue code.

*//***************************************************************************/

#ifndef __SEC_H
#define __SEC_H

/**************************************************************************//**
@Description	SEC Returned Status

 SEC Job termination status/error word
 bits 31-28      bits 3-0 / bits 7-0
 (Source of      (ERRID)  / (Error Code)
  the status
  code)
 -----------     ---------
 2h (CCB)	    Ah - ICV check failed
 -----------     ---------
 4h (DECO)	    83h - Anti-replay LATE error
		    84h - Anti-replay REPLAY error
		    85h - Sequence number overflow

*//***************************************************************************/

/** No Error */
#define	SEC_NO_ERROR					0x00000000

#define SEC_COMPRESSED_ERROR				0x83000000
#define SEC_COMPRESSED_ERROR_MASK			0xFF000000

/** ICV comparison failed */
#define	SEC_ICV_COMPARE_FAIL				0x2000000A
#define	SEC_ICV_COMPARE_FAIL_COMPRESSED			0x8320000A

/** Anti Replay Check: Late packet */
#define	SEC_AR_LATE_PACKET				0x40000083
#define	SEC_AR_LATE_PACKET_COMPRESSED			0x83400083

/** Anti Replay Check: Replay packet */
#define	SEC_AR_REPLAY_PACKET				0x40000084
#define	SEC_AR_REPLAY_PACKET_COMPRESSED			0x83400084

/** Sequence Number overflow */
#define	SEC_SEQ_NUM_OVERFLOW				0x40000085
#define	SEC_SEQ_NUM_OVERFLOW_COMPRESSED			0x83400085

/** Buffer Pool depletion */
#define	SEC_TABLE_BP0_DEPLETION				0x100000C0
#define	SEC_TABLE_BP0_DEPLETION_COMPRESSED		0x831000C0
#define	SEC_DATA_BP0_DEPLETION_NO_OF			0x100000C2
#define	SEC_DATA_BP0_DEPLETION_NO_OF_COMPRESSED		0x831000C2
#define	SEC_DATA_BP0_DEPLETION_PART_OF			0x100000C4
#define	SEC_DATA_BP0_DEPLETION_PART_OF_COMPRESSED	0x831000C4

#define	SEC_CCB_ERROR_MASK				0xF000000F
#define	SEC_DECO_ERROR_MASK				0xF00000FF

#define	SEC_CCB_ERROR_MASK_COMPRESSED			0xFFF0000F
#define	SEC_DECO_ERROR_MASK_COMPRESSED			0xFFF000FF

/*
 * PS (Pointer Size)
 * This bit determines the size of SEC descriptor address pointers
 * 0 - SEC descriptor pointers require one 32-bit word
 * 1 - SEC descriptor pointers require two 32-bit words
 */
#define SEC_POINTER_SIZE		1

#define SEC_FLOW_CONTEXT_SIZE		64

/**************************************************************************//**
 @Description	SEC Flow Context (FLC) Descriptor
*//***************************************************************************/
struct sec_flow_context {
	/** Word0[11-0]  SDID */
	uint16_t word0_sdid;
	/** Word0[31-12] reserved */
	uint16_t word0_res;
	/** Word1[5-0] SDL; Word1[7-6] reserved */
	uint8_t word1_sdl;
	/** Word1[11-8] CRID; Word1[14-12] reserved; Word1[15] CRJD */
	uint8_t word1_bits_15_8;
	/**
	 * Word1[16] EWS; Word1[17] DAC; Word1[18-20] ?; Word1[23-21] reserved
	 */
	uint8_t word1_bits23_16;
	/** Word1[24] RSC; Word1[25] RBMT; Word1[31-26] reserved */
	uint8_t word1_bits31_24;
	/** Word2 RFLC[31-0] */
	uint32_t word2_rflc_31_0;
	/** Word3 RFLC[63-32] */
	uint32_t word3_rflc_63_32;
	/** Word4[15-0] ICID */
	uint16_t word4_iicid;
	/** Word4[31-16] OICID */
	uint16_t word4_oicid;
	/** Word5[23-0] OFQID */
	uint8_t word5_7_0;
	uint8_t word5_15_8;
	uint8_t word5_23_16;
	/**
	 * Word5[24] OSC; Word5[25] OBMT; Word5[29-26] reserved;
	 * Word5[31-30] ICR
	 */
	uint8_t word5_31_24;
	/** Word6 OFLC[31-0] */
	uint32_t word6_oflc_31_0;
	/** Word7 OFLC[63-32] */
	uint32_t word7_oflc_63_32;
	/** Words 8-15 are a copy of the standard storage profile */
	uint64_t storage_profile[4];
};

#endif /* __SEC_H */
