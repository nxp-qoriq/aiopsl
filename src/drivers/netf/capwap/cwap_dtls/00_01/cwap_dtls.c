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
@File		cwap_dtls.c

@Description	This file contains the AIOP CAPWAP DTLS implementation.

*//***************************************************************************/

#include "fsl_types.h"
#include "fsl_cdma.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_osm.h"
#include "osm_inline.h"
#include "fsl_checksum.h"
#include "fsl_net.h"
#include "system.h"
#ifdef AIOP_VERIF
#include "slab_stub.h"
#else
#include "fsl_sl_slab.h"
#endif /* AIOP_VERIF */
#include "sec.h"
#include "rta.h"
#include "desc/tls.h"
#include "cwap_dtls.h"

extern __PROFILE_SRAM
struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];

int cwap_dtls_early_init(uint32_t total_instance_num,
			 uint32_t total_committed_sa_num,
			 uint32_t total_max_sa_num)
{
	int mem_id;
	uint32_t committed_buffs, max_buffs;

	committed_buffs = total_instance_num + total_committed_sa_num;
	max_buffs = total_instance_num + total_max_sa_num;
	mem_id = fsl_mem_exists(CWAP_DTLS_PRIMARY_MEM_PARTITION_ID) ?
			CWAP_DTLS_PRIMARY_MEM_PARTITION_ID :
			CWAP_DTLS_SECONDARY_MEM_PARTITION_ID;

	return slab_register_context_buffer_requirements(committed_buffs,
			max_buffs, CWAP_DTLS_SA_DESC_BUF_SIZE,
			CWAP_DTLS_SA_DESC_BUF_ALIGN,
			(enum memory_partition_id)mem_id, 0, 0);
}

int cwap_dtls_create_instance(uint32_t committed_sa_num, uint32_t max_sa_num,
			      cwap_dtls_instance_handle_t *instance_handle)
{
	struct cwap_dtls_instance_params instance;
	int err, mem_id;

	mem_id = fsl_mem_exists(CWAP_DTLS_PRIMARY_MEM_PARTITION_ID) ?
			CWAP_DTLS_PRIMARY_MEM_PARTITION_ID :
			CWAP_DTLS_SECONDARY_MEM_PARTITION_ID;
	instance.sa_count = 0;
	instance.committed_sa_num = committed_sa_num;
	instance.max_sa_num = max_sa_num;

	/* Descriptor and Instance Buffers */
	err = slab_find_and_reserve_bpid(committed_sa_num + 1,
		CWAP_DTLS_SA_DESC_BUF_SIZE, CWAP_DTLS_SA_DESC_BUF_ALIGN,
		(enum memory_partition_id)mem_id, NULL, &instance.desc_bpid);
	if (err)
		return -ENOMEM;

	/* Allocate a buffer for the instance */
	err = cdma_acquire_context_memory(instance.desc_bpid, instance_handle);
	if (err) {
		/* Upon an error, un-reserve the buffers */
		slab_find_and_unreserve_bpid((int32_t)(committed_sa_num + 1),
					     instance.desc_bpid);
		/* not checking for an error since the BPID must be valid */
		return -ENOSPC;
	}

	cdma_write(*instance_handle, &instance,	sizeof(instance));

	return SUCCESS;
}

int cwap_dtls_delete_instance(cwap_dtls_instance_handle_t instance_handle)
{
	struct cwap_dtls_instance_params instance;
	int err;

	cdma_read(&instance, instance_handle, sizeof(instance));

	/* Check if all SAs were deleted */
	if (instance.sa_count)
		return -EPERM;

	/* Release the instance buffer */
	cdma_release_context_memory(instance_handle);

	/* Un-reserve "committed + 1" buffers back to the slab */
	err = slab_find_and_unreserve_bpid(
		(int32_t)(instance.committed_sa_num + 1), instance.desc_bpid);

	return err ? -ENAVAIL : SUCCESS;
}

int cwap_dtls_get_buffer(cwap_dtls_instance_handle_t instance_handle,
			 cwap_dtls_sa_handle_t *sa_handle)
{
	struct cwap_dtls_instance_params instance;
	int err, mem_id;

	mem_id = fsl_mem_exists(CWAP_DTLS_PRIMARY_MEM_PARTITION_ID) ?
			CWAP_DTLS_PRIMARY_MEM_PARTITION_ID :
			CWAP_DTLS_SECONDARY_MEM_PARTITION_ID;
	cdma_read_with_mutex(instance_handle, CDMA_PREDMA_MUTEX_WRITE_LOCK,
			     &instance, sizeof(instance));

	if (instance.sa_count < instance.committed_sa_num) {
		instance.sa_count++;
		/* Write and release lock */
		cdma_write_with_mutex(instance_handle,
				      CDMA_POSTDMA_MUTEX_RM_BIT,
				      &instance.sa_count,
				      sizeof(instance.sa_count));
		err = cdma_acquire_context_memory(instance.desc_bpid,
						  sa_handle);
		if (err)
			goto get_buffer_alloc_err;
	} else if (instance.sa_count < instance.max_sa_num) {
		instance.sa_count++;
		/* Write and release lock */
		cdma_write_with_mutex(instance_handle,
				      CDMA_POSTDMA_MUTEX_RM_BIT,
				      &instance.sa_count,
				      sizeof(instance.sa_count));
		/* Descriptor Buffer */
		err = slab_find_and_reserve_bpid(1, CWAP_DTLS_SA_DESC_BUF_SIZE,
			CWAP_DTLS_SA_DESC_BUF_ALIGN,
			(enum memory_partition_id)mem_id, NULL,
			&instance.desc_bpid);
		/* Check if Slab has no buffers */
		if (err)
			goto get_buffer_alloc_err;

		err = cdma_acquire_context_memory(instance.desc_bpid,
						  sa_handle);
		if (err)
			goto get_buffer_alloc_err;
	} else {
		cdma_mutex_lock_release(instance_handle);
		return -EPERM;
	}

	return SUCCESS;

get_buffer_alloc_err:
	cdma_read_with_mutex(instance_handle, CDMA_PREDMA_MUTEX_WRITE_LOCK,
			     &instance.sa_count, sizeof(instance.sa_count));
	instance.sa_count--;
	cdma_write_with_mutex(instance_handle, CDMA_POSTDMA_MUTEX_RM_BIT,
			      &instance.sa_count, sizeof(instance.sa_count));

	return -ENOSPC;
}

