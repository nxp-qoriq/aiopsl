/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_IPSEC_H__
#define __DESC_IPSEC_H__

#include "flib/rta.h"
#include "common.h"

/**
 * DOC: IPsec Shared Descriptor Constructors
 *
 * Shared descriptors for IPsec protocol.
 */

/* General IPSec ESP encap / decap PDB options */

/**
 * PDBOPTS_ESP_ESN - Extended sequence included
 */
#define PDBOPTS_ESP_ESN		0x10

/**
 * PDBOPTS_ESP_IPVSN - Process IPv6 header
 *
 * Valid only for IPsec legacy mode.
 */
#define PDBOPTS_ESP_IPVSN	0x02

/**
 * PDBOPTS_ESP_TUNNEL - Tunnel mode next-header byte
 *
 * Valid only for IPsec legacy mode.
 */
#define PDBOPTS_ESP_TUNNEL	0x01

/* IPSec ESP Encap PDB options */

/**
 * PDBOPTS_ESP_UPDATE_CSUM - Update ip header checksum
 *
 * Valid only for IPsec legacy mode.
 */
#define PDBOPTS_ESP_UPDATE_CSUM 0x80

/**
 * PDBOPTS_ESP_DIFFSERV - Copy TOS/TC from inner iphdr
 *
 * Valid only for IPsec legacy mode.
 */
#define PDBOPTS_ESP_DIFFSERV	0x40

/**
 * PDBOPTS_ESP_IVSRC - IV comes from internal random gen
 */
#define PDBOPTS_ESP_IVSRC	0x20

/**
 * PDBOPTS_ESP_IPHDRSRC - IP header comes from PDB
 *
 * Valid only for IPsec legacy mode.
 */
#define PDBOPTS_ESP_IPHDRSRC	0x08

/**
 * PDBOPTS_ESP_INCIPHDR - Prepend IP header to output frame
 *
 * Valid only for IPsec legacy mode.
 */
#define PDBOPTS_ESP_INCIPHDR	0x04

/**
 * PDBOPTS_ESP_OIHI_MASK - Mask for Outer IP Header Included
 *
 * Valid only for IPsec new mode.
 */
#define PDBOPTS_ESP_OIHI_MASK	0x0c

/**
 * PDBOPTS_ESP_OIHI_PDB_INL - Prepend IP header to output frame from PDB (where
 *                            it is inlined).
 *
 * Valid only for IPsec new mode.
 */
#define PDBOPTS_ESP_OIHI_PDB_INL 0x0c

/**
 * PDBOPTS_ESP_OIHI_PDB_REF - Prepend IP header to output frame from PDB
 *                            (referenced by pointer).
 *
 * Vlid only for IPsec new mode.
 */
#define PDBOPTS_ESP_OIHI_PDB_REF 0x08

/**
 * PDBOPTS_ESP_OIHI_IF - Prepend IP header to output frame from input frame
 *
 * Valid only for IPsec new mode.
 */
#define PDBOPTS_ESP_OIHI_IF	0x04

/**
 * PDBOPTS_ESP_NAT - Enable RFC 3948 UDP-encapsulated-ESP
 *
 * Valid only for IPsec new mode.
 */
#define PDBOPTS_ESP_NAT		0x02

/**
 * PDBOPTS_ESP_NUC - Enable NAT UDP Checksum
 *
 * Valid only for IPsec new mode.
 */
#define PDBOPTS_ESP_NUC		0x01

/* IPSec ESP Decap PDB options */

/**
 * PDBOPTS_ESP_ARSNONE - No antireplay window
 */
#define PDBOPTS_ESP_ARSNONE	0x00

/**
 * PDBOPTS_ESP_ARS64 - 64-entry antireplay window
 */
#define PDBOPTS_ESP_ARS64	0xc0

/**
 * PDBOPTS_ESP_ARS128 - 128-entry antireplay window
 *
 * Valid only for IPsec new mode.
 */
#define PDBOPTS_ESP_ARS128	0x80

/**
 * PDBOPTS_ESP_ARS32 - 32-entry antireplay window
 */
#define PDBOPTS_ESP_ARS32	0x40

/**
 * PDBOPTS_ESP_VERIFY_CSUM - Validate ip header checksum
 *
 * Valid only for IPsec legacy mode.
 */
#define PDBOPTS_ESP_VERIFY_CSUM 0x20

/**
 * PDBOPTS_ESP_TECN - Implement RRFC6040 ECN tunneling from outer header to
 *                    inner header.
 *
 * Valid only for IPsec new mode.
 */
#define PDBOPTS_ESP_TECN	0x20

/**
 * PDBOPTS_ESP_OUTFMT - Output only decapsulation
 *
 * Valid only for IPsec legacy mode.
 */
#define PDBOPTS_ESP_OUTFMT	0x08

/**
 * PDBOPTS_ESP_AOFL - Adjust out frame len
 *
 * Valid only for IPsec legacy mode and for SEC >= 5.3.
 */
#define PDBOPTS_ESP_AOFL	0x04

/**
 * PDBOPTS_ESP_ETU - EtherType Update
 *
 * Add corresponding ethertype (0x0800 for IPv4, 0x86dd for IPv6) in the output
 * frame.
 * Valid only for IPsec new mode.
 */
#define PDBOPTS_ESP_ETU		0x01

#define PDBHMO_ESP_DECAP_SHIFT	12
#define PDBHMO_ESP_ENCAP_SHIFT	4

/**
 * PDBHMO_ESP_DECAP_DTTL - IPsec ESP decrement TTL (IPv4) / Hop limit (IPv6)
 *                         HMO option.
 */
#define PDBHMO_ESP_DECAP_DTTL	(0x02 << PDBHMO_ESP_DECAP_SHIFT)

