/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
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

/**************************************************************************//**
@File		aiop_verification.h

@Description	This file contains the AIOP SW Verification Structures

*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_H_
#define __AIOP_VERIFICATION_H_

#include "aiop_verification_data.h"
#include "general.h"
#include "fsl_ipsec.h"
#include "aiop_verification_fdma.h"
#include "aiop_verification_tman.h"
#include "aiop_verification_ste.h"
#include "aiop_verification_cdma.h"
#include "aiop_verification_keygen.h"
#include "aiop_verification_table.h"
#include "aiop_verification_parser.h"
#include "aiop_verification_hm.h"
#include "aiop_verification_gso.h"
#include "aiop_verification_gro.h"
#include "aiop_verification_osm.h"
#include "aiop_verification_ipf.h"
#include "aiop_verification_ipr.h"
#include "aiop_verification_ipsec.h"
#include "fsl_ip.h"
#include "fsl_ipsec.h"
#include "fsl_l2.h"
#include "fsl_l4.h"
#include "fsl_nat.h"
#include "fsl_osm.h"
#include "fsl_dpni_drv.h"
#include "fsl_dpni.h"
#include "fsl_dpni_cmd.h"


	/**< ACCEL_ID cmd mask */
#define ACCEL_ID_CMD_MASK		0xFFFF0000

	/**< AIOP Terminate Flow Verification command structure identifier */
#define AIOP_TERMINATE_FLOW_CMD_STR	(TERMINATE_FLOW_MODULE << 16)
	/**< AIOP IF Verification command structure identifier */
#define AIOP_IF_CMD_STR			(IF_MODULE << 16)
	/**< AIOP IF ELSE command structure identifier */
#define AIOP_IF_ELSE_CMD_STR		(IF_ELSE_MODULE << 16)

	/**< Struct size error code */
#define STR_SIZE_ERR			0xFFFF
	/**< Struct size error code */
#define STR_SIZE_BIG			0xFFFE
	/**< Buffer Data chunk size in bytes.
	 *   Must be > 64 bytes*/
#define DATA_SIZE	448
	/**< Buffer Data chunk address in workspace. */
#define WS_DATA_ADDR	0x100
	/**< IPF Fragment's fragmentation commands mask */
#define IPF_FRAGMENT_FRAGMENTATION_COMMANDS_MASK 0x00000010


/* E200-AIOP special regs */

/* Number of tasks as they defined by CTSCSR register. */
#ifndef CTSCSR_ENABLE
#define CTSCSR_ENABLE 0x80000000
#endif

#ifndef CTSCSR_1_TASKS
#define CTSCSR_1_TASKS 0
#endif

#ifndef CTSCSR_2_TASKS
#define CTSCSR_2_TASKS (1 << 24)
#endif

#ifndef CTSCSR_4_TASKS
#define CTSCSR_4_TASKS (2 << 24)
#endif

#ifndef CTSCSR_8_TASKS
#define CTSCSR_8_TASKS (3 << 24)
#endif

#ifndef CTSCSR_16_TASKS
#define CTSCSR_16_TASKS (4 << 24)
#endif

#ifndef CTSCSR_TASKS_MASK
#define CTSCSR_TASKS_MASK (CTSCSR_2_TASKS | CTSCSR_4_TASKS | CTSCSR_8_TASKS \
              | CTSCSR_16_TASKS)
#endif

#define __getctscsr0(_res)                      \
       asm ("mfdcr %[result], 464\n"                   \
              :[result]"=r"(_res)               \
              );

#define __setctscsr0(_val)                      \
       asm ("mtdcr 464, %[value]\n"                    \
              :[value]"=r"(_val)                \
              );

#define GET_CTSCSR0()                                  \
       (uint32_t)({register uint32_t __rR;             \
       uint32_t temp;                                  \
       __getctscsr0(temp);                      \
       __rR = temp; })

#define SET_CTSCSR0(_val)                       \
       __setctscsr0(_val);

/**************************************************************************//**
@Group	FSL_VERIF_FATAL_FLAGS_DEFINES Verification Fatal CMD Defines
@{
*//***************************************************************************/
/* Flags are currently not used */
/** @} */ /* end of FSL_VERIF_FATAL_FLAGS_DEFINES */

/**************************************************************************//**
 @Group		AIOP_Verification

 @Description	AIOP Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		AIOP_Service_Routines_Verification

 @Description	AIOP Service Routines Verification structures definitions.

 @{
*//***************************************************************************/


/** @} */ /* end of AIOP_Service_Routines_Verification */


/**************************************************************************//**
 @Group		AIOP_FMs_Verification

 @Description	AIOP Applications Verification structures definitions.

 @note		All structures should be aligned to 64bit

 @{
*//***************************************************************************/