int cwap_dtls_release_buffer(cwap_dtls_instance_handle_t instance_handle,
			     cwap_dtls_sa_handle_t sa_handle)
{
	struct cwap_dtls_instance_params instance;

	cdma_read_with_mutex(instance_handle, CDMA_PREDMA_MUTEX_WRITE_LOCK,
			     &instance, sizeof(instance));

	if (instance.sa_count == 0) {
		/* Not permitted to delete SA from empty instance */
		cdma_mutex_lock_release(instance_handle);
		return -EPERM;
	}

	/* Release the buffer */
	cdma_release_context_memory(sa_handle);

	instance.sa_count--;

	/* Write (just the counter) and release lock */
	cdma_write_with_mutex(instance_handle, CDMA_POSTDMA_MUTEX_RM_BIT,
			      &instance.sa_count, sizeof(instance.sa_count));

	/*
	 * If buffer taken from 'max' quanta, need to return to slab.
	 * The ">=" is because sa_count was already decremented above.
	 */
	if (instance.sa_count >= instance.committed_sa_num)
		/* Un-reserve one buffer back to the slab */
		if (slab_find_and_unreserve_bpid(1, instance.desc_bpid))
			/* bman pool not found */
			return -ENAVAIL;

	return SUCCESS;
}

/*
 * The inline_max_size inline_max_total_size pragmas are here to eliminate
 * non-inlined build and warnings of the RTA.
 */
#pragma push
#pragma inline_max_size(10000)
#pragma inline_max_total_size(50000)

int cwap_dtls_generate_encap_sd(struct cwap_dtls_sa_descriptor_params *params,
				uint64_t sd_addr, int *sd_size)
{
	/* Temporary Workspace SEC shared descriptor */
	uint32_t ws_shared_desc[CWAP_DTLS_SD_MAX_LEN_WORDS] = {0};
	int pdb_len, err;

	/*
	 * A split authentication key is required for HMAC-based cipher suites.
	 * AEAD cipher suites (GCM, CCM) don't need this.
	 */
	if (params->authdata.keylen)
		ws_shared_desc[0] = split_key_len(params->authdata.algtype);

	ws_shared_desc[1] = params->cipherdata.keylen;

	/*
	 * Note: PDB length might actually be smaller, depending on options
	 * (TrICV) and encryption algorithm (IV size).
	 * There is no ARS for encapsulation.
	 */
	if (params->flags & CWAP_DTLS_FLG_CIPHER_GCM)
		pdb_len = sizeof(params->pdb.gcm) -
			  sizeof(params->pdb.gcm.anti_replay);
	else
		pdb_len = sizeof(params->pdb.cbc) -
			  sizeof(params->pdb.cbc.anti_replay);
	err = rta_inline_query(CWAP_DTLS_ENC_BASE_SD_LEN + pdb_len,
			       CWAP_DTLS_JD_MAX_LEN, (unsigned *)ws_shared_desc,
			       &ws_shared_desc[2], 2);
	if (err < 0)
		return err;

	params->authdata.key_type = ws_shared_desc[2] & 1 ? RTA_DATA_IMM_DMA :
							    RTA_DATA_PTR;
	params->cipherdata.key_type = ws_shared_desc[2] & 2 ? RTA_DATA_IMM_DMA :
							      RTA_DATA_PTR;

	*sd_size = cnstr_shdsc_cwap_dtls((uint32_t *)ws_shared_desc,
			SEC_POINTER_SIZE, TRUE,	(uint8_t *)&params->pdb,
			&params->protcmd, &params->cipherdata,
			&params->authdata);

	/* Write the descriptor to external memory */
	cdma_write(sd_addr, ws_shared_desc, (uint16_t)((*sd_size) << 2));

	return SUCCESS;
}
#pragma pop

/*
 * The inline_max_size inline_max_total_size pragmas are here to eliminate
 * non-inlined build and warnings of the RTA
 */
#pragma push
#pragma inline_max_size(10000)
#pragma inline_max_total_size(50000)

int cwap_dtls_generate_decap_sd(struct cwap_dtls_sa_descriptor_params *params,
				uint64_t sd_addr, int *sd_size)
{
	/* Temporary Workspace SEC shared descriptor */
	uint32_t ws_shared_desc[CWAP_DTLS_SD_MAX_LEN_WORDS] = {0};
	int pdb_len, err;

	/*
	 * A split authentication key is required for HMAC-based cipher suites.
	 * AEAD cipher suites (GCM, CCM) don't need this.
	 */
	if (params->authdata.keylen)
		ws_shared_desc[0] = split_key_len(params->authdata.algtype);

	ws_shared_desc[1] = params->cipherdata.keylen;

	/*
	 * Note: PDB length might actually be smaller, depending on PDB options
	 * (ARS, TrICV) and encryption algorithm (IV size).
	 */
	if (params->flags & CWAP_DTLS_FLG_CIPHER_GCM)
		pdb_len = sizeof(params->pdb.gcm) -
			  sizeof(params->pdb.gcm.anti_replay);
	else
		pdb_len = sizeof(params->pdb.cbc) -
			  sizeof(params->pdb.cbc.anti_replay);
	err = rta_inline_query(CWAP_DTLS_DEC_BASE_SD_LEN + pdb_len,
			       CWAP_DTLS_JD_MAX_LEN, (unsigned *)ws_shared_desc,
			       &ws_shared_desc[2], 2);
	if (err < 0)
		return err;

	params->authdata.key_type = ws_shared_desc[2] & 1 ? RTA_DATA_IMM_DMA :
							    RTA_DATA_PTR;
	params->cipherdata.key_type = ws_shared_desc[2] & 2 ? RTA_DATA_IMM_DMA :
							      RTA_DATA_PTR;

	*sd_size = cnstr_shdsc_cwap_dtls((uint32_t *)ws_shared_desc,
			SEC_POINTER_SIZE, TRUE,	(uint8_t *)&params->pdb,
			&params->protcmd, &params->cipherdata,
			&params->authdata);

	/* Write the descriptor to external memory */
	cdma_write(sd_addr, ws_shared_desc, (uint16_t)((*sd_size) << 2));

	return SUCCESS;
}
#pragma pop

