/**************************************************************************//**
@File		ipsec.h

@Description	This file contains the AIOP IPSec 
		internal functions and definitions.
		
*//***************************************************************************/

#ifndef __AIOP_IPSEC_H
#define __AIOP_IPSEC_H

#include "common/types.h"


/**************************************************************************//**
 @Group		NETF NETF (Network Libraries)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
@Group	FSL_IPSEC IPSEC

@Description	Freescale AIOP IPsec API

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		IPSEC_ENUM IPsec Enumerations

 @Description	IPsec Enumerations

 @{
*//***************************************************************************/

enum ipsec_cipher_type {
	CIPHER_TYPE_CBC = 0, 	/* CBC */
	CIPHER_TYPE_CTR, /* CTR */
	CIPHER_TYPE_CCM, /* CCM */
	CIPHER_TYPE_GCM /* GCM */
};


/* @} end of IPSEC_ENUM */

/**************************************************************************//**
@Group	FSL_IPSEC_MACROS IPsec Macros

@Description	Freescale IPsec Macros

@{
*//***************************************************************************/

#define IPSEC_PROFILE_SRAM_ADDR 0x00030000 /* hard wired address */
#define IPSEC_STORAGE_PROFILE_SIZE_SHIFT 5 /* 32 bytes */
#define IPSEC_INTERNAL_PARMS_SIZE 128 /* 128 bytes */
#define IPSEC_FLOW_CONTEXT_SIZE 64 /* 64 bytes */
#define IPSEC_ENC_PDB_HMO_MASK 0xFF00
#define IPSEC_DEC_PDB_HMO_MASK 0xF000
#define IPSEC_PDB_OPTIONS_MASK 0x00FF

/* OPTIONS[3:2] - OIHI: Outer IP Header Included 
 * 00 : No Outer IP Header provided
 * 01 : First PDB:Opt IP Hdr Len bytes of Input frame is the
 * 		Outer IP Header Material to be included in Output Frame
 * 10 : PDB contains address to Outer IP Header Material to be
 * 		included in Output Frame (length is PDB:Opt IP Hdr Len
*/
#define IPSEC_ENC_PDB_OPTIONS_OIHI_PDB 0x04

/* 28 (HMO 4 out of 7:0) Sequence Number Rollover control. 
 * 0 : Sequence Number Rollover causes an error
 * 1 : Sequence Number Rollover permitted
*/
#define IPSEC_ENC_PDB_HMO_SNR 0x10


#define IPSEC_ARS_MASK	0x00c0   /* anti-replay window option mask */
#define IPSEC_ESN_MASK 0x10 /* Extended sequence number option mask */
#define IPSEC_SEC_NEW_BUFFER_MODE 0
#define IPSEC_SEC_REUSE_BUFFER_MODE 1

#define IPSEC_SA_DESC_BUF_SIZE 512 /* SA descriptor buffer size */
#define IPSEC_SA_DESC_BUF_ALIGN 64 /* SA descriptor alignment */
#define IPSEC_KEY_BUF_SIZE 512 /* Key buffer size */
#define IPSEC_MAX_NUM_OF_TASKS 256 /* Total maximum number of tasks in AIOP */
#define IPSEC_MEM_PARTITION_ID MEM_PART_1ST_DDR_NON_CACHEABLE 
					/* Memory partition ID */
#define IPSEC_MAX_ASA_SIZE 960 /* Maximum ASA size (960 bytes) */
#define IPSEC_MAX_ASA_BUF_ALIGN 8 /* ASA buffer alignment */

