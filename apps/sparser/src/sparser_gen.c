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

/*******************************************************************************
 *                              - CAUTION -
 *
 * This code must not be distributed till we haven't a clear understanding
 * about what internals of the Parser may be exposed to customers.
 ******************************************************************************/

#include "fsl_types.h"
#include <__mem.h>
#include "fsl_stdio.h"
#include "fsl_sparser_gen.h"
#include "fsl_sparser_drv.h"
#include "fsl_sparser_disa.h"
#include "fsl_dbg.h"

/******************************************************************************/
#define SP_JMP_DST_MASK			0x07FF	/* Address space mask */

#define SP_JMP_DST_GFLAG		0x8000	/* Gosub flag */
#define SP_JMP_DST_LFLAG		0x4000	/* Relative address flag */
#define SP_JMP_DST_AFLAG		0x2000	/* Advance flag */
#define SP_JMP_DST_BFLAG		0x0800	/* Label flag */
#define SP_JMP_DST_SFLAG		0x0400	/* Sign bit */

#define SP_JMP_DST_FLAGS	(SP_JMP_DST_GFLAG | SP_JMP_DST_LFLAG |	\
				SP_JMP_DST_AFLAG | SP_JMP_DST_BFLAG)

/******************************************************************************/
#if (SP_GEN_CHECK_ERRORS == 1)
	#define SPARSER_DISA_INSTR(_len)				      \
			sparser_disa_instr((uint16_t)(spd.pc - (_len)),	      \
					   (uint8_t *)(spd.sp_code - (_len)), \
					   (uint16_t)(2 * (_len)))
#else
	#define SPARSER_DISA_INSTR(_len)
#endif

/******************************************************************************/
struct sparser_ref_label_info {
	uint16_t	pc;
	uint8_t		off;
};

/* The number of references to a "local labels" i.e how many times a local
 * label may be used as a jump destination from inside a soft parser code. */
#define SPARSE_LOCAL_LABEL_REF_COUNT	32

struct sparser_ref_label {
	int				cnt;
	/* Information in this array is filled when an instruction uses, as
	 * jump destination, a local label enumerated value */
	struct sparser_ref_label_info	pc_info[SPARSE_LOCAL_LABEL_REF_COUNT];
};

struct sparser_labels_info {
	/* Information in this field is filled when the sparser_set_label() API
	 * call is used */
	uint16_t			to_pc;
	struct sparser_ref_label	from_pc;
};

/* The number of "local labels" must equal the number of enumerated values in
 * the "sparser_local_label" enumeration */
#define SPARSE_LOCAL_LABELS_NUM		16

struct sparser_bytecode_data {
	uint8_t				initialized;
	uint16_t			start_pc;
	uint8_t				*sp;
	uint16_t			sz;
	uint16_t			*sp_code;
	uint16_t			pc;
	int				words;
	struct sparser_labels_info	sp_labels[SPARSE_LOCAL_LABELS_NUM];
};

/******************************************************************************/
enum sparser_wr {
	/* WR0 */
	SP_WR0 = 0,
	/* WR1 */
	SP_WR1
};

/******************************************************************************/
enum sparser_shift_op {
	/* No shift */
	SP_NO_SHIFT = 0,
	/* Shift */
	SP_SHIFT
};

/******************************************************************************/
enum sparser_arith_op {
	/* Addition */
	SP_ARITH_ADD = 0,
	/* Subtraction */
	SP_ARITH_SUB
};

/******************************************************************************/
enum sparser_cmp_op {
	/* Equality */
	SP_CMP_EQ = 0,
	/* Non-Equality */
	SP_CMP_NE,
	/* Greater Than */
	SP_CMP_GT,
	/* Less Than */
	SP_CMP_LT,
	/* Greater Than or Equal */
	SP_CMP_GE,
	/* Less Than or Equal */
	SP_CMP_LE,
	/* No comparison performed */
	SP_CMP_NOP
};

/******************************************************************************/
enum sparser_protocol {
	/* L2 protocols */
	SP_L2 = 0,
	/* L3 protocols */
	SP_L3,
	/* L4 protocols */
	SP_L4
};

/******************************************************************************/
enum sparser_bitwise_op {
	/* Bitwise OR operation */
	SP_BITWISE_OR = 0,
	/* Bitwise AND operation */
	SP_BITWISE_AND,
	/* Bitwise XOR operation */
	SP_BITWISE_XOR,
	/* Bitwise clear destination */
	SP_BITWISE_CLR
};

/******************************************************************************/
static struct sparser_bytecode_data	spd;

/*******************************/
/* Internal API implementation */
/*******************************/

