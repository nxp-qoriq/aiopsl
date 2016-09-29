/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
@File		cwap_dtls.h

@Description	This file contains the AIOP CAPWAP DTLS
		internal functions and definitions.
*//***************************************************************************/

#ifndef __AIOP_CWAP_DTLS_H
#define __AIOP_CWAP_DTLS_H

#include "fsl_types.h"
#include "fsl_gen.h"
#include "fsl_net.h"
#include "system.h"
#include "fsl_platform.h"
#include "fsl_icontext.h"
#include "fsl_cwap_dtls.h"
#include "sec.h"

/* Extern variable used by CWAP/DTLS */
extern __PROFILE_SRAM
struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];
extern struct icontext icontext_aiop;
extern struct platform_app_params g_app_params;

/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group		FSL_CWAP_DTLS CAPWAP DTLS

@Description	AIOP CAPWAP DTLS API

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		CAPWAP_DTLS_ENUM_INT CAPWAP DTLS Enumerations (internal)

 @Description	CAPWAP DTLS Enumerations (internal)

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description	CAPWAP DTLS Function ID
*//***************************************************************************/
enum cwap_dtls_function_identifier {
	CWAP_DTLS_FRAME_DECRYPT = 1,
	CWAP_DTLS_FRAME_ENCRYPT
};

/**************************************************************************//**
 @Description	CAPWAP DTLS SR/Hardware ID
*//***************************************************************************/
enum cwap_dtls_service_identifier {
	CWAP_DTLS_INTERNAL_SERVICE = 1,
	CWAP_DTLS_SEC_HW,
	CWAP_DTLS_FDMA_PRESENT_DEFAULT_FRAME,
	CWAP_DTLS_FDMA_REPLACE_DEFAULT_SEGMENT_DATA,
	CWAP_DTLS_FDMA_STORE_DEFAULT_FRAME_DATA,
	CWAP_DTLS_PARSE_RESULT_GENERATE_DEFAULT
};

/** @} */ /* end of CWAP_DTLS_ENUM_INT */

/**************************************************************************//**
@Group		FSL_CWAP_DTLS_STRUCTS_INT CAPWAP DTLS Structures (internal)

@Description	CAPWAP DTLS Structures (internal)

@{
*//***************************************************************************/

/**************************************************************************//**
 @Description	CAPWAP DTLS Instance parameters
*//***************************************************************************/
struct cwap_dtls_instance_params {
	/** SA (descriptors) counter. Initialized to max number */
	uint32_t sa_count;
	/** Committed SAs (descriptors) */
	uint32_t committed_sa_num;
	/** Maximum SAs (descriptors) */
	uint32_t max_sa_num;
	/** Buffer pool ID outer IP header (TODO) */
	uint16_t outer_ip_bpid;
	/** Buffer pool ID for the SA descriptor */
	uint16_t desc_bpid;
};

/**************************************************************************//**
 @Description	CAPWAP DTLS SA Descriptor parameters for internal usage
*//***************************************************************************/
struct cwap_dtls_sa_params {
	/** Instance handle */
	cwap_dtls_instance_handle_t instance_handle;
	/**
	 * Transport mode, UDP encap, pad check, counters enable, outer
	 * IP version, etc. 4B
	 */
	uint32_t flags;
	/** BPID of output frame in new buffer mode */
	uint16_t bpid;
	/** new/reuse. 1B */
	uint8_t sec_buffer_mode;
	/** SEC output buffer SPID */
	uint8_t output_spid;
	/*
	 * Total size =
	 * 8*8 (64) + 3*4 (12) + 4*2 (8) + 3*1 (3) = 87 bytes
	 * Aligned size = 88 bytes
	 */
};

/**************************************************************************//**
 @Description   CAPWAP DTLS per SA debug information
*//***************************************************************************/
struct cwap_dtls_debug_info {
	enum cwap_dtls_function_identifier func_id;
	enum cwap_dtls_service_identifier service_id;
	uint32_t line;
	int status;
};

