/**************************************************************************//**
@File		fsl_ipsec.h

@Description	This file contains the AIOP IPSec API.
*//***************************************************************************/

#ifndef __FSL_IPSEC_H
#define __FSL_IPSEC_H

#include "common/types.h"


/**************************************************************************//**
@Group	FSL_IPSEC FSL_AIOP_IPSEC

@Description	Freescale AIOP IPsec API

@{
*//***************************************************************************/

/**************************************************************************//**
 @Group		IPSEC_ENUM IPsec Enumerations

 @Description	IPsec Enumerations

 @{
*//***************************************************************************/

/**************************************************************************//**
 @enum ipsec_cipher_alg
 
 @Description   IPSEC Cipher-Algorithm Options
 
*//***************************************************************************/

enum ipsec_cipher_alg {
	/** DES_CBC_HMAC_96_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_DES_CBC_HMAC_96_MD5_128,
	/** DES_CBC_HMAC_96_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_DES_CBC_HMAC_96_SHA_160,
	/** DES_CBC_HMAC_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_DES_CBC_HMAC_MD5_128,
	/** DES_CBC_HMAC_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_DES_CBC_HMAC_SHA_160,
	/** DES_CBC_HMAC_SHA_256_128 ciphersuite */
	IPSEC_CIPHER_ALG_DES_CBC_HMAC_SHA_256_128,
	/** DES_CBC_HMAC_SHA_384_192 ciphersuite */
	IPSEC_CIPHER_ALG_DES_CBC_HMAC_SHA_384_192,
	/** DES_CBC_HMAC_SHA_512_256 ciphersuite */
	IPSEC_CIPHER_ALG_DES_CBC_HMAC_SHA_512_256,
	/** 3DES_CBC_HMAC_96_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_3DES_CBC_HMAC_96_MD5_128,
	/** 3DES_CBC_HMAC_96_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_3DES_CBC_HMAC_96_SHA_160,
	/** 3DES_CBC_HMAC_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_3DES_CBC_HMAC_MD5_128,
	/** 3DES_CBC_HMAC_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_3DES_CBC_HMAC_SHA_160,
	/** 3DES_CBC_HMAC_SHA_256_128 ciphersuite */
	IPSEC_CIPHER_ALG_3DES_CBC_HMAC_SHA_256_128,
	/** 3DES_CBC_HMAC_SHA_384_192 ciphersuite */
	IPSEC_CIPHER_ALG_3DES_CBC_HMAC_SHA_384_192,
	/** 3DES_CBC_HMAC_SHA_512_256 ciphersuite */
	IPSEC_CIPHER_ALG_3DES_CBC_HMAC_SHA_512_256,
	/** NULL_ENC_HMAC_96_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_HMAC_96_MD5_128,
	/** NULL_ENC_HMAC_96_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_HMAC_96_SHA_160,
	/** NULL_ENC_AES_XCBC_MAC_96 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_AES_XCBC_MAC_96,
	/** NULL_ENC_HMAC_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_HMAC_MD5_128,
	/** NULL_ENC_HMAC_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_HMAC_SHA_160,
	/** NULL_ENC_AES_CMAC_96 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_AES_CMAC_96,
	/** NULL_ENC_HMAC_SHA_256_128 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_HMAC_SHA_256_128,
	/** NULL_ENC_HMAC_SHA_384_192 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_HMAC_SHA_384_192,
	/** NULL_ENC_HMAC_SHA_512_256 ciphersuite */
	IPSEC_CIPHER_ALG_NULL_ENC_HMAC_SHA_512_256,
	/** AES_CBC_HMAC_96_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_HMAC_96_MD5_128,
	/** AES_CBC_HMAC_96_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_HMAC_96_SHA_160,
	/** AES_CBC_AES_XCBC_MAC_96 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_AES_XCBC_MAC_96,
	/** AES_CBC_HMAC_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_HMAC_MD5_128,
	/** AES_CBC_HMAC_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_HMAC_SHA_160,
	/** AES_CBC_AES_CMAC_96 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_AES_CMAC_96,
	/** AES_CBC_HMAC_SHA_256_128 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_HMAC_SHA_256_128,
	/** AES_CBC_HMAC_SHA_384_192 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_HMAC_SHA_384_192,
	/** AES_CBC_HMAC_SHA_512_256 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CBC_HMAC_SHA_512_256,
	/** AES_CTR_HMAC_96_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_HMAC_96_MD5_128,
	/** AES_CTR_HMAC_96_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_HMAC_96_SHA_160,
	/** AES_CTR_AES_XCBC_MAC_96 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_AES_XCBC_MAC_96,
	/** AES_CTR_HMAC_MD5_128 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_HMAC_MD5_128,
	/** AES_CTR_HMAC_SHA_160 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_HMAC_SHA_160,
	/** AES_CTR_AES_CMAC_96 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_AES_CMAC_96,
	/** AES_CTR_HMAC_SHA_256_128 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_HMAC_SHA_256_128,
	/** AES_CTR_HMAC_SHA_384_192 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_HMAC_SHA_384_192,
	/** AES_CTR_HMAC_SHA_512_256 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CTR_HMAC_SHA_512_256,
	/** AES_CCM_8 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CCM_8,
	/** AES_CCM_12 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CCM_12,
	/** AES_CCM_16 ciphersuite */
	IPSEC_CIPHER_ALG_AES_CCM_16,
	/** AES_GCM_8 ciphersuite */
	IPSEC_CIPHER_ALG_AES_GCM_8,
	/** AES_GCM_12 ciphersuite */
	IPSEC_CIPHER_ALG_AES_GCM_12,
	/** AES_GCM_16 ciphersuite */
	IPSEC_CIPHER_ALG_AES_GCM_16,
	/** AES_NULL_WITH_GMAC ciphersuite */
	IPSEC_CIPHER_ALG_AES_NULL_WITH_GMAC
};

