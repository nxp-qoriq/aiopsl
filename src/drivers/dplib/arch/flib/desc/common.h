/*
 * Copyright 2008-2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause or GPL-2.0+
 */

#ifndef __DESC_COMMON_H__
#define __DESC_COMMON_H__

#include "flib/rta.h"

/**
 * DOC: Shared Descriptor Constructors - shared structures
 *
 * Data structures shared between algorithm, protocol implementations.
 */

/**
 * enum rta_data_type - Indicates how is the data provided and how to include it
 *                      in the descriptor.
 * @RTA_DATA_PTR: Data is in memory and accessed by reference; data address is a
 *               physical (bus) address.
 * @RTA_DATA_IMM: Data is inlined in descriptor and accessed as immediate data;
 *               data address is a virtual address.
 * @RTA_DATA_IMM_DMA: (AIOP only) Data is inlined in descriptor and accessed as
 *                   immediate data; data address is a physical (bus) address
 *                   in external memory and CDMA is programmed to transfer the
 *                   data into descriptor buffer being built in Workspace Area.
 */
enum rta_data_type {
	RTA_DATA_PTR = 1,
	RTA_DATA_IMM,
	RTA_DATA_IMM_DMA
};

/**
 * struct alginfo - Container for algorithm details
 * @algtype: algorithm selector; for valid values, see documentation of the
 *           functions where it is used.
 * @keylen: length of the provided algorithm key, in bytes
 * @key: address where algorithm key resides; virtual address if key_type is
 *       RTA_DATA_IMM, physical (bus) address if key_type is RTA_DATA_PTR or
 *       RTA_DATA_IMM_DMA.
 * @key_enc_flags: key encryption flags; see encrypt_flags parameter of KEY
 *                 command for valid values.
 * @key_type: enum rta_data_type
 */
struct alginfo {
	uint32_t algtype;
	uint32_t keylen;
	uint64_t key;
	uint32_t key_enc_flags;
	enum rta_data_type key_type;
};

static inline uint32_t inline_flags(enum rta_data_type data_type)
{
	switch (data_type) {
	case RTA_DATA_PTR:
		return 0;
	case RTA_DATA_IMM:
		return IMMED | COPY;
	case RTA_DATA_IMM_DMA:
		return IMMED | DCOPY;
	default:
		/* warn and default to RTA_DATA_PTR */
		pr_warn("RTA: defaulting to RTA_DATA_PTR parameter type\n");
		return 0;
	}
}

#define INLINE_KEY(alginfo)	inline_flags(alginfo->key_type)

/**
 * rta_inline_query() - Provide indications on which data items can be inlined
 *                      and which shall be referenced in a shared descriptor.
 * @sd_base_len: Shared descriptor base length - bytes consumed by the commands,
 *               excluding the data items to be inlined (or corresponding
 *               pointer if an item is not inlined). Each cnstr_* function that
 *               generates descriptors should have a define mentioning
 *               corresponding length.
 * @jd_len: Maximum length of the job descriptor(s) that will be used
 *          together with the shared descriptor.
 * @data_len: Array of lengths of the data items trying to be inlined
 * @inl_mask: 32bit mask with bit x = 1 if data item x can be inlined, 0
 *            otherwise.
 * @count: Number of data items (size of @data_len array); must be <= 32
 *
 * Return: 0 if data can be inlined / referenced, negative value if not. If 0,
 *         check @inl_mask for details.
 */
static inline int rta_inline_query(unsigned sd_base_len, unsigned jd_len,
				   unsigned *data_len, uint32_t *inl_mask,
				   unsigned count)
{
	int rem_bytes = (int)(CAAM_DESC_BYTES_MAX - sd_base_len - jd_len);
	unsigned i;

	*inl_mask = 0;
	for (i = 0; (i < count) && (rem_bytes > 0); i++) {
		if (rem_bytes - (int)(data_len[i] +
			(count - i - 1) * CAAM_PTR_SZ) >= 0) {
			rem_bytes -= data_len[i];
			*inl_mask |= (1 << i);
		} else {
			rem_bytes -= CAAM_PTR_SZ;
		}
	}

	return (rem_bytes >= 0) ? 0 : -1;
}

/**
 * struct protcmd - Container for Protocol Operation Command fields
 * @optype: command type
 * @protid: protocol Identifier
 * @protinfo: protocol Information
 */
struct protcmd {
	uint32_t optype;
	uint32_t protid;
	uint16_t protinfo;
};

/**
 * split_key_len - Compute MDHA split key length for a given algorithm
 * @hash: Hashing algorithm selection, one of OP_ALG_ALGSEL_* - MD5, SHA1,
 *        SHA224, SHA384, SHA512.
 *
 * Return: MDHA split key length
 */
static inline uint32_t split_key_len(uint32_t hash)
{
	/* Sizes for MDHA pads (*not* keys): MD5, SHA1, 224, 256, 384, 512 */
	static const uint8_t mdpadlen[] = { 16, 20, 32, 32, 64, 64 };
	uint32_t idx;

	idx = (hash & OP_ALG_ALGSEL_SUBMASK) >> OP_ALG_ALGSEL_SHIFT;

	return (uint32_t)(mdpadlen[idx] * 2);
}

/**
 * split_key_pad_len - Compute MDHA split key pad length for a given algorithm
 * @hash: Hashing algorithm selection, one of OP_ALG_ALGSEL_* - MD5, SHA1,
 *        SHA224, SHA384, SHA512.
 *
 * Return: MDHA split key pad length
 */
static inline uint32_t split_key_pad_len(uint32_t hash)
{
	return ALIGN(split_key_len(hash), 16);
}

#endif /* __DESC_COMMON_H__ */