#if (SP_GEN_CHECK_ERRORS == 1)
/******************************************************************************/
static __COLD_CODE inline void sparser_gen_check_rem_words(int needed_words)
{
	if (!spd.initialized) {
		pr_err("SP byte-code not initialized\n");
		ASSERT_COND(0);
		return;
	}
	if (spd.words < needed_words) {
		pr_err("Exceeded SP size : %d\n", spd.sz);
		ASSERT_COND(0);
		return;
	}
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_check_jmp_dest(uint16_t jmp_dst,
						   uint8_t len, uint8_t off)
{
	uint8_t		rel_addr, sign;
	uint16_t	spd_pc, jmp_pc;

	spd_pc = (uint16_t)(spd.pc - len);
	/* Check the validity of a local label */
	if (jmp_dst & SP_JMP_DST_BFLAG) {
		uint16_t	label;
		int		idx;

		if (!(jmp_dst & SP_JMP_DST_LFLAG)) {
			pr_err("Relative addressing not set for label : 0x%x\n",
			       jmp_dst);
			ASSERT_COND(0);
			return;
		}
		label = jmp_dst & (SP_JMP_DST_LFLAG | SP_JMP_DST_BFLAG |
				   SP_JMP_DST_MASK);
		if (label < sp_label_1 | label > sp_label_16) {
			pr_err("Invalid label : 0x%x\n", label);
			ASSERT_COND(0);
			return;
		}
		/* Add Current PC to the list of labels */
		idx = (int)(label & 0x0F);
		if (spd.sp_labels[idx].from_pc.cnt ==
		    SPARSE_LOCAL_LABEL_REF_COUNT) {
			pr_err("sp_label_%d referred more than %d times\n",
			       idx + 1, SPARSE_LOCAL_LABEL_REF_COUNT);
			ASSERT_COND(0);
			return;
		}
		ASSERT_COND(off < len);
		spd.sp_labels[idx].from_pc.pc_info
			[spd.sp_labels[idx].from_pc.cnt].pc = spd_pc;
		spd.sp_labels[idx].from_pc.pc_info
			[spd.sp_labels[idx].from_pc.cnt].off = off;
		spd.sp_labels[idx].from_pc.cnt++;
		return;	/* Valid label */
	}
	/* Check the validity of a jump destination */
	/* Clear the valid flags */
	rel_addr = (jmp_dst & SP_JMP_DST_LFLAG) ? 1 : 0;
	jmp_dst &= ~(SP_JMP_DST_GFLAG | SP_JMP_DST_LFLAG);
	if (jmp_dst > sp_end_parsing_hxs_dst) {
		pr_err("Invalid jump destination : 0x%x\n", jmp_dst);
		ASSERT_COND(0);
		return;
	}
	jmp_pc = jmp_dst & SP_JMP_DST_MASK;
	if (rel_addr) {
		sign = (jmp_pc & SP_JMP_DST_SFLAG) ? 1 : 0;
		jmp_pc &= ~SP_JMP_DST_SFLAG;
	} else {
		sign = 0;
	}
	if (rel_addr) {
		/* Relative addressing */
		if (!jmp_pc) {
			pr_err("Relative jump on same PC (offset 0)\n");
			ASSERT_COND(0);
			return;
		}
		if (sign) {
			/* Negative jump */
			if (jmp_pc > spd_pc) {
				pr_err("Relative jump before PC = 0\n");
				ASSERT_COND(0);
				return;
			}
			jmp_pc = spd_pc - jmp_pc;
		} else {
			/* Positive jump */
			if (spd_pc + jmp_pc > sp_end_parsing_hxs_dst) {
				pr_err("Relative jump after PC = 0x7FF\n");
				ASSERT_COND(0);
				return;
			}
			jmp_pc = spd_pc + jmp_pc;
			if (jmp_pc < spd_pc + len) {
				pr_err("Relative jump inside the current PC\n");
				ASSERT_COND(0);
				return;
			}
		}
	} else {
		/* Absolute addressing */
		if (jmp_pc >= spd_pc && jmp_pc < spd_pc + len) {
			pr_err("Absolute jump 0x%x inside the current PC\n",
			       jmp_pc);
			ASSERT_COND(0);
			return;
		}
	}
	if (jmp_pc < 0x20 && jmp_pc != sp_eth_hxs_dst &&
	    jmp_pc != sp_llc_snap_hxs_dst &&  jmp_pc != sp_vlan_hxs_dst &&
	    jmp_pc != sp_pppoe_ppp_hxs_dst && jmp_pc != sp_mpls_hxs_dst &&
	    jmp_pc != sp_arp_hxs_dst && jmp_pc != sp_ip_hxs_dst &&
	    jmp_pc != sp_ipv4_hxs_dst && jmp_pc != sp_ipv6_hxs_dst &&
	    jmp_pc != sp_gre_hxs_dst && jmp_pc != sp_minencap_hxs_dst &&
	    jmp_pc != sp_other_l3_shell_hxs_dst && jmp_pc != sp_tcp_hxs_dst &&
	    jmp_pc != sp_udp_hxs_dst && jmp_pc != sp_ipsec_hxs_dst &&
	    jmp_pc != sp_sctp_hxs_dst && jmp_pc != sp_dccp_hxs_dst &&
	    jmp_pc != sp_other_l4_shell_hxs_dst && jmp_pc != sp_gtp_hxs_dst &&
	    jmp_pc != sp_esp_hxs_dst && jmp_pc != sp_vxlan_hxs_dst &&
	    jmp_pc != sp_l5_shell_hxs_dst && jmp_pc != sp_final_shell_hxs_dst) {
		pr_err("Jump to an invalid hard HXS : 0x%x\n", jmp_pc);
		ASSERT_COND(0);
		return;
	}
}
#else
	#define sparser_gen_check_rem_words(_a)
	#define sparser_gen_check_jmp_dest(_a, _b, _c)
#endif	/* SP_GEN_CHECK_ERRORS */

/******************************************************************************/
static __COLD_CODE void sparser_gen_clr_wrx(enum sparser_wr wr)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
	opcode = 0x0004 | (uint16_t)wr;		/* W */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void
sparser_gen_jump_to_lx_protocol(enum sparser_protocol proto)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
	opcode = 0x0044 | (uint16_t)proto;		/* P */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void
sparser_gen_op32_wrx_to_wrx(enum sparser_arith_op op, enum sparser_wr from_wr,
			    enum sparser_wr to_wr)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
	opcode = 0x0048 | (uint16_t)from_wr;		/* L */
	opcode |= ((uint16_t)op << 1);			/* O */
	opcode |= ((uint16_t)to_wr << 2);		/* V */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void
sparser_gen_op32_wrx_imm_to_wrx(uint32_t imm, enum sparser_imm_val imm_sz,
				enum sparser_arith_op op,
				enum sparser_wr from_wr,
				enum sparser_wr to_wr)
{
	uint8_t		sz;
	uint16_t	opcode;

	sz = (uint8_t)imm_sz;
#if (SP_GEN_CHECK_ERRORS == 1)
	if (sz > 2) {
		pr_err("Immediate value on %d bits\n", 16 * sz);
		ASSERT_COND(0);
		return;
	}
#endif
	sparser_gen_check_rem_words(sz + 1);
	opcode = 0x0050 | (uint16_t)from_wr;		/* W */
	opcode |= ((uint16_t)op << 1);			/* O */
	opcode |= ((uint16_t)to_wr << 2);		/* V */
	opcode |= ((uint16_t)(sz - 1) << 3);		/* S */
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = (uint16_t)imm;
	if (sz > 1)
		*(spd.sp_code++) = (uint16_t)(imm >> 16);
	spd.words -= sz + 1;
	spd.pc += sz + 1;
	SPARSER_DISA_INSTR(sz + 1);
}

/******************************************************************************/
static __COLD_CODE
void sparser_gen_or_wr0_wr1_to_wrx(enum sparser_wr wr,
				   enum sparser_bitwise_op op)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
	opcode = 0x0070 | (uint16_t)wr;		/* W */
	opcode |= ((uint16_t)op << 1);		/* f */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_cmp_wr0_op_wr1(uint16_t jmp_dst,
						   enum sparser_cmp_op op)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(2);
	/* Clear the A flag */
	jmp_dst &= ~SP_JMP_DST_AFLAG;
	opcode = 0x0078 | (uint16_t)op;		/* c */
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst;
	spd.words -= 2;
	spd.pc += 2;
	SPARSER_DISA_INSTR(2);
	sparser_gen_check_jmp_dest(jmp_dst, 2, 1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_ld_add_wrx_to_wo(enum sparser_wr w,
						     uint8_t add_op)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
	opcode = 0x0080 | (uint16_t)w;			/* W */
	opcode |= ((uint16_t)add_op << 1);		/* A */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE
void sparser_gen_bitwise_op_wrx_imm_to_wrx(enum sparser_bitwise_op op,
					   uint64_t imm,
					   enum sparser_imm_val imm_sz,
					   enum sparser_wr from_wr,
					   enum sparser_wr to_wr)
{
	uint8_t		sz;
	uint16_t	opcode;

	sz = (uint8_t)imm_sz;
	sparser_gen_check_rem_words(sz + 1);
	opcode = 0x00c0 | (uint16_t)from_wr;		/* W */
	opcode |= ((uint16_t)op << 1);			/* f */
	opcode |= ((uint16_t)(sz - 1) << 3);		/* i */
	opcode |= ((uint16_t)to_wr << 5);		/* V */
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = (uint16_t)imm;
	if (sz > 1)
		*(spd.sp_code++) = (uint16_t)(imm >> 16);
	if (sz > 2)
		*(spd.sp_code++) = (uint16_t)(imm >> 32);
	if (sz > 3)
		*(spd.sp_code++) = (uint16_t)(imm >> 48);
	spd.words -= sz + 1;
	spd.pc += sz + 1;
	SPARSER_DISA_INSTR(sz + 1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_shl_wrx_by_sv(uint8_t shift,
						  enum sparser_wr wr)
{
	uint16_t	opcode;

#if (SP_GEN_CHECK_ERRORS == 1)
	if (!shift || shift > 64) {
		pr_err("Invalid shift value %d : 0 or > 64", shift);
		ASSERT_COND(0);
		return;
	}
#endif
	sparser_gen_check_rem_words(1);
	opcode = 0x0100 | (uint16_t)wr;			/* W */
	opcode |= ((uint16_t)(shift - 1) << 1);		/* shift */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_shr_wrx_by_sv(uint8_t shift,
						  enum sparser_wr wr)
{
	uint16_t	opcode;

#if (SP_GEN_CHECK_ERRORS == 1)
	if (!shift || shift > 64) {
		pr_err("Invalid shift value %d : 0 or > 64", shift);
		ASSERT_COND(0);
		return;
	}
#endif
	sparser_gen_check_rem_words(1);
	opcode = 0x0180 | (uint16_t)wr;			/* W */
	opcode |= ((uint16_t)(shift - 1) << 1);		/* shift */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_ldx_imm_to_wrx(uint8_t n, uint64_t imm,
						   enum sparser_imm_val imm_sz,
						   enum sparser_shift_op s,
						   enum sparser_wr w)
{
	uint16_t	opcode;

#if (SP_GEN_CHECK_ERRORS == 1)
	if (!n || n > 16 * (uint8_t)imm_sz) {
		pr_err("Number of bits : %d is 0 or > %d", n,
		       16 * (uint8_t)imm_sz);
		ASSERT_COND(0);
		return;
	}
#else
	UNUSED(imm_sz);
#endif
	sparser_gen_check_rem_words((n - 1) / 16 + 2);
	opcode = 0x0200 | (uint16_t)w;			/* W */
	opcode |= ((uint16_t)(n - 1) << 1);		/* n */
	opcode |= ((uint16_t)s << 7);			/* S */
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = (uint16_t)imm;
	if (n / 16 > 1)
		*(spd.sp_code++) = (uint16_t)(imm >> 16);
	if (n / 16 > 2)
		*(spd.sp_code++) = (uint16_t)(imm >> 32);
	if (n / 16 > 3)
		*(spd.sp_code++) = (uint16_t)(imm >> 48);
	spd.words -= (n - 1) / 16 + 2;
	spd.pc += (n - 1) / 16 + 2;
	SPARSER_DISA_INSTR((n - 1) / 16 + 2);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_op_faf_bit(enum sparser_faf_bit faf_bit,
					       uint8_t op)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
	opcode = 0x0300 | (uint16_t)faf_bit;		/* j */
	opcode |= ((uint16_t)op << 7);			/* c */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_ldx_pa_to_wrx(uint8_t pos, uint8_t n,
						  enum sparser_shift_op s,
						  enum sparser_wr w)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
#if (SP_GEN_CHECK_ERRORS == 1)
	if (!n || n > 8) {
		pr_err("Number of bytes : %d is 0 or > 8", n);
		ASSERT_COND(0);
		return;
	}
	if (pos > 63) {
		pr_err("Byte position : %d > 63", pos);
		ASSERT_COND(0);
		return;
	}
	if (pos + n > 64) {
		pr_err("Outside Parameters Array read \"pos + n\" : %d > 64",
		       pos + n);
		ASSERT_COND(0);
		return;
	}
#endif
	opcode = 0x1000 | (uint16_t)w;			/* W */
	opcode |= ((uint16_t)(n - 1) << 1);		/* j */
	opcode |= ((uint16_t)(pos + n - 1) << 4);	/* k */
	opcode |= ((uint16_t)s << 10);			/* S */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_st_wrx_to_ra(uint8_t pos, uint8_t n,
						 enum sparser_wr wr)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
#if (SP_GEN_CHECK_ERRORS == 1)
	if (!n || n > 8) {
		pr_err("Number of bytes : %d is 0 or > 8", n);
		ASSERT_COND(0);
		return;
	}
	if (pos > 127) {
		pr_err("Byte position : %d > 127", pos);
		ASSERT_COND(0);
		return;
	}
	if (pos + n > 128) {
		pr_err("Outside Parse Array write \"pos + n\" : %d > 128",
		       pos + n);
		ASSERT_COND(0);
		return;
	}
#endif
	opcode = 0x2800 | (uint16_t)wr;			/* W */
	opcode |= ((uint16_t)(pos + n - 1) << 1);	/* t */
	opcode |= ((uint16_t)(n - 1) << 8);		/* s */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_ldx_ra_to_wrx(uint8_t pos, uint8_t n,
						  enum sparser_shift_op s,
						  enum sparser_wr w)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
#if (SP_GEN_CHECK_ERRORS == 1)
	if (!n || n > 8) {
		pr_err("Number of bytes : %d is 0 or > 8", n);
		ASSERT_COND(0);
		return;
	}
	if (pos > 127) {
		pr_err("Byte position : %d > 127", pos);
		ASSERT_COND(0);
		return;
	}
	if (pos + n > 128) {
		pr_err("Outside Parse Array read \"pos + n\" : %d > 128",
		       pos + n);
		ASSERT_COND(0);
		return;
	}
#endif
	opcode = 0x3000 | (uint16_t)w;			/* W */
	opcode |= ((uint16_t)(n - 1) << 1);		/* k */
	opcode |= ((uint16_t)(pos + n - 1) << 4);	/* j */
	opcode |= ((uint16_t)s << 11);			/* S */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/******************************************************************************/
static __COLD_CODE void sparser_gen_ldx_fw_to_wrx(uint8_t pos, uint8_t n,
						  enum sparser_shift_op s,
						  enum sparser_wr w)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
#if (SP_GEN_CHECK_ERRORS == 1)
	if (!n || n > 64) {
		pr_err("Number of bits : %d is 0 or > 64", n);
		ASSERT_COND(0);
		return;
	}
	if (pos > 127) {
		pr_err("Bit position : %d > 127", pos);
		ASSERT_COND(0);
		return;
	}
	if (pos + n > 128) {
		pr_err("Outside Frame Window read \"pos + n\" : %d > 128",
		       pos + n);
		ASSERT_COND(0);
		return;
	}
#endif
	opcode = 0x8000 | (uint16_t)w;			/* W */
	opcode |= ((uint16_t)(n - 1) << 1);		/* n */
	opcode |= ((uint16_t)(pos + n - 1) << 7);	/* m */
	opcode |= ((uint16_t)s << 14);			/* S */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/*******************************/
/* External API implementation */
/*******************************/
/* SPARSER_BEGIN */
/******************************************************************************/
__COLD_CODE void sparser_begin_bytecode_wrt(uint16_t pc, uint8_t *sp,
					    uint16_t sz)
{
	int	i, j;

#if (SP_GEN_CHECK_ERRORS == 1)
	if (spd.initialized) {
		pr_err("SP byte-code already initialized\n");
		ASSERT_COND(0);
		return;
	}
	if (pc < PARSER_MIN_PC || pc >= PARSER_MAX_PC) {
		pr_err("Invalid starting PC 0x%x (< 0x%x or >= 0x%x)\n",
		       pc, PARSER_MIN_PC, PARSER_MAX_PC);
		ASSERT_COND(0);
		return;
	}
	if (!sz) {
		pr_err("SP size is 0\n");
		ASSERT_COND(0);
		return;
	}
	if (sz % 4) {
		pr_err("SP size %d is not 4 multiple\n", sz);
		ASSERT_COND(0);
		return;
	}
#endif
	memset(sp, 0, sz);
	/* Initialize "local labels" information */
	for (i = 0; i < SPARSE_LOCAL_LABELS_NUM; i++) {
		spd.sp_labels[i].to_pc = (uint16_t)-1;
		for (j = 0; j < SPARSE_LOCAL_LABEL_REF_COUNT; j++)
			spd.sp_labels[i].from_pc.pc_info[j].pc = (uint16_t)-1;
	}
	spd.start_pc = pc;
	spd.pc = pc;
	spd.sp = sp;
	spd.sz = sz;
	spd.words = (sz / 2);
	spd.initialized = 1;
	spd.sp_code = (uint16_t *)sp;
}

/* SPARSER_DUMP */
/******************************************************************************/
__COLD_CODE void sparser_bytecode_dump(void)
{
	uint8_t		*pb;
	int		i, len;

	len = 2 * (spd.pc - spd.start_pc);
	fsl_print("\nDump of SP(PC = 0x%x:0x%x) byte array (%d bytes)\n",
		  spd.start_pc, spd.pc - 1, len);
	pb = spd.sp;
	for (i = 0; i < len; i++) {
		fsl_print("0x%02x, ", *pb++);
		if (!((i + 1) % 10))
			fsl_print("\n");
	}
	if (len % 10)
		fsl_print("\n");
	fsl_print("\n");
}

/* SPARSER_END */
/******************************************************************************/
__COLD_CODE void sparser_end_bytecode_wrt(void)
{
	int		i, j, count;
	uint8_t		off;
	uint16_t	from_pc, to_pc, jmp_dst, *sp_code;

	spd.initialized = 0;
	fsl_print("\nPatching local labels...\n");
	count = 0;
	/* Count the "local labels" */
	for (i = 0; i < SPARSE_LOCAL_LABELS_NUM; i++) {
		/* Label not used and not referred */
		if (spd.sp_labels[i].to_pc == (uint16_t)-1 &&
		    !spd.sp_labels[i].from_pc.cnt)
				continue;
		/* Label used and not referred */
		if (spd.sp_labels[i].to_pc != (uint16_t)-1 &&
		    !spd.sp_labels[i].from_pc.cnt) {
			fsl_print("\t sp_label_%d at PC = 0x%x not referred\n",
				  i + 1, spd.sp_labels[i].to_pc);
			continue;
		}
#if (SP_GEN_CHECK_ERRORS == 1)
		/* Label not used but referred !!! */
		if (spd.sp_labels[i].to_pc == (uint16_t)-1 &&
		    spd.sp_labels[i].from_pc.cnt) {
			fsl_print("\t sp_label_%d not used but referred at :\n",
				  i + 1);
			for (j = 0; j < spd.sp_labels[i].from_pc.cnt; j++) {
				from_pc = spd.sp_labels[i].
						from_pc.pc_info[j].pc;
				fsl_print("\t\t PC = 0x%x\n", from_pc);
			}
			ASSERT_COND(0);
			return;
		}
#endif
		count += spd.sp_labels[i].from_pc.cnt;
	}
	if (!count) {
		fsl_print("\t No local label found\n");
		return;
	}
	/* Patch the "local labels" */
	for (i = 0; i < SPARSE_LOCAL_LABELS_NUM; i++) {
		if (spd.sp_labels[i].to_pc == (uint16_t)-1)
			continue;
		to_pc = spd.sp_labels[i].to_pc;
		for (j = 0; j < spd.sp_labels[i].from_pc.cnt; j++) {
			off = spd.sp_labels[i].from_pc.pc_info[j].off;
			from_pc = spd.sp_labels[i].from_pc.pc_info[j].pc;
			jmp_dst = LF;
			if (to_pc > from_pc)
				jmp_dst |= (to_pc - from_pc);
			else
				jmp_dst |= SF | (from_pc - to_pc);
			sp_code = (uint16_t *)spd.sp;
			sp_code += (from_pc - spd.start_pc) + off;
			*sp_code = jmp_dst;
		}
	}
}

/* SP_LABEL */
/******************************************************************************/
__COLD_CODE void sparser_set_label(enum sparser_local_label label)
{
	int	idx;

	idx = (int)(label & 0x0F);
#if (SP_GEN_CHECK_ERRORS == 1)
	if (!spd.initialized) {
		pr_err("SP byte-code not initialized\n");
		ASSERT_COND(0);
		return;
	}
	if (spd.sp_labels[idx].to_pc != (uint16_t)-1) {
		pr_err("Label sp_label_%d already used at PC = 0x%x\n",
		       idx + 1, spd.sp_labels[idx].to_pc);
		ASSERT_COND(0);
		return;
	}
	fsl_print("sp_label_%d:\n", idx + 1);
#endif
	spd.sp_labels[idx].to_pc = spd.pc;
}

/* NOP */
/******************************************************************************/
__COLD_CODE void sparser_gen_nop(void)
{
	sparser_gen_check_rem_words(1);
	*(spd.sp_code++) = 0x0000;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/* ADVANCE_HB_BY_WO */
/******************************************************************************/
__COLD_CODE void sparser_gen_advance_hb_by_wo(void)
{
	sparser_gen_check_rem_words(1);
	*(spd.sp_code++) = 0x0002;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/* CLR_WRx */
/******************************************************************************/
__COLD_CODE void sparser_gen_clr_wr0(void)
{
	sparser_gen_clr_wrx(SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_clr_wr1(void)
{
	sparser_gen_clr_wrx(SP_WR1);
}

/* CHECKSUM16_WR1_TO_WR0 */
/******************************************************************************/
__COLD_CODE void sparser_gen_checksum16_wr1_to_wr0(void)
{
	sparser_gen_check_rem_words(1);
	*(spd.sp_code++) = 0x0006;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/* RETURN_SUB */
/******************************************************************************/
__COLD_CODE void sparser_gen_return_sub(void)
{
	sparser_gen_check_rem_words(1);
	*(spd.sp_code++) = 0x0007;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/* CASE1_DJ_WR_to_WR */
/******************************************************************************/
__COLD_CODE void sparser_gen_case1_dj_wr_to_wr(uint16_t jmp_dst_1,
					       uint16_t jmp_dst_2)
{
	uint16_t	opcode;
	uint8_t		a3, a2;

	sparser_gen_check_rem_words(3);
	a3 = (jmp_dst_1 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a2 = (jmp_dst_2 & SP_JMP_DST_AFLAG) ? 1 : 0;
	/* Clear the A flag */
	jmp_dst_1 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_2 &= ~SP_JMP_DST_AFLAG;
	opcode = 0x0008 | (uint16_t)a3;
	opcode |= (uint16_t)(a2 << 1);
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst_1;
	*(spd.sp_code++) = jmp_dst_2;
	spd.words -= 3;
	spd.pc += 3;
	SPARSER_DISA_INSTR(3);
	sparser_gen_check_jmp_dest(jmp_dst_1, 3, 1);
	sparser_gen_check_jmp_dest(jmp_dst_2, 3, 2);
}

/* CASE2_DC_WR_to_WR */
/******************************************************************************/
__COLD_CODE void sparser_gen_case2_dc_wr_to_wr(uint16_t jmp_dst_1,
					       uint16_t jmp_dst_2)
{
	uint16_t	opcode;
	uint8_t		a3, a2;

	sparser_gen_check_rem_words(3);
	a3 = (jmp_dst_1 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a2 = (jmp_dst_2 & SP_JMP_DST_AFLAG) ? 1 : 0;
	/* Clear the A flag */
	jmp_dst_1 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_2 &= ~SP_JMP_DST_AFLAG;
	opcode = 0x000C | (uint16_t)a3;
	opcode |= (uint16_t)(a2 << 1);
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst_1;
	*(spd.sp_code++) = jmp_dst_2;
	spd.words -= 3;
	spd.pc += 3;
	SPARSER_DISA_INSTR(3);
	sparser_gen_check_jmp_dest(jmp_dst_1, 3, 1);
	sparser_gen_check_jmp_dest(jmp_dst_2, 3, 2);
}

/* CASE2_DJ_WR_to_WR */
/******************************************************************************/
__COLD_CODE void sparser_gen_case2_dj_wr_to_wr(uint16_t jmp_dst_1,
					       uint16_t jmp_dst_2,
					       uint16_t jmp_dst_3)
{
	uint16_t	opcode;
	uint8_t		a3, a2, a1;

	sparser_gen_check_rem_words(3);
	a3 = (jmp_dst_1 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a2 = (jmp_dst_2 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a1 = (jmp_dst_3 & SP_JMP_DST_AFLAG) ? 1 : 0;
	/* Clear the A flag */
	jmp_dst_1 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_2 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_3 &= ~SP_JMP_DST_AFLAG;
	opcode = 0x0010 | (uint16_t)a3;
	opcode |= (uint16_t)(a2 << 1);
	opcode |= (uint16_t)(a1 << 2);
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst_1;
	*(spd.sp_code++) = jmp_dst_2;
	*(spd.sp_code++) = jmp_dst_3;
	spd.words -= 4;
	spd.pc += 4;
	SPARSER_DISA_INSTR(4);
	sparser_gen_check_jmp_dest(jmp_dst_1, 4, 1);
	sparser_gen_check_jmp_dest(jmp_dst_2, 4, 2);
	sparser_gen_check_jmp_dest(jmp_dst_3, 4, 3);
}

/* CASE3_DC_WR_to_WR */
/******************************************************************************/
__COLD_CODE void sparser_gen_case3_dc_wr_to_wr(uint16_t jmp_dst_1,
					       uint16_t jmp_dst_2,
					       uint16_t jmp_dst_3)
{
	uint16_t	opcode;
	uint8_t		a3, a2, a1;

	sparser_gen_check_rem_words(3);
	a3 = (jmp_dst_1 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a2 = (jmp_dst_2 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a1 = (jmp_dst_3 & SP_JMP_DST_AFLAG) ? 1 : 0;
	/* Clear the A flag */
	jmp_dst_1 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_2 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_3 &= ~SP_JMP_DST_AFLAG;
	opcode = 0x0018 | (uint16_t)a3;
	opcode |= (uint16_t)(a2 << 1);
	opcode |= (uint16_t)(a1 << 2);
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst_1;
	*(spd.sp_code++) = jmp_dst_2;
	*(spd.sp_code++) = jmp_dst_3;
	spd.words -= 4;
	spd.pc += 4;
	SPARSER_DISA_INSTR(4);
	sparser_gen_check_jmp_dest(jmp_dst_1, 4, 1);
	sparser_gen_check_jmp_dest(jmp_dst_2, 4, 2);
	sparser_gen_check_jmp_dest(jmp_dst_3, 4, 3);
}

/* CASE3_DJ_WR_to_WR */
/******************************************************************************/
__COLD_CODE void sparser_gen_case3_dj_wr_to_wr(uint16_t jmp_dst_1,
					       uint16_t jmp_dst_2,
					       uint16_t jmp_dst_3,
					       uint16_t jmp_dst_4)
{
	uint16_t	opcode;
	uint8_t		a3, a2, a1, a0;

	sparser_gen_check_rem_words(3);
	a3 = (jmp_dst_1 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a2 = (jmp_dst_2 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a1 = (jmp_dst_3 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a0 = (jmp_dst_4 & SP_JMP_DST_AFLAG) ? 1 : 0;
	/* Clear the A flag */
	jmp_dst_1 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_2 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_3 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_4 &= ~SP_JMP_DST_AFLAG;
	opcode = 0x0020 | (uint16_t)a3;
	opcode |= (uint16_t)(a2 << 1);
	opcode |= (uint16_t)(a1 << 2);
	opcode |= (uint16_t)(a0 << 3);
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst_1;
	*(spd.sp_code++) = jmp_dst_2;
	*(spd.sp_code++) = jmp_dst_3;
	*(spd.sp_code++) = jmp_dst_4;
	spd.words -= 5;
	spd.pc += 5;
	SPARSER_DISA_INSTR(5);
	sparser_gen_check_jmp_dest(jmp_dst_1, 5, 1);
	sparser_gen_check_jmp_dest(jmp_dst_2, 5, 2);
	sparser_gen_check_jmp_dest(jmp_dst_3, 5, 3);
	sparser_gen_check_jmp_dest(jmp_dst_4, 5, 4);
}

/* CASE4_DC_WR_to_WR */
/******************************************************************************/
__COLD_CODE void sparser_gen_case4_dc_wr_to_wr(uint16_t jmp_dst_1,
					       uint16_t jmp_dst_2,
					       uint16_t jmp_dst_3,
					       uint16_t jmp_dst_4)
{
	uint16_t	opcode;
	uint8_t		a3, a2, a1, a0;

	sparser_gen_check_rem_words(3);
	a3 = (jmp_dst_1 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a2 = (jmp_dst_2 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a1 = (jmp_dst_3 & SP_JMP_DST_AFLAG) ? 1 : 0;
	a0 = (jmp_dst_4 & SP_JMP_DST_AFLAG) ? 1 : 0;
	/* Clear the A flag */
	jmp_dst_1 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_2 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_3 &= ~SP_JMP_DST_AFLAG;
	/* Clear the A flag */
	jmp_dst_4 &= ~SP_JMP_DST_AFLAG;
	opcode = 0x0030 | (uint16_t)a3;
	opcode |= (uint16_t)(a2 << 1);
	opcode |= (uint16_t)(a1 << 2);
	opcode |= (uint16_t)(a0 << 3);
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst_1;
	*(spd.sp_code++) = jmp_dst_2;
	*(spd.sp_code++) = jmp_dst_3;
	*(spd.sp_code++) = jmp_dst_4;
	spd.words -= 5;
	spd.pc += 5;
	SPARSER_DISA_INSTR(5);
	sparser_gen_check_jmp_dest(jmp_dst_1, 5, 1);
	sparser_gen_check_jmp_dest(jmp_dst_2, 5, 2);
	sparser_gen_check_jmp_dest(jmp_dst_3, 5, 3);
	sparser_gen_check_jmp_dest(jmp_dst_4, 5, 4);
}

/* JMP_TO_Lx_PROTOCOL */
/******************************************************************************/
__COLD_CODE void sparser_gen_jump_to_l2_protocol(void)
{
	sparser_gen_jump_to_lx_protocol(SP_L2);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_jump_to_l3_protocol(void)
{
	sparser_gen_jump_to_lx_protocol(SP_L3);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_jump_to_l4_protocol(void)
{
	sparser_gen_jump_to_lx_protocol(SP_L4);
}

/* ADD32_WRx_TO_WRx
 * SUB32_WRx_TO_WRx */
/******************************************************************************/
__COLD_CODE void sparser_gen_add32_wr0_to_wr0(void)
{
	sparser_gen_op32_wrx_to_wrx(SP_ARITH_ADD, SP_WR0, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_add32_wr0_to_wr1(void)
{
	sparser_gen_op32_wrx_to_wrx(SP_ARITH_ADD, SP_WR0, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_add32_wr1_to_wr1(void)
{
	sparser_gen_op32_wrx_to_wrx(SP_ARITH_ADD, SP_WR1, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_add32_wr1_to_wr0(void)
{
	sparser_gen_op32_wrx_to_wrx(SP_ARITH_ADD, SP_WR1, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_sub32_wr0_to_wr0(void)
{
	sparser_gen_op32_wrx_to_wrx(SP_ARITH_SUB, SP_WR0, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_sub32_wr0_to_wr1(void)
{
	sparser_gen_op32_wrx_to_wrx(SP_ARITH_SUB, SP_WR0, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_sub32_wr1_to_wr1(void)
{
	sparser_gen_op32_wrx_to_wrx(SP_ARITH_SUB, SP_WR1, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_sub32_wr1_to_wr0(void)
{
	sparser_gen_op32_wrx_to_wrx(SP_ARITH_SUB, SP_WR1, SP_WR0);
}

/* ADD32_WRx_IMM_TO_WRx
 * SUB32_WRx_IMM_TO_WRx */
/******************************************************************************/
__COLD_CODE void sparser_gen_add32_wr0_imm_to_wr0(uint32_t imm,
						  enum sparser_imm_val imm_sz)
{
	sparser_gen_op32_wrx_imm_to_wrx(imm, imm_sz,
					SP_ARITH_ADD, SP_WR0, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_add32_wr0_imm_to_wr1(uint32_t imm,
						  enum sparser_imm_val imm_sz)
{
	sparser_gen_op32_wrx_imm_to_wrx(imm, imm_sz,
					SP_ARITH_ADD, SP_WR0, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_add32_wr1_imm_to_wr1(uint32_t imm,
						  enum sparser_imm_val imm_sz)
{
	sparser_gen_op32_wrx_imm_to_wrx(imm, imm_sz,
					SP_ARITH_ADD, SP_WR1, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_add32_wr1_imm_to_wr0(uint32_t imm,
						  enum sparser_imm_val imm_sz)
{
	sparser_gen_op32_wrx_imm_to_wrx(imm, imm_sz,
					SP_ARITH_ADD, SP_WR1, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_sub32_wr0_imm_to_wr0(uint32_t imm,
						  enum sparser_imm_val imm_sz)
{
	sparser_gen_op32_wrx_imm_to_wrx(imm, imm_sz,
					SP_ARITH_SUB, SP_WR0, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_sub32_wr0_imm_to_wr1(uint32_t imm,
						  enum sparser_imm_val imm_sz)
{
	sparser_gen_op32_wrx_imm_to_wrx(imm, imm_sz,
					SP_ARITH_SUB, SP_WR0, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_sub32_wr1_imm_to_wr1(uint32_t imm,
						  enum sparser_imm_val imm_sz)
{
	sparser_gen_op32_wrx_imm_to_wrx(imm, imm_sz,
					SP_ARITH_SUB, SP_WR1, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_sub32_wr1_imm_to_wr0(uint32_t imm,
						  enum sparser_imm_val imm_sz)
{
	sparser_gen_op32_wrx_imm_to_wrx(imm, imm_sz,
					SP_ARITH_SUB, SP_WR1, SP_WR0);
}

/* OR_WR0_WR1_TO_WRx
 * AND_WR0_WR1_TO_WRx
 * XOR_WR0_WR1_TO_WRx
 * CLR_WR0_WR1_TO_WRx */
/******************************************************************************/
__COLD_CODE void sparser_gen_or_wr0_wr1_to_wr0(void)
{
	sparser_gen_or_wr0_wr1_to_wrx(SP_WR0, SP_BITWISE_OR);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_or_wr0_wr1_to_wr1(void)
{
	sparser_gen_or_wr0_wr1_to_wrx(SP_WR1, SP_BITWISE_OR);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_and_wr0_wr1_to_wr0(void)
{
	sparser_gen_or_wr0_wr1_to_wrx(SP_WR0, SP_BITWISE_AND);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_and_wr0_wr1_to_wr1(void)
{
	sparser_gen_or_wr0_wr1_to_wrx(SP_WR1, SP_BITWISE_AND);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_xor_wr0_wr1_to_wr0(void)
{
	sparser_gen_or_wr0_wr1_to_wrx(SP_WR0, SP_BITWISE_XOR);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_xor_wr0_wr1_to_wr1(void)
{
	sparser_gen_or_wr0_wr1_to_wrx(SP_WR1, SP_BITWISE_XOR);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_clr_wr0_wr1_to_wr0(void)
{
	sparser_gen_or_wr0_wr1_to_wrx(SP_WR0, SP_BITWISE_CLR);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_clr_wr0_wr1_to_wr1(void)
{
	sparser_gen_or_wr0_wr1_to_wrx(SP_WR1, SP_BITWISE_CLR);
}

/* CMP_WR0_xx_WR1 */
/******************************************************************************/
__COLD_CODE void sparser_gen_cmp_wr0_eq_wr1(uint16_t jmp_dst)
{
	sparser_gen_cmp_wr0_op_wr1(jmp_dst, SP_CMP_EQ);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_cmp_wr0_ne_wr1(uint16_t jmp_dst)
{
	sparser_gen_cmp_wr0_op_wr1(jmp_dst, SP_CMP_NE);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_cmp_wr0_gt_wr1(uint16_t jmp_dst)
{
	sparser_gen_cmp_wr0_op_wr1(jmp_dst, SP_CMP_GT);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_cmp_wr0_lt_wr1(uint16_t jmp_dst)
{
	sparser_gen_cmp_wr0_op_wr1(jmp_dst, SP_CMP_LT);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_cmp_wr0_ge_wr1(uint16_t jmp_dst)
{
	sparser_gen_cmp_wr0_op_wr1(jmp_dst, SP_CMP_GE);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_cmp_wr0_le_wr1(uint16_t jmp_dst)
{
	sparser_gen_cmp_wr0_op_wr1(jmp_dst, SP_CMP_LE);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_cmp_wr0_nop_wr1(uint16_t jmp_dst)
{
	sparser_gen_cmp_wr0_op_wr1(jmp_dst, SP_CMP_NOP);
}

/* LD_WR1_TO_WO
ADD_WR0_TO_WO */
/******************************************************************************/
__COLD_CODE void sparser_gen_ld_wr0_to_wo(void)
{
	sparser_gen_ld_add_wrx_to_wo(SP_WR0, 0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_ld_wr1_to_wo(void)
{
	sparser_gen_ld_add_wrx_to_wo(SP_WR1, 0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_add_wr0_to_wo(void)
{
	sparser_gen_ld_add_wrx_to_wo(SP_WR0, 1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_add_wr1_to_wo(void)
{
	sparser_gen_ld_add_wrx_to_wo(SP_WR1, 1);
}

/* OR_WRx_IMM_TO_WRx
 * AND_WRx_IMM_TO_WRx
 * XOR_WRx_IMM_TO_WRx
 * CLR_WRx_IMM_TO_WRx */
/******************************************************************************/
__COLD_CODE
void sparser_gen_or_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_OR, imm, imm_sz,
					      SP_WR0, SP_WR0);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_or_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_OR, imm, imm_sz,
					      SP_WR0, SP_WR1);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_or_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_OR, imm, imm_sz,
					      SP_WR1, SP_WR1);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_or_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_OR, imm, imm_sz,
					      SP_WR1, SP_WR0);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_and_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_AND, imm, imm_sz,
					      SP_WR0, SP_WR0);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_and_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_AND, imm, imm_sz,
					      SP_WR0, SP_WR1);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_and_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_AND, imm, imm_sz,
					      SP_WR1, SP_WR1);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_and_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_AND, imm, imm_sz,
					      SP_WR1, SP_WR0);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_xor_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_XOR, imm, imm_sz,
					      SP_WR0, SP_WR0);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_xor_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_XOR, imm, imm_sz,
					      SP_WR0, SP_WR1);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_xor_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_XOR, imm, imm_sz,
					      SP_WR1, SP_WR1);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_xor_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_XOR, imm, imm_sz,
					      SP_WR1, SP_WR0);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_clr_wr0_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_CLR, imm, imm_sz,
					      SP_WR0, SP_WR0);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_clr_wr0_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_CLR, imm, imm_sz,
					      SP_WR0, SP_WR1);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_clr_wr1_imm_to_wr1(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_CLR, imm, imm_sz,
					      SP_WR1, SP_WR1);
}

/******************************************************************************/
__COLD_CODE
void sparser_gen_clr_wr1_imm_to_wr0(uint64_t imm, enum sparser_imm_val imm_sz)
{
	sparser_gen_bitwise_op_wrx_imm_to_wrx(SP_BITWISE_CLR, imm, imm_sz,
					      SP_WR1, SP_WR0);
}

/* SHL_WRx_BY_SV */
/******************************************************************************/
__COLD_CODE void sparser_gen_shl_wr0_by_sv(uint8_t shift)
{
	sparser_gen_shl_wrx_by_sv(shift, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_shl_wr1_by_sv(uint8_t shift)
{
	sparser_gen_shl_wrx_by_sv(shift, SP_WR1);
}

/* SHR_WRx_BY_SV */
/******************************************************************************/
__COLD_CODE void sparser_gen_shr_wr0_by_sv(uint8_t shift)
{
	sparser_gen_shr_wrx_by_sv(shift, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_shr_wr1_by_sv(uint8_t shift)
{
	sparser_gen_shr_wrx_by_sv(shift, SP_WR1);
}

/* LDx_IMM_BITS_TO_WRx */
/******************************************************************************/
__COLD_CODE void sparser_gen_ld_imm_bits_to_wr0(uint8_t n, uint64_t imm,
						enum sparser_imm_val imm_sz)
{
	sparser_gen_ldx_imm_to_wrx(n, imm, imm_sz, SP_NO_SHIFT, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_lds_imm_bits_to_wr0(uint8_t n, uint64_t imm,
						 enum sparser_imm_val imm_sz)
{
	sparser_gen_ldx_imm_to_wrx(n, imm, imm_sz, SP_SHIFT, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_ld_imm_bits_to_wr1(uint8_t n, uint64_t imm,
						enum sparser_imm_val imm_sz)
{
	sparser_gen_ldx_imm_to_wrx(n, imm, imm_sz, SP_NO_SHIFT, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_lds_imm_bits_to_wr1(uint8_t n, uint64_t imm,
						 enum sparser_imm_val imm_sz)
{
	sparser_gen_ldx_imm_to_wrx(n, imm, imm_sz, SP_SHIFT, SP_WR1);
}

/* SET_FAF_BIT
 * CLEAR_FAF_BIT */
/******************************************************************************/
__COLD_CODE void sparser_gen_set_faf_bit(enum sparser_faf_bit faf_bit)
{
	sparser_gen_op_faf_bit(faf_bit, 1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_clear_faf_bit(enum sparser_faf_bit faf_bit)
{
	sparser_gen_op_faf_bit(faf_bit, 0);
}

/* LD_SV_TO_WO */
/******************************************************************************/
__COLD_CODE void sparser_gen_ld_sv_to_wo(uint8_t sv)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
	opcode = 0x0600 | (uint16_t)sv;		/* short val */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/* ADD_SV_TO_WO */
/******************************************************************************/
__COLD_CODE void sparser_gen_add_sv_to_wo(uint8_t sv)
{
	uint16_t	opcode;

	sparser_gen_check_rem_words(1);
	opcode = 0x0700 | (uint16_t)sv;		/* short val */
	*(spd.sp_code++) = opcode;
	spd.words--;
	spd.pc++;
	SPARSER_DISA_INSTR(1);
}

/* ST_IMM_BYTES_TO_RA */
/******************************************************************************/
__COLD_CODE void sparser_gen_st_imm_bytes_to_ra(uint8_t pos, uint8_t n,
						uint64_t imm,
						enum sparser_imm_val imm_sz)
{
	uint16_t	opcode;

#if (SP_GEN_CHECK_ERRORS == 1)
	if (!n || n > 2 * (uint8_t)imm_sz) {
		pr_err("Number of bytes : %d is 0 or > %d", n,
		       2 * (uint8_t)imm_sz);
		ASSERT_COND(0);
		return;
	}
	if (pos > 127) {
		pr_err("Byte position : %d > 127", pos);
		ASSERT_COND(0);
		return;
	}
	if (pos + n > 128) {
		pr_err("Outside Parse Array write \"pos + n\" : %d > 128",
		       pos + n);
		ASSERT_COND(0);
		return;
	}
#else
	UNUSED(imm_sz);
#endif
	sparser_gen_check_rem_words((n - 1) / 2 + 2);
	opcode = 0x0800 | (uint16_t)(pos + n - 1);	/* t */
	opcode |= ((uint16_t)(n - 1) << 7);		/* s */
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = (uint16_t)imm;
	if (n / 2 > 1)
		*(spd.sp_code++) = (uint16_t)(imm >> 16);
	if (n / 2 > 2)
		*(spd.sp_code++) = (uint16_t)(imm >> 32);
	if (n / 2 > 3)
		*(spd.sp_code++) = (uint16_t)(imm >> 48);
	spd.words -= (n - 1) / 2 + 2;
	spd.pc += (n - 1) / 2 + 2;
	SPARSER_DISA_INSTR((n - 1) / 2 + 2);
}

/* JMP_FAF */
/******************************************************************************/
__COLD_CODE void sparser_gen_jmp_faf(enum sparser_faf_bit faf_bit,
				     uint16_t jmp_dst)
{
	uint16_t	opcode;
	uint8_t		a;

	sparser_gen_check_rem_words(2);
	a = (jmp_dst & SP_JMP_DST_AFLAG) ? 1 : 0;
	/* Clear the A flag */
	jmp_dst &= ~SP_JMP_DST_AFLAG;
	opcode = 0x0c00 | (uint16_t)faf_bit;	/* j */
	opcode |= (uint16_t)(a << 7);		/* A */
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst;
	spd.words -= 2;
	spd.pc += 2;
	SPARSER_DISA_INSTR(2);
	sparser_gen_check_jmp_dest(jmp_dst, 2, 1);
}

/* LDx_PA_TO_WRx */
/******************************************************************************/
__COLD_CODE void sparser_gen_ld_pa_to_wr0(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_pa_to_wrx(pos, n, SP_NO_SHIFT, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_lds_pa_to_wr0(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_pa_to_wrx(pos, n, SP_SHIFT, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_ld_pa_to_wr1(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_pa_to_wrx(pos, n, SP_NO_SHIFT, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_lds_pa_to_wr1(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_pa_to_wrx(pos, n, SP_SHIFT, SP_WR1);
}

/* JMP */
/******************************************************************************/
__COLD_CODE void sparser_gen_jmp(uint16_t jmp_dst)
{
	uint16_t	opcode;
	uint8_t		a;

	sparser_gen_check_rem_words(2);
	a = (jmp_dst & SP_JMP_DST_AFLAG) ? 1 : 0;
	/* Clear the A flag */
	jmp_dst &= ~SP_JMP_DST_AFLAG;
	opcode = 0x1800 | (uint16_t)a;		/* A */
	*(spd.sp_code++) = opcode;
	*(spd.sp_code++) = jmp_dst;
	spd.words -= 2;
	spd.pc += 2;
	SPARSER_DISA_INSTR(2);
	sparser_gen_check_jmp_dest(jmp_dst, 2, 1);
}

/* ST_WRx_TO_RA */
/******************************************************************************/
__COLD_CODE void sparser_gen_st_wr0_to_ra(uint8_t pos, uint8_t n)
{
	sparser_gen_st_wrx_to_ra(pos, n, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_st_wr1_to_ra(uint8_t pos, uint8_t n)
{
	sparser_gen_st_wrx_to_ra(pos, n, SP_WR1);
}

/* LDx_RA_TO_WRx */
/******************************************************************************/
__COLD_CODE void sparser_gen_ld_ra_to_wr0(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_ra_to_wrx(pos, n, SP_NO_SHIFT, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_lds_ra_to_wr0(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_ra_to_wrx(pos, n, SP_SHIFT, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_ld_ra_to_wr1(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_ra_to_wrx(pos, n, SP_NO_SHIFT, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_lds_ra_to_wr1(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_ra_to_wrx(pos, n, SP_SHIFT, SP_WR1);
}

/* LDx_FW_TO_WRx */
/******************************************************************************/
__COLD_CODE void sparser_gen_ld_fw_to_wr0(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_fw_to_wrx(pos, n, SP_NO_SHIFT, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_lds_fw_to_wr0(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_fw_to_wrx(pos, n, SP_SHIFT, SP_WR0);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_ld_fw_to_wr1(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_fw_to_wrx(pos, n, SP_NO_SHIFT, SP_WR1);
}

/******************************************************************************/
__COLD_CODE void sparser_gen_lds_fw_to_wr1(uint8_t pos, uint8_t n)
{
	sparser_gen_ldx_fw_to_wrx(pos, n, SP_SHIFT, SP_WR1);
}
