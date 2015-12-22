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

/**************************************************************************//**
@File		ipsec.h

@Description	This file contains the AIOP IPSec 
		internal functions and definitions.
		
*//***************************************************************************/

#ifndef __AIOP_IPSEC_H
#define __AIOP_IPSEC_H

#include "fsl_types.h"
#include "fsl_net.h"

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
 * @Description   Indicates how is the data provided and how to include it in
 *                the descriptor.
*//***************************************************************************/
enum rta_param_type {
	RTA_PARAM_PTR = 1,  /**< Data is in memory and accessed by reference;
				 data address is a physical (bus) address */
	RTA_PARAM_IMM, /**< Data is inlined in descriptor and accessed as
			    immediate data; data address is a virtual address */
	RTA_PARAM_IMM_DMA /**< (AIOP only) Data is inlined in descriptor and
			       accessed as immediate data; data address is a
			       physical (bus) address in external memory and
			       CDMA is programmed to transfer the data into
			       descriptor buffer being built in WS Area */
};

/**************************************************************************//**
@Group	FSL_IPSEC_MACROS IPsec Macros

@Description	Freescale IPsec Macros

@{
*//***************************************************************************/

#ifdef IPSEC_HOT_CODE
	#define __IPSEC_HOT_CODE __HOT_CODE
#else
	#define __IPSEC_HOT_CODE 
#endif


/* Internal Flags */
/* flags[31] : 1 = outbound, 0 = inbound */
#define IPSEC_FLG_DIR_OUTBOUND 0x80000000 
/* flags[30] : 1 = IPv6, 0 = IPv4 (useed for transport mode) */
#define IPSEC_FLG_IPV6 0x40000000 
/* flags[29] : 1 = IPv6, 0 = IPv4 (useed for tunnel mode) */
#define IPSEC_FLG_OUTER_HEADER_IPV6 0x20000000 

/** Preserve the ASA (Accelerator Specific Annotation) */
/* Obsolete, Not supported */
/* #define IPSEC_FLG_PRESERVE_ASA		0x00020000 */

#define IPSEC_IP_VERSION_MASK 0xF0000000 
#define IPSEC_IP_VERSION_IPV6 0x60000000 
#define IPSEC_ETHERTYPE_IPV6 0x86DD 
#define IPSEC_ETHERTYPE_IPV4 0x0800 

/* IPv4 header checksum is in bytes 10-11, lower 32-bit word #2 */
#define IPSEC_OUTER_HEADER_IPV_MASK 0xF0000000
#define IPSEC_OUTER_HEADER_IPV4     0x40000000
#define IPSEC_OUTER_HEADER_CHECKSUM_MASK 0xFFFF0000

/* IPv4 DSCP, bits 8-13 */
#define IPSEC_DSCP_MASK_IPV4 0x00FC0000
/* IPv6 DSCP, bits 4-9 */
#define IPSEC_DSCP_MASK_IPV6 0x0FC00000

#define IPSEC_IPV4_CHECKSUM_OFFSET 10

/* PS Pointer Size. This bit determines the size of address pointers */
#define IPSEC_SEC_POINTER_SIZE 1 /* 1 - SEC Pointers require two 32-bit words */ 

#define IPSEC_PROFILE_SRAM_ADDR 0x00030000 /* hard wired address */
#define IPSEC_STORAGE_PROFILE_SIZE_SHIFT 5 /* 32 bytes */
#define IPSEC_INTERNAL_PARMS_SIZE 128 /* 128 bytes */
#define IPSEC_FLOW_CONTEXT_SIZE 64 /* 64 bytes */
#define IPSEC_KEY_SEGMENT_SIZE 128 /* Key Copy segment, 128 bytes */
#define IPSEC_CIPHER_KEY_SIZE 32 /* Cipher Key Copy, 32 bytes */
#define IPSEC_DEBUG_SEGMENT_SIZE 32 /* Debug info, 32 bytes */

#define IPSEC_SP_ASAR_MASK 0x000F0000 /* Storage profile ASAR field mask */
#define IPSEC_SP_DHR_MASK 0x00000FFF
#define IPSEC_SP_REUSE_BS_FF 0xA0000000


