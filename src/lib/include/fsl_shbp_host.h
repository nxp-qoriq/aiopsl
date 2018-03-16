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
 * @file    fsl_shbp_host.h
 * @brief   Shared Buffer Pool wrapper API for fsl_shbp_flib.h
 *
 *
 */

#ifndef __FSL_SHBP_HOST_H
#define __FSL_SHBP_HOST_H


/*!
 * @Group	shbp_host_g  Shared Buffer Pool API
 *
 * @brief	API to be used by GPP application for shared buffer pool.
 *
 * @{
 */

#define SHBP_HOST_IS_MASTER	0x1	/*!< Host is the allocation master */
#define SHBP_HOST_SAFE		0x2	/*!< Host API is multithread safe */


/**
 * @brief	Get buffer from shared pool
 *
 * @param[in]	bp - Buffer pool handle
 *
 * @returns	Address on Success; or NULL code otherwise
 *
 */
void *shbp_acquire(uint64_t bp);

/**
 * @brief	Return or add buffer into the shared pool
 *
 * @param[in]	bp  - Buffer pool handle
 * @param[in]	buf - Pointer to buffer
 *
 * @returns	0 on Success; or POSIX error code otherwise
 *
 */
int shbp_release(uint64_t bp, void *buf);

/**
 * @brief	Create full shared pool
 *
 *
 * @param[in]	flags    - Flags to be used for pool creation, 0 means AIOP is
 * 		the allocation master, #SHBP_HOST_IS_MASTER means GPP is
 * 		the allocation master and can acquire buffers from it,
 * 		#SHBP_HOST_SAFE makes it thread safe.
 * @param[in]	buf_num  - Maximal number of buffers in pool.
 * 		Must be power of 2.
 * @param[out]  bp       - Pointer to shared pool handle
 * @returns	0 on Success; or POSIX error code otherwise
 *
 */
int shbp_create(uint32_t flags, uint32_t buf_num, uint32_t buf_size,
		uint16_t alignment, uint64_t *bp);

/**
 * @brief	Move free buffers into allocation queue
 *
 * @param[in]	bp  - Buffer pool handle
 *
 * @returns	POSIX error code on failure or the number of the buffers added
 * 		to the allocation queue
 *
 */
int shbp_refill(uint64_t bp);


/**
 * @brief	Destroys shbp and frees all the memory.
 *
 * After this call the bp is no longer valid.
 *
 * @param[in]	bp       - Buffer pool handle
 *
 * @returns	0 on Success; or POSIX error code otherwise
 *
 */
int shbp_destroy(uint64_t bp);

/** @} */ /* end of shbp_g group */

#endif /* __FSL_SHBP_HOST_H */
