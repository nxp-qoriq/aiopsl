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


#ifndef __DESC_JOBDESC_H__
#define __DESC_JOBDESC_H__

#include "flib/rta.h"
#include "common.h"

/**
 * @file                 jobdesc.h
 * @brief                SEC Job Descriptor Construction Library
 *                       Job Descriptor Constructors
 */

/**
 * @defgroup jobdesc_group Job Descriptor Example Routines
 * @ingroup descriptor_lib_group
 * @{
 */


/**
 * @details Generate an MDHA split key
 *          Split keys are IPAD/OPAD pairs. For details, refer to MDHA Split
 *          Keys chapter in SEC Reference Manual
 *
 * @param[out] descbuf pointer to buffer to hold constructed descriptor
 * @param[out] bufsize pointer to size of descriptor once constructed
 * @param[in] ps       If 36/40bit addressing is desired, this parameter must be
 *                     non-zero.
 * @param[in] alg_key  Pointer to HMAC key to generate ipad/opad from
 * @param[in] keylen   HMAC key length.
 * @param[in] cipher   HMAC algorithm selection, one of OP_ALG_ALGSEL_*.
 *           The algorithm determines key size (bytes):
 *      -  OP_ALG_ALGSEL_MD5    - 16
 *      -  OP_ALG_ALGSEL_SHA1   - 20
 *      -  OP_ALG_ALGSEL_SHA224 - 28
 *      -  OP_ALG_ALGSEL_SHA256 - 32
 *      -  OP_ALG_ALGSEL_SHA384 - 48
 *      -  OP_ALG_ALGSEL_SHA512 - 64
 *
 * @param [out] padbuf - Pointer to buffer to store generated ipad/opad
 */

static inline void cnstr_jobdesc_mdsplitkey(uint32_t *descbuf,
					    unsigned *bufsize,
					    unsigned short ps,
					    uint64_t alg_key,
					    uint8_t keylen,
					    uint32_t cipher,
					    uint64_t padbuf)
{
	/* Sizes for MDHA pads (*not* keys) in bytes */
	static const uint8_t mdpadlen[] = {
		16,	/* MD5 */
		20,	/* SHA1 */
		32,	/* SHA224 */
		32,	/* SHA256 */
		64,	/* SHA384 */
		64	/* SHA512 */
	};
	uint32_t split_key_len, idx;
	struct program prg;
	struct program *program = &prg;

	idx = (cipher & OP_ALG_ALGSEL_SUBMASK) >> OP_ALG_ALGSEL_SHIFT;
	split_key_len = (uint32_t)(mdpadlen[idx] * 2);

	PROGRAM_CNTXT_INIT(descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR();
	JOB_HDR(SHR_NEVER, 1, 0, 0);
	KEY(KEY2, 0, PTR(alg_key), keylen, 0);
	ALG_OPERATION(cipher,
		      OP_ALG_AAI_HMAC,
		      OP_ALG_AS_INIT,
		      ICV_CHECK_DISABLE,
		      OP_ALG_DECRYPT);
	FIFOLOAD(MSG2, PTR(0), 0, LAST2 | IMMED);
	JUMP(IMM(1), LOCAL_JUMP, ALL_TRUE, CLASS2);
	FIFOSTORE(MDHA_SPLIT_KEY, 0, padbuf, split_key_len, 0);
	*bufsize = PROGRAM_FINALIZE();
}

/** @} end of jobdesc_group */

#endif /* __DESC_JOBDESC_H__ */