#define IPSEC_ENC_PDB_HMO_MASK 0xF000
#define IPSEC_DEC_PDB_HMO_MASK 0xF000
#define IPSEC_PDB_OPTIONS_MASK 0x00FF
#define IPSEC_ENC_PDB_HMO_SHIFT 16
#define IPSEC_DEC_PDB_HMO_SHIFT 16

#define IPSEC_DECAP_PDB_ARS_MASK 0xC0 /* PDB options[ARS], bits 7:6 */

#define IPSEC_DEC_OPTS_ETU 	0x0001 /** ETU: EtherType Update 
* If set this has two implications: 
* - The first PDB:AOIPHO-2 bytes of the Outer IP Header Material 
* 	are copied to the output frame.validate IP header checksum 
* - The EtherType field is updated */

/* OPTIONS[3:2] - OIHI: Outer IP Header Included 
 * 00 : No Outer IP Header provided
 * 01 : First PDB:Opt IP Hdr Len bytes of Input frame is the
 * 		Outer IP Header Material to be included in Output Frame
 * 10 : PDB contains address to Outer IP Header Material to be
 * 		included in Output Frame (length is PDB:Opt IP Hdr Len  bytes)
 * 11 : PDB contains Outer IP Header Material to be included in Output Frame 
 * 		(length is PDB:Opt IP Hdr Len bytes)
*/
#define IPSEC_ENC_PDB_OPTIONS_OIHI_PDB 0x0C

//TODO: this was in fsl_ipsec.h, but probably not necessary, check if to remove
/** Add Output IP header to the frame
* Relevant for tunnel mode only */
#define IPSEC_ENC_OPTS_ADD_IPHDR	0x000c /* Add IP header */

/* Inc IPHdr - Include Optional IP Header, Prepend IP Header to output frame
 * Required for transport mode. Must be together with IPHdrSrc = 0 */
#define IPSEC_ENC_OPTS_INC_IPHDR	0x0004 /* Add IP header */

//TODO: this was in fsl_ipsec.h, but probably not necessary, check if to remove
/** Copy TOS field (IPv4) or Traffic-Class field (IPv6) from outer
 * IP header to inner IP header. Not valid for tunnel mode */
#define IPSEC_ENC_OPTS_DIFFSERV		0x0040

/** Perform computations to update header checksum for IPv4 header.
 * Not valid for tunnel mode */
#define IPSEC_ENC_OPTS_UPDATE_CSUM	0x0080

/** Perform checksum verification to IPv4 header in Transport mode.
 * Transport mode only. Not valid for tunnel mode */
#define IPSEC_DEC_OPTS_VERIFY_CSUM 	0x0020 /** validate IP header checksum */

/* NAT and NUC Options for tunnel mode encapsulation */
/* Bit 1 : NAT Enable RFC 3948 UDP-encapsulated-ESP */
/* Bit 0 : NUC Enable NAT UDP Checksum */
#define IPSEC_ENC_PDB_OPTIONS_NAT 0x02
#define IPSEC_ENC_PDB_OPTIONS_NUC 0x01

/* 28 (HMO 4 out of 7:0) Sequence Number Rollover control. 
 * 0 : Sequence Number Rollover causes an error
 * 1 : Sequence Number Rollover permitted
*/
#define IPSEC_ENC_PDB_HMO_SNR 0x10

/*
3 	OUT_FMT 	Output Frame format:
	0 - All Input Frame fields copied to Output Frame
	1 - Output Frame is just the decapsulated PDU
2 	AOFL 	Adjust Output Frame Length
	0 - Don't adjust output frame length 
	-- output frame length reflects output frame actually written to memory,
		including the padding, Pad Length, and Next Header fields.
	1 - Adjust output frame length -- subtract the length of the padding, 
		the Pad Length, and the Next Header
		byte from the output frame length reported to the frame consumer.
	If outFMT==0, this bit is reserved and must be zero.
*/
#define IPSEC_DEC_PDB_OPTIONS_AOFL		0x04
#define IPSEC_DEC_PDB_OPTIONS_OUTFMT	0x08

