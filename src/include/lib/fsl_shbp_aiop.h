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
 * @file    fsl_shbp_aiop.h
 * @brief   Shared Buffer Pool API
 *
 * This is shared pool API for AIOP.
 *
 */

#ifndef __FSL_SHBP_AIOP_H
#define __FSL_SHBP_AIOP_H

#include <shbp_aiop.h>

/*!
 * @Group	shbp_aiop_g  Shared Buffer Pool API for AIOP
 *
 * @brief	API to be used for shared buffer pool.
 *
 * @{
 */

/**
 * @brief	Get buffer from shared pool
 *
 * @param[in]	bp - AIOP buffer pool handle
 *
 * @returns	Address on Success; or NULL code otherwise
 *
 */
uint64_t shbp_acquire(struct shbp_aiop *bp);

/**
 * @brief	Return buffer into shared pool
 *
 * @param[in]	bp  - AIOP buffer pool handle
 * @param[in]	buf - Buffer address
 *
 * @returns	0 on Success; or error code otherwise
 *
 */
int shbp_release(struct shbp_aiop *bp, uint64_t buf);

/**
 * @brief	Enable shared buffer pool for AIOP usage 
 * 
 * Must be called before shared pool usage on AIOP 
 *
 * @param[in]	swc_id    - Software context id (DPCI) of this shared pool
 * @param[in]	shbp_iova - I/O virtual address of shared pool as received 
 * 		from GPP (pointer to struct shbp), should be in Big Endian
 * @param[out]	bp        - AIOP buffer pool handle, must reside in SHRAM
 * @returns	0 on Success; or error code otherwise
 *
 */
int shbp_enable(uint16_t swc_id, uint64_t shbp_iova, struct shbp_aiop *bp);


/** @} */ /* end of shbp_aiop_g group */

#endif
