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
@File		cdma.h

@Description	This file contains the AIOP SW CDMA internal API

@note		This file is intended for internal use only (it may not be 
		included elsewhere).
*//***************************************************************************/


#ifndef __CDMA_H_
#define __CDMA_H_

#include "fsl_types.h"
#include "fsl_cdma.h"


/* CDMA Command IDs */
	/** CDMA Acquire Context Memory command code */
#define CDMA_ACQUIRE_CONTEXT_MEM_CMD	0x00000080
	/** CDMA Access Context command code */
#define CDMA_ACCESS_CONTEXT_MEM_CMD	0x00000081
	/** CDMA Release Context Memory command code  */
#define CDMA_RELEASE_CONTEXT_MEM_CMD	0x00000082
	/** CDMA Memory Initialization command code */
#define CDMA_MEMORY_INIT_CMD		0x00000083

/* CDMA Commands general definitions */
	/** CDMA Status Mask. */
#define CDMA_STATUS_MASK		0x000000FF
	/** CDMA Status Offset. */
#define CDMA_STATUS_OFFSET		0x3
	/** CDMA Status Offset. */
#define CDMA_REF_CNT_OFFSET		0x4

/* CDMA Command Flags */
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,00,0] */
#define CDMA_READ_CMD_FLAGS  0x00000000
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,00,0] */
#define CDMA_WRITE_CMD_FLAGS  0x00000000
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[DA]=[01] */
#define CDMA_READ_CMD_ENB    0x40000000
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[DA]=[10] */
#define CDMA_WRITE_CMD_ENB    0x80000000
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,01,0] */
#define CDMA_REFCNT_INC_CMD_FLAGS  0x00000200
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,11,0] */
#define CDMA_REFCNT_DEC_CMD_FLAGS  0x00000600
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,10,0] */
#define CDMA_REFCNT_DEC_REL_CMD_FLAGS  0x00000400
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,00,1] */
#define CDMA_EXT_MUTEX_LOCK_RELEASE_CMD_FLAGS  0x00000100
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,10,01,0] */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_FLAGS  0x00001200
/** CDMA_ACCESS_CONTEXT_MEM_FLAGS[AA,MO,TL,MT,UR,RM]=[0,0,0,00,11,1] */
#define CDMA_WRITE_REL_LOCK_AND_DEC_CMD_FLAGS  0x00000700


/* CDMA Command Arguments */
#define CDMA_ACQUIRE_CONTEXT_MEM_CMD_ARG2(_ws_return_address, _pool_id)	\
	(uint32_t)((_ws_return_address << 16) | _pool_id)