#define IPSEC_ARS_MASK	0x00c0   /* anti-replay window option mask */
#define IPSEC_ESN_MASK 0x10 /* Extended sequence number option mask */
#define IPSEC_SEC_NEW_BUFFER_MODE 0
#define IPSEC_SEC_REUSE_BUFFER_MODE 1
#define IPSEC_FMT_SINGLE_BUFFER 0

/**************************************************************************//**
@Description	IPSec Key Encryption Flags (currently unsupported)
 To be set to the alg_info.key_enc_flags field
*//***************************************************************************/
#define IPSEC_KEY_ENC			0x00400000
	/**< ENC: Encrypted - Key is encrypted either with the KEK, or
	 * 	with the TDKEK if this descriptor is trusted */
#define IPSEC_KEY_NWB			0x00200000
	/**< NWB: No Write Back - Do not allow key to be FIFO STOREd */
#define IPSEC_KEY_EKT			0x00100000
	/**< EKT: Enhanced Encryption of Key */
#define IPSEC_KEY_TK			0x00008000
	/**< TK: Encrypted with Trusted Key */

/*                 SA Descriptor Structure
 * ------------------------------------------------------
 * | ipsec_sa_params                  | 128 bytes       | + 0
 * ------------------------------------------------------
 * | sec_flow_context                 | 64 bytes        | + 128
 * -----------------------------------------------------
 * | sec_shared_descriptor            | Up to 256 bytes | + 192
 * ------------------------------------------------------
 * | Replacement Job Descriptor (TBD) | Up to 64 (TBD)  | + 448
 * ------------------------------------------------------
 * | Authentication Key Copy          | 128 bytes       | + 512 
 * ------------------------------------------------------
 * | Cipher Key Copy                  | 32 bytes        | + 640 
 * ------------------------------------------------------
 * | Debug/Error information          | 32 bytes        | + 672 
 * ------------------------------------------------------ 
 *  
 * ipsec_sa_params - Parameters used by the IPsec functional module	128 bytes
 * sec_flow_context	- SEC Flow Context. 64 bytes
 * 			Should be 64-byte aligned for optimal performance.	
 * sec_shared_descriptor - Shared descriptor. Up to 256 bytes
 * Replacement Job Descriptor (RJD) for Peer Gateway Adaptation 
 * (Outer IP change)	TBD 
 * Key Copy area, for CAAM DKP and upon HF-NIC requirement
 * 
 * Aligned Buffer size = 128 + 64 + 256 + 64 + 128 + 32 + 32 = 704
 * Requested buffer size = 12*64 = 768 bytes
*/

#define IPSEC_SA_DESC_BUF_SIZE 768 /* SA descriptor buffer size */
#define IPSEC_SA_DESC_BUF_ALIGN 64 /* SA descriptor alignment */
#define IPSEC_BUF_META_DATA_SIZE 8
//#define IPSEC_ALIGN_OFFSET IPSEC_SA_DESC_BUF_ALIGN - IPSEC_BUF_META_DATA_SIZE
//	/* The slab buffer handle alignment is at requested alignment + meta-data */
#define IPSEC_ALIGN_OFFSET 0
	/* After the slab change, the slab returned buffer handle is aligned */

#define IPSEC_MAX_NUM_OF_TASKS 256 /* Total maximum number of tasks in AIOP */

/* Memory partition ID */
#ifndef IPSEC_PRIMARY_MEM_PARTITION_ID
	#define IPSEC_PRIMARY_MEM_PARTITION_ID MEM_PART_DP_DDR
#endif

#ifndef IPSEC_SECONDARY_MEM_PARTITION_ID
	#define IPSEC_SECONDARY_MEM_PARTITION_ID MEM_PART_SYSTEM_DDR
#endif

/* Keys Copy Offsets */
#define IPSEC_KEY_SEGMENT_OFFSET 512 /* Offset from params start */
#define IPSEC_CIPHER_KEY_OFFSET\
		IPSEC_KEY_SEGMENT_OFFSET + IPSEC_KEY_SEGMENT_SIZE

#define IPSEC_DEBUG_INFO_OFFSET\
		IPSEC_CIPHER_KEY_OFFSET + IPSEC_CIPHER_KEY_SIZE

/* Key Offset from FLC start */
#define IPSEC_KEY_OFFSET_FROM_FLC IPSEC_KEY_SEGMENT_OFFSET -\
											IPSEC_INTERNAL_PARMS_SIZE

