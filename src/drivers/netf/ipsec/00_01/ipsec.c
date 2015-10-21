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
@File		ipsec.c

@Description	This file contains the AIOP IPSec implementation.
		
*//***************************************************************************/

#include "fsl_types.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_cdma.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_l2.h"
#include "fsl_tman.h"
#include "fsl_ste.h"
#include "fsl_osm.h"
#include "net.h"
#include "fsl_checksum.h"

/*#include "cdma.h"*/
#include "osm_inline.h"
#include "system.h"
#include "fsl_malloc.h"

#ifdef AIOP_VERIF
#include "slab_stub.h"
#else
#include "fsl_sl_slab.h"
#endif /* AIOP_VERIF */

#include "fsl_ipsec.h"
#include "ipsec.h"

#include "rta.h"
#include "desc/ipsec.h"

/* SEC Era version for RTA */
enum rta_sec_era rta_sec_era = RTA_SEC_ERA_8;

/* Global parameters */
extern __PROFILE_SRAM struct storage_profile 
			storage_profile[SP_NUM_OF_STORAGE_PROFILES];



/**************************************************************************//**
*	ipsec_early_init
*//****************************************************************************/
int ipsec_early_init(
		uint32_t total_instance_num,
		uint32_t total_committed_sa_num,
		uint32_t total_max_sa_num,
		uint32_t flags)
{

	int return_val;
	int mem_id = IPSEC_PRIMARY_MEM_PARTITION_ID;
	       
	uint32_t dummy = flags; /* dummy assignment, to avoid warning */
	
	uint32_t committed_buffs;
	uint32_t max_buffs;
	committed_buffs = total_instance_num + total_committed_sa_num;
	max_buffs = total_instance_num + total_max_sa_num;

    if (!(fsl_mem_exists(IPSEC_PRIMARY_MEM_PARTITION_ID))) {
    	mem_id = IPSEC_SECONDARY_MEM_PARTITION_ID;
    }
    
	return_val = slab_register_context_buffer_requirements(
			committed_buffs,
			max_buffs, /* uint32_t max_buffs */
			IPSEC_SA_DESC_BUF_SIZE, /* uint16_t buff_size */
			IPSEC_SA_DESC_BUF_ALIGN, /* uint16_t alignment */
			(enum memory_partition_id)mem_id, /* enum memory_partition_id  mem_pid */
	        0, /* uint32_t flags */
	        0 /* uint32_t num_ddr_pools */
	        );
	
	return return_val;
}

/**************************************************************************//**
*	ipsec_create_instance
*//****************************************************************************/
int ipsec_create_instance (
		uint32_t committed_sa_num,
		uint32_t max_sa_num,
		uint32_t instance_flags,
		uint8_t tmi_id,
		ipsec_instance_handle_t *instance_handle)
{
	int32_t return_val;
	struct ipsec_instance_params instance; 

	int mem_id = IPSEC_PRIMARY_MEM_PARTITION_ID;
	    	
	if (!(fsl_mem_exists(IPSEC_PRIMARY_MEM_PARTITION_ID))) {
	    	mem_id = IPSEC_SECONDARY_MEM_PARTITION_ID;
	}
	
	instance.sa_count = 0;
	instance.committed_sa_num = committed_sa_num;
	instance.max_sa_num = max_sa_num;
	instance.instance_flags = instance_flags;
	instance.tmi_id = tmi_id;

	/* Descriptor and Instance Buffers */
	return_val = slab_find_and_reserve_bpid(
			(committed_sa_num + 1), /* uint32_t num_buffs */
			IPSEC_SA_DESC_BUF_SIZE, /* uint16_t buff_size */
			IPSEC_SA_DESC_BUF_ALIGN, /* uint16_t alignment */ 
			(enum memory_partition_id)mem_id, /* mem_partition_id */
            NULL, /*NULL*/
            &(instance.desc_bpid)); /* uint16_t *bpid */
	
	if (return_val) {
		return -ENOMEM;
	}
	
	/* Allocate a buffer for the instance */
	return_val = (int32_t)cdma_acquire_context_memory(
		instance.desc_bpid,
		instance_handle); /* context_memory */ 
	
	/* Upon an error, un-reserve the buffers */
	if (return_val) {
		slab_find_and_unreserve_bpid(
				(int32_t)(committed_sa_num + 1), /* int32_t num_buffs */
				instance.desc_bpid); /* uint16_t bpid */
		/* not checking for an error here since the BPID must be valid */
		return -ENOSPC;
	}
		
	/* Write the Instance to external memory */
	cdma_write(
			*instance_handle, /* ext_address */
			&instance, /* ws_src */
			(uint16_t)(sizeof(instance))); /* size */

	return IPSEC_SUCCESS; 
}


/**************************************************************************//**
*	ipsec_delete_instance
*//****************************************************************************/
int ipsec_delete_instance(ipsec_instance_handle_t instance_handle)
{
	int32_t return_val;

	struct ipsec_instance_params instance; 

	cdma_read(
			&instance, /* void *ws_dst */
			instance_handle, /* uint64_t ext_address */
			sizeof(instance) /* uint16_t size */
			);
	
	/* Check if all SAs were deleted */
	if (instance.sa_count == 0) {
	
		/* Release the instance buffer */ 
		cdma_release_context_memory(instance_handle);
		
		/* Un-reserve "committed + 1" buffers back to the slab */
		return_val = slab_find_and_unreserve_bpid(
			(int32_t)(instance.committed_sa_num + 1), /* int32_t num_buffs */
			instance.desc_bpid); /* uint16_t bpid */
		/* check for slab error */
		if (return_val) {
			return -ENAVAIL; /* Resource not available, or not found */
		}
		
		return IPSEC_SUCCESS;
	} else {
		return -EPERM; /* Operation not permitted */
	}
} /* End of ipsec_delete_instance */

/**************************************************************************//**
*	ipsec_get_buffer
*//****************************************************************************/
int ipsec_get_buffer(ipsec_instance_handle_t instance_handle,
		ipsec_handle_t *ipsec_handle, uint8_t *tmi_id)
{
	int return_val;
	struct ipsec_instance_params instance; 
	int mem_id = IPSEC_PRIMARY_MEM_PARTITION_ID;
	    	
	if (!(fsl_mem_exists(IPSEC_PRIMARY_MEM_PARTITION_ID))) {
	    	mem_id = IPSEC_SECONDARY_MEM_PARTITION_ID;
	}
	
	cdma_read_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&instance, /* void *ws_dst */
			sizeof(instance) /* uint16_t size */	
	);

	if (instance.sa_count < instance.committed_sa_num) {
		instance.sa_count++;
		/* Write and release lock */
		cdma_write_with_mutex(
				instance_handle, /* uint64_t ext_address */
				CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
				&instance.sa_count, /* void *ws_dst */
				sizeof(instance.sa_count) /* uint16_t size */	
		);
		
		return_val = (int)cdma_acquire_context_memory(
				instance.desc_bpid,
				ipsec_handle); /* context_memory */

		/* Check if CDMA allocation failed */
		if (return_val) goto get_buffer_alloc_err;
		
	} else if (instance.sa_count < instance.max_sa_num) {
		instance.sa_count++;
		/* Write and release lock */
		cdma_write_with_mutex(
				instance_handle, /* uint64_t ext_address */
				CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
				&instance.sa_count, /* void *ws_dst */
				sizeof(instance.sa_count) /* uint16_t size */	
		);
		/* Descriptor Buffer */
		return_val = slab_find_and_reserve_bpid(
				1, /* uint32_t num_buffs */
				IPSEC_SA_DESC_BUF_SIZE, /* uint16_t buff_size */
				IPSEC_SA_DESC_BUF_ALIGN, /* uint16_t alignment */
				(enum memory_partition_id)mem_id, /* mem_partition_id */
	            NULL, /* NULL */
	            &(instance.desc_bpid)); /* uint16_t *bpid */

		/* Check if Slab has no buffers */
		if (return_val) goto get_buffer_alloc_err;

		return_val = (int)cdma_acquire_context_memory(
				instance.desc_bpid,
				ipsec_handle); /* context_memory */
		
		/* Check if CDMA allocation failed */
		if (return_val) goto get_buffer_alloc_err;

	} else {
		/* Release lock */
		cdma_mutex_lock_release(instance_handle);
		return -EPERM;
	}
	
	*tmi_id = instance.tmi_id;
	
	return IPSEC_SUCCESS; 

get_buffer_alloc_err:
	cdma_read_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&instance.sa_count, /* void *ws_dst */
			sizeof(instance.sa_count) /* uint16_t size */	
	);
	
	instance.sa_count--;
	
	cdma_write_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
			&instance.sa_count, /* void *ws_dst */
			sizeof(instance.sa_count) /* uint16_t size */	
	);
	
	return -ENOSPC;
} /* End of ipsec_get_buffer */

/**************************************************************************//**
*	ipsec_release_buffer
*//****************************************************************************/
int ipsec_release_buffer(ipsec_instance_handle_t instance_handle,
		ipsec_handle_t ipsec_handle)
{
	int32_t err;
	struct ipsec_instance_params instance; 

	cdma_read_with_mutex(
			instance_handle, /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&instance, /* void *ws_dst */
			sizeof(instance) /* uint16_t size */	
	);

	if (instance.sa_count > 0) {
		/* Release the buffer */ 
		cdma_release_context_memory(ipsec_handle); 
				
		instance.sa_count--;
		
		/* Write (just the counter ) and release lock */
		cdma_write_with_mutex(
				instance_handle, /* uint64_t ext_address */
				CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
				&instance.sa_count, /* void *ws_dst */
				sizeof(instance.sa_count) /* uint16_t size */	
		);
		
		/* If buffer taken from 'max' quanta, need to return to slab */
		/* The ">=" is because sa_count was already decremented above */
		if (instance.sa_count >= instance.committed_sa_num) {
			/* Un-reserve one buffer back to the slab */
			err = slab_find_and_unreserve_bpid(
					1, /* int32_t num_buffs */
					instance.desc_bpid); /* uint16_t bpid */
			/* Check for slab error */
			if(err) {
				return -ENAVAIL; /* bman pool not found */
			}
		}
		
		return IPSEC_SUCCESS;
	} else {
		/* Release lock */
		cdma_mutex_lock_release(instance_handle);
		/* EPERM = 1, Operation not permitted */
		return -EPERM; /* trying to delete SA from empty instance */
	}
} /* End of ipsec_release_buffer */	
		

/**************************************************************************//**
@Function		ipsec_generate_encap_sd 

@Description	Generate SEC Shared Descriptor for Encapsulation
*//***************************************************************************/
/* The inline_max_size inline_max_total_size pragmas are here to eliminate
 * non-inlined build and warnings of the RTA */
#pragma push
#pragma inline_max_size (5000)
#pragma inline_max_total_size(10000)

int ipsec_generate_encap_sd(
		uint64_t sd_addr, /* Shared Descriptor Address in external memory */
		struct ipsec_descriptor_params *params,
		int *sd_size) /* Shared descriptor Length */
{
	
	uint8_t cipher_type = 0;
	uint8_t pdb_options = 0;

	/* Temporary Workspace Shared Descriptor */
	uint32_t ws_shared_desc[IPSEC_MAX_SD_SIZE_WORDS] = {0}; 
	
	/* ws_shared_desc[0-2] is used as data_len[0-2]; 
	 * ws_shared_desc[3] is used as inl_mask */

	int err;
	struct ipsec_encap_pdb pdb;

	/* For tunnel mode IPv4, calculate the outer header checksum */
	/* ip_hdr_len = IP header length in bytes.
	 * includes additional 8 bytes if UDP encapsulation is enabled */
	if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
		if((*params->encparams.outer_hdr & IPSEC_OUTER_HEADER_IPV_MASK) ==
					IPSEC_OUTER_HEADER_IPV4) {
			
			/* Clear the input checksum */
			params->encparams.outer_hdr[2] &= IPSEC_OUTER_HEADER_CHECKSUM_MASK;

			/* calculate the length in 16-bit words */
			if (!(params->flags & IPSEC_ENC_OPTS_NAT_EN)) {
				ws_shared_desc[0] = 
						(unsigned)(params->encparams.ip_hdr_len)>>1;
			} else {
				ws_shared_desc[0] = 
						(unsigned)(params->encparams.ip_hdr_len - 8)>>1;
			}
			
			/* ws_shared_desc[0]: length, ws_shared_desc[1]: index, 
			 * ws_shared_desc[2]: checksum */
			ws_shared_desc[2] = 
					(uint16_t)*((uint16_t *)params->encparams.outer_hdr);
			for (ws_shared_desc[1] = 1; 
					ws_shared_desc[1] < ws_shared_desc[0]; 
													ws_shared_desc[1]++) {
				ws_shared_desc[2] = (uint16_t)cksum_ones_complement_sum16(
						(uint16_t)ws_shared_desc[2],
						(uint16_t)*((uint16_t *)
								params->encparams.outer_hdr+ws_shared_desc[1])
						);
			}
			
			/* Invert and update the outer header */
			params->encparams.outer_hdr[2] |= 
					(~ws_shared_desc[2] & (~0xFFFF0000));

			ws_shared_desc[0] = 0;
			ws_shared_desc[1] = 0;
			ws_shared_desc[2] = 0;
		}
	}
	
	/* Build PDB fields for the RTA */
	
	ws_shared_desc[1] = 1; /* Flag for split key calculation. 
					 * A "1" indicates that a split key is required */ 
	
	/* Check which method is it according to the key */
	switch (params->cipherdata.algtype) {
		case IPSEC_CIPHER_AES_CBC:
		case IPSEC_CIPHER_DES_IV64:
		case IPSEC_CIPHER_DES:
		case IPSEC_CIPHER_3DES:
		case IPSEC_CIPHER_NULL: /* No usage of IV for null encryption */
			cipher_type = CIPHER_TYPE_CBC;
			break;
		case IPSEC_CIPHER_AES_CTR:
			cipher_type = CIPHER_TYPE_CTR;
			break;
		/* To construct the CCM B0, SEC uses the B0 flags byte of the PDB
		 * according to the size of ICV transmitted. (RFC4309, RFC3610)
		 * For an 8-byte ICV, select a value of 5Bh.
		 * For a 12-byte ICV, select a value of 6Bh.
		 * For a 16-byte ICV, select a value of 7Bh. */
		 /* @ccm_opt: CCM algorithm options - MSB-LSB description:
		  *  b0_flags (8b) - CCM B0;
		  *  ctr_flags (8b) - counter flags; constant equal to 0x3
		  *  ctr_initial (16b) - initial count constant */
		/* For CCM the counter flags field should be 0x03 */
		case IPSEC_CIPHER_AES_CCM8:
			cipher_type = CIPHER_TYPE_CCM;
			pdb.ccm.ccm_opt = 0x5B030000; /* b0_flags=0x5B, ctr_flags=0x03 */
			break;
		case IPSEC_CIPHER_AES_CCM12:
			cipher_type = CIPHER_TYPE_CCM;
			pdb.ccm.ccm_opt = 0x6B030000; /* b0_flags=0x6B, ctr_flags=0x03 */
			break;
		case IPSEC_CIPHER_AES_CCM16:
			cipher_type = CIPHER_TYPE_CCM;
			pdb.ccm.ccm_opt = 0x7B030000; /* b0_flags=0x7B, ctr_flags=0x03 */
			break;
		case IPSEC_CIPHER_AES_GCM8:
		case IPSEC_CIPHER_AES_GCM12:
		case IPSEC_CIPHER_AES_GCM16:
		case IPSEC_CIPHER_AES_NULL_WITH_GMAC:
			cipher_type = CIPHER_TYPE_GCM;
			break;
		default:
			cipher_type = CIPHER_TYPE_CBC; // TODO: check if this is correct
	}
	
	switch (cipher_type) {
		case CIPHER_TYPE_CBC:
			/* uint8_t iv[16] */
			memcpy(pdb.cbc.iv, params->encparams.cbc.iv, 
					sizeof(params->encparams.cbc.iv));
			break;
		case CIPHER_TYPE_CTR:
			/*	uint8_t ctr_nonce[4]; */
			/*	uint32_t ctr_initial; */
			/*	uint32_t iv[2]; */
			memcpy(pdb.ctr.ctr_nonce, params->encparams.ctr.ctr_nonce,
			       sizeof(params->encparams.ctr.ctr_nonce));
			pdb.ctr.ctr_initial = 1;
			pdb.ctr.iv = params->encparams.ctr.iv;
			break;
		case CIPHER_TYPE_CCM:
			/*	uint8_t salt[4]; lower 24 bits */
			/*	uint32_t ccm_opt; */
			/*	uint32_t iv[2]; */
			memcpy(pdb.ccm.salt, params->encparams.ccm.salt,
			       sizeof(params->encparams.ccm.salt));
			pdb.ccm.iv = params->encparams.ccm.iv;
			ws_shared_desc[1] = 0;  /* No room required for authentication key */
			break;
		case CIPHER_TYPE_GCM:
			/*	uint8_t salt[4]; lower 24 bits */
			/*	uint32_t rsvd1; */
			/*	uint32_t iv[2]; */
			memcpy(pdb.gcm.salt, params->encparams.gcm.salt,
			       sizeof(params->encparams.gcm.salt));
			pdb.gcm.rsvd = 0;
			pdb.gcm.iv = params->encparams.gcm.iv;
			ws_shared_desc[1] = 0;  /* No room required for authentication key */
			break;
		default:
			memcpy(pdb.cbc.iv, params->encparams.cbc.iv, 
					sizeof(params->encparams.cbc.iv));
	}
	
	/* Tunnel Mode Parameters */
	if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
		/* NAT and NUC Options for tunnel mode encapsulation */
		/* Bit 1 : NAT Enable RFC 3948 UDP-encapsulated-ESP */
		/* Bit 0 : NUC Enable NAT UDP Checksum */
		if (params->flags & IPSEC_ENC_OPTS_NAT_EN)
				pdb_options = IPSEC_ENC_PDB_OPTIONS_NAT; 
		if (params->flags & IPSEC_ENC_OPTS_NUC_EN)
				pdb_options |= IPSEC_ENC_PDB_OPTIONS_NUC;
		
		/* outer header from PDB */
		pdb_options |= IPSEC_ENC_PDB_OPTIONS_OIHI_PDB;
		
	} else {
	/* Transport Mode Parameters */
		/* IPv6, no checksum */
		if ((params->encparams.options) & IPSEC_PDB_OPTIONS_MASK & 
				IPSEC_OPTS_ESP_IPVSN) {
			pdb_options |= IPSEC_ENC_OPTS_INC_IPHDR;

		} else {
		/* IPv4, checksum update */
			pdb_options |= (IPSEC_ENC_OPTS_UPDATE_CSUM | 
					IPSEC_ENC_OPTS_INC_IPHDR);
		}
	}

	/*
	 * Transport mode Next Header and Next Header Offset are initialized to
	 * zero, since they come from DPOVRD.
	 */
	pdb.options = 
		(uint8_t)((((params->encparams.options) & IPSEC_PDB_OPTIONS_MASK)) |
		pdb_options
		);
	pdb.options |= (
			(((params->encparams.options) & IPSEC_ENC_PDB_HMO_MASK))
			<<IPSEC_ENC_PDB_HMO_SHIFT);

	pdb.seq_num_ext_hi = params->encparams.seq_num_ext_hi;
	pdb.seq_num = params->encparams.seq_num;
	
	pdb.spi = params->encparams.spi;
	pdb.ip_hdr_len = (uint32_t) params->encparams.ip_hdr_len;

	/* Lengths of items to be inlined in descriptor; order is important.
	 * Note: For now we assume that inl_mask[0] = 1, i.e. that the
	 * Outer IP Header can be inlined. 
	 * Job descriptor maximum length is hard-coded to 7 * CAAM_CMD_SZ +
	 * 3 * CAAM_PTR_SZ, and pointer size considered extended.
	*/
	ws_shared_desc[0] = pdb.ip_hdr_len; /* Outer IP header length */
	
	/* Check if a split authentication key is required 
	 * cbc(aes) OR cbc(des) OR cbc(3des) OR ctr(aes) 
	 * + hmac(xxx) -> need split key
	 * + null authentication OR aes-xcbc-mac OR aes-cmac -> don't need split key
	 * all other modes -> don't need split key 
	 *	  e.g. gcm(aes), ccm(aes), gmac(aes) - don't need split key
	*/
	if (ws_shared_desc[1]) {
		/* Sizes for MDHA pads (*not* keys): MD5, SHA1, 224, 256, 384, 512 */
		/*                                   16,  20,   32,  32,  64,  64  */
		switch (params->authdata.algtype) {
			case IPSEC_AUTH_HMAC_NULL:
				ws_shared_desc[1] = 0;
				break;
			case IPSEC_AUTH_AES_XCBC_MAC_96:
			case IPSEC_AUTH_AES_CMAC_96:
				ws_shared_desc[1] = params->authdata.keylen; /* No split */
				break;
			case IPSEC_AUTH_HMAC_MD5_96:
			case IPSEC_AUTH_HMAC_MD5_128:
				ws_shared_desc[1] = 2*16;
				break;
			case IPSEC_AUTH_HMAC_SHA1_96:
			case IPSEC_AUTH_HMAC_SHA1_160:	
				ws_shared_desc[1] = 2*20;
				break;
			case IPSEC_AUTH_HMAC_SHA2_256_128:
				ws_shared_desc[1] = 2*32;
				break;
			default:
				/* IPSEC_AUTH_HMAC_SHA2_384_192 */
				/* IPSEC_AUTH_HMAC_SHA2_512_256 */
				ws_shared_desc[1] = 2*64;
		}	
	} 

	ws_shared_desc[2] = params->cipherdata.keylen;
	
	err = rta_inline_query(IPSEC_NEW_ENC_BASE_DESC_LEN, 
			IPSEC_MAX_AI_JOB_DESC_SIZE, 
			(unsigned *)ws_shared_desc, 
			&ws_shared_desc[3], 
			3);

	if (err < 0)
		return err;
	
	if (ws_shared_desc[3] & (1 << 1)) {
		params->authdata.key_type = (enum key_types)RTA_PARAM_IMM_DMA;
	} else {
		params->authdata.key_type = (enum key_types)RTA_PARAM_PTR;
	}	
	
	if (ws_shared_desc[3] & (1 << 2)) {
		params->cipherdata.key_type = (enum key_types)RTA_PARAM_IMM_DMA;
	} else {
		params->cipherdata.key_type = (enum key_types)RTA_PARAM_PTR;
	}
	
	params->authdata.algmode = 0;
	params->cipherdata.algmode = 0;
	
	/* Clear reused fields */
	ws_shared_desc[0] = 0;
	ws_shared_desc[1] = 0;
	ws_shared_desc[2] = 0;
	ws_shared_desc[3] = 0;
	               
	/* Call RTA function to build an encap descriptor */
	if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
		/* Tunnel mode, SEC "new thread" */	
		*sd_size = cnstr_shdsc_ipsec_new_encap(
			(uint32_t *)(ws_shared_desc), /* uint32_t *descbuf */
			IPSEC_SEC_POINTER_SIZE, /* unsigned short ps */
			TRUE, /* swap */
			&pdb, /* PDB */
			(uint8_t *)params->encparams.outer_hdr, /* uint8_t *opt_ip_hdr */
			(struct alginfo *)(&params->cipherdata),
			(struct alginfo *)(&params->authdata)
		);
	} else {
		/* Transport mode, SEC legacy new thread */
		*sd_size = cnstr_shdsc_ipsec_encap(
			(uint32_t *)(ws_shared_desc), /* uint32_t *descbuf */
			IPSEC_SEC_POINTER_SIZE, /* unsigned short ps */
			TRUE, /* bool swap */
			&pdb, /* PDB */
			(struct alginfo *)(&params->cipherdata),
			(struct alginfo *)(&params->authdata)
		);
	}	
	
	/* Write the descriptor to external memory */
	cdma_write(
			sd_addr, /* ext_address */
			ws_shared_desc, /* ws_src */
			(uint16_t)((*sd_size)<<2)); /* sd_size is in 32-bit words */
	
	return IPSEC_SUCCESS;

} /* End of ipsec_generate_encap_sd */
#pragma pop
/**************************************************************************//**
@Function		ipsec_generate_decap_sd 

@Description	Generate SEC Shared Descriptor for Encapsulation
*//***************************************************************************/
/* The inline_max_size inline_max_total_size pragmas are here to eliminate
 * non-inlined build and warnings of the RTA */