/** @} */ /* end of FSL_CWAP_DTLS_STRUCTS_INT */

/**************************************************************************//**
@Group		FSL_CWAP_DTLS_MACROS_INT CAPWAP DTLS Macros (internal)

@Description	CAPWAP DTLS Macros (internal)

@{
*//***************************************************************************/

#define CWAP_DTLS_IS_OUTBOUND_DIR(optype) \
	(((optype) & OP_TYPE_MASK) == OP_TYPE_ENCAP_PROTOCOL)

/* Internal Flags */
/* flags[31] : 1 = outbound, 0 = inbound */
#define CWAP_DTLS_FLG_DIR_OUTBOUND	0x80000000
/* flags[30] : DTLS cipher mode - 1 = GCM, 0 = CBC */
#define CWAP_DTLS_FLG_CIPHER_GCM	0x40000000

/* Storage profile ASAR field mask */
#define CWAP_DTLS_SP_ASAR_MASK		0x000F0000
#define CWAP_DTLS_SP_DHR_MASK		0x00000FFF
#define CWAP_DTLS_SP_REUSE_BS_FF	0xA0000000

#define CWAP_DTLS_SEC_NEW_BUFFER_MODE	0
#define CWAP_DTLS_SEC_REUSE_BUFFER_MODE	1

/**
 *                  SA Descriptor Structure
 * ------------------------------------------------------
 * | cwap_dtls_sa_params              | 128 bytes       | + 0
 * ------------------------------------------------------
 * | sec_flow_context                 | 64 bytes        | + 128
 * -----------------------------------------------------
 * | SEC shared descriptor (SD)       | Up to 256 bytes | + 192
 * ------------------------------------------------------
 * | Authentication Key Copy          | 128 bytes       | + 448
 * ------------------------------------------------------
 * | Cipher Key Copy                  | 32 bytes        | + 576
 * ------------------------------------------------------
 * | Debug/Error information          | 32 bytes        | + 608
 * ------------------------------------------------------
 *
 * cwap_dtls_sa_params	- Parameters used by the CAPWAP DTLS functional module
 * sec_flow_context	- SEC Flow Context
 *			  Should be 64-byte aligned for optimal performance.
 * SEC shared descriptor - Shared descriptor
 * Authentication/Cipher Key Copy - Key Copy area, for CAAM DKP and
 *				    upon HF-NIC requirement
 *
 * Aligned Buffer size = 128 + 64 + 256 + 128 + 32 + 32 = 640
 * Requested buffer size = 10 * 64 = 640 bytes
 */

#define CWAP_DTLS_INTERNAL_PARAMS_SIZE	128
#define CWAP_DTLS_AUTH_KEY_SIZE		128
#define CWAP_DTLS_CIPHER_KEY_SIZE	32
#define CWAP_DTLS_DEBUG_INFO_SIZE	32

/*
 * Maxiumum key size = max(CWAP_DTLS_AUTH_KEY_SIZE, CWAP_DTLS_CIPHER_KEY_SIZE)
 */
#define CWAP_DTLS_KEY_MAX_SIZE		128

/* SA descriptor buffer size */
#define CWAP_DTLS_SA_DESC_BUF_SIZE	640
/* SA descriptor alignment */
#define CWAP_DTLS_SA_DESC_BUF_ALIGN	64

/* Keys copy offsets (from params start) */
#define CWAP_DTLS_AUTH_KEY_OFFSET	448
#define CWAP_DTLS_CIPHER_KEY_OFFSET	(CWAP_DTLS_AUTH_KEY_OFFSET + \
					 CWAP_DTLS_AUTH_KEY_SIZE)

#define CWAP_DTLS_DEBUG_INFO_OFFSET	(CWAP_DTLS_CIPHER_KEY_OFFSET + \
					 CWAP_DTLS_CIPHER_KEY_SIZE)

