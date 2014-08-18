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

/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __DESC_JOBDESC_H__
#define __DESC_JOBDESC_H__

#include "flib/rta.h"
#include "common.h"

/**
 * DOC: Job Descriptor Constructors
 *
 * Job descriptors for certain tasks, like generating MDHA split keys.
 */

/**
 * cnstr_jobdesc_mdsplitkey - Generate an MDHA split key
 * @descbuf: pointer to buffer to hold constructed descriptor
 * @ps: if 36/40bit addressing is desired, this parameter must be true
 * @alg_key: pointer to HMAC key to generate ipad/opad from
 * @keylen: HMAC key length
 * @cipher: HMAC algorithm selection, one of OP_ALG_ALGSEL_*
 *     The algorithm determines key size (bytes):
 *     -  OP_ALG_ALGSEL_MD5    - 16
 *     -  OP_ALG_ALGSEL_SHA1   - 20
 *     -  OP_ALG_ALGSEL_SHA224 - 28
 *     -  OP_ALG_ALGSEL_SHA256 - 32
 *     -  OP_ALG_ALGSEL_SHA384 - 48
 *     -  OP_ALG_ALGSEL_SHA512 - 64
 * @padbuf: pointer to buffer to store generated ipad/opad
 *
 * Split keys are IPAD/OPAD pairs. For details, refer to MDHA Split Keys chapter
 * in SEC Reference Manual.
 *
 * Return: size of descriptor written in words
 */

static inline int cnstr_jobdesc_mdsplitkey(uint32_t *descbuf, bool ps,
					   uint64_t alg_key, uint8_t keylen,
					   uint32_t cipher, uint64_t padbuf)
{
	struct program prg;
	struct program *p = &prg;

	PROGRAM_CNTXT_INIT(p, descbuf, 0);
	if (ps)
		PROGRAM_SET_36BIT_ADDR(p);
	JOB_HDR(p, SHR_NEVER, 1, 0, 0);
	KEY(p, KEY2, 0, alg_key, keylen, 0);
	ALG_OPERATION(p, cipher,
		      OP_ALG_AAI_HMAC,
		      OP_ALG_AS_INIT,
		      ICV_CHECK_DISABLE,
		      OP_ALG_DECRYPT);
	FIFOLOAD(p, MSG2, 0, 0, LAST2 | IMMED | COPY);
	JUMP(p, 1, LOCAL_JUMP, ALL_TRUE, CLASS2);
	FIFOSTORE(p, MDHA_SPLIT_KEY, 0, padbuf, split_key_len(cipher), 0);
	return PROGRAM_FINALIZE(p);
}

#endif /* __DESC_JOBDESC_H__ */