void cwap_dtls_generate_flc(struct cwap_dtls_sa_descriptor_params *params,
			    int sd_size, uint64_t flc_address)
{
	struct sec_flow_context flow_context;
	struct storage_profile *sp_addr = &storage_profile[0];
	uint8_t *sp_byte;
	uint32_t sp_controls;
	int i;

	/* Clear the Flow Context area */
	cdma_ws_memory_init(&flow_context, sizeof(flow_context), 0);

	sp_addr += params->spid;
	sp_byte = (uint8_t *)sp_addr;

	/*
	 * Word 1
	/* 5-0 SDL = Shared Descriptor length, 7-6 reserved
	 * SDL is encoded in terms of 32-bit descriptor command words
	 */
	flow_context.word1_sdl = (uint8_t)(sd_size & 0x000000FF);

#if 0
	/* 11-8 CRID, 14-12 reserved, 15 CRJD */
	flow_context.word1_bits_15_8 = 0;

	/* 16	EWS,17 DAC,18-20?, 23-21 reserved */
	flow_context.word1_bits23_16 = 0;

	/*
	 * 24 RSC (not used for AIOP), 25 RBMT (not used for AIOP),
	 * 31-26 reserved
	 */
	flow_context.word1_bits31_24 = 0;
#endif
	/* TODO: check regarding EWS in buffer reuse mode */

	/* Debug */
#if 0
	/* 16 EWS; 17 DAC; 18,19,20 ?; 23-21 reserved */
	flow_context.word1_bits23_16 = 0x01;

	/* word 2  RFLC[31-0]; Not used for AIOP */
	flow_context.word2_rflc_31_0 = 0;

	/* word 3  RFLC[63-32]; Not used for AIOP */
	flow_context.word3_rflc_63_32 = 0;

	/* word 4 Not used, should be NULL */
	/* 15-0  IICID */
	flow_context.word4_iicid = 0;
	/* 31-16 OICID */
	flow_context.word4_oicid = 0;

	/* word 5 */
	/* 23-0 OFQID, not used for AIOP */
	flow_context.word5_7_0 = 0;
	flow_context.word5_15_8 = 0;
	flow_context.word5_23_16 = 0;
	/*
	 * 31-30 ICR = 2. AIOP is a trusted user - no need for any restrictions.
	 * 24 OSC : not used for AIOP
	 * 25 OBMT : not used for AIOP
	 * 29-26 reserved
	 * 31-30 ICR
	 */
	flow_context.word5_31_24 = 0x40;

	/* word 6 Not used for AIOP */
	flow_context.word6_oflc_31_0 = 0;

	/* word 7 Not used for AIOP */
	flow_context.word7_oflc_63_32 = 0;
#endif

	/*
	 * Storage profile format:
	 * 0x00 IP-Specific Storage Profile Information
	 * 0x08 Frame Format and Data Placement Controls
	 * 0x10 Buffer Pool 2, Buffer Pool 1 Attributes and Controls
	 * 0x18 Buffer Pool 4, Buffer Pool 3 Attributes and Controls
	 *
	 * Only The data from offset 0x08 and 0x10 is copied to SEC flow context
	 */
	/* Copy the standard Storage Profile to Flow Context words 8-15 */
	/* No need to for the first 8 bytes, so start from 8 */
	/* TODO: optionally use for copy */
	/* fdma_copy_data(24, 0 ,sp_byte,flow_context.storage_profile + 8); */
	for (i = 8; i < 32; i++)
		*((uint8_t *)((uint8_t *)flow_context.storage_profile + i -
			      8)) = *(sp_byte + i);

	if (params->flags & CWAP_DTLS_FLG_BUFFER_REUSE) {
		/*
		 * In reuse buffer mode (BS=1) the DHR field is treated
		 * as a signed value of a data headroom correction and defines
		 * by how many bytes an existing offset should be adjusted to
		 * make room for additional output data or any need to move the
		 * output ‘forward’
		 * The SEC expects that |DHR| >= frame growth
		 * DL is not considered in reuse mode.
		 */

		/* SP word at offset 0x4 */
		/* 31|30|29-28|27-25|24 |23  |22-21|20  |19-16|15-12|11-0| */
		/* BS|R | FF  |  R  |DLC|PTAR|  R  |SGHR|ASAR |  R  |DHR | */

		/*
		 * For reuse mode:
		 * BS = 1
		 * FF = 10 - Reuse input buffers if they provide sufficient
		 *           space
		 * DLC = 0
		 * PTAR = 0 (ignored)
		 * SGHR = 0 (ignored)
		 * ASAR - preserve existing size
		 * DHR = negative correction value in 2's complement format
		 */

		/* Read-swap the storage profile word at offset 4 */
		sp_controls = LW_SWAP(4,
				      (uint32_t *)flow_context.storage_profile);

		/* Clear all bits but ASAR; set BS = 0b1, FF = 0b10 */
		if (CWAP_DTLS_IS_OUTBOUND_DIR(params->protcmd.optype))
			/*
			 * Set BS=1, FF = 01, ASAR and DHR (negative, 2's
			 * complement)
			 */
			sp_controls = (sp_controls & CWAP_DTLS_SP_ASAR_MASK) |
				      CWAP_DTLS_SP_REUSE_BS_FF |
				      (CWAP_DTLS_SP_DHR_MASK &
				       (-CWAP_DTLS_MAX_FRAME_GROWTH));
		else
			sp_controls = (sp_controls & CWAP_DTLS_SP_ASAR_MASK) |
				      CWAP_DTLS_SP_REUSE_BS_FF;

		/*
		 * Store the new storage profile word with swapping to little
		 * endian
		 */
		STW_SWAP(sp_controls, 4,
			 (uint32_t *)flow_context.storage_profile);
	} else if (CWAP_DTLS_IS_OUTBOUND_DIR(params->protcmd.optype)) {
		/* New output buffer mode; set the DL (tailroom growth) */
		sp_controls = CWAP_DTLS_MAX_FRAME_GROWTH;
		STW_SWAP(sp_controls, 0,
			 (uint32_t *)flow_context.storage_profile);
	}

#if 0
	{
		uint32_t j, val;

		fsl_print("CWAP DTLS: Flow Context Storage Profile\n");
		for (j = 0; j < 8; j++) {
			val = *(uint32_t *)((uint32_t)flow_context.storage_profile + j * 4);
			fsl_print("Word %d = 0x%x\n", j, val);
		}
	}
#endif

	cdma_write(flc_address, &flow_context, SEC_FLOW_CONTEXT_SIZE);
}

void cwap_dtls_create_key_copy(uint64_t src_key_addr, uint64_t dst_key_addr,
			       uint16_t src_key_len)
{
	uint8_t key[CWAP_DTLS_KEY_MAX_SIZE];

	/* Read the key from the application buffer */
	cdma_read(key, src_key_addr, src_key_len);

	/* Store the key to the copy location */
	cdma_write(dst_key_addr, key, src_key_len);
}