/*
* Big-endian systems are systems in which the most significant byte of the word 
* is stored in the smallest address given and the least significant byte 
* is stored in the largest. 
* In contrast, little endian systems are those in which the 
* least significant byte is stored in the smallest address.
*/
/* Little Endian
 Register:
* +---------------------------------------------------------------+
* |   A   |   B   |   C   |   D   |   E   |   F   |   G   |   H   |
* |  MSB  |       |       |       |       |       |       |  LSB  |
* +---------------------------------------------------------------+
* Bytes address
* |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
* +---------------------------------------------------------------+

Big Endian
 Register:
* +---------------------------------------------------------------+
* |   H   |   G   |   F   |   E   |   D   |   C   |   B   |   A   |
* |  LSB  |       |       |       |       |       |       |  MSB  |
* +---------------------------------------------------------------+
* Bytes address
* |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
* +---------------------------------------------------------------+
*/

/* FLC[63:0] = { 16’b0, checksum[15:0], byte_count[31:0] } */
/* FLC Little Endian Format 
* +---------------------------------------------------------------+
* |   x   |   x   |  CS1  |  CS0  |  BC3  |  BC2  |  BC1  |  BC0  |
* |       |       |  MSB  |  LSB  |  MSB  |       |       |  LSB  |
* +---------------------------------------------------------------+
* Byte address
* |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
* +---------------------------------------------------------------+

* FLC Big Endian Format 
* +---------------------------------------------------------------+
* |  BC0  |  BC1  |  BC2  |  BC3  |  CS1  |  CS0  |   x   |   x   |
* |  LSB  |       |  MSB  |  MSB  |  LSB  |  MSB  |       |       |
* +---------------------------------------------------------------+
* Byte address
* |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
* +---------------------------------------------------------------+
*/
/* FLC Checksum Little Endian Mask */
#define IPSEC_RETURN_FLC_CHECKSUM_LE_MASK 0x0000FFFF00000000
/* FLC Checksum Big Endian Mask */
#define IPSEC_RETURN_FLC_CHECKSUM_BE_MASK 0x00000000FFFF0000
/* FLC Byte Count Big Endian Mask */
#define IPSEC_RETURN_FLC_BCNT_BE_MASK 0xFFFFFFFF00000000

/* FLC Checksum Big Endian Shift */
#define IPSEC_RETURN_FLC_CHECKSUM_BE_SHIFT 16
/* FLC Byte Count Big Endian Shift */
#define IPSEC_RETURN_FLC_BCNT_BE_SHIFT 32

/* AAP Command Fields */
#define  IPSEC_AAP_USE_FLC_SP 0x10000000
#define  IPSEC_AAP_OS_EX 0x00800000

// TODO: temporary, need to update general.h
/** AAP SEC accelerator ID  (according to Archdef 7.5)*/
#define AAP_SEC_ACCEL_ID	0x03

/* DPOVRD OVRD */
#define IPSEC_DPOVRD_OVRD 0x80000000


// TMP, removed from the external API
/** Frames do not include a L2 Header */
#define IPSEC_FLG_NO_L2_HEADER		0x00000010

/* Output IP header source options. Use one of the following options. 
 * Relevant for tunnel mode only */
#define IPSEC_ENC_OPTS_IPHDR_SRC_NONE	0x00 /* IP header not included */
#define IPSEC_ENC_OPTS_IPHDR_SRC_FRAME 	0x04 /* IP header from input frame */
#define IPSEC_ENC_OPTS_IPHDR_SRC_ADDR	0x08 /* IP header ref from parameters */
#define IPSEC_ENC_OPTS_IPHDR_SRC_PARAM	0x0c /* IP header from parameters */

/**************************************************************************//**
@Description	IPSec ESP Encapsulation HMO field  

		Use for ipsec_encap_params.hmo
*//***************************************************************************/

// TMP, removed from the external API
/* SNR: Sequence Number Rollover control 
 * If not set, a Sequence Number Rollover causes an error
 * if set, Sequence Number Rollover is permitted*/
#define IPSEC_HMO_ENCAP_SNR	0x01                             


/**************************************************************************//**
@Description	IPSec ESP Decapsulation HMO field  

		Use for ipsec_decap_params.hmo
*//***************************************************************************/