/*GSO, IPF, GRO, IPR, ...*/


/** @} */ /* end of AIOP_FMs_Verification */


/**************************************************************************//**
 @enum verif_modules_ids

 @Description	AIOP verification Modules IDs enumeration.

 @{
*//***************************************************************************/
enum verif_modules_ids {
	GSO_MODULE = 0,
	GRO_MODULE, 	// 1
	IPR_MODULE, 	// 2
	IPF_MODULE, 	// 3
	FDMA_MODULE, 	// 4
	TMAN_MODULE, 	// 5
	STE_MODULE, 	// 6
	CDMA_MODULE, 	// 7
	TABLE_MODULE, 	//  8
	PARSE_MODULE, 	// 9
	HM_MODULE, 	// 10
	VPOOL_MODULE, 	// 11
	IF_MODULE, 	// 12
	IF_ELSE_MODULE, // 13
	TERMINATE_FLOW_MODULE, 	// 14
	KEYGEN_MODULE, 	// 15
	IPSEC_MODULE, 	// 16
	WRITE_DATA_TO_WS_MODULE,// 17
	UPDATE_ASA_VARIABLE, 	// 18
	OSM_MODULE, 	// 19
	EXCEPTION_MODULE, 	// 20
	UPDATE_EXT_VARIABLE, 	// 21
	UPDATE_DEFAULT_SP_ASAR,
	UPDATE_DEFAULT_SP_PTAR
};

/**************************************************************************//**
 @enum cond_ids

 @Description	AIOP verification enumeration of if-statement condition IDs.

 @{
*//***************************************************************************/
enum cond_ids {
		/** Equal condition. */
	COND_EQUAL = 0,
		/** Non Equal condition. */
	COND_NON_EQUAL
};

/**************************************************************************//**
 @enum compared_variable_ids

 @Description	AIOP verification enumeration of the compared variable IDs.

 @{
*//***************************************************************************/
enum compared_variable_ids {
		/** Compare GRO last status. */
	COMPARE_GRO_STATUS = 0,
		/** Compare GSO last status. */
	COMPARE_GSO_STATUS,
		/** Compare IPR last status. */
	COMPARE_IPR_STATUS,
		/** Compare IPF last status. */
	COMPARE_IPF_STATUS,
	/** Compare IPF last status. */
	COMPARE_LAST_STATUS
};

/**************************************************************************//**
 @enum compared_variable_size

 @Description	AIOP verification enumeration of the compared variable sizes.

 @{
*//***************************************************************************/
enum compared_variable_size {
		/** Compare 1 byte. */
	COMPARE_1BYTE = 0,
		/** Compare 2 byte. */
	COMPARE_2BYTE,
		/** Compare 4 byte. */
	COMPARE_4BYTE,
		/** Compare 8 byte. */
	COMPARE_8BYTE
};

/**************************************************************************//**
 @enum arith_operation

 @Description	AIOP verification enumeration of the arithmentical operation.

 @{
*//***************************************************************************/
enum arith_operation {
		/** Increment operation. */
	INCREMENT_OPER = 0,
		/** Decrement operation. */
	DECREMENT_OPER,
		/** Set operation. */
	SET_OPER
};

/**************************************************************************//**
@Description	AIOP IF Verification Command structure.

		This command generates an if statement in the verification
		process.

*//***************************************************************************/
struct aiop_if_verif_command {
		/** AIOP Verification IF command structure identifier. */
	uint32_t opcode;
		/** Size of the compared task variable.
		 * Please see \ref cond_ids for more details. */
	uint32_t compared_size;
		/** Compared value.
		 * This value will be compared to a variable chosen according to
		 * the compared_variable_addr. */
	int64_t compared_value;
		/** Workspace address of the compared task variable. */
	uint16_t compared_variable_addr;
		/** An offset from the beginning of the commands buffer to the
		 * command to be executed in case of a TRUE result in the IF
		 * statement. */
	uint16_t true_cmd_offset;
		/** Condition to be checked in the if statement.
		* Please see \ref cond_ids for more details. */
	uint8_t cond;
		/** 64-bit alignment. */
	uint8_t	pad[3];
};

/**************************************************************************//**
@Description	AIOP IF-ELSE Verification Command structure.

		This command generates an if-else statement in the verification
		process.

*//***************************************************************************/
struct aiop_if_else_verif_command {
		/** AIOP Verification IF command structure identifier. */
	uint32_t opcode;
		/** Size of the compared task variable.
		 * Please see \ref cond_ids for more details. */
	uint32_t compared_size;
		/** Compared value.
		 * This value will be compared to a variable chosen according to
		 * the compared_variable_addr. */
	int64_t compared_value;
		/** Workspace address of the compared task variable. */
	uint16_t compared_variable_addr;
		/** An offset from the beginning of the commands buffer to the
		 * command to be executed in case of a TRUE result in the IF
		 * statement. */
	uint16_t true_cmd_offset;
		/** An offset from the beginning of the commands buffer to the
		 * command to be executed in case of a FALSE result in the IF
		 * statement. */
	uint16_t false_cmd_offset;
		/** Condition to be checked in the if statement. */
	uint8_t cond;
		/** 64-bit alignment. */
	uint8_t	pad[1];
};