void cwap_dtls_generate_sa_params(cwap_dtls_instance_handle_t instance_handle,
				  struct cwap_dtls_sa_descriptor_params *params,
				  cwap_dtls_sa_handle_t sa_handle)
{
	struct cwap_dtls_sa_params sap;

	sap.instance_handle = instance_handle;
	sap.flags = params->flags;

	if (CWAP_DTLS_IS_OUTBOUND_DIR(params->protcmd.optype))
		sap.flags |= CWAP_DTLS_FLG_DIR_OUTBOUND;

	if (sap.flags & CWAP_DTLS_FLG_BUFFER_REUSE) {
		sap.sec_buffer_mode = CWAP_DTLS_SEC_REUSE_BUFFER_MODE;
	} else {
		sap.sec_buffer_mode = CWAP_DTLS_SEC_NEW_BUFFER_MODE;

#ifndef TKT265088_WA_DISABLE
		/*
		 * TKT265088:
		 * CAAM/SEC: The FD[BPID] is not updated after an AIOP operation
		 */
		struct storage_profile *sp_addr = &storage_profile[0];

		sp_addr += params->spid;

		/*
		 * 14-bit BPID is at offset 0x12 (18) of the storage profile
		 * Read-swap and mask the 2 MSbs
		 */
		sap.bpid = (LH_SWAP(0, (uint16_t *)((uint8_t *)sp_addr + 0x12)))
			   & 0x3FFF;
#endif
	}

	sap.output_spid = (uint8_t)(params->spid);

	cdma_write(CWAP_DTLS_SA_DESC_ADDR(sa_handle), &sap, sizeof(sap));
}

void cwap_dtls_init_debug_info(cwap_dtls_sa_handle_t desc_addr)
{
	uint8_t debug_info[CWAP_DTLS_DEBUG_INFO_SIZE] = {0};

	/* Clear debug area */
	cdma_write(CWAP_DTLS_DEBUG_INFO_ADDR(desc_addr), &debug_info,
		   sizeof(debug_info));
}

int cwap_dtls_add_sa_descriptor(struct cwap_dtls_sa_descriptor_params *params,
				cwap_dtls_instance_handle_t instance_handle,
				cwap_dtls_sa_handle_t *sa_handle)
{
	cwap_dtls_sa_handle_t desc_addr;
	int err, sd_size;

	err = cwap_dtls_get_buffer(instance_handle, sa_handle);
	if (err)
		return err;

	desc_addr = CWAP_DTLS_SA_DESC_ADDR(*sa_handle);

	/*
	 * If the authentication key length is not 0, create a copy of the
	 * authentication key in the local buffer.
	 */
	if (params->authdata.keylen) {
		cwap_dtls_create_key_copy(params->authdata.key,
					  CWAP_DTLS_AUTH_KEY_ADDR(desc_addr),
					  (uint16_t)params->authdata.keylen);
		/* Now switch the original key address with the copy address */
		params->authdata.key = CWAP_DTLS_AUTH_KEY_ADDR(desc_addr);
	}

	/*
	 * If the cipher key length is not 0,
	 * create a copy of the cipher key in the local buffer.
	 */
	if (params->cipherdata.keylen) {
		cwap_dtls_create_key_copy(params->cipherdata.key,
					  CWAP_DTLS_CIPHER_KEY_ADDR(desc_addr),
					  (uint16_t)params->cipherdata.keylen);
		/* Now switch the original key address with the copy address */
		params->cipherdata.key = CWAP_DTLS_CIPHER_KEY_ADDR(desc_addr);
	}

	/*
	 * Build a SEC DTLS shared descriptor with the RTA library and
	 * then write it to memory with CDMA.
	 */
	if (rta_tls_cipher_mode(params->protcmd.protinfo) == RTA_TLS_CIPHER_GCM)
		params->flags |= CWAP_DTLS_FLG_CIPHER_GCM;
	if (CWAP_DTLS_IS_OUTBOUND_DIR(params->protcmd.optype))
		err = cwap_dtls_generate_encap_sd(params,
						  CWAP_DTLS_SD_ADDR(desc_addr),
						  &sd_size);
	else
		err = cwap_dtls_generate_decap_sd(params,
						  CWAP_DTLS_SD_ADDR(desc_addr),
						  &sd_size);
	if (err) {
		/*
		 * TODO: free the buffer, decrement SA counter?, fix error value
		 */

		/* Release the buffer. No check for error here */
		cwap_dtls_release_buffer(instance_handle, *sa_handle);

		return -ENAVAIL;
	}

	/*
	 * Generate the SEC Flow Context descriptor and write to memory with
	 * CDMA
	 */
	cwap_dtls_generate_flc(params, sd_size, CWAP_DTLS_FLC_ADDR(desc_addr));

	/* Prepare descriptor parameters and write them to memory with CDMA */
	cwap_dtls_generate_sa_params(instance_handle, params, desc_addr);

	/* Initialize the debug area */
	cwap_dtls_init_debug_info(desc_addr);

	return SUCCESS;
}

int cwap_dtls_del_sa_descriptor(cwap_dtls_sa_handle_t sa_handle)
{
	cwap_dtls_instance_handle_t instance_handle;
	cwap_dtls_sa_handle_t desc_addr;

	desc_addr = CWAP_DTLS_SA_DESC_ADDR(sa_handle);

	/* Read the instance handle from params area */
	cdma_read(&instance_handle, CWAP_DTLS_INSTANCE_HANDLE_ADDR(desc_addr),
		  sizeof(instance_handle));

	/* Release the buffer */
	return cwap_dtls_release_buffer(instance_handle, sa_handle);
}

CWAP_DTLS_CODE_PLACEMENT
int cwap_dtls_frame_encrypt(cwap_dtls_sa_handle_t sa_handle)
{
	struct cwap_dtls_sa_params sap;
	struct parse_result *pr;;
	struct presentation_context *prc;
	struct scope_status_params scope_status;
	cwap_dtls_sa_handle_t desc_addr;
	struct ipv4hdr *ipv4_hdr;
	struct udphdr *udp_hdr;
	uint64_t orig_flc;
	uint32_t orig_frc, sec_frc;
	uint32_t dpovrd = TLS_DPOVRD_USE;
	int err, enc_status = 0;
	uint8_t metadata_length;

	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	prc = (struct presentation_context *) HWC_PRC_ADDRESS;
	desc_addr = CWAP_DTLS_SA_DESC_ADDR(sa_handle);

	/* 2. Read relevant descriptor fields with CDMA. */
	cdma_read(&sap, desc_addr, sizeof(sap));

	/* 5. Save original FD[FLC], FD[FRC] (to stack) */
	orig_flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	orig_frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);

	/*
	 * Prepare DPOVRD parameters - metadata (L2/L3/L4 headers to be copied
	 * verbatim to output frame) length
	 */
