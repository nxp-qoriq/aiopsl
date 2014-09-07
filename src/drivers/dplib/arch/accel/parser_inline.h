#include "parser.h"
#include "general.h"
#include "common/types.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_parser.h"

#include "system.h"
#include "id_pool.h"

inline int parse_result_generate(enum parser_starting_hxs_code starting_hxs,
	uint8_t starting_offset, uint8_t flags)
{
	uint32_t arg1, arg2, arg3, arg4;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	/* 8 Byte aligned for stqw optimization */
	struct parser_input_message_params input_struct
				__attribute__((aligned(16)));

	arg1 = (uint32_t)default_task_params.parser_profile_id;
	__e_rlwimi(arg1, (uint32_t)flags, 8, 16, 23);
	__e_rlwimi(arg1, (uint32_t)starting_hxs, 13, 8, 18);
	__e_rlwimi(arg1, (uint32_t)starting_offset, 24, 0, 7);

	/* If Gross Running Sum != 0 then it is valid */
	if (pr->gross_running_sum) {
		arg3=0;
		arg4=0;
		       
		__stdw(arg3, arg4, 0, &input_struct);
		__stdw(arg3, arg4, 8, &input_struct);
		__stdw(arg3, arg4, 16, &input_struct);

		input_struct.gross_running_sum = pr->gross_running_sum;
		arg2 = ((uint32_t)(&input_struct) << 16) |
				(uint32_t)HWC_PARSE_RES_ADDRESS;
		__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
	} else {
		/* If L4 checksum validation is required, calculate it first */
		if (flags & PARSER_VALIDATE_L4_CHECKSUM) {
			arg3=0;
			arg4=0;
			       
			__stdw(arg3, arg4, 0, &input_struct);
			__stdw(arg3, arg4, 8, &input_struct);
			__stdw(arg3, arg4, 16, &input_struct);

			fdma_calculate_default_frame_checksum(0, 0xFFFF,
						&pr->gross_running_sum);
			input_struct.gross_running_sum = pr->gross_running_sum;
			arg2 = ((uint32_t)(&input_struct) << 16) |
				(uint32_t)HWC_PARSE_RES_ADDRESS;
			__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
		} else {
			/* Gross Running Sum == 0 and validation is not
			 * required */
			__stqw(PARSER_GEN_PARSE_RES_MTYPE,
			(uint32_t)HWC_PARSE_RES_ADDRESS, 0, arg1,
			HWC_ACC_IN_ADDRESS, 0);
		}
	}

	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
	if (status == PARSER_HW_STATUS_SUCCESS) {
		return 0;
	} else if ((status ==
			PARSER_HW_STATUS_L3_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L4_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L3_L4_CHECKSUM_VALIDATION_SUCCEEDED)) {
		return 0;
	} else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
		return (-1);
	} else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE,
			__LINE__, 
			(status & 
			PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION));
		return (-1);
	} else if (status & PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED) {
		return -ENOSPC;
	} else {
		return -EIO;
	}
}

inline int parse_result_generate_default(uint8_t flags)
{
	uint32_t arg1, arg2, arg3, arg4;
	int32_t status;
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	struct parser_input_message_params input_struct
					__attribute__((aligned(16)));

	arg1 = (uint32_t)default_task_params.parser_profile_id;
	__e_rlwimi(arg1, (uint32_t)flags, 8, 16, 23);
	__e_rlwimi(arg1, (uint32_t)default_task_params.parser_starting_hxs, 13,
			8, 18);

	/* If Gross Running Sum != 0 then it is valid */
	if (pr->gross_running_sum) {
		arg3=0;
		arg4=0;
		       
		__stdw(arg3, arg4, 0, &input_struct);
		__stdw(arg3, arg4, 8, &input_struct);
		__stdw(arg3, arg4, 16, &input_struct);

		input_struct.gross_running_sum = pr->gross_running_sum;

		arg2 = ((uint32_t)(&input_struct) << 16) |
				(uint32_t)HWC_PARSE_RES_ADDRESS;

		__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
	} else {
		/* If L4 checksum validation is required, calculate it first */
		if (flags & PARSER_VALIDATE_L4_CHECKSUM) {
			arg3=0;
			arg4=0;
			       
			__stdw(arg3, arg4, 0, &input_struct);
			__stdw(arg3, arg4, 8, &input_struct);
			__stdw(arg3, arg4, 16, &input_struct);

			fdma_calculate_default_frame_checksum(0, 0xFFFF,
						&pr->gross_running_sum);
			input_struct.gross_running_sum = pr->gross_running_sum;
			arg2 = ((uint32_t)(&input_struct) << 16) |
				(uint32_t)HWC_PARSE_RES_ADDRESS;
			__stqw((PARSER_GRSV_MASK | PARSER_GEN_PARSE_RES_MTYPE),
				arg2, 0, arg1, HWC_ACC_IN_ADDRESS, 0);
		} else {
			/* Gross Running Sum == 0 and validation is not
			 * required */
			__stqw(PARSER_GEN_PARSE_RES_MTYPE,
			(uint32_t)HWC_PARSE_RES_ADDRESS, 0, arg1,
			HWC_ACC_IN_ADDRESS, 0);
		}
	}
	
	__e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

	status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
	/* implementation of errors is priority based (if-else) since multiple
	 * error indications may appear at the same time */
	if (status == PARSER_HW_STATUS_SUCCESS) {
		return 0;
	} else if ((status ==
			PARSER_HW_STATUS_L3_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L4_CHECKSUM_VALIDATION_SUCCEEDED) ||
		(status ==
			PARSER_HW_STATUS_L3_L4_CHECKSUM_VALIDATION_SUCCEEDED)) {
		return 0;
	} else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE_DEFAULT,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
		return (-1);
	} else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE_DEFAULT,
			__LINE__, 
			(status & 
			PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION));
		return (-1);
	} else if (status & PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED) {
		return -ENOSPC;
	} else {
		return -EIO;
	}
}

inline int parse_result_generate_basic(void)
{
       int32_t status;

       __stqw(PARSER_GEN_PARSE_RES_MTYPE,
             (uint32_t)HWC_PARSE_RES_ADDRESS, 0, 0,
             HWC_ACC_IN_ADDRESS, 0);

       __e_hwacceli(CTLU_PARSE_CLASSIFY_ACCEL_ID);

       status = *(int32_t *)HWC_ACC_OUT_ADDRESS;
       if (status == PARSER_HW_STATUS_SUCCESS) {
             return 0;
       } else if (status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED) {
		parser_exception_handler(PARSE_RESULT_GENERATE_BASIC,
			__LINE__, 
			(status & PARSER_HW_STATUS_CYCLE_LIMIT_EXCCEEDED));
             return (-1);
       } else if (status & PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION) {
		parser_exception_handler(PARSE_RESULT_GENERATE_BASIC,
			__LINE__, 
			(status & 
			PARSER_HW_STATUS_INVALID_SOFT_PARSE_INSTRUCTION));
             return (-1);
       } else if (status & PARSER_HW_STATUS_BLOCK_LIMIT_EXCCEEDED) {
             return -ENOSPC;
       } else {
             return -EIO;
       }
}


