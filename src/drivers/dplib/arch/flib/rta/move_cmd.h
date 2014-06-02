/* Copyright 2008-2013 Freescale Semiconductor, Inc. */

#ifndef __RTA_MOVE_CMD_H__
#define __RTA_MOVE_CMD_H__

#define MOVE_SET_AUX_SRC	0x01
#define MOVE_SET_AUX_DST	0x02
#define MOVE_SET_AUX_LS		0x03
#define MOVE_SET_LEN_16b	0x04

#define MOVE_SET_AUX_MATH	0x10
#define MOVE_SET_AUX_MATH_SRC	(MOVE_SET_AUX_SRC | MOVE_SET_AUX_MATH)
#define MOVE_SET_AUX_MATH_DST	(MOVE_SET_AUX_DST | MOVE_SET_AUX_MATH)

#define MASK_16b  0xFF

extern enum rta_sec_era rta_sec_era;

static const uint32_t move_src_table[][2] = {
/*1*/	{ _CONTEXT1, MOVE_SRC_CLASS1CTX },
	{ _CONTEXT2, MOVE_SRC_CLASS2CTX },
	{ _OFIFO,    MOVE_SRC_OUTFIFO },
	{ _DESCBUF,  MOVE_SRC_DESCBUF },
	{ _MATH0,    MOVE_SRC_MATH0 },
	{ _MATH1,    MOVE_SRC_MATH1 },
	{ _MATH2,    MOVE_SRC_MATH2 },
	{ _MATH3,    MOVE_SRC_MATH3 },
/*9*/	{ _IFIFOABD, MOVE_SRC_INFIFO },
	{ _IFIFOAB1, MOVE_SRC_INFIFO_CL | MOVE_AUX_LS },
	{ _IFIFOAB2, MOVE_SRC_INFIFO_CL },
/*12*/	{ _ABD,      MOVE_SRC_INFIFO_NO_NFIFO },
	{ _AB1,      MOVE_SRC_INFIFO_NO_NFIFO | MOVE_AUX_LS },
	{ _AB2,      MOVE_SRC_INFIFO_NO_NFIFO | MOVE_AUX_MS }
};

/* Allowed MOVE / MOVE_LEN sources for each SEC Era.
 * Values represent the number of entries from move_src_table[] that are
 * supported.
 */
static const unsigned move_src_table_sz[] = {9, 11, 14, 14, 14, 14, 14, 14};

static const uint32_t move_dst_table[][2] = {
/*1*/	{ _CONTEXT1,  MOVE_DEST_CLASS1CTX },
	{ _CONTEXT2,  MOVE_DEST_CLASS2CTX },
	{ _OFIFO,     MOVE_DEST_OUTFIFO },
	{ _DESCBUF,   MOVE_DEST_DESCBUF },
	{ _MATH0,     MOVE_DEST_MATH0 },
	{ _MATH1,     MOVE_DEST_MATH1 },
	{ _MATH2,     MOVE_DEST_MATH2 },
	{ _MATH3,     MOVE_DEST_MATH3 },
	{ _IFIFOAB1,  MOVE_DEST_CLASS1INFIFO },
	{ _IFIFOAB2,  MOVE_DEST_CLASS2INFIFO },
	{ _PKA,       MOVE_DEST_PK_A },
	{ _KEY1,      MOVE_DEST_CLASS1KEY },
	{ _KEY2,      MOVE_DEST_CLASS2KEY },
/*14*/	{ _IFIFO,     MOVE_DEST_INFIFO },
/*15*/	{_ALTSOURCE,  MOVE_DEST_ALTSOURCE}
};

/* Allowed MOVE / MOVE_LEN destinations for each SEC Era.
 * Values represent the number of entries from move_dst_table[] that are
 * supported.
 */
static const unsigned move_dst_table_sz[] = {13, 14, 14, 15, 15, 15, 15, 15};

static inline int set_move_offset(struct program *program, uint64_t src,
				  uint16_t src_offset, uint64_t dst,
				  uint16_t dst_offset, uint16_t *offset,
				  uint16_t *opt);

static inline int math_offset(uint16_t offset);