/**
 * PDBHMO_ESP_ENCAP_DTTL - IPsec ESP increment TTL (IPv4) / Hop limit (IPv6)
 *                         HMO option.
 */
#define PDBHMO_ESP_ENCAP_DTTL	(0x02 << PDBHMO_ESP_ENCAP_SHIFT)

/**
 * PDBHMO_ESP_DIFFSERV - (Decap) DiffServ Copy - Copy the IPv4 TOS or IPv6
 *                       Traffic Class byte from the outer IP header to the
 *                       inner IP header.
 */
#define PDBHMO_ESP_DIFFSERV	(0x01 << PDBHMO_ESP_DECAP_SHIFT)

/**
 * PDBHMO_ESP_SNR - (Encap) - Sequence Number Rollover control
 *
 * Configures behaviour in case of SN / ESN rollover:
 * error if SNR = 1, rollover allowed if SNR = 0.
 * Valid only for IPsec new mode.
 */
#define PDBHMO_ESP_SNR		(0x01 << PDBHMO_ESP_ENCAP_SHIFT)

/**
 * PDBHMO_ESP_DFBIT - (Encap) Copy DF bit - if an IPv4 tunnel mode outer IP
 *                    header is coming from the PDB, copy the DF bit from the
 *                    inner IP header to the outer IP header.
 */
#define PDBHMO_ESP_DFBIT	(0x04 << PDBHMO_ESP_ENCAP_SHIFT)

/**
 * PDBHMO_ESP_DFV - (Decap) - DF bit value
 *
 * If ODF = 1, DF bit in output frame is replaced by DFV.
 * Valid only from SEC Era 5 onwards.
 */
#define PDBHMO_ESP_DFV		(0x04 << PDBHMO_ESP_DECAP_SHIFT)

/**
 * PDBHMO_ESP_ODF - (Decap) Override DF bit in IPv4 header of decapsulated
 *                  output frame.
 *
 * If ODF = 1, DF is replaced with the value of DFV bit.
 * Valid only from SEC Era 5 onwards.
 */
#define PDBHMO_ESP_ODF		(0x08 << PDBHMO_ESP_DECAP_SHIFT)

/**
 * struct ipsec_encap_cbc - PDB part for IPsec CBC encapsulation
 * @iv: initialization vector
 */
struct ipsec_encap_cbc {
	uint32_t iv[4];
};


/**
 * struct ipsec_encap_ctr - PDB part for IPsec CTR encapsulation
 * @ctr_nonce: 4-byte nonce
 * @ctr_initial: initial count constant
 * @iv: initialization vector
 */
struct ipsec_encap_ctr {
	uint32_t ctr_nonce;
	uint32_t ctr_initial;
	uint32_t iv[2];
};

/**
 * struct ipsec_encap_ccm - PDB part for IPsec CCM encapsulation
 * @salt: 3-byte salt (lower 24 bits)
 * @b0_flags: CCM B0; managed by SEC
 * @ctr_flags: counter flags; constant equal to 0x3
 * @ctr_initial: initial count constant
 * @iv: initialization vector
 */
struct ipsec_encap_ccm {
	uint32_t salt;
	uint8_t b0_flags;
	uint8_t ctr_flags;
	uint16_t ctr_initial;
	uint32_t iv[2];
};

/**
 * struct ipsec_encap_gcm - PDB part for IPsec GCM encapsulation
 * @salt: 3-byte salt (lower 24 bits)
 * @rsvd1: reserved, do not use
 * @iv: initialization vector
 */
struct ipsec_encap_gcm {
	uint32_t salt;
	uint32_t rsvd1;
	uint32_t iv[2];
};

/**
 * struct ipsec_encap_pdb - PDB for IPsec encapsulation
 * @hmo: header manipulation options
 * @options: option flags; depend on selected algorithm
 * @seq_num_ext_hi: (optional) IPsec Extended Sequence Number (ESN)
 * @seq_num: IPsec sequence number
 * @spi: IPsec SPI (Security Parameters Index)
 * @rsvd2: reserved, do not use
 * @ip_hdr_len: optional IP Header length (in bytes)
 * @ip_hdr: optional IP Header content (only for IPsec legacy mode)
 */
struct ipsec_encap_pdb {
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
	uint32_t ip_hdr[0];
};

/**
 * struct ipsec_decap_cbc - PDB part for IPsec CBC decapsulation
 * @rsvd: reserved, do not use
 */
struct ipsec_decap_cbc {
	uint32_t rsvd[2];
};

/**
 * struct ipsec_decap_ctr - PDB part for IPsec CTR decapsulation
 * @salt: 4-byte salt
 * @ctr_initial: initial count constant
 */
struct ipsec_decap_ctr {
	uint32_t salt;
	uint32_t ctr_initial;
};

/**
 * struct ipsec_decap_ccm - PDB part for IPsec CCM decapsulation
 * @salt: 3-byte salt (lower 24 bits)
 * @iv_flags: TBD
 * @ctr_flags: counter flags
 * @ctr_initial: initial count constant
 */
struct ipsec_decap_ccm {
	uint32_t salt;
	uint8_t iv_flags;
	uint8_t ctr_flags;
	uint16_t ctr_initial;
};

/**
 * struct ipsec_decap_gcm - PDB part for IPsec GCN decapsulation
 * @salt: 4-byte salt
 * @resvd: reserved, do not used
 */
struct ipsec_decap_gcm {
	uint32_t salt;
	uint32_t resvd;
};

