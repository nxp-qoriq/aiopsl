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


#ifndef __RTA_JUMP_CMD_H__
#define __RTA_JUMP_CMD_H__

extern enum rta_sec_era rta_sec_era;

static const uint32_t jump_test_cond[][2] = {
	{ NIFP,     JUMP_COND_NIFP },
	{ NIP,      JUMP_COND_NIP },
	{ NOP,      JUMP_COND_NOP },
	{ NCP,      JUMP_COND_NCP },
	{ CALM,     JUMP_COND_CALM },
	{ SELF,     JUMP_COND_SELF },
	{ SHRD,     JUMP_COND_SHRD },
	{ JQP,      JUMP_COND_JQP },
	{ MATH_Z,   JUMP_COND_MATH_Z },
	{ MATH_N,   JUMP_COND_MATH_N },
	{ MATH_NV,  JUMP_COND_MATH_NV },
	{ MATH_C,   JUMP_COND_MATH_C },
	{ PK_0,     JUMP_COND_PK_0 },
	{ PK_GCD_1, JUMP_COND_PK_GCD_1 },
	{ PK_PRIME, JUMP_COND_PK_PRIME },
	{ CLASS1,   JUMP_CLASS_CLASS1 },
	{ CLASS2,   JUMP_CLASS_CLASS2 },
	{ BOTH,     JUMP_CLASS_BOTH }
};

static const uint32_t jump_test_math_cond[][2] = {
	{ MATH_Z,   JUMP_COND_MATH_Z },
	{ MATH_N,   JUMP_COND_MATH_N },
	{ MATH_NV,  JUMP_COND_MATH_NV },
	{ MATH_C,   JUMP_COND_MATH_C }
};

static const uint32_t jump_src_dst[][2] = {
	{ _MATH0,     JUMP_SRC_DST_MATH0 },
	{ _MATH1,     JUMP_SRC_DST_MATH1 },
	{ _MATH2,     JUMP_SRC_DST_MATH2 },
	{ _MATH3,     JUMP_SRC_DST_MATH3 },
	{ _DPOVRD,    JUMP_SRC_DST_DPOVRD },
	{ _SEQINSZ,   JUMP_SRC_DST_SEQINLEN },
	{ _SEQOUTSZ,  JUMP_SRC_DST_SEQOUTLEN },
	{ _VSEQINSZ,  JUMP_SRC_DST_VARSEQINLEN },
	{ _VSEQOUTSZ, JUMP_SRC_DST_VARSEQOUTLEN }
};

static inline unsigned rta_jump(struct program *program, uint64_t address,
				int address_type, uint32_t jump_type,
				uint32_t test_type, uint32_t test_condition,
				uint32_t src_dst, int type_src_dst)
{
	uint32_t opcode = CMD_JUMP;
	unsigned start_pc = program->current_pc;

	if ((address_type != IMM_DATA) && (address_type != PTR_DATA)) {
		pr_err("JUMP: Address must be either IMM or PTR\n");
		goto err;
	}

	if (((jump_type == GOSUB) || (jump_type == RETURN)) &&
	    (rta_sec_era < RTA_SEC_ERA_4)) {
		pr_err("JUMP: Jump type not supported by SEC Era %d\n",
		       USER_SEC_ERA(rta_sec_era));
		goto err;
	}

	if (((jump_type == LOCAL_JUMP_INC) || (jump_type == LOCAL_JUMP_DEC)) &&
	    (rta_sec_era <= RTA_SEC_ERA_5)) {
		pr_err("JUMP_INCDEC: Jump type not supported by SEC Era %d\n",
		       USER_SEC_ERA(rta_sec_era));
		goto err;
	}

	switch (jump_type) {
	case (LOCAL_JUMP):
		/*
		 * opcode |= JUMP_TYPE_LOCAL;
		 * JUMP_TYPE_LOCAL is 0
		 */
		break;
	case (HALT):
		opcode |= JUMP_TYPE_HALT;
		break;
	case (HALT_STATUS):
		opcode |= JUMP_TYPE_HALT_USER;
		break;
	case (FAR_JUMP):
		opcode |= JUMP_TYPE_NONLOCAL;
		break;
	case (GOSUB):
		opcode |= JUMP_TYPE_GOSUB;
		break;
	case (RETURN):
		opcode |= JUMP_TYPE_RETURN;
		break;
	case (LOCAL_JUMP_INC):
		opcode |= JUMP_TYPE_LOCAL_INC;
		break;
	case (LOCAL_JUMP_DEC):
		opcode |= JUMP_TYPE_LOCAL_DEC;
		break;
	default:
		pr_err("JUMP: Invalid jump type. SEC Program Line: %d\n",
		       program->current_pc);
		goto err;
	}

	switch (test_type) {
	case (ALL_TRUE):
		/*
		 * opcode |= JUMP_TEST_ALL;
		 * JUMP_TEST_ALL is 0
		 */
		break;
	case (ALL_FALSE):
		opcode |= JUMP_TEST_INVALL;
		break;
	case (ANY_TRUE):
		opcode |= JUMP_TEST_ANY;
		break;
	case (ANY_FALSE):
		opcode |= JUMP_TEST_INVANY;
		break;
	default:
		pr_err("JUMP: test type not supported. SEC Program Line: %d\n",
		       program->current_pc);
		goto err;
	}

	/* write test condition field */
	if ((jump_type != LOCAL_JUMP_INC) && (jump_type != LOCAL_JUMP_DEC)) {
		__rta_map_flags(test_condition, jump_test_cond,
				ARRAY_SIZE(jump_test_cond), &opcode);
	} else {
		uint32_t val = 0;
		int ret;

		if (type_src_dst != REG_TYPE) {
			pr_err("JUMP_INCDEC: Incorrect SRC_DST type. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		}

		ret = __rta_map_opcode(src_dst, jump_src_dst,
				       ARRAY_SIZE(jump_src_dst), &val);
		if (ret == -1) {
			pr_err("JUMP_INCDEC: SRC_DST not supported. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		}
		opcode |= val;

		__rta_map_flags(test_condition, jump_test_math_cond,
				ARRAY_SIZE(jump_test_math_cond), &opcode);
	}

	/* write local offset field for local jumps and user-defined halt */
	if ((jump_type == LOCAL_JUMP) || (jump_type == LOCAL_JUMP_INC) ||
	    (jump_type == LOCAL_JUMP_DEC) || (jump_type == GOSUB) ||
	    (jump_type == HALT_STATUS))
		opcode |= (uint32_t)(address & JUMP_OFFSET_MASK);

	__rta_out32(program, opcode);
	program->current_instruction++;

	if (jump_type == FAR_JUMP)
		__rta_out64(program, program->ps, address);

	return start_pc;

 err:
	program->first_error_pc = start_pc;
	program->current_instruction++;
	return start_pc;
}

#endif /* __RTA_JUMP_CMD_H__ */
