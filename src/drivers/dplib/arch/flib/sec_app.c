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

//#include "common/fsl_string.h"
//#include "common/fsl_stdio.h"
//#include "fsl_malloc.h"
//#include "kernel/platform.h"

//#include "fsl_dbg.h"
//#include "drivers/fsl_caam.h"

#define IPSEC_DONT_USE_SEC_APP

#ifndef IPSEC_DONT_USE_SEC_APP

#include "dplib/fsl_cdma.h"
/*#include "cdma.h"*/

#include "sec_app.h"


#include "ipsec_test_vector.h"

//enum rta_sec_era rta_sec_era = RTA_SEC_ERA_8;
enum descriptors descriptor;
struct ipsec_ref_vector_s rtv;

/* Initialize reference test vectors for IPsec */
int init_ref_test_vector_ipsec()
{
	int ret = 0;
	/* test only the first set of test vectors */
	const int proto_offset = 0;
	//struct ipsec_encap_pdb *e_pdb;
	
	struct my_ipsec_encap_pdb mypdb;
	
	struct my_ipsec_encap_pdb *e_pdb = &mypdb;
	
	rtv.auth_alginfo = OP_PCL_IPSEC_HMAC_MD5_96;
	rtv.auth_key = (uintptr_t)ipsec_test_auth_key[proto_offset];
	rtv.auth_keylen = (unsigned short)ipsec_test_auth_keylen[proto_offset];
	//rtv.dma_addr_auth_key = fsl_os_virt_to_phys(ipsec_test_auth_key[proto_offset]);

	/* IPsec 3DES-CBC & HMAC-MD5-96 */
	/* Hard coded according to the log file instead of virt_to_phys*/
	//[13] 04030020          key: class2-mdha-split len=32
	//[14] 00000000               ptr->@0x0e02003d0
	//[15] E02003D0
	//[16] 02000018          key: class1-keyreg len=24
	//[17] 00000000               ptr->@0x0e02003b8
	//[18] E02003B8
	
			
	rtv.cipher_alginfo = OP_PCL_IPSEC_3DES;
	rtv.key = (uintptr_t)ipsec_test_cipher_key[proto_offset];
	rtv.cipher_keylen = (unsigned short)ipsec_test_cipher_keylen[proto_offset];
	//rtv.dma_addr_key = fsl_os_virt_to_phys(ipsec_test_cipher_key[proto_offset]);

	// Put a fixed dummy value as Key pointer
	rtv.dma_addr_auth_key = 0x0e02003d0;
	rtv.dma_addr_key = 0x0e02003b8;
	
	rtv.block_size = DES_BLOCK_SIZE;
	rtv.iv_size = rtv.block_size;
	rtv.icv_size = IPSEC_ICV_MD5_TRUNC_SIZE;

	rtv.plaintext_length = ipsec_test_data_in_len[proto_offset];
	rtv.ciphertext_length = ipsec_test_data_out_len[proto_offset];
	rtv.plaintext = ipsec_test_data_in[proto_offset];
	rtv.ciphertext = ipsec_test_data_out[proto_offset];

	/* set IPSEC encapsulation PDB */
	//e_pdb = fsl_os_malloc(sizeof(struct ipsec_encap_pdb) +
	//	       ipsec_opt_ip_hdr_len[proto_offset]);
	//if (!e_pdb) {
	//	fsl_os_print("error: fsl_os_malloc for IPSEC encapsulation PDB failed\n");
	//	ret = -ENOMEM;
	//	goto err;
	//}
	e_pdb->options = PDBOPTS_ESP_IPHDRSRC | PDBOPTS_ESP_INCIPHDR |
					 PDBOPTS_ESP_TUNNEL;
	e_pdb->seq_num = ipsec_test_seq_num[proto_offset];
	memcpy(&e_pdb->cbc.iv[2], ipsec_test_iv[proto_offset], rtv.iv_size);
	e_pdb->spi = ipsec_test_spi[proto_offset];
	e_pdb->ip_hdr_len = (unsigned short)ipsec_opt_ip_hdr_len[proto_offset];
	memcpy(e_pdb->ip_hdr, ipsec_opt_ip_hdr[proto_offset],
	       e_pdb->ip_hdr_len);
	rtv.e_pdb = e_pdb;

err:
	//fsl_os_free(rtv.e_pdb);
	return ret;
}