/**
 * struct ipsec_decap_pdb - PDB for IPsec decapsulation
 * @ip_hdr_len: HMO (upper nibble) + IP header length (lower 3 nibbles)
 * @options: option flags; depend on selected algorithm
 * @seq_num_ext_hi: (optional) IPsec Extended Sequence Number (ESN)
 * @seq_num: IPsec sequence number
 * @anti_replay: Anti-replay window bits array
 */
struct ipsec_decap_pdb {
	uint16_t ip_hdr_len;
	union {
		uint8_t ip_nh_offset;	/* next header offset for legacy mode */
		uint8_t aoipho;		/* actual outer IP header offset for
					 * new mode */
	};
	uint8_t options;
	union {
		struct ipsec_decap_cbc cbc;
		struct ipsec_decap_ctr ctr;
		struct ipsec_decap_ccm ccm;
		struct ipsec_decap_gcm gcm;
	};
	uint32_t seq_num_ext_hi;
	uint32_t seq_num;
	uint32_t anti_replay[4];
};

/**
 * enum ipsec_icv_size - Type selectors for icv size in IPsec protocol
 * @IPSEC_ICV_MD5_SIZE: full-length MD5 ICV
 * @IPSEC_ICV_MD5_TRUNC_SIZE: truncated MD5 ICV
 */
enum ipsec_icv_size {
	IPSEC_ICV_MD5_SIZE = 16,
	IPSEC_ICV_MD5_TRUNC_SIZE = 12
};

/*
 * IPSec ESP Datapath Protocol Override Register (DPOVRD)
 */

#define IPSEC_DECO_DPOVRD_USE		0x80

struct ipsec_deco_dpovrd {
	uint8_t ovrd_ecn;
	uint8_t ip_hdr_len;
	uint8_t nh_offset;
	union {
		uint8_t next_header;	/* next header if encap */
		uint8_t rsvd;		/* reserved if decap */
	};
};

struct ipsec_new_encap_deco_dpovrd {
#define IPSEC_NEW_ENCAP_DECO_DPOVRD_USE	0x8000
	uint16_t ovrd_ip_hdr_len;	/* OVRD + outer IP header material
					 * length */
#define IPSEC_NEW_ENCAP_OIMIF		0x80
	uint8_t oimif_aoipho;		/* OIMIF + actual outer IP header
					 * offset */
	uint8_t rsvd;
};

struct ipsec_new_decap_deco_dpovrd {
	uint8_t ovrd;
	uint8_t aoipho_hi;		/* upper nibble of actual outer IP
					 * header */
	uint16_t aoipho_lo_ip_hdr_len;	/* lower nibble of actual outer IP
					 * header + outer IP header material */
};