#pragma push
#pragma inline_max_size (5000)
#pragma inline_max_total_size(10000)

int ipsec_generate_decap_sd(
		uint64_t sd_addr, /* Shared Descriptor Address in external memory */
		struct ipsec_descriptor_params *params,
		int *sd_size) /* Shared descriptor Length */
{
	
	uint8_t cipher_type = 0;
	
	/* Temporary Workspace Shared Descriptor */
	uint32_t ws_shared_desc[IPSEC_MAX_SD_SIZE_WORDS] = {0}; 
	
	/* ws_shared_desc[0-1] is used as data_len[0-1]; 
	 * ws_shared_desc[3] is used as inl_mask */

	int err;

	struct ipsec_decap_pdb pdb;

	/* Build PDB fields for the RTA */
	
	ws_shared_desc[0] = 1; /* Flag for split key calculation. 
					 * A "1" indicates that a split key is required */ 
	
	/* Check which method is it according to the key */
	switch (params->cipherdata.algtype) {
		case IPSEC_CIPHER_AES_CBC:
		case IPSEC_CIPHER_DES_IV64:
		case IPSEC_CIPHER_DES:
		case IPSEC_CIPHER_3DES:
		case IPSEC_CIPHER_NULL: /* No usage of IV for null encryption */
			cipher_type = CIPHER_TYPE_CBC;
			break;
		case IPSEC_CIPHER_AES_CTR:
			cipher_type = CIPHER_TYPE_CTR;
			break;
		
		/* To construct the CCM B0, SEC uses the B0 flags byte of the PDB
		 * according to the size of ICV transmitted.
		 * For an 8-byte ICV, select a value of 5Bh.
		 * For a 12-byte ICV, select a value of 6Bh.
		 * For a 16-byte ICV, select a value of 7Bh. */
			 /* @ccm_opt: CCM algorithm options - MSB-LSB description:
			  *  b0_flags (8b) - CCM B0;
			  *  ctr_flags (8b) - counter flags; constant equal to 0x3
			  *  ctr_initial (16b) - initial count constant */
		/* For CCM the counter flags field should be 0x03 
		 * For CTR the Initial count should be 0x0001 */		
		case IPSEC_CIPHER_AES_CCM8:
			cipher_type = CIPHER_TYPE_CCM;
			pdb.ccm.ccm_opt = 0x5B030000; /* b0_flags=0x5B, ctr_flags=0x03 */
			break;
		case IPSEC_CIPHER_AES_CCM12:
			cipher_type = CIPHER_TYPE_CCM;
			pdb.ccm.ccm_opt = 0x6B030000; /* b0_flags=0x6B, ctr_flags=0x03 */
			break;
		case IPSEC_CIPHER_AES_CCM16:
			cipher_type = CIPHER_TYPE_CCM;
			pdb.ccm.ccm_opt = 0x7B030000; /* b0_flags=0x5B, ctr_flags=0x03 */
			break;		
		case IPSEC_CIPHER_AES_GCM8:
		case IPSEC_CIPHER_AES_GCM12:
		case IPSEC_CIPHER_AES_GCM16:
		case IPSEC_CIPHER_AES_NULL_WITH_GMAC:
			cipher_type = CIPHER_TYPE_GCM;
			break;
		default:
			cipher_type = CIPHER_TYPE_CBC; // TODO: check if this is correct
	}
		/*----------------------------------*/
		/* 	ipsec_generate_decap_sd			*/
		/*----------------------------------*/
	
	switch (cipher_type) {
		case CIPHER_TYPE_CBC:
			/* uint32_t rsvd[2]; */
			pdb.cbc.rsvd[0] = 0;
			pdb.cbc.rsvd[1] = 0;
            break;
		case CIPHER_TYPE_CTR:
			/* uint8_t ctr_nonce[4]; */
			/* uint32_t ctr_initial; */
			memcpy(pdb.ctr.ctr_nonce,
			       params->decparams.ctr.ctr_nonce,
			       sizeof(params->decparams.ctr.ctr_nonce)); 
			pdb.ctr.ctr_initial = 1;
			break;
		case CIPHER_TYPE_CCM:
			/* uint8_t salt[4]; */
			/* uint32_t ccm_opt; */
			memcpy(pdb.ccm.salt, params->decparams.ccm.salt,
			       sizeof(params->decparams.ccm.salt));
			ws_shared_desc[0] = 0; /* No room required for authentication key */
			break;
		case CIPHER_TYPE_GCM:
			/* uint8_t salt[4]; */
			/* uint32_t rsvd; */
			memcpy(pdb.gcm.salt, params->decparams.gcm.salt,
			       sizeof(params->decparams.gcm.salt));
			pdb.gcm.rsvd = 0;
			ws_shared_desc[0] = 0; /* No room required for authentication key */
			break;
		default:
			pdb.cbc.rsvd[0] = 0;
			pdb.cbc.rsvd[1] = 0;
	}
	
			/*----------------------------------*/
			/* 	ipsec_generate_decap_sd			*/
			/*----------------------------------*/
	
	/* uint16_t ip_hdr_len : 
	 * 		HMO (upper nibble)
	 * 		IP header length (lower 3 nibbles) is not relevant for tunnel
	 * 		and will be set by DPOVRD for transport */
	/*
	 * Transport mode Next Header / Tunnel mode AOIPHO will be set by DPOVRD.
	 */
	pdb.options = 
		(uint8_t)(((params->decparams.options) & IPSEC_PDB_OPTIONS_MASK));
	pdb.options |= (((params->decparams.options) & IPSEC_DEC_PDB_HMO_MASK)
			<<IPSEC_DEC_PDB_HMO_SHIFT);
	
	if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
		pdb.options |= IPSEC_DEC_OPTS_ETU;
	} else {
		/* Transport mode */
		/* If ESP pad checking is not required output frame is only the PDU */
		if (!(params->flags & IPSEC_FLG_TRANSPORT_PAD_CHECK)) {
			pdb.options |= (IPSEC_DEC_PDB_OPTIONS_AOFL | 
					IPSEC_DEC_PDB_OPTIONS_OUTFMT);
		} else {
			/* Transport mode pad checking */
			pdb.options |= IPSEC_DEC_PDB_OPTIONS_OUTFMT;
		}
		
		/*  IPv4, checksum update (in IPv6 there is no checksum) */
		if (!((params->encparams.options) & IPSEC_PDB_OPTIONS_MASK & 
				IPSEC_OPTS_ESP_IPVSN)) {
			pdb.options |= IPSEC_DEC_OPTS_VERIFY_CSUM;
		}
	}
	
	/*
	3 	OUT_FMT 	Output Frame format:
		0 - All Input Frame fields copied to Output Frame
		1 - Output Frame is just the decapsulated PDU
	2 	AOFL 	Adjust Output Frame Length
		0 - Don't adjust output frame length 
		  output frame length reflects output frame actually written to memory,
		  including the padding, Pad Length, and Next Header fields.
		1 - Adjust output frame length 
		subtract the length of the padding, the Pad Length, and the Next Header
		byte from the output frame length reported to the frame consumer.
		If outFMT==0, this bit is reserved and must be zero.
	*/

	pdb.seq_num_ext_hi = params->decparams.seq_num_ext_hi;
	pdb.seq_num = params->decparams.seq_num;
	
	/* uint32_t anti_replay[4]; */
	pdb.anti_replay[0] = 0;
	pdb.anti_replay[1] = 0;
	pdb.anti_replay[2] = 0;
	pdb.anti_replay[3] = 0;

	/*
	 * Lengths of items to be inlined in descriptor; order is important.
	 * Job descriptor maximum length is hard-coded to 7 * CAAM_CMD_SZ +
	 * 3 * CAAM_PTR_SZ, and pointer size considered extended.
	*/

	/* Check if a split authentication key is required 
	 * cbc(aes) OR cbc(des) OR cbc(3des) OR ctr(aes) 
	 * + hmac(xxx) -> need split key
	 * + null authentication OR aes-xcbc-mac OR aes-cmac -> don't need split key
	 * all other modes -> don't need split key 
	 *	  e.g. gcm(aes), ccm(aes), gmac(aes) - don't need split key
	*/
	if (ws_shared_desc[0]) {
		/* Sizes for MDHA pads (*not* keys): MD5, SHA1, 224, 256, 384, 512 */
		/*                                   16,  20,   32,  32,  64,  64  */
		switch (params->authdata.algtype) {
			case IPSEC_AUTH_HMAC_NULL:
				ws_shared_desc[0] = 0;
				break;
			case IPSEC_AUTH_AES_XCBC_MAC_96:
			case IPSEC_AUTH_AES_CMAC_96:
				ws_shared_desc[0] = params->authdata.keylen; /* No split */
				break;
			case IPSEC_AUTH_HMAC_MD5_96:
			case IPSEC_AUTH_HMAC_MD5_128:
				ws_shared_desc[0] = 2*16;
				break;
			case IPSEC_AUTH_HMAC_SHA1_96:
			case IPSEC_AUTH_HMAC_SHA1_160:	
				ws_shared_desc[0] = 2*20;
				break;
			case IPSEC_AUTH_HMAC_SHA2_256_128:
				ws_shared_desc[0] = 2*32;
				break;
			default:
				/* IPSEC_AUTH_HMAC_SHA2_384_192 */
				/* IPSEC_AUTH_HMAC_SHA2_512_256 */
				ws_shared_desc[0] = 2*64;
		}	
	} 

	ws_shared_desc[1] = params->cipherdata.keylen;
	
	err = rta_inline_query(IPSEC_NEW_DEC_BASE_DESC_LEN, 
			IPSEC_MAX_AI_JOB_DESC_SIZE,(unsigned *)ws_shared_desc, 
			&ws_shared_desc[3], 2);
	
	if (err < 0)
		return err;
	
	if (ws_shared_desc[3] & (1 << 0)) {
		params->authdata.key_type = (enum key_types)RTA_PARAM_IMM_DMA;
	} else {
		params->authdata.key_type = (enum key_types)RTA_PARAM_PTR;
	}
		
	if (ws_shared_desc[3] & (1 << 1)) {
		params->cipherdata.key_type = (enum key_types)RTA_PARAM_IMM_DMA;
	} else {
		params->cipherdata.key_type = (enum key_types)RTA_PARAM_PTR;
	}

	params->authdata.algmode = 0;
	params->cipherdata.algmode = 0;

	/* Call RTA function to build an encap descriptor */
	if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
		/* Tunnel mode, SEC "new thread" */	
		*sd_size = cnstr_shdsc_ipsec_new_decap(
			(uint32_t *)(ws_shared_desc), /* uint32_t *descbuf */
			IPSEC_SEC_POINTER_SIZE, /* unsigned short ps */
			TRUE, /* swap */
			&pdb, /* struct ipsec_encap_pdb *pdb */
			(struct alginfo *)(&params->cipherdata),
			(struct alginfo *)(&params->authdata)
		);
	} else {
		/* Transport mode, SEC legacy new thread */
		*sd_size = cnstr_shdsc_ipsec_decap(
			(uint32_t *)(ws_shared_desc), /* uint32_t *descbuf */
			IPSEC_SEC_POINTER_SIZE, /* unsigned short ps */
			TRUE, /* bool swap */
			&pdb, /* struct ipsec_encap_pdb *pdb */
			(struct alginfo *)(&params->cipherdata),
			(struct alginfo *)(&params->authdata)
		);
	}	
	
	/* Write the descriptor to external memory */
	cdma_write(
			sd_addr, /* ext_address */
			ws_shared_desc, /* ws_src */
			(uint16_t)((*sd_size)<<2)); /* sd_size is in 32-bit words */

	return IPSEC_SUCCESS;
} /* End of ipsec_generate_decap_sd */
#pragma pop

