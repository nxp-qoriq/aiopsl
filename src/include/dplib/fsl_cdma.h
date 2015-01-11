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
@File          fsl_cdma.h

@Description   This file contains the AIOP SW CDMA Operations API

@Cautions      None.
*//***************************************************************************/

#ifndef __FSL_CDMA_H
#define __FSL_CDMA_H

#include "common/types.h"
#include "fsl_errors.h"


/**************************************************************************//**
 @Group		ACCEL Accelerators APIs

 @Description	AIOP Accelerators APIs

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group		FSL_CDMA CDMA

 @Description	FSL AIOP CDMA macros and functions

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group		CDMA_Commands_Flags CDMA Commands Flags

 @Description	CDMA Commands Flags

 @{
*//***************************************************************************/

/**************************************************************************//**
@Group			CDMA_DMA_MUTEX_ModeBits CDMA DMA MUTEX ModeBits

@Description	DMA Mutex flags

| 0 - 18|        19 - 20       |         23         | 24 - 31 |
|-------|----------------------|--------------------|---------|
|       |  Pre DMA Mutex Lock  | Post DMA Rel-Mutex |         |


@{
*//***************************************************************************/

/** Pre DMA Mutex Lock */
		/** No mutex lock is requested. */
#define CDMA_DMA_NO_MUTEX_LOCK		0x00000000
		/** Mutex read lock is requested. */
#define CDMA_PREDMA_MUTEX_READ_LOCK	0x00000800
		/** Mutex write lock is requested. */
#define CDMA_PREDMA_MUTEX_WRITE_LOCK	0x00001000

/** Post DMA Rel-Mutex */
		/** Post DMA Release Mutex Lock.
		- 0: No mutex lock to release.
		- 1: Release mutex lock. */
#define CDMA_POSTDMA_MUTEX_RM_BIT	0x00000100

/** @} end of group CDMA_DMA_MUTEX_ModeBits */


/**************************************************************************//**
@Group			CDMA_MUTEX_ModeBits  CDMA MUTEX ModeBits

@Description	Mutex Lock flags

@{
*//***************************************************************************/

/** Mutex Lock */
		/** Mutex read lock is requested. */
#define CDMA_MUTEX_READ_LOCK	0x00000800
		/** Mutex write lock is requested. */
#define CDMA_MUTEX_WRITE_LOCK	0x00001000

/** @} end of group CDMA_MUTEX_ModeBits */

/** @} end of group CDMA_Commands_Flags */


/**************************************************************************//**
@Group		CDMA_Functions	CDMA Functions

@Description	AIOP CDMA operations functions.

@{
*//***************************************************************************/

#include "cdma_inline.h"

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

@Return		None.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
inline void cdma_read(
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

@Return		None.

@Cautions	The maximum legal access size (in bytes) is 0x3FFF.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
void cdma_write(
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

@Return		None.

@remark
		- Each task can have a maximum of 4 simultaneous mutex locks
		active.
		- When a task is terminated, the CDMA will release any active
		mutex lock(s) associated with the task to avoid mutex lock
		leak.

@Cautions	A mutex lock taken by a task must be released by the same task.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
inline void cdma_mutex_lock_take(
		uint64_t mutex_id,
		uint32_t flags);


/*************************************************************************//**
@Function	cdma_mutex_lock_release

@Description	This routine release mutex ID lock (read or write).

@Param[in]	mutex_id - A 64 bits mutex ID. Can be a pointer to the
		internal/external memory (DDR/PEB/shared SRAM).

@Return		None.

@remark		Each task can have a maximum of 4 simultaneous mutex locks
		active.

@Cautions	A mutex lock taken by a task must be released by the same task.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
inline void cdma_mutex_lock_release(
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
inline void cdma_read_with_mutex(
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
void cdma_write_with_mutex(
		uint64_t ext_address,
		uint32_t flags,
		void *ws_src,
		uint16_t size);


/*************************************************************************//**
@Function	cdma_ws_memory_init

@Description	This routine writes a provided pattern into a range of
		workspace memory.

@Param[in]	ws_dst - A pointer to the Workspace.
@Param[in]	size - Data initialization size, in bytes.
@Param[in]	data_pattern - Data to write in workspace.

@Return		None.

@Cautions	If size is not a multiple  of 4 bytes, the MSBs of
		the data_pattern are used to complete the initialization.
@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
void cdma_ws_memory_init(
		void *ws_dst,
		uint16_t size,
		uint32_t data_pattern);


/*************************************************************************//**
@Function	cdma_refcount_get

@Description	This routine returns the current value of reference count.

@Param[in]	context_address - A pointer to the Context memory.

@Param[out]	refcount_value - Current value of reference count.

@Return		None.

@remark		This function is for verification only.
@remark		This function can be used only if the context memory was read
		and the reference count was incremented by this task before
		calling this function.

@Cautions	In this function the task yields.
@Cautions	This function may result in a fatal error.

*//***************************************************************************/
void cdma_refcount_get(
		uint64_t context_address,
		uint32_t *refcount_value);

/** @} end of group CDMA_Functions */
/** @} */ /* end of FSL_CDMA */
/** @} */ /* end of ACCEL */


#endif /* __FSL_CDMA_H */
