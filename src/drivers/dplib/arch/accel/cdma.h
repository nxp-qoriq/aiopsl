/**************************************************************************//**
@File		cdma.h

@Description	This file contains the AIOP SW CDMA internal API
*//***************************************************************************/


#ifndef __CDMA_H_
#define __CDMA_H_

#include "common/types.h"
#include "dplib/fsl_cdma.h"


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

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* ++++++++++++++++++++++++  Status/Errors Per Command +++++++++++++++++++*/
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**************************************************************************//**
 @Group		CDMA_Internal_Commands_Satus CDMA Internal Commands Status

 @Description	CDMA internal commands return status

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		CDMA_ACQUIRE_CONTEXT_MEMORY_STATUS

@Description	CDMA status returned by the cdma_acquire_context_memory command

@{
*//***************************************************************************/
/* Success */
#define CDMA_ACQUIRE_CONTEXT_MEMORY__SUCCESS CDMA_SUCCESS
/* failed due to buffer pool depletion */
#define CDMA_ACQUIRE_CONTEXT_MEMORY__BUFFER_POOL_DEPLETION_ERR \
			CDMA_BUFFER_POOL_DEPLETION_ERR
/*  Workspace memory read Error */
#define CDMA_ACQUIRE_CONTEXT_MEMORY__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* Workspace memory write Error */
#define CDMA_ACQUIRE_CONTEXT_MEMORY__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* External memory write error */
#define CDMA_ACQUIRE_CONTEXT_MEMORY__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR

/* @} end of group CDMA_ACQUIRE_CONTEXT_MEMORY_STATUS */

/**************************************************************************//**
@Group		CDMA_RELEASE_CONTEXT_MEMORY_STATUS

@Description	CDMA status returned by the cdma_release_context_memory command

@{
*//***************************************************************************/
/* Success */
#define CDMA_RELEASE_CONTEXT_MEMORY__SUCCESS CDMA_SUCCESS
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_RELEASE_CONTEXT_MEMORY__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_RELEASE_CONTEXT_MEMORY__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_RELEASE_CONTEXT_MEMORY__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR

/* @} end of group CDMA_RELEASE_CONTEXT_MEMORY_STATUS */

/**************************************************************************//**
@Group		CDMA_REFCOUNT_DECREMENT_AND_RELEASE_STATUS

@Description	CDMA status returned by the cdma_refcount_decrement_and_release
		command

@{
*//***************************************************************************/
/* Success */
#define CDMA_REFCOUNT_DECREMENT_AND_RELEASE__SUCCESS CDMA_SUCCESS
/** Decrement reference count caused the reference count to
 * go to zero. (not an error) */
#define CDMA_REFCOUNT_DECREMENT_AND_RELEASE__REFCOUNT_DECREMENT_TO_ZERO \
			CDMA_REFCOUNT_DECREMENT_TO_ZERO
/** Decrement reference count failed, count is at zero. */
#define CDMA_REFCOUNT_DECREMENT_AND_RELEASE__REFCOUNT_DECREMENT_ERR \
			CDMA_REFCOUNT_DECREMENT_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_REFCOUNT_DECREMENT_AND_RELEASE__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* Workspace memory write Error */
#define CDMA_REFCOUNT_DECREMENT_AND_RELEASE__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_REFCOUNT_DECREMENT_AND_RELEASE__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* External memory write error */
#define CDMA_REFCOUNT_DECREMENT_AND_RELEASE__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_REFCOUNT_DECREMENT_AND_RELEASE__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR

/* @} end of group CDMA_REFCOUNT_DECREMENT_AND_RELEASE_STATUS */

/**************************************************************************//**
@Group		CDMA_REFCOUNT_INCREMENT_STATUS

@Description	CDMA status returned by the cdma_refcount_increment command

@{
*//***************************************************************************/
/* Success */
#define CDMA_REFCOUNT_INCREMENT__SUCCESS CDMA_SUCCESS
/** Increment reference count failed, count is at max value.*/
#define CDMA_REFCOUNT_INCREMENT__REFCOUNT_INCREMENT_ERR \
			CDMA_REFCOUNT_INCREMENT_ERR
/** Invalid reference count operation, address is not base address.*/
#define CDMA_REFCOUNT_INCREMENT__REFCOUNT_INVALID_OPERATION_ERR \
			CDMA_REFCOUNT_INVALID_OPERATION_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_REFCOUNT_INCREMENT__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* Workspace memory write Error */
#define CDMA_REFCOUNT_INCREMENT__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_REFCOUNT_INCREMENT__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* External memory write error */
#define CDMA_REFCOUNT_INCREMENT__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_REFCOUNT_INCREMENT__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR

/* @} end of group CDMA_REFCOUNT_INCREMENT_STATUS */

/**************************************************************************//**
@Group		CDMA_REFCOUNT_DECREMENT_STATUS

@Description	CDMA status returned by the cdma_refcount_decrement command

@{
*//***************************************************************************/
/* Success */
#define CDMA_REFCOUNT_DECREMENT__SUCCESS CDMA_SUCCESS
/** Decrement reference count caused the reference count to
 * go to zero. (not an error) */
#define CDMA_REFCOUNT_DECREMENT__REFCOUNT_DECREMENT_TO_ZERO \
			CDMA_REFCOUNT_DECREMENT_TO_ZERO
/** Decrement reference count failed, count is at zero. */
#define CDMA_REFCOUNT_DECREMENT__REFCOUNT_DECREMENT_ERR \
			CDMA_REFCOUNT_DECREMENT_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_REFCOUNT_DECREMENT__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* Workspace memory write Error */
#define CDMA_REFCOUNT_DECREMENT__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_REFCOUNT_DECREMENT__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* External memory write error */
#define CDMA_REFCOUNT_DECREMENT__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_REFCOUNT_DECREMENT__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR

/* @} end of group CDMA_REFCOUNT_DECREMENT_STATUS */

/**************************************************************************//**
@Group		CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT_STATUS

@Description	CDMA status returned by the 
		cdma_write_lock_dma_read_and_increment command

@{
*//***************************************************************************/
/* Success */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__SUCCESS CDMA_SUCCESS
/** Bound Check failed, DMA access outside the Context boundaries.*/
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__BOUND_CHECK_ERR \
			CDMA_BOUND_CHECK_ERR
/** Mutex lock depletion (max of 4 reached for the task). */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__MUTEX_DEPLETION_ERR \
			CDMA_MUTEX_DEPLETION_ERR
/** 0x93 invalid mutex lock request,
 * the task already has a mutex on this address */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__INVALID_MUTEX_LOCK_REQ_ERR \
			CDMA_INVALID_MUTEX_LOCK_REQ_ERR
/** Increment reference count failed, count is at max value.*/
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__REFCOUNT_INCREMENT_ERR \
			CDMA_REFCOUNT_INCREMENT_ERR
/** Invalid reference count operation, address is not base address.*/
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__REFCOUNT_INVALID_OPERATION_ERR \
			CDMA_REFCOUNT_INVALID_OPERATION_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* Workspace memory write Error */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* External memory write error */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR

/* @} end of group CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT_STATUS */

/**************************************************************************//**
@Group		CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT_STATUS

@Description	CDMA status returned by the 
		cdma_write_release_lock_and_decrement command

@{
*//***************************************************************************/
/* Success */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__SUCCESS CDMA_SUCCESS
/* Invalid mutex lock release, address not found in active mutex lock list. */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__INVALID_MUTEX_RELEASE_ERR \
			CDMA_INVALID_MUTEX_RELEASE_ERR
/** Decrement reference count caused the reference count to
 * go to zero. (not an error) */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__REFCOUNT_DECREMENT_TO_ZERO \
			CDMA_REFCOUNT_DECREMENT_TO_ZERO
/** Decrement reference count failed, count is at zero. */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__REFCOUNT_DECREMENT_ERR \
			CDMA_REFCOUNT_DECREMENT_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* Workspace memory write Error */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* External memory write error */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR
/* @} end of group CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT_STATUS */

/* TODO: add to the code a check if DMA failed and release the mutex */

/**************************************************************************//**
@Group		CDMA_ACCESS_CONTEXT_MEMORY_STATUS

@Description	CDMA status returned by the cdma_access_context_memory command

@{
*//***************************************************************************/
/* Success */
#define CDMA_ACCESS_CONTEXT_MEMORY__SUCCESS CDMA_SUCCESS
/** Bound Check failed, DMA access outside the Context boundaries.*/
#define CDMA_ACCESS_CONTEXT_MEMORY__BOUND_CHECK_ERR \
			CDMA_BOUND_CHECK_ERR
/** Mutex lock depletion (max of 4 reached for the task). */
#define CDMA_ACCESS_CONTEXT_MEMORY__MUTEX_DEPLETION_ERR \
			CDMA_MUTEX_DEPLETION_ERR
/** 0x93 invalid mutex lock request,
 * the task already has a mutex on this address */
#define CDMA_ACCESS_CONTEXT_MEMORY__INVALID_MUTEX_LOCK_REQ_ERR \
			CDMA_INVALID_MUTEX_LOCK_REQ_ERR
/* Invalid mutex lock release, address not found in active mutex lock list. */
#define CDMA_ACCESS_CONTEXT_MEMORY__INVALID_MUTEX_RELEASE_ERR \
			CDMA_INVALID_MUTEX_RELEASE_ERR
/** Increment reference count failed, count is at max value.*/
#define CDMA_ACCESS_CONTEXT_MEMORY__REFCOUNT_INCREMENT_ERR \
			CDMA_REFCOUNT_INCREMENT_ERR
/** Invalid reference count operation, address is not base address.*/
#define CDMA_ACCESS_CONTEXT_MEMORY__REFCOUNT_INVALID_OPERATION_ERR \
			CDMA_REFCOUNT_INVALID_OPERATION_ERR
/** Decrement reference count caused the reference count to
 * go to zero. (not an error) */
#define CDMA_ACCESS_CONTEXT_MEMORY__REFCOUNT_DECREMENT_TO_ZERO \
			CDMA_REFCOUNT_DECREMENT_TO_ZERO
/** Decrement reference count failed, count is at zero. */
#define CDMA_ACCESS_CONTEXT_MEMORY__REFCOUNT_DECREMENT_ERR \
			CDMA_REFCOUNT_DECREMENT_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_ACCESS_CONTEXT_MEMORY__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_ACCESS_CONTEXT_MEMORY__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* Workspace memory write Error */
#define CDMA_ACCESS_CONTEXT_MEMORY__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* External memory write error */
#define CDMA_ACCESS_CONTEXT_MEMORY__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_ACCESS_CONTEXT_MEMORY__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR

/* @} end of group CDMA_ACCESS_CONTEXT_MEMORY_STATUS */

/* @} end of group CDMA_Internal_Commands_Satus */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* ^^^^^^^^^^^^^^^^^^ End of Status/Errors Per Command ^^^^^^^^^^^^^^^^^^^*/
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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

/* @} end of group CDMA_AccessCM_ModeBits */


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

/* @} end of group CDMA_AccessCM_DMA */
/* @} end of group CDMA_Internal_Commands_Flags */

/**************************************************************************//**
@Group		CDMA_Internal_Functions	CDMA Internal Functions

@Description	AIOP CDMA operations internal functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	cdma_acquire_context_memory

@Description	This routine requests a Context memory block from a BMan pool.
		The returned value is a 64 bit address to the context in
		external memory (DDR/PEB).
		The 64 bit address is used to make CDMA access to the Context
		memory and can also be used as an address to mutex lock to all
		or part of the Context.
		The reference count is set to 1.

@Param[in]	pool_id - BMan pool ID used for the Acquire Context Memory.
@Param[in]	context_memory - A pointer to the Workspace where to return
		the acquired 64 bit address of the Context memory.

@Return		Status - Success or Failure.
		(\ref CDMA_ACQUIRE_CONTEXT_MEMORY_STATUS)

@Cautions	The maximum legal pool_id value is 0x03FF.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_acquire_context_memory(
		uint16_t pool_id,
		uint64_t *context_memory);


/*************************************************************************//**
@Function	cdma_release_context_memory

@Description	This routine releases (frees) the Context memory block to the
		BMan pool it was acquired from.

@Param[in]	context_address - Address of Context memory.

@Return		Status - Success or Failure.
		(\ref CDMA_RELEASE_CONTEXT_MEMORY_STATUS).

@remark		This command should be used after task calls to
		cdma_refcount_decrement() routine and as a result, a
		Context Memory reference count has dropped to zero.

@Cautions	A mutex lock (if exists) will not be released.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_release_context_memory(
		uint64_t context_address);


/*************************************************************************//**
@Function	cdma_refcount_decrement_and_release

@Description	This routine decrements reference count of Context memory
		object. If resulting reference count is zero, the following
		\ref CDMA_REFCOUNT_DECREMENT_TO_ZERO status code is reported
		and the Context memory block is automatically released to the
		BMan pool it was acquired from.

@Param[in]	context_address - A pointer to the Context address.

@Return		Status - Success or Failure.
		(\ref CDMA_REFCOUNT_DECREMENT_AND_RELEASE_STATUS).

@remark		Only possible if the address provided with the command is the
		address of the Context.

@Cautions	If the context memory is released when the reference count
		drops to zero, a mutex lock (if exists) will not be released.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_refcount_decrement_and_release(
		uint64_t context_address);


/*************************************************************************//**
@Function	cdma_refcount_increment

@Description	This routine increments reference count of Context memory
		object.

@Param[in]	context_address - A pointer to the Context memory.

@Return		Status - Success or Failure.
		(\ref CDMA_REFCOUNT_INCREMENT_STATUS).

@remark		Only possible if the address provided with the command is the
		address of the Context memory.

@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_refcount_increment(
		uint64_t context_address);


/*************************************************************************//**
@Function	cdma_refcount_decrement

@Description	This routine decrements reference count of Context memory
		object. If the reference count goes to zero, the following
		\ref CDMA_REFCOUNT_DECREMENT_TO_ZERO status code is reported.

@Param[in]	context_address - A pointer to the Context memory.

@Return		Status - Success or Failure.
		(\ref CDMA_REFCOUNT_DECREMENT_STATUS).

@remark		Only possible if the address provided with the command is the
		address of the Context.

@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_refcount_decrement(
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

@Return		Status - Success or Failure (e.g. DMA error (\ref
		CDMA_WRITE_LOCK_DMA_READ_AND_INCREMENT_STATUS)).

@remark		None.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_write_lock_dma_read_and_increment(
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

@Return		Status - Success or Failure (e.g. DMA error (\ref
		CDMA_WRITE_RELEASE_LOCK_AND_DECREMENT_STATUS)).

@remark		None.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_write_release_lock_and_decrement(
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

@Return		Status - Success or Failure (e.g. DMA error (\ref
		CDMA_ACCESS_CONTEXT_MEMORY_STATUS)).

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

*//***************************************************************************/
int32_t cdma_access_context_memory(
		uint64_t context_address,
		uint32_t flags,
		uint16_t offset,
		void *ws_address,
		uint16_t dma_param,
		uint32_t *refcount_value);

/** @} end of group CDMA_Internal_Functions */

#endif /* __CDMA_H_ */