/**
 * cnstr_shdsc_ipsec_encap - IPSec ESP encapsulation protocol-level shared
 *                           descriptor. Requires an MDHA split key.
 * @descbuf: pointer to buffer used for descriptor construction
 * @ps: if 36/40bit addressing is desired, this parameter must be true
 * @swap: if true, perform descriptor byte swapping on a 4-byte boundary
 * @pdb: pointer to the PDB to be used with this descriptor
 *       This structure will be copied inline to the descriptor under
 *       construction. No error checking will be made. Refer to the
 *       block guide for a details of the encapsulation PDB.
 * @cipherdata: pointer to block cipher transform definitions
 *              Valid algorithm values - one of OP_PCL_IPSEC_*
 * @authdata: pointer to authentication transform definitions. Note that since a
 *            split key is to be used, the size of the split key itself is
 *            specified. Valid algorithm values - one of OP_PCL_IPSEC_*
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_ipsec_encap(uint32_t *descbuf, bool ps, bool swap,
					  struct ipsec_encap_pdb *pdb,
					  struct alginfo *cipherdata,
					  struct alginfo *authdata)
{
	struct program prg;
	struct program *p = &prg;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);
	LABEL(hdr);
	REFERENCE(phdr);

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	if (swap)
		PROGRAM_SET_BSWAP(p);
	if (ps)
		PROGRAM_SET_36BIT_ADDR(p);
	phdr = SHR_HDR(p, SHR_SERIAL, hdr, 0);
	COPY_DATA(p, (uint8_t *)pdb,
		  sizeof(struct ipsec_encap_pdb) + pdb->ip_hdr_len);
	SET_LABEL(p, hdr);
	pkeyjmp = JUMP(p, keyjmp, LOCAL_JUMP, ALL_TRUE, BOTH|SHRD);
	KEY(p, MDHA_SPLIT_KEY, authdata->key_enc_flags, authdata->key,
	    authdata->keylen, INLINE_KEY(authdata));
	KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
	    cipherdata->keylen, INLINE_KEY(cipherdata));
	SET_LABEL(p, keyjmp);
	PROTOCOL(p, OP_TYPE_ENCAP_PROTOCOL,
		 OP_PCLID_IPSEC,
		 (uint16_t)(cipherdata->algtype | authdata->algtype));
	PATCH_JUMP(p, pkeyjmp, keyjmp);
	PATCH_HDR(p, phdr, hdr);
	return PROGRAM_FINALIZE(p);
}

/**
 * cnstr_shdsc_ipsec_decap - IPSec ESP decapsulation protocol-level sharedesc
 *                           Requires an MDHA split key.
 * @descbuf: pointer to buffer used for descriptor construction
 * @ps: if 36/40bit addressing is desired, this parameter must be true
 * @swap: if true, perform descriptor byte swapping on a 4-byte boundary
 * @pdb: pointer to the PDB to be used with this descriptor
 *       This structure will be copied inline to the descriptor under
 *       construction. No error checking will be made. Refer to the
 *       block guide for details about the decapsulation PDB.
 * @cipherdata: pointer to block cipher transform definitions.
 *              Valid algorithm values - one of OP_PCL_IPSEC_*
 * @authdata: pointer to authentication transform definitions. Note that since a
 *            split key is to be used, the size of the split key itself is
 *            specified. Valid algorithm values - one of OP_PCL_IPSEC_*
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_ipsec_decap(uint32_t *descbuf, bool ps, bool swap,
					  struct ipsec_decap_pdb *pdb,
					  struct alginfo *cipherdata,
					  struct alginfo *authdata)
{
	struct program prg;
	struct program *p = &prg;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);
	LABEL(hdr);
	REFERENCE(phdr);

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	if (swap)
		PROGRAM_SET_BSWAP(p);
	if (ps)
		PROGRAM_SET_36BIT_ADDR(p);
	phdr = SHR_HDR(p, SHR_SERIAL, hdr, 0);
	COPY_DATA(p, (uint8_t *)pdb, sizeof(struct ipsec_decap_pdb));
	SET_LABEL(p, hdr);
	pkeyjmp = JUMP(p, keyjmp, LOCAL_JUMP, ALL_TRUE, BOTH|SHRD);
	KEY(p, MDHA_SPLIT_KEY, authdata->key_enc_flags, authdata->key,
	    authdata->keylen, INLINE_KEY(authdata));
	KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
	    cipherdata->keylen, INLINE_KEY(cipherdata));
	SET_LABEL(p, keyjmp);
	PROTOCOL(p, OP_TYPE_DECAP_PROTOCOL,
		 OP_PCLID_IPSEC,
		 (uint16_t)(cipherdata->algtype | authdata->algtype));
	PATCH_JUMP(p, pkeyjmp, keyjmp);
	PATCH_HDR(p, phdr, hdr);
	return PROGRAM_FINALIZE(p);
}

/**
 * cnstr_shdsc_ipsec_encap_des_aes_xcbc - IPSec DES-CBC/3DES-CBC and
 *     AES-XCBC-MAC-96 ESP encapsulation shared descriptor.
 * @descbuf: pointer to buffer used for descriptor construction
 * @pdb: pointer to the PDB to be used with this descriptor
 *       This structure will be copied inline to the descriptor under
 *       construction. No error checking will be made. Refer to the
 *       block guide for a details of the encapsulation PDB.
 * @cipherdata: pointer to block cipher transform definitions
 *              Valid algorithm values - OP_PCL_IPSEC_DES, OP_PCL_IPSEC_3DES.
 * @authdata: pointer to authentication transform definitions
 *            Valid algorithm value: OP_PCL_IPSEC_AES_XCBC_MAC_96.
 *
 * Supported only for platforms with 32-bit address pointers and SEC ERA 4 or
 * higher. The tunnel/transport mode of the IPsec ESP is supported only if the
 * Outer/Transport IP Header is present in the encapsulation output packet.
 * The descriptor performs DES-CBC/3DES-CBC & HMAC-MD5-96 and then rereads
 * the input packet to do the AES-XCBC-MAC-96 calculation and to overwrite
 * the MD5 ICV.
 * The descriptor uses all the benefits of the built-in protocol by computing
 * the IPsec ESP with a hardware supported algorithms combination
 * (DES-CBC/3DES-CBC & HMAC-MD5-96). The HMAC-MD5 authentication algorithm
 * was chosen in order to speed up the computational time for this intermediate
 * step.
 * Warning: The user must allocate at least 32 bytes for the authentication key
 * (in order to use it also with HMAC-MD5-96),even when using a shorter key
 * for the AES-XCBC-MAC-96.
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_ipsec_encap_des_aes_xcbc(uint32_t *descbuf,
		struct ipsec_encap_pdb *pdb, struct alginfo *cipherdata,
		struct alginfo *authdata)
{
	struct program prg;
	struct program *p = &prg;

	LABEL(hdr);
	LABEL(shd_ptr);
	LABEL(keyjump);
	LABEL(outptr);
	LABEL(swapped_seqin_fields);
	LABEL(swapped_seqin_ptr);
	REFERENCE(phdr);
	REFERENCE(pkeyjump);
	REFERENCE(move_outlen);
	REFERENCE(move_seqout_ptr);
	REFERENCE(swapped_seqin_ptr_jump);
	REFERENCE(write_swapped_seqin_ptr);

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	phdr = SHR_HDR(p, SHR_SERIAL, hdr, 0);
	COPY_DATA(p, (uint8_t *)pdb,
		  sizeof(struct ipsec_encap_pdb) + pdb->ip_hdr_len);
	SET_LABEL(p, hdr);
	pkeyjump = JUMP(p, keyjump, LOCAL_JUMP, ALL_TRUE, SHRD | SELF);
	/*
	 * Hard-coded KEY arguments. The descriptor uses all the benefits of
	 * the built-in protocol by computing the IPsec ESP with a hardware
	 * supported algorithms combination (DES-CBC/3DES-CBC & HMAC-MD5-96).
	 * The HMAC-MD5 authentication algorithm was chosen with
	 * the keys options from below in order to speed up the computational
	 * time for this intermediate step.
	 * Warning: The user must allocate at least 32 bytes for
	 * the authentication key (in order to use it also with HMAC-MD5-96),
	 * even when using a shorter key for the AES-XCBC-MAC-96.
	 */
	KEY(p, MDHA_SPLIT_KEY, 0, authdata->key, 32, INLINE_KEY(authdata));
	SET_LABEL(p, keyjump);
	LOAD(p, LDST_SRCDST_WORD_CLRW | CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS |
	     CLRW_CLR_C1CTX | CLRW_CLR_C1KEY | CLRW_RESET_CLS1_CHA, CLRW, 0, 4,
	     IMMED);
	KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
	    cipherdata->keylen, INLINE_KEY(cipherdata));
	PROTOCOL(p, OP_TYPE_ENCAP_PROTOCOL, OP_PCLID_IPSEC,
		 (uint16_t)(cipherdata->algtype | OP_PCL_IPSEC_HMAC_MD5_96));
	/* Swap SEQINPTR to SEQOUTPTR. */
	move_seqout_ptr = MOVE(p, DESCBUF, 0, MATH1, 0, 16, WAITCOMP | IMMED);
	MATHB(p, MATH1, AND, ~(CMD_SEQ_IN_PTR ^ CMD_SEQ_OUT_PTR), MATH1,
	      8, IFB | IMMED2);