/**************************************************************************//**
 @enum ipsec_ars
 
 @Description   IPSEC Anti-Replay-Size Options
 
*//***************************************************************************/
enum ipsec_ars {
	/** No anti-replay window */
	IPSEC_ARS_0 = 0,
	/** 32-entry anti-replay window */
	IPSEC_ARS_32,
	/** 64-entry anti-replay window */
	IPSEC_ARS_64,
	/** 128-entry anti-replay window, valid only for tunnel
	new thread operational mode and not for legacy modes */
	IPSEC_ARS_128
};

/**************************************************************************//**
 @enum key_src
 
 @Description   Key-Source Options
 
*//***************************************************************************/
enum key_src {
	/** Key is inside the descriptor */
	KEY_SRC_INTERNAL,
	/** Key is outside the descriptor */
	KEY_SRC_EXTERNAL
};

/**************************************************************************//**
 @enum key_type
 
 @Description   Key-Type Options
 
*//***************************************************************************/
enum key_type {
	/** Key is not encrypted */
	KEY_TYPE_NON_ENC,
	/** Key is ECB-encrypted */
	KEY_TYPE_ECB_ENC,
	/** Key is CCM-encrypted */
	KEY_TYPE_CCM_ENC
};

/**************************************************************************//**
 @enum key_dest

 @Description   Key-Destination Options
 TODO: maybe we can omit it if split is supported by HW
       removed e_KEY_DEST_PKHA_E_MEM, e_KEY_DEST_AFHA_SBOX

*//***************************************************************************/

enum key_dest {
	/** Key Register */
	KEY_DEST_REGISTER,
	/** MDHA Split Key */
	KEY_DEST_MDHA_SPLIT
};

/**************************************************************************//**
 @enum ipsec_ver

 @Description   IPSEC Version Options
 
*//***************************************************************************/
enum ipsec_ver {
	/** IPv4, valid only for transport mode */
	IPSEC_VER_4 = 4,
	/** IPv6, valid only for transport mode */
	IPSEC_VER_6 = 6
};

/**************************************************************************//**
 @enum ipsec_hdr

 @Description   IPSEC Header Options
 
*//***************************************************************************/
enum ipsec_hdr {
	/** IP header not include */
	IPSEC_HDR_NONE = 0,
	/** IP header included in each frame */
	IPSEC_HDR_IN_FRAME,
	/** IP header included in parameters */
	IPSEC_HDR_IN_PARAM,
	/** IP header address included in parameters, valid only for tunnel
	new thread operational mode and not for legacy modes */
	IPSEC_HDR_ADDRESS_IN_PARAM
};

/* @} end of IPSEC_ENUM */

