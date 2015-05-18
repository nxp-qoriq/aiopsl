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
@File          fsl_slab.h

@Description   External prototypes for the buffer pool manager
 *//***************************************************************************/

#ifndef __FSL_SLAB_H
#define __FSL_SLAB_H

#include "common/types.h"
#include "fsl_cdma.h"
#include "fsl_platform.h"

/**************************************************************************//**
@Group         slab_g   SLAB

@Description   Slab Memory Manager module functions and definitions.

@{
 *//***************************************************************************/

/* Each buffer is of the following structure:
 *
 *
 *  +------------------+--------------------------------------+-----------+
 *  |  HW metadata     |             Data field               | Alignment |
 *  |  8 Bytes         |             (align)                  | Padding   |
 *  +------------------+--------------------------------------+-----------+
 *  Alignment padding may exist in the end of the buffer.
 */

/**************************************************************************//**
@Description   Slab handle type
*//***************************************************************************/
struct slab;

/**Flag to use for requesting slab managed in DDR (lower performance)*/
#define SLAB_DDR_MANAGEMENT_FLAG    0x01

/** Decrement reference count caused the reference count to
	go to zero. (not an error) */
#define SLAB_CDMA_REFCOUNT_DECREMENT_TO_ZERO 0x3


/**************************************************************************//**
@Description   Available debug information about every slab
*//***************************************************************************/
struct slab_debug_info {
	/** Maximal buffer size */
	uint32_t buff_size;
	/** The number of available buffers */
	uint32_t committed_buffs;
	/** Maximal number of buffers inside this pool */
	uint32_t max_buffs;
	/** HW pool ID */
	uint16_t pool_id;
	/** Maximal alignment */
	uint16_t alignment;
	/** Memory partition */
	uint16_t mem_pid;
};

/**************************************************************************//**
@Description	Type of the function callback to be called on release of buffer
		into pool
*//***************************************************************************/
typedef void (slab_release_cb_t)(uint64_t);

/**************************************************************************//**
@Function	slab_create

@Description	Create a new buffers pool.

@Param[in]	committed_buffs     Number of buffers in new pool.
@Param[in]	max_buffs           Maximal number of buffers that
		can be allocated by this new pool; max_buffs >= committed_buffs.
@Param[in]	buff_size           Size of buffers in pool, actual buffer size
		will be 8 bytes larger, to accommodate for metadata.
@Param[in]	alignment           Requested alignment for buffer in bytes.
@Param[in]	mem_partition_id    Memory partition ID for allocation.
		AIOP: HW pool supports only PEB and DPAA DDR.
@Param[in]	flags               Set it to 0 for default slab creation.
		For extra pools managed in DDR with lower
		performance set: SLAB_DDR_MANAGEMENT_FLAG
@Param[in]	release_cb          Function to be called on release of buffer
@Param[out]	slab                Handle to new pool is returned through here.

@Cautions	The alignment starts from data field and must be a power of 2.
		Buffer size + 8 bytes HW metadata and alignment should match
		(not higher than) the inputs that were pre-registered in:
		slab_register_context_buffer_requirements() function.
@Return		0        - on success,
		-ENAVAIL - resource not available or not found,
		-ENOMEM  - not enough memory for mem_partition_id
 *//***************************************************************************/
int slab_create(uint32_t    committed_buffs,
		uint32_t    max_buffs,
		uint16_t    buff_size,
		uint16_t    alignment,
		enum memory_partition_id  mem_partition_id,
		uint32_t    flags,
		slab_release_cb_t *release_cb,
		struct slab **slab);

/**************************************************************************//**
@Function	slab_free

@Description	Free a specific pool and all it's buffers.

@Param[in]	slab - Handle to memory pool.

@Return		0      - on success,
		-EBUSY  - this slab can't be freed
		-EINVAL - not a valid slab handle
 *//***************************************************************************/
int slab_free(struct slab **slab);