/* The CAPWAP DTLS data structure should be aligned to 64 bytes (for CAAM) */
#if (CWAP_DTLS_SA_DESC_BUF_ALIGN != 64)
#define CWAP_DTLS_SA_DESC_ADDR(ADDRESS)	ALIGN_UP_64((ADDRESS), \
						    CWAP_DTLS_SA_DESC_BUF_ALIGN)
#else
#define CWAP_DTLS_SA_DESC_ADDR(ADDRESS)	(ADDRESS)
#endif

/* Flow Context Address */
#define CWAP_DTLS_FLC_ADDR(ADDRESS)	((ADDRESS) + \
					 CWAP_DTLS_INTERNAL_PARAMS_SIZE)

/* SEC shared descriptor address */
#define CWAP_DTLS_SD_ADDR(ADDRESS)	(CWAP_DTLS_FLC_ADDR(ADDRESS) + \
					 SEC_FLOW_CONTEXT_SIZE)

/* Key copy addresses */
#define CWAP_DTLS_AUTH_KEY_ADDR(ADDRESS)	((ADDRESS) + \
						 CWAP_DTLS_AUTH_KEY_OFFSET)

#define CWAP_DTLS_CIPHER_KEY_ADDR(ADDRESS)	((ADDRESS) + \
						 CWAP_DTLS_CIPHER_KEY_OFFSET)

#define CWAP_DTLS_DEBUG_INFO_ADDR(ADDRESS)	((ADDRESS) + \
						 CWAP_DTLS_DEBUG_INFO_OFFSET)

#define CWAP_DTLS_FLAGS_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct cwap_dtls_sa_params, flags)))

#define CWAP_DTLS_INSTANCE_HANDLE_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct cwap_dtls_sa_params, instance_handle)))

/* PDB address */
#define CWAP_DTLS_PDB_ADDR(ADDRESS)	(CWAP_DTLS_SD_ADDR(ADDRESS) + 4)

#ifndef CWAP_DTLS_PRIMARY_MEM_PARTITION_ID
	#define CWAP_DTLS_PRIMARY_MEM_PARTITION_ID	MEM_PART_DP_DDR
#endif

#ifndef CWAP_DTLS_SECONDARY_MEM_PARTITION_ID
#ifndef LS1088A_REV1
#define CWAP_DTLS_SECONDARY_MEM_PARTITION_ID	MEM_PART_DP_DDR
#else
#define CWAP_DTLS_SECONDARY_MEM_PARTITION_ID	MEM_PART_SYSTEM_DDR
#endif
#endif

/* AAP Command Fields */
#define CWAP_DTLS_AAP_USE_FLC_SP	0x10000000
#define CWAP_DTLS_AAP_OS_EX		0x00800000

/* Debug error codes */
/* Crypto padding is longer than presentation */
#define CAWP_DTLS_INT_ERR_PAD_TOO_LONG	0x00000001
/* DTLS padding check error */
#define CWAP_DTLS_INT_PAD_CHECK_ERR	0x00000002

/*
 * OSM temporary defines
 * TODO: should move to general or OSM include file
 */
#define CWAP_DTLS_OSM_CONCURRENT	0
#define CWAP_DTLS_OSM_EXCLUSIVE		1

/* Maximum job descriptor length in bytes */
#define CWAP_DTLS_JD_MAX_LEN		((7 * CAAM_CMD_SZ) + (3 * CAAM_PTR_SZ))

/*
 * The max shared descriptor size in 32 bit words when using the AI is
 * 64 words - 13 words reserved for the Job descriptor.
 */
#define CWAP_DTLS_SD_MAX_LEN_WORDS \
	(64 - CWAP_DTLS_JD_MAX_LEN / CAAM_CMD_SZ)

