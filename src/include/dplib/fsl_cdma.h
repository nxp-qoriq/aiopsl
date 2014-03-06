/**************************************************************************//**
@File          fsl_cdma.h

@Description   This file contains the AIOP SW CDMA Operations API

@Cautions      None.
*//***************************************************************************/

#ifndef __FSL_CDMA_H
#define __FSL_CDMA_H

#include "common/types.h"
#include "common/errors.h"


/**************************************************************************//**
 @Group		ACCEL ACCEL (Accelerator APIs)

 @Description	AIOP Accelerator APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group		FSL_CDMA CDMA

 @Description	FSL AIOP CDMA macros and functions

 @{
*//***************************************************************************/


/**************************************************************************//**
 @Group		CDMA_Enumerations CDMA Enumerations

 @Description	CDMA Enumerations

 @{
*//***************************************************************************/


/**************************************************************************//**
 @enum cdma_errors

 @Description	AIOP CDMA Error codes.

 @{
*//***************************************************************************/
enum cdma_errors {
		/** Success. */
	CDMA_SUCCESS = E_OK,
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
		/** Increment reference count failed, count is at max value.*/
	CDMA_REFCOUNT_INCREMENT_ERR = 0x90,
		/** Decrement reference count failed, count is at zero. */
	CDMA_REFCOUNT_DECREMENT_ERR = 0x91,
		/** Invalid reference count operation, address is not base
		 * address.*/
	CDMA_REFCOUNT_INVALID_OPERATION_ERR = 0x92,
		/** 0x93 invalid mutex lock request,
		 * the task already has a mutex on this address */
	CDMA_INVALID_MUTEX_LOCK_REQ_ERR = 0x93,
		/** Bound Check failed, DMA access outside the Context
		 * boundaries.*/
	CDMA_BOUND_CHECK_ERR = 0x94,
		/** Invalid mutex lock release, address not found in active
		 * mutex lock list. */
	CDMA_INVALID_MUTEX_RELEASE_ERR = 0x95,
		/** Internal memory ECC uncorrectable ECC error. */
	CDMA_INTERNAL_MEMORY_ECC_ERR = 0xA0,
		/** Workspace memory read Error. */
	CDMA_WORKSPACE_MEMORY_READ_ERR = 0xA1,
		/** Workspace memory write Error. */
	CDMA_WORKSPACE_MEMORY_WRITE_ERR = 0xA2,
		/** External memory read error (permission or ECC). */
	CDMA_EXTERNAL_MEMORY_READ_ERR = 0xA3,
		/** External memory write error (permission or ECC). */
	CDMA_EXTERNAL_MEMORY_WRITE_ERR = 0xA4
};

/* @} end of enum cdma_errors */

/* @} end of group CDMA_Enumerations */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* ++++++++++++++++++++++++  Status/Errors Per Command +++++++++++++++++++*/
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**************************************************************************//**
 @Group		CDMA_Commands_Satus CDMA Commands Status

 @Description	CDMA commands return status

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group		CDMA_READ_STATUS

@Description	CDMA status returned by the cdma_read command