/*
 * TODO: RTA currently doesn't support creating a LOAD command
 * with another command as IMM.
 * To be changed when proper support is added in RTA.
 */
	LOAD(p, 0xa00000e5, MATH3, 4, 4, IMMED);
	MATHB(p, MATH3, SHLD, MATH3, MATH3,  8, 0);
	write_swapped_seqin_ptr = MOVE(p, MATH1, 0, DESCBUF, 0, 20, WAITCOMP |
				       IMMED);
	swapped_seqin_ptr_jump = JUMP(p, swapped_seqin_ptr, LOCAL_JUMP,
				      ALL_TRUE, 0);
	LOAD(p, LDST_SRCDST_WORD_CLRW | CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS |
	     CLRW_CLR_C1CTX | CLRW_CLR_C1KEY | CLRW_RESET_CLS1_CHA, CLRW, 0, 4,
	     0);
	SEQOUTPTR(p, 0, 65535, RTO);
	move_outlen = MOVE(p, DESCBUF, 0, MATH0, 4, 8, WAITCOMP | IMMED);
	MATHB(p, MATH0, SUB,
	      (uint64_t)(pdb->ip_hdr_len + IPSEC_ICV_MD5_TRUNC_SIZE),
	      VSEQINSZ, 4, IMMED2);
	MATHB(p, MATH0, SUB, IPSEC_ICV_MD5_TRUNC_SIZE, VSEQOUTSZ, 4, IMMED2);
	KEY(p, KEY1, authdata->key_enc_flags, authdata->key, authdata->keylen,
	    0);
	ALG_OPERATION(p, OP_ALG_ALGSEL_AES, OP_ALG_AAI_XCBC_MAC,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_DISABLE, DIR_ENC);
	SEQFIFOLOAD(p, SKIP, pdb->ip_hdr_len, 0);
	SEQFIFOLOAD(p, MSG1, 0, VLF | FLUSH1 | LAST1);
	SEQFIFOSTORE(p, SKIP, 0, 0, VLF);
	SEQSTORE(p, CONTEXT1, 0, IPSEC_ICV_MD5_TRUNC_SIZE, 0);
/*
 * TODO: RTA currently doesn't support adding labels in or after Job Descriptor.
 * To be changed when proper support is added in RTA.
 */
	/* Label the Shared Descriptor Pointer */
	SET_LABEL(p, shd_ptr);
	shd_ptr += 1;
	/* Label the Output Pointer */
	SET_LABEL(p, outptr);
	outptr += 3;
	/* Label the first word after JD */
	SET_LABEL(p, swapped_seqin_fields);
	swapped_seqin_fields += 8;
	/* Label the second word after JD */
	SET_LABEL(p, swapped_seqin_ptr);
	swapped_seqin_ptr += 9;

	PATCH_HDR(p, phdr, hdr);
	PATCH_JUMP(p, pkeyjump, keyjump);
	PATCH_JUMP(p, swapped_seqin_ptr_jump, swapped_seqin_ptr);
	PATCH_MOVE(p, move_outlen, outptr);
	PATCH_MOVE(p, move_seqout_ptr, shd_ptr);
	PATCH_MOVE(p, write_swapped_seqin_ptr, swapped_seqin_fields);
	return PROGRAM_FINALIZE(p);
}

