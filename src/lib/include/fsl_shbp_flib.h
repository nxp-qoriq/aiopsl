/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @file    fsl_shbp.h
 * @brief   Shared Buffer Pool API for the side that creates the pool (not AIOP)
 *
 *
 */

#ifndef __FSL_SHBP_FLIB_H
#define __FSL_SHBP_FLIB_H

#include <shbp_flib.h>

/*!
 * @Group	shbp_flib_g  Shared Buffer Pool API
 *
 * @brief	API to be used for shared buffer pool.
 *
 * @{
 */

/*!
 * @details	Calculator for 'mem_ptr' size for shbp_create(). num_bufs must
 *		be 2^x and higher than 8.
 *
 * @param[in]	num_bufs - Number of buffers which will be added into the
 *		SHBP pool. num_bufs must be 2^x and higher than 8.
 *
 * @returns	The 'mem_ptr' size required by shbp_create()
 *
 */
uint32_t shbp_flib_mem_ptr_size(uint32_t num_bufs);

/**
 * @brief	Get buffer from shared pool
 *
 * @param[in]	bp - Buffer pool handle
 *
 * @returns	Address on Success; or NULL code otherwise
 *
 */
void *shbp_flib_acquire(uint64_t bp);

/**
 * @brief	Return or add buffer into the shared pool
 *
 * @param[in]	bp  - Buffer pool handle
 * @param[in]	buf - Pointer to buffer
 *
 * @returns	0 on Success; or POSIX error code otherwise
 *
 */
int shbp_flib_release(uint64_t bp, void *buf);

/**
 * @brief	Create shared pool from a given buffer
 *
 * The shared pool is created as empty, use shbp_release() to fill it
 *
 * @param[in]	mem_ptr  - Pointer to memory to be used for shared management;
 * 		it should be aligned to cache line.
 * 		It must be from Write-Back Cacheable and Outer Shareable memory
 *
 * @param[in]	size     - Size of mem_ptr
 * @param[in]	flags    - Flags to be used for pool creation, 0 means AIOP is
 * 		the allocation master. See #SHBP_GPP_MASTER.
 * @param[out]  bp       - Pointer to shared pool handle
 *
 * @returns	0 on Success; or POSIX error code otherwise
 *
 *
 */
int shbp_flib_create(void *mem_ptr, uint32_t size, uint32_t flags,
		     uint64_t *bp);

/**
 * @brief	Move free buffers into allocation queue
 *
 * @param[in]	bp  - Buffer pool handle
 *
 * @returns	POSIX error code on failure or the number of the buffers added
 * 		to the allocation queue
 *
 */
int shbp_flib_refill(uint64_t bp);


/**
 * @brief	Returns the pointers from pool that need to be freed upon pool
 * 		destruction
 *
 * Pointer to struct shbp will not be returned by shbp_destroy() but it
 * must be freed by user
 *
 * @param[in]	bp       - Buffer pool handle
 * @param[out]	ptr      - Pointer to be freed for pool destruction
 *
 * @returns	POSIX error code until there are buffers inside shared pool
 * 		that need to be freed, 0 if there are no buffers to be freed
 *
 */
int shbp_flib_destroy(uint64_t bp, void **ptr);

/** @} */ /* end of shbp_flib_g group */

#endif
