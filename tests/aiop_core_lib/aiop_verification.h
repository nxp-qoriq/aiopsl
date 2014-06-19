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
#include "aiop_verification_keygen.h"
#include "aiop_verification_table.h"
#include "aiop_verification_parser.h"
#include "aiop_verification_hm.h"
#include "verification_virtual_pools.h"
#include "aiop_verification_gso.h"
#include "aiop_verification_gro.h"
#include "aiop_verification_osm.h"
#include "aiop_verification_ipf.h"
#include "aiop_verification_ipr.h"
#include "aiop_verification_ipsec.h"
#include "dplib/fsl_ip.h"
#include "dplib/fsl_ipsec.h"
#include "dplib/fsl_l2.h"
#include "dplib/fsl_l4.h"
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
#define DATA_SIZE	320
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
	TABLE_MODULE,
	PARSE_MODULE,
	HM_MODULE,
	VPOOL_MODULE,
	IF_MODULE,
	IF_ELSE_MODULE,
	TERMINATE_FLOW_MODULE,
	KEYGEN_MODULE,
	IPSEC_MODULE,
	WRITE_DATA_TO_WS_MODULE,
	UPDATE_ASA_VARIABLE,
	OSM_MODULE,
	EXCEPTION_MODULE
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
@Description	Write fatal FQID to workspace TLS Command structure.

		Includes information needed for writing fqid to workspace TLS
		(as a preceding step for a replace command).

*//***************************************************************************/
struct write_fatal_fqid_to_workspace_tls_command {
		/** Write Data to workspace command structure identifier. */
	uint32_t opcode;
		/**
		 * FQID to save in TLS. */
	uint32_t fqid;
};

void aiop_verification_parse();
void aiop_verification_sr();
void aiop_verification_fm();
void aiop_verification_fm_temp();
void aiop_verif_init_parser();
uint32_t if_statement_result(
		uint16_t compared_variable_addr,
		uint32_t size,
		int64_t compared_value,
		uint8_t cond);
void timeout_cb_verif(uint64_t arg);


/** @} */ /* end of AIOP_Verification */

#endif /* __AIOP_VERIFICATION_H_ */