static inline unsigned rta_move(struct program *program, int cmd_type,
				uint64_t src, int type_src, uint16_t src_offset,
				uint64_t dst, int type_dst, uint16_t dst_offset,
				uint32_t length, int type_length,
				uint32_t flags)
{
	uint32_t opcode = 0;
	uint16_t offset = 0, opt = 0;
	uint32_t val = 0;
	int ret, is_move_len_cmd = 0;
	unsigned start_pc = program->current_pc;

	if ((type_src != REG_TYPE) || (type_dst != REG_TYPE)) {
		pr_err("MOVE: Incorrect src / dst type. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}

	if ((rta_sec_era < RTA_SEC_ERA_7) && (cmd_type != __MOVE)) {
		pr_err("MOVE: MOVEB / MOVEDW not supported by SEC Era %d. SEC PC: %d; Instr: %d\n",
		       USER_SEC_ERA(rta_sec_era), program->current_pc,
		       program->current_instruction);
		goto err;
	}

	/* write command type */
	if (cmd_type == __MOVEB) {
		opcode = CMD_MOVEB;
	} else if (cmd_type == __MOVEDW) {
		opcode = CMD_MOVEDW;
	} else if (type_length == REG_TYPE) {
		if (rta_sec_era < RTA_SEC_ERA_3) {
			pr_err("MOVE: MOVE_LEN not supported by SEC Era %d. SEC PC: %d; Instr: %d\n",
			       USER_SEC_ERA(rta_sec_era), program->current_pc,
			       program->current_instruction);
			goto err;
		}

		if ((length != _MATH0) && (length != _MATH1) &&
		    (length != _MATH2) && (length != _MATH3)) {
			pr_err("MOVE: MOVE_LEN length must be MATH[0-3]. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		}

		opcode = CMD_MOVE_LEN;
		is_move_len_cmd = 1;
	} else {
		opcode = CMD_MOVE;
	}

	/* write offset first, to check for invalid combinations or incorrect
	 * offset values sooner; decide which offset should be here
	 * (src or dst)
	 */
	ret = set_move_offset(program, src, src_offset, dst, dst_offset,
			      &offset, &opt);
	if (ret)
		goto err;

	opcode |= (offset << MOVE_OFFSET_SHIFT) & MOVE_OFFSET_MASK;

	/* set AUX field if required */
	if (opt == MOVE_SET_AUX_SRC)
		opcode |= ((src_offset / 16) << MOVE_AUX_SHIFT) & MOVE_AUX_MASK;
	else if (opt == MOVE_SET_AUX_DST)
		opcode |= ((dst_offset / 16) << MOVE_AUX_SHIFT) & MOVE_AUX_MASK;
	else if (opt == MOVE_SET_AUX_LS)
		opcode |= MOVE_AUX_LS;
	else if (opt & MOVE_SET_AUX_MATH) {
		if (opt & MOVE_SET_AUX_SRC)
			offset = src_offset;
		else
			offset = dst_offset;

		if (rta_sec_era < RTA_SEC_ERA_6) {
			if (offset)
				pr_debug("MOVE: Offset not supported by SEC Era %d. SEC PC: %d; Instr: %d\n",
					 USER_SEC_ERA(rta_sec_era),
					 program->current_pc,
					 program->current_instruction);
			/* nothing to do for offset = 0 */
		} else {
			ret = math_offset(offset);
			if (ret == -1) {
				pr_err("MOVE: Invalid offset in MATH register. SEC PC: %d; Instr: %d\n",
				       program->current_pc,
				       program->current_instruction);
				goto err;
			}

			opcode |= (uint32_t)ret;
		}
	}

	/* write source field */
	ret = __rta_map_opcode((uint32_t)src, move_src_table,
			       move_src_table_sz[rta_sec_era], &val);
	if (ret == -1) {
		pr_err("MOVE: Invalid SRC. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}
	opcode |= val;

	/* write destination field */
	ret = __rta_map_opcode((uint32_t)dst, move_dst_table,
			       move_dst_table_sz[rta_sec_era], &val);
	if (ret == -1) {
		pr_err("MOVE: Invalid DST. SEC PC: %d; Instr: %d\n",
		       program->current_pc, program->current_instruction);
		goto err;
	}
	opcode |= val;

	/* write flags */
	if (flags & (FLUSH1 | FLUSH2))
		opcode |= MOVE_AUX_MS;
	if (flags & (LAST2 | LAST1))
		opcode |= MOVE_AUX_LS;
	if (flags & WAITCOMP)
		opcode |= MOVE_WAITCOMP;

	if (!is_move_len_cmd) {
		/* write length */
		if (opt == MOVE_SET_LEN_16b)
			opcode |= (length & (MOVE_OFFSET_MASK | MOVE_LEN_MASK));
		else
			opcode |= (length & MOVE_LEN_MASK);
	} else {
		/* write mrsel */
		switch (length) {
		case (_MATH0):
			/*
			 * opcode |= MOVELEN_MRSEL_MATH0;
			 * MOVELEN_MRSEL_MATH0 is 0
			 */
			break;
		case (_MATH1):
			opcode |= MOVELEN_MRSEL_MATH1;
			break;
		case (_MATH2):
			opcode |= MOVELEN_MRSEL_MATH2;
			break;
		case (_MATH3):
			opcode |= MOVELEN_MRSEL_MATH3;
			break;
		}

		/* write size */
		if (rta_sec_era >= RTA_SEC_ERA_7) {
			if (flags & SIZE_WORD)
				opcode |= MOVELEN_SIZE_WORD;
			else if (flags & SIZE_BYTE)
				opcode |= MOVELEN_SIZE_BYTE;
			else if (flags & SIZE_DWORD)
				opcode |= MOVELEN_SIZE_DWORD;
		}
	}

	__rta_out32(program, opcode);
	program->current_instruction++;

	return start_pc;

 err:
	program->first_error_pc = start_pc;
	program->current_instruction++;
	return start_pc;
}

static inline int set_move_offset(struct program *program, uint64_t src,
				  uint16_t src_offset, uint64_t dst,
				  uint16_t dst_offset, uint16_t *offset,
				  uint16_t *opt)
{
	switch (src) {
	case (_CONTEXT1):
	case (_CONTEXT2):
		if (dst == _DESCBUF) {
			*opt = MOVE_SET_AUX_SRC;
			*offset = dst_offset;
		} else if ((dst == _KEY1) || (dst == _KEY2)) {
			if ((src_offset) && (dst_offset)) {
				pr_err("MOVE: Bad offset. SEC PC: %d; Instr: %d\n",
				       program->current_pc,
				       program->current_instruction);
				goto err;
			}
			if (dst_offset) {
				*opt = MOVE_SET_AUX_LS;
				*offset = dst_offset;
			} else {
				*offset = src_offset;
			}
		} else {
			if ((dst == _MATH0) || (dst == _MATH1) ||
			    (dst == _MATH2) || (dst == _MATH3)) {
				*opt = MOVE_SET_AUX_MATH_DST;
			} else if (((dst == _OFIFO) || (dst == _ALTSOURCE)) &&
			    (src_offset % 4)) {
				pr_err("MOVE: Bad offset alignment. SEC PC: %d; Instr: %d\n",
				       program->current_pc,
				       program->current_instruction);
				goto err;
			}

			*offset = src_offset;
		}
		break;

	case (_OFIFO):
		if (dst == _OFIFO) {
			pr_err("MOVE: Invalid DST. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		}
		if (((dst == _IFIFOAB1) || (dst == _IFIFOAB2) ||
		     (dst == _IFIFO) || (dst == _PKA)) &&
		    (src_offset || dst_offset)) {
			pr_err("MOVE: Offset should be zero. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		}
		*offset = dst_offset;
		break;

	case (_DESCBUF):
		if ((dst == _CONTEXT1) || (dst == _CONTEXT2)) {
			*opt = MOVE_SET_AUX_DST;
		} else if ((dst == _MATH0) || (dst == _MATH1) ||
			   (dst == _MATH2) || (dst == _MATH3)) {
			*opt = MOVE_SET_AUX_MATH_DST;
		} else if (dst == _DESCBUF) {
			pr_err("MOVE: Invalid DST. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		} else if (((dst == _OFIFO) || (dst == _ALTSOURCE)) &&
		    (src_offset % 4)) {
			pr_err("MOVE: Invalid offset alignment. SEC PC: %d; Instr %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		}

		*offset = src_offset;
		break;

	case (_MATH0):
	case (_MATH1):
	case (_MATH2):
	case (_MATH3):
		if ((dst == _OFIFO) || (dst == _ALTSOURCE)) {
			if (src_offset % 4) {
				pr_err("MOVE: Bad offset alignment. SEC PC: %d; Instr: %d\n",
				       program->current_pc,
				       program->current_instruction);
				goto err;
			}
			*offset = src_offset;
		} else if ((dst == _IFIFOAB1) || (dst == _IFIFOAB2) ||
			   (dst == _IFIFO) || (dst == _PKA)) {
			*offset = src_offset;
		} else {
			*offset = dst_offset;

			/*
			 * This condition is basically the negation of:
			 * dst in { _CONTEXT[1-2], _MATH[0-3] }
			 */
			if ((dst != _KEY1) && (dst != _KEY2))
				*opt = MOVE_SET_AUX_MATH_SRC;
		}
		break;

	case (_IFIFOABD):
	case (_IFIFOAB1):
	case (_IFIFOAB2):
	case (_ABD):
	case (_AB1):
	case (_AB2):
		if ((dst == _IFIFOAB1) || (dst == _IFIFOAB2) ||
		    (dst == _IFIFO) || (dst == _PKA) || (dst == _ALTSOURCE)) {
			pr_err("MOVE: Bad DST. SEC PC: %d; Instr: %d\n",
			       program->current_pc,
			       program->current_instruction);
			goto err;
		} else {
			if (dst == _OFIFO) {
				*opt = MOVE_SET_LEN_16b;
			} else {
				if (dst_offset % 4) {
					pr_err("MOVE: Bad offset alignment. SEC PC: %d; Instr: %d\n",
					       program->current_pc,
					       program->current_instruction);
					goto err;
				}
				*offset = dst_offset;
			}
		}
		break;
	default:
		break;
	}

	return 0;
 err:
	return -1;
}

static inline int math_offset(uint16_t offset)
{
	switch (offset) {
	case 0:
		return 0;
	case 4:
		return MOVE_AUX_LS;
	case 6:
		return MOVE_AUX_MS;
	case 7:
		return MOVE_AUX_LS | MOVE_AUX_MS;
	}

	return -1;
}

#endif /* __RTA_MOVE_CMD_H__ */
