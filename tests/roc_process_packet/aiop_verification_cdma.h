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
@File          aiop_verification_cdma.h

@Description   This file contains the AIOP CDMA SW Verification Structures
*//***************************************************************************/


#ifndef __AIOP_VERIFICATION_CDMA_H_
#define __AIOP_VERIFICATION_CDMA_H_

#include "fsl_types.h"


/* CDMA Command IDs (from CDMA section in ArchDef) */
#define	CDMA_ACQUIRE_CONTEXT_MEM_CMD 	0x00000080
	/**< This comand is used to request a Context Memory to be acquired from a BMan pool */
#define CDMA_ACCESS_CONTEXT_MEM_CMD	0x00000081
	/**< This comand is used to do operations on a Context Memory. */
#define CDMA_RELEASE_CONTEXT_MEM_CMD	0x00000082
	/**< This comand is used is used to free the Context Memory to the BMan pool it was acquired from. */
#define CDMA_MEM_INIT_CMD		0x00000083
	/**< This comand is used is used to write a provided pattern into a range of workspace memory. */

/* CDMA Command IDs (based on CDMA_ACCESS_CONTEXT_MEM_CMD in ArchDef) */
#define	CDMA_READ_CMD 			0x00000181
	/**< CDMA read command */
#define	CDMA_WRITE_CMD 			0x00000281
	/**< CDMA write command */
#define	CDMA_MUTEX_LOCK_TAKE_CMD 	0x00000381
	/**< CDMA mutex lock take command */
#define	CDMA_MUTEX_LOCK_RELEASE_CMD 	0x00000481
	/**< CDMA mutex lock release command */
#define	CDMA_READ_WITH_MUTEX_CMD 	0x00000581
	/**< CDMA read with mutex command */
#define	CDMA_WRITE_WITH_MUTEX_CMD 	0x00000681
	/**< CDMA write with mutex command */
#define	CDMA_REFCOUNT_INC_CMD 		0x00000781
	/**< CDMA reference count increment command */
#define	CDMA_REFCOUNT_DEC_CMD 		0x00000881
	/**< CDMA reference count decrement command */
#define	CDMA_REFCOUNT_DEC_AND_RELEASE_CMD 	0x00000981
	/**< CDMA reference count decrement and release command */
#define	CDMA_LOCK_DMA_READ_AND_INC_CMD 		0x00000a81
	/**< CDMA lock dma read and increment command */
#define	CDMA_WRITE_RELEASE_LOCK_AND_DEC_CMD 	0x00000b81
	/**< CDMA write release lock and decrement command */
#define	CDMA_REFCOUNT_GET_CMD 			0x00000c81
	/**< CDMA reference count getter command */



/* CDMA Commands Structure identifiers */
#define CDMA_ACQUIRE_CONTEXT_MEM_CMD_STR	((CDMA_MODULE << 16) | CDMA_ACQUIRE_CONTEXT_MEM_CMD)
	/* CDMA cdma_acquire_context_memory Command Structure identifier */
#define CDMA_ACCESS_CONTEXT_MEM_CMD_STR		((CDMA_MODULE << 16) | CDMA_ACCESS_CONTEXT_MEM_CMD)
	/* CDMA cdma_access_context_memory Command Structure identifier */
#define CDMA_RELEASE_CONTEXT_MEM_CMD_STR	((CDMA_MODULE << 16) | CDMA_RELEASE_CONTEXT_MEM_CMD)
	/* CDMA acdma_release_context_memory Command Structure identifier */
#define CDMA_WS_MEM_INIT_CMD_STR		((CDMA_MODULE << 16) | CDMA_MEM_INIT_CMD)
	/* CDMA cdma_memory_init Command Structure identifier */

#define CDMA_READ_CMD_STR			((CDMA_MODULE << 16) | CDMA_READ_CMD)
	/* CDMA cdma_read Command Structure identifier */
#define CDMA_WRITE_CMD_STR			((CDMA_MODULE << 16) | CDMA_WRITE_CMD)
	/* CDMA cdma_write Command Structure identifier */
