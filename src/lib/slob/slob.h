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

/****************************************************************
 *
 * File:  slob.h
 *
 *
 * Description:
 *  MM (Memory Management) object definitions.
 *  It also includes definitions of the Free Block, Busy Block
 *  and Memory Block structures used by the MM object.
 *
 ****************************************************************/

#ifndef __SLOB_H
#define __SLOB_H

#include "fsl_errors.h"
#include "fsl_slob.h"



#define MAKE_ALIGNED(addr, align)    \
    (((uint64_t)(addr) + ((align) - 1)) & (~(((uint64_t)align) - 1)))


/* t_MemBlock data structure defines parameters of the Memory Block */
typedef struct t_mem_block {
    uint64_t  base;                 /* Base address of the memory block */
    uint64_t  end;                  /* End address of the memory block */
    uint64_t  next_addr;            /*  Address of the next memory block */
} t_mem_block;


/* t_FreeBlock data structure defines parameters of the Free Block */
typedef struct t_slob_block
{
    uint64_t  base;                 /* Base address of the block */
    uint64_t  end;                  /* End address of the block */
    uint64_t  next_addr;            /*  Address of the next free block */
} t_slob_block;



/* t_MM data structure defines parameters of the MM object */
typedef struct t_MM
{
    uint64_t 	     head_mem_blocks; /* List of memory blocks (Memory list) */
    /*t_slob_block     *busy_blocks; */
    uint64_t         head_busy_blocks_addr;
                     /* List of busy blocks (Busy list) */
    /*t_slob_block     *free_blocks[MM_MAX_ALIGNMENT + 1];*/
    uint64_t         head_free_blocks_addr;
                     /* Alignment lists of free blocks (Free lists) */
    uint64_t        free_mem_size;    /* Total size of free memory (in bytes) */
    void *          h_mem_mng; // boot memory manager
} t_MM;


#ifdef SL_DEBUG
/**************************************************************************//**
@Function	slob_dump

@Description	Dumps a memory slob, printing the list of busy blocks and the
		alignment lists of free blocks.
		Note : If application calls this function, application and
		AIOP_SL must be build with the SL_DEBUG macro defined.

@Param[in]	slob : handle of the memory manager associated with the slob
			(usually it would be the memory manager handle of a
			memory partition)

@Return		None

*//***************************************************************************/
void slob_dump(uint64_t *slob);

#endif /* SL_DEBUG */

#endif /* __SLOB_H */