/* Key Offset from Shared Descriptor start */
#define IPSEC_KEY_OFFSET_FROM_SD IPSEC_KEY_SEGMENT_OFFSET -\
											IPSEC_INTERNAL_PARMS_SIZE -\
											IPSEC_FLOW_CONTEXT_SIZE

/* Obsolete, ASA preservation not supported */
/* #define IPSEC_MAX_ASA_SIZE 960 */ /* Maximum ASA size (960 bytes) */
/* #define IPSEC_MAX_ASA_BUF_ALIGN 8 */ /* ASA buffer alignment */

/**< Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
#define IPSEC_ALIGN_64(ADDRESS, ALIGNMENT)           \
        ((((uint64_t)(ADDRESS)) + ((uint64_t)(ALIGNMENT)) - 1) & \
        								(~(((uint64_t)(ALIGNMENT)) - 1)))

#define IPSEC_DESC_ALIGN(ADDRESS) \
	IPSEC_ALIGN_64((ADDRESS), IPSEC_SA_DESC_BUF_ALIGN)

/* The IPsec data structure should be aligned to 64 bytes (for CAAM) */
#if (IPSEC_SA_DESC_BUF_ALIGN != 64)
	/* Aligned Descriptor Address (parameters area start) */
	#define IPSEC_DESC_ADDR(ADDRESS) IPSEC_DESC_ALIGN(ADDRESS)
#else
	#define IPSEC_DESC_ADDR(ADDRESS) (ADDRESS + IPSEC_ALIGN_OFFSET)
#endif

/* Flow Context Address */
#define IPSEC_FLC_ADDR(ADDRESS) ((ADDRESS) + IPSEC_INTERNAL_PARMS_SIZE)

/* Shared Descriptor Address */
#define IPSEC_SD_ADDR(ADDRESS) \
	((ADDRESS) + IPSEC_INTERNAL_PARMS_SIZE + IPSEC_FLOW_CONTEXT_SIZE)

/* Key Copy Segment */
#define IPSEC_KEY_SEGMENT_ADDR(ADDRESS) ((ADDRESS) + IPSEC_KEY_SEGMENT_OFFSET)
#define IPSEC_KEY_ADDR_FROM_FLC(ADDRESS) ((ADDRESS) + IPSEC_KEY_OFFSET_FROM_FLC)
#define IPSEC_KEY_ADDR_FROM_SD(ADDRESS) ((ADDRESS) + IPSEC_KEY_OFFSET_FROM_SD)

#define IPSEC_CIPHER_KEY_ADDR(ADDRESS) ((ADDRESS) + IPSEC_CIPHER_KEY_OFFSET)

#define IPSEC_DEBUG_INFO_ADDR(ADDRESS) ((ADDRESS) + IPSEC_DEBUG_INFO_OFFSET)


#define IPSEC_GET_SEGMENT_ADDRESS(prc_addr) \
	((struct presentation_context *)prc_addr)->seg_address
#define IPSEC_GET_SEGMENT_OFFSET(prc_addr) \
	((struct presentation_context *)prc_addr)->seg_offset

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
#define IPSEC_DPOVRD_OVRD_TRANSPORT 0x80

/* DPOVRD Next Header for ESP */
/* 4 (0x04) IP-in-IP IP in IP (encapsulation) */
#define IPSEC_NEXT_HEADER_IPV4 0x04
/* 41 (0x29) IPv6 IPv6 Encapsulation */ 
#define IPSEC_NEXT_HEADER_IPV6 0x29

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
@Description	SEC Returned Status  

		Use for ipsec_decap_params.hmo
*//***************************************************************************/
/* SEC Job termination status/error word 
* bits 31-28      bits 3-0 / bits 7-0 
* (Source of      (ERRID)  / (Error Code)
*  the status 
*  code)
* -----------     ---------
* 2h (CCB)	    Ah - ICV check failed
* -----------     ---------
* 4h (DECO)		83h - Anti-replay LATE error
*				84h - Anti-replay REPLAY error
*				85h - Sequence number overflow
*/
/** No Error */
#define	SEC_NO_ERROR 0x00000000