#ifdef LS2085A_REV1
	metadata_length =
		(uint8_t)((uint8_t *)PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() -
			  (uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT());
#else
	metadata_length = (uint8_t)((uint8_t *)PARSER_GET_L5_OFFSET_DEFAULT() -
				    (uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT());
#endif
	dpovrd |= (metadata_length << DTLS_DPOVRD_METADATA_LEN_SHIFT) &
		  DTLS_DPOVRD_METADATA_LEN_MASK;

	/* Update FD[FRC] for DPOVRD */
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, dpovrd);

	/* 6. Update the FD[FLC] with the flow context buffer address. */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, CWAP_DTLS_FLC_ADDR(desc_addr));

	/*
	 * 7. FDMA store default frame command
	 * (for closing the frame, updating the other FD fields)
	 */
	err = fdma_store_default_frame_data();
	if (err) {
		cwap_dtls_error_handler(sa_handle, CWAP_DTLS_FRAME_ENCRYPT,
					CWAP_DTLS_FDMA_STORE_DEFAULT_FRAME_DATA,
					__LINE__, err);
		enc_status = CWAP_DTLS_INTERNAL_ERR;
		return enc_status;
	}

	/* 8. Prepare AAP parameters in the Workspace memory. */

	/* 8.1.	Use accelerator macros for storing parameters */

	/*
	 * 3 USE_FLC_SP Use Flow Context Storage Profile = 1
	 * The Storage Profile (SP) is embedded as part of the
	 * flow context pointed to by the Flow Context field in the Frame
	 * Descriptor (FD) for this acceleration operation.
	 *
	 * 8 OS_EX Ordering Scope Exclusive Phase.
	 * 0 Indicates that the accelerator call is not made during the
	 * exclusive phase of an Ordering Scope.
	 * 1 Indicates that the accelerator call is made during the
	 * exclusive phase of an Ordering Scope.
	 */

	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	/*
	 * If in Concurrent ordering scope, move to Exclusive (increment scope
	 * ID).
	 */
	if (scope_status.scope_mode == CWAP_DTLS_OSM_CONCURRENT) {
		/* Set OS_EX so AAP will do relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) =
			CWAP_DTLS_AAP_USE_FLC_SP | CWAP_DTLS_AAP_OS_EX;
		/* 9. Call the AAP */
		__e_ordhwacceli_(AAP_SEC_ACCEL_ID,
				 OSM_SCOPE_TRANSITION_TO_EXCL_OP,
				 OSM_SCOPE_ID_STAGE_INCREMENT_MASK);
	} else {
		/* Call AAP without relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = CWAP_DTLS_AAP_USE_FLC_SP;
		/* 9. Call the AAP */
		__e_hwacceli(AAP_SEC_ACCEL_ID);
	}

	/* 10. SEC Doing Encryption */

	if (sap.sec_buffer_mode == CWAP_DTLS_SEC_NEW_BUFFER_MODE) {
		/*
		 * In new output buffer mode, clear the PRC ASA Size,
		 * since the SEC does not preserve the ASA
		 * This is relevant for LS2085 Rev1 only. On Rev2 this macro is
		 * empty.
		 */
		PRC_SET_ASA_SIZE(0);

		/*
		 * Update the SPID of the new frame (SEC output) in the
		 * HW Context
		 */
		*((uint8_t *)HWC_SPID_ADDRESS) = sap.output_spid;

#ifndef TKT265088_WA_DISABLE
		LDPAA_FD_SET_BPID(HWC_FD_ADDRESS, sap.bpid);
#endif
	}

	/* 12. Read the SEC return status from the FD[FRC] */
	sec_frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
	if (sec_frc != 0) {
		if ((sec_frc & SEC_COMPRESSED_ERROR_MASK) ==
		    SEC_COMPRESSED_ERROR) {
			/* Compressed mode errors */
			if ((sec_frc & SEC_DECO_ERROR_MASK_COMPRESSED) ==
			    SEC_SEQ_NUM_OVERFLOW_COMPRESSED)
				enc_status |= CWAP_DTLS_SEQ_NUM_OVERFLOW;
			else
				enc_status |= CWAP_DTLS_GEN_ENCR_ERR;
		} else {
			/* Non-compressed mode errors */
			if ((sec_frc & SEC_DECO_ERROR_MASK) ==
			    SEC_SEQ_NUM_OVERFLOW)
				enc_status |= CWAP_DTLS_SEQ_NUM_OVERFLOW;
			else
				enc_status |= CWAP_DTLS_GEN_ENCR_ERR;
		}

		cwap_dtls_error_handler(sa_handle, CWAP_DTLS_FRAME_ENCRYPT,
					CWAP_DTLS_SEC_HW, __LINE__,
					(int)sec_frc);
		return enc_status;
	}

	/*
	 * 11. FDMA present default frame command (open frame)
	 * because earlier the segment was not presented, added
	 * PRC_RESET_NDS_BIT();
	 */
	PRC_RESET_NDS_BIT();

	/*
	 * There is no need to check for FDMA error here since if it is not
	 * fatal, it can return only SUCCESS or FDMA_STATUS_UNABLE_PRES_DATA_SEG
	 * which is not an error in this case.
	 */
	fdma_present_default_frame();

	/*
	 * Update IP total length, IP checksum, UDP length, UDP checksum
	 * Since L2/L3/L4 are left unchanged after SEC encap / decap, parser
	 * results are still valid.
	 */
	ipv4_hdr = (struct ipv4hdr *)PARSER_GET_OUTER_IP_POINTER_DEFAULT();
	ipv4_hdr->hdr_cksum = cksum_accumulative_update_uint32(
				ipv4_hdr->hdr_cksum, ipv4_hdr->total_length,
				LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	ipv4_hdr->total_length =
		(uint16_t)(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			   PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	udp_hdr = (struct udphdr *)PARSER_GET_L4_POINTER_DEFAULT();
	udp_hdr->length = (uint16_t)(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				     PARSER_GET_L4_OFFSET_DEFAULT());
	udp_hdr->checksum = 0;
	fdma_modify_default_segment_data(
		PARSER_GET_OUTER_IP_OFFSET_DEFAULT(),
		(uint16_t)(metadata_length - PARSER_GET_ETH_OFFSET_DEFAULT()));

	/*
	 * Set the gross running to 0 (invalidate)
	 * There will be no checksum calculation with PARSER_NO_FLAGS
	 */
	pr->gross_running_sum = 0;

	/* Run parser and check for errors. */
	err = parse_result_generate_default(PARSER_NO_FLAGS);
	if (err) {
		cwap_dtls_error_handler(sa_handle, CWAP_DTLS_FRAME_ENCRYPT,
					CWAP_DTLS_PARSE_RESULT_GENERATE_DEFAULT,
					__LINE__, err);
		enc_status = CWAP_DTLS_INTERNAL_ERR;
		return enc_status;
	}

	/*
	 * 17.	Restore the original FD[FLC], FD[FRC] (from stack).
	 * No need for additional FDMA command.
	 */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, orig_flc);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, orig_frc);

	return SUCCESS;
}