#define CDMA_READ_CMD_ARG1()	\
	(CDMA_READ_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_READ_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)((((_size << 16) | CDMA_READ_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_WRITE_CMD_ARG1()	\
	(CDMA_WRITE_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)((((_size << 16) | CDMA_WRITE_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_EXT_MUTEX_LOCK_TAKE_CMD_ARG1(_flags)	\
	(_flags | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_EXT_MUTEX_LOCK_RELEASE_CMD_ARG1()	\
	(CDMA_EXT_MUTEX_LOCK_RELEASE_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_READ_WITH_MUTEX_CMD_ARG1(_flags)	\
	(_flags | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_READ_WITH_MUTEX_CMD_ARG2(_size, _ws_address) \
	(uint32_t)((((_size << 16) | CDMA_READ_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_WRITE_WITH_MUTEX_CMD_ARG1(_flags)	\
	(_flags | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_WITH_MUTEX_CMD_ARG2(_size, _ws_address) \
	(uint32_t)((((_size << 16) | CDMA_WRITE_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_REFCNT_INC_CMD_ARG1()	\
	(CDMA_REFCNT_INC_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_REFCNT_DEC_CMD_ARG1()	\
	(CDMA_REFCNT_DEC_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_REFCNT_DEC_REL_CMD_ARG1()	\
	(CDMA_REFCNT_DEC_REL_CMD_FLAGS | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_ARG1()	\
	(CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_FLAGS |	\
	CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_LOCK_DMA_READ_AND_INC_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)((((_size << 16) | CDMA_READ_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_WRITE_REL_LOCK_AND_DEC_CMD_ARG1()	\
	(CDMA_WRITE_REL_LOCK_AND_DEC_CMD_FLAGS |	\
	CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_WRITE_REL_LOCK_AND_DEC_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)((((_size << 16) | CDMA_WRITE_CMD_ENB) & 0xFFFF0000) \
			| (_ws_address & 0x0000FFFF))
#define CDMA_MEMORY_INIT_CMD_ARG2(_size, _ws_address)	\
	(uint32_t)(((_size << 16) & 0xFFFF0000) | (_ws_address & 0x0000FFFF))
#define CDMA_ACCESS_CONTEXT_MEM_CMD_ARG1(_ext_offset, _flags)	\
	((_ext_offset << 16) | _flags | CDMA_ACCESS_CONTEXT_MEM_CMD);
#define CDMA_ACCESS_CONTEXT_MEM_CMD_ARG2(_dma_param, _ws_address) \
	(uint32_t)((_dma_param << 16) | (_ws_address & 0x0000FFFF))

/* CDMA CFG register bits */
#define CDMA_BDI_BIT		0x00080000
#define CDMA_PL_BIT		0x00040000
#define CDMA_BMT_BIT		0x00020000
#define CDMA_VA_BIT		0x00010000
#define CDMA_ICID_MASK		0x00007FFF

/**************************************************************************//**
 @enum cdma_errors

 @Description	AIOP CDMA Error codes.

 @{
*//***************************************************************************/
enum cdma_errors {
		/** Success. */
	CDMA_SUCCESS = 0,
		/** Failed due to buffer pool depletion. */
	CDMA_BUFFER_POOL_DEPLETION_ERR = 0x2,
		/** Decrement reference count caused the reference count to
		 * go to zero. (not an error) */
	CDMA_REFCOUNT_DECREMENT_TO_ZERO = 0x3,
		/** Mutex lock failed on a Try Lock request. */
	CDMA_MUTEX_LOCK_FAILED = 0x5,
		/** Mutex lock depletion (max of 4 reached for the task). */
	CDMA_MUTEX_DEPLETION_ERR = 0x84,
		/** Invalid DMA command arguments. */
	CDMA_INVALID_DMA_COMMAND_ARGS_ERR = 0x86,
		/** Invalid DMA command. */
	CDMA_INVALID_DMA_COMMAND_ERR = 0x87,
		/** Increment reference count failed, count is at max value.*/
	CDMA_REFCOUNT_INCREMENT_ERR = 0x90,
		/** Decrement reference count failed, count is at zero. */
	CDMA_REFCOUNT_DECREMENT_ERR = 0x91,
		/** Invalid reference count operation, address is not base
		 * address.*/
	CDMA_REFCOUNT_INVALID_OPERATION_ERR = 0x92,
		/** Invalid mutex lock request,
		 * the task already has a mutex on this address */
	CDMA_INVALID_MUTEX_LOCK_REQ_ERR = 0x93,
		/** Invalid mutex lock release, address not found in active
		 * mutex lock list. */
	CDMA_INVALID_MUTEX_RELEASE_ERR = 0x95,
		/** Internal memory ECC uncorrectable ECC error. */
	CDMA_INTERNAL_MEMORY_ECC_ERR = 0xA0,
		/** Workspace memory read Error. */
	CDMA_WORKSPACE_MEMORY_READ_ERR = 0xA1,
		/** Workspace memory write Error. */
	CDMA_WORKSPACE_MEMORY_WRITE_ERR = 0xA2,
		/** System memory read error (permission or ECC). */
	CDMA_SYSTEM_MEMORY_READ_ERR = 0xA3,
		/** System memory write error (permission or ECC). */
	CDMA_SYSTEM_MEMORY_WRITE_ERR = 0xA4,
		/** Internal error (SRU depletion). */
	CDMA_INTERNAL_ERR = 0xA7
};

/** @} end of enum cdma_errors */

/**************************************************************************//**
 @enum cdma_functions

 @Description	AIOP CDMA Functions enumertion.

 @{
*//***************************************************************************/
enum cdma_function_identifier {
	CDMA_READ = 0,
	CDMA_WRITE,
	CDMA_MUTEX_LOCK_TAKE,
	CDMA_MUTEX_LOCK_RELEASE,
	CDMA_READ_WITH_MUTEX,
	CDMA_WRITE_WITH_MUTEX,
	CDMA_WS_MEMORY_INIT,
	CDMA_REFCOUNT_GET,
	CDMA_ACQUIRE_CONTEXT_MEMORY,
	CDMA_RELEASE_CONTEXT_MEMORY,
	CDMA_REFCOUNT_DECREMENT_AND_RELEASE,
	CDMA_REFCOUNT_INCREMENT,
	CDMA_REFCOUNT_DECREMENT,
	CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT,
	CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT,
	CDMA_ACCESS_CONTEXT_MEMORY
};

/** @}*/ /* end of group CDMA_Enumerations */

/**************************************************************************//**
 @Group		CDMA_Internal_Commands_Flags CDMA Internal Commands Flags

 @Description	CDMA Internal Commands Flags

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group			CDMA_AccessCM_ModeBits

@Description	CDMA Access Context Memory flags

| 0 - 15  | 16 | 17 | 18 |    19 - 20   | 21 - 22 |      23      | 24 - 31 |
|---------|----|----|----|--------------|---------|--------------|---------|
|         | AA | MO | TL |Pre Mutex Lock|ref-count|Post Rel-Mutex|         |

@{
*//***************************************************************************/

/* AA */
		/** Address Attribute (AA).
		- 0: The ext_address provided is the address of the Context
		Memory. Reference count updates are allowed.
		- 1: The ext_address provided is not the address of the Context
		Memory. Reference count updates are not possible. */
#define CDMA_ACCESS_CONTEXT_MEM_AA_BIT		0x00008000

/* MO */
		/** Mutex Object (MO).
		- 0: Mutex lock Take/Release with the ext_address.
		- 1: Mutex lock Take/Release with the ext_address +
		ext_offset */
#define CDMA_ACCESS_CONTEXT_MEM_MO_BIT		0x00004000

/* TL */
		/** Try Lock (TL).
		- 0: The Mutex Lock request in
		\ref CDMA_ACCESS_CONTEXT_MEM_MO_BIT will only return when the
		mutex lock is obtained, any DMA action or post actions will
		also wait for the mutex lock.
		- 1: The Mutex Lock request in
		\ref CDMA_ACCESS_CONTEXT_MEM_MO_BIT will return with a
		\ref CDMA_MUTEX_LOCK_FAILED status if it can not be obtained
		right away. */
#define CDMA_ACCESS_CONTEXT_MEM_TL_BIT		0x00002000

/* Pre DMA Mutex Lock */
		/* No mutex lock is requested. */
#define CDMA_ACCESS_CONTEXT_MEM_NO_MUTEX_LOCK		0x00000000
		/* Mutex read lock is requested. */
#define CDMA_ACCESS_CONTEXT_MEM_MUTEX_READ_LOCK		0x00000800
		/* Mutex write lock is requested. */
#define CDMA_ACCESS_CONTEXT_MEM_MUTEX_WRITE_LOCK	0x00001000

/* Reference count */
		/* Increment reference count. */
#define CDMA_ACCESS_CONTEXT_MEM_INC_REFCOUNT		 0x00000200
		/* Decrement reference count. */
#define CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT		 0x00000600
		/* Decrement reference count and release Context Memory if
		resulting reference count is zero. */
#define CDMA_ACCESS_CONTEXT_MEM_DEC_REFCOUNT_AND_REL 0x00000400

/* Post DMA Rel-Mutex */
		/** Post DMA Release Mutex Lock.
		- 0: No mutex lock to release
		- 1: Release mutex lock */
#define CDMA_ACCESS_CONTEXT_MEM_RM_BIT		0x00000100

/** @} end of group CDMA_AccessCM_ModeBits */


/**************************************************************************//**
@Group				CDMA_AccessCM_DMA

@Description	CDMA Access Context Memory DMA Parameters

|   0 - 1  |        2 - 15      |
|----------|--------------------|
|Read/Write|  Data Access Size  |

@{
*//***************************************************************************/

/* Read/Write Data Access */
		/* No data access */
#define CDMA_ACCESS_CONTEXT_NO_MEM_DMA		0x0000
		/* Read data access from external memory to Workspace */
#define CDMA_ACCESS_CONTEXT_MEM_DMA_READ	0x4000
		/* Write data access from Workspace to external memory */
#define CDMA_ACCESS_CONTEXT_MEM_DMA_WRITE	0x8000

/* Data Access Size */
		/** DMA Size.
		Read/Write data access size, in bytes. */
#define CDMA_ACCESS_CONTEXT_MEM_DMA_SIZE	0x3fff

/** @} end of group CDMA_AccessCM_DMA */
/** @} end of group CDMA_Internal_Commands_Flags */

/*#include "cdma_inline.h"*/

/**************************************************************************//**
@Group		CDMA_Internal_Functions	CDMA Internal Functions

@Description	AIOP CDMA operations internal functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	cdma_refcount_increment

@Description	This routine increments reference count of Context memory
		object.

@Param[in]	context_address - A pointer to the Context memory.

@Return		None.

@remark		Only possible if the address provided with the command is the
		address of the Context memory.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
inline void cdma_refcount_increment(
		uint64_t context_address);


/*************************************************************************//**
@Function	cdma_refcount_decrement

@Description	This routine decrements reference count of Context memory
		object. If the reference count goes to zero, the following
		\ref CDMA_REFCOUNT_DECREMENT_TO_ZERO status code is reported.

@Param[in]	context_address - A pointer to the Context memory.

@Return		0 on Success, or positive value on indication.

@Retval		0 - Success
@Retval		CDMA_REFCOUNT_DECREMENT_TO_ZERO - Decrement reference count
		caused the reference count to go to zero. (not an error).

@remark		Only possible if the address provided with the command is the
		address of the Context.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
inline int cdma_refcount_decrement(
		uint64_t context_address);


/*************************************************************************//**
@Function	cdma_write_lock_dma_read_and_increment

@Description	This routine implements these operations in the following
		order:
		- Mutex write lock take.
		- DMA read (read data from context memory to Workspace).
		- Reference count increment of Context memory object.

		The reading data access will be granted only if no mutex write
		lock is preceding the same context_address. In this case,
		reading data access will take place only when the write lock is
		released. i.e. this routine returns only when the read data
		access is done.

@Param[in]	ws_dst - A pointer to the Workspace.
@Param[in]	context_address - A pointer to the Context address. This
		address is used to read data access and mutex write lock take.
@Param[in]	size - Read data access size, in bytes.

@Return		None.

@remark		None.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
void cdma_write_lock_dma_read_and_increment(
		void *ws_dst,
		uint64_t context_address,
		uint16_t size);


/*************************************************************************//**
@Function	cdma_write_release_lock_and_decrement

@Description	This routine implements these operations in the following
		order:
		- DMA write (write data from Workspace to context memory).
		- Reference count decrement of Context memory object.
		- Release lock.

		If the reference count goes to zero, the following
		\ref CDMA_REFCOUNT_DECREMENT_TO_ZERO status code is reported.

@Param[in]	context_address - A pointer to the Context address. This
		address is used write data access and mutex lock release.
@Param[in]	ws_src - A pointer to the Workspace.
@Param[in]	size - Write data access size, in bytes.

@Return		0 on Success, or positive value on indication.

@Retval		0 - Success
@Retval		CDMA_REFCOUNT_DECREMENT_TO_ZERO - Decrement reference count
		caused the reference count to go to zero. (not an error).

@remark		None.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
int cdma_write_release_lock_and_decrement(
		uint64_t context_address,
		void *ws_src,
		uint16_t size);


/*************************************************************************//**
@Function	cdma_access_context_memory

@Description	This routine is used to perform operations on a Context Memory.
		It can be used as a combo command (instead of some of the
		previous CDMA routines) or can be used to perform any of the
		following operations as a stand-alone command:
		- Mutex lock request
		- DMA read
		- DMA write
		- Increment reference count
		- Decrement reference count and optional release of the Context
		Memory
		- Mutex lock release


@Param[in]	context_address - A pointer to Context address. This address is
		used to read/write data access and/or mutex lock take/release.
@Param[in]	flags - \link CDMA_AccessCM_ModeBits CDMA Access Context Memory
		flags \endlink.
@Param[in]	offset - An offset (in bytes) from the context_address.
@Param[in]	ws_address - A pointer to the Workspace.
@Param[in]	dma_param - \link CDMA_AccessCM_DMA CDMA Access Context Memory
		DMA Parameters \endlink.
@Param[out]	refcount_value - Current value of reference count.

@Return		0 on Success, negative value on error or positive value on
		indication.

@Retval		0 - Success
@Retval		EBUSY - Mutex Lock lock failed on a Try Lock request.
@Retval		CDMA_REFCOUNT_DECREMENT_TO_ZERO - Decrement reference count
		caused the reference count to go to zero. (not an error).

@remark
		- All reference count features are possible only if the
		ext_address provided with the command is the address of the
		Context memory (i.e. \ref CDMA_ACCESS_CONTEXT_MEM_AA_BIT == 0).
		- Each task can have a maximum of 4 simultaneous mutex locks
		active.
		- A mutex lock taken by a task must be released by the same
		task.

@Cautions	If the context memory is released when the reference count
		drops to zero, a mutex lock (if exists) will not be released.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
inline int cdma_access_context_memory(
		uint64_t context_address,
		uint32_t flags,
		uint16_t offset,
		void *ws_address,
		uint16_t dma_param,
		uint32_t *refcount_value);

/*************************************************************************//**
@Function	cdma_access_context_memory_wrp

@Description	Wrapper to the function cdma_access_context_memory.
		See description of the function cdma_access_context_memory_wrp.


@Param[in]	context_address - A pointer to Context address. This address is
		used to read/write data access and/or mutex lock take/release.
@Param[in]	flags - \link CDMA_AccessCM_ModeBits CDMA Access Context Memory
		flags \endlink.
@Param[in]	offset - An offset (in bytes) from the context_address.
@Param[in]	ws_address - A pointer to the Workspace.
@Param[in]	dma_param - \link CDMA_AccessCM_DMA CDMA Access Context Memory
		DMA Parameters \endlink.
@Param[out]	refcount_value - Current value of reference count.

@Return		0 on Success, negative value on error or positive value on
		indication.

@Retval		0 - Success
@Retval		EBUSY - Mutex Lock lock failed on a Try Lock request.
@Retval		CDMA_REFCOUNT_DECREMENT_TO_ZERO - Decrement reference count
		caused the reference count to go to zero. (not an error).

@remark
		- All reference count features are possible only if the
		ext_address provided with the command is the address of the
		Context memory (i.e. \ref CDMA_ACCESS_CONTEXT_MEM_AA_BIT == 0).
		- Each task can have a maximum of 4 simultaneous mutex locks
		active.
		- A mutex lock taken by a task must be released by the same
		task.

@Cautions	If the context memory is released when the reference count
		drops to zero, a mutex lock (if exists) will not be released.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
int cdma_access_context_memory_wrp(
		uint64_t context_address,
		uint32_t flags,
		uint16_t offset,
		void *ws_address,
		uint16_t dma_param,
		uint32_t *refcount_value);


/*************************************************************************//**
@Function	cdma_read_wrp

@Description	Wrapper to the function cdma_read.
		See description of the function cdma_read..

@Param[in]	ws_dst - A pointer to the Workspace.
@Param[in]	ext_address - A pointer to a context memory address in the
		external memory (DDR/PEB).
@Param[in]	size - Read data access size, in bytes.

@Return		None.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
void cdma_read_wrp(void *ws_dst, uint64_t ext_address, uint16_t size);

/*************************************************************************//**
@Function	cdma_read_with_mutex_wrp

@Description	Wrapper of the function cdma_read_with_mutex.
		See description of the function cdma_read_with_mutex.

@Param[in]	ext_address - A pointer to a context memory address in the
		external memory (DDR/PEB). This address is used to read data
		access and mutex lock take/release.
@Param[in]	flags - \link CDMA_DMA_MUTEX_ModeBits CDMA Mutex flags
		\endlink.
@Param[in]	ws_dst - A pointer to the Workspace.
@Param[in]	size - Read data access size, in bytes.

@Return		None.

@remark
		- Each task can have a maximum of 4 simultaneous mutex locks
		active.
		- A mutex lock taken by a task must be released by the same
		task.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
void cdma_read_with_mutex_wrp(  uint64_t ext_address,
				uint32_t flags,
				void *ws_dst,
				uint16_t size);

/*************************************************************************//**
@Function	cdma_write_wrp

@Description	Wrapper to the function cdma_write.
		See description of the function cdma_write.

@Param[in]	ext_address - A pointer to a context memory address in the
		external memory (DDR/PEB).
@Param[in]	ws_src - A pointer to the Workspace.
@Param[in]	size - Write data access size, in bytes.

@Return		None.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
void cdma_write_wrp(uint64_t ext_address,
			   void *ws_src,
			   uint16_t size);


/*************************************************************************//**
@Function	cdma_handle_fatal_errors

@Description	This routine gets cdma error status and call to
		handle_fatal_error function accordantly.

@Param[in]	status - error status.

*//***************************************************************************/
void cdma_handle_fatal_errors(
		uint8_t status);

/**************************************************************************//**
@Function	cdma_exception_handler

@Description	Handler for the error status returned from the CDMA API
		functions.

@Param[in]	file_path - The path of the file in which the error occurred.
@Param[in]	func_id - The function in which the error occurred.
@Param[in]	line - The line in which the error occurred.
@Param[in]	status - Status to be handled be this function.

@Return		None.

@Cautions	This is a non return function.
*//***************************************************************************/
void cdma_exception_handler(enum cdma_function_identifier func_id,
			     uint32_t line,
			     int32_t status);


/** @} end of group CDMA_Internal_Functions */

#endif /* __CDMA_H_ */