#define	SEC_COMPRESSED_ERROR 0x83000000
#define	SEC_COMPRESSED_ERROR_MASK 0xFF000000

/** ICV comparison failed */
#define	SEC_ICV_COMPARE_FAIL 0x2000000A
#define	SEC_ICV_COMPARE_FAIL_COMPRESSED 0x8320000A

/** Anti Replay Check: Late packet */
#define	SEC_AR_LATE_PACKET 0x40000083
#define	SEC_AR_LATE_PACKET_COMPRESSED 0x83400083

/** Anti Replay Check: Replay packet */
#define	SEC_AR_REPLAY_PACKET 0x40000084
#define	SEC_AR_REPLAY_PACKET_COMPRESSED 0x83400084

/** Sequence Number overflow */
#define	SEC_SEQ_NUM_OVERFLOW 0x40000085
#define	SEC_SEQ_NUM_OVERFLOW_COMPRESSED 0x83400085

#define	SEC_CCB_ERROR_MASK 0xF000000F
#define	SEC_DECO_ERROR_MASK 0xF00000FF

#define	SEC_CCB_ERROR_MASK_COMPRESSED  0xFFF0000F
#define	SEC_DECO_ERROR_MASK_COMPRESSED 0xFFF000FF

/* Debug error codes */
#define IPSEC_INT_ERR_PAD_TOO_LONG 0x00000001
		/* Crypto padding is longer than presentation */
#define IPSEC_INT_TRANSPORT_PAD_CHECK_ERR 0x00000002
	/* Transport mode padding check error */

/* OSM temporary defines */
/* TODO: should move to general or OSM include file */
#define IPSEC_OSM_CONCURRENT			0
#define IPSEC_OSM_EXCLUSIVE				1

/* General Headers Parameters */
#define IPSEC_IP_NEXT_HEADER_UDP 0x11 /* UDP = 17 */
#define IPSEC_IP_NEXT_HEADER_ESP 0x32 /* ESP = 50 */ 

/* Max job descriptor size in bytes (13 words) */
#define IPSEC_MAX_AI_JOB_DESC_SIZE ((7 * CAAM_CMD_SZ) + (3 * CAAM_PTR_SZ))

/* The max shared descriptor size in 32 bit words when using the AI is 
 * 64 words - 13 words reserved for the Job descriptor */
#define IPSEC_MAX_SD_SIZE_WORDS (64-13) 

/* Total max growth for encapsulation (not including outer IP/UDP header):	
 * 4-byte SPI, 4-byte Seq Num, 16-byte IV, 15-byte Padding (AES),
 * 1-byte pad length, 1-byte Next Header, 32-byte ICV (SHA 512)	
 * = Total: 73, rounded up to 76 */
#define IPSEC_MAX_FRAME_GROWTH 76 


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

/* Instance Parameters structure */
struct ipsec_instance_params {
	uint32_t sa_count; /* SA (descriptors) counter. Initialized to max number */
	uint32_t committed_sa_num; /* Committed SAs (descriptors) */
	uint32_t max_sa_num; /* Maximum SAs (descriptors) */
	uint32_t instance_flags; /* Flags place holder */
	uint16_t outer_ip_bpid; /* Buffer pool ID outer IP header (TBD) */
	uint16_t desc_bpid; /* Buffer pool ID for the SA descriptor */
	uint8_t tmi_id; /* TMAN Instance ID  */
};

/* Note: For ste_inc_and_acc_counters function, the accumulator memory address 
 * should be counter_addr + sizeof(counter) 
 * In this case "accumulator" = byte counter, "counter" = packets counter*/

#define IPSEC_PACKET_COUNTER_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params_part1, packet_counter)))

#define IPSEC_KB_COUNTER_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params_part1, byte_counter)))

#define IPSEC_FLAGS_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params_part1, flags)))

#define IPSEC_STATUS_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params_part1, status)))

#define IPSEC_INSTANCE_HANDLE_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params_part1, instance_handle)))

#define IPSEC_SOFT_SEC_EXPIRED_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params_part1, soft_sec_expired)))

#define IPSEC_HARD_SEC_EXPIRED_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params_part1, hard_sec_expired)))

#define IPSEC_SOFT_SEC_LIMIT_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params, sap2.soft_seconds_limit)))