/**************************************************************************//**
@Function		ipsec_generate_flc 

@Description	Generate SEC Flow Context Descriptor
*//***************************************************************************/
void ipsec_generate_flc(
		uint64_t flc_address, /* Flow Context Address in external memory */
		//uint16_t spid, /* Storage Profile ID of the SEC output frame */
		struct ipsec_descriptor_params *params, 
		int sd_size) /* Shared descriptor Length  in words*/
{
	
	struct ipsec_flow_context flow_context;

	int i;
	
	struct storage_profile *sp_addr = &storage_profile[0];
	uint8_t *sp_byte;
	uint32_t sp_controls;
	
	//sp_addr += spid;
	sp_addr += params->spid; 

	sp_byte = (uint8_t *)sp_addr;
	
	/* Clear the Flow Context area */
	cdma_ws_memory_init(&flow_context,sizeof(struct ipsec_flow_context),0);

	/* Word 0 */

	/* Word 1 */
	/* 5-0 SDL = Shared Descriptor length, 7-6 reserved */
	/* SDL is encoded in terms of 32-bit descriptor command words */ 
	flow_context.word1_sdl = (uint8_t)(sd_size & 0x000000FF);
	
	//flow_context.word1_bits_15_8 = 0; /* 11-8 CRID, 14-12 reserved, 15 CRJD */
	//flow_context.word1_bits23_16 = 0; /* 16	EWS,17 DAC,18-20?, 23-21 reserved */
	//flow_context.word1_bits31_24 = 0; /* 24 RSC (not used for AIOP), 
	//	25 RBMT (not used for AIOP), 31-26 reserved */
	// TODO: check regarding EWS in buffer reuse mode
	
	// Debug
	//flow_context.word1_bits23_16 = 0x01; /* 16	EWS */
								/* 17 DAC */
								/* 18,19,20 ? */
								/* 23-21 reserved */
	
	/* word 2  RFLC[31-0] */
	//flow_context.word2_rflc_31_0 = 0; /* Not used for AIOP */

	/* word 3  RFLC[63-32] */
	//flow_context.word3_rflc_63_32 = 0; /* Not used for AIOP */

	/* word 4 */ /* Not used, should be NULL */
	//flow_context.word4_iicid = 0; /* 15-0  IICID */
	//flow_context.word4_oicid = 0; /* 31-16 OICID */
	
	/* word 5 */ 	
	//flow_context.word5_7_0 = 0; /* 23-0 OFQID, not used for AIOP */
	//flow_context.word5_15_8 = 0;
	//flow_context.word5_23_16 = 0;
	/* 31-30 ICR = 2. AIOP is a trusted user - no need for any restrictions. */
	//flow_context.word5_31_24 = 0x40;
						/* 24 OSC : not used for AIOP */
						/* 25 OBMT : not used for AIOP */
						/* 29-26 reserved */
						/* 31-30 ICR */
	/* word 6 */
	//flow_context.word6_oflc_31_0 = 0; /* Not used for AIOP */
	
	/* word 7 */
	//flow_context.word7_oflc_63_32 = 0; /* Not used for AIOP */
	
	/* Storage profile format:
	* 0x00 IP-Specific Storage Profile Information 
	* 0x08 Frame Format and Data Placement Controls 
	* 0x10 Buffer Pool 2, Buffer Pool 1 Attributes and Controls 
	* 0x18 Buffer Pool 4, Buffer Pool 3 Attributes and Controls
	* 
	* Only The data from offset 0x08 and 0x10 is copied to SEC flow context 
	*/
	/* Copy the standard Storage Profile to Flow Context words 8-15 */
	/* No need to for the first 8 bytes, so start from 8 */
	// TODO: optionally use for copy 	
	//fdma_copy_data(24, 0 ,sp_byte,flow_context.storage_profile + 8);
	for (i = 8; i < 32; i++) {
		*((uint8_t *)((uint8_t *)flow_context.storage_profile + i - 8)) = 
				*(sp_byte + i); 
	}
	
	/* reuse buffer mode */
	if (params->flags & IPSEC_FLG_BUFFER_REUSE) {
		
		/* In reuse buffer mode (BS=1) the DHR field is treated
		 * as a signed value of a data headroom correction and defines by
		 * how many bytes an existing offset should be adjusted to make room 
		 * for additional output data or any need to move the output ‘forward’
		 * The SEC expects that |DHR| >= frame growth  
		 * DL is not considered in reuse mode. */
		
		/* SP word at offset 0x4 */
		/* 31|30|29-28|27-25|24 |23  |22-21|20  |19-16|15-12|11-0| */
		/* BS|R | FF  |  R  |DLC|PTAR|  R  |SGHR|ASAR |  R  |DHR | */
		
		/* For reuse mode:
		 * BS = 1
		 * FF = 10 - Reuse input buffers if they provide sufficient space
		 * DLC = 0
		 * PTAR = 0 (ignored)
		 * SGHR = 0 (ignored)
		 * ASAR - preserve existing size
		 * DHR = negative correction value in 2's complement format
		*/
		
		/* Read-swap the storage profile word at offset 4 */
		/* LW_SWAP(_disp, _base) */
		sp_controls = LW_SWAP(4, ((uint32_t *)flow_context.storage_profile));
		
		/* Clear all bits but ASAR */
		/* set BS = 0b1, FF = 0b10 */
		if (params->direction == IPSEC_DIRECTION_OUTBOUND) {

			/* Set BS=1, FF = 01, ASAR and DHR (negative, 2's complement) */
			sp_controls = (sp_controls & IPSEC_SP_ASAR_MASK) | 
					IPSEC_SP_REUSE_BS_FF |
					(IPSEC_SP_DHR_MASK & (-(IPSEC_MAX_FRAME_GROWTH + 
							(uint32_t)params->encparams.ip_hdr_len))); 
			
		} else {
			sp_controls = (sp_controls & IPSEC_SP_ASAR_MASK) | 
					IPSEC_SP_REUSE_BS_FF; 
		}
		
		/* Store the new storage profile word with swapping to little endian */
		/* STW_SWAP(_val, _disp, _base)	*/
		STW_SWAP(sp_controls, 4, (uint32_t *)flow_context.storage_profile);
		
	} else {
		/* New output buffer mode */ 
		
		/* Set the DL (tailroom growth) */
		if (params->direction == IPSEC_DIRECTION_OUTBOUND) {
			sp_controls = (uint32_t)params->encparams.ip_hdr_len + 
					IPSEC_MAX_FRAME_GROWTH;
			STW_SWAP(sp_controls, 0, (uint32_t *)flow_context.storage_profile);
		} 

	}
	
#if(0)
	{
		fsl_print("IPSEC: Flow Context Storage Profile\n");
		uint32_t j;
		uint32_t val;
		for(j=0;j<8;j++) {
			val = *(uint32_t *)((uint32_t)flow_context.storage_profile + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
	}
#endif	
	/* Write the Flow Context to external memory with CDMA */
	cdma_write(
			flc_address, /* ext_address */
			&flow_context, /* ws_src */
			IPSEC_FLOW_CONTEXT_SIZE); /* uint16_t size */
	
} /* End of ipsec_generate_flc */

/**************************************************************************//**
@Function		ipsec_create_key_copy 

@Description	Creates a copy of the key, used for CAAM DKP 
*//***************************************************************************/
void ipsec_create_key_copy(
		uint64_t src_key_addr, /*  Source Key Address */
		uint64_t dst_key_addr, /*  Destination Key Address */
		uint16_t keylen)   /* Length of the provided key, in bytes */
{
	
	uint8_t key[IPSEC_KEY_SEGMENT_SIZE];

	/* Read the key from the application buffer */
	cdma_read(
			key, /* void *ws_dst */
			src_key_addr, /* uint64_t ext_address */
			keylen); /* uint16_t size */

	/* Store the key to the copy location */
	cdma_write(
			dst_key_addr, /* ext_address */
			key, /* ws_src */
			keylen); /* uint16_t size */
	
} /* End of ipsec_create_key_copy */


/**************************************************************************//**
@Function		ipsec_generate_sa_params 

@Description	Generate and store the functional module internal parameter
*//***************************************************************************/
int ipsec_generate_sa_params(
		struct ipsec_descriptor_params *params, 
		ipsec_handle_t desc_addr, /* Parameters area */
		ipsec_instance_handle_t instance_handle,
		uint8_t tmi_id)
{
	int return_val;
	struct ipsec_sa_params sap;
	uint16_t tmr_duration;
	
	sap.sap1.instance_handle = instance_handle; 
	
	/* Descriptor Part #1 */
	sap.sap1.flags = params->flags; // TMP 
		/* 	transport mode, UDP encap, pad check, counters enable, 
					outer IP version, etc. 4B */
	
	/* UDP Encap for transport mode */
	sap.sap1.udp_src_port = 0; /* UDP source for transport mode. */
	sap.sap1.udp_dst_port = 0; /* UDP destination for transport mode. */

	
	if (params->direction == IPSEC_DIRECTION_OUTBOUND) {
		/* Outbound (encryption) */
		
		/* Add inbound/outbound indication to the flags field */
		/* Inbound indication is 0, so no action */
		sap.sap1.flags |= IPSEC_FLG_DIR_OUTBOUND;
	
		if (params->flags & IPSEC_FLG_TUNNEL_MODE) {
			if ((*(params->encparams.outer_hdr) & IPSEC_IP_VERSION_MASK) == 
					IPSEC_IP_VERSION_IPV6) {
				sap.sap1.flags |= IPSEC_FLG_OUTER_HEADER_IPV6;
				
				/* DSCP for IPv6 */
				sap.sap1.outer_hdr_dscp = IPSEC_DSCP_MASK_IPV6 &
					(*(uint32_t *)params->encparams.outer_hdr);
			} else {
				/* DSCP for IPv4 */
				sap.sap1.outer_hdr_dscp = IPSEC_DSCP_MASK_IPV4 &
					(*(uint32_t *)params->encparams.outer_hdr);
			}
			
		} else {
			/* Encap, Transport mode */
			
			/* Add IPv6/IPv4 indication to the flags field in transport mode */
			if ((params->encparams.options) & IPSEC_PDB_OPTIONS_MASK & 
					IPSEC_OPTS_ESP_IPVSN) {
				sap.sap1.flags |= IPSEC_FLG_IPV6;
			}
			
			/* If UDP Encap enabled for transport mode */
			if (params->flags & IPSEC_ENC_OPTS_NAT_EN) {
				/* Save the UDP source and destination ports */
				sap.sap1.udp_src_port = 
						*(uint16_t *)params->encparams.outer_hdr; 
					/* UDP source for transport mode. TMP */
				sap.sap1.udp_dst_port = 
					*((uint16_t *)params->encparams.outer_hdr + 1); 
					/* UDP destination for transport mode. TMP */
			}
		}
		
		/* Length in bytes of the encap frame IP+ESP header, 
		 * to be used for segment length presentation */
		sap.sap1.encap_header_length = 
				params->encparams.ip_hdr_len + 8;
		
	} else {
		/* Inbound (decryption) */

		if (!(params->flags & IPSEC_FLG_TUNNEL_MODE)) {
			/* Add IPv6/IPv4 indication to the flags field in transport mode */
			if ((params->decparams.options) & IPSEC_PDB_OPTIONS_MASK & 
					IPSEC_OPTS_ESP_IPVSN) {
				sap.sap1.flags |= IPSEC_FLG_IPV6;
			}
		}
	}
	
	//sap.sap1.status = 0; /* 	lifetime expiry, semaphores	*/
	sap.sap1.soft_sec_expired = 0; /* soft seconds lifetime expired */
	sap.sap1.hard_sec_expired = 0; /* hard seconds lifetime expired */
		
	/* new/reuse mode */
	if (sap.sap1.flags & IPSEC_FLG_BUFFER_REUSE) {
		sap.sap1.sec_buffer_mode = IPSEC_SEC_REUSE_BUFFER_MODE;
	} else {
		sap.sap1.sec_buffer_mode = IPSEC_SEC_NEW_BUFFER_MODE; 

#ifndef  TKT265088_WA_DISABLE
		{
			/* TKT265088: 
			 * CAAM/SEC: The FD[BPID] is not updated after an AIOP operation */
			struct storage_profile *sp_addr = &storage_profile[0];
			sp_addr += params->spid; 
		
			/* 14 bit BPID is at offset 0x12 (18) of the storage profile */ 
			/* Read-swap and mask the 2 MSbs */
			sap.sap1.bpid = (LH_SWAP(0,(uint16_t *)((uint8_t *)sp_addr + 0x12)))
					& 0x3FFF;
		}	
#endif		
	}
	
	sap.sap1.output_spid = (uint8_t)(params->spid);

	sap.sap1.soft_byte_limit = params->soft_kilobytes_limit; 
	sap.sap1.soft_packet_limit = params->soft_packet_limit; 
	sap.sap1.hard_byte_limit = params->hard_kilobytes_limit; 
	sap.sap1.hard_packet_limit = params->hard_packet_limit; 
		
	sap.sap1.byte_counter = 0; /* Encrypted/decrypted bytes counter */
	sap.sap1.packet_counter = 0; /*	Packets counter */

	/* Set valid flag */
	sap.sap1.valid = 1; /* descriptor valid. */

	/* Descriptor Part #2 */
	//sap.sap2.sec_callback_func = (uint32_t)params->lifetime_callback;
	sap.sap2.sec_callback_func = params->lifetime_callback;
	sap.sap2.sec_callback_arg = params->callback_arg;
		
	/* init one-shot timers according to:
	 * soft_seconds_limit and hard_seconds_limit */ 
	if (params->flags & IPSEC_FLG_LIFETIME_SEC_CNTR_EN) {
		
		/* Soft seconds lifetime timer */
		
		/* If the lifetime is larger than the TMAN max,
		 * it is required to invoke the timer multiple times */
		if (params->soft_seconds_limit > IPSEC_MAX_TIMER_DURATION) {
			tmr_duration = IPSEC_MAX_TIMER_DURATION;
			sap.sap2.soft_seconds_limit = params->soft_seconds_limit - 
					IPSEC_MAX_TIMER_DURATION;
		} else {
			tmr_duration = (uint16_t)params->soft_seconds_limit;
			sap.sap2.soft_seconds_limit = 0;
		}
		
		/* Create soft seconds lifetime timer */
		return_val = tman_create_timer(
				tmi_id, /* uint8_t tmi_id */
				TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY |
					TMAN_CREATE_TIMER_ONE_SHOT, /* uint32_t flags */
					/* 1 Sec timer ticks, one shot */
				tmr_duration, /* uint16_t duration; */
				desc_addr, /* tman_arg_8B_t opaque_data1 */
				IPSEC_SOFT_SEC_LIFETIME_EXPIRED, /* tman_arg_2B_t opaque_data2 */ 
				&ipsec_tman_callback,
				&sap.sap2.soft_tmr_handle); /* uint32_t *timer_handle */
		/* Check for TMAN Error */
		if (return_val) {
			return return_val;
		}
			
		/* Hard seconds lifetime timer */
		
		/* If the lifetime is larger than the TMAN max,
		 * it is required to invoke the timer multiple times */
		if (params->hard_seconds_limit > IPSEC_MAX_TIMER_DURATION) {
			tmr_duration = IPSEC_MAX_TIMER_DURATION;
			sap.sap2.hard_seconds_limit = params->hard_seconds_limit - 
					IPSEC_MAX_TIMER_DURATION;
		} else {
			tmr_duration = (uint16_t)params->hard_seconds_limit;
			sap.sap2.hard_seconds_limit = 0;
		}
		
		/* Create hard seconds lifetime timer */
		return_val = tman_create_timer(
				tmi_id, /* uint8_t tmi_id */
				TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY |
					TMAN_CREATE_TIMER_ONE_SHOT, /* uint32_t flags */
					/* 1 Sec timer ticks, one shot */
				tmr_duration, /* uint16_t duration; */
				desc_addr, /* tman_arg_8B_t opaque_data1 */
				IPSEC_HARD_SEC_LIFETIME_EXPIRED, /* tman_arg_2B_t opaque_data2 */ 
				&ipsec_tman_callback,
				&sap.sap2.hard_tmr_handle); /* uint32_t *timer_handle */
		/* Check for TMAN Error */
		if (return_val) {
			return return_val;
		}
		
		sap.sap2.tmi_id = tmi_id; /* save the TMI ID */
		
	} else {
		/* No secnds lifetime timers */
		sap.sap2.soft_tmr_handle = NULL; 
		sap.sap2.hard_tmr_handle = NULL; 
	}
	
	/* Get timestamp from TMAN */
	tman_get_timestamp(&(sap.sap1.timestamp));
	
	/* Cleasr the STE counters, since CDMA is not enough due to 
	 * the STE internal cache */ 
	ste_set_64bit_counter(
			IPSEC_PACKET_COUNTER_ADDR(desc_addr), /* uint64_t counter_addr */
			0); /* uint64_t value */

	ste_set_64bit_counter(
			IPSEC_KB_COUNTER_ADDR(desc_addr), /* uint64_t counter_addr */
			0); /* uint64_t value */
	
	/* Store to external memory with CDMA */
	cdma_write(
			desc_addr, /* uint64_t ext_address */
			&sap, /* void *ws_src */
			(uint16_t)(sizeof(sap)) /* uint16_t size */
			);
	
	return IPSEC_SUCCESS;
} /* End of ipsec_generate_sa_params */

/**************************************************************************//**
@Function		ipsec_init_debug_info 

@Description	Initialize the debug segment of the descriptor 
*//***************************************************************************/
void ipsec_init_debug_info(
		ipsec_handle_t desc_addr) /* Parameters area */
{
	uint8_t debug_segment[IPSEC_DEBUG_SEGMENT_SIZE] = {0};
	
	/* Clear debug area */
	cdma_write(
		IPSEC_DEBUG_INFO_ADDR(desc_addr), /* ext_address */
		&debug_segment, /* ws_src */
		IPSEC_DEBUG_SEGMENT_SIZE); /* size */
	
} /* End of ipsec_create_key_copy */

/**************************************************************************//**
*	ipsec_add_sa_descriptor
*//****************************************************************************/

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
*/

int ipsec_add_sa_descriptor(
		struct ipsec_descriptor_params *params,
		ipsec_instance_handle_t instance_handle,
		ipsec_handle_t *ipsec_handle)
{

	int return_val;
	int sd_size; /* shared descriptor size, set by the RTA */
	ipsec_handle_t desc_addr;
	uint8_t tmi_id; /* TMAN Instance ID  */

	/* Create a shared descriptor */

	return_val = ipsec_get_buffer(instance_handle,
			ipsec_handle, &tmi_id);
	
	/* Check for allocation error */
	if (return_val) {
		return return_val;
	}
		
	desc_addr = IPSEC_DESC_ADDR(*ipsec_handle);
	
	/* If the authentication key length is not 0, 
	 * create a copy of the authentication key in the local buffer */
	if (params->authdata.keylen) {
		ipsec_create_key_copy(
			params->authdata.key, /* Source Key Address */
			IPSEC_KEY_SEGMENT_ADDR(desc_addr), /* Destination Key Address */
			(uint16_t)params->authdata.keylen);  
								/* Length of the provided key, in bytes */
	
		/* Now switch the original key address with the copy address */
		params->authdata.key = IPSEC_KEY_SEGMENT_ADDR(desc_addr);
	}

	/* If the cipher key length is not 0, 
	 * create a copy of the cipher key in the local buffer */
	if (params->cipherdata.keylen) {
		ipsec_create_key_copy(
			params->cipherdata.key, /* Source Key Address */
			IPSEC_CIPHER_KEY_ADDR(desc_addr), /* Destination Key Address */
			(uint16_t)params->cipherdata.keylen);  
								/* Length of the provided key, in bytes */
	
		/* Now switch the original key address with the copy address */
		params->cipherdata.key = IPSEC_CIPHER_KEY_ADDR(desc_addr);
	}
	
	/* Build a shared descriptor with the RTA library */
	/* Then store it in the memory with CDMA */
	if (params->direction == IPSEC_DIRECTION_INBOUND) {
		return_val = 
			ipsec_generate_decap_sd(IPSEC_SD_ADDR(desc_addr),params, &sd_size);
	} else {
		return_val = 
			ipsec_generate_encap_sd(IPSEC_SD_ADDR(desc_addr),params, &sd_size);
	}
	
	/* Check for IPsec descriptor generation error */
	if (return_val) {
		// TODO: free the buffer, decrement SA counter?, fix error value
		
		/* Release the buffer. No check for error here */ 
		ipsec_release_buffer(instance_handle, *ipsec_handle);
		
		return -ENAVAIL;
	}
	
	/* Generate the SEC Flow Context descriptor and write to memory with CDMA */
	ipsec_generate_flc(
			IPSEC_FLC_ADDR(desc_addr), 
				/* Flow Context Address in external memory */
			params,
			sd_size); /* Shared descriptor size in words */
	
	/*	Prepare descriptor parameters:
	 * Kilobytes and packets lifetime limits.
	 * Modes indicators and other flags */
	/* Store the descriptor parameters to memory (CDMA write). */
	return_val = ipsec_generate_sa_params(
			params,
			desc_addr, /* Parameters area (start of buffer) */
			instance_handle,
			tmi_id);
	/* An error can occur here due to TMAN call */
	if (return_val) {
		return return_val;
	}
	
	/* Initialize the debug area */
	ipsec_init_debug_info(desc_addr);
	
	/* Success, handle returned. */
	return IPSEC_SUCCESS;
	
} /* End of ipsec_add_sa_descriptor */

/**************************************************************************//**
*	ipsec_del_sa_descriptor
*//****************************************************************************/
int ipsec_del_sa_descriptor(
		ipsec_handle_t ipsec_handle)
{

	int return_val;
	ipsec_instance_handle_t instance_handle;
	ipsec_handle_t desc_addr;

	// TODO Delete the timers; take care of callbacks in the middle of operation.
	
	desc_addr = IPSEC_DESC_ADDR(ipsec_handle);

	/* Flush all the counter updates that are pending in the 
	 * statistics engine request queue. */
	ste_barrier();

	/* Cleasr the STE counters, due to the STE internal cache */ 
	ste_set_64bit_counter(
			IPSEC_PACKET_COUNTER_ADDR(desc_addr), /* uint64_t counter_addr */
			0); /* uint64_t value */

	ste_set_64bit_counter(
			IPSEC_KB_COUNTER_ADDR(desc_addr), /* uint64_t counter_addr */
			0); /* uint64_t value */
	
	/* Read the instance handle from params area */
	cdma_read(
			&instance_handle, /* void *ws_dst */
			IPSEC_INSTANCE_HANDLE_ADDR(desc_addr),
			sizeof(instance_handle) /* uint16_t size */
			);
	
	/* Release the buffer */ 
	return_val = ipsec_release_buffer(instance_handle, ipsec_handle);
	
	if (return_val) { /* error */
		return return_val; /* */
	} else { /* success */
		return IPSEC_SUCCESS; 
	}
	
} /* End of ipsec_del_sa_descriptor */

/**************************************************************************//**
* ipsec_frame_encrypt
*//****************************************************************************/
__IPSEC_HOT_CODE int ipsec_frame_encrypt(
		ipsec_handle_t ipsec_handle,
		uint32_t *enc_status
		)
{
	int return_val;
	uint8_t eth_header[40]; /* Ethernet header place holder, 40 bytes */ 
	uint8_t eth_length = 0; /* Ethernet header length and indicator */ 
	uint64_t orig_flc;
	uint32_t orig_frc;
	uint16_t orig_seg_addr;
	uint8_t *segment_pointer;
	uint32_t byte_count;
	//uint32_t checksum;
	uint8_t dont_encrypt = 0;
	ipsec_handle_t desc_addr;
	//uint16_t offset;
	uint32_t original_val, new_val;

	struct ipsec_sa_params_part1 sap1; /* Parameters to read from ext buffer */
	struct scope_status_params scope_status;
	struct dpovrd_general dpovrd;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	
	struct   presentation_context *prc =
				(struct presentation_context *) HWC_PRC_ADDRESS;

	*enc_status = 0; /* Initialize */
	
	/* 	Outbound frame encryption and encapsulation */
#if(0)
	// Debug //
	{
		fsl_print("IPSEC: Reading FD and FRC before SEC\n");
		uint32_t j;
		uint32_t val;
		for(j=0;j<8;j++) {
			val = *(uint32_t *)((uint32_t)0x60 + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
		//val = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
		//fsl_print("FRC = 0x%x\n", val);
		
		//val = LDPAA_FD_GET_BPID(HWC_FD_ADDRESS);
		val = *(uint8_t *)((uint32_t)0x60 + 12);
		fsl_print("FD[BPID] = 0x%x\n", val);

		// Offset
		//val = *(uint32_t *)((uint32_t)0x60 + 3*4);
		//val = LDPAA_FD_GET_OFFSET(HWC_FD_ADDRESS);
		//fsl_print("FD[OFFSET] = 0x%x %x\n", (val & 0xFF), (val & 0xFF00));
		//fsl_print("FD[OFFSET] = 0x%x%x\n", (val & 0x0F), (val & 0xFF00)>>8);
		
		fsl_print("IPSEC: Reading Additional Dequeue Context (0x40):\n");
		for(j=0;j<4;j++) {
			val = *(uint32_t *)((uint32_t)0x40 + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}

	}
	// Debug End //
#endif
	
	desc_addr = IPSEC_DESC_ADDR(ipsec_handle);

	/* 	2.	Read relevant descriptor fields with CDMA. */
	cdma_read(
			&sap1, /* void *ws_dst */
			desc_addr, /* uint64_t ext_address */
			sizeof(sap1) /* uint16_t size */
			);

	/*---------------------*/
	/* ipsec_frame_encrypt */
	/*---------------------*/
	
	/* 	3.	Check that hard kilobyte/packet/seconds lifetime limits have 
	 * not expired. If expired, return with error and go to END */
	/* The seconds lifetime status is checked in the params[status] 
	 * and the kilobyte/packet status is checked from the params[counters].
	 * This is done to avoid doing mutex lock for kilobyte/packet status */
	
	/* Seconds Lifetime */
	if (sap1.soft_sec_expired) {
		if (sap1.flags & IPSEC_FLG_LIFETIME_SEC_CNTR_EN) {
			if (sap1.soft_sec_expired) {
				*enc_status |= IPSEC_STATUS_SOFT_SEC_EXPIRED;
			}
			if (sap1.hard_sec_expired) {
				*enc_status |= IPSEC_STATUS_HARD_SEC_EXPIRED;
				return IPSEC_ERROR;
			}
		}
	}
	
	/* KB lifetime counters */
	if (sap1.flags & IPSEC_FLG_LIFETIME_KB_CNTR_EN) {
		if (sap1.byte_counter >= sap1.soft_byte_limit) {
			*enc_status |= IPSEC_STATUS_SOFT_KB_EXPIRED;
			if (sap1.byte_counter >= sap1.hard_byte_limit) {
				*enc_status |= IPSEC_STATUS_HARD_KB_EXPIRED;
				return IPSEC_ERROR;
			}
		}
	}
	
	/* Packets lifetime counters*/
	if (sap1.flags & IPSEC_FLG_LIFETIME_PKT_CNTR_EN) {
		if (sap1.packet_counter >= sap1.soft_packet_limit) {
			*enc_status |= IPSEC_STATUS_SOFT_PACKET_EXPIRED;
			if (sap1.packet_counter >= sap1.hard_packet_limit) {
				*enc_status |= IPSEC_STATUS_HARD_PACKET_EXPIRED;
				return IPSEC_ERROR;
			}
		}
	}
	// TODO: check if packet counter can be 32 bit
	
		/*---------------------*/
		/* ipsec_frame_encrypt */
		/*---------------------*/
	
	if (sap1.flags & IPSEC_FLG_TUNNEL_MODE) {
		/* Tunnel Mode */
		/* Clear FD[FRC], so DPOVRD takes no action */
		//dpovrd.tunnel_encap.word = 0; 
		
		if (PARSER_IS_OUTER_IPV4_DEFAULT()) {
			dpovrd.tunnel_encap.word = 
				IPSEC_DPOVRD_OVRD | IPSEC_NEXT_HEADER_IPV4;
		} else {
			dpovrd.tunnel_encap.word = 
				IPSEC_DPOVRD_OVRD | IPSEC_NEXT_HEADER_IPV6;
		}
		
		// 13-8 AOIPHO
		//dpovrd.tunnel_encap.word |= (16<<8); // including Ethernet   
		// 27-16 Outer IP Header Material Length 
		//dpovrd.tunnel_encap.word |= (14<<16); // including Ethernet   

	} else {
		/* For Transport mode set DPOVRD */
		/* 31 OVRD, 30-28 Reserved, 27-24 ECN (Not relevant for transport mode)
		 * 23-16 IP Header Length in bytes, 
		* of the portion of the IP header that is not encrypted.
		* 15-8 NH_OFFSET - location of the next header within the IP header.
		* 7-0 Next Header */
		dpovrd.transport_encap.ovrd = IPSEC_DPOVRD_OVRD_TRANSPORT;
		
		/* Header Length according to IPv6/IPv4 */
		if (sap1.flags & IPSEC_FLG_IPV6) { /* IPv6 header */
			/* Get the NH_OFFSET for the last header to encapsulate*/
			dpovrd.transport_encap.nh_offset = 
				ipsec_get_ipv6_nh_offset(
					(struct ipv6hdr *)PARSER_GET_OUTER_IP_POINTER_DEFAULT(),
					&(dpovrd.transport_encap.ip_hdr_len));
		} else { /* IPv4 */
			/* IPv4 Header Length in Bytes */
			dpovrd.transport_encap.ip_hdr_len = ((uint8_t)
				((*((uint8_t *)PARSER_GET_OUTER_IP_POINTER_DEFAULT())) & 
											IPV4_HDR_IHL_MASK)) << 2;
			/* If transport/IPv4 for any non-zero value of NH_OFFSET 
			 * (typically set to 01h), the N byte comes from byte 9 of 
			 * the IP header */
			dpovrd.transport_encap.nh_offset = 0x1;
		}
		
		/* Set the Next Header to ESP (the same for IPv4 and IPv6) */
		dpovrd.transport_encap.next_hdr = IPSEC_IP_NEXT_HEADER_ESP;
	}
	
	/*---------------------*/
	/* ipsec_frame_encrypt */
	/*---------------------*/

	/* Save the original original segment address 
	 * before the (optional) L2 header removal */
	orig_seg_addr = PRC_GET_SEGMENT_ADDRESS();

	/* 	4.	Identify if L2 header exist in the frame: */
	/* Check if Ethernet/802.3 MAC header exist and remove it */
	if (PARSER_IS_ETH_MAC_DEFAULT()) { /* Check if Ethernet header exist */
		
		/* For tunnel mode, update the Ethertype field according to the 
		 * outer header (IPv4/Ipv6), since after SEC encryption
		 * the parser results are not valid any more */
		if (sap1.flags & IPSEC_FLG_TUNNEL_MODE) {
			/* Update the Ethertype according to the outher IP header */
			if (sap1.flags & IPSEC_FLG_OUTER_HEADER_IPV6) {
				*((uint16_t *)PARSER_GET_LAST_ETYPE_POINTER_DEFAULT()) =
						IPSEC_ETHERTYPE_IPV6;
			} else {
				*((uint16_t *)PARSER_GET_LAST_ETYPE_POINTER_DEFAULT()) =
						IPSEC_ETHERTYPE_IPV4;
			}
		}

		/* Save Ethernet header. Note: no swap */
		/* up to 6 VLANs x 4 bytes + 14 regular bytes */
		
		/* Ethernet header length and indicator */ 
		eth_length = (uint8_t)(
						(uint8_t *)PARSER_GET_OUTER_IP_OFFSET_DEFAULT() - 
								(uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT()); 

		segment_pointer = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	
		/* Copy the input frame Ethernet header to workspace */
		memcpy(eth_header, segment_pointer, eth_length);
		
		/* Remove L2 Header */	
		return_val = fdma_replace_default_segment_data(
				(uint16_t)PARSER_GET_ETH_OFFSET_DEFAULT(),
				eth_length,
				NULL,
				0,
				(void *)prc->seg_address,
				128,
				(uint32_t)(FDMA_REPLACE_SA_CLOSE_BIT));
		/* Check FDMA return status */
		if (return_val) {
			ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_ENCRYPT,  /* Function ID */
					IPSEC_FDMA_REPLACE_DEFAULT_SEGMENT_DATA,  /* SR ID */
					__LINE__,
					return_val); /* Error/Status value */
			*enc_status = IPSEC_INTERNAL_ERR;
			return IPSEC_ERROR;
		}
	}
			/*---------------------*/
			/* ipsec_frame_encrypt */
			/*---------------------*/
	
	/* 	5.	Save original FD[FLC], FD[FRC] (to stack) */
	orig_flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	orig_frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
	
	/* Update FD[FRC] for DPOBERD */
	//LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, 0);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, *((uint32_t *)(&dpovrd)));

	/* 	6.	Update the FD[FLC] with the flow context buffer address. */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, IPSEC_FLC_ADDR(desc_addr));	

	/* 	7.	FDMA store default frame command 
	 * (for closing the frame, updating the other FD fields) */
	return_val = fdma_store_default_frame_data();
	/* check FDMA return status */
	if (return_val) {
		ipsec_error_handler(
				ipsec_handle, /* ipsec_handle_t ipsec_handle */
				IPSEC_FRAME_ENCRYPT,  /* Function ID */
				IPSEC_FDMA_STORE_DEFAULT_FRAME_DATA,  /* SR/Hardware ID */
				__LINE__,
				return_val); /* Error/Status value */
		*enc_status = IPSEC_INTERNAL_ERR;
		return IPSEC_ERROR;
	}
	
	/* 	8.	Prepare AAP parameters in the Workspace memory. */
	/* 	8.1.	Use accelerator macros for storing parameters */
	/* 
	* 3 USE_FLC_SP Use Flow Context Storage Profile = 1 
	* The Storage Profile (SP) is embedded as part of the 
	* flow context pointed to by the Flow Context field in the Frame Descriptor 
	* (FD) for this acceleration operation.
	* 
	* 8 OS_EX Ordering Scope Exclusive Phase.
	* 0 Indicates that the accelerator call is not made during the 
	* exclusive phase of an Ordering Scope.
	* 1 Indicates that the accelerator call is made during the 
	* exclusive phase of an Ordering Scope.
	*/
	
	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	/* If in Concurrent ordering scope, move to Exclusive 
	 * (increment scope ID). */ 
	if (scope_status.scope_mode == IPSEC_OSM_CONCURRENT) {
	    /* Move to exclusive */
	    osm_scope_transition_to_exclusive_with_increment_scope_id();
		/* Set OS_EX so AAP will do relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = 
				(IPSEC_AAP_USE_FLC_SP | IPSEC_AAP_OS_EX);
		/* Register as concurrent to avoid an "if" later */
		REGISTER_OSM_CONCURRENT;

	} else {
		/* Call AAP without relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = IPSEC_AAP_USE_FLC_SP;
	}
	
	/* 	9.	Call the AAP */
	__e_hwacceli(AAP_SEC_ACCEL_ID);
	
	/* 	10.	SEC Doing Encryption */
	
			/*---------------------*/
			/* ipsec_frame_encrypt */
			/*---------------------*/
#if(0)
	// Debug //
	{
		fsl_print("IPSEC: Reading FD and FRC after SEC (before FDMA)\n");
		uint32_t j;
		uint32_t val;
		for(j=0;j<8;j++) {
			val = *(uint32_t *)((uint32_t)0x60 + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
		
		//val = *(uint8_t *)((uint32_t)0x60 + 12);
		//fsl_print("FD[BPID] = 0x%x\n", val);
		
		val = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
		fsl_print("FRC = 0x%x\n", val);
		
		val = (uint32_t)LDPAA_FD_GET_FMT(HWC_FD_ADDRESS);
		fsl_print("FMT = 0x%x\n", val);

		// Offset
		//val = *(uint32_t *)((uint32_t)0x60 + 3*4);
		//fsl_print("FD[OFFSET] = 0x%x%x\n", (val & 0x0F), (val & 0xFF00)>>8);
		
		//val = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
		//fsl_print("FD[LENGTH] = 0x%x\n", val);
	}
	// Debug End //
#endif	
	
	/* If the L2 header was removed, the segment address have changed, 
	 * so set the original segment address before opening the new frame 
	 * (for performance optimization it is done in any case) */
	PRC_SET_SEGMENT_ADDRESS(orig_seg_addr); 
			
	/* Update the segment length for the new frame in the presentation context,
	 * for a case where the encap frame header is larger than the original
	 * presentation size (this may happen for relatively short frames
	 * and large outer header in tunnel mode).
	 * Otherwise there will be a parser error later. */
		
	if (PRC_GET_SEGMENT_LENGTH() < (sap1.encap_header_length + eth_length)) {
		PRC_SET_SEGMENT_LENGTH((sap1.encap_header_length + eth_length));
	}

	/* New output buffer mode */
	if (sap1.sec_buffer_mode == IPSEC_SEC_NEW_BUFFER_MODE) { 
		/* In new output buffer mode, clear the PRC ASA Size, 
		 * since the SEC does not preserve the ASA
		 * This is relevant for LS2085 Rev1 only. On Rev2 this macro is
		 * empty. */
		PRC_SET_ASA_SIZE(0);

		/* Update the SPID of the new frame (SEC output) in the HW Context*/
		*((uint8_t *)HWC_SPID_ADDRESS) = sap1.output_spid;

#ifndef  TKT265088_WA_DISABLE
		LDPAA_FD_SET_BPID(HWC_FD_ADDRESS, sap1.bpid);
#endif		
	
	}
	
#if(0)
	// Debug //
	{
		fsl_print("IPSEC: Reading FD and FRC after SEC\n");
		uint32_t j;
		uint32_t val;
		for(j=0;j<8;j++) {
			val = *(uint32_t *)((uint32_t)0x60 + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
		val = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
		fsl_print("FRC = 0x%x\n", val);
		// Offset
		val = *(uint32_t *)((uint32_t)0x60 + 3*4);
		fsl_print("FD[OFFSET] = 0x%x%x\n", (val & 0x0F), (val & 0xFF00)>>8);
		
		val = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
		fsl_print("FD[LENGTH] = 0x%x\n", val);
	}
	// Debug End //
#endif	

	/* 	12.	Read the SEC return status from the FD[FRC]. Use swap macro. */
	if ((LDPAA_FD_GET_FRC(HWC_FD_ADDRESS)) != 0) {
		/* Compressed mode errors */		
		if ((LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) & SEC_COMPRESSED_ERROR_MASK)
													== SEC_COMPRESSED_ERROR) {
			if ((LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) 
					& SEC_DECO_ERROR_MASK_COMPRESSED)
					== SEC_SEQ_NUM_OVERFLOW_COMPRESSED) { 
				/* Sequence Number overflow */
				*enc_status |= IPSEC_SEQ_NUM_OVERFLOW;
			} else {
				*enc_status |= IPSEC_GEN_ENCR_ERR;
			}
		} else {
		/* Non-compressed mode errors */
			if ((LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) & SEC_DECO_ERROR_MASK)
					== SEC_SEQ_NUM_OVERFLOW) { /* Sequence Number overflow */
				*enc_status |= IPSEC_SEQ_NUM_OVERFLOW;
			} else {
				*enc_status |= IPSEC_GEN_ENCR_ERR;
			}
		}
		
		/* Write SEC status to debug area */
		ipsec_error_handler(
			ipsec_handle, /* ipsec_handle_t ipsec_handle */
			IPSEC_FRAME_ENCRYPT,  /* Function ID */
			IPSEC_SEC_HW, /* SR ID */
			__LINE__,
			(int)LDPAA_FD_GET_FRC(HWC_FD_ADDRESS)); /* SEC status */

		return IPSEC_ERROR; /* Exit */
	}
	
	/* 	11.	FDMA present default frame command (open frame) */
	/* because earlier the segment was not presented,
	 * added PRC_RESET_NDS_BIT(); */
	PRC_RESET_NDS_BIT();
	return_val = fdma_present_default_frame();
	/* check for FDMA error */
	if (return_val) {
		/* No error if the frame was just shorter than the segment size */
		if (return_val != FDMA_STATUS_UNABLE_PRES_DATA_SEG) {
			ipsec_error_handler(
				ipsec_handle, /* ipsec_handle_t ipsec_handle */
				IPSEC_FRAME_ENCRYPT,  /* Function ID */
				IPSEC_FDMA_PRESENT_DEFAULT_FRAME,  /* SR/Hardware ID */
				__LINE__,
				return_val); /* Error/Status value */
			*enc_status = IPSEC_INTERNAL_ERR;
			return IPSEC_ERROR;
		}
	}
		
	/* 	14.	Get new running sum and byte count (encrypted/encapsulated frame) 
	 * from the FD[FLC] */
	/* The least significant 6 bytes of the 8-byte FLC in the enqueued FD 
	 * contain a 2-byte checksum and 4-byte encrypted/decrypted byte count.
	 * FLC[63:0] = { 16’b0, checksum[15:0], byte_count[31:0] } 
	 * SEC HW is naturally in little endian and does not swap anything in the FD
	 * For AIOP, the fields are transparent and is just a 8 byte entry,
	 * regardless of what field values are embedded within the 8 byte entry. 
	 * FLC[63:0] indicates that LSB will be at the right most side,
	 * and correspondingly the first entry in memory.
	 * For example: if FLC = 0x0000_C1C2_B1B2_B3B4, then in the memory:
	 * Offset: 0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7
	 * Value:  0xB4 0xB3 0xB2 0xB1 0xC2 0xC1 0x00 0x00.
	 */
	
	/** Load 2 bytes with endian swap.
	 * The address loaded from memory is calculated as: _displ + _base.
	 * _displ - a word aligned constant value between 0-1020.
	 * _base - a variable containing the base address.
	 * If 'base' is a literal 0, the base address is considered as 0. */
	//checksum = (uint16_t)LH_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 4, 0);
	// not using the SEC calculated checksum for encryption
	
	byte_count = LW_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 0, 0);
	
	/* In tunnel mode, optionally set the DSCP field 
	 * This option is not supported by the SEC Era 8 hardware */	
	if (sap1.flags & IPSEC_FLG_ENC_DSCP_SET) {
		/* Set the DS according to the provided outer header, 
		 * and update the IP checksum */
			
		/* Note: the IP header is always at the beginning of the presented 
		 * frame at this stage, which might have been L2 at the original 
		 * frame. Assuming the frame is at beginning of the segment */
		original_val = 
				*((uint32_t *)IPSEC_GET_SEGMENT_ADDRESS(HWC_PRC_ADDRESS));

		if (sap1.flags & IPSEC_FLG_OUTER_HEADER_IPV6) { /* IPv6 header */
			/* IPv6 header: 0-3:Version, 4-11:Traffic Class (4-9: DSCP) */
			/* New first 16 bits of the IP header, including DSCP */
			new_val = (original_val & (~IPSEC_DSCP_MASK_IPV6)) |
					sap1.outer_hdr_dscp;
				
		} else { /* IPv4 */
			/* IPv4 header: 0-3:Version, 4-7:IHL, 8-13:DSCP, 14-15:ECN */				
			/* New first 16 bits of the IP header, including DSCP */
			new_val = (original_val & (~IPSEC_DSCP_MASK_IPV4)) |
					sap1.outer_hdr_dscp;
			
			/* Update the IP header checksum */
			cksum_update_uint32(((uint16_t *) /* uint16_t *cs_ptr */
					((uint32_t)IPSEC_GET_SEGMENT_ADDRESS(HWC_PRC_ADDRESS) + 
								IPSEC_IPV4_CHECKSUM_OFFSET)), 
					original_val, /* uint32_t old_val */
					new_val); /* int32_t new_val */
		}
			
		/*---------------------*/
		/* ipsec_frame_encrypt */
		/*---------------------*/
		
		/* Update the IP header in the segment */
		*((uint32_t *)(IPSEC_GET_SEGMENT_ADDRESS(HWC_PRC_ADDRESS))) = 
				new_val;

		/* Call FDMA Modify */
		fdma_modify_default_segment_data(
				0, /* offset */
				12);  /* uint16_t size 
							12 bytes cover both DSCP and Checksum (IPv4)*/
		
	} /* End of DSCP setting */

	/* 	16.	If L2 header existed in the original frame, add it back: */
	if (eth_length) {
		/* Note: The Ethertype was already updated before removing the 
		 * L2 header */
		return_val = fdma_insert_default_segment_data(
				0, /* uint16_t to_offset */
				eth_header, /* void	 *from_ws_src */
				eth_length, /* uint16_t insert_size */
				FDMA_REPLACE_SA_REPRESENT_BIT 
					/* uint32_t flags */
				);
		/* Check for FDMA error */
		if (return_val) {
			ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_ENCRYPT,  /* Function ID */
					IPSEC_FDMA_INSERT_DEFAULT_SEGMENT_DATA, /* SR/Hardware ID */
					__LINE__,
					return_val); /* Error/Status value */
			*enc_status = IPSEC_INTERNAL_ERR;
			return IPSEC_ERROR;
		}
	}

		/*---------------------*/
		/* ipsec_frame_encrypt */
		/*---------------------*/
	
	/* In transport mode, optionally add UDP encapsulation */
	
	/* UDP-Encapsulated ESP Header Format
	    0                   1                   2                   3
	    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |        Source Port            |      Destination Port         |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |           Length              |           Checksum            |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	   |                      ESP header [RFC2406]                     |
	   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/

	if ((!(sap1.flags & IPSEC_FLG_TUNNEL_MODE)) &&
			(sap1.flags & IPSEC_ENC_OPTS_NAT_EN)) {
	
		/* Get the pointer to the start of the ESP header */
		segment_pointer = (uint8_t *)PARSER_GET_OUTER_IP_POINTER_DEFAULT() +
				dpovrd.transport_encap.ip_hdr_len;
		
		*(uint16_t *)segment_pointer = sap1.udp_src_port;
		*((uint16_t *)segment_pointer + 1) = sap1.udp_dst_port;
		/* UDP length =  
		 * IP total length - IP header length + UDP header length (8) */
		*((uint16_t *)segment_pointer + 2) = (uint16_t)
				(*((uint16_t *)PARSER_GET_OUTER_IP_POINTER_DEFAULT() + 1)) -
				(uint16_t)dpovrd.transport_encap.ip_hdr_len + 8; /* length */
		*((uint16_t *)segment_pointer + 3) = 0; /* Checksum */
		
		/* Change pointer to the IP header */
		segment_pointer = (uint8_t *)PARSER_GET_OUTER_IP_POINTER_DEFAULT();
		
		/* Add the UDP header length (8) to the total IP length */
		*(uint32_t *)segment_pointer += 8;
		
		/* Change the protocol */
		*((uint8_t *)segment_pointer + 9) = 0x11; /* Protocol, byte 9 */
				
		/* Update the IP header checksum (for length and protocol change) */
		/* Change from protocol = ESP (0x32) to UDP (0x11) and increase the 
		 * total length by 8 bytes. Total length is a 16 bit word and 
		 * Protocol is the lower part of a 16 bit word in the IPv4 header. So: 
		 * length + protocol delta = 0x8 + 0x11 */		
		cksum_update_uint32(
				(uint16_t *)segment_pointer + 5, 
				//(uint16_t *)PARSER_GET_OUTER_IP_POINTER_DEFAULT() + 5, 
					/* uint16_t *cs_ptr */ 
				0x32, /* uint32_t old_val (ESP protocol)*/
				(0x8 + 0x11)); /* new_val (delta length + UDP protocol)*/
		
		/* Check if the presentation length is too small for the header
		 * after adding the 8 UDP bytes */
		new_val = (uint32_t)dpovrd.transport_encap.ip_hdr_len + 
				(uint32_t)eth_length + 8 + 8;
		if (PRC_GET_SEGMENT_LENGTH() < new_val) {
			PRC_SET_SEGMENT_LENGTH(new_val);
		}
		
		/* Insert the UDP and change the IP length */	
		return_val = fdma_replace_default_segment_data(
				(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT(), /* to_offset */
				(uint16_t)(dpovrd.transport_encap.ip_hdr_len), /* to_size (original IP size) */
				(void *)segment_pointer, /* void *from_ws_src */
				(uint16_t)(dpovrd.transport_encap.ip_hdr_len + 8), /* from_size (new size)*/
				(void *)prc->seg_address, /* void *ws_dst_rs */
				(uint16_t)PRC_GET_SEGMENT_LENGTH(), /* uint16_t size_rs */
				FDMA_REPLACE_SA_REPRESENT_BIT); /* flags */
		
		/* Check for FDMA error */
		if (return_val) {
			ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_ENCRYPT,  /* Function ID */
					IPSEC_FDMA_REPLACE_DEFAULT_SEGMENT_DATA,/* SR/Hardware ID */
					__LINE__,
					return_val); /* Error/Status value */
			*enc_status = IPSEC_INTERNAL_ERR;
			return IPSEC_ERROR;
		}
		
		// TODO, optionally calculate UDP checksum
	} 

	
#if(0)
	// Debug //
	{
		fsl_print("IPSEC: Reading FD before the parser\n");
		uint32_t j;
		uint32_t val;
		for(j=0;j<8;j++) {
			val = *(uint32_t *)((uint32_t)0x60 + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
		val = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
		fsl_print("FRC = 0x%x\n", val);
		// Offset
		val = *(uint32_t *)((uint32_t)0x60 + 3*4);
		fsl_print("FD[OFFSET] = 0x%x%x\n", (val & 0x0F), (val & 0xFF00)>>8);
		
		val = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
		fsl_print("FD[LENGTH] = 0x%x\n", val);
	}
	// Debug End //
#endif	
	
	/* 	Set the gross running to 0 (invalidate) 
	 * There will be no checksum calculation with PARSER_NO_FLAGS */
	pr->gross_running_sum = 0;
	
	/* 	Run parser and check for errors. */
	return_val = parse_result_generate_default(PARSER_NO_FLAGS);
	/* Check for Parser error */
	if (return_val) {
		ipsec_error_handler(
				ipsec_handle, /* ipsec_handle_t ipsec_handle */
				IPSEC_FRAME_ENCRYPT,  /* Function ID */
				IPSEC_PARSE_RESULT_GENERATE_DEFAULT, /* SR/Hardware ID */
				__LINE__,
				return_val); /* Error/Status value */
		*enc_status = IPSEC_INTERNAL_ERR;
		return IPSEC_ERROR;
	}
	
#if(0)
	// Debug //
	{
		fsl_print("IPSEC: Reading Parser results after encryption\n");
		uint32_t j;
		uint32_t val;
		for(j=0;j<12;j++) {
			val = *(uint32_t *)((uint32_t)0x80 + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
		val = (uint32_t)((uint8_t *)PARSER_GET_L5_OFFSET_DEFAULT());
		fsl_print("PARSER_GET_L5_OFFSET_DEFAULT = 0x%x\n", val);
		val = (uint32_t)((uint8_t *)PARSER_GET_OUTER_IP_OFFSET_DEFAULT());	
		fsl_print("PARSER_GET_OUTER_IP_OFFSET_DEFAULT = 0x%x\n", val);
		
	}
	// Debug End //
#endif	
	
	/* 	17.	Restore the original FD[FLC], FD[FRC] (from stack). 
	 * No need for additional FDMA command. */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, orig_flc);	
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, orig_frc)	
	
	/* 	18.	Handle lifetime counters */
		/* 	18.1.	Read lifetime counters (CDMA) */
		/* 	18.2.	Add byte-count from SEC and one packet count. */
		/* 	18.4.	Update the kilobytes and/or packets lifetime counters 
		 * (STE increment + accumulate). */
	
	/* always count */
	ste_inc_and_acc_counters(
			IPSEC_PACKET_COUNTER_ADDR(desc_addr), /* uint64_t counter_addr */
			byte_count,	/* uint32_t acc_value */
			/* uint32_t flags */
			(STE_MODE_COMPOUND_64_BIT_CNTR_SIZE |
			STE_MODE_COMPOUND_64_BIT_ACC_SIZE |
			STE_MODE_COMPOUND_CNTR_SATURATE |
			STE_MODE_COMPOUND_ACC_SATURATE));
	
	/* 	19.	END */
	return IPSEC_SUCCESS;

} /* End of ipsec_frame_encrypt */

/**************************************************************************//**
* ipsec_frame_decrypt
*//****************************************************************************/
__IPSEC_HOT_CODE int ipsec_frame_decrypt(
		ipsec_handle_t ipsec_handle,
		uint32_t *dec_status
		)
{
	int return_val;
	uint8_t eth_header[40]; /* Ethernet header place holder, 40 bytes */ 
	uint8_t eth_length = 0; /* Ethernet header length and indicator */ 
	uint64_t orig_flc; /* Original FLC */
	uint32_t orig_frc;
	uint16_t orig_seg_addr;
	uint16_t outer_material_length;
	uint8_t *segment_pointer;
	uint32_t byte_count;
	uint32_t checksum;
	uint8_t dont_decrypt = 0;
	ipsec_handle_t desc_addr;
	uint16_t orig_seg_length;
	uint16_t orig_seg_offset;
	uint8_t pad_length;
	uint16_t end_seg_len;

	struct ipsec_sa_params_part1 sap1; /* Parameters to read from ext buffer */
	struct scope_status_params scope_status;

	struct dpovrd_general dpovrd;
	//struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	
	struct   presentation_context *prc =
					(struct presentation_context *) HWC_PRC_ADDRESS;

	*dec_status = 0; /* Initialize */
	
	/* 	Inbound frame decryption and decapsulation */
	
	desc_addr = IPSEC_DESC_ADDR(ipsec_handle);

	/* 	2.	Read relevant descriptor fields with CDMA. */
	cdma_read(
			&sap1, /* void *ws_dst */
			desc_addr, /* uint64_t ext_address */
			sizeof(sap1) /* uint16_t size */
			);
	
	/*---------------------*/
	/* ipsec_frame_decrypt */
	/*---------------------*/
	
	/* 	3.	Check that hard kilobyte/packet/seconds lifetime limits 
	 * have expired. If expired, return with error. go to END */

	/* The seconds lifetime status is checked in the params[status] 
	 * and the kilobyte/packet status is checked from the params[counters].
	 * This is done to avoid doing mutex lock for kilobyte/packet status */
	/* Seconds Lifetime */
	if (sap1.soft_sec_expired) {
		if (sap1.flags & IPSEC_FLG_LIFETIME_SEC_CNTR_EN) {
			if (sap1.soft_sec_expired) {
				*dec_status |= IPSEC_STATUS_SOFT_SEC_EXPIRED;
				if (sap1.hard_sec_expired) {
					*dec_status |= IPSEC_STATUS_HARD_SEC_EXPIRED;
					return IPSEC_ERROR;
				}
			}
		}
	}
	
	/* KB lifetime counters */
	if (sap1.flags & IPSEC_FLG_LIFETIME_KB_CNTR_EN) {
		if (sap1.byte_counter >= sap1.soft_byte_limit) {
			*dec_status |= IPSEC_STATUS_SOFT_KB_EXPIRED;
			if (sap1.byte_counter >= sap1.hard_byte_limit) {
				*dec_status |= IPSEC_STATUS_HARD_KB_EXPIRED;
				return IPSEC_ERROR;
			}
		}
	}

	/* Packets lifetime counters*/
	if (sap1.flags & IPSEC_FLG_LIFETIME_PKT_CNTR_EN) {
		if (sap1.packet_counter >= sap1.soft_packet_limit) {
			*dec_status |= IPSEC_STATUS_SOFT_PACKET_EXPIRED;
			if (sap1.packet_counter >= sap1.hard_packet_limit) {
				*dec_status |= IPSEC_STATUS_HARD_PACKET_EXPIRED;
				return IPSEC_ERROR;
			}
		}	 
	}
	
			/*---------------------*/
			/* ipsec_frame_decrypt */
			/*---------------------*/
	
	/* Save the original segment address, before the (optional) 
	 * L2 header removal */
	orig_seg_addr = PRC_GET_SEGMENT_ADDRESS();
	
	/* 	4.	Identify if L2 header exist in the frame, 
	 * and if yes get the L2 header length. */
	if (PARSER_IS_ETH_MAC_DEFAULT()) { /* Check if Ethernet header exist */
		/* Note: For tunnel mode decryption there is no need to update 
		 * the Ethertype field, since SEC HW is doing it */
		
		/* Ethernet header length and indicator */ 
		eth_length = (uint8_t)
				((uint8_t *)PARSER_GET_OUTER_IP_OFFSET_DEFAULT() - 
								(uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT()); 
	}
	
	/* Prepare DPOVRD Parameters */
	/* For transport mode: IP header length, Next header offset */
	/* For tunnel mode: 
	 * IP header length, Actual Outer IP Header Offset (AOIPHO), including L2 */
	if (sap1.flags & IPSEC_FLG_TUNNEL_MODE) {
		/* Prepare DPOVRD */
		/* 31 OVRD
		 * 30-20 Reserved
		 * 19-12 AOIPHO: Actual Outer IP Header Offset. 
		 * 		AOIPHO indicates the number of bytes of material in the 
		 * 		Input Frame prior to the actual Outer IP Header.
		 * 11-0 Outer IP Header Material Length 
		 * 		Length for the Outer IP Header Material (in bytes). 
		 * 		This field indicates the total length of the material that 
		 * 		includes the Outer IP Header, up to but not including the 
		 * 		ESP Header.
		*/
		outer_material_length = (uint16_t)
			((uint32_t)((uint8_t *)PARSER_GET_L5_OFFSET_DEFAULT()) - 
				(uint32_t)((uint8_t *)PARSER_GET_OUTER_IP_OFFSET_DEFAULT()) +
				eth_length); 
		
		dpovrd.tunnel_decap.word = 
				IPSEC_DPOVRD_OVRD |
				(eth_length<<12) | /* AOIPHO */
				outer_material_length; /* Outer IP Header Material Length */

		/*---------------------*/
		/* ipsec_frame_decrypt */
		/*---------------------*/

	} else { /* Transport Mode */
		/* For Transport mode set DPOVRD */
		/* 31 OVRD, 30-28 Reserved, 27-24 ECN (Not relevant for transport mode)
		 * 23-16 IP Header Length in bytes, 
		* of the portion of the IP header that is not encrypted.
		* 15-8 NH_OFFSET - location of the next header within the IP header.
		* 7-0 Reserved */
		dpovrd.transport_decap.ovrd = IPSEC_DPOVRD_OVRD_TRANSPORT;

		if (sap1.flags & IPSEC_FLG_IPV6) { /* IPv6 header */
			/* Get the NH_OFFSET for the last header before ESP */
			dpovrd.transport_decap.nh_offset = 
				ipsec_get_ipv6_nh_offset(
					(struct ipv6hdr *)PARSER_GET_OUTER_IP_POINTER_DEFAULT(),
					&(dpovrd.transport_decap.ip_hdr_len));
			
		} else { /* IPv4 */
			/* If transport/IPv4 for any non-zero value of NH_OFFSET 
			 * (typically set to 01h), the N byte comes from byte 9 of 
			 * the IP header */
			dpovrd.transport_decap.nh_offset = 0x1;
			
			/* Header Length up to ESP */
			dpovrd.transport_decap.ip_hdr_len = (uint8_t)
				((uint32_t)((uint8_t *)PARSER_GET_L5_OFFSET_DEFAULT()) - 
					(uint32_t)
						((uint8_t *)PARSER_GET_OUTER_IP_OFFSET_DEFAULT()));
			
			/* In transport mode, if the packet is UDP encapsulated,
			 * remove the UDP header. IPv4[protocol] = byte #9 */
			/* Get the pointer to the start of the ESP header */
			segment_pointer = 
					(uint8_t *)PARSER_GET_OUTER_IP_POINTER_DEFAULT();
			if (*(segment_pointer + 9) == 0x11) { /* If UDP */

				/* Remove the UDP length (according to the parser result) */
				dpovrd.transport_decap.ip_hdr_len -= 8; 
				
				/* Reduce the UDP header length (8) from the total IP length */
				*(uint32_t *)segment_pointer -= 8;
				
				/* Change the protocol to ESP */
				*(segment_pointer + 9) = 0x32; 
						
				/* Update the IP header checksum (for length/protocol change) */
				/* Change from protocol = UDP (0x11) to ESP (0x32) and decrease 
				 * the total length by 8 bytes. Total length is a 16 bit word 
				 * and Protocol is the lower part of a 16 bit word in the 
				 * IPv4 header. So: length + protocol delta = 0x32 - 0x8 */		
				cksum_update_uint32(
						(uint16_t *)PARSER_GET_OUTER_IP_POINTER_DEFAULT() + 5, 
							/* uint16_t *cs_ptr */ 
						0x11, /* uint32_t old_val (UDP protocol)*/
						(0x32 - 0x8)); /* new_val (ESP protocol - length)*/
				
				/* Remove the UDP header and change the IP length */	
				return_val = fdma_replace_default_segment_data(
					(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT(), 
						/* to_offset (IP header)*/
					(uint16_t)(dpovrd.transport_decap.ip_hdr_len + 8), 
						/* to_size (original IP+UDP size) */
					(void *)segment_pointer, /* void *from_ws_src */
					(uint16_t)(dpovrd.transport_decap.ip_hdr_len), 
						/* from_size (new size)*/
					(void *)prc->seg_address, /* void *ws_dst_rs */
					(uint16_t)(PRC_GET_SEGMENT_LENGTH()), /* uint16_t size_rs */
					FDMA_REPLACE_SA_REPRESENT_BIT); /* flags */
				
				/* check FDMA return status */
				if (return_val) {
					ipsec_error_handler(
							ipsec_handle, /* ipsec_handle_t ipsec_handle */
							IPSEC_FRAME_DECRYPT,  /* Function ID */
							IPSEC_FDMA_REPLACE_DEFAULT_SEGMENT_DATA, /* SR ID */
							__LINE__,
							return_val); /* Error/Status value */
					*dec_status = IPSEC_INTERNAL_ERR;
					return IPSEC_ERROR;
				}
								
				/* TODO: possibly unify FDMA replace for both UDP and L2
				 * headers removal */
			}
		}
		
		dpovrd.transport_decap.reserved = 0;

		/* 	If L2 header exist in the frame, save it and remove from frame */
		if (eth_length) {
		/* Save Ethernet header. Note: no swap */
		/* up to 6 VLANs x 4 bytes + 14 regular bytes */
			segment_pointer = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
			/* Copy the input frame Ethernet header to workspace */
			memcpy(eth_header, segment_pointer, eth_length);

			/* Remove L2 Header */	
			return_val = fdma_replace_default_segment_data(
					(uint16_t)PARSER_GET_ETH_OFFSET_DEFAULT(),
					eth_length,
					NULL,
					0,
					(void *)prc->seg_address,
					128,
					(uint32_t)(FDMA_REPLACE_SA_CLOSE_BIT));
			/* Check FDMA return status */
			if (return_val) {
				ipsec_error_handler(
						ipsec_handle, /* ipsec_handle_t ipsec_handle */
						IPSEC_FRAME_DECRYPT,  /* Function ID */
						IPSEC_FDMA_REPLACE_DEFAULT_SEGMENT_DATA, /* SR ID */
						__LINE__,
						return_val); /* Error/Status value */
				*dec_status = IPSEC_INTERNAL_ERR;
				return IPSEC_ERROR;
			}
			
			/* This is done here because L2 is removed only in Transport */
			PRC_RESET_NDS_BIT(); 
			
		}
	}

#if(0)
	// Debug //
	{
		fsl_print("IPSEC: Reading FD and FRC before SEC\n");
		uint32_t j;
		uint32_t val;
		for(j=0;j<8;j++) {
			val = *(uint32_t *)((uint32_t)0x60 + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
		val = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
		fsl_print("FRC = 0x%x\n", val);
		
		// Offset
		val = *(uint32_t *)((uint32_t)0x60 + 3*4);
		//val = LDPAA_FD_GET_OFFSET(HWC_FD_ADDRESS);
		//fsl_print("FD[OFFSET] = 0x%x %x\n", (val & 0xFF), (val & 0xFF00));
		fsl_print("FD[OFFSET] = 0x%x%x\n", (val & 0x0F), (val & 0xFF00)>>8);
		fsl_print("DPOVRD = 0x%x\n", *((uint32_t *)(&dpovrd)));

	}
	// Debug End //
#endif

	/*---------------------*/
	/* ipsec_frame_decrypt */
	/*---------------------*/
	
	/* 	5.	Save original FD[FLC], FD[FRC] (to stack) */
	orig_flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	orig_frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
	
	/* 	6.	Update the FD[FLC] with the flow context buffer address. */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, IPSEC_FLC_ADDR(desc_addr));	
	
	/* 7.	Update the FD[FRC] with SEC DPOVRD parameters */
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, *((uint32_t *)(&dpovrd)));

	/* 	8.	FDMA store default frame command 
	 * (for closing the frame, updating the other FD fields) */
	return_val = fdma_store_default_frame_data();
	/* check FDMA return status */
	if (return_val) {
		ipsec_error_handler(
				ipsec_handle, /* ipsec_handle_t ipsec_handle */
				IPSEC_FRAME_DECRYPT,  /* Function ID */
				IPSEC_FDMA_STORE_DEFAULT_FRAME_DATA, /* SR ID */
				__LINE__,
				return_val); /* Error/Status value */
		*dec_status = IPSEC_INTERNAL_ERR;
		return IPSEC_ERROR;
	}
	
	/* 	9.	Prepare AAP parameters in the Workspace memory. */
	/* 3 USE_FLC_SP Use Flow Context Storage Profile = 1 */ 
	/* 8 OS_EX Ordering Scope Exclusive Phase.
	* 0 Indicates that the accelerator call is not made during the 
	* exclusive phase of an Ordering Scope.
	* 1 Indicates that the accelerator call is made during the 
	* exclusive phase of an Ordering Scope.
	*/
	
	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	/* If in Concurrent ordering scope, move to Exclusive 
	 * (increment scope ID). */ 
	if (scope_status.scope_mode == IPSEC_OSM_CONCURRENT) {
	    /* Move to exclusive */
	    osm_scope_transition_to_exclusive_with_increment_scope_id();
		/* Set OS_EX so AAP will do relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = 
				(IPSEC_AAP_USE_FLC_SP | IPSEC_AAP_OS_EX);
		/* Register concurrent here to save the "if" later */
		REGISTER_OSM_CONCURRENT;

	} else {
		/* Call AAP without relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = IPSEC_AAP_USE_FLC_SP;
	}
		
	/* 	10.	Call the AAP */
	__e_hwacceli(AAP_SEC_ACCEL_ID);

	/* 	11.	SEC Doing Decryption */

	/*---------------------*/
	/* ipsec_frame_decrypt */
	/*---------------------*/
	
	/* If the L2 header was removed, the segment address have changed, 
	 * so set the original segment address before opening the new frame. 
	 * (for performance optimization it is done in any case) */
	PRC_SET_SEGMENT_ADDRESS(orig_seg_addr); 
	
	/* Update the default segment length for the new frame  in 
	 * the presentation context */
	//PRC_SET_SEGMENT_LENGTH(DEFAULT_SEGMENT_SIZE);
	
	/* New output buffer mode */
	if (sap1.sec_buffer_mode == IPSEC_SEC_NEW_BUFFER_MODE) { 
		/* In new output buffer mode, clear the PRC ASA Size, 
		 * since the SEC does not preserve the ASA
		 * This is relevant for LS2085 Rev1 only. On Rev2 this macro is
		 * empty. */
		PRC_SET_ASA_SIZE(0);
			
		/* Update the SPID of the new frame (SEC output) in the HW Context*/
		*((uint8_t *)HWC_SPID_ADDRESS) = sap1.output_spid;

#ifndef  TKT265088_WA_DISABLE
		LDPAA_FD_SET_BPID(HWC_FD_ADDRESS, sap1.bpid);
#endif	
		
	}	
		
#if(0)
	// Debug //
	{
		fsl_print("IPSEC: Reading FD and FRC after SEC\n");
		uint32_t j;
		uint32_t val;
		for(j=0;j<8;j++) {
			val = *(uint32_t *)((uint32_t)0x60 + j*4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
		val= LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
		fsl_print("FRC = 0x%x\n", val);
		// Offset
		//val = *(uint32_t *)((uint32_t)0x60 + 3*4);
		//fsl_print("FD[OFFSET] = 0x%x%x\n", (val & 0x0F), (val & 0xFF00)>>8);
	}
	// Debug End //
#endif
		/*---------------------*/
		/* ipsec_frame_decrypt */
		/*---------------------*/
	
	/* 	13.	Read the SEC return status from the FD[FRC]. Use swap macro. */
	if ((LDPAA_FD_GET_FRC(HWC_FD_ADDRESS)) != 0) {
		/* Compressed mode errors */		
		if ((LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) & SEC_COMPRESSED_ERROR_MASK)
													== SEC_COMPRESSED_ERROR) {
			if ((LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) & 
							SEC_CCB_ERROR_MASK_COMPRESSED) == 
									SEC_ICV_COMPARE_FAIL_COMPRESSED) {
				*dec_status |= IPSEC_ICV_COMPARE_FAIL;
			} else {
				switch (LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) & 
						SEC_DECO_ERROR_MASK_COMPRESSED) {
					case SEC_SEQ_NUM_OVERFLOW_COMPRESSED: 
						/** Sequence Number overflow */
						*dec_status |= IPSEC_SEQ_NUM_OVERFLOW;
						break;
					case SEC_AR_LATE_PACKET_COMPRESSED: 
						/* Anti Replay Check: Late packet */
						*dec_status |= IPSEC_AR_LATE_PACKET;
						break;
					case SEC_AR_REPLAY_PACKET_COMPRESSED: 
						/*Anti Replay Check: Replay packet */
						*dec_status |= IPSEC_AR_REPLAY_PACKET;
						break;
					default:
						*dec_status |= IPSEC_GEN_DECR_ERR;
				}	
			}	
		} else {
		/* Non-compressed mode errors */	
			if ((LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) & SEC_CCB_ERROR_MASK)
											== SEC_ICV_COMPARE_FAIL) {
				*dec_status |= IPSEC_ICV_COMPARE_FAIL;
			} else {
				switch (LDPAA_FD_GET_FRC(HWC_FD_ADDRESS) & 
													SEC_DECO_ERROR_MASK) {
					case SEC_SEQ_NUM_OVERFLOW: /* Sequence Number overflow */
						*dec_status |= IPSEC_SEQ_NUM_OVERFLOW;
						break;
					case SEC_AR_LATE_PACKET: 
						/* Anti Replay Check: Late packet */
						*dec_status |= IPSEC_AR_LATE_PACKET;
						break;
					case SEC_AR_REPLAY_PACKET: 
						/* Anti Replay Check: Replay packet */
						*dec_status |= IPSEC_AR_REPLAY_PACKET;
						break;
					default:
						*dec_status |= IPSEC_GEN_DECR_ERR;
				}	
			}
		}
		
		/* Write SEC status to debug area */
		ipsec_error_handler(
			ipsec_handle, /* ipsec_handle_t ipsec_handle */
			IPSEC_FRAME_DECRYPT,  /* Function ID */
			IPSEC_SEC_HW, /* SR ID */
			__LINE__,
			(int)LDPAA_FD_GET_FRC(HWC_FD_ADDRESS)); 
										/* SEC status */
		return IPSEC_ERROR; /* Exit */
	}

	/*---------------------*/
	/* ipsec_frame_decrypt */
	/*---------------------*/

	/* Check ESP padding for Transport mode */
	/* When IPSEC_FLG_TRANSPORT_PAD_CHECK is enabled, AOFL=0 which means
	 * that the SEC output frame includes also the ESP trailer, 
	 * where the last two bytes are Pad Length and Next Header.
	 * The default padding values are is 1,2,3,..., and the last
	 * padding byte is equal to the pad length value. 
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                   IP header + Payload                         |
	~                                                               ~
	|                                                               |
	+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|               |     Padding (0-255 bytes)                     |
	+-+-+-+-+-+-+-+-+               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               |  Pad Length   | Next Header   |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	*/
	if (sap1.flags & IPSEC_FLG_TRANSPORT_PAD_CHECK) {
		/* Save original presentation length and offset */
		orig_seg_length = PRC_GET_SEGMENT_LENGTH();
		orig_seg_offset = PRC_GET_SEGMENT_OFFSET();

		/* Calculate the required presentation size from the end of the frame, 
		 * in case the frame is shorter than the presentation size */
		if (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) > orig_seg_length) {
			end_seg_len = orig_seg_length;
		} else {
			end_seg_len = (uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
		}

		/* Set length and offset to present at the end of the frame */
		PRC_SET_SEGMENT_LENGTH(end_seg_len);
		PRC_SET_SEGMENT_OFFSET(end_seg_len);		
		
		/* Present from the end */
		PRC_SET_SR_BIT();
			
		return_val = fdma_present_default_frame();
		/* Check FDMA return status */
		if (return_val) {
			/* No error if the frame was just shorter than the segment size */
			if (return_val != FDMA_STATUS_UNABLE_PRES_DATA_SEG) {
				ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_DECRYPT,  /* Function ID */
					IPSEC_FDMA_PRESENT_DEFAULT_FRAME, /* SR ID */
					__LINE__,
					return_val); /* Error/Status value */
				*dec_status = IPSEC_INTERNAL_ERR;
				return IPSEC_ERROR;
			}	
		}

		/* Get the pad length byte */
		segment_pointer = (uint8_t *)orig_seg_addr + end_seg_len - 2;
		pad_length = *segment_pointer;

		/* Point to the last pad */
		segment_pointer--;
			
		/* Note: only supporting pad check within a single presentation */
		while ((pad_length > 0) && 
			(segment_pointer >=	(uint8_t *)orig_seg_addr)) {	
				if (*segment_pointer != pad_length) {
					ipsec_error_handler(
						ipsec_handle, /* ipsec_handle_t ipsec_handle */
						IPSEC_FRAME_DECRYPT, /* Function ID */
						IPSEC_INTERNAL_SERVICE, /* SR ID */
						__LINE__,
						IPSEC_INT_TRANSPORT_PAD_CHECK_ERR); /* Error value */

					*dec_status |= IPSEC_GEN_DECR_ERR;
					return IPSEC_ERROR;
				} 

				segment_pointer--;
				pad_length--;
		}
		
		/* If 'pad_length' != 0, it means the presentation size was too small 
		 * for the entire padding, and this is not supported. Return an error */
		if(pad_length) {
			ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_DECRYPT, /* Function ID */
					IPSEC_INTERNAL_SERVICE, /* SR ID */
					__LINE__,
					IPSEC_INT_ERR_PAD_TOO_LONG); /* Error/Status value */

			*dec_status |= IPSEC_INTERNAL_ERR;
			return IPSEC_ERROR;
		}
		
		/* Re-read the pad length, because it was subtracted in the while loop*/
		pad_length = *((uint8_t *)orig_seg_addr + end_seg_len - 2);

		/* Remove the ESP trailer */	
		return_val = fdma_replace_default_segment_data(
				(uint16_t)PARSER_GET_ETH_OFFSET_DEFAULT(),/* to_offset */
				end_seg_len, /* to_size (original size) */
				(void *)orig_seg_addr,
				(end_seg_len - (uint16_t)pad_length -2), 
				/* from_size (new size), remove the ESP trailer */
				(void *)prc->seg_address,
				(uint16_t)PRC_GET_SEGMENT_LENGTH(),
				(uint32_t)(FDMA_REPLACE_SA_CLOSE_BIT));
		/* Check FDMA return status */
		if (return_val) {
			ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_DECRYPT, /* Function ID */
					IPSEC_FDMA_REPLACE_DEFAULT_SEGMENT_DATA, /* SR ID */
					__LINE__,
					return_val); /* Error/Status value */
			*dec_status = IPSEC_INTERNAL_ERR;
			return IPSEC_ERROR;
		}
			
		/* Present from the beginning of the frame */
		PRC_RESET_SR_BIT();
		PRC_SET_SEGMENT_LENGTH(orig_seg_length);
		PRC_SET_SEGMENT_OFFSET(orig_seg_offset);		
			
		return_val = fdma_present_default_frame_segment(
				FDMA_PRES_NO_FLAGS, /* uint32_t flags */
				(void *)orig_seg_addr, /* void	 *ws_dst */
				orig_seg_offset, /* uint16_t offset */
				orig_seg_length /* uint16_t present_size */
				);
		
		/* Check FDMA return status */
		if (return_val) {
			/* No error if the frame was just shorter than the segment size */
			if (return_val != FDMA_STATUS_UNABLE_PRES_DATA_SEG) {
				ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_DECRYPT, /* Function ID */
					IPSEC_FDMA_PRESENT_DEFAULT_FRAME_SEGMENT, /* SR ID */
					__LINE__,
					return_val); /* Error/Status value */
				*dec_status = IPSEC_INTERNAL_ERR;
				return IPSEC_ERROR;
			}	
		}
		
		/* End of TRANSPORT PAD CHECK section */
	} else {
		/* Present for no transport pad check case */ 
		return_val = fdma_present_default_frame();
		/* Check FDMA return status */
		if (return_val) {
			/* No error if the frame was just shorter than the segment size */
			if (return_val != FDMA_STATUS_UNABLE_PRES_DATA_SEG) {
				ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_DECRYPT, /* Function ID */
					IPSEC_FDMA_PRESENT_DEFAULT_FRAME, /* SR ID */
					__LINE__,
					return_val); /* Error/Status value */
				*dec_status = IPSEC_INTERNAL_ERR;
				return IPSEC_ERROR;
			}	
		}		
	}
	
	/* 	15.	Get new running sum and byte count (encrypted/encapsulated frame) 
	 * from the FD[FLC] */
		
		/*---------------------*/
		/* ipsec_frame_decrypt */
		/*---------------------*/
	
	/* FLC structure:
	 * A 32-bit byte count is stored in the LS portion of the FLC in LE format.
	 * A 2-byte checksum is stored starting at offset 4 relative to the 
	 * beginning of the FLC.
	 * FLC[63:0] = { 16’b0, checksum[15:0], byte_count[31:0] }
	 * SEC HW is naturally in little endian and does not swap anything in the FD
	 * For AIOP, the fields are transparent and is just a 8 byte entry,
	 * regardless of what field values are embedded within the 8 byte entry. 
	 * FLC[63:0] indicates that LSB will be at the right most side,
	 * and correspondingly the first entry in memory.
	 * For example: if FLC = 0x0000_C1C2_B1B2_B3B4, then in the memory:
	 * Offset: 0x0  0x1  0x2  0x3  0x4  0x5  0x6  0x7
	 * Value:  0xB4 0xB3 0xB2 0xB1 0xC2 0xC1 0x00 0x00.
	*/
	
	/* checksum and bytecount from SEC */
	checksum = LH_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 4, 0);
	
	byte_count = LW_SWAP(HWC_FD_ADDRESS + FD_FLC_DS_AS_CS_OFFSET + 0, 0);	
	/* 	16.	Update the gross running checksum in the Workspace parser results.*/
	//pr->gross_running_sum = 0;
	// TODO: currently setting to 0 (invalid), so parser will call
	// FDMA to recalculate the gross running sum
	// Later need to manipulate the checksum returned from SEC
	
	// TODO: special handling of running sum in transport mode
	// since L2 header was removed
	
	/* In Transport mode, if L2 header existed in the original frame, 
	 * add it back */
	if ((!(sap1.flags & IPSEC_FLG_TUNNEL_MODE)) && eth_length) {
		/* Note: The Ethertype was already updated before removing the 
		 * L2 header */
		return_val = fdma_insert_default_segment_data(
				0, /* uint16_t to_offset */
				eth_header, /* void	 *from_ws_src */
				eth_length, /* uint16_t insert_size */
				FDMA_REPLACE_SA_REPRESENT_BIT 
					/* uint32_t flags */
				);
		
		/* Check FDMA return status */
		if (return_val) {
			ipsec_error_handler(
					ipsec_handle, /* ipsec_handle_t ipsec_handle */
					IPSEC_FRAME_DECRYPT,  /* Function ID */
					IPSEC_FDMA_INSERT_DEFAULT_SEGMENT_DATA, /* SR ID */
					__LINE__,
					return_val); /* Error/Status value */
			*dec_status = IPSEC_INTERNAL_ERR;
			return IPSEC_ERROR;
		}
		
		/* TODO: Update the gross running sum if adding L2 header */
	}
	
			/*---------------------*/
			/* ipsec_frame_decrypt */
			/*---------------------*/
	
	/* 	17.	Run parser and check for errors. */
	//return_val = parse_result_generate_default(PARSER_VALIDATE_L3_L4_CHECKSUM);
	return_val = parse_result_generate_default(PARSER_NO_FLAGS);
	
	/* Check the Parser return status for frame validity */
	if (return_val) {
		if (return_val == -EIO) { /* Frame validity fail */
			*dec_status = IPSEC_DECR_VALIDITY_ERR;
		} else { /* Other parser error */
			*dec_status = IPSEC_INTERNAL_ERR;
		}
		
		/* Get the detailed parser error code */
		return_val = PARSER_GET_PARSE_ERROR_CODE_DEFAULT();
		
		ipsec_error_handler(
				ipsec_handle, /* ipsec_handle_t ipsec_handle */
				IPSEC_FRAME_DECRYPT,  /* Function ID */
				IPSEC_PARSE_RESULT_GENERATE_DEFAULT, /* SR ID */
				__LINE__,
				return_val); /* Error/Status value */

		return IPSEC_ERROR;
	}
	
	/* 	19.	Restore the original FD[FLC], FD[FRC] (from stack) */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, orig_flc);	
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, orig_frc)	

	/* 	20.	Handle lifetime counters */
	/* 	20.1.	Read lifetime counters (CDMA) */
	/* 	20.2.	Add byte-count from SEC and one packet count. */
	/* 	20.3.	Calculate locally if lifetime counters crossed the limits. 
	 * If yes set flag in the descriptor statistics (CDMA write). */
	/* 	20.4.	Update the kilobytes and/or packets lifetime counters 
	 * (STE increment + accumulate). */
	/* always count */
	ste_inc_and_acc_counters(
			IPSEC_PACKET_COUNTER_ADDR(desc_addr), /* uint64_t counter_addr */
			byte_count,	/* uint32_t acc_value */
			(STE_MODE_COMPOUND_64_BIT_CNTR_SIZE |
			STE_MODE_COMPOUND_64_BIT_ACC_SIZE |
			STE_MODE_COMPOUND_CNTR_SATURATE |
			STE_MODE_COMPOUND_ACC_SATURATE)); /* uint32_t flags */
	
	/* END */
	return IPSEC_SUCCESS;
} /* End of ipsec_frame_decrypt */