// TMP, removed from the external API
/* ODF: the DF bit in the IPv4 header in the output frame is replaced 
 * with the DFV value as shown below. 
 * Note: * Must not be set for IPv6 */
#define IPSEC_HMO_DECAP_ODF	0x08

/* DFV -- DF bit Value */
#define IPSEC_HMO_DECAP_DFV	0x04


/**************************************************************************//**
@Description	IPSec handle Type definition

*//***************************************************************************/
//typedef uint64_t ipsec_handle_t;


// TMP, removed from the external API
/**************************************************************************//**
 * @struct    ipsec_storage_params
 * @ingroup   ipsec_storage_params
 * @details   Container for IPsec descriptor storage parameters
*//***************************************************************************/
struct ipsec_storage_params {
	uint16_t sdid; /** Security domain ID */
	uint16_t spid; /** Storage profile ID of SEC output frame */
	uint8_t rife; /** Return input frame on error */
	uint8_t crid; /** Critical resource ID */
};


/*
struct ipsec_encap_params {
	// TMP, removed from the external API
	uint8_t ip_nh_offset; /** Next header offset used for transport mode */
/* */

//struct ipsec_decap_params {
//uint16_t ip_hdr_len; /* The length, in bytes, of the portion of the 
//			IP header that is not encrypted. */
// TODO: TBD if we need a fixed values for ip_hdr_len
//
//uint8_t ip_nh_offset; /* The location of the next header field within 
//			the IP header of the transport mode packet. */ 
// TODO: TBD if we need a fixed values for ip_nh_offset

/* Global Parameters structure */
struct ipsec_global_params {
	uint32_t sa_count; /* SA (descriptors) counter. Initialized to max number */
	uint16_t asa_bpid; /* Buffer pool ID for ASA copy */
	uint16_t desc_bpid; /* Buffer pool ID for the SA descriptor */
	uint8_t tmi; /* Timer Instance ID  */
	uint8_t spinlock; /* Spinlock indicator, for SA counter  */
};

/* Instance Parameters structure */
struct ipsec_instance_params {
	uint32_t sa_count; /* SA (descriptors) counter. Initialized to max number */
	uint16_t asa_bpid; /* Buffer pool ID for ASA copy */
	uint16_t desc_bpid; /* Buffer pool ID for the SA descriptor */
	uint8_t tmi_id; /* TMAN Instance ID  */
};


#define SAP_STATUS_SOFT_KB_EXPIRED			0x00000001
#define SAP_STATUS_HARD_KB_EXPIRED			0x00000002
#define SAP_STATUS_SOFT_PACKET_EXPIRED		0x00000004
#define SAP_STATUS_HARD_PACKET_EXPIRED		0x00000008
#define SAP_STATUS_SOFT_SEC_EXPIRED			0x00000010
#define SAP_STATUS_HARD_SEC_EXPIRED			0x00000020


/* SA Descriptor Parameter for Internal Usage */ 
/* Part 1 */
struct ipsec_sa_params_part1 {
	/* Always required, except timer callback */
	/* 2x4 + 2x2 + 4x1 = 8 + 4 + 4 = 16 bytes */
	uint32_t flags; /* 	transport mode, UDP encap, pad check, counters enable, 
					outer IP version, etc. 4B */
	uint32_t status; /* lifetime expiry, semaphores	4-8B */
	
	uint16_t udp_src_port; /* UDP source for transport mode. 2B */
	uint16_t udp_dst_port; /* UDP destination for transport mode. 2B */
	
	uint8_t valid; /* descriptor valid. 1B */
	uint8_t esn; /* Extended sequence number enabled. 1B */
	
	uint8_t anti_replay_size; /* none/32/64/128	1B */
	uint8_t sec_buffer_mode; /* new/reuse (for ASA copy). 1B */