#define	CDMA_MUTEX_LOCK_TAKE_CMD_STR 		((CDMA_MODULE << 16) | CDMA_MUTEX_LOCK_TAKE_CMD)
	/**< CDMA mutex lock take command Structure identifier */
#define	CDMA_MUTEX_LOCK_RELEASE_CMD_STR 	((CDMA_MODULE << 16) | CDMA_MUTEX_LOCK_RELEASE_CMD)
	/**< CDMA mutex lock release command Structure identifier */
#define	CDMA_READ_WITH_MUTEX_CMD_STR		((CDMA_MODULE << 16) | CDMA_READ_WITH_MUTEX_CMD)
	/**< CDMA read with mutex command Structure identifier */
#define	CDMA_WRITE_WITH_MUTEX_CMD_STR 		((CDMA_MODULE << 16) | CDMA_WRITE_WITH_MUTEX_CMD)
	/**< CDMA write with mutex command Structure identifier */
#define	CDMA_REFCOUNT_INC_CMD_STR 		((CDMA_MODULE << 16) | CDMA_REFCOUNT_INC_CMD)
	/**< CDMA reference count increment command Structure identifier */
#define CDMA_REFCOUNT_DEC_CMD_STR		((CDMA_MODULE << 16) | CDMA_REFCOUNT_DEC_CMD)
	/* CDMA cdma_refcount_decrement Command Structure identifier */
#define CDMA_REFCOUNT_DEC_AND_RELEASE_CMD_STR	((CDMA_MODULE << 16) | CDMA_REFCOUNT_DEC_AND_RELEASE_CMD)
	/* CDMA cdma_refcount_decrement_and_release Command Structure identifier */
#define	CDMA_LOCK_DMA_READ_AND_INC_CMD_STR 	((CDMA_MODULE << 16) | CDMA_LOCK_DMA_READ_AND_INC_CMD)
	/**< CDMA lock dma read and increment command Structure identifier*/
#define	CDMA_WRITE_RELEASE_LOCK_AND_DEC_CMD_STR ((CDMA_MODULE << 16) | CDMA_WRITE_RELEASE_LOCK_AND_DEC_CMD)
	/**< CDMA write release lock and decrement command Structure identifier*/
#define	CDMA_REFCOUNT_GET_CMD_STR ((CDMA_MODULE << 16) | CDMA_REFCOUNT_GET_CMD)
	/**< CDMA reference count getter command Structure identifier*/


/** \addtogroup AIOP_Service_Routines_Verification
 *  @{
 */


/**************************************************************************//**
 @Group		AIOP_CDMA_SRs_Verification

 @Description	AIOP CDMA Verification structures definitions.

 @{
*//***************************************************************************/

/**************************************************************************//**
@Description	CDMA Acquire Context Memory Command Structure.

		Includes information needed for CDMA Acquire Context Memory
		command verification.

*//***************************************************************************/
struct cdma_acquire_context_memory_command {
	uint32_t 	opcode;
		/**< CDMA cdma_acquire_context_memory Command Structure identifier. */
	uint16_t	pool_id;
		/**< BMan pool ID used for the Acquire Context Memory. */
	uint8_t		pad1[6];
		/**< 64-bit alignment. */
	uint32_t 	context_memory;
		/**< An address to the Workspace where to return
		the acquired 64 bit address of the Context memory. */
	uint64_t	context_memory_addr;
		/**< A returned pointer to the acquired 64 bit address
		 * of the Context memory. */
	int8_t  	status;
			/**< Command returned status. */
	uint8_t		pad2[7];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Release Context Memory Command Structure.