/**************************************************************************//**
	ipsec_get_lifetime_stats
*//****************************************************************************/
int ipsec_get_lifetime_stats(
		ipsec_handle_t ipsec_handle,
		uint64_t *kilobytes,
		uint64_t *packets,
		uint32_t *sec)
{
	
	uint64_t current_timestamp;
	ipsec_handle_t desc_addr;

	/* Note: this struct must be equal to the head of ipsec_sa_params_part1 */
	struct counters_and_timestamp {
		uint64_t packet_counter; /*	Packets counter, 8B */
		uint64_t byte_counter; /* Encrypted/decrypted bytes counter, 8B */
		uint64_t timestamp; /* TMAN timestamp in micro-seconds, 8 Bytes */
	} ctrs;
	
	desc_addr = IPSEC_DESC_ADDR(ipsec_handle);

	/* Flush all the counter updates that are pending in the 
	 * statistics engine request queue. */
	ste_barrier();

	/* 	Read relevant descriptor fields with CDMA. */
	cdma_read(
			&ctrs, /* void *ws_dst */
			desc_addr, /* uint64_t ext_address */
			sizeof(ctrs) /* uint16_t size */
			);
	
	*packets = ctrs.packet_counter;
	*kilobytes =  ctrs.byte_counter;
	
	/* Get current timestamp from TMAN (in micro-seconds)*/
	tman_get_timestamp(&current_timestamp);

	/* Calculate elapsed time in seconds */
	/* Do shift 20, since 2^20 = 1,048,576 */
	if (current_timestamp >= ctrs.timestamp) { /* No roll-over */
		*sec = (uint32_t)((current_timestamp - ctrs.timestamp)>>20);
	} else { /* Roll-over */
		*sec = (uint32_t)(
				(current_timestamp + 
						(IPSEC_MAX_TIMESTAMP - ctrs.timestamp) + 1)>>20);
	}

	return IPSEC_SUCCESS;
	
} /* End of ipsec_get_lifetime_stats */

