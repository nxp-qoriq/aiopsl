/**************************************************************************//**
@File		aiop_verification.h

@Description	This file contains the AIOP SW Verification Structures

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_H_
#define __AIOP_VERIFICATION_H_

#include "aiop_verification_data.h"
#include "general.h"
#include "dplib/fsl_ipsec.h"
#include "aiop_verification_fdma.h"
#include "aiop_verification_tman.h"
#include "aiop_verification_ste.h"
#include "aiop_verification_cdma.h"
#include "aiop_verification_ctlu.h"
#include "aiop_verification_parser.h"
#include "aiop_verification_hm.h"
#include "verification_virtual_pools.h"
#include "aiop_verification_gso.h"
#include "aiop_verification_gro.h"
#include "aiop_verification_ipf.h"
#include "aiop_verification_ipr.h"
#include "dplib/fsl_ip.h"
#include "dplib/fsl_ipsec.h"
#include "dplib/fsl_ipv4_checksum.h"
#include "dplib/fsl_l2.h"
#include "dplib/fsl_l4.h"
#include "dplib/fsl_l4_checksum.h"
#include "dplib/fsl_nat.h"
#include "dplib/fsl_osm.h"
#include "dplib/dpni_drv.h"
#include "dplib/fsl_dpni.h"
#include "dplib/fsl_dplib_sys.h"
#include "dplib/fsl_dpni_cmd.h"


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
#define DATA_SIZE	256
	/**< Buffer Data chunk address in workspace. */
#define WS_DATA_ADDR	0x100
	/**< IPF Fragment's fragmentation commands mask */
#define IPF_FRAGMENT_FRAGMENTATION_COMMANDS_MASK 0x00000010


/* E200-AIOP special regs */

/* Number of tasks as they defined by CTSCSR register. */
#define CTSCSR_ENABLE 0x80000000
#define CTSCSR_1_TASKS 0
#define CTSCSR_2_TASKS (1 << 24)
#define CTSCSR_4_TASKS (2 << 24)
#define CTSCSR_8_TASKS (3 << 24)
#define CTSCSR_16_TASKS (4 << 24)
#define CTSCSR_TASKS_MASK (CTSCSR_2_TASKS | CTSCSR_4_TASKS | CTSCSR_8_TASKS \
              | CTSCSR_16_TASKS)

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
	GRO_MODULE,
	IPR_MODULE,
	IPF_MODULE,
	FDMA_MODULE,
	TMAN_MODULE,
	STE_MODULE,
	CDMA_MODULE,
	CTLU_MODULE,
	CTLU_PARSE_CLASSIFY_MODULE,
	HM_MODULE,
	VPOOL_MODULE,
	IF_MODULE,
	IF_ELSE_MODULE,
	TERMINATE_FLOW_MODULE
};

/**************************************************************************//**
 @enum cond_ids

 @Description	AIOP verification enumeration of if-statement condition IDs.

 @{
*//***************************************************************************/
enum cond_ids {
		/** Equal condition. */
	COND_EQUAL,
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
	COMPARE_GRO_STATUS,
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
@Description	AIOP IF Verification Command structure.

		This command generates an if statement in the verification
		process.

*//***************************************************************************/
struct aiop_if_verif_command {
		/** AIOP Verification IF command structure identifier. */
	uint32_t opcode;
		/** Compared value.
		 * This value will be compared to a variable chosen according to
		 * the compared_variable_addr. */
	int32_t compared_value;
		/** Workspace address of the compared task variable. */
	uint32_t compared_variable_addr;
		/** An offset from the beginning of the commands buffer to the
		 * command to be executed in case of a TRUE result in the IF
		 * statement. */
	uint16_t true_cmd_offset;
		/** Id of the compared task variable.
		 * Please see \ref compared_variable_ids for more details.
	uint8_t compared_variable_id;*/
		/** Condition to be checked in the if statement.
		* Please see \ref cond_ids for more details. */
	uint8_t cond;
		/** 64-bit alignment. */
	uint8_t	pad[1];
};

/**************************************************************************//**
@Description	AIOP IF-ELSE Verification Command structure.

		This command generates an if-else statement in the verification
		process.

*//***************************************************************************/
struct aiop_if_else_verif_command {
		/** AIOP Verification IF command structure identifier. */
	uint32_t opcode;
		/** Compared value.
		 * This value will be compared to a variable chosen according to
		 * the compared_variable_addr. */
	int32_t compared_value;
		/** Workspace address of the compared task variable. */
	uint32_t compared_variable_addr;
		/** An offset from the beginning of the commands buffer to the
		 * command to be executed in case of a TRUE result in the IF
		 * statement. */
	uint16_t true_cmd_offset;
		/** An offset from the beginning of the commands buffer to the
		 * command to be executed in case of a FALSE result in the IF
		 * statement. */
	uint16_t false_cmd_offset;
	/** Id of the compared task variable.
			 * Please see \ref compared_variable_ids for details.
	uint8_t compared_variable_id;*/
		/** Condition to be checked in the if statement. */
	uint8_t cond;
		/** 64-bit alignment. */
	uint8_t	pad[7];
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


void aiop_verification();
void aiop_verification_fm();
void aiop_verification_fm_temp();
void aiop_verif_init_parser();
uint32_t if_statement_result(
		uint32_t compared_variable_addr,
		int32_t compared_value,
		uint8_t cond);


/** @} */ /* end of AIOP_Verification */

#endif /* __AIOP_VERIFICATION_H_ */