CWAP_DTLS_CODE_PLACEMENT
int cwap_dtls_frame_decrypt(cwap_dtls_sa_handle_t sa_handle)
{
	struct cwap_dtls_sa_params sap;
	struct parse_result *pr;
	struct presentation_context *prc;
	struct scope_status_params scope_status;
	cwap_dtls_sa_handle_t desc_addr;
	struct ipv4hdr *ipv4_hdr;
	struct udphdr *udp_hdr;
	int err, dec_status = 0;
	uint8_t metadata_length;
	uint64_t orig_flc;
	uint32_t orig_frc, sec_frc;
	uint32_t dpovrd = TLS_DPOVRD_USE;
	uint16_t orig_seg_addr;

	pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	prc = (struct presentation_context *) HWC_PRC_ADDRESS;
	desc_addr = CWAP_DTLS_SA_DESC_ADDR(sa_handle);

	/*  2. Read relevant descriptor fields with CDMA. */
	cdma_read(&sap, desc_addr, sizeof(sap));

	/* Save the original segment address */
	orig_seg_addr = PRC_GET_SEGMENT_ADDRESS();

	/* 5. Save original FD[FLC], FD[FRC] (to stack) */
	orig_flc = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
	orig_frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);

	/* 6. Update the FD[FLC] with the flow context buffer address */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, CWAP_DTLS_FLC_ADDR(desc_addr));

	/*
	 * Prepare DPOVRD parameters - metadata (headers to be copied verbatim
	 * to output frame) length
	 */