	/* Required at Add descriptor and enc/dec */
	/* 6x8 = 48 bytes */
	uint64_t soft_byte_limit; /* soft byte count limit,	8 Bytes */
	uint64_t soft_packet_limit; /* soft packet limit, 8B */
	uint64_t hard_byte_limit; /* hard byte count limit, 8B */
	uint64_t hard_packet_limit; /* hard packet limit, 8B */
	uint64_t byte_counter; /* Encrypted/decrypted bytes counter, 8B */
	uint64_t packet_counter; /*	Packets counter, 8B */
	
	/* Total size = 16 + 48 = 64 bytes */
};
/* Part 2 */
struct ipsec_sa_params_part2 {
	
	/* Required at Add descriptor and timer callback */
	/* 2x8 + 3x4 = 16+12 = 28 bytes */
	uint64_t timestamp; /* TMAN timestamp in micro-seconds, 8 Bytes */
	uint64_t sec_callback_arg; /* SA handle used as argument for the 
								application callback function. 8B */
	uint32_t sec_callback_func; /* Application callback function, 
	 	 	 	 	to call upon a seconds lifetime expiry. 4B */
	
	uint32_t soft_tmr_handle; /* Soft seconds timer handle, 4B */
	uint32_t hard_tmr_handle; /* Hard seconds timer handle, 4B */
	
	/* Total size = 28 bytes */
};

/* Total size for part 1 + part 2 = 64+28 = 92 bytes */
/* Remaining = 128 - 92 = 36 bytes */
struct ipsec_sa_params {
		struct ipsec_sa_params_part1 sap1;
		struct ipsec_sa_params_part2 sap2;
};



/* SEC Flow Context Descriptor */ 
struct sec_flow_context {
	/* TODO: add actual fields.
	 * Note: little endian */
	uint32_t flow_context_word[16];
};

/* SEC Shared Descriptor Place Holder*/ 
struct sec_shared_descriptor {
	/* TODO: add actual fields.
	 * Note: little endian */
	uint8_t sd[256];
};


/* DPOVRD for Tunnel Encap mode */
struct dpovrd_tunnel_encap {
	uint8_t reserved; /* 7-0 Reserved */
	uint8_t aoipho; /* 13-8 AOIPHO */
					/* 14 Reserved */
					/* 15 OIMIF */
	uint16_t outer_material_length; /* 27-16 Outer IP Header Material Length */
									/* 30-28 Reserved */
									/* 31 OVRD */
};

/* DPOVRD for Tunnel Decap mode */
struct dpovrd_tunnel_decap {
	uint32_t word;
	/* 31 OVRD
	 * 30-20 Reserved
	 * 19-12 AOIPHO
	 * 11-0 Outer IP Header Material Length */
};




struct dpovrd_general {
	union {
		struct dpovrd_tunnel_decap tunnel_decap;
	};
};



/* SEC Flow Context Descriptor */

struct ipsec_flow_context {
	/* word 0 */
	uint16_t word0_sdid; /* 11-0  SDID */
	uint16_t word0_res; /* 31-12 reserved */
	
	/* word 1 */
	uint8_t word1_sdl; 	/* 5-0 SDL 	*/
						/* 7-6 reserved */
	
	uint8_t word1_bits_15_8; 	/* 11-8 CRID */
								/* 14-12 reserved */
								/* 15 CRJD */
	
	uint8_t word1_bits23_16; /* 16	EWS */
								/* 17 DAC */
								/* 18,19,20 ? */
								/* 23-21 reserved */

	uint8_t word1_bits31_24; /* 24 RSC */
							/* 25 RBMT */
	 	 	 	 	 	 	 /* 31-26 reserved */
	
	/* word 2  RFLC[31-0] */
	uint32_t word2_rflc_31_0;

	/* word 3  RFLC[63-32] */
	uint32_t word3_rflc_63_32;

	/* word 4 */
	uint16_t word4_iicid; /* 15-0  IICID */
	uint16_t word4_oicid; /* 31-16 OICID */
	
