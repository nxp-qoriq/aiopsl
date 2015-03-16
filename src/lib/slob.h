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


#define __ERR_MODULE__  MODULE_SLOB

#define MAKE_ALIGNED(addr, align)    \
    (((uint64_t)(addr) + ((align) - 1)) & (~(((uint64_t)align) - 1)))


/* t_MemBlock data structure defines parameters of the Memory Block */
typedef struct t_mem_block {
    struct t_mem_block *p_next;      /* Pointer to the next memory block */

    uint64_t  base;                 /* Base address of the memory block */
    uint64_t  end;                  /* End address of the memory block */
} t_mem_block;


/* t_FreeBlock data structure defines parameters of the Free Block */
typedef struct t_free_block
{
    struct t_free_block *p_next;     /* Pointer to the next free block */

    uint64_t  base;                 /* Base address of the block */
    uint64_t  end;                  /* End address of the block */
} t_free_block;


/* t_BusyBlock data structure defines parameters of the Busy Block  */
typedef struct t_busy_block
{
    struct t_busy_block *p_next;         /* Pointer to the next free block */

    uint64_t    base;                   /* Base address of the block */
    uint64_t    end;                    /* End address of the block */
    char        name[MM_MAX_NAME_LEN];  /* That block of memory was allocated for
                                           something specified by the Name */
} t_busy_block;


/* t_MM data structure defines parameters of the MM object */
typedef struct t_MM
{
#ifdef AIOP
    uint8_t          *lock;
#else
    fsl_handle_t        lock;
#endif

    t_mem_block      *mem_blocks;     /* List of memory blocks (Memory list) */
    t_busy_block     *busy_blocks;    /* List of busy blocks (Busy list) */
    t_free_block     *free_blocks[MM_MAX_ALIGNMENT + 1];
                                    /* Alignment lists of free blocks (Free lists) */

    uint64_t        free_mem_size;    /* Total size of free memory (in bytes) */
    /* A flag that shows whether there was allocation for free_blocks */
    uint32_t        free_blocks_initialized;
    uint64_t        base;
    uint64_t        size;
    fsl_handle_t    h_mem_mng; // boot memory manager
} t_MM;


#endif /* __SLOB_H */