/**************************************************************************//**
	ipsec_decr_lifetime_counters
*//****************************************************************************/
int ipsec_decr_lifetime_counters(
		ipsec_handle_t ipsec_handle,
		uint32_t kilobytes_decr_val,
		uint32_t packets_decr_val
		)
{
	/* Note: there is no check of counters enable, nor current value.
	 * Assuming that it is only called appropriately by the upper layer */
	ipsec_handle_t desc_addr;

	desc_addr = IPSEC_DESC_ADDR(ipsec_handle);

	/* Flush all the counter updates that are pending in the 
	 * statistics engine request queue. */
	ste_barrier();
	
	if (kilobytes_decr_val) {
		ste_dec_counter(
				//IPSEC_KB_COUNTER_ADDR,
				IPSEC_KB_COUNTER_ADDR(desc_addr),
				kilobytes_decr_val,
				(STE_MODE_SATURATE | STE_MODE_64_BIT_CNTR_SIZE));
	}
	
	if (packets_decr_val) {
		ste_dec_counter(
				IPSEC_PACKET_COUNTER_ADDR(desc_addr),
				packets_decr_val,
				(STE_MODE_SATURATE | STE_MODE_64_BIT_CNTR_SIZE));
	}	
	
	return IPSEC_SUCCESS;	
} /* End of ipsec_decr_lifetime_counters */