	/* word 5 */
	uint8_t word5_7_0; /* 23-0 OFQID */
	uint8_t word5_15_8;
	uint8_t word5_23_16;
	
	uint8_t word5_31_24; 
						/* 24 OSC */
						/* 25 OBMT */
						/* 29-26 reserved 	*/
						/* 31-30 ICR */
	
	//uint32_t word5_31_0; 
	
	/* word 6 */
	uint32_t word6_oflc_31_0;

	/* word 7 */
	uint32_t word7_oflc_63_32;
	 
	/* words 8-15 are a copy of the standard storage profile */
	uint64_t storage_profile[4];
};

//-------------------------------------------------------------------------
// 							Obsolete Structures
//-------------------------------------------------------------------------

/* Obsolete SEC Flow Context Descriptor (bit fields)*/
struct obsolete_flow_context {
	/* word 0 */
	uint32_t word0_res: 20; /* 31-12 reserved */
	uint32_t word0_sdid:12; /* 11-0  SDID */
	
	/* word 1 */
	uint32_t word1_reserved31_26: 6; /* 31-26 reserved (6) 	*/
	uint32_t word1_rbmt 		: 1; /* 25 RBMT (1)	*/
	uint32_t word1_rsc	 		: 1; /* 24 RSC (1)	*/
	uint32_t word1_reserved23_21: 3; /* 23-21 reserved (3)	*/
	uint32_t word1_bit20 		: 1; /* 20 	?	*/
	uint32_t word1_bit19 		: 1; /* 19 	?	*/
	uint32_t word1_bit18 		: 1; /* 18 	?	*/
	uint32_t word1_dac	 		: 1; /* 17 	DAC (1)	*/
	uint32_t word1_ews	 		: 1; /* 16 	EWS (1)	*/
	uint32_t word1_crjd	 		: 1; /* 15 	CRJD (1)	*/
	uint32_t word1_reserved14_12: 3; /* 14-12 reserved (3)	*/
	uint32_t word1_crid	 		: 4; /* 11-8 CRID (4)	*/
	uint32_t word1_reserved7_6	: 2; /* 7-6 reserved (2)	*/
	uint32_t word1_sdl	 		: 6; /* 5-0 SDL (6)	*/
	
	/* word 2  RFLC[31-0] */
	uint32_t word2_rflc_31_0;

	/* word 3  RFLC[63-32] */
	uint32_t word3_rflc_63_32;

	/* word 4 */
	uint32_t word4_oicid 	: 16; /* 31-16 OICID */
	uint32_t word4_iicid 	: 16; /* 15-0  IICID */
	
	/* word 5 */
	uint32_t word5_icr 			: 2; /* 31-30 ICR */
	uint32_t word5_reserved29_26: 4; /* 29-26 reserved 	*/
	uint32_t word5_obmt			: 1;  /* 25 OBMT */
	uint32_t word5_osc			: 1;  /* 24 OSC */
	uint32_t word5_ofqid 		: 24; /* 23-0 OFQID */

	/* word 6 */
	uint32_t word6_oflc_31_0;

	/* word 7 */
	uint32_t word7_oflc_63_32;
	 
	/* word 8 */
	uint32_t word8_reserved31_16 	: 16; /* 31-16 reserved */
	uint32_t word8_dl 				: 16; /* 15-0  DL */
	
	/* word 9 */
	uint32_t word9_bs 				: 1; /* 31 BS */
	uint32_t word9_rsv30 			: 1; /* 30 rsv */
	uint32_t word9_ff 				: 2; /* 29-28 FF */
	uint32_t word9_rsv27			: 1; /* 27 rsv */
	uint32_t word9_va				: 1; /* 26 VA */
	uint32_t word9_rsv25			: 1; /* 25 rsv */
	uint32_t word9_dlc				: 1; /* 24 DLC */
	uint32_t word9_ptar				: 1; /* 23 DLC */
	uint32_t word9_rsv22_21			: 2; /* 22-21 rsv */
	uint32_t word9_sghr				: 1; /* 20 SGHR */
	uint32_t word9_asar				: 4; /* 19-16 ASAR */
	uint32_t word9_rsv15_12			: 2; /* 15-12 rsv */
	uint32_t word9_dhr				: 12; /* 11-0 DHR */