/**
 * cnstr_shdsc_ipsec_decap_des_aes_xcbc - IPSec DES-CBC/3DES-CBC and
 *     AES-XCBC-MAC-96 ESP decapsulation shared descriptor.
 * @descbuf: pointer to buffer used for descriptor construction
 * @pdb: pointer to the PDB to be used with this descriptor
 *       This structure will be copied inline to the descriptor under
 *       construction. No error checking will be made. Refer to the
 *       block guide for a details of the encapsulation PDB.
 * @cipherdata: pointer to block cipher transform definitions
 *              Valid algorithm values - OP_PCL_IPSEC_DES, OP_PCL_IPSEC_3DES.
 * @authdata: pointer to authentication transform definitions
 *            Valid algorithm value: OP_PCL_IPSEC_AES_XCBC_MAC_96.
 *
 * Supported only for platforms with 32-bit address pointers and SEC ERA 4 or
 * higher. The tunnel/transport mode of the IPsec ESP is supported only if the
 * Outer/Transport IP Header is present in the decapsulation input packet.
 * The descriptor computes the AES-XCBC-MAC-96 to check if the received ICV
 * is correct, rereads the input packet to compute the MD5 ICV, overwrites
 * the XCBC ICV, and then sends the modified input packet to the
 * DES-CBC/3DES-CBC & HMAC-MD5-96 IPsec.
 * The descriptor uses all the benefits of the built-in protocol by computing
 * the IPsec ESP with a hardware supported algorithms combination
 * (DES-CBC/3DES-CBC & HMAC-MD5-96). The HMAC-MD5 authentication algorithm
 * was chosen in order to speed up the computational time for this intermediate
 * step.
 * Warning: The user must allocate at least 32 bytes for the authentication key
 * (in order to use it also with HMAC-MD5-96),even when using a shorter key
 * for the AES-XCBC-MAC-96.
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_ipsec_decap_des_aes_xcbc(uint32_t *descbuf,
		struct ipsec_decap_pdb *pdb, struct alginfo *cipherdata,
		struct alginfo *authdata)
{
	struct program prg;
	struct program *p = &prg;

	LABEL(hdr);
	LABEL(jump_cmd);
	LABEL(keyjump);
	LABEL(outlen);
	LABEL(seqin_ptr);
	LABEL(seqout_ptr);
	LABEL(swapped_seqout_fields);
	LABEL(swapped_seqout_ptr);
	REFERENCE(seqout_ptr_jump);
	REFERENCE(phdr);
	REFERENCE(pkeyjump);
	REFERENCE(move_jump);
	REFERENCE(move_jump_back);
	REFERENCE(move_seqin_ptr);
	REFERENCE(swapped_seqout_ptr_jump);
	REFERENCE(write_swapped_seqout_ptr);

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	phdr = SHR_HDR(p, SHR_SERIAL, hdr, 0);
	COPY_DATA(p, (uint8_t *)pdb, sizeof(struct ipsec_decap_pdb));
	SET_LABEL(p, hdr);
	pkeyjump = JUMP(p, keyjump, LOCAL_JUMP, ALL_TRUE, SHRD | SELF);
	/*
	 * Hard-coded KEY arguments. The descriptor uses all the benefits of
	 * the built-in protocol by computing the IPsec ESP with a hardware
	 * supported algorithms combination (DES-CBC/3DES-CBC & HMAC-MD5-96).
	 * The HMAC-MD5 authentication algorithm was chosen with
	 * the keys options from bellow in order to speed up the computational
	 * time for this intermediate step.
	 * Warning: The user must allocate at least 32 bytes for
	 * the authentication key (in order to use it also with HMAC-MD5-96),
	 * even when using a shorter key for the AES-XCBC-MAC-96.
	 */
	KEY(p, MDHA_SPLIT_KEY, 0, authdata->key, 32, INLINE_KEY(authdata));
	SET_LABEL(p, keyjump);
	LOAD(p, LDST_SRCDST_WORD_CLRW | CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS |
	     CLRW_CLR_C1CTX | CLRW_CLR_C1KEY | CLRW_RESET_CLS1_CHA, CLRW, 0, 4,
	     0);
	KEY(p, KEY1, authdata->key_enc_flags, authdata->key, authdata->keylen,
	    INLINE_KEY(authdata));
	MATHB(p, SEQINSZ, SUB,
	      (uint64_t)(pdb->ip_hdr_len + IPSEC_ICV_MD5_TRUNC_SIZE),
	      MATH0, 4, IMMED2);
	MATHB(p, MATH0, SUB, ZERO, VSEQINSZ, 4, 0);
	ALG_OPERATION(p, OP_ALG_ALGSEL_MD5, OP_ALG_AAI_HMAC_PRECOMP,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_DISABLE, DIR_ENC);
	ALG_OPERATION(p, OP_ALG_ALGSEL_AES, OP_ALG_AAI_XCBC_MAC,
		      OP_ALG_AS_INITFINAL, ICV_CHECK_ENABLE, DIR_DEC);
	SEQFIFOLOAD(p, SKIP, pdb->ip_hdr_len, 0);
	SEQFIFOLOAD(p, MSG1, 0, VLF | FLUSH1);
	SEQFIFOLOAD(p, ICV1, IPSEC_ICV_MD5_TRUNC_SIZE, FLUSH1 | LAST1);
	/* Swap SEQOUTPTR to SEQINPTR. */
	move_seqin_ptr = MOVE(p, DESCBUF, 0, MATH1, 0, 16, WAITCOMP | IMMED);
	MATHB(p, MATH1, OR, CMD_SEQ_IN_PTR ^ CMD_SEQ_OUT_PTR, MATH1, 8,
	      IFB | IMMED2);
/*
 * TODO: RTA currently doesn't support creating a LOAD command
 * with another command as IMM.
 * To be changed when proper support is added in RTA.
 */
	LOAD(p, 0xA00000e1, MATH3, 4, 4, IMMED);
	MATHB(p, MATH3, SHLD, MATH3, MATH3,  8, 0);
	write_swapped_seqout_ptr = MOVE(p, MATH1, 0, DESCBUF, 0, 20, WAITCOMP |
					IMMED);
	swapped_seqout_ptr_jump = JUMP(p, swapped_seqout_ptr, LOCAL_JUMP,
				       ALL_TRUE, 0);
/*
 * TODO: To be changed when proper support is added in RTA (can't load
 * a command that is also written by RTA).
 * Change when proper RTA support is added.
 */
	SET_LABEL(p, jump_cmd);
	WORD(p, 0xA00000f3);
	SEQINPTR(p, 0, 65535, RTO);
	MATHB(p, MATH0, SUB, ZERO, VSEQINSZ, 4, 0);
	MATHB(p, MATH0, ADD, pdb->ip_hdr_len, VSEQOUTSZ, 4, IMMED2);
	move_jump = MOVE(p, DESCBUF, 0, OFIFO, 0, 8, WAITCOMP | IMMED);
	move_jump_back = MOVE(p, OFIFO, 0, DESCBUF, 0, 8, IMMED);
	SEQFIFOLOAD(p, SKIP, pdb->ip_hdr_len, 0);
	SEQFIFOLOAD(p, MSG2, 0, VLF | LAST2);
	SEQFIFOSTORE(p, SKIP, 0, 0, VLF);
	SEQSTORE(p, CONTEXT2, 0, IPSEC_ICV_MD5_TRUNC_SIZE, 0);
	seqout_ptr_jump = JUMP(p, seqout_ptr, LOCAL_JUMP, ALL_TRUE, CALM);

	LOAD(p, LDST_SRCDST_WORD_CLRW | CLRW_CLR_C1MODE | CLRW_CLR_C1DATAS |
	     CLRW_CLR_C1CTX | CLRW_CLR_C1KEY | CLRW_CLR_C2MODE |
	     CLRW_CLR_C2DATAS | CLRW_CLR_C2CTX | CLRW_RESET_CLS1_CHA, CLRW, 0,
	     4, 0);
	SEQINPTR(p, 0, 65535, RTO);
	MATHB(p, MATH0, ADD,
	      (uint64_t)(pdb->ip_hdr_len + IPSEC_ICV_MD5_TRUNC_SIZE),
	      SEQINSZ, 4, IMMED2);
	KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
	    cipherdata->keylen, INLINE_KEY(cipherdata));
	PROTOCOL(p, OP_TYPE_DECAP_PROTOCOL, OP_PCLID_IPSEC,
		 (uint16_t)(cipherdata->algtype | OP_PCL_IPSEC_HMAC_MD5_96));
