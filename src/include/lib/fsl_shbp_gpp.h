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

/*!
 * @file    fsl_shbp_gpp.h
 * @brief   Shared Buffer Pool wrapper API for fsl_shbp.h
 *
 *
 */

#ifndef __FSL_SHBP_GPP_H
#define __FSL_SHBP_GPP_H

#include <fsl_shbp.h>

/*!
 * @Group	shbp_gpp_g  Shared Buffer Pool API
 *
 * @brief	API to be used by GPP application for shared buffer pool.
 *
 * @{
 */

struct shbp_gpp;

/**
 * @brief	Get the shared handle for this shared pool
 *
 * @param[in]	bp - AIOP buffer pool handle
 *
 * @returns	The address of the shared handle; or NULL code otherwise
 *
 */
void *shbp_gpp_get(struct shbp_aiop *bp);

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
uint32_t shbp_gpp_mem_ptr_size(uint32_t num_bufs);

/**
 * @brief	Get buffer from shared pool
 *
 * @param[in]	bp - Buffer pool handle
 *
 * @returns	Address on Success; or NULL code otherwise
 *
 */
void *shbp_gpp_acquire(struct shbp_gpp *bp);

/**
 * @brief	Return or add buffer into the shared pool
 *
 * @param[in]	bp  - Buffer pool handle
 * @param[in]	buf - Pointer to buffer
 *
 * @returns	0 on Success; or POSIX error code otherwise
 *
 */
int shbp_gpp_release(struct shbp_gpp *bp, void *buf);

/**
 * @brief	Create empty shared pool
 *
 * The shared pool is created as empty, use shbp_gpp_release() to fill it
 *
 * @param[in]	flags    - Flags to be used for pool creation, 0 means AIOP is
 * 		the allocation master, #SHBP_GPP_MASTER means GPP is
 * 		the allocation master.
 * @param[in]	buf_num  - Number of buffers, maximal pool capacity
 * @param[out]  bp       - Pointer to shared pool handle
 * @returns	0 on Success; or POSIX error code otherwise
 *
 */
int shbp_gpp_create(uint32_t flags, uint32_t buf_num, struct shbp_gpp *bp);

/**
 * @brief	Move free buffers into allocation queue
 *
 * @param[in]	bp  - Buffer pool handle
 *
 * @returns	POSIX error code on failure or the number of the buffers added
 * 		to the allocation queue
 *
 */
int shbp_gpp_refill(struct shbp_gpp *bp);


/**
 * @brief	Returns the pointers from pool that need to be freed upon pool
 * 		destruction
 *
 * Pointer to struct shbp will not be returned by shbp_destroy() but it
 * must be freed by user
 *
 * @param[in]	bp       - Buffer pool handle
 *
 * @returns	0 on Success; or POSIX error code otherwise
 *
 */
int shbp_gpp_destroy(struct shbp_gpp *bp);

/** @} */ /* end of shbp_g group */

#endif