	/* word 10 */
	uint32_t word10_bmt0 			: 1; /* 31 BMT0 */
	uint32_t word10_rsv30 			: 1; /* 30 rsv */
	uint32_t word10_bpid0 			: 14; /* 29-16 BPID0 */
	uint32_t word10_rsv15_6 		: 10; /* 15-6 rsv */
	uint32_t word10_sr0 			: 1; /* 5 SR0 */
	uint32_t word10_rsv4_1 			: 4; /* 4-1 rsv */
	uint32_t word10_bpv0 			: 1; /* 0 BPV0 */

	/* word 11 */
	uint32_t word11_bmt1 			: 1; /* 31 BMT1 */
	uint32_t word11_rsv30 			: 1; /* 30 rsv */
	uint32_t word11_bpid1 			: 14; /* 29-16 BPID1 */
	uint32_t word11_rsv15_6 		: 10; /* 15-6 rsv */
	uint32_t word11_sr1 			: 1; /* 5 SR1 */
	uint32_t word11_rsv4_1 			: 4; /* 4-1 rsv */
	uint32_t word11_bpv1 			: 1; /* 0 BPV1 */
	
	/* words 12-15 reserved */
	uint32_t word12_reserved;
	uint32_t word13_reserved;
	uint32_t word14_reserved;
	uint32_t word15_reserved;
};


/** @} */ /* end of FSL_IPSEC_STRUCTS */


/**************************************************************************//**
@Group		FSL_IPSEC_Functions IPsec Functions

@Description	Freescale AIOP IPsec Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function	ipsec_init

@Description	This function performs reservation of:
		1. Maximum number of IPsec SA buffers.
		2. The maximum number of buffers needed for ASA copying.
		
		These parameters are saved in the shared RAM.
		
@Param[in]	max_sa_no - Max. number of SAs.

@Return		Status

*//****************************************************************************/
int32_t ipsec_init(uint32_t max_sa_no);

/**************************************************************************//**
@Function		ipsec_generate_flc 

@Description	Generate SEC Flow Context Descriptor
*//***************************************************************************/
int32_t ipsec_generate_flc(
		uint64_t flc_address, /* Flow Context Address in external memory */
		uint16_t spid, /* Storage Profile ID of the SEC output frame */
		uint32_t sd_size /* Shared descriptor Length */
);

/**************************************************************************//**
@Function		ipsec_generate_sd 

@Description	Generate SEC Shared Descriptor for Encapsulation
*//***************************************************************************/
int32_t ipsec_generate_encap_sd(
		uint64_t sd_addr, /* Flow Context Address in external memory */
		struct ipsec_descriptor_params *params,
		uint32_t *sd_size /* Shared descriptor Length */
);

/**************************************************************************//**
@Function		ipsec_generate_sd 

@Description	Generate SEC Shared Descriptor for Decapsulation
*//***************************************************************************/
int32_t ipsec_generate_decap_sd(
		uint64_t sd_addr, /* Flow Context Address in external memory */
		struct ipsec_descriptor_params *params,
		uint32_t *sd_size /* Shared descriptor Length */
);


/**************************************************************************//**
@Function		ipsec_generate_sa_params 

@Description	Generate and store the functional module internal parameter
*//***************************************************************************/
int32_t ipsec_generate_sa_params(
		ipsec_handle_t *ipsec_handle, /* Parameters area (start of buffer) */
		struct ipsec_descriptor_params *params);

/**************************************************************************//**
	 
******************************************************************************	

/*


/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */
/** @} */ /* end of NETF */


#endif /* __AIOP_IPSEC_H */