		Includes information needed for CDMA Release Context Memory
		command verification.

*//***************************************************************************/
struct cdma_release_context_memory_command{
	uint32_t 	opcode;
		/**< CDMA cdma_release_context_memory Command Structure identifier. */
	uint8_t		pad1[4];
		/**< 64-bit alignment. */
	uint32_t 	context_memory;
		/**< An address to the Workspace where
		the 64 bit address of the Context memory is found. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[3];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Read Command structure.

		Includes information needed for CDMA DMA read command
		verification.

*//***************************************************************************/
struct cdma_read_command {
	uint32_t 	opcode;
		/**< CDMA cdma_read Command Structure identifier. */
	uint32_t 	ws_dst;
		/**< A pointer to the Workspace. */
	uint32_t 	context_memory;
				/**< An address to the Workspace where
				the 64 bit address of the Context memory is found. */
	uint16_t	size;
		/**< Read data access size, in bytes. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[1];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Write Command Structure.

		Includes information needed for CDMA DMA write command
		verification.

*//***************************************************************************/
struct cdma_write_command {
	uint32_t 	opcode;
		/**< CDMA cdma_write Command Structure identifier. */
	uint32_t 	ws_src;
			/**< A pointer to the Workspace. */
	uint32_t 	context_memory;
				/**< An address to the Workspace where
				the 64 bit address of the Context memory is found. */
	uint16_t	size;
		/**< Write data access size, in bytes. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[1];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Mutex Lock Take Command Structure.

		Includes information needed for CDMA Mutex Lock Take command
		verification.

*//***************************************************************************/
struct cdma_mutex_lock_take_command  {
	uint32_t 	opcode;
		/**< CDMA cdma_mutex_lock_take Command Structure identifier. */
	uint8_t		pad1[4];
			/**< 64-bit alignment. */
	uint64_t	mutex_id;
		/**< A 64 bits mutex ID. */
	uint32_t	flags;
		/**< CDMA Mutex flags */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad2[3];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Mutex Lock Release Command Structure.

		Includes information needed for CDMA Mutex Lock Release command
		verification.

*//***************************************************************************/
struct cdma_mutex_lock_release_command {
	uint32_t 	opcode;
		/**< CDMA cdma_mutex_lock_release Command Structure identifier. */
	uint8_t		pad1[4];
		/**< 64-bit alignment. */
	uint64_t	mutex_id;
		/**< A 64 bits mutex ID. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad2[7];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Read with Mutex Command structure.

		Includes information needed for CDMA DMA read with Mutex command
		verification.

*//***************************************************************************/
struct cdma_read_with_mutex_command {
	uint32_t 	opcode;
		/**< CDMA cdma_read_with_mutex Command Structure identifier. */
	uint32_t 	ws_dst;
		/**< A pointer to the Workspace. */
	uint32_t 	context_memory;
			/**< An address to the Workspace where
			the 64 bit address of the Context memory is found. */
	uint32_t	flags;
		/**< CDMA Mutex flags */
	uint16_t	size;
		/**< Read data access size, in bytes. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[5];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Write with Mutex Command Structure.

		Includes information needed for CDMA DMA write with Mutex command
		verification.

*//***************************************************************************/
struct cdma_write_with_mutex_command {
	uint32_t 	opcode;
		/**< CDMA cdma_write_with_mutex Command Structure identifier. */
	uint32_t 	ws_src;
		/**< A pointer to the Workspace. */
	uint32_t 	context_memory;
			/**< An address to the Workspace where
			the 64 bit address of the Context memory is found. */
	uint32_t	flags;
		/**< CDMA Mutex flags */
	uint16_t	size;
		/**< Read data access size, in bytes. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[5];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Refcount Increment Command Structure.

		Includes information needed for CDMA Refcount Increment command verification.

*//***************************************************************************/
struct cdma_refcount_increment_command{
	uint32_t 	opcode;
		/**< CDMA cdma_refcount_Increment Command Structure identifier. */
	uint8_t		pad1[4];
			/**< 64-bit alignment. */
	uint32_t 	context_memory;
			/**< An address to the Workspace where
		the 64 bit address of the Context memory is found. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad2[3];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Refcount Decrement Command Structure.

		Includes information needed for CDMA Refcount Decrement command verification.

*//***************************************************************************/
struct cdma_refcount_decrement_command{
	uint32_t 	opcode;
		/**< CDMA cdma_refcount_decrement Command Structure identifier. */
	uint8_t		pad1[4];
		/**< 64-bit alignment. */
	uint32_t 	context_memory;
		/**< An address to the Workspace where
			the 64 bit address of the Context memory is found. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad2[3];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Refcount Decrement and Release Command Structure.

		Includes information needed for CDMA Refcount Decrement and
		Release command verification.

*//***************************************************************************/
struct cdma_refcount_decrement_and_release_command{
	uint32_t 	opcode;
		/**< CDMA cdma_refcount_decrement_and_release Command Structure identifier. */
	uint8_t		pad1[4];
		/**< 64-bit alignment. */
	uint32_t 	context_memory;
		/**< An address to the Workspace where
			the 64 bit address of the Context memory is found. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad2[3];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Write Lock DMA Read and Increment Command structure.

		Includes information needed for Write Lock DMA Read and
		Increment command verification.

*//***************************************************************************/
struct cdma_write_lock_dma_read_and_increment_command  {
	uint32_t 	opcode;
		/**< CDMA cdma_write_lock_dma_read_and_increment Command Structure identifier. */
	uint32_t 	ws_dst;
		/**< A pointer to the Workspace. */
	uint32_t 	context_memory;
		/**< An address to the Workspace where
		the 64 bit address of the Context memory is found. */
	uint16_t	size;
		/**< Read data access size, in bytes. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[1];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Write Release Lock and Decrement Command Structure.

		Includes information needed for CDMA Write Release Lock and
		Decrement command verification.

*//***************************************************************************/
struct cdma_write_release_lock_and_decrement_command {
	uint32_t 	opcode;
		/**< CDMA cdma_write_release_lock_and_decrement Command Structure identifier. */
	uint32_t 	ws_src;
		/**< A pointer to the Workspace. */
	uint32_t 	context_memory;
		/**< An address to the Workspace where
			the 64 bit address of the Context memory is found. */
	uint16_t	size;
		/**< Write data access size, in bytes. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[1];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Memory Init Command Structure.

		Includes information needed for CDMA Memory Initialization
		command verification.

*//***************************************************************************/
struct cdma_ws_memory_init_command{
	uint32_t 	opcode;
		/**< CDMA cdma_ws_memory_init Command Structure identifier. */
	uint32_t 	ws_dst;
		/**< A pointer to the Workspace. */
	uint32_t	data_pattern;
			/**< Data to write in workspace */
	uint16_t	size;
		/**< Data initialization size, in bytes. */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[1];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Access Context Memory Command Structure.