#define IPSEC_HARD_SEC_LIMIT_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params, sap2.hard_seconds_limit)))

#define IPSEC_SA_PARAMS_2_ADDR(ADDRESS) \
	(ADDRESS + (offsetof(struct ipsec_sa_params, sap2)))

/* Shared descriptor address */
#define IPSEC_SHARED_DESC_ADDR(ADDRESS) (ADDRESS + \
	IPSEC_INTERNAL_PARMS_SIZE + IPSEC_FLOW_CONTEXT_SIZE)

/* PDB address */
#define IPSEC_PDB_ADDR(ADDRESS) (IPSEC_SHARED_DESC_ADDR(ADDRESS) + 4) 
	
#define IPSEC_MAX_TIMESTAMP 0xFFFFFFFFFFFFFFFF

/* duration - Timer duration time (the number of timer ticks).
 * The duration must have a value larger than 10 ticks and smaller
 * than 2^16-10 ticks. */
#define IPSEC_MAX_TIMER_DURATION (0x10000-10-2)
#define IPSEC_MIN_TIMER_DURATION 10

/* SA Descriptor Parameter for Internal Usage */ 
/* Part 1 */
struct ipsec_sa_params_part1 {
	/* Required at Add descriptor and enc/dec */
	/* 6x8 = 48 bytes */
	uint64_t packet_counter; /*	Packets counter, 8B */
	uint64_t byte_counter; /* Encrypted/decrypted bytes counter, 8B */
	uint64_t timestamp; /* TMAN timestamp in micro-seconds, 8 Bytes */

	uint64_t soft_byte_limit; /* soft byte count limit,	8 Bytes */
	uint64_t soft_packet_limit; /* soft packet limit, 8B */
	uint64_t hard_byte_limit; /* hard byte count limit, 8B */
	uint64_t hard_packet_limit; /* hard packet limit, 8B */
	
	/* Always required, except timer callback */
	ipsec_instance_handle_t instance_handle; /* Instance handle 8B */

	uint32_t flags; /* 	transport mode, UDP encap, pad check, counters enable, 
					outer IP version, etc. 4B */
	//uint32_t status; /* lifetime expire, semaphores	4-8B */


	uint32_t outer_hdr_dscp; /* Outer Header DSCP, for set mode */
	
	uint16_t udp_src_port; /* UDP source for transport mode. 2B */
	uint16_t udp_dst_port; /* UDP destination for transport mode. 2B */
	uint16_t bpid; /* BPID of output frame in new buffer mode */
	uint16_t encap_header_length; /* Encapsulated IP+ESP header length */

	uint8_t valid; /* descriptor valid. 1B */
	uint8_t sec_buffer_mode; /* new/reuse. 1B */
	uint8_t output_spid; /* SEC output buffer SPID */
	
	uint8_t soft_sec_expired; /* soft seconds lifetime expired */
	uint8_t hard_sec_expired; /* hard seconds lifetime expired */

	/* Total size = */
	/* 8*8 (64) + 3*4 (12) + 4*2 (8) + 3*1 (3) = 87 bytes */
	/* Aligned size = 88 bytes */
};
/* Part 2 */
struct ipsec_sa_params_part2 {
	
	/* Required at Add descriptor and timer callback */
	/* 2x8 + 3x4 = 16+12 = 28 bytes */
	uint64_t sec_callback_arg; /* SA handle used as argument for the 
								application callback function. 8B */
	ipsec_lifetime_callback_t *sec_callback_func; /* Application callback function, 
	 	 	 	 	to call upon a seconds lifetime expiry. 4B */
	
	/* Do not change this order of limit followed by handle */
	uint32_t soft_seconds_limit; 
	uint32_t soft_tmr_handle; /* Soft seconds timer handle, 4B */

	/* Do not change this order of limit followed by handle */
	uint32_t hard_seconds_limit; 
	uint32_t hard_tmr_handle; /* Hard seconds timer handle, 4B */
	
	 
	uint8_t tmi_id;
	
	/* Total size = 8 + 5*4 (20) + 1 = 29 bytes */
	/* Aligned size = 32 bytes */
};

