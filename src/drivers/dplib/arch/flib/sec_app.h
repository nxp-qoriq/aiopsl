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

//#include "flib/desc.h"
//#include "flib/jobdesc.h"
//#include "flib/protoshared.h"
#include "desc.h"
#include "jobdesc.h"

//#define IPSEC_DONT_USE_SEC_APP

#ifndef IPSEC_DONT_USE_SEC_APP
//#include "protoshared.h"
#include "desc/ipsec.h"

#define DES_BLOCK_SIZE	8
/**< Integer number of bits in given bytes  */
#define NO_OF_BITS(bytelen)	(bytelen << 3)

int init_ref_test_vector_ipsec(void);
void create_shdesc(uint32_t *desc, unsigned *bufsize,
				   uint64_t data_out_addr);
//int sec_run_desc(void);
int sec_run_desc(uint64_t *descriptor_addr);


void sec_run_desc_free(void);

enum descriptors {
	IPSEC_PROT_NEW_JOB_DESC,
	IPSEC_PROT_NEW_SHARED_DESC
};


struct my_ipsec_encap_pdb {
	uint8_t hmo;
	union {
		uint8_t ip_nh;	/* next header for legacy mode */
		uint8_t rsvd;	/* reserved for new mode */
	};
	union {
		uint8_t ip_nh_offset;	/* next header offset for legacy mode */
		uint8_t aoipho;		/* actual outer IP header offset for
					 * new mode */
	};
	uint8_t options;
	uint32_t seq_num_ext_hi;
	uint32_t seq_num;
	union {
		struct ipsec_encap_cbc cbc;
		struct ipsec_encap_ctr ctr;
		struct ipsec_encap_ccm ccm;
		struct ipsec_encap_gcm gcm;
	};
	uint32_t spi;
	uint16_t rsvd2;
	uint16_t ip_hdr_len;
	uint32_t ip_hdr[5]; /* optional IP Header content */
};


/**
 * @details  IPSec new mode ESP encapsulation protocol-level JOB descriptor.
 *           Requires a MDHA split key.
 *
 * @param[in,out] descbuf    Pointer to buffer used for descriptor construction
 * @param[in,out] bufsize    Pointer to descriptor size to be written back upon
 *      completion
 * @param [in] ps            If 36/40bit addressing is desired, this parameter
 *      must be non-zero.
 * @param[in] pdb         Pointer to the PDB to be used with this descriptor.
 *      This structure will be copied inline to the descriptor under
 *      construction. No error checking will be made. Refer to the
 *      block guide for details about the encapsulation PDB.
 * @param[in] cipherdata  Pointer to block cipher transform definitions. Valid
 *      algorithm values: one of OP_PCL_IPSEC_*
 * @param[in] authdata    Pointer to authentication transform definitions. Note
 *      that since a split key is to be used, the size of the split key itself
 *      is specified. Valid algorithm values: one of OP_PCL_IPSEC_*
 */

/*
static inline void cnstr_jd_ipsec_new_encap(uint32_t *descbuf,
					       unsigned *bufsize,
					       unsigned short ps,
					       struct ipsec_encap_pdb *pdb,
					       struct alginfo *cipherdata,
					       struct alginfo *authdata,
					       uint64_t data_in_addr,
					       unsigned data_in_len,
					       uint64_t data_out_addr,
					       unsigned data_out_len)
{
	struct program prg;
	struct program *program = &prg;

	LABEL(hdr);
	REFERENCE(phdr);

	if (rta_sec_era < RTA_SEC_ERA_8) {
		pr_debug("IPsec new mode encap: available only for Era %d or above\n",
			 USER_SEC_ERA(RTA_SEC_ERA_8));
		*bufsize = 0;
		return;
	}

	PROGRAM_CNTXT_INIT(descbuf, 0);
	PROGRAM_SET_BSWAP();
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	phdr = JOB_HDR(SHR_NEVER, hdr, 0, 0);
	switch (pdb->options & PDBOPTS_ESP_OIHI_MASK) {
	case PDBOPTS_ESP_OIHI_PDB_INL:
		ENDIAN_DATA((uint8_t *)pdb,
			    sizeof(struct ipsec_encap_pdb) + pdb->ip_hdr_len);
		break;
	case PDBOPTS_ESP_OIHI_PDB_REF:
		if (ps)
			ENDIAN_DATA((uint8_t *)pdb,
				    sizeof(struct ipsec_encap_pdb) + BYTES_8);
		else
			ENDIAN_DATA((uint8_t *)pdb,
				    sizeof(struct ipsec_encap_pdb) + BYTES_4);
		break;
	default:
		ENDIAN_DATA((uint8_t *)pdb, sizeof(struct ipsec_encap_pdb));
		break;
	}

	SET_LABEL(hdr);
	SEQINPTR(data_in_addr, data_in_len, 0);
	SEQOUTPTR(data_out_addr, data_out_len, 0);
	if (authdata->keylen)
		KEY(MDHA_SPLIT_KEY, authdata->key_enc_flags, PTR(authdata->key),
		    authdata->keylen, 0);
	if (cipherdata->keylen)
		KEY(KEY1, cipherdata->key_enc_flags, PTR(cipherdata->key),
		    cipherdata->keylen, 0);
	PROTOCOL(OP_TYPE_ENCAP_PROTOCOL,
			 OP_PCLID_IPSEC_NEW,
			 (uint16_t)(cipherdata->algtype | authdata->algtype));

	PATCH_HDR(phdr, hdr);
	*bufsize = PROGRAM_FINALIZE();
}

*/
#endif /* IPSEC_DONT_USE_SEC_APP */
