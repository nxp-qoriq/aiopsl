/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
 * @file    fsl_shbp.h
 * @brief   Shared Buffer Pool API
 *
 * This is uniform API for GPP and AIOP.
 *
 */

#ifndef __FSL_SHBP_H
#define __FSL_SHBP_H

#include <shbp.h>
#include <cmdif.h> /* TODO rename cmdif.h to arch.h */

/*!
 * @Group	shbp_g  Shared Buffer Pool API
 *
 * @brief	API to be used for shared buffer pool.
 *
 * @{
 */

/**
 * @brief	Get buffer from shared pool
 *
 * @param[in]	bp - Buffer pool handle
 *
 * @returns	Address on Success; or NULL code otherwise
 *
 */
uint64_t shbp_acquire(shbp_t bp);

/**
 * @brief	Return buffer into shared pool
 *
 * @param[in]	bp  - Buffer pool handle
 * @param[in]	buf - Buffer address
 *
 * @returns	0 on Success; or error code otherwise
 *
 */
int shbp_release(shbp_t bp, uint64_t buf);

/**
 * @brief	Create shared pool from a given buffer
 *
 * @param[in]	buf  - Buffer address
 * @param[in]	size - buf size
 * @param[in]	buf_size - Size of each buffer in pool
 * @param[in]	flags - Flags to be used for pool creation

 * @returns	Address on Success; or NULL otherwise
 *
 */
shbp_t shbp_create(uint64_t buf, uint32_t size,
			uint32_t buf_size, uint32_t flags);

/**
 * @brief	Move freed buffers into allocation queue
 *
 * @param[in]	bp  - Buffer pool handle
 *
 * @returns	Number of the buffers added to the allocation queue
 *
 */
int shbp_fill(shbp_t bp);

/** @} */ /* end of shbp_g group */

#endif
