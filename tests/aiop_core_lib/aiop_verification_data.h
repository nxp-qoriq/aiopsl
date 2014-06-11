/**************************************************************************//**
@File		aiop_verification_data.h

@Description	This file contains the AIOP SW Verification Data
		needed for the test

		Copyright 2013 Freescale Semiconductor, Inc.
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