		Includes information needed for CDMA Access Context Memory
		command verification.

*//***************************************************************************/
struct cdma_access_context_memory_command {
	uint32_t 	opcode;
		/**< CDMA cdma_access_context_memory Command Structure identifier. */
	uint32_t 	ws_address;
			/**< A pointer to the Workspace. */
	uint32_t 	context_memory;
			/**< An address to the Workspace where
			the 64 bit address of the Context memory is found. */
	uint32_t	flags;
			/**< CDMA flags */
	uint32_t        refcount_value;
			/**< Command returned Current value of reference count */
	uint16_t	offset;
		/**< An offset (in bytes) from the context_address. */
	uint16_t	dma_param;
		/**< DMA Parameters */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[7];
		/**< 64-bit alignment. */
};

/**************************************************************************//**
@Description	CDMA Reference count getter Command Structure.

		Includes information needed for CDMA reference count getter
		command verification.

*//***************************************************************************/
struct cdma_refcount_get_command {
	uint32_t 	opcode;
		/**< CDMA cdma_refcount_get_command Command Structure identifier. */
	uint32_t 	context_memory;
		/**< An address to the Workspace where
		 * the 64 bit address of the Context memory is found. */
	uint32_t        refcount_value;
		/**< Command returned Current value of reference count */
	int8_t  	status;
		/**< Command returned status. */
	uint8_t		pad[3];
		/**< 64-bit alignment. */
};

/** @} */ /* end of AIOP_CDMA_SRs_Verification */

/** @}*/ /* end of AIOP_Service_Routines_Verification */

uint16_t aiop_verification_cdma(uint32_t asa_seg_addr);

#endif /* __AIOP_VERIFICATION_CDMA_H_ */