/*
 * TODO: RTA currently doesn't support adding labels in or after Job Descriptor.
 * To be changed when proper support is added in RTA.
 */
	/* Label the SEQ OUT PTR */
	SET_LABEL(p, seqout_ptr);
	seqout_ptr += 2;
	/* Label the Output Length */
	SET_LABEL(p, outlen);
	outlen += 4;
	/* Label the SEQ IN PTR */
	SET_LABEL(p, seqin_ptr);
	seqin_ptr += 5;
	/* Label the first word after JD */
	SET_LABEL(p, swapped_seqout_fields);
	swapped_seqout_fields += 8;
	/* Label the second word after JD */
	SET_LABEL(p, swapped_seqout_ptr);
	swapped_seqout_ptr += 9;

	PATCH_HDR(p, phdr, hdr);
	PATCH_JUMP(p, pkeyjump, keyjump);
	PATCH_JUMP(p, seqout_ptr_jump, seqout_ptr);
	PATCH_JUMP(p, swapped_seqout_ptr_jump, swapped_seqout_ptr);
	PATCH_MOVE(p, move_jump, jump_cmd);
	PATCH_MOVE(p, move_jump_back, seqin_ptr);
	PATCH_MOVE(p, move_seqin_ptr, outlen);
	PATCH_MOVE(p, write_swapped_seqout_ptr, swapped_seqout_fields);
	return PROGRAM_FINALIZE(p);
}

/**
 * IPSEC_NEW_ENC_BASE_DESC_LEN - IPsec new mode encap shared descriptor length
 *
 * Accounts only for the "base" commands and is intended to be used by upper
 * layers to determine whether Outer IP Header and/or keys can be inlined or
 * not. To be used as first parameter of rta_inline_query().
 */
#define IPSEC_NEW_ENC_BASE_DESC_LEN	(5 * CAAM_CMD_SZ + \
					 sizeof(struct ipsec_encap_pdb))

/**
 * IPSEC_NEW_NULL_ENC_BASE_DESC_LEN - IPsec new mode encap shared descriptor
 *                                    length for the case of
 *                                    NULL encryption / authentication
 *
 * Accounts only for the "base" commands and is intended to be used by upper
 * layers to determine whether Outer IP Header and/or key can be inlined or
 * not. To be used as first parameter of rta_inline_query().
 */
#define IPSEC_NEW_NULL_ENC_BASE_DESC_LEN	(4 * CAAM_CMD_SZ + \
						 sizeof(struct ipsec_encap_pdb))

/**
 * cnstr_shdsc_ipsec_new_encap -  IPSec new mode ESP encapsulation
 *     protocol-level shared descriptor. If an authentication key is required by
 *     the protocol, it must be a MDHA split key.
 * @descbuf: pointer to buffer used for descriptor construction
 * @ps: if 36/40bit addressing is desired, this parameter must be true
 * @pdb: pointer to the PDB to be used with this descriptor
 *       This structure will be copied inline to the descriptor under
 *       construction. No error checking will be made. Refer to the
 *       block guide for details about the encapsulation PDB.
 * @opt_ip_hdr:  pointer to Optional IP Header
 *     -if OIHI = PDBOPTS_ESP_OIHI_PDB_INL, opt_ip_hdr points to the buffer to
 *     be inlined in the PDB. Number of bytes (buffer size) copied is provided
 *     in pdb->ip_hdr_len.
 *     -if OIHI = PDBOPTS_ESP_OIHI_PDB_REF, opt_ip_hdr points to the address of
 *     the Optional IP Header. The address will be inlined in the PDB verbatim.
 *     -for other values of OIHI options field, opt_ip_hdr is not used.
 * @cipherdata: pointer to block cipher transform definitions
 *              Valid algorithm values - one of OP_PCL_IPSEC_*
 * @authdata: pointer to authentication transform definitions. Note that since a
 *            split key is to be used, the size of the split key itself is
 *            specified. Valid algorithm values - one of OP_PCL_IPSEC_*
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_ipsec_new_encap(uint32_t *descbuf, bool ps,
					      struct ipsec_encap_pdb *pdb,
					      uint8_t *opt_ip_hdr,
					      struct alginfo *cipherdata,
					      struct alginfo *authdata)
{
	struct program prg;
	struct program *p = &prg;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);
	LABEL(hdr);
	REFERENCE(phdr);

	if (rta_sec_era < RTA_SEC_ERA_8) {
		pr_err("IPsec new mode encap: available only for Era %d or above\n",
		       USER_SEC_ERA(RTA_SEC_ERA_8));
		return -ENOTSUP;
	}

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	PROGRAM_SET_BSWAP(p);
	if (ps)
		PROGRAM_SET_36BIT_ADDR(p);
	phdr = SHR_HDR(p, SHR_SERIAL, hdr, 0);

	COPY_DATA(p, (uint8_t *)pdb, sizeof(struct ipsec_encap_pdb));

	switch (pdb->options & PDBOPTS_ESP_OIHI_MASK) {
	case PDBOPTS_ESP_OIHI_PDB_INL:
		COPY_DATA(p, opt_ip_hdr, pdb->ip_hdr_len);
		break;
	case PDBOPTS_ESP_OIHI_PDB_REF:
		if (ps)
			COPY_DATA(p, opt_ip_hdr, 8);
		else
			COPY_DATA(p, opt_ip_hdr, 4);
		break;
	default:
		break;
	}
	SET_LABEL(p, hdr);

	pkeyjmp = JUMP(p, keyjmp, LOCAL_JUMP, ALL_TRUE, SHRD);
	if (authdata->keylen)
		KEY(p, MDHA_SPLIT_KEY, authdata->key_enc_flags, authdata->key,
		    authdata->keylen, INLINE_KEY(authdata));
	if (cipherdata->keylen)
		KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
		    cipherdata->keylen, INLINE_KEY(cipherdata));
	SET_LABEL(p, keyjmp);
	PROTOCOL(p, OP_TYPE_ENCAP_PROTOCOL,
		 OP_PCLID_IPSEC_NEW,
		 (uint16_t)(cipherdata->algtype | authdata->algtype));
	PATCH_JUMP(p, pkeyjmp, keyjmp);
	PATCH_HDR(p, phdr, hdr);
	return PROGRAM_FINALIZE(p);
}

/**
 * IPSEC_NEW_DEC_BASE_DESC_LEN - IPsec new mode decap shared descriptor length
 *
 * Accounts only for the "base" commands and is intended to be used by upper
 * layers to determine whether keys can be inlined or not. To be used as first
 * parameter of rta_inline_query().
 */