#ifdef LS2085A_REV1
	metadata_length =
		(uint8_t)((uint8_t *)PARSER_GET_NEXT_HEADER_OFFSET_DEFAULT() -
			  (uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT());
#else
	metadata_length = (uint8_t)((uint8_t *)PARSER_GET_L5_OFFSET_DEFAULT() -
				    (uint8_t *)PARSER_GET_ETH_OFFSET_DEFAULT());
#endif
	dpovrd |= (metadata_length << DTLS_DPOVRD_METADATA_LEN_SHIFT) &
		  DTLS_DPOVRD_METADATA_LEN_MASK;

	/* 7. Update the FD[FRC] with SEC DPOVRD parameters */
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, dpovrd);

	/*
	 * 8. FDMA store default frame command
	 * (for closing the frame, updating the other FD fields)
	 */
	err = fdma_store_default_frame_data();
	if (err) {
		cwap_dtls_error_handler(sa_handle, CWAP_DTLS_FRAME_DECRYPT,
					CWAP_DTLS_FDMA_STORE_DEFAULT_FRAME_DATA,
					__LINE__, err);
		return CWAP_DTLS_INTERNAL_ERR;
	}

	/* 9. Prepare AAP parameters in the Workspace memory */

	/* 3 USE_FLC_SP Use Flow Context Storage Profile = 1 */

	/*
	 * 8 OS_EX Ordering Scope Exclusive Phase.
	 * 0 Indicates that the accelerator call is not made during the
	 * exclusive phase of an Ordering Scope.
	 * 1 Indicates that the accelerator call is made during the
	 * exclusive phase of an Ordering Scope.
	 */

	/* Get OSM status (ordering scope mode and levels) */
	osm_get_scope(&scope_status);

	/*
	 * If in Concurrent ordering scope, move to Exclusive
	 * (increment scope ID).
	 */
	if (scope_status.scope_mode == CWAP_DTLS_OSM_CONCURRENT) {
		/* Set OS_EX so AAP will do relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = CWAP_DTLS_AAP_USE_FLC_SP |
						      CWAP_DTLS_AAP_OS_EX;
		/* 10. Call the AAP */
		__e_ordhwacceli_(AAP_SEC_ACCEL_ID,
				 OSM_SCOPE_TRANSITION_TO_EXCL_OP,
				 OSM_SCOPE_ID_STAGE_INCREMENT_MASK);
	} else {
		/* Call AAP without relinquish */
		*((uint32_t *)(HWC_ACC_IN_ADDRESS)) = CWAP_DTLS_AAP_USE_FLC_SP;
		/* 10. Call the AAP */
		__e_hwacceli(AAP_SEC_ACCEL_ID);
	}

	/* 11. SEC Doing Decryption */

	if (sap.sec_buffer_mode == CWAP_DTLS_SEC_NEW_BUFFER_MODE) {
		/*
		 * In new output buffer mode, clear the PRC ASA Size,
		 * since the SEC does not preserve the ASA
		 * This is relevant for LS2085 Rev1 only. On Rev2 this macro is
		 * empty.
		 */
		PRC_SET_ASA_SIZE(0);

		/*
		 * Update the SPID of the new frame (SEC output) in the
		 * HW Context
		 */
		*((uint8_t *)HWC_SPID_ADDRESS) = sap.output_spid;

#ifndef TKT265088_WA_DISABLE
		LDPAA_FD_SET_BPID(HWC_FD_ADDRESS, sap.bpid);
#endif
	}

	/* 13. Read the SEC return status from the FD[FRC] */
	sec_frc = LDPAA_FD_GET_FRC(HWC_FD_ADDRESS);
	if (sec_frc != 0) {
		if ((sec_frc & SEC_COMPRESSED_ERROR_MASK) ==
		    SEC_COMPRESSED_ERROR) {
			/* Compressed mode errors */
			if ((sec_frc & SEC_CCB_ERROR_MASK_COMPRESSED) ==
			    SEC_ICV_COMPARE_FAIL_COMPRESSED)
				dec_status |= CWAP_DTLS_ICV_CHK_FAIL;
			else
				switch (sec_frc &
					SEC_DECO_ERROR_MASK_COMPRESSED) {
				case SEC_SEQ_NUM_OVERFLOW_COMPRESSED:
					dec_status |=
						CWAP_DTLS_SEQ_NUM_OVERFLOW;
					break;

				case SEC_AR_LATE_PACKET_COMPRESSED:
					dec_status |= CWAP_DTLS_AR_LATE_PACKET;
					break;

				case SEC_AR_REPLAY_PACKET_COMPRESSED:
					dec_status |=
						CWAP_DTLS_AR_REPLAY_PACKET;
					break;

				default:
					dec_status |= CWAP_DTLS_GEN_DECR_ERR;
				}
		} else if ((sec_frc & SEC_CCB_ERROR_MASK) ==
			   SEC_ICV_COMPARE_FAIL) {
			dec_status |= CWAP_DTLS_ICV_CHK_FAIL;
		} else {
			switch (sec_frc & SEC_DECO_ERROR_MASK) {
			case SEC_SEQ_NUM_OVERFLOW:
				dec_status |= CWAP_DTLS_SEQ_NUM_OVERFLOW;
				break;

			case SEC_AR_LATE_PACKET:
				dec_status |= CWAP_DTLS_AR_LATE_PACKET;
				break;

			case SEC_AR_REPLAY_PACKET:
				dec_status |= CWAP_DTLS_AR_REPLAY_PACKET;
				break;

			default:
				dec_status |= CWAP_DTLS_GEN_DECR_ERR;
			}
		}

		cwap_dtls_error_handler(sa_handle, CWAP_DTLS_FRAME_DECRYPT,
					CWAP_DTLS_SEC_HW, __LINE__,
					(int)sec_frc);
		return dec_status;
	}

	/*
	 * Check DTLS padding
	 * When CWAP_DTLS_FLG_PAD_CHECK is enabled, outFMT=2b'10 which means
	 * that the SEC output frame includes also the DTLS trailer,
	 * where the last byte is the Pad Length (PL).
	 * The default padding value is (PL-1) for all padding bytes.
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |                   Metadata + DTLS header                      |
	 * ~          (including Optional IV, Optional IV Mask) +          ~
	 * |                    Payload + ICV                              |
	 * +               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 * |               |     Padding (0-255 bytes)                     |
	 * +-+-+-+-+-+-+-+-+                               +-+-+-+-+-+-+-+-+
	 * |                                               |  Pad Length   |
	 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	 */
	if (sap.flags & CWAP_DTLS_FLG_PAD_CHECK) {
		uint8_t *segment_pointer;
		uint16_t end_seg_len, orig_seg_length, orig_seg_offset;
		uint8_t pad_length, pad_byte;

		/* Save original presentation length and offset */
		orig_seg_length = PRC_GET_SEGMENT_LENGTH();
		orig_seg_offset = PRC_GET_SEGMENT_OFFSET();

		/*
		 * Calculate the required presentation size from the end of the
		 * frame, in case the frame is shorter than the presentation
		 * size
		 */
		if (LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) > orig_seg_length)
			end_seg_len = orig_seg_length;
		else
			end_seg_len =
				(uint16_t)LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

		/* Set length and offset to present at the end of the frame */
		PRC_SET_SEGMENT_LENGTH(end_seg_len);
		PRC_SET_SEGMENT_OFFSET(end_seg_len);

		/* Present from the end */
		PRC_SET_SR_BIT();

		err = fdma_present_default_frame();
		/*
		 * No error if the frame was just shorter than the segment size
		 */
		if (err && (err != FDMA_STATUS_UNABLE_PRES_DATA_SEG)) {
			cwap_dtls_error_handler(sa_handle,
				CWAP_DTLS_FRAME_DECRYPT,
				CWAP_DTLS_FDMA_PRESENT_DEFAULT_FRAME, __LINE__,
				err);
			return CWAP_DTLS_INTERNAL_ERR;
		}

		/* Get the pad length byte */
		segment_pointer = (uint8_t *)orig_seg_addr + end_seg_len - 1;
		pad_length = *segment_pointer;
		pad_byte = pad_length;

		/* Point to the last pad */
		segment_pointer--;

		/*
		 * Note: only supporting pad check within a single presentation
		 */
		while ((pad_length > 0) &&
		       (segment_pointer >= (uint8_t *)orig_seg_addr)) {
			if (*segment_pointer != pad_byte) {
				cwap_dtls_error_handler(sa_handle,
					CWAP_DTLS_FRAME_DECRYPT,
					CWAP_DTLS_INTERNAL_SERVICE, __LINE__,
					CWAP_DTLS_INT_PAD_CHECK_ERR);

				dec_status |= CWAP_DTLS_PAD_CHECK_ERR;
				return dec_status;
			}

			segment_pointer--;
			pad_length--;
		}

		/*
		 * If pad_length != 0, it means the presentation size was too
		 * small for the entire padding, and this is not supported.
		 */
		if (pad_length) {
			cwap_dtls_error_handler(sa_handle,
						CWAP_DTLS_FRAME_DECRYPT,
						CWAP_DTLS_INTERNAL_SERVICE,
						__LINE__,
						CAWP_DTLS_INT_ERR_PAD_TOO_LONG);
			dec_status |= CWAP_DTLS_INTERNAL_ERR;
			return dec_status;
		}

		/*
		 * Re-read the pad length, because it was subtracted in the
		 * while loop
		 */
		pad_length = *((uint8_t *)orig_seg_addr + end_seg_len - 1);

		/*
		 * Remove the DTLS trailer
		 * TODO: What about Optional IV, Optional IV Mask?
		 */
		err = fdma_replace_default_segment_data(
			orig_seg_offset,
			end_seg_len,
			(void *)orig_seg_addr,
			(end_seg_len - (uint16_t)pad_length - 1),
			(void *)prc->seg_address,
			(uint16_t)PRC_GET_SEGMENT_LENGTH(),
			(uint32_t)(FDMA_REPLACE_SA_CLOSE_BIT));
		if (err) {
			cwap_dtls_error_handler(sa_handle,
				CWAP_DTLS_FRAME_DECRYPT,
				CWAP_DTLS_FDMA_REPLACE_DEFAULT_SEGMENT_DATA,
				__LINE__, err);
			dec_status = CWAP_DTLS_INTERNAL_ERR;
			return dec_status;
		}

		/* Present from the beginning of the frame */
		PRC_RESET_SR_BIT();
		PRC_SET_SEGMENT_LENGTH(orig_seg_length);
		PRC_SET_SEGMENT_OFFSET(orig_seg_offset);

		/*
		 * There is no need to check for FDMA error here since if it is
		 * not fatal, it can return only SUCCESS or
		 * FDMA_STATUS_UNABLE_PRES_DATA_SEG which is not an error
		 * in this case.
		 */
		fdma_present_default_frame_segment(FDMA_PRES_NO_FLAGS,
						   (void *)orig_seg_addr,
						   orig_seg_offset,
						   orig_seg_length);
	} else {
		/* Present for no pad check case */
		err = fdma_present_default_frame();
		/*
		 * No error if the frame was just shorter than the
		 * segment size
		 */
		if (err && (err != FDMA_STATUS_UNABLE_PRES_DATA_SEG)) {
			cwap_dtls_error_handler(sa_handle,
				CWAP_DTLS_FRAME_DECRYPT,
				CWAP_DTLS_FDMA_PRESENT_DEFAULT_FRAME, __LINE__,
				err);
			dec_status = CWAP_DTLS_INTERNAL_ERR;
			return dec_status;
		}
	}

	/*
	 * Update IP total length, IP checksum, UDP length, UDP checksum
	 * Since L2/L3/L4 are left unchanged after SEC encap / decap, parser
	 * results are still valid.
	 * FD[LEN] does not include an the CAPWAP DTLS header (4B)
	 */
	ipv4_hdr = (struct ipv4hdr *)PARSER_GET_OUTER_IP_POINTER_DEFAULT();
	ipv4_hdr->hdr_cksum = cksum_accumulative_update_uint32(
				ipv4_hdr->hdr_cksum, ipv4_hdr->total_length,
				LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	ipv4_hdr->total_length =
		(uint16_t)(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
			   PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	udp_hdr = (struct udphdr *)PARSER_GET_L4_POINTER_DEFAULT();
	udp_hdr->length = (uint16_t)(LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS) -
				     PARSER_GET_L4_OFFSET_DEFAULT());
	udp_hdr->checksum = 0;
	fdma_modify_default_segment_data(
		PARSER_GET_OUTER_IP_OFFSET_DEFAULT(),
		(uint16_t)(metadata_length - PARSER_GET_ETH_OFFSET_DEFAULT()));

	/*
	 * 16. Update the gross running checksum in the Workspace parser results
	 */

	/*
	 * TODO: currently setting to 0 (invalid), so parser will call
	 * FDMA to recalculate the gross running sum
	 * Later need to manipulate the checksum returned from SEC
	 */
	pr->gross_running_sum = 0;

	/* 17. Run parser and check return status for frame validity */
	/* err = parse_result_generate_default(PARSER_VALIDATE_L3_L4_CHECKSUM); */
	err = parse_result_generate_default(PARSER_NO_FLAGS);
	if (err) {
		if (err == -EIO)
			/* Frame validity fail */
			dec_status = CWAP_DTLS_DECR_VALIDITY_ERR;
		else
			/* Other parser error */
			dec_status = CWAP_DTLS_INTERNAL_ERR;

		/* Get the detailed parser error code */
		cwap_dtls_error_handler(sa_handle, CWAP_DTLS_FRAME_DECRYPT,
			CWAP_DTLS_PARSE_RESULT_GENERATE_DEFAULT, __LINE__,
			PARSER_GET_PARSE_ERROR_CODE_DEFAULT());

		return dec_status;
	}

	/* 19. Restore the original FD[FLC], FD[FRC] (from stack) */
	LDPAA_FD_SET_FLC(HWC_FD_ADDRESS, orig_flc);
	LDPAA_FD_SET_FRC(HWC_FD_ADDRESS, orig_frc);

	return SUCCESS;
}