/**************************************************************************//**
	ipsec_get_seq_num
*//****************************************************************************/
int ipsec_get_seq_num(
		ipsec_handle_t ipsec_handle,
		uint32_t *sequence_number,
		uint32_t *extended_sequence_number,
		uint32_t anti_replay_bitmap[4])
{
	
	ipsec_handle_t desc_addr;
	uint32_t params_flags;
	uint8_t pdb_options;

	union {
		struct ipsec_encap_pdb encap_pdb;
		struct ipsec_decap_pdb decap_pdb;
	} pdb;
	
	desc_addr = IPSEC_DESC_ADDR(ipsec_handle);

	/* Read he descriptor flags to identify the direction */
	cdma_read(
			&params_flags, /* void *ws_dst */
			IPSEC_FLAGS_ADDR(desc_addr), /* uint64_t ext_address */
			sizeof(params_flags) /* uint16_t size */
	);
	
	/* Outbound (encapsulation) PDB format */
	if (params_flags & IPSEC_FLG_DIR_OUTBOUND) {
		/* 	Read the PDB from the descriptor with CDMA. */
		cdma_read(
			&pdb.encap_pdb, /* void *ws_dst */
			IPSEC_PDB_ADDR(desc_addr), /* uint64_t ext_address */
			sizeof(pdb.encap_pdb) /* uint16_t size */
		);
		
		/* Return swapped values (little to big endian conversion) */
		/* Always read from PDB, regardless of ESN enabled/disabled */
		*extended_sequence_number = LW_SWAP(0,&(pdb.encap_pdb.seq_num_ext_hi));
		*sequence_number = LW_SWAP(0,&(pdb.encap_pdb.seq_num));
		
		/* No anti-replay bitmap for encap, so just return zero */
		anti_replay_bitmap[0] = 0x0;
		anti_replay_bitmap[1] = 0x0;
		anti_replay_bitmap[2] = 0x0;
		anti_replay_bitmap[3] = 0x0;
	} else {
	/* Inbound (decapsulation) PDB format */
				
		/* 	Read the PDB from the descriptor with CDMA. */
		cdma_read(
			&(pdb.decap_pdb), /* void *ws_dst */
			IPSEC_PDB_ADDR(desc_addr), /* uint64_t ext_address */
			sizeof(pdb.decap_pdb) /* uint16_t size */
		);
	
		/* Return swapped values (little to big endian conversion) */
		/* Always read from PDB, regardless of ESN enabled/disabled */
		*extended_sequence_number = LW_SWAP(0,&(pdb.decap_pdb.seq_num_ext_hi));
		*sequence_number = LW_SWAP(0,&(pdb.decap_pdb.seq_num));
		
		/* PDB Word 0 is read from the little endian memory, 
		 * so the Options byte is at the least significant address */
		pdb_options = *((uint8_t *)(&pdb.decap_pdb));

		/* TODO: should the anti_replay_bitmap be swapped??? */
		
		switch (pdb_options & IPSEC_DECAP_PDB_ARS_MASK) {
			case IPSEC_DEC_OPTS_ARSNONE:
				anti_replay_bitmap[0] = 0x0;
				anti_replay_bitmap[1] = 0x0;
				anti_replay_bitmap[2] = 0x0;
				anti_replay_bitmap[3] = 0x0;
				break;
			case IPSEC_DEC_OPTS_ARS32:
				anti_replay_bitmap[0] = LW_SWAP(
						0,&(pdb.decap_pdb.anti_replay[0]));
				anti_replay_bitmap[1] = 0x0;
				anti_replay_bitmap[2] = 0x0;
				anti_replay_bitmap[3] = 0x0;
				break;
			case IPSEC_DEC_OPTS_ARS64:
				anti_replay_bitmap[0] = LW_SWAP(
						0,&(pdb.decap_pdb.anti_replay[0]));
				anti_replay_bitmap[1] = LW_SWAP(
						0,&(pdb.decap_pdb.anti_replay[1]));
				anti_replay_bitmap[2] = 0x0;
				anti_replay_bitmap[3] = 0x0;
				break;		
			case IPSEC_DEC_OPTS_ARS128:	
				anti_replay_bitmap[0] = LW_SWAP(
						0,&(pdb.decap_pdb.anti_replay[0]));
				anti_replay_bitmap[1] = LW_SWAP(
						0,&(pdb.decap_pdb.anti_replay[1]));
				anti_replay_bitmap[2] = LW_SWAP(
						0,&(pdb.decap_pdb.anti_replay[2]));
				anti_replay_bitmap[3] = LW_SWAP(
						0,&(pdb.decap_pdb.anti_replay[3]));
				break;
			default:
				anti_replay_bitmap[0] = 0x0;
				anti_replay_bitmap[1] = 0x0;
				anti_replay_bitmap[2] = 0x0;
				anti_replay_bitmap[3] = 0x0;	
		}
	}
	
	return IPSEC_SUCCESS;	

} /* End of ipsec_get_seq_num */