/* Total aligned size for part 1 + part 2 = 88 + 32 = 120 bytes */
/* Remaining = 128 - 120 = 8 bytes */
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
//struct dpovrd_tunnel_encap {
//	uint8_t reserved; /* 7-0 Reserved */
//	uint8_t aoipho; /* 13-8 AOIPHO */
//					/* 14 Reserved */
//					/* 15 OIMIF */
//	uint16_t outer_material_length; /* 27-16 Outer IP Header Material Length */
//									/* 30-28 Reserved */
//									/* 31 OVRD */
//};

struct dpovrd_tunnel_encap {
	uint32_t word;
};
	
/* DPOVRD for Tunnel Decap mode */
struct dpovrd_tunnel_decap {
	uint32_t word;
	/* 31 OVRD
	 * 30-20 Reserved
	 * 19-12 AOIPHO
	 * 11-0 Outer IP Header Material Length */
};

/* DPOVRD for Transport mode Encap  */
struct dpovrd_transport_encap {
	uint8_t ovrd;
	uint8_t ip_hdr_len;
	uint8_t nh_offset;
	uint8_t next_hdr;
};

/* DPOVRD for Transport mode Decap  */
struct dpovrd_transport_decap {
	uint8_t ovrd;
	uint8_t ip_hdr_len;
	uint8_t nh_offset;
	uint8_t reserved;
};


struct dpovrd_general {
	union {
		struct dpovrd_tunnel_encap tunnel_encap;
		struct dpovrd_tunnel_decap tunnel_decap;
		struct dpovrd_transport_encap transport_encap;
		struct dpovrd_transport_decap transport_decap;
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


/** @} */ /* end of FSL_IPSEC_STRUCTS */


/**************************************************************************//**
@Group		FSL_IPSEC_Functions IPsec Functions

@Description	Freescale AIOP IPsec Functions

@{
*//***************************************************************************/

/**************************************************************************//**
@Function		ipsec_generate_flc 

@Description	Generate SEC Flow Context Descriptor
*//***************************************************************************/
void ipsec_generate_flc(
		uint64_t flc_address, /* Flow Context Address in external memory */
		//uint16_t spid, /* Storage Profile ID of the SEC output frame */
		struct ipsec_descriptor_params *params, 
		int sd_size /* Shared descriptor Length */
);

/**************************************************************************//**
@Function		ipsec_create_key_copy 

@Description	Creates a copy of the key, used for CAAM DKP 
*//***************************************************************************/
void ipsec_create_key_copy(
		uint64_t src_key_addr, /*  Source Key Address */
		uint64_t dst_key_addr, /*  Destination Key Address */
		uint16_t keylen);   /* Length of the provided key, in bytes */

/**************************************************************************//**
@Function		ipsec_generate_sd 

@Description	Generate SEC Shared Descriptor for Encapsulation
*//***************************************************************************/
int ipsec_generate_encap_sd(
		uint64_t sd_addr, /* Flow Context Address in external memory */
		struct ipsec_descriptor_params *params,
		int *sd_size /* Shared descriptor Length */
);

/**************************************************************************//**
@Function		ipsec_generate_sd 

@Description	Generate SEC Shared Descriptor for Decapsulation
*//***************************************************************************/
int ipsec_generate_decap_sd(
		uint64_t sd_addr, /* Flow Context Address in external memory */
		struct ipsec_descriptor_params *params,
		int *sd_size /* Shared descriptor Length */
);

/**************************************************************************//**
@Function		ipsec_generate_sa_params 

@Description	Generate and store the functional module internal parameter
*//***************************************************************************/
int ipsec_generate_sa_params(
		struct ipsec_descriptor_params *params,
		ipsec_handle_t ipsec_handle, /* Parameters area (start of buffer) */
		ipsec_instance_handle_t instance_handle,
		uint8_t tmi_id);

/**************************************************************************//**
*	ipsec_get_buffer
*	
*	@Description	Allocates a buffer for the IPsec parameters	according 
*			to the instance parameters and increments the instance counters 
*	
*//****************************************************************************/
int ipsec_get_buffer(ipsec_instance_handle_t instance_handle,
		ipsec_handle_t *ipsec_handle,
		uint8_t *tmi_id
	);

/**************************************************************************//**
*	ipsec_release_buffer
*	@Description	release a buffer and decrements the instance counters 
*		
*//****************************************************************************/
int ipsec_release_buffer(ipsec_instance_handle_t instance_handle,
		ipsec_handle_t ipsec_handle
	);

/**************************************************************************//**
*	ipsec_get_ipv6_nh_offset
*	
*	@Description	calculate the nh_offset for IPv6 transport mode 
*//****************************************************************************/
uint8_t ipsec_get_ipv6_nh_offset(struct ipv6hdr *ipv6_hdr, uint8_t *length);

/**************************************************************************//**
*	ipsec_tman_callback
*	
*	@Description	Generic TMAN callback function 
*//****************************************************************************/
void ipsec_tman_callback(uint64_t ipsec_handle, uint16_t indicator);

/**************************************************************************//**
@Function		ipsec_init_debug_info 

@Description	Initialize the debug segment of the descriptor 
*//***************************************************************************/
void ipsec_init_debug_info(ipsec_handle_t desc_addr);

/**************************************************************************//**
*	Debug Information and Functions
*	
*	@Description: 
*//****************************************************************************/
enum ipsec_function_identifier {
	/* External Functions */
	IPSEC_EARLY_INIT = 1,
	IPSEC_CREATE_INSTANCE,
	IPSEC_DELETE_INSTANCE,
	IPSEC_ADD_SA_DESCRIPTOR,
	IPSEC_DEL_SA_DESCRIPTOR,
	IPSEC_GET_LIFETIME_STATS,
	IPSEC_DECR_LIFETIME_COUNTERS,
	IPSEC_GET_SEQ_NUM,
	IPSEC_FRAME_DECRYPT,
	IPSEC_FRAME_ENCRYPT,
	IPSEC_FORCE_SECONDS_LIFETIME_EXPIRY,