void cwap_dtls_get_ar_info_cbc(cwap_dtls_sa_handle_t desc_addr,
			       uint32_t params_flags,
			       uint64_t *sequence_number,
			       uint32_t anti_replay_bitmap[4])
{
	struct tls_block_pdb pdb;
	uint8_t i = 0;

	/* Read the PDB from the descriptor with CDMA */
	fsl_read_external_data(&pdb, CWAP_DTLS_PDB_ADDR(desc_addr), sizeof(pdb),
			       READ_DATA_USING_CDMA);

	if (params_flags & CWAP_DTLS_FLG_DIR_OUTBOUND) {
		*sequence_number = LDW_SWAP(0, &pdb.dtls_enc.word2);
	} else {
		uint8_t ars;

		*sequence_number = LDW_SWAP(0, &pdb.dtls_dec.word2);
		ars = rta_dtls_pdb_ars(LW_SWAP(0, &pdb.dtls_dec.word1));
		for (; i < ars; i++)
			anti_replay_bitmap[i] = LW_SWAP(0, &pdb.anti_replay[i]);
	}

	for (; i < 4; i++)
		anti_replay_bitmap[i] = 0;
}

void cwap_dtls_get_ar_info_gcm(cwap_dtls_sa_handle_t desc_addr,
			       uint32_t params_flags,
			       uint64_t *sequence_number,
			       uint32_t anti_replay_bitmap[4])
{
	struct tls_gcm_pdb pdb;
	uint8_t i = 0;

	/* Read the PDB from the descriptor with CDMA */
	fsl_read_external_data(&pdb, CWAP_DTLS_PDB_ADDR(desc_addr), sizeof(pdb),
			       READ_DATA_USING_CDMA);

	if (params_flags & CWAP_DTLS_FLG_DIR_OUTBOUND) {
		*sequence_number = LDW_SWAP(0, &pdb.dtls_enc.word2);
	} else {
		uint8_t ars;

		*sequence_number = LDW_SWAP(0, &pdb.dtls_dec.word2);
		ars = rta_dtls_pdb_ars(LW_SWAP(0, &pdb.dtls_dec.word1));
		for (; i < ars; i++)
			anti_replay_bitmap[i] = LW_SWAP(0, &pdb.anti_replay[i]);
	}

	for (; i < 4; i++)
		anti_replay_bitmap[i] = 0;
}

void cwap_dtls_get_ar_info(cwap_dtls_sa_handle_t sa_handle,
			   uint64_t *sequence_number,
			   uint32_t anti_replay_bitmap[4])
{
	cwap_dtls_sa_handle_t desc_addr = CWAP_DTLS_SA_DESC_ADDR(sa_handle);
	uint32_t params_flags;

	cdma_read(&params_flags, CWAP_DTLS_FLAGS_ADDR(desc_addr),
		  sizeof(params_flags));

	if (params_flags & CWAP_DTLS_FLG_CIPHER_GCM)
		cwap_dtls_get_ar_info_gcm(desc_addr, params_flags,
					  sequence_number, anti_replay_bitmap);
	else
		cwap_dtls_get_ar_info_cbc(desc_addr, params_flags,
					  sequence_number, anti_replay_bitmap);
}

void cwap_dtls_error_handler(cwap_dtls_sa_handle_t sa_handle,
			     enum cwap_dtls_function_identifier func_id,
			     enum cwap_dtls_service_identifier service_id,
			     uint32_t line, int status)
{
	uint32_t handle_high, handle_low;
	struct cwap_dtls_debug_info info;
	cwap_dtls_sa_handle_t desc_addr = CWAP_DTLS_SA_DESC_ADDR(sa_handle);

	cdma_read_with_mutex(CWAP_DTLS_DEBUG_INFO_ADDR(desc_addr),
			     CDMA_PREDMA_MUTEX_WRITE_LOCK, &info, sizeof(info));

	/* Write only if current status is clear (no previous error) */
	if (!info.status) {
		info.func_id = func_id;
		info.service_id = service_id;
		info.line = line;
		info.status = status;

		/* Write the status to external memory */
		cdma_write(CWAP_DTLS_DEBUG_INFO_ADDR(desc_addr), &info,
			   sizeof(info));

		/* Release lock */
		cdma_mutex_lock_release(CWAP_DTLS_DEBUG_INFO_ADDR(desc_addr));
	}

#pragma push
#pragma stackinfo_ignore on

	handle_high = (uint32_t)((sa_handle & 0xffffffff00000000) >> 32);
	handle_low = (uint32_t)(sa_handle & 0x00000000ffffffff);

	pr_debug("CAPWAP DTLS debug info: Fn=%d, Sr=%d, Ln=%d, St=%d (0x%x)\n",
		 info.func_id, info.service_id, info.line, info.status,
		 info.status);
	pr_debug("\t[CAPWAP DTLS handle: 0x%x_%x]\n", handle_high, handle_low);

#pragma pop
}