/**************************************************************************//**
@Group	FSL_IPSEC_MACROS IPsec Macros

@Description	Freescale IPsec Macros

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPSec handle Type definition

*//***************************************************************************/
typedef uint64_t ipsec_handle_t;

/**************************************************************************//**
@Description	ipsec encryption parameters flags

*//***************************************************************************/

/** IPsec mode: transport or new thread tunnel operational mode */
#define IPSEC_ENC_MODE                                  0x80000000
/** NAT Support. Tunnel new thread operational mode only */
#define IPSEC_ENC_NAT_SUPPORT                           0x40000000
/** NAT UDP checksum enable. Tunnel new thread operational mode only */
#define IPSEC_ENC_NAT_UDP_CHKSUM_EN                     0x20000000
/** Lifetime KB Limit Enable */
#define IPSEC_ENC_LIFETIME_KB_LIMIT_EN                  0x10000000
/** Lifetime SEC Limit Enable */
#define IPSEC_ENC_LIFETIME_SEC_LIMIT_EN                 0x08000000
/** Copy DF bit (IPv4 Tunnel Mode only) from inner IP header to outer
IP header */
#define IPSEC_ENC_COPY_DF                               0x04000000
/** Decrement TTL field (IPv4) or Hop-Limit field (IPv6) within inner
IP header */
#define IPSEC_ENC_DTTL                                  0x02000000
/** Perform computations to update header checksum for
IPv4 header. Not valid for tunnel new thread operational mode */
#define IPSEC_ENC_EN_CHKSUM                             0x01000000
/** Copy TOS field (IPv4) or Traffic-Class field (IPv6) from outer
IP header to inner IP header. Not valid for tunnel new thread
operational mode */
#define IPSEC_ENC_COPY_TOS                              0x00800000
/** Generate random initial vector before starting encapsulation */
#define IPSEC_ENC_RAND_IV                               0x00400000
/** Use a 64-bit sequence number instead of a 32-bit sequence number */
#define IPSEC_ENC_EXT_SEQ_NUM                           0x00200000
/** Propagate ASA */
#define IPSEC_ENC_COPY_ASA                              0x00020000

/**************************************************************************//**
@Description	ipsec decryption parameters flags
TODO: removed outFmt and removeTrailer.

*//***************************************************************************/

/** IPsec mode: transport or new thread tunnel operational mode */
#define IPSEC_DEC_MODE                                  0x80000000
/** Lifetime KB Limit Enable */
#define IPSEC_DEC_LIFETIME_KB_LIMIT_EN                  0x10000000
/** Lifetime SEC Limit Enable */
#define IPSEC_DEC_LIFETIME_SEC_LIMIT_EN                 0x08000000
/** Decrement TTL field (IPv4) or Hop-Limit field (IPv6) within inner
IP header */
#define IPSEC_DEC_DTTL                                  0x02000000
/** Perform computations to update header checksum for
IPv4 header. Not valid for tunnel new thread operational mode */
#define IPSEC_DEC_EN_CHKSUM                             0x01000000
/** Copy TOS field (IPv4) or Traffic-Class field (IPv6) from outer
IP header to inner IP header */
#define IPSEC_DEC_COPY_TOS                              0x00800000
/** Use a 64-bit sequence number instead of a 32-bit sequence number */
#define IPSEC_ENC_EXT_SEQ_NUM                           0x00200000
/** Enable Tunnel ECN according to RFC 6040 */
#define IPSEC_DEC_TECN                                  0x00100000
/** Enable Transport mode pad check */
#define IPSEC_DEC_PAD_CHECK                             0x00080000
/** EtherType field update */
#define IPSEC_DEC_ETU                                   0x00040000
/** Propagate ASA */
#define IPSEC_DEC_COPY_ASA                              0x00020000

/** @} */ /* end of FSL_IPSEC_MACROS */

/**************************************************************************//**
@Group		FSL_IPSEC_STRUCTS IPsec Structures

@Description	Freescale IPsec Structures

@{
*//***************************************************************************/

/**************************************************************************//**
@Description	IPsec Key Parameters 
*//***************************************************************************/
struct key_param {
	/** Key source */
	enum key_src  src;
	/** Key type */
	enum key_type type;
	/** Key destination */
	enum key_dest dest;
	uint8_t pad1;
	/** Key size in bytes */
	uint32_t size;
	/** Key data pointer */
	uint8_t *data_ptr;
};