	/* Internal Functions */
	IPSEC_GENERATE_FLC,
	IPSEC_CREATE_KEY_COPY,
	IPSEC_GENERATE_ENCAP_SD,
	IPSEC_GENERATE_DECAP_SD,
	IPSEC_GENERATE_SA_PARAMS,
	IPSEC_GET_BUFFER,
	IPSEC_RELEASE_BUFFER,
	IPSEC_GET_IPV6_NH_OFFSE,
	IPSEC_TMAN_CALLBACK,
	IPSEC_ERROR_HANDLER
};

enum ipsec_service_identifier {
	IPSEC_INTERNAL_SERVICE = 1,
	IPSEC_SEC_HW,
	IPSEC_CDMA_ACQUIRE_CONTEXT_MEMORY,
	IPSEC_FDMA_INSERT_DEFAULT_SEGMENT_DATA,
	IPSEC_FDMA_PRESENT_DEFAULT_FRAME,
	IPSEC_FDMA_PRESENT_DEFAULT_FRAME_SEGMENT,
	IPSEC_FDMA_REPLACE_DEFAULT_SEGMENT_DATA,
	IPSEC_FDMA_STORE_DEFAULT_FRAME_DATA,
	IPSEC_RTA_INLINE_QUERY,
	IPSEC_SLAB_FIND_AND_RESERVE_BPID,
	IPSEC_SLAB_FIND_AND_UNRESERVE_BPID,
	IPSEC_SLAB_REGISTER_CONTEXT_BUFFER_REQUIREMENTS,
	IPSEC_TMAN_CREATE_TIMER,
	IPSEC_TMAN_DELETE_TIMER,
	IPSEC_PARSE_RESULT_GENERATE_DEFAULT
};

/* Instance Parameters structure */
struct ipsec_debug_info {
	enum ipsec_function_identifier func_id;
	enum ipsec_service_identifier service_id;  /* SR/Hardware ID */
	uint32_t line;
	int status; /* Error/Status value */
};

void ipsec_error_handler(
		ipsec_handle_t ipsec_handle,
		enum ipsec_function_identifier func_id,  /* Function ID */
		enum ipsec_service_identifier service_id,  /* SR/Hardware ID */
		uint32_t line,
		int status); /* Error/Status value */

/**************************************************************************//**
******************************************************************************/

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */
/** @} */ /* end of NETF */


#endif /* __AIOP_IPSEC_H */
