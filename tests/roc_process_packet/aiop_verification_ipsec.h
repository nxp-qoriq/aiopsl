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
@File		aiop_verification_ipsec.h

@Description	This file contains the AIOP IPsec SW Verification Structures.

*//***************************************************************************/

/**************************************************************************//**
	IPsec Verification Wrapper Usage Flow Example
	----------------------------------------------------
 1. IPSEC_INIT_CMD - set the maximum number of SAs
 2. IPSEC_ADD_SA_DESCRIPTOR_CMD - create SA descriptor for Encryption
 3. IPSEC_ADD_SA_DESCRIPTOR_CMD - create SA descriptor for Decryption
 4. IPSEC_FRAME_ENCR_DECR_CMD - encrypt and decrypt a frame in a single command
 5. IPSEC_DEL_SA_DESCRIPTOR_CMD - delete the Encryption SA descriptor
 6. IPSEC_DEL_SA_DESCRIPTOR_CMD - delete the Decryption SA descriptor
 
*//***************************************************************************/

#ifndef __AIOP_VERIFICATION_IPSEC_H_
#define __AIOP_VERIFICATION_IPSEC_H_

#include "fsl_ipsec.h"

/* IPSEC Commands IDs */
#define IPSEC_INIT_CMD 						1 | (IPSEC_MODULE << 16)
#define IPSEC_ADD_SA_DESCRIPTOR_CMD			2 | (IPSEC_MODULE << 16)
#define IPSEC_DEL_SA_DESCRIPTOR_CMD			3 | (IPSEC_MODULE << 16)
#define IPSEC_GET_LIFETIME_STATS_CMD		4 | (IPSEC_MODULE << 16)
#define IPSEC_DECR_LIFETIME_COUNTERS_CMD	5 | (IPSEC_MODULE << 16)
#define IPSEC_GET_SEQ_NUM_CMD				6 | (IPSEC_MODULE << 16)
#define IPSEC_FRAME_DECRYPT_CMD				7 | (IPSEC_MODULE << 16)
#define IPSEC_FRAME_ENCRYPT_CMD 			8 | (IPSEC_MODULE << 16)
#define IPSEC_CREATE_INSTANCE_CMD 			10 | (IPSEC_MODULE << 16)
#define IPSEC_DELETE_INSTANCE_CMD 			11 | (IPSEC_MODULE << 16)
#define IPSEC_FRAME_ENCR_DECR_CMD 		   16 | (IPSEC_MODULE << 16)

// Temporary workaround commands
#define IPSEC_RUN_DESC_DEBUG 				9 | (IPSEC_MODULE << 16)
#define IPSEC_CREATE_DEBUG_BUFFER_CMD		17 | (IPSEC_MODULE << 16)
#define IPSEC_EARLY_INIT_CMD				18 | (IPSEC_MODULE << 16)

#define IPSEC_FORCE_SECONDS_LIFTEIME_EXP_CMD	19 | (IPSEC_MODULE << 16)

#define IPSEC_PARSER_HXS_ENET 0x00
#define IPSEC_PARSER_HXS_IP   0x06
#define IPSEC_PARSER_HXS_IPV4 0x07
#define IPSEC_PARSER_HXS_IPV6 0x08

/** \addtogroup AIOP_FMs_Verification
 *  @{
 */

/**************************************************************************//**
 @Group		AIOP_GRO_Verification

 @Description	AIOP GRO Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPsec Early Init Command structure.
*
* This function enables returning a dummy error value according to the 
* committed_buffs input
* if committed_buffs == 0xFFFFFFF0, it returns ENAVAIL.
* if committed_buffs == 0xFFFFFFF2, it returns ENOMEM.
* Otherwise it will return 0.
* 
*//****************************************************************************/
struct ipsec_early_init_command {
	uint32_t opcode;
	uint32_t total_instance_num;
	uint32_t total_committed_sa_num;
	uint32_t total_max_sa_num;
	uint32_t flags;
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Add SA Descriptor Command structure.

*//****************************************************************************/
struct ipsec_init_command {
	uint32_t opcode;
	uint32_t max_sa_no; /* pointer of descriptor handle */

	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Create Instance Command structure.

*//****************************************************************************/
struct ipsec_create_instance_command {
	uint32_t opcode;
	uint32_t committed_sa_num;
	uint32_t max_sa_num;
	uint32_t instance_flags;
	uint8_t tmi_id;
	
	uint32_t instance_id;
	uint64_t instance_addr; /* descriptor address */
	
	//uint64_t instance_handle;
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Delete Instance Command structure.

*//****************************************************************************/
struct ipsec_delete_instance_command {
	uint32_t opcode;

	uint32_t instance_id;
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};
	

/**************************************************************************//**
@Description	IPsec Add SA Descriptor Command structure.

*//****************************************************************************/
struct ipsec_add_sa_descriptor_command {
	uint32_t opcode;
	struct ipsec_descriptor_params params;
	//uint32_t ipsec_handle_ptr; /* pointer of descriptor handle */
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */

	//uint64_t instance_handle;
	uint32_t instance_id;

	uint64_t descriptor_addr; /* descriptor address */
    uint8_t outer_ip_header[136]; /* outer IP header */
    
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Delete SA Descriptor Command structure.

*//****************************************************************************/
struct ipsec_del_sa_descriptor_command {
	uint32_t opcode;
	//uint32_t ipsec_handle_ptr; /* pointer of descriptor handle */
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */

	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Get Lifetime Counters Command structure.

*//****************************************************************************/
struct ipsec_get_lifetime_stats_command {
	uint32_t opcode;
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */
	uint64_t kilobytes;
	uint64_t packets;
	uint64_t dropped_pkts;
	uint32_t sec;
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;
	int32_t status; /* Function call return status */
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Decrement Lifetime Counters Command structure.

*//****************************************************************************/
struct ipsec_decr_lifetime_counters_command {
	uint32_t opcode;
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */
	uint32_t kilobytes_decr_val;
	uint32_t packets_decr_val;
	uint32_t dropped_pkts_decr_val;

	/** Returned Value: presentation context. */
	struct presentation_context prc;
	int32_t status; /* Function call return status */
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Get Sequence Number Command structure.

*//****************************************************************************/
struct ipsec_get_seq_num_command {
	uint32_t opcode;
	//uint32_t ipsec_handle_ptr; /* pointer of descriptor handle */
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */

	uint32_t sequence_number;
	uint32_t extended_sequence_number;
	uint32_t anti_replay_bitmap[4];
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Decryption Command structure.

*//****************************************************************************/
struct ipsec_frame_decrypt_command {
	uint32_t opcode;
	//uint32_t ipsec_handle_ptr; /* pointer of descriptor handle */
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */

	uint32_t dec_status; /* SEC Decryption status */
	
	uint16_t starting_hxs; /* parser starting HXS.
	 	 	 	 	 	 	 * use one IPSEC_PARSER_HXS_* */
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;
	
	/** Returned Value: parse results. */
	struct parse_result pr;
	
	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Encryption Command structure.

*//****************************************************************************/
struct ipsec_frame_encrypt_command {
	uint32_t opcode;
	//uint32_t ipsec_handle_ptr; /* pointer of descriptor handle */
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */

	uint32_t enc_status; /* SEC Encryption status */
	
	uint16_t starting_hxs; /* parser starting HXS.
	 	 	 	 	 	 	 * use one IPSEC_PARSER_HXS_* */
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	/** Returned Value: parse results. */
	struct parse_result pr;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/**************************************************************************//**
@Description	IPsec Encryption and Decryption Command structure.
				Encrypt and Decrypt the same frame in a single command

*//****************************************************************************/
struct ipsec_frame_encr_decr_command {
	uint32_t opcode;
	//uint32_t ipsec_encr_handle_ptr; /* pointer of encr. descriptor handle */
	//uint32_t ipsec_decr_handle_ptr; /* pointer of decr. descriptor handle */
	
	uint32_t encr_sa_desc_id; /* Descriptor ID, of handles array in shared RAM */
	uint32_t decr_sa_desc_id; /* Descriptor ID, of handles array in shared RAM */
	
	uint32_t enc_status; /* SEC Encryption status */
	uint32_t dec_status; /* SEC Decryption status */

	uint16_t starting_hxs; /* parser starting HXS.
	 	 	 	 	 	 	 * use one IPSEC_PARSER_HXS_* */
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	/** Returned Value: parse results. */
	struct parse_result pr;

	int32_t fm_encr_status; /* Encryption Function call return status */
	int32_t fm_decr_status; /* Decryption Function call return status */

	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t fm_encr_status_addr;
	uint32_t fm_decr_status_addr;
};

/* RTA descriptor debug */
struct ipsec_run_desc_debug_command {
	uint32_t opcode;
	struct ipsec_descriptor_params params;
	
	uint64_t descriptor_addr; /* descriptor address */
    
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};


/* Create debug buffer */
struct ipsec_create_debug_buffer_command {
	uint32_t opcode;
	
	uint64_t buffer_addr; /* buffer address */
	uint32_t buffer_size;
	
	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

/* Force seconds lifetime expiry command */
struct ipsec_force_seconds_lifetime_exp_command {
	uint32_t opcode;
	
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */

	/** Returned Value: presentation context. */
	struct presentation_context prc;

	int32_t status; /* Function call return status */
	
	/** Workspace address of the last returned status.
	 * Should be defined in the TLS area. */
	uint32_t status_addr;
};

uint16_t  aiop_verification_ipsec(uint32_t data_addr);

/** @} */ /* end of AIOP_GRO_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */

#endif /* __AIOP_VERIFICATION_IPSEC_H_ */
