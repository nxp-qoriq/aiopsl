/**************************************************************************//**
@File		aiop_verification_ipsec.h

@Description	This file contains the AIOP IPsec SW Verification Structures.

		Copyright 2014 Freescale Semiconductor, Inc.
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

#include "dplib/fsl_ipsec.h"

/* IPSEC Commands IDs */
#define IPSEC_INIT_CMD 						1 | (IPSEC_MODULE << 16)
#define IPSEC_ADD_SA_DESCRIPTOR_CMD			2 | (IPSEC_MODULE << 16)
#define IPSEC_DEL_SA_DESCRIPTOR_CMD			3 | (IPSEC_MODULE << 16)
#define IPSEC_GET_LIFETIME_STATS_CMD		4 | (IPSEC_MODULE << 16)
#define IPSEC_DECR_LIFETIME_COUNTERS_CMD	5 | (IPSEC_MODULE << 16)
#define IPSEC_GET_SEQ_NUM_CMD				6 | (IPSEC_MODULE << 16)
#define IPSEC_FRAME_DECRYPT_CMD				7 | (IPSEC_MODULE << 16)
#define IPSEC_FRAME_ENCRYPT_CMD 			8 | (IPSEC_MODULE << 16)
#define IPSEC_FRAME_ENCR_DECR_CMD 		   16 | (IPSEC_MODULE << 16)

// Temporary workaround commands
#define IPSEC_RUN_DESC_DEBUG 				9 | (IPSEC_MODULE << 16)

/** \addtogroup AIOP_FMs_Verification
 *  @{
 */

/**************************************************************************//**
 @Group		AIOP_GRO_Verification

 @Description	AIOP GRO Verification structures definitions.

 @{
*//***************************************************************************/

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
@Description	IPsec Add SA Descriptor Command structure.

*//****************************************************************************/
struct ipsec_add_sa_descriptor_command {
	uint32_t opcode;
	struct ipsec_descriptor_params params;
	//uint32_t ipsec_handle_ptr; /* pointer of descriptor handle */
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */
	
	uint64_t descriptor_addr; /* descriptor address */
    uint8_t outer_ip_header[80]; /* outer IP header */
    
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
	//uint32_t ipsec_handle_ptr; /* pointer of descriptor handle */
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */

	uint64_t kilobytes;
	uint64_t packets;
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
	//uint32_t ipsec_handle_ptr; /* pointer of descriptor handle */
	uint32_t sa_desc_id; /* Descriptor ID, of handles array in shared RAM */
	
	uint32_t kilobytes_decr_val;
	uint32_t packets_decr_val;
		
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



uint16_t  aiop_verification_ipsec(uint32_t data_addr);

/** @} */ /* end of AIOP_GRO_Verification */

/** @}*/ /* end of AIOP_FMs_Verification */

#endif /* __AIOP_VERIFICATION_IPSEC_H_ */
