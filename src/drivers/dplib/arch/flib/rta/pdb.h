/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_PDB_H__
#define __RTA_PDB_H__

/**
 * @defgroup pdb_group SEC Protocol Data Block Data Structures
 * @ingroup descriptor_lib_group
 * @{
 */
/** @} end of pdb_group */

/**
 * @defgroup ipsec_encap_pdb ipsec_encap_pdb
 * @ingroup pdb_group
 * @{
 */
/** @} end of ipsec_encap_pdb */

/**
 * @defgroup ipsec_decap_pdb ipsec_decap_pdb
 * @ingroup pdb_group
 * @{
 */
/** @} end of ipsec_decap_pdb */

/** @addtogroup defines_group
 *  @{
 */
/* General IPSec ESP encap / decap PDB options */
#define PDBOPTS_ESP_ESN		0x10   /**< extended sequence included */
#define PDBOPTS_ESP_IPVSN	0x02   /**< process IPv6 header
					    valid only for IPsec legacy mode */
#define PDBOPTS_ESP_TUNNEL	0x01   /**< tunnel mode next-header byte
					    valid only for IPsec legacy mode */

/* IPSec ESP Encap PDB options */
#define PDBOPTS_ESP_UPDATE_CSUM 0x80   /**< update ip header checksum
					    valid only for IPsec legacy mode */
#define PDBOPTS_ESP_DIFFSERV	0x40   /**< copy TOS/TC from inner iphdr
					    valid only for IPsec legacy mode */
#define PDBOPTS_ESP_IVSRC	0x20   /**< IV comes from internal random gen */
#define PDBOPTS_ESP_IPHDRSRC	0x08   /**< IP header comes from PDB */
#define PDBOPTS_ESP_INCIPHDR	0x04   /**< prepend IP header to output frame */
#define PDBOPTS_ESP_NAT		0x02   /**< enable RFC 3948 UDP-encapsulated-ESP
					    valid only for IPsec new mode */
#define PDBOPTS_ESP_NUC		0x01   /**< enable NAT UDP Checksum
					    valid only for IPsec new mode */

/* IPSec ESP Decap PDB options */
#define PDBOPTS_ESP_ARSNONE	0x00   /**< no antireplay window */
#define PDBOPTS_ESP_ARS64	0xc0   /**< 64-entry antireplay window */
#define PDBOPTS_ESP_ARS128	0x80   /**< 128-entry antireplay window
					    valid only for IPsec new mode */
#define PDBOPTS_ESP_ARS32	0x40   /**< 32-entry antireplay window */
#define PDBOPTS_ESP_VERIFY_CSUM 0x20   /**< validate ip header checksum
					    valid only for IPsec legacy mode */
#define PDBOPTS_ESP_TECN	0x20   /**< implement RRFC6040 ECN tunneling
					    from outer header to inner header;
					    valid only for IPsec new mode */
#define PDBOPTS_ESP_OUTFMT	0x08   /**< output only decapsulation
					    valid only for IPsec legacy mode */
#define PDBOPTS_ESP_AOFL	0x04   /**< adjust out frame len (SEC>=5.3)
					    valid only for IPsec legacy mode */
#define PDBOPTS_ESP_ETU		0x01   /**< EtherType Update - add corresponding
					    ethertype (0x0800 for IPv4, 0x86dd
					    for IPv6) in the output frame;
					    valid only for IPsec new mode */

#define PDBHMO_ESP_DECAP_SHIFT	12
#define PDBHMO_ESP_ENCAP_SHIFT	4

/**
 * IPsec ESP decrement TTL (IPv4) / Hop limit (IPv6) HMO option
 */
#define PDBHMO_ESP_DECAP_DTTL	(0x02 << PDBHMO_ESP_DECAP_SHIFT)
#define PDBHMO_ESP_ENCAP_DTTL	(0x02 << PDBHMO_ESP_ENCAP_SHIFT)

/**
 * Decap - DiffServ Copy - Copy the IPv4 TOS or IPv6 Traffic Class byte
 * from the outer IP header to the inner IP header.
 */
#define PDBHMO_ESP_DIFFSERV	(0x01 << PDBHMO_ESP_DECAP_SHIFT)