/**************************************************************************//**
 @Description   IPsec Shared-Descriptor Parameters
*//***************************************************************************/
struct ipsec_shrdesc_param {
	/** Cryptographic key parameters */
	struct key_param crypto_key;
	/** Authentication key parameters */
	struct key_param authen_key;
	/** Cipher algorithm */
	enum ipsec_cipher_alg cipher_alg;
	uint8_t pad[3];
};

/**************************************************************************//**
 @Description   IPSEC Initial-Vector Attributes
*//***************************************************************************/
union ipsec_iv_attr {
	struct {
		/** NONCE value */
		uint32_t nonce;
		/** Initial count */
		uint32_t count;
	} ctr;
	struct {
		/** SALT value */
		uint32_t salt;
	} ccm;
	struct {
		/** SALT value */
		uint32_t salt;
	} gcm;
};

/**************************************************************************//**
 @Description   IPSEC-Flow Context Parameters
*//***************************************************************************/
struct ipsec_flow_context_param {
	/** Security domain ID */
	uint16_t sdid;
	/** Storage profile ID of SEC output frame */
	uint16_t spid;
	/** Return input frame on error */
	uint8_t rife;
	/** Critical resource ID */
	uint8_t crid;
	uint16_t pad;
};

/**************************************************************************//**
 @Description   IPSEC-Encapsulation Parameters
*//***************************************************************************/
struct ipsec_encap_param {
	/** flags */
	uint32_t flags;
	/** Next header value used for transport mode */
	uint8_t next_hdr;
	/** Next header offset used for transport mode */
	uint8_t nh_offset;
	/** IP header length */
	uint16_t iphdr_len;
	/** Initial sequence number */
	uint64_t seq_num;
	/** Initial vector attributes (relevant only for CTR/CCM/GCM) */
	union ipsec_iv_attr iv_attr;
	/** Initial vector pointer (16 bytes for CBC, 8 bytes for CTR/CCM/GCM,
	not in use for NULL_ENC) */
	uint8_t *p_iv;
	/** Security parameter index */
	uint32_t spi;
	/** IP header pointer */
	uint8_t *ip_hdr;
	/** IP version */
	enum ipsec_ver ipsec_ver;
	/** IP header source */
	enum ipsec_hdr ipsec_hdr;
	uint16_t pad;
	/** Soft Kilobytes expiry */
	uint32_t soft_kilobytes_limit;
	/** Hard Kilobytes expiry */
	uint32_t hard_kilobytes_limit;
	/** Soft Seconds expiry */
	uint32_t soft_seconds_limit;
	/** Hard Second expiry */
	uint32_t hard_seconds_limit;
	/** IPsec flow context */
	struct ipsec_flow_context_param ipsec_flow_context_param;
};

/**************************************************************************//**
 @Description   IPSEC-Decapsulation Parameters
*//***************************************************************************/
struct ipsec_decap_param {
	/** flags */
	uint32_t flags;
	/** IP header length */
	uint16_t iphdr_len;
	/** Next header offset used for transport mode */
	uint8_t nh_offset;
	uint8_t pad1;
	/** Initial sequence number */
	uint64_t seq_num;
	/** Initial vector attributes (relevant only for CTR/CCM/GCM) */
	union ipsec_iv_attr iv_attr;
	/** Anti-replay window size */
	enum ipsec_ars ipsec_ars;
	/** IP version */
	enum ipsec_ver ipsec_ver;
	uint16_t pad2;
	/** Soft Kilobytes expiry */
	uint32_t soft_kilobytes_limit;
	/** Hard Kilobytes expiry */
	uint32_t hard_kilobytes_limit;
	/** Soft Seconds expiry */
	uint32_t soft_seconds_limit;
	/** Hard Second expiry */
	uint32_t hard_seconds_limit;
	/** IPsec flow context */
	struct ipsec_flow_context_param ipsec_flow_context_param;
	uint32_t pad3;
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
		2. Optionally maximum number of buffers needed for ASA copying.

