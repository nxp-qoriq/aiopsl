/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_STORE_CMD_H__
#define __RTA_STORE_CMD_H__

extern enum rta_sec_era rta_sec_era;

static const uint32_t store_src_table[][2] = {
/*1*/	{ _KEY1SZ,       LDST_CLASS_1_CCB | LDST_SRCDST_WORD_KEYSZ_REG },
	{ _KEY2SZ,       LDST_CLASS_2_CCB | LDST_SRCDST_WORD_KEYSZ_REG },
	{ _DJQDA,        LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_JQDAR },
	{ _MODE1,        LDST_CLASS_1_CCB | LDST_SRCDST_WORD_MODE_REG },
	{ _MODE2,        LDST_CLASS_2_CCB | LDST_SRCDST_WORD_MODE_REG },
	{ _DJQCTRL,      LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_JQCTRL },
	{ _DATA1SZ,      LDST_CLASS_1_CCB | LDST_SRCDST_WORD_DATASZ_REG },
	{ _DATA2SZ,      LDST_CLASS_2_CCB | LDST_SRCDST_WORD_DATASZ_REG },
	{ _DSTAT,        LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_STAT },
	{ _ICV1SZ,       LDST_CLASS_1_CCB | LDST_SRCDST_WORD_ICVSZ_REG },
	{ _ICV2SZ,       LDST_CLASS_2_CCB | LDST_SRCDST_WORD_ICVSZ_REG },
	{ _DPID,         LDST_CLASS_DECO | LDST_SRCDST_WORD_PID },
	{ _CCTRL,        LDST_SRCDST_WORD_CHACTRL },
	{ _ICTRL,        LDST_SRCDST_WORD_IRQCTRL },
	{ _CLRW,         LDST_SRCDST_WORD_CLRW },
	{ _MATH0,        LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_MATH0 },
	{ _CSTAT,        LDST_SRCDST_WORD_STAT },
	{ _MATH1,        LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_MATH1 },
	{ _MATH2,        LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_MATH2 },
	{ _AAD1SZ,       LDST_CLASS_1_CCB | LDST_SRCDST_WORD_DECO_AAD_SZ },
	{ _MATH3,        LDST_CLASS_DECO | LDST_SRCDST_WORD_DECO_MATH3 },
	{ _IV1SZ,        LDST_CLASS_1_CCB | LDST_SRCDST_WORD_CLASS1_IV_SZ },
	{ _PKASZ,        LDST_CLASS_1_CCB | LDST_SRCDST_WORD_PKHA_A_SZ },
	{ _PKBSZ,        LDST_CLASS_1_CCB | LDST_SRCDST_WORD_PKHA_B_SZ },
	{ _PKESZ,        LDST_CLASS_1_CCB | LDST_SRCDST_WORD_PKHA_E_SZ },
	{ _PKNSZ,        LDST_CLASS_1_CCB | LDST_SRCDST_WORD_PKHA_N_SZ },
	{ _CONTEXT1,     LDST_CLASS_1_CCB | LDST_SRCDST_BYTE_CONTEXT },
	{ _CONTEXT2,     LDST_CLASS_2_CCB | LDST_SRCDST_BYTE_CONTEXT },
	{ _DESCBUF,      LDST_CLASS_DECO | LDST_SRCDST_WORD_DESCBUF },
/*30*/	{ _JOBDESCBUF,   LDST_CLASS_DECO | LDST_SRCDST_WORD_DESCBUF_JOB },
	{ _SHAREDESCBUF, LDST_CLASS_DECO | LDST_SRCDST_WORD_DESCBUF_SHARED },
/*32*/	{ _JOBDESCBUF_EFF,   LDST_CLASS_DECO |
		LDST_SRCDST_WORD_DESCBUF_JOB_WE },
	{ _SHAREDESCBUF_EFF, LDST_CLASS_DECO |
		LDST_SRCDST_WORD_DESCBUF_SHARED_WE },
/*34*/	{ _GTR,          LDST_CLASS_DECO | LDST_SRCDST_WORD_GTR },
	{ _STR,          LDST_CLASS_DECO | LDST_SRCDST_WORD_STR }
};

/*
 * Allowed STORE sources for each SEC ERA.
 * Values represent the number of entries from source_src_table[] that are
 * supported.
 */
static const unsigned store_src_table_sz[] = {29, 31, 33, 33, 33, 33, 35, 35};

static inline unsigned rta_store(struct program *program, uint64_t src,
				 int type_src, uint16_t offset, uint64_t dst,
				 int type_dst, uint32_t length, uint32_t flags)
{
	uint32_t opcode = 0, val;
	int ret;
	unsigned start_pc = program->current_pc;

	if (flags & SEQ)
		opcode = CMD_SEQ_STORE;
	else
		opcode = CMD_STORE;

	if (type_src == IMM_DATA)
		flags |= IMMED;

	/* parameters check */
	if ((flags & IMMED) && (flags & SGF)) {
		pr_err("STORE: Invalid flag. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}
	if ((flags & IMMED) && (offset != 0)) {
		pr_err("STORE: Invalid flag. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}

	if ((flags & SEQ) && ((src == _JOBDESCBUF) || (src == _SHAREDESCBUF) ||
			      (src == _JOBDESCBUF_EFF) ||
			      (src == _SHAREDESCBUF_EFF))) {
		pr_err("STORE: Invalid SRC type. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}

	if (flags & IMMED)
		opcode |= LDST_IMM;

	if ((flags & SGF) || (flags & VLF))
		opcode |= LDST_VLF;

	/* source for data to be stored can be specified as:
	 *    - register location; set in src field[9-15];
	 *    - if immed flag is set, data is set in value field [0-31];
	 *      user can give this value as actual value or pointer to data;
	 */
	if (type_src == REG_TYPE) {
		ret = __rta_map_opcode((uint32_t)src, store_src_table,
				       store_src_table_sz[rta_sec_era], &val);
		if (ret == -1) {
			pr_err("STORE: Invalid source. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		}
		opcode |= val;
	}

	/* DESC BUFFER: length / offset values are specified in 4-byte words */
	if ((src == _DESCBUF) || (src == _JOBDESCBUF) ||
	    (src == _SHAREDESCBUF) || (src == _JOBDESCBUF_EFF) ||
	    (src == _SHAREDESCBUF_EFF)) {
		opcode |= (length >> 2);
		opcode |= (uint32_t)((offset >> 2) << LDST_OFFSET_SHIFT);
	} else {
		opcode |= length;
		opcode |= (uint32_t)(offset << LDST_OFFSET_SHIFT);
	}

	__rta_out32(program, opcode);
	program->current_instruction++;

	if ((src == _JOBDESCBUF) || (src == _SHAREDESCBUF) ||
	    (src == _JOBDESCBUF_EFF) || (src == _SHAREDESCBUF_EFF))
		return start_pc;

	/* for STORE, a pointer to where the data will be stored is needed */
	if (!(flags & SEQ)) {
		if (type_dst != PTR_DATA) {
			pr_err("STORE: Invalid dst type. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		}
		__rta_out64(program, program->ps, dst);
	}

	/* for imm data, place the data here */
	if (flags & IMMED)
		__rta_inline_data(program, src, type_src, length);

	return start_pc;

 err:
	program->first_error_pc = start_pc;
	program->current_instruction++;
	return start_pc;
}

#endif /* __RTA_STORE_CMD_H__ */