/**************************************************************************//**
	ipsec_get_ipv6_nh_offset
	
	The Destination header creates 2 different options for IPv6 extensions order 

	1.	IPv6 header – Destination – Routing – Fragment – Destination 
	The first destination header is for intermediate destinations, 
	and the second one is for the last destination.
	This option can occur only when Routing header is present and 
	the first destination placed before Routing. 
	The second Destination header is optional 
 
	2.	IPv6 header – Fragment - Destination 
	The destination header is for the last destination.
	Routing header is not present, or 
	Destination is placed after Routing header.
	
*//****************************************************************************/
__IPSEC_HOT_CODE uint8_t ipsec_get_ipv6_nh_offset(
		struct ipv6hdr *ipv6_hdr, 
		uint8_t *length)
{
	uint32_t current_hdr_ptr;
	uint16_t current_hdr_size;
	uint8_t current_ver;
	uint8_t next_hdr;
	uint8_t dst_ext;
	uint8_t nh_offset = 0; /* default value for no extensions */
	uint8_t header_after_dest;
	
	/* Destination extension can appear only once on fragment request */
	dst_ext = IPV6_EXT_DESTINATION;

	/* Copy initial IPv6 header */
	current_hdr_ptr = (uint32_t)ipv6_hdr;
	current_hdr_size = sizeof(struct ipv6hdr);
	next_hdr = ipv6_hdr->next_header;

	/* IP Header Length for SEC encapsulation, including IP header and
	 * extensions before ESP */
	*length = sizeof(struct ipv6hdr);

	/* From RFC 2460: 4.1  Extension Header Order
	 * IPv6 nodes must accept and attempt to process extension headers in
	 *  any order and occurring any number of times in the same packet,
	 *  except for the Hop-by-Hop Options header which is restricted to
	 *  appear immediately after an IPv6 header only. */
	
	/* Skip to next extension header until extension isn't ipv6 header
	 * or until extension is the fragment position (depend on flag) */
	while ((next_hdr == IPV6_EXT_HOP_BY_HOP) ||
		(next_hdr == IPV6_EXT_ROUTING) || (next_hdr == dst_ext) ||
		(next_hdr == IPV6_EXT_FRAGMENT)) {
		
		/* Increment hh_offset if this is the first extension */
		if (!nh_offset) {
			nh_offset += ((sizeof(struct ipv6hdr))>>3);
		}

		current_ver = next_hdr;
		current_hdr_ptr += current_hdr_size;
		next_hdr = *((uint8_t *)(current_hdr_ptr));
		current_hdr_size = *((uint8_t *)(current_hdr_ptr + 1));
		
		/* Calculate current extension size  */
		switch (current_ver) {

		case IPV6_EXT_DESTINATION:
		{
			/* If the next header is not an extension, this should be
			 * the starting point for ESP encapsulation  */
			if ((next_hdr != IPV6_EXT_ROUTING) && 
					(next_hdr != IPV6_EXT_FRAGMENT)) {
				/* Don't add to NH_OFFSET/length and Exit from the while loop */
				dst_ext = 0;
			} else {
				/* Next header is an Extension */
				nh_offset += (current_hdr_size + 1); /* in 8 bytes multiples */
				current_hdr_size = ((current_hdr_size + 1) << 3);
				*length += current_hdr_size;
			}
			break;
		}
		case IPV6_EXT_FRAGMENT:
		{
			/* Increment NH_OFFSET only if next header is extension */
			if (next_hdr == IPV6_EXT_ROUTING) {
				/* in 8 bytes multiples */
				nh_offset += IPV6_FRAGMENT_HEADER_LENGTH>>3; 
			} else if (next_hdr == IPV6_EXT_DESTINATION) {
				/* Get the header after the following destination */
				header_after_dest = 
					*((uint8_t *)(current_hdr_ptr + 
							IPV6_FRAGMENT_HEADER_LENGTH));
				/* Increment NH_OFFSET only if the following DEST header
				 * is not the last extension */
				if (header_after_dest == IPV6_EXT_ROUTING) {
					nh_offset += IPV6_FRAGMENT_HEADER_LENGTH>>3; 
				}	
			}

			current_hdr_size = IPV6_FRAGMENT_HEADER_LENGTH;
			*length += current_hdr_size;
			break;
		}
		/* Routing, Hop By Hop */
		default:
		{
			/* Increment NH_OFFSET only if next header is extension */
			if ((next_hdr == IPV6_EXT_ROUTING) ||
				(next_hdr == IPV6_EXT_FRAGMENT)) {
				/* in 8 bytes multiples */
				nh_offset += (current_hdr_size + 1); 
			} else if (next_hdr == IPV6_EXT_DESTINATION) {
				header_after_dest = 
					*((uint8_t *)(current_hdr_ptr + 
							((current_hdr_size + 1)<<3)));
				/* Increment NH_OFFSET only if this is not the last ext. header
				 * before Destination */
				if ((header_after_dest == IPV6_EXT_ROUTING) ||
					(header_after_dest == IPV6_EXT_FRAGMENT)) {
						nh_offset += (current_hdr_size + 1); 
				}
			}
			
			current_hdr_size = ((current_hdr_size + 1) << 3);
			*length += current_hdr_size;
			break;
		}
		}
	}

	/* Return NH_OFFSET as expected by the SEC */
	if (nh_offset) {
		/* NH_OFFSET in 8 bytes multiples for IP header + Extensions */
		return nh_offset;
	} else {
		return 0x1; /* NH_OFFSET in case of no Extensions */
	}
} /* End of ipsec_get_ipv6_nh_offset */