@{
*//***************************************************************************/
/* Success */
#define CDMA_READ__SUCCESS CDMA_SUCCESS
/** Bound Check failed, DMA access outside the Context boundaries.*/
#define CDMA_READ__BOUND_CHECK_ERR \
			CDMA_BOUND_CHECK_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_READ__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* Workspace memory write Error */
#define CDMA_READ__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_READ__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR

/* @} end of group CDMA_READ_STATUS */

/**************************************************************************//**
@Group		CDMA_WRITE_STATUS

@Description	CDMA status returned by the cdma_write command

@{
*//***************************************************************************/
/* Success */
#define CDMA_WRITE__SUCCESS CDMA_SUCCESS
/** Bound Check failed, DMA access outside the Context boundaries.*/
#define CDMA_WRITE__BOUND_CHECK_ERR \
			CDMA_BOUND_CHECK_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_WRITE__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_WRITE__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* External memory write error */
#define CDMA_WRITE__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR

/* @} end of group CDMA_WRITE_STATUS */


/**************************************************************************//**
@Group		CDMA_MUTEX_LOCK_TAKE_STATUS

@Description	CDMA status returned by the cdma_mutex_lock_take command

@{
*//***************************************************************************/
/* Success */
#define CDMA_MUTEX_LOCK_TAKE__SUCCESS CDMA_SUCCESS

/** Mutex lock depletion (max of 4 reached for the task). */
#define CDMA_MUTEX_LOCK_TAKE__MUTEX_DEPLETION_ERR \
			CDMA_MUTEX_DEPLETION_ERR
/** 0x93 invalid mutex lock request,
 * the task already has a mutex on this address */
#define CDMA_MUTEX_LOCK_TAKE__INVALID_MUTEX_LOCK_REQ_ERR \
			CDMA_INVALID_MUTEX_LOCK_REQ_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_MUTEX_LOCK_TAKE__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR

/* @} end of group CDMA_MUTEX_LOCK_TAKE_STATUS */

/**************************************************************************//**
@Group		CDMA_MUTEX_LOCK_RELEASE_STATUS

@Description	CDMA status returned by the cdma_mutex_lock_release command

@{
*//***************************************************************************/
/* Success */
#define CDMA_MUTEX_LOCK_RELEASE__SUCCESS CDMA_SUCCESS
/* Invalid mutex lock release, address not found in active mutex lock list. */
#define CDMA_MUTEX_LOCK_RELEASE__INVALID_MUTEX_RELEASE_ERR \
			CDMA_INVALID_MUTEX_RELEASE_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_MUTEX_LOCK_TAKE__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR

/* @} end of group CDMA_MUTEX_LOCK_RELEASE_STATUS */

/**************************************************************************//**
@Group		CDMA_READ_WITH_MUTEX_STATUS

@Description	CDMA status returned by the cdma_read_with_mutex command

@{
*//***************************************************************************/
/* Success */
#define CDMA_READ_WITH_MUTEX__SUCCESS CDMA_SUCCESS
/** Bound Check failed, DMA access outside the Context boundaries.*/
#define CDMA_READ_WITH_MUTEX__BOUND_CHECK_ERR \
			CDMA_BOUND_CHECK_ERR
/** Mutex lock depletion (max of 4 reached for the task). */
#define CDMA_READ_WITH_MUTEX__MUTEX_DEPLETION_ERR \
			CDMA_MUTEX_DEPLETION_ERR
/** 0x93 invalid mutex lock request,
 * the task already has a mutex on this address */
#define CDMA_MUTEX_LOCK_TAKE__INVALID_MUTEX_LOCK_REQ_ERR \
			CDMA_INVALID_MUTEX_LOCK_REQ_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_READ_WITH_MUTEX__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* Workspace memory write Error */
#define CDMA_READ_WITH_MUTEX__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_READ_WITH_MUTEX__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR

/* @} end of group CDMA_READ_WITH_MUTEX_STATUS */

/* TODO: add to the code a check if DMA failed and release the mutex */

/**************************************************************************//**
@Group		CDMA_WRITE_WITH_MUTEX_STATUS

@Description	CDMA status returned by the cdma_write_with_mutex command

@{
*//***************************************************************************/
/* Success */
#define CDMA_WRITE_WITH_MUTEX__SUCCESS CDMA_SUCCESS
/** Bound Check failed, DMA access outside the Context boundaries.*/
#define CDMA_WRITE_WITH_MUTEX__BOUND_CHECK_ERR \
			CDMA_BOUND_CHECK_ERR
/** Mutex lock depletion (max of 4 reached for the task). */
#define CDMA_WRITE_WITH_MUTEX__MUTEX_DEPLETION_ERR \
			CDMA_MUTEX_DEPLETION_ERR
/** 0x93 invalid mutex lock request,
 * the task already has a mutex on this address */
#define CDMA_WRITE_WITH_MUTEX__INVALID_MUTEX_LOCK_REQ_ERR \
			CDMA_INVALID_MUTEX_LOCK_REQ_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_WRITE_WITH_MUTEX__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_WRITE_WITH_MUTEX__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* External memory write error */
#define CDMA_WRITE_WITH_MUTEX__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR

/* @} end of group CDMA_WRITE_WITH_MUTEX_STATUS */

/* TODO: add to the code a check if DMA failed and release the mutex */

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

/**************************************************************************//**
@Group		CDMA_WS_MEMORY_INIT_STATUS

@Description	CDMA status returned by the cdma_ws_memory_init command

@{
*//***************************************************************************/
/* Success */
#define CDMA_WS_MEMORY_INIT__SUCCESS CDMA_SUCCESS
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_WS_MEMORY_INIT__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* Workspace memory write Error */
#define CDMA_WS_MEMORY_INIT__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_WS_MEMORY_INIT__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR

/* @} end of group CDMA_WS_MEMORY_INIT_STATUS */

/**************************************************************************//**
@Group		CDMA_REFCOUNT_GET_STATUS

@Description	CDMA status returned by the cdma_refcount_get command

@{
*//***************************************************************************/
/* Success */
#define CDMA_REFCOUNT_GET__SUCCESS CDMA_SUCCESS
/** Increment reference count failed, count is at max value.*/
#define CDMA_REFCOUNT_GET__REFCOUNT_INCREMENT_ERR \
			CDMA_REFCOUNT_INCREMENT_ERR
/** Decrement reference count caused the reference count to
 * go to zero. (not an error) */
#define CDMA_REFCOUNT_GET__REFCOUNT_DECREMENT_TO_ZERO \
			CDMA_REFCOUNT_DECREMENT_TO_ZERO
/** Decrement reference count failed, count is at zero. */
#define CDMA_REFCOUNT_GET__REFCOUNT_DECREMENT_ERR \
			CDMA_REFCOUNT_DECREMENT_ERR
/* 0xA0: Internal memory ECC uncorrected ECC error */
#define CDMA_REFCOUNT_GET__INTERNAL_MEMORY_ECC_ERR \
			CDMA_INTERNAL_MEMORY_ECC_ERR
/* Workspace memory write Error */
#define CDMA_REFCOUNT_GET__WORKSPACE_MEMORY_WRITE_ERR \
			CDMA_WORKSPACE_MEMORY_WRITE_ERR
/* 0xA1: Workspace memory read Error */
#define CDMA_REFCOUNT_GET__WORKSPACE_MEMORY_READ_ERR \
			CDMA_WORKSPACE_MEMORY_READ_ERR
/* External memory write error */
#define CDMA_REFCOUNT_GET__EXTERNAL_MEMORY_WRITE_ERR \
			CDMA_EXTERNAL_MEMORY_WRITE_ERR
/* 0xA3: System memory read error (permission or ECC) */
#define CDMA_REFCOUNT_GET__EXTERNAL_MEMORY_READ_ERR \
			CDMA_EXTERNAL_MEMORY_READ_ERR
/** Invalid reference count operation, address is not base address.*/
#define CDMA_REFCOUNT_GET__REFCOUNT_INVALID_OPERATION_ERR \
			CDMA_REFCOUNT_INVALID_OPERATION_ERR
/* @} end of group CDMA_REFCOUNT_GET_STATUS */


/* @} end of group CDMA_Commands_Satus */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* ^^^^^^^^^^^^^^^^^^ End of Status/Errors Per Command ^^^^^^^^^^^^^^^^^^^*/
/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**************************************************************************//**
 @Group		CDMA_Commands_Flags CDMA Commands Flags

 @Description	CDMA Commands Flags

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group			CDMA_DMA_MUTEX_ModeBits

@Description	DMA Mutex flags

| 0 - 18|        19 - 20       |         23         | 24 - 31 |
|-------|----------------------|--------------------|---------|
|       |  Pre DMA Mutex Lock  | Post DMA Rel-Mutex |         |


@{
*//***************************************************************************/

/* Pre DMA Mutex Lock */
		/* No mutex lock is requested. */
#define CDMA_DMA_NO_MUTEX_LOCK		0x00000000
		/* Mutex read lock is requested. */
#define CDMA_PREDMA_MUTEX_READ_LOCK	0x00000800
		/* Mutex write lock is requested. */
#define CDMA_PREDMA_MUTEX_WRITE_LOCK	0x00001000

/* Post DMA Rel-Mutex */
		/** Post DMA Release Mutex Lock.
		- 0: No mutex lock to release.
		- 1: Release mutex lock. */
#define CDMA_POSTDMA_MUTEX_RM_BIT	0x00000100

/* @} end of group CDMA_DMA_MUTEX_ModeBits */


/**************************************************************************//**
@Group			CDMA_MUTEX_ModeBits

@Description	Mutex Lock flags

@{
*//***************************************************************************/

/* Mutex Lock */
		/* Mutex read lock is requested. */
#define CDMA_MUTEX_READ_LOCK	0x00000800
		/* Mutex write lock is requested. */
#define CDMA_MUTEX_WRITE_LOCK	0x00001000

/* @} end of group CDMA_MUTEX_ModeBits */

/* @} end of group CDMA_Commands_Flags */


/**************************************************************************//**
@Group		CDMA_Functions	CDMA Functions

@Description	AIOP CDMA operations functions.

@{
*//***************************************************************************/

/*************************************************************************//**
@Function	cdma_read

@Description	This routine is used to read data from context memory to
		Workspace.

		The read data access will be done regardless of any mutex lock
		that was set by another task.

@Param[in]	ws_dst - A pointer to the Workspace.
@Param[in]	ext_address - A pointer to a context memory address in the
		external memory (DDR/PEB).
@Param[in]	size - Read data access size, in bytes.

@Return		Status - Success or Failure (e.g. DMA error (\ref
		CDMA_READ_STATUS)).

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_read(
		void *ws_dst,
		uint64_t ext_address,
		uint16_t size);


/*************************************************************************//**
@Function	cdma_write

@Description	This routine writes data from Workspace to context memory.

		The write data access will be done regardless of any mutex
		lock that was set by another task.

@Param[in]	ext_address - A pointer to a context memory address in the
		external memory (DDR/PEB).
@Param[in]	ws_src - A pointer to the Workspace.
@Param[in]	size - Write data access size, in bytes.

@Return		Status - Success or Failure (e.g. DMA error (\ref
		CDMA_WRITE_STATUS)).

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_write(
		uint64_t ext_address,
		void *ws_src,
		uint16_t size);


/*************************************************************************//**
@Function	cdma_mutex_lock_take

@Description	This routine sets Mutex lock on 64 bits mutex ID.
		Support for mutex lock access, both read lock and write lock.
		Write lock is granted when there are no preceding locks (read
		or write) active or pending on the same mutex ID.
		Read locks are granted when there are no preceding write locks
		active or pending on the same mutex ID.
		If the lock can not be obtained, the function waits and return
		only when the requested mutex lock is obtained.

@Param[in]	mutex_id - A 64 bits mutex ID. Can be a pointer to the
		internal/external memory (DDR/PEB/shared SRAM).
@Param[in]	flags - \link CDMA_MUTEX_ModeBits CDMA Mutex flags \endlink

@Return		Status - Success or Failure. (\ref CDMA_MUTEX_LOCK_TAKE_STATUS).

@remark
		- Each task can have a maximum of 4 simultaneous mutex locks
		active.
		- When a task is terminated, the CDMA will release any active
		mutex lock(s) associated with the task to avoid mutex lock
		leak.

@Cautions	A mutex lock taken by a task must be released by the same task.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_mutex_lock_take(
		uint64_t mutex_id,
		uint32_t flags);


/*************************************************************************//**
@Function	cdma_mutex_lock_release

@Description	This routine release mutex ID lock (read or write).

@Param[in]	mutex_id - A 64 bits mutex ID. Can be a pointer to the
		internal/external memory (DDR/PEB/shared SRAM).

@Return		Status - Success or Failure.
		(\ref CDMA_MUTEX_LOCK_RELEASE_STATUS).

@remark		Each task can have a maximum of 4 simultaneous mutex locks
		active.

@Cautions	A mutex lock taken by a task must be released by the same task.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_mutex_lock_release(
		uint64_t mutex_id);


/*************************************************************************//**
@Function	cdma_read_with_mutex

@Description	This routine reads data from external memory to Workspace.
		Optional mutex lock (read or write) request before the read
		transaction occurs and/or mutex lock release after the read
		transaction occurred.

		Write lock is granted when there are no preceding locks (read
		or write) active or pending on the same address.
		Read locks are granted when there are no preceding write locks
		active or pending on the same address.

		Reading data access is granted only if no mutex write lock is
		preceding the same ext_address. In this case, reading data
		access will take place only when the write lock is released.
		i.e. this routine returns only when the read data access is
		done.

@Param[in]	ext_address - A pointer to a context memory address in the
		external memory (DDR/PEB). This address is used to read data
		access and mutex lock take/release.
@Param[in]	flags - \link CDMA_DMA_MUTEX_ModeBits CDMA Mutex flags
		\endlink.
@Param[in]	ws_dst - A pointer to the Workspace.
@Param[in]	size - Read data access size, in bytes.

@Return		Status - Success or Failure (e.g. DMA error (\ref
		CDMA_READ_WITH_MUTEX_STATUS)).

@remark
		- Each task can have a maximum of 4 simultaneous mutex locks
		active.
		- A mutex lock taken by a task must be released by the same
		task.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_read_with_mutex(
		uint64_t ext_address,
		uint32_t flags,
		void *ws_dst,
		uint16_t size);


/*************************************************************************//**
@Function	cdma_write_with_mutex

@Description	This routine writes data from Workspace to external memory.
		Optional write mutex lock request before the write transaction
		occurs and/or mutex lock release after the write transaction
		occurred.

		Write lock is granted when there are no preceding locks (read
		or write) active or pending on the same address.

		Writing data access is granted only if no mutex read/write lock
		is preceding the same destination address. In this case,
		writing data access will take place only when the write lock is
		released.

@Param[in]	ext_address - A pointer to a context memory address in the
		external memory (DDR/PEB). This address is used to write data
		access and mutex lock take/release.
@Param[in]	flags - \link CDMA_DMA_MUTEX_ModeBits CDMA Mutex flags
		\endlink.
@Param[in]	ws_src - A pointer to the Workspace.
@Param[in]	size - Read data access size, in bytes.

@Return		Status - Success or Failure (e.g. DMA error (\ref
		CDMA_WRITE_WITH_MUTEX_STATUS)).

@remark
		- Each task can have a maximum of 4 simultaneous mutex locks
		active.
		- A mutex lock taken by a task must be released by the same
		task.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_write_with_mutex(
		uint64_t ext_address,
		uint32_t flags,
		void *ws_src,
		uint16_t size);


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
@Function	cdma_ws_memory_init

@Description	This routine writes a provided pattern into a range of
		workspace memory.

@Param[in]	ws_dst - A pointer to the Workspace.
@Param[in]	size - Data initialization size, in bytes.
@Param[in]	data_pattern - Data to write in workspace.

@Return		Status - Success or Failure. (\ref CDMA_WS_MEMORY_INIT_STATUS).

@Cautions	If size is not a multiple  of 4 bytes, the MSBs of
		the data_pattern are used to complete the initialization.
@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_ws_memory_init(
		void *ws_dst,
		uint16_t size,
		uint32_t data_pattern);


/*************************************************************************//**
@Function	cdma_refcount_get 

@Description	This routine returns the current value of reference count.

@Param[in]	context_address - A pointer to the Context memory.

@Param[out]	refcount_value - Current value of reference count.

@Return		Status - Success or Failure.
		(\ref CDMA_REFCOUNT_GET_STATUS).

@remark		This function is for verification only.

@Cautions	In this function the task yields.

*//***************************************************************************/
int32_t cdma_refcount_get(
		uint64_t context_address,
		uint16_t *refcount_value);

/** @} end of group CDMA_Functions */
/** @} */ /* end of FSL_CDMA */
/** @} */ /* end of ACCEL */


#endif /* __FSL_CDMA_H */