/*
 * Total max growth for DTLS encapsulation:
 * 4-byte CAPWAP DTLS header, 1-byte type, 2-byte version,
 * 8-byte Epoch + Seq Num, 2-byte length, max. 16-byte Optional IV,
 * max. 16-byte Optional IV Mask, max. 32-byte ICV (SHA 512),
 * max 15-byte padding, 1-byte pad length
 * = Total: 97, rounded up to 100
 */
#define CWAP_DTLS_MAX_FRAME_GROWTH	100

/** @} */ /* end of FSL_CWAP_DTLS_MACROS_INT */

/**************************************************************************//**
@Group		FSL_CWAP_DTLS_Functions_INT CAPWAP DTLS Functions (internal)

@Description	AIOP CAPWAP DTLS Functions (internal)

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	cwap_dtls_generate_flc

@Description	Generate SEC Flow Context Descriptor

@Param[in]	params - pointer to descriptor parameters
@Param[in]	sd_size - SEC shared descriptor length (in words)
@Param[out]	flc_address - Flow Context address in external memory
*//***************************************************************************/
void cwap_dtls_generate_flc(struct cwap_dtls_sa_descriptor_params *params,
			    int sd_size, uint64_t flc_address);

/**************************************************************************//**
@Function	cwap_dtls_create_key_copy

@Description	Creates a copy of the key, used for CAAM DKP

@Param[in]	src_key_addr - source key address
@Param[out]	dst_key_addr - destination key address
@Param[in]	src_key_len - length of the provided key (in bytes)
*//***************************************************************************/
void cwap_dtls_create_key_copy(uint64_t src_key_addr, uint64_t dst_key_addr,
			       uint16_t src_key_len);

/**************************************************************************//**
@Function	cwap_dtls_generate_encap_sd

@Description	Generate SEC Shared Descriptor for Encapsulation

@Param[in]	params - pointer to descriptor parameters
@Param[out]	sd_addr - Flow Context address in external memory
@Param[out]	sd_size - generated SEC shared descriptor length (in words)
*//***************************************************************************/
int cwap_dtls_generate_encap_sd(struct cwap_dtls_sa_descriptor_params *params,
				uint64_t sd_addr, int *sd_size);

/**************************************************************************//**
@Function	cwap_dtls_generate_decap_sd

@Description	Generate SEC Shared Descriptor for Decapsulation

@Param[in]	params - pointer to descriptor parameters
@Param[out]	sd_addr - Flow Context address in external memory
@Param[out]	sd_size - generated SEC shared descriptor length (in words)
*//***************************************************************************/
int cwap_dtls_generate_decap_sd(struct cwap_dtls_sa_descriptor_params *params,
				uint64_t sd_addr, int *sd_size);

/**************************************************************************//**
@Function	cwap_dtls_generate_sa_params

@Description	Generate and store the functional module internal parameters

@Param[in]	instance_handle - CAPWAP DTLS instance handle achieved with
		cwap_dtls_create_instance()
@Param[in]	params - pointer to descriptor parameters
@Param[out]	sa_handle - parameters area (start of buffer)
*//***************************************************************************/
void cwap_dtls_generate_sa_params(cwap_dtls_instance_handle_t instance_handle,
				  struct cwap_dtls_sa_descriptor_params *params,
				  cwap_dtls_sa_handle_t sa_handle);

/**************************************************************************//**
@Function	cwap_dtls_get_buffer

@Description	Allocates a buffer for the CAPWAP DTLS parameters according
		to the instance parameters and increments the instance counters

@Param[in]	instance_handle - CAPWAP DTLS instance handle achieved with
		cwap_dtls_create_instance()
@Param[out]	sa_handle - CAPWAP DTLS SA handle
*//****************************************************************************/
int cwap_dtls_get_buffer(cwap_dtls_instance_handle_t instance_handle,
			 cwap_dtls_sa_handle_t *sa_handle);