/**
 * Encap - Sequence Number Rollover control
 * Valid only for IPsec new mode.
 * Configures behaviour in case of SN / ESN rollover:
 * error if SNR = 1, rollover allowed if SNR = 0.
 */
#define PDBHMO_ESP_SNR		(0x01 << PDBHMO_ESP_ENCAP_SHIFT)

/**
 * Encap - Copy DF bit - if an IPv4 tunnel mode outer IP header is coming from
 * the PDB, copy the DF bit from the inner IP header to the outer IP header.
 */
#define PDBHMO_ESP_DFBIT	(0x04 << PDBHMO_ESP_ENCAP_SHIFT)

/**
 * Decap - DF bit value
 * Valid only from SEC Era 5 onwards.
 * If ODF = 1, DF bit in output frame is replaced by DFV.
 */
#define PDBHMO_ESP_DFV		(0x04 << PDBHMO_ESP_DECAP_SHIFT)

/**
 * Decap - Override DF bit in IPv4 header of decapsulated output frame
 * If ODF = 1, DF is replaced with the value of DFV bit.
 * Valid only from SEC Era 5 onwards.
 */
#define PDBHMO_ESP_ODF		(0x08 << PDBHMO_ESP_DECAP_SHIFT)

/** @} */ /* end of defines_group */

/**
 * @struct    ipsec_encap_cbc pdb.h
 * @ingroup   ipsec_encap_pdb
 * @details   IV field for IPsec CBC encapsulation
 */
struct ipsec_encap_cbc {
	uint32_t iv[4];
};


/**
 * @struct    ipsec_encap_ctr pdb.h
 * @ingroup   ipsec_encap_pdb
 * @details   Nonce and IV fields for IPsec CTR encapsulation
 */
struct ipsec_encap_ctr {
	uint32_t ctr_nonce;
	uint32_t ctr_initial;
	uint32_t iv[2];
};

/**
 * @struct    ipsec_encap_ccm pdb.h
 * @ingroup   ipsec_encap_pdb
 * @details   Salt and IV fields for IPsec CCM encapsulation
 */
struct ipsec_encap_ccm {
	uint32_t salt; /* lower 24 bits */
	uint8_t b0_flags;
	uint8_t ctr_flags;
	uint16_t ctr_initial;
	uint32_t iv[2];
};

/**
 * @struct    ipsec_encap_gcm pdb.h
 * @ingroup   ipsec_encap_pdb
 * @details   Salt and IV fields for IPsec GCM encapsulation
 */
struct ipsec_encap_gcm {
	uint32_t salt; /* lower 24 bits */
	uint32_t rsvd1;
	uint32_t iv[2];
};

/**
 * @struct    ipsec_encap_pdb pdb.h
 * @ingroup   ipsec_encap_pdb
 * @details   Container for encapsulation PDB
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
	uint32_t ip_hdr[0]; /* optional IP Header content */
};

/**
 * @struct    ipsec_decap_cbc pdb.h
 * @ingroup   ipsec_decap_pdb
 * @details   Placeholder for reserved words
 */
struct ipsec_decap_cbc {
	uint32_t rsvd[2];
};

/**
 * @struct    ipsec_decap_ctr pdb.h
 * @ingroup   ipsec_decap_pdb
 * @details   Salt and counter fields for IPsec CTR decapsulation
 */
struct ipsec_decap_ctr {
	uint32_t salt;
	uint32_t ctr_initial;
};

/**
 * @struct    ipsec_decap_ctr pdb.h
 * @ingroup   ipsec_decap_pdb
 * @details   Salt, counter and flag fields for IPsec CCM decapsulation
 */
struct ipsec_decap_ccm {
	uint32_t salt;
	uint8_t iv_flags;
	uint8_t ctr_flags;
	uint16_t ctr_initial;
};

/**
 * @struct    ipsec_decap_gcm pdb.h
 * @ingroup   ipsec_decap_pdb
 * @details   Salt field for IPsec GCM decapsulation
 */
struct ipsec_decap_gcm {
	uint32_t salt;
	uint32_t resvd;
};

/**
 * @struct    ipsec_decap_pdb pdb.h
 * @ingroup   ipsec_decap_pdb
 * @details   Container for decapsulation PDB
 */
