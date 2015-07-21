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
@File		aiop_verification_data.h

@Description	This file contains the AIOP SW Verification Data
		needed for the test

*//***************************************************************************/

#include "common/types.h"

#ifndef __AIOP_VERIFICATION_DATA_H_
#define __AIOP_VERIFICATION_DATA_H_

#define __VERIF_GLOBAL __declspec(section ".verif_data")
#define __VERIF_TLS __declspec(section ".verif_tdata")
//#define __VERIF_PROFILE_SRAM __declspec(section ".verif_psram_data")

#pragma section RW ".verif_data" ".verif_bss"
#pragma section RW ".verif_tdata" ".verif_tbss"
//#pragma section RW ".verif_psram_data" ".verif_psram_bss"

/** DDR code location */
#pragma section RX ".dtext_vle"
#define __COLD_CODE __declspec(section ".dtext_vle")


/*
struct profile_sram {
	uint64_t ip_secific_sp_info; *< IP-Specific SP Information
	uint16_t dl; *<  DataLength(correction)
	uint16_t reserved; *< reserved
	uint16_t dhr; *< DataHeadRoom(correction)
	uint8_t  mode_bits1; *< mode bits
	uint8_t  mode_bits2; *< mode bits
	uint16_t pbs1; *<  Pool Buffer Size
	uint16_t bpid1; *<  Bypass Memory Translation
	uint16_t pbs2; *<  Pool Buffer Size
	uint16_t bpid2; *<  Bypass Memory Translation
	uint16_t pbs3; *<  Pool Buffer Size
	uint16_t bpid3; *<  Bypass Memory Translation
	uint16_t pbs4; *<  Pool Buffer Size
	uint16_t bpid4; *<  Bypass Memory Translation
};
*/

//struct profile_sram {
//	uint64_t ip_secific_sp_info; /**< IP-Specific SP Information 	 */
//	uint16_t reserved; /**< reserved 	 */
//	uint16_t dl; /**<  DataLength(correction)	 */
//	uint8_t  mode_bits2; /**< mode bits 	 */
//	uint8_t  mode_bits1; /**< mode bits 	 */
//	uint16_t dhr; /**< DataHeadRoom(correction) */
//	uint16_t bpid1; /**<  Bypass Memory Translation	 */
//	uint16_t pbs1; /**<  Pool Buffer Size	 */
//	uint16_t bpid2; /**<  Bypass Memory Translation	 */
//	uint16_t pbs2; /**<  Pool Buffer Size	 */
//	uint16_t bpid3; /**<  Bypass Memory Translation	 */
//	uint16_t pbs3; /**<  Pool Buffer Size	 */
//	uint16_t bpid4; /**<  Bypass Memory Translation	 */
//	uint16_t pbs4; /**<  Pool Buffer Size	 */
//};

/*
	#define mode_bits1_PTAR_MASK  0x80
	#define mode_bits1_SGHR_MASK  0x10
	#define mode_bits1_ASAR_MASK  0x0f

	#define mode_bits2_BS_MASK  0x80
	#define mode_bits2_FF_MASK  0x30
	#define mode_bits2_VA_MASK  0x04
	#define mode_bits2_DLC_MASK 0x01

	#define mode_bits1_PTAR  0x80
	#define mode_bits1_Rsrvd 0x00
	#define mode_bits1_SGHR  0x00
	#define mode_bits1_ASAR  0x0f

	#define mode_bits2_BS  0x00
	#define mode_bits2_FF  0x00
	#define mode_bits2_VA  0x00
	#define mode_bits2_DLC 0x00

*/

void init_verif();
void init_profile_sram();

#endif /* __AIOP_VERIFICATION_DATA_H_ */
