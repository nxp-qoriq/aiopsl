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

#include "common/errors.h"
#include "common/slob.h"


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
} t_MM;


#endif /* __SLOB_H */