struct ipsec_decap_pdb {
	uint16_t ip_hdr_len;		/* HMO (upper nibble) + IP header length
	 	 	 	 	 * (lower 3 nibbles) */
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
	uint32_t anti_replay[2];
	uint32_t end_index[0];
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

/*
 * IEEE 802.11 WiFi Protocol Data Block
 */
#define WIFI_PDBOPTS_FCS	0x01
#define WIFI_PDBOPTS_AR		0x40

/**
 * @def WIFI_B0_FLAGS
 * The value of B0 flags used for WiFi AES-CCM Context construction.
 *
 */
#define WIFI_B0_FLAGS			0x59

/**
 * @def WIFI_FRM_CTRL_MASK
 * The Frame Control Mask value used in computing the additional
 * authentication data (AAD).
 */
#define WIFI_FRM_CTRL_MASK		0x8fc7

/**
 * @def WIFI_SEQ_CTRL_MASK
 * The Sequence Control Mask value used in computing the additional
 * authentication data (AAD). *
 */
#define WIFI_SEQ_CTRL_MASK		0x0f00

/**
 * @def WIFI_CTR_FLAGS
 * The value of Counter Flags used for CCM Counter block construction.
 */
#define WIFI_CTR_FLAGS			0x01

struct wifi_encap_pdb {
	uint16_t mac_hdr_len;	/* MAC Header Length - 24 or 30 bytes */
	uint8_t rsvd;		/* Reserver bits */
	uint8_t options;	/* Options byte */
	uint8_t b0_flags;	/* CCM B0 block flags */
	uint8_t pri;		/* Frame Priority */
	uint16_t pn1;		/* Packet Number MS 16 bits */
	uint32_t pn2;		/* Packet Number LS 32 bits */
	uint16_t frm_ctrl_mask; /* Frame Control mask */
	uint16_t seq_ctrl_mask; /* Sequence Control mask */
	uint16_t rsvd1;	    /* Reserved bits */
	uint8_t cnst;		/* CCM Counter block constant - 0x0000 */
	uint8_t key_id;		/* Key ID*/
	uint8_t ctr_flags;	/* CCM Counter block flags */
	uint8_t rsvd2;		/* Reserved bits */
	uint16_t ctr_init;	/* CCM Counter block init counter */
};

struct wifi_decap_pdb {
	uint16_t mac_hdr_len;	/* MAC Header Length - 24 or 30 bytes */
	uint8_t rsvd;		/* Reserver bits */
	uint8_t options;	/* Options byte */
	uint8_t b0_flags;	/* CCM B0 block flags */
	uint8_t pri;		/* Frame Priority */
	uint16_t pn1;		/* Packet Number MS 16 bits */
	uint32_t pn2;		/* Packet Number LS 32 bits */
	uint16_t frm_ctrl_mask; /* Frame Control mask */
	uint16_t seq_ctrl_mask; /* Sequence Control mask */
	uint32_t rsvd1;	    /* Reserved bits */
	uint8_t ctr_flags;	/* CCM Counter block flags */
	uint8_t rsvd2;		/* Reserved bits */
	uint16_t ctr_init;	/* CCM Counter block init counter */
};

/*
 * PDCP Control Plane Protocol Data Blocks
 */
#define PDCP_C_PLANE_PDB_HFN_SHIFT		5
#define PDCP_C_PLANE_PDB_BEARER_SHIFT		27
#define PDCP_C_PLANE_PDB_DIR_SHIFT		26
#define PDCP_C_PLANE_PDB_HFN_THR_SHIFT		5

#define PDCP_U_PLANE_PDB_OPT_SHORT_SN		0x2
#define PDCP_U_PLANE_PDB_SHORT_SN_HFN_SHIFT	7
#define PDCP_U_PLANE_PDB_LONG_SN_HFN_SHIFT	12
#define PDCP_U_PLANE_PDB_15BIT_SN_HFN_SHIFT	15
#define PDCP_U_PLANE_PDB_BEARER_SHIFT		27
#define PDCP_U_PLANE_PDB_DIR_SHIFT		26
#define PDCP_U_PLANE_PDB_SHORT_SN_HFN_THR_SHIFT	7
#define PDCP_U_PLANE_PDB_LONG_SN_HFN_THR_SHIFT	12
#define PDCP_U_PLANE_PDB_15BIT_SN_HFN_THR_SHIFT	15

struct pdcp_pdb {
	union {
		uint32_t opt;
		uint32_t rsvd;
	} opt_res;
	uint32_t hfn_res;	/* HyperFrame number,(27, 25 or 21 bits),
				 * left aligned & right-padded with zeros. */
	uint32_t bearer_dir_res;/* Bearer(5 bits), packet direction (1 bit),
				 * left aligned & right-padded with zeros. */
	uint32_t hfn_thr_res;	/* HyperFrame number threshold (27, 25 or 21
				 * bits), left aligned & right-padded with
				 * zeros. */
};

/*
 * PDCP internal PDB types
 */
enum pdb_type_e {
	PDCP_PDB_TYPE_NO_PDB,
	PDCP_PDB_TYPE_FULL_PDB,
	PDCP_PDB_TYPE_REDUCED_PDB,
	PDCP_PDB_TYPE_INVALID
};

/*
 * IEEE 802.16 WiMAX Protocol Data Block
 */
/** @addtogroup defines_group
 *  @{
 */
#define WIMAX_PDBOPTS_FCS       0x01    /**< Options Byte with FCS enabled */
#define WIMAX_PDBOPTS_AR        0x40    /**< Options Byte with AR enabled */
/** @}*/
#define WIMAX_PDB_B0            0x19    /* Initial Block B0 Flags */
#define WIMAX_PDB_CTR           0x01    /* Counter Block Flags */

struct wimax_encap_pdb {
	uint8_t rsvd[3];                /* Reserved Bits */
	uint8_t options;                /* Options Byte */
	uint32_t nonce;                 /* Nonce Constant */
	uint8_t b0_flags;               /* Initial Block B0 */
	uint8_t ctr_flags;              /* Counter Block Flags */
	uint16_t ctr_init_count;
	/* begin DECO writeback region */
	uint32_t pn;                    /* Packet Number */
	/* end DECO writeback region */
};

struct wimax_decap_pdb {
	uint8_t rsvd[3];                /* Reserved Bits */
	uint8_t options;                /* Options Byte */
	uint32_t nonce;                 /* Nonce Constant */
	uint8_t iv_flags;               /* Initialization Vector Flags */
	uint8_t ctr_flags;              /* Counter Block Flags */
	uint16_t ctr_init_count;
	/* begin DECO writeback region */
	uint32_t pn;                    /* Packet Number */
	uint8_t rsvd1[2];               /* Reserved Bits */
	uint16_t antireplay_len;
	uint32_t antireplay_scorecard_hi;
	uint32_t antireplay_scorecard_lo;
	/** end DECO writeback region */
};

/*
 * IEEE 801.AE MacSEC Protocol Data Block
 */
#define MACSEC_PDBOPTS_FCS	0x01
#define MACSEC_PDBOPTS_AR	0x40	/* used in decap only */

struct macsec_encap_pdb {
	uint16_t aad_len;
	uint8_t rsvd;
	uint8_t options;
	uint32_t sci_hi;
	uint32_t sci_lo;
	uint16_t ethertype;
	uint8_t tci_an;
	uint8_t rsvd1;
	/* begin DECO writeback region */
	uint32_t pn;
	/* end DECO writeback region */
};

struct macsec_decap_pdb {
	uint16_t aad_len;
	uint8_t rsvd;
	uint8_t options;
	uint32_t sci_hi;
	uint32_t sci_lo;
	uint8_t rsvd1[3];
	/* begin DECO writeback region */
	uint8_t antireplay_len;
	uint32_t pn;
	uint32_t antireplay_scorecard_hi;
	uint32_t antireplay_scorecard_lo;
	/* end DECO writeback region */
};

/**
 * @defgroup tls_pdb tls_pdb
 * @ingroup pdb_group
 * @{
 */
/** @} end of tls_pdb */

/*
 * TLS family encapsulation/decapsulation PDB definitions.
 */

#define DTLS_PDBOPTS_ARS32	0x40	/* DTLS only */
#define DTLS_PDBOPTS_ARS64	0xc0	/* DTLS only */
#define TLS_PDBOPTS_OUTFMT	0x08
#define TLS_PDBOPTS_IV_WRTBK	0x02	/* TLS1.1/TLS1.2/DTLS only */
#define TLS_PDBOPTS_EXP_RND_IV	0x01	/* TLS1.1/TLS1.2/DTLS only */
#define TLS_PDBOPTS_TR_ICV	0x10	/* Available starting with SEC ERA 5 */

/**
 * @struct   tls_block_enc pdb.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options and Sequence Number fields for
 *           SSL3.0/TLS1.0/TLS1.1/TLS1.2 block encapsulation PDB.
 */
struct tls_block_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   dtls_block_enc pdb.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options, Epoch and Sequence Number fields for
 *           DTLS1.0 block encapsulation PDB.
 */
struct dtls_block_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_block_dec pdb.h
 * @ingroup  tls_pdb
 * @details  Options and Sequence Number fields for SSL3.0/TLS1.0/TLS1.1/TLS1.2
 *           block decapsulation PDB.
 */
struct tls_block_dec {
	uint8_t rsvd[3];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   dtls_block_enc pdb.h
 * @ingroup  tls_pdb
 * @details  Options, Epoch and Sequence Number fields for DTLS1.0 block
 *           decapsulation PDB.
 */
struct dtls_block_dec {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_block_pdb pdb.h
 * @ingroup  tls_pdb
 * @details  SSL3.0/TLS1.0/TLS1.1/TLS1.2/DTLS1.0
 *           block encapsulation/decapsulation PDB.
 *           The zero-length array expands with one/two words for the
 *           Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set
 *           in the DTLS1.0 decapsulation PDB Options byte.
 *           If SEC ERA is equal or greater than SEC ERA 5 and
 *           TLS_PDBOPTS_TR_ICV is set in the PDB Options Byte, the last member
 *           of the tls_block_pdb structure expands for ICVLen.
 */
struct tls_block_pdb {
	union {
		struct tls_block_enc tls_enc;
		struct dtls_block_enc dtls_enc;
		struct tls_block_dec tls_dec;
		struct dtls_block_dec dtls_dec;
	};
	uint32_t iv[4];
	uint32_t end_index[0];
};

/**
 * @struct   tls_stream_enc pdb.h
 * @ingroup  tls_pdb
 * @details  Version and Options fields for SSL3.0/TLS1.0/TLS1.1/TLS1.2
 *           stream encapsulation PDB.
 */
struct tls_stream_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
};

/**
 * @struct   tls_stream_enc pdb.h
 * @ingroup  tls_pdb
 * @details  Options field for SSL3.0/TLS1.0/TLS1.1/TLS1.2
 *           stream decapsulation PDB.
 */
struct tls_stream_dec {
	uint8_t rsvd[3];
	uint8_t options;
};

/**
 * @struct   tls_stream_pdb pdb.h
 * @ingroup  tls_pdb
 * @details  SSL3.0/TLS1.0/TLS1.1/TLS1.2
 *           stream encapsulation/decapsulation PDB. The zero-length array
 *           expands for ICVLen if SEC ERA is equal or greater than SEC ERA 5
 *           and TLS_PDBOPTS_TR_ICV is set in the PDB Options Byte.
 */
struct tls_stream_pdb {
	union {
		struct tls_stream_enc enc;
		struct tls_stream_dec dec;
	};
	uint32_t seq_num[2];
	uint32_t end_index[0];
};

/**
 * @struct   tls_ctr_enc pdb.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options and Sequence Number fields for
 *           TLS1.1/TLS1.2 AES CTR encapsulation PDB.
 */
struct tls_ctr_enc {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   tls_ctr pdb.h
 * @ingroup  tls_pdb
 * @details  Options, Epoch and Sequence Number fields for
 *           TLS1.1/TLS1.2 AES CTR decapsulation PDB and
 *           DTLS1.0 AES CTR encapsulation/decapsulation PDB.
 */
struct tls_ctr {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_ctr_pdb pdb.h
 * @ingroup  tls_pdb
 * @details  TLS1.1/TLS1.2/DTLS1.0 AES CTR encapsulation/decapsulation PDB.
 *           TLS1.1/TLS1.2/DTLS1.0 AES CTR encryption processing is supported
 *           starting with SEC ERA 5.
 *           The zero-length array expands with one/two words for the
 *           Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set
 *           in the DTLS1.0 decapsulation PDB Options Byte.
 *           If TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte,
 *           the last member of the tls_block_pdb structure expands for ICVLen.
 */
struct tls_ctr_pdb {
	union {
		struct tls_ctr_enc tls_enc;
		struct tls_ctr ctr;
	};
	uint16_t write_iv[3];
	uint16_t constant;
	uint32_t end_index[0];
};

/**
 * @struct   tls12_gcm_encap pdb.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options and Sequence Number fields for
 *           TLS1.2 AES GCM encapsulation PDB.
 */
struct tls12_gcm_encap {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   tls12_gcm_decap pdb.h
 * @ingroup  tls_pdb
 * @details  Options and Sequence Number fields for TLS1.2 AES GCM
 *           encapsulation PDB.
 */
struct tls12_gcm_decap {
	uint8_t rsvd[3];
	uint8_t options;
	uint32_t seq_num[2];
};
/**
 * @struct   dtls_gcm pdb.h
 * @ingroup  tls_pdb
 * @details  Options and Epoch fields for DTLS1.0 AES GCM
 *           encapsulation/decapsulation PDB.
 */
struct dtls_gcm {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_gcm_pdb pdb.h
 * @ingroup  tls_pdb
 * @details  TLS1.2/DTLS1.0 AES GCM encapsulation/decapsulation PDB
 *           The zero-length array expands with one/two words for the
 *           Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set
 *           in the DTLS1.0 decapsulation PDB Options byte.
 *           If SEC ERA is equal or greater than SEC ERA 5 and
 *           TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte,
 *           the last member of the tls_block_pdb structure expands for ICVLen.
 */
struct tls_gcm_pdb {
	union {
		struct tls12_gcm_encap tls12_enc;
		struct tls12_gcm_decap tls12_dec;
		struct dtls_gcm dtls;
	};
	uint32_t salt;
	uint32_t end_index[0];
};

/**
 * @struct   tls12_ccm_encap pdb.h
 * @ingroup  tls_pdb
 * @details  Type, Version, Options and Sequence Number fields for
 *           TLS1.2 AES CCM encapsulation PDB.
 */
struct tls12_ccm_encap {
	uint8_t type;
	uint8_t version[2];
	uint8_t options;
	uint32_t seq_num[2];
};

/**
 * @struct   tls_ccm pdb.h
 * @ingroup  tls_pdb
 * @details  Options, Epoch and Sequence Number fields for
 *           TLS12 AES CCM decapsulation PDB and
 *           DTLS1.0 AES CCM encapsulation/decapsulation PDB.
 */
struct tls_ccm {
	uint8_t rsvd[3];
	uint8_t options;
	uint16_t epoch;
	uint16_t seq_num[3];
};

/**
 * @struct   tls_ccm_pdb pdb.h
 * @ingroup  tls_pdb
 * @details  TLS1.2/DTLS1.0 AES CCM encapsulation/decapsulation PDB.
 *           The zero-length array expands with one/two words for the
 *           Anti-Replay Scorecard if DTLS_PDBOPTS_ARS32/64 is set
 *           in the DTLS1.0 decapsulation PDB Options byte.
 *           If SEC ERA is equal or greater than SEC ERA 5 and
 *           TLS_PDBOPTS_TR_ICV is set in the PDB Option Byte,
 *           the last member of the tls_block_pdb structure expands for ICVLen.
 */
struct tls_ccm_pdb {
	union {
		struct tls12_ccm_encap tls12;
		struct tls_ccm ccm;
	};
	uint32_t write_iv;
	uint8_t b0_flags;
	uint8_t ctr0_flags;
	uint8_t rsvd[3];
	uint8_t ctr0[3];
	uint32_t end_index[0];
};

/*
 * SRTP Protocol Data Blocks
 */
#define SRTP_PDBOPTS_MKI	0x08
#define SRTP_PDBOPTS_AR		0x40
#define SRTP_CIPHER_SALT_LEN	14

struct srtp_encap_pdb {
	uint8_t x_len;			   /* RTP Extension length */
	uint8_t mki_len;		   /* MKI length */
	uint8_t n_tag;			   /* ICV length */
	uint8_t options;		   /* Options Byte */
	uint32_t cnst0;			   /* Constant Bits */
	uint8_t rsvd[2];		   /* Reserved Bits */
	uint16_t cnst1;			   /* Constant Bits */
	uint8_t salt[SRTP_CIPHER_SALT_LEN];/* Cipher Salt */
	uint16_t cnst2;			   /* Constant Bits */
	uint32_t rsvd1;			   /* Reserved Bits */
	uint32_t roc;			   /* Rollover Counter */
	uint32_t opt_mki;		   /* MKI */
};

struct srtp_decap_pdb {
	uint8_t x_len;			   /* RTP Extension length */
	uint8_t mki_len;		   /* MKI length */
	uint8_t n_tag;			   /* ICV length */
	uint8_t options;		   /* Options Byte */
	uint32_t cnst0;			   /* Constant Bits */
	uint8_t rsvd[2];		   /* Reserved Bits */
	uint16_t cnst1;			   /* Constant Bits */
	uint8_t salt[SRTP_CIPHER_SALT_LEN];/* Cipher Salt */
	uint16_t cnst2;			   /* Constant Bits */
	uint16_t rsvd1;			   /* Reserved Bits */
	uint16_t seq_num;		   /* Sequence Number */
	uint32_t roc;			   /* Rollover Counter */
	uint64_t antireplay_scorecard;	   /* Anti-replay Scorecard */
};

/**
 * @defgroup rsa_pdb rsa_pdb
 * @ingroup pdb_group
 * @{
 */
/** @} end of rsa_pdb */

/*
 * RSA encryption/decryption PDB definitions.
 */

#define RSA_ENC_SGF_SHIFT	28
#define RSA_ENC_RSV_SHIFT	24
#define RSA_ENC_E_LEN_SHIFT	12

/**
 * @struct   rsa_encrypt_pdb_64b pdb.h
 * @ingroup  rsa_pdb
 * @details  RSA encryption PDB for 64 bits addresses
 */
struct rsa_encrypt_pdb_64b {
	uint32_t header;	/* Contains sgf, rsv, #e, #n fields */
	uint32_t f_ref_high;	/* Reference to input */
	uint32_t f_ref_low;
	uint32_t g_ref_high;	/* Reference to output */
	uint32_t g_ref_low;
	uint32_t n_ref_high;	/* Reference to modulus */
	uint32_t n_ref_low;
	uint32_t e_ref_high;	/* Reference to public key */
	uint32_t e_ref_low;
	uint32_t f_len;		/* Input length */
};

/**
 * @struct   rsa_encrypt_pdb pdb.h
 * @ingroup  rsa_pdb
 * @details  RSA encryption PDB for 32 bits addresses
 */
struct rsa_encrypt_pdb {
	uint32_t header;	/* Contains sgf, rsv, #e, #n fields */
	uint32_t f_ref;		/* Reference to input */
	uint32_t g_ref;		/* Reference to output */
	uint32_t n_ref;		/* Reference to modulus */
	uint32_t e_ref;		/* Reference to public key */
	uint32_t f_len;		/* Input length */
};

#define RSA_DEC1_SGF_SHIFT	28
#define RSA_DEC1_RSV_SHIFT	24
#define RSA_DEC1_D_LEN_SHIFT	12

/**
 * @struct   rsa_dec_pdb_form1_64b pdb.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form1 PDB for 64 bits addresses
 */
struct rsa_dec_pdb_form1_64b {
	uint32_t header;	/* Contains sgf, rsv, #d, #n fields */
	uint32_t g_ref_high;	/* Reference to input */
	uint32_t g_ref_low;
	uint32_t f_ref_high;	/* Reference to output */
	uint32_t f_ref_low;
	uint32_t n_ref_high;	/* Reference to modulus */
	uint32_t n_ref_low;
	uint32_t d_ref_high;	/* Reference to private key */
	uint32_t d_ref_low;
};

/**
 * @struct   rsa_dec_pdb_form1 pdb.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form1 PDB for 32 bits addresses
 */
struct rsa_dec_pdb_form1 {
	uint32_t header;	/* Contains sgf, rsv, #d, #n fields */
	uint32_t g_ref;		/* Reference to input */
	uint32_t f_ref;		/* Reference to output */
	uint32_t n_ref;		/* Reference to modulus */
	uint32_t d_ref;		/* Reference to private key */
};

#define RSA_DEC2_SGF_SHIFT	25
#define RSA_DEC2_D_LEN_SHIFT	12
#define RSA_DEC2_Q_LEN_SHIFT	12

/**
 * @struct   rsa_dec_pdb_form2_64b pdb.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form2 PDB for 64 bits addresses
 */
struct rsa_dec_pdb_form2_64b {
	uint32_t header;	/* Contains sgf, rsv, #d, #n fields */
	uint32_t g_ref_high;	/* Reference to input */
	uint32_t g_ref_low;
	uint32_t f_ref_high;	/* Reference to output */
	uint32_t f_ref_low;
	uint32_t d_ref_high;	/* Reference to private key */
	uint32_t d_ref_low;
	uint32_t p_ref_high;	/* Reference to prime p */
	uint32_t p_ref_low;
	uint32_t q_ref_high;	/* Reference to prime q */
	uint32_t q_ref_low;
	uint32_t tmp1_ref_high;	/* Reference to tmp1 */
	uint32_t tmp1_ref_low;
	uint32_t tmp2_ref_high;	/* Reference to tmp2 */
	uint32_t tmp2_ref_low;
	uint32_t trailer;	/* Contains rsv, #q, #p fields */
};

/**
 * @struct   rsa_dec_pdb_form2 pdb.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form2 PDB for 32 bits addresses
 */
struct rsa_dec_pdb_form2 {
	uint32_t header;	/* Contains sgf, rsv, #d, #n fields */
	uint32_t g_ref;		/* Reference to input */
	uint32_t f_ref;		/* Reference to output */
	uint32_t d_ref;		/* Reference to private key */
	uint32_t p_ref;		/* Reference to prime p */
	uint32_t q_ref;		/* Reference to prime q */
	uint32_t tmp1_ref;	/* Reference to tmp1 */
	uint32_t tmp2_ref;	/* Reference to tmp2 */
	uint32_t trailer;	/* Contains rsv, #q, #p fields */
};

#define RSA_DEC3_SGF_SHIFT	25
#define RSA_DEC3_Q_LEN_SHIFT	12

/**
 * @struct   rsa_dec_pdb_form3_64b pdb.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form3 PDB for 64 bits addresses
 */
struct rsa_dec_pdb_form3_64b {
	uint32_t header;	/* Contains sgf, rsv, #n fields */
	uint32_t g_ref_high;	/* Reference to input */
	uint32_t g_ref_low;
	uint32_t f_ref_high;	/* Reference to output */
	uint32_t f_ref_low;
	uint32_t c_ref_high;	/* Reference to c */
	uint32_t c_ref_low;
	uint32_t p_ref_high;	/* Reference to prime p */
	uint32_t p_ref_low;
	uint32_t q_ref_high;	/* Reference to prime q */
	uint32_t q_ref_low;
	uint32_t dp_ref_high;	/* Reference to dp */
	uint32_t dp_ref_low;
	uint32_t dq_ref_high;	/* Reference to dq */
	uint32_t dq_ref_low;
	uint32_t tmp1_ref_high;	/* Reference to tmp1 */
	uint32_t tmp1_ref_low;
	uint32_t tmp2_ref_high;	/* Reference to tmp2 */
	uint32_t tmp2_ref_low;
	uint32_t trailer;	/* Contains rsv, #q, #p fields */
};

/**
 * @struct   rsa_dec_pdb_form3 pdb.h
 * @ingroup  rsa_pdb
 * @details  RSA decryption form3 PDB for 32 bits addresses
 */
struct rsa_dec_pdb_form3 {
	uint32_t header;	/* Contains sgf, rsv, #n fields */
	uint32_t g_ref;		/* Reference to input */
	uint32_t f_ref;		/* Reference to output */
	uint32_t c_ref;		/* Reference to c */
	uint32_t p_ref;		/* Reference to prime p */
	uint32_t q_ref;		/* Reference to prime q */
	uint32_t dp_ref;	/* Reference to dp */
	uint32_t dq_ref;	/* Reference to dq */
	uint32_t tmp1_ref;	/* Reference to tmp1 */
	uint32_t tmp2_ref;	/* Reference to tmp2 */
	uint32_t trailer;	/* Contains rsv, #q, #p fields */
};

#endif /* __RTA_PDB_H__ */