#define IPSEC_NEW_DEC_BASE_DESC_LEN	(5 * CAAM_CMD_SZ + \
					 sizeof(struct ipsec_decap_pdb))

/**
 * IPSEC_NEW_NULL_DEC_BASE_DESC_LEN - IPsec new mode decap shared descriptor
 *                                    length for the case of
 *                                    NULL decryption / authentication
 *
 * Accounts only for the "base" commands and is intended to be used by upper
 * layers to determine whether key can be inlined or not. To be used as first
 * parameter of rta_inline_query().
 */
#define IPSEC_NEW_NULL_DEC_BASE_DESC_LEN	(4 * CAAM_CMD_SZ + \
						 sizeof(struct ipsec_decap_pdb))

/**
 * cnstr_shdsc_ipsec_new_decap - IPSec new mode ESP decapsulation protocol-level
 *     shared descriptor. If an authentication key is required by the protocol,
 *     it must be a MDHA split key.
 * @descbuf: pointer to buffer used for descriptor construction
 * @ps: if 36/40bit addressing is desired, this parameter must be true
 * @pdb: pointer to the PDB to be used with this descriptor
 *       This structure will be copied inline to the descriptor under
 *       construction. No error checking will be made. Refer to the
 *       block guide for details about the decapsulation PDB.
 * @cipherdata: pointer to block cipher transform definitions
 *              Valid algorithm values 0 one of OP_PCL_IPSEC_*
 * @authdata: pointer to authentication transform definitions. Note that since a
 *            split key is to be used, the size of the split key itself is
 *            specified. Valid algorithm values - one of OP_PCL_IPSEC_*
 *
 * Return: size of descriptor written in words or negative number on error
 */
static inline int cnstr_shdsc_ipsec_new_decap(uint32_t *descbuf, bool ps,
					      struct ipsec_decap_pdb *pdb,
					      struct alginfo *cipherdata,
					      struct alginfo *authdata)
{
	struct program prg;
	struct program *p = &prg;

	LABEL(keyjmp);
	REFERENCE(pkeyjmp);
	LABEL(hdr);
	REFERENCE(phdr);

	if (rta_sec_era < RTA_SEC_ERA_8) {
		pr_err("IPsec new mode decap: available only for Era %d or above\n",
		       USER_SEC_ERA(RTA_SEC_ERA_8));
		return -ENOTSUP;
	}

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	PROGRAM_SET_BSWAP(p);
	if (ps)
		PROGRAM_SET_36BIT_ADDR(p);
	phdr = SHR_HDR(p, SHR_SERIAL, hdr, 0);
	COPY_DATA(p, (uint8_t *)pdb, sizeof(struct ipsec_decap_pdb));
	SET_LABEL(p, hdr);
	pkeyjmp = JUMP(p, keyjmp, LOCAL_JUMP, ALL_TRUE, SHRD);
	if (authdata->keylen)
		KEY(p, MDHA_SPLIT_KEY, authdata->key_enc_flags, authdata->key,
		    authdata->keylen, INLINE_KEY(authdata));
	if (cipherdata->keylen)
		KEY(p, KEY1, cipherdata->key_enc_flags, cipherdata->key,
		    cipherdata->keylen, INLINE_KEY(cipherdata));
	SET_LABEL(p, keyjmp);
	PROTOCOL(p, OP_TYPE_DECAP_PROTOCOL,
		 OP_PCLID_IPSEC_NEW,
		 (uint16_t)(cipherdata->algtype | authdata->algtype));
	PATCH_JUMP(p, pkeyjmp, keyjmp);
	PATCH_HDR(p, phdr, hdr);
	return PROGRAM_FINALIZE(p);
}

#endif /* __DESC_IPSEC_H__ */