/**************************************************************************/

void ipsec_tman_callback(uint64_t desc_addr, uint16_t indicator)
{
	uint8_t expired_indicator;
	uint16_t tmr_duration;
	struct ipsec_sa_params_part2 sap2; /* Parameters to read from ext buffer */

	if(indicator == IPSEC_SOFT_SEC_LIFETIME_EXPIRED) {
		/* Soft seconds timer */

		/******************************************/
		/* Lock the soft seconds expire indicator */
		/******************************************/
		cdma_read_with_mutex(
				IPSEC_SOFT_SEC_EXPIRED_ADDR(desc_addr), /* ext_address */
				CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
				&expired_indicator, /* void *ws_dst */
				1); /* uint16_t size */
		
		/* 	Read relevant descriptor fields with CDMA. */
		cdma_read(
				&sap2, /* void *ws_dst */
				IPSEC_SA_PARAMS_2_ADDR(desc_addr),
				sizeof(sap2) /* uint16_t size */
				);

		/* If the lifetime is larger than the TMAN max,
		 * it is required to invoke the timer multiple times */
		if (sap2.soft_seconds_limit > IPSEC_MAX_TIMER_DURATION) {
			tmr_duration = IPSEC_MAX_TIMER_DURATION;
			sap2.soft_seconds_limit -= IPSEC_MAX_TIMER_DURATION;
		} else if (sap2.soft_seconds_limit > IPSEC_MIN_TIMER_DURATION) {
			tmr_duration = (uint16_t)sap2.soft_seconds_limit;
			sap2.soft_seconds_limit = 0;
		} else {
			tmr_duration = 0; /* lifetime fully expiered */
		}
		
		/* Check if a new timer needs to be invoked */
		if(tmr_duration) {
			
			/* Create soft seconds lifetime timer */
			tman_create_timer(
				sap2.tmi_id, /* uint8_t tmi_id */
				TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY |
					TMAN_CREATE_TIMER_ONE_SHOT, /* uint32_t flags */
					/* 1 Sec timer ticks, one shot*/
				tmr_duration, /* uint16_t duration; */
				desc_addr, /* tman_arg_8B_t opaque_data1 */
				IPSEC_SOFT_SEC_LIFETIME_EXPIRED, /* tman_arg_2B_t opaque_data2*/ 
				&ipsec_tman_callback,
				&sap2.soft_tmr_handle); /* uint32_t *timer_handle */
			
			/* Update the limit and timer handle in the params descriptor*/
			/* It is assumed the minimum timer duration is long enough so
			 * the new timer will not expire before the CDMA write completes */
			cdma_write(
					IPSEC_SOFT_SEC_LIMIT_ADDR(desc_addr), /* ext_address */
					&sap2.soft_seconds_limit, /* ws_src */
					8); /* size */
			
			/* Just release lock */
			cdma_mutex_lock_release(IPSEC_SOFT_SEC_EXPIRED_ADDR(desc_addr));
			
		} else {
			/* If the timer fully expired */
			/* Write expire indication in params and release lock */
			expired_indicator = 1;
			cdma_write_with_mutex(
					IPSEC_SOFT_SEC_EXPIRED_ADDR(desc_addr), /* ext_address */
					CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
					&expired_indicator, /* void *ws_src */
					1); /* uint16_t size */	
			
			/* Optionally call call the user callback */
			if (sap2.sec_callback_func != NULL) {
				sap2.sec_callback_func(sap2.sec_callback_arg, 
						IPSEC_SOFT_SEC_LIFETIME_EXPIRED);
			}
			
			/* TMan Confirmation */
			tman_timer_completion_confirmation(sap2.soft_tmr_handle);

		}
	} else {
		/* Hard seconds timer */

		/******************************************/
		/* Lock the hard seconds expire indicator */
		/******************************************/
		cdma_read_with_mutex(
				IPSEC_HARD_SEC_EXPIRED_ADDR(desc_addr), /* uint64_t ext_address */
				CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
				&expired_indicator, /* void *ws_dst */
				1); /* uint16_t size */
		
		/* 	Read relevant descriptor fields with CDMA. */
		cdma_read(
				&sap2, /* void *ws_dst */
				IPSEC_SA_PARAMS_2_ADDR(desc_addr),
				sizeof(sap2) /* uint16_t size */
				);
		
		/* If the lifetime is larger than the TMAN max,
		 * it is required to invoke the timer multiple times */
		if (sap2.hard_seconds_limit > IPSEC_MAX_TIMER_DURATION) {
			tmr_duration = IPSEC_MAX_TIMER_DURATION;
			sap2.hard_seconds_limit -= IPSEC_MAX_TIMER_DURATION;
		} else if (sap2.hard_seconds_limit > IPSEC_MIN_TIMER_DURATION) {
			tmr_duration = (uint16_t)sap2.hard_seconds_limit;
			sap2.hard_seconds_limit = 0;
		} else {
			tmr_duration = 0;
		}
		
		/* Check if a new timer needs to be invoked */
		if(tmr_duration) {
			/* Create hard seconds lifetime timer */
			tman_create_timer(
				sap2.tmi_id, /* uint8_t tmi_id */
				TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY | 
					TMAN_CREATE_TIMER_ONE_SHOT, /* uint32_t flags */
					/* 1 Sec timer ticks, one shot*/
				tmr_duration, /* uint16_t duration; */
				desc_addr, /* tman_arg_8B_t opaque_data1 */
				IPSEC_HARD_SEC_LIFETIME_EXPIRED, /* tman_arg_2B_t opaque_data2*/ 
				&ipsec_tman_callback,
				&sap2.soft_tmr_handle); /* uint32_t *timer_handle */
			
			/* Update the limit and timer handle in the params descriptor*/
			/* It is assumed the minimum timer duration is long enough so
			 * the new timer will not expire before the CDMA write completes */
			cdma_write(
					IPSEC_HARD_SEC_LIMIT_ADDR(desc_addr), /* ext_address */
					&sap2.hard_seconds_limit, /* ws_src */
					8); /* size */
			
			/* Just release lock */
			cdma_mutex_lock_release(IPSEC_HARD_SEC_EXPIRED_ADDR(desc_addr));
			
		} else {
			/* If the timer fully expired - */
			/* Write expire indication in params and release lock */
			expired_indicator = 1;
			cdma_write_with_mutex(
					IPSEC_HARD_SEC_EXPIRED_ADDR(desc_addr), /* uint64_t ext_address */
					CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
					&expired_indicator, /* void *ws_src */
					1); /* uint16_t size */	
			
			/* Optionally call call the user callback */
			if (sap2.sec_callback_func != NULL) {
				sap2.sec_callback_func(sap2.sec_callback_arg, 
					IPSEC_HARD_SEC_LIFETIME_EXPIRED);
			}
		}
		/* TMan Confirmation */
		tman_timer_completion_confirmation(sap2.hard_tmr_handle);
	}
} /* End of ipsec_tman_callback */

/**************************************************************************//**
@Function	ipsec_force_seconds_lifetime_expiry

*//****************************************************************************/
int ipsec_force_seconds_lifetime_expiry(
		ipsec_handle_t ipsec_handle)
{
	uint8_t expired_indicator; /* [0] : soft, [1] : hard */
	int return_val;
	ipsec_handle_t desc_addr;
	struct ipsec_sa_params_part2 sap2; /* timers parameters */
	uint8_t params_valid = 0;
	
	desc_addr = IPSEC_DESC_ADDR(ipsec_handle);

	/******************************************/
	/* Read the soft seconds expire indicator */
	/******************************************/
	cdma_read_with_mutex(
			IPSEC_SOFT_SEC_EXPIRED_ADDR(desc_addr), /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&expired_indicator, /* void *ws_dst */
			1); /* uint16_t size */
	
	/* Check soft seconds timer */ 
	if (!expired_indicator) {
		/* 	Read timers parameters from the descriptor */
		cdma_read(
				&sap2, /* void *ws_dst */
				IPSEC_SA_PARAMS_2_ADDR(desc_addr),
				sizeof(sap2) /* uint16_t size */
				);
		
		params_valid = 1; /* indicate that the params were already read */
		
		/* In Rev 1, for one-shot timers, tman_delete_timer()
		 * should be called after calling the tman_recharge_timer()
		 * as a workaround for errata ERR009310 */
		tman_recharge_timer(sap2.soft_tmr_handle);
		
		/* delete the timer and force NO expiration */
		return_val = tman_delete_timer(
				sap2.soft_tmr_handle, /* uint32_t timer_handle */
				TMAN_TIMER_DELETE_MODE_WO_EXPIRATION); /* uint32_t flags */
		
		if (!return_val) { /* timer not already expired */
			/* Indicate in the params */
			expired_indicator = 1;

			/* Write expire indication in params and release lock */
			cdma_write_with_mutex(
					IPSEC_SOFT_SEC_EXPIRED_ADDR(desc_addr), /* ext_address */
					CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
					&expired_indicator, /* void *ws_src */
					1); /* uint16_t size */	
		} else {
			/* Just release lock */
			cdma_mutex_lock_release(IPSEC_SOFT_SEC_EXPIRED_ADDR(desc_addr));
		}
		
		if (!return_val) { /* timer not already expired */
			/* Optionally call call the user callback */
			if (sap2.sec_callback_func != NULL) {
				sap2.sec_callback_func(sap2.sec_callback_arg, 
					IPSEC_SOFT_SEC_LIFETIME_EXPIRED);
			}
		}
		
	} else {
		/* If the timer already expired, there is no need to do anything */ 
		/* Release lock */
		cdma_mutex_lock_release(IPSEC_SOFT_SEC_EXPIRED_ADDR(desc_addr));
	}

	/******************************************/
	/* Read the hard seconds expire indicator */
	/******************************************/
	cdma_read_with_mutex(
			IPSEC_HARD_SEC_EXPIRED_ADDR(desc_addr), /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&expired_indicator, /* void *ws_dst */
			1); /* uint16_t size */
	
	/* Check hard seconds timer */ 
	if (!expired_indicator) {
		if (!params_valid) {
			/* 	Read timers parameters from the descriptor */
			cdma_read(
				&sap2, /* void *ws_dst */
				IPSEC_SA_PARAMS_2_ADDR(desc_addr),
				sizeof(sap2) /* uint16_t size */
			);
		}
		
		/* In Rev 1, for one-shot timers, tman_delete_timer()
		 * should be called after calling the tman_recharge_timer()
		 * as a workaround for errata ERR009310 */
		tman_recharge_timer(sap2.hard_tmr_handle);
		
		/* delete the timer and force NO expiration */
		return_val = tman_delete_timer(
				sap2.hard_tmr_handle, /* uint32_t timer_handle */
				TMAN_TIMER_DELETE_MODE_WO_EXPIRATION); /* uint32_t flags */
		
		if (!return_val) { /* timer not already expired */
			/* Write expire indication in params and release lock */
			expired_indicator = 1;
			cdma_write_with_mutex(
					IPSEC_HARD_SEC_EXPIRED_ADDR(desc_addr), /* uint64_t ext_address */
					CDMA_POSTDMA_MUTEX_RM_BIT, /* uint32_t flags */
					&expired_indicator, /* void *ws_src */
					1); /* uint16_t size */	
		} else {
			/* Just release lock */
			cdma_mutex_lock_release(IPSEC_HARD_SEC_EXPIRED_ADDR(desc_addr));
		}

		if (!return_val) { /* timer not already expired */
			/* Optionally call call the user callback */
			if (sap2.sec_callback_func != NULL) {
				sap2.sec_callback_func(sap2.sec_callback_arg, 
					IPSEC_HARD_SEC_LIFETIME_EXPIRED);
			}
		}
		
	} else {
		/* If the timer already expired, there is no need to do anything */ 
		/* Release lock */
		cdma_mutex_lock_release(IPSEC_HARD_SEC_EXPIRED_ADDR(desc_addr));
	}

	return IPSEC_SUCCESS;
	
} /* End of ipsec_force_seconds_lifetime_expiry */

/**************************************************************************//**
@Function	ipsec_error_handler

*//****************************************************************************/
void ipsec_error_handler(
		ipsec_handle_t ipsec_handle,
		enum ipsec_function_identifier func_id,  /* Function ID */
		enum ipsec_service_identifier service_id,  /* SR/Hardware ID */
		uint32_t line,
		int status) /* Error/Status value */
{
	uint32_t handle_high, handle_low;
	struct ipsec_debug_info info;
	ipsec_handle_t desc_addr;
	desc_addr = IPSEC_DESC_ADDR(ipsec_handle);

	cdma_read_with_mutex(
			IPSEC_DEBUG_INFO_ADDR(desc_addr), /* uint64_t ext_address */
			CDMA_PREDMA_MUTEX_WRITE_LOCK, /* uint32_t flags */
			&info, /* void *ws_dst */
			(uint16_t)sizeof(info) /* uint16_t size */	
	);

	/* Write only if current status is clear (no previous error) */
	if(!info.status) {
		info.func_id = func_id;
		info.service_id = service_id;
		info.line = line;
		info.status = status;
	
		/* Write the status to external memory */
		cdma_write(
			IPSEC_DEBUG_INFO_ADDR(desc_addr), /* ext_address */
			&info, /* ws_src */
			(uint16_t)(sizeof(info))); /* size */
		
	
		/* Release lock */
		cdma_mutex_lock_release(IPSEC_DEBUG_INFO_ADDR(desc_addr));
	}
#pragma push
#pragma stackinfo_ignore on
		
	handle_high =
			(uint32_t)((ipsec_handle & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ipsec_handle & 0x00000000ffffffff);
	
		/* Debug Print */
		pr_debug("IPsec debug info: Fn=%d, Sr=%d, Ln=%d, St=%d (0x%x)\n",
			info.func_id,
			info.service_id,
			info.line,
			info.status, info.status);
		
		pr_debug("\t[IPsec handle: 0x%x_%x]\n",handle_high, handle_low);

#pragma pop	
}


/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */
/** @} */ /* end of NETF */


