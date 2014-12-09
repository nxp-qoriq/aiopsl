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
 * @file    shbp.h
 * @brief   Shared Buffer Pool API
 *
 * Internal header file shared by AIOP and GPP.
 *
 */

#ifndef __SHBP_H
#define __SHBP_H

#include <cmdif.h>

/**
 * @brief	Structure representing buffer descriptor
 */
struct shbp_bd {
	uint64_t addr; /*!< Buffer address after meta data */
};

/**
 * @brief	Structure representing buffer meta data
 */
struct shbp_bd_meta {
	uint8_t alloc_master;	/*!< Master of the allocation */
	uint8_t creator_id;	/*!< Creator pool id */
	uint8_t alloc_id;	/*!< Allocation owner pool id */
};

/**
 * @brief	Structure representing one ring
 */
struct shbp_q  {
	uint64_t base;	/*!< Base address of the pool */
	uint32_t enq;	/*!< Number of enqueued buffers */
	uint32_t deq;	/*!< Number of dequeued buffers */
};

/**
 * @brief	Structure representing shared buffer pool
 * 		Must reside in non cacheable memory.
 */
struct shbp {
	uint64_t mem_ptr;
	/*!< Pointer to memory as received at shbp_create */
	struct shbp_q alloc;
	/*!< Allocations queue */
	struct shbp_q free;
	/*!< Free queue */
	uint8_t alloc_master;	/*!< Master of the allocation */
	uint8_t align;		/*!< Buffer alignment is 2^align */
	uint8_t size;		/*!< Max number of BDs in the pool is 2^size */
	uint8_t reserved[21];
};

#define SHBP_ALLOC_IS_FULL(BP)	(((BP)->alloc.enq - (BP)->alloc.deq) == (BP)->size)
#define SHBP_ALLOC_IS_EMPTY(BP)	(((BP)->alloc.enq - (BP)->alloc.deq) == 0)
#define SHBP_FREE_IS_EMPTY(BP)	(((BP)->free.enq - (BP)->free.deq) == 0)
#define SHBP_SIZE(BP)		(0x1 << (BP)->size)
#define SHBP_SIZE_BYTES(BP)	(SHBP_SIZE(BP) << 3)

#endif /* _SHBP_H */