/**************************************************************************//**
@Function	slab_acquire

@Description	Get a buffer of memory from a pool;
		AIOP HW pool buffer reference counter will be set to 1.

@Param[in]	slab - Handle to memory pool.
@Param[out]	buff - The acquired buffer from HW pool.

@Return		0      - on success,
		-ENOMEM - no buffer available,
		-EINVAL - not a valid slab handle
 *//***************************************************************************/
int slab_acquire(struct slab *slab, uint64_t *buff);

/**************************************************************************//**
@Function	slab_release

@Description	Return the buffer back to a pool;
		AIOP HW pool buffer reference counter must be 0, it
		is NOT decremented.

@Param[in]	slab - Handle to memory pool.
@Param[in]	buff - The buffer to return.

@Return		0      - on success,
		-EINVAL - not a valid slab handle
		-EFAULT - bad address, trying to release to wrong slab
*//***************************************************************************/
int slab_release(struct slab *slab, uint64_t buff);

/**************************************************************************//**
@Function	slab_refcount_incr

@Description	Increment buffer reference counter

@Param[in]	buff - The buffer for which to increment reference counter.

*//***************************************************************************/
inline void slab_refcount_incr(uint64_t buff){
	cdma_refcount_increment(buff);
}

/**************************************************************************//**
@Function	slab_refcount_decr

@Description	Decrement buffer reference counter;
		The buffer is not released if reference counter is drops to 0.
		Use slab_release() to release the buffer.

@Param[in]	buff - The buffer for which to decrement reference counter.

@Return		0       - on success,
		#SLAB_CDMA_REFCOUNT_DECREMENT_TO_ZERO - On success and the
		reference counter is 0.
*//***************************************************************************/
inline int slab_refcount_decr(uint64_t buff){
	return cdma_refcount_decrement(buff);
}

/**************************************************************************//**
@Function	slab_debug_info_get

@Description	Decrement buffer reference counter if such exists
		and return the buffer back to a pool.

@Param[in]	slab - Handle to memory pool.
@Param[out]	slab_info - The pointer to place the debug information.

@Return		0       - on success,
		-EINVAL - invalid parameter.
*//***************************************************************************/
int slab_debug_info_get(struct slab *slab, struct slab_debug_info *slab_info);


/**************************************************************************//**
@Function	slab_register_context_buffer_requirements

@Description	register a request for buffers requirement.

@Param[in]	committed_buffs     Number of buffers needed for the app.
@Param[in]	max_buffs           Maximal number of buffers that
		can be allocated by the app; max_buffs >= committed_buffs;
@Param[in]	buff_size           Size of buffers in pool, actual buffer size
		will be 8 bytes larger, to accommodate for metadata.
@Param[in]	alignment           Requested alignment for buffer in bytes.
@Param[in]	mem_pid             Memory partition ID for allocation.
		AIOP: HW pool supports only PEB and DPAA DDR.
@Param[in]	flags               Set it to 0 for default.
@Param[in]	num_ddr_pools       Number of pools needed in the future
		(managed in DDR - slow performance).
@Cautions	Max buffer size supported - 32760 Byte. Actual buffer size will
		be 8 bytes larger, to accommodate for metadata (32768).
		Buffer size must be higher than 0.
		Max alignment supported 32768 Byte (minimum is 0).
		0 <= Alignment <= Buffer size + Meta data.
		The alignment starts from data field and must be a power of 2.
@Return		0        - on success,
		-ENAVAIL - resource not available or not found,
		-ENOMEM  - not enough memory for requested memory partition
 *//***************************************************************************/
int slab_register_context_buffer_requirements(uint32_t    committed_buffs,
                                              uint32_t    max_buffs,
                                              uint16_t    buff_size,
                                              uint16_t    alignment,
                                              enum memory_partition_id  mem_pid,
                                              uint32_t    flags,
                                              uint32_t    num_ddr_pools);
/** @} *//* end of slab_g group */

#endif /* __FSL_SLAB_H */