//int sec_run_desc(void)
int sec_run_desc(uint64_t *descriptor_addr)
{
	struct alginfo cipher_info, auth_info;
	//uint8_t *out_data;
	uint32_t *desc, status = 0;
	//uint64_t data_in_addr, data_out_addr;
	//dma_addr_t ccsr_reg_bar;
	unsigned bufsize;
	int ret = 0;
	
	//ccsr_reg_bar = sys_get_memory_mapped_module_base(
	//			FSL_OS_MOD_SEC_GEN, 0, E_MAPPED_MEM_TYPE_GEN_REGS);
	//ASSERT_COND(ccsr_reg_bar);

	uint32_t desc_buffer[64] = {0}; // instead of malloc
	desc = desc_buffer;
	
	
	desc_buffer[0] = 0xDB01DB02; // debug
	            
	/* Select descriptor to run */
	descriptor = IPSEC_PROT_NEW_SHARED_DESC;

	//desc = fsl_os_malloc(CAAM_CMD_SZ * 64);
	//if (!desc) {
	//	ret = -ENOMEM;
	//	goto err;
	//}

	ret = init_ref_test_vector_ipsec();
	if (ret)
		goto err;

	//out_data = fsl_os_malloc(rtv.ciphertext_length);
	//if(!out_data) {
	//	ret = -ENOMEM;
	//	goto err;
	//}

	cipher_info.algtype = rtv.cipher_alginfo;
	cipher_info.key = rtv.dma_addr_key;
	cipher_info.keylen = rtv.cipher_keylen;
	cipher_info.key_enc_flags = 0;

	auth_info.algtype = rtv.auth_alginfo;
	auth_info.key = rtv.dma_addr_auth_key;
	auth_info.keylen = rtv.auth_keylen;
	auth_info.key_enc_flags = 0;

	switch (descriptor) {
	case IPSEC_PROT_NEW_SHARED_DESC:
		/*
		 * Don't send the descriptor to SEC.
		 * Call this descriptor creation function
		 * only for printing purposes.
		 */
		//cnstr_shdsc_ipsec_new_encap(desc, &bufsize, 1, rtv.e_pdb,
		//							&cipher_info, &auth_info);
		cnstr_shdsc_ipsec_new_encap(desc, &bufsize, 1, (struct ipsec_encap_pdb *)rtv.e_pdb,
									&cipher_info, &auth_info);

		break;
	//case IPSEC_PROT_NEW_JOB_DESC:
	//	data_in_addr = fsl_os_virt_to_phys(rtv.plaintext);
	//	data_out_addr = fsl_os_virt_to_phys(out_data);

	//	cnstr_jd_ipsec_new_encap(desc, &bufsize, 1, rtv.e_pdb,
	//		       &cipher_info, &auth_info,
	//		       data_in_addr, rtv.plaintext_length,
	//		       data_out_addr, rtv.ciphertext_length);

		//run_descriptor_jr(ccsr_reg_bar, desc, &status);
	//	break;
	}

	desc_buffer[63] = 0xDB03DB04; // debug

	// Allocate a buffer with CDMA.
	ret = (int32_t)cdma_acquire_context_memory(
		1, // BPID 
		descriptor_addr); /* context_memory */ 
	
	// Write descriptor to external memory
	ret = cdma_write(
			*descriptor_addr, /* ext_address */
			desc, /* ws_src */
			256); /* uint16_t size */
	
	//fsl_os_print(">>>>>> Dump Descriptor (Little Endian):\n");
	//for (i = 0; i < bufsize; i++)
	//	fsl_os_print("%08x\n", desc[i]);

	//convert_desc_to_le(desc, desc);
	//fsl_os_print(">>>>>> Dump Descriptor (Big Endian):\n");
	//for (i = 0; i < bufsize; i++)
	//	fsl_os_print("%08x\n", desc[i]);

	//fsl_os_print(">>>>>>Print SEC output data:\n");
	//for (i = 0; i < rtv.ciphertext_length; i++)
	//	fsl_os_print("%02x", out_data[i]);
	//fsl_os_print("\n");
	//fsl_os_print("Exiting SEC application\n");

err:
	//fsl_os_free(desc);
	//fsl_os_free(out_data);
	return ret;
}

void sec_run_desc_free(void)
{
    /* TODO - complete!*/
}

#endif /* IPSEC_DONT_USE_SEC_APP */