/**************************************************************************//**
@Description	AIOP Update ASA Variable Command structure.

		This command generates an update ASA variable command in the
		verification process.

*//***************************************************************************/
struct update_asa_variable_command {
		/** AIOP Verification Update ASA variable command structure
		 * identifier. */
	uint32_t opcode;
		/** ASA offset to the changed variable. */
	uint16_t asa_offset;
		/** Value to be added to the changed variable. */
	uint16_t value;
};

/**************************************************************************//**
@Description	AIOP Terminate Flow Verification Command structure.

		This command ends the verification process.

*//***************************************************************************/
struct aiop_terminate_flow_command {
		/** AIOP Terminate Flow Verification command structure
		 * identifier. */
	uint32_t opcode;
		/** 64-bit alignment. */
	uint8_t	pad[4];
};

/**************************************************************************//**
@Description	Write Data to workspace Command structure.

		Includes information needed for writing Data to workspace (as a
		preceding step for a replace command).

*//***************************************************************************/
struct write_data_to_workspace_command {
		/** Write Data to workspace command structure identifier. */
	uint32_t opcode;
		/** Pointer to the workspace start location of represented
		* segment (relevant if (flags == \ref
		* FDMA_REPLACE_SA_REPRESENT_BIT)). */
	uint32_t ws_dst_rs;
		/** Replacing Data. If size > 24 then the replacing data will
		 * be wrapped. */
	uint8_t	data[32];
		/** Data size. */
	uint16_t size;
		/** Command returned status. */
	int8_t	status;
		/** 64-bit alignment. */
	uint8_t	pad[5];
};

/**************************************************************************//**
@Description	Update default SP ASAR Command structure.

*//***************************************************************************/
struct update_default_sp_asar_command {
		/** Update default SP ASAR command structure identifier. */
	uint32_t opcode;
		/** ASAR value to update (in 64 bytes units)
		 * valid values: 0x0 - 0xF */
	uint32_t asar_val;
};

/**************************************************************************//**
@Description	Update default SP PTAR Command structure.

*//***************************************************************************/
struct update_default_sp_ptar_command {
		/** Update default SP PTAR command structure identifier. */
	uint32_t opcode;
		/** PTAR value to update 
		 * valid values: 0, 1 */
	uint32_t ptar_val;
};

/**************************************************************************//**
@Description	Fatal Error Command structure.

		Includes information needed for fatal handling.

		This structure should be zeroed before usage.
*//***************************************************************************/
struct fatal_error_command {
		/**
		 * Fatal Error command structure identifier.
		 * */
	uint32_t opcode;
		/**
		 * Flags - Specify options to this function, please refer to
		 * \ref FSL_VERIF_FATAL_FLAGS_DEFINES.
		 */
	uint32_t flags;
		/**
		 * Returned Value:
		 * File name in which the error occurred.
		 */
	char  file_name[32];
		/**
		 * Returned Value:
		 * Function name in which the error occurred.
		 */
	char  function_name[48];
		/**
		 * Returned Value:
		 * The error message.
		 */
	char  err_msg[256];
		/**
		 * 128-byte alignment.
		 * */
	uint8_t pad[40];
};

/**************************************************************************//**
@Description	Update external command variable Command structure.

		Includes information needed for updating a value in an external 
		verification command.
*//***************************************************************************/
struct update_ext_cmd_var_command {
		/** Update external command variable command structure 
		 * identifier. */
	uint32_t opcode;
		/** Operation kind. */
	enum arith_operation operation;
		/** Updating value. */
	uint32_t value;
		/** Offset of the variable to be updated relative to the 
		 * external commands start address. */
	uint32_t offset;	
};

void aiop_verification_parse();
__declspec(entry_point) void aiop_verification_fm();
void aiop_verification_fm_temp();
void aiop_verif_init_parser();
uint32_t if_statement_result(
		uint16_t compared_variable_addr,
		uint32_t size,
		int64_t compared_value,
		uint8_t cond);
void timeout_cb_verif(uint64_t arg);
void ipr_timeout_cb_verif(uint64_t arg, uint32_t flags);


/** @} */ /* end of AIOP_Verification */

#endif /* __AIOP_VERIFICATION_H_ */
