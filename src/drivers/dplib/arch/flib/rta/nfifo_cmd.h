
#ifndef __RTA_NFIFO_CMD_H__
#define __RTA_NFIFO_CMD_H__

extern enum rta_sec_era rta_sec_era;

static const uint32_t nfifo_src[][2] = {
/*1*/	{ _IFIFO,       NFIFOENTRY_STYPE_DFIFO },
	{ _OFIFO,       NFIFOENTRY_STYPE_OFIFO },
	{ _PAD,         NFIFOENTRY_STYPE_PAD },
/*4*/	{ _MSGOUTSNOOP, NFIFOENTRY_STYPE_SNOOP | NFIFOENTRY_DEST_BOTH },
/*5*/	{ _ALTSOURCE,   NFIFOENTRY_STYPE_ALTSOURCE },
	{ _OFIFO_SYNC,  NFIFOENTRY_STYPE_OFIFO_SYNC },
/*7*/	{ _MSGOUTSNOOP_ALT, NFIFOENTRY_STYPE_SNOOP_ALT | NFIFOENTRY_DEST_BOTH }
};

/*
 * Allowed NFIFO LOAD sources for each SEC Era.
 * Values represent the number of entries from nfifo_src[] that are supported.
 */
static const unsigned nfifo_src_sz[] = {4, 5, 5, 5, 5, 5, 5, 7};

static const uint32_t nfifo_data[][2] = {
	{ _MSG,   NFIFOENTRY_DTYPE_MSG },
	{ _MSG1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_MSG },
	{ _MSG2,  NFIFOENTRY_DEST_CLASS2 | NFIFOENTRY_DTYPE_MSG },
	{ _IV1,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_IV },
	{ _IV2,   NFIFOENTRY_DEST_CLASS2 | NFIFOENTRY_DTYPE_IV },
	{ _ICV1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_ICV },
	{ _ICV2,  NFIFOENTRY_DEST_CLASS2 | NFIFOENTRY_DTYPE_ICV },
	{ _SAD1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_SAD },
	{ _AAD1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_AAD },
	{ _AAD2,  NFIFOENTRY_DEST_CLASS2 | NFIFOENTRY_DTYPE_AAD },
	{ _AFHA_SBOX, NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_SBOX },
	{ _SKIP,  NFIFOENTRY_DTYPE_SKIP },
	{ _PKE,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_E },
	{ _PKN,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_N },
	{ _PKA,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A },
	{ _PKA0,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A0 },
	{ _PKA1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A1 },
	{ _PKA2,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A2 },
	{ _PKA3,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_A3 },
	{ _PKB,   NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B },
	{ _PKB0,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B0 },
	{ _PKB1,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B1 },
	{ _PKB2,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B2 },
	{ _PKB3,  NFIFOENTRY_DEST_CLASS1 | NFIFOENTRY_DTYPE_PK_B3 },
	{ _AB1,   NFIFOENTRY_DEST_CLASS1 },
	{ _AB2,   NFIFOENTRY_DEST_CLASS2 },
	{ _ABD,   NFIFOENTRY_DEST_DECO }
};

static const uint32_t nfifo_flags[][2] = {
/*1*/	{ LAST1,         NFIFOENTRY_LC1 },
	{ LAST2,         NFIFOENTRY_LC2 },
	{ FLUSH1,        NFIFOENTRY_FC1 },
	{ BP,            NFIFOENTRY_BND },
	{ PAD_ZERO,      NFIFOENTRY_PTYPE_ZEROS },
	{ PAD_NONZERO,   NFIFOENTRY_PTYPE_RND_NOZEROS },
	{ PAD_INCREMENT, NFIFOENTRY_PTYPE_INCREMENT },
	{ PAD_RANDOM,    NFIFOENTRY_PTYPE_RND },
	{ PAD_ZERO_N1,   NFIFOENTRY_PTYPE_ZEROS_NZ },
	{ PAD_NONZERO_0, NFIFOENTRY_PTYPE_RND_NZ_LZ },
	{ PAD_N1,        NFIFOENTRY_PTYPE_N },
/*12*/	{ PAD_NONZERO_N, NFIFOENTRY_PTYPE_RND_NZ_N },
	{ FLUSH2,        NFIFOENTRY_FC2 },
	{ OC,            NFIFOENTRY_OC }
};

/*
 * Allowed NFIFO LOAD flags for each SEC Era.
 * Values represent the number of entries from nfifo_flags[] that are supported.
 */
static const unsigned nfifo_flags_sz[] = {12, 14, 14, 14, 14, 14, 14, 14};

static const uint32_t nfifo_pad_flags[][2] = {
	{ BM, NFIFOENTRY_BM },
	{ PS, NFIFOENTRY_PS },
	{ PR, NFIFOENTRY_PR }
};

/*
 * Allowed NFIFO LOAD pad flags for each SEC Era.
 * Values represent the number of entries from nfifo_pad_flags[] that are
 * supported.
 */
static const unsigned nfifo_pad_flags_sz[] = {2, 2, 2, 2, 3, 3, 3, 3};

static inline unsigned rta_nfifo_load(struct program *program, uint32_t src,
				      int type_src, uint32_t data,
				      int type_data, uint32_t length,
				      uint32_t flags)
{
	uint32_t opcode = 0, val;
	int ret;
	uint32_t load_cmd = CMD_LOAD | LDST_IMM | LDST_CLASS_IND_CCB |
			    LDST_SRCDST_WORD_INFO_FIFO;
	unsigned start_pc = program->current_pc;

	if ((type_src != REG_TYPE) || (type_data != REG_TYPE)) {
		pr_err("NFIFO: Incorrect src / data type. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}

	if ((data == _AFHA_SBOX) && (rta_sec_era == RTA_SEC_ERA_7)) {
		pr_err("NFIFO: AFHA S-box not supported by SEC Era %d\n",
		       USER_SEC_ERA(rta_sec_era));
		goto err;
	}

	/* write source field */
	ret = __rta_map_opcode(src, nfifo_src, nfifo_src_sz[rta_sec_era], &val);
	if (ret == -1) {
		pr_err("NFIFO: Invalid SRC. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}
	opcode |= val;

	/* write type field */
	ret = __rta_map_opcode(data, nfifo_data, ARRAY_SIZE(nfifo_data), &val);
	if (ret == -1) {
		pr_err("NFIFO: Invalid data. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}
	opcode |= val;

	/* write DL field */
	if (!(flags & EXT)) {
		opcode |= length & NFIFOENTRY_DLEN_MASK;
		load_cmd |= 4;
	} else {
		load_cmd |= 8;
	}

	/* write flags */
	__rta_map_flags(flags, nfifo_flags, nfifo_flags_sz[rta_sec_era],
			&opcode);

	/* in case of padding, check the destination */
	if (src == _PAD)
		__rta_map_flags(flags, nfifo_pad_flags,
				nfifo_pad_flags_sz[rta_sec_era], &opcode);

	/* write LOAD command first */
	__rta_out32(program, load_cmd);
	__rta_out32(program, opcode);

	if (flags & EXT)
		__rta_out32(program, length & NFIFOENTRY_DLEN_MASK);

	program->current_instruction++;

	return start_pc;

 err:
	program->first_error_pc = start_pc;
	program->current_instruction++;
	return start_pc;
}

#endif /* __RTA_NFIFO_CMD_H__ */