/**************************************************************************//**
@Function	cwap_dtls_release_buffer

@Description	Release a buffer and decrements the instance counters

@Param[in]	instance_handle - CAPWAP DTLS instance handle achieved with
		cwap_dtls_create_instance()
@Param[out]	sa_handle - CAPWAP DTLS SA handle
*//****************************************************************************/
int cwap_dtls_release_buffer(cwap_dtls_instance_handle_t instance_handle,
			     cwap_dtls_sa_handle_t sa_handle);

/**************************************************************************//**
@Function	cwap_dtls_init_debug_info

@Description	Initialize the debug section of the SA descriptor
*//***************************************************************************/
void cwap_dtls_init_debug_info(cwap_dtls_sa_handle_t desc_addr);

/**************************************************************************//**
@Function	cwap_dtls_error_handler

@Description	Error handler

@Param[in]	sa_handle - SA descriptor
@Param[in]	func_id - Function ID
@Param[in]	service_id - SR/Hardware ID
@Param[in]	line - line number where error handler is called
@Param[in]	status - Error/Status value
*//***************************************************************************/
void cwap_dtls_error_handler(cwap_dtls_sa_handle_t sa_handle,
			     enum cwap_dtls_function_identifier func_id,
			     enum cwap_dtls_service_identifier service_id,
			     uint32_t line, int status);

/**************************************************************************//**
@Function	cwap_dtls_get_ar_info_cbc

@Description	This function returns anti-replay related information for
		DTLS SAs using CBC ciphersuites:
		- DTLS sequence number and epoch
		- Anti-replay bitmap (scorecard) (if applicable)

@Param[in]	desc_addr - CAPWAP DTLS SEC descriptor
@Param[in]	params_flags - value of cwap_dtls_sa_params.flags
@Param[out]	sequence_number - 64-bit value consisting of DTLS epoch (upper
		16 bits) and sequence number (lower 48 bits).
@Param[out]	anti_replay_bitmap - Anti-replay bitmap (4 words):
		* For 1-entry ARS, only the first word is valid
		* For 2-entry ARS, only the first two words are valid
		* For 4-entry ARS, all four words are valid

@Cautions	anti_replay_bitmap is relevant for inbound (decapsulation) only,
		and should be ignored for outbound (encapsulation).

*//****************************************************************************/
void cwap_dtls_get_ar_info_cbc(cwap_dtls_sa_handle_t desc_addr,
			       uint32_t params_flags,
			       uint64_t *sequence_number,
			       uint32_t anti_replay_bitmap[4]);

/**************************************************************************//**
@Function	cwap_dtls_get_ar_info_gcm

@Description	This function returns anti-replay related information for
		DTLS SAs using GCM ciphersuites:
		- DTLS sequence number and epoch
		- Anti-replay bitmap (scorecard) (if applicable)

@Param[in]	desc_addr - CAPWAP DTLS SEC descriptor
@Param[in]	params_flags - value of cwap_dtls_sa_params.flags
@Param[out]	sequence_number - 64-bit value consisting of DTLS epoch (upper
		16 bits) and sequence number (lower 48 bits).
@Param[out]	anti_replay_bitmap - Anti-replay bitmap (4 words):
		* For 1-entry ARS, only the first word is valid
		* For 2-entry ARS, only the first two words are valid
		* For 4-entry ARS, all four words are valid

@Cautions	anti_replay_bitmap is relevant for inbound (decapsulation) only,
		and should be ignored for outbound (encapsulation).

*//****************************************************************************/
void cwap_dtls_get_ar_info_gcm(cwap_dtls_sa_handle_t desc_addr,
			       uint32_t params_flags,
			       uint64_t *sequence_number,
			       uint32_t anti_replay_bitmap[4]);

/** @} */ /* end of FSL_CWAP_DTLS_Functions_INT */
/** @} */ /* end of FSL_CWAP_DTLS */
/** @} */ /* end of NETF */

#endif /* __AIOP_CWAP_DTLS_H */