		TODO need to finalize the buffer mechanism.
		Implicitly: 
		Updated the BPID of SA in the SRAM.
		Optionally updated the BPID of ASA copying in the SRAM.
		
@Param[in]	Max. number of SA.

@Param[in]	Flag indicating if SA copying is needed.

@Return		TODO

*//****************************************************************************/
int32_t ipsec_init(uint16_t max_sa_no, uint8_t asa_copy);

/**************************************************************************//**
@Function	ipsec_add_sa_encap

@Description	This function performs add SA for encapsulation:
		creating the IPsec flow context and the Shared Descriptor.

		Implicit Input: BPID in the SRAM.
@Param[in]	

@Param[out]	IPsec handler which is the pointer to the buffer 
		allocated for SA handling.

@Return		TODO error status for CDMA buffer allocation failure.

*//****************************************************************************/
int32_t ipsec_add_sa_encap(
		struct ipsec_encap_param *ipsec_encap_param,
		struct ipsec_shrdesc_param *ipsec_shrdesc_param,
		ipsec_handle_t *ipsec_handle);

/**************************************************************************//**
@Function	ipsec_add_sa_decap

@Description	This function performs add SA for decapsulation:
		creating the IPsec flow context and the Shared Descriptor.

		Implicit Input: BPID in the SRAM.
@Param[in]	

@Param[out]	IPsec handler which is the pointer to the buffer 
		allocated for SA handling.

@Return		TODO error status for CDMA buffer allocation failure.

*//****************************************************************************/
int32_t ipsec_add_sa_decap(
		struct ipsec_decap_param *ipsec_decap_param,
		struct ipsec_shrdesc_param *ipsec_shrdesc_param,
		ipsec_handle_t *ipsec_handle);

/**************************************************************************//**
@Function	ipsec_sa_rem

@Description	This function performs buffer deallocation of the IPsec handler.

		Implicit Input: BPID in the SRAM.

@Param[in]	ipsec_handle - IPsec handle.

@Return		TODO

*//****************************************************************************/
int32_t ipsec_sa_rem(ipsec_handle_t ipsec_handle);

/**************************************************************************//**
@Function	ipsec_read_stats

@Description	This function returns SA lifetime counters:
		seconds and kilobyte.

@Param[in]	ipsec_handle - IPsec handle.

@Return		TODO

*//****************************************************************************/
int32_t ipsec_read_stats(
		ipsec_handle_t ipsec_handle,
		uint32_t *kilobytes,
		uint32_t *sec);

/**************************************************************************//**
@Function	ipsec_frame_decrypt

@Description	This function performs the decryption and the required IPsec
		protocol changes (according to RFC4303). This function also
		handles UDP encapsulated IPsec packets according to RFC3948.
		Both Tunnel and Transport modes are supported.
		Tunneling of IPv4/IPv6 packets (either with (up to 64 Bytes) or
		without Ethernet L2) within IPv4/IPv6 is supported.
		The function also updates the decrypted frame parser result and
		checks the inner UDP checksum (if available).

@Param[in]	ipsec_handle - IPsec handle.

@Return		TODO (implementation dependent)

@Cautions	User should note the following:
		 - In this function the task yields.
		 - This function preserves the Order Scope mode of the task. If
		the Order Scope is of mode concurrent, the Order Scope ID is
		incremented by 1.
		 - It is assumed that IPv6 ESP extension is the last IPv6
		extension in the packet.
		 - This function does not support input frames which are IPv6
		jumbograms.
*//****************************************************************************/
int32_t ipsec_frame_decrypt(ipsec_handle_t ipsec_handle);

/**************************************************************************//**
@Function	ipsec_frame_encrypt

@Description	This function performs the encryption and the required IPsec
		protocol changes (according to RFC4303).  This function
		enables UDP encapsulation of the IPsec packets according
		to RFC3948.
		Both Tunnel and Transport modes are supported.
		Tunneling of IPv4/IPv6 packets (either with (up to 64 Bytes) or
		without Ethernet L2) within IPv4/IPv6 is supported.
		The function also updates the encrypted frame parser result.

@Param[in]	ipsec_handle - IPsec handle.

@Return		TODO (implementation dependent)

@Cautions	User should note the following:
		 - In this function the task yields.
		 - This function preserves the Order Scope mode of the task. If
		the Order Scope is of mode concurrent, the Order Scope ID is
		incremented by 1.
		 - This function does not support encrypted frames which are
		IPv6 jumbograms.
*//****************************************************************************/
int32_t ipsec_frame_encrypt(ipsec_handle_t ipsec_handle);

/** @} */ /* end of FSL_IPSEC_Functions */

/** @} */ /* end of FSL_IPSEC */


#endif /* __FSL_IPSEC_H */
