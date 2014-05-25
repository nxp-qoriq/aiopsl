#if 0
#ifndef __SW_SLAB_H
#define __SW_SLAB_H

#include "common/slab.h"
#include "common/list.h"


#define __ERR_MODULE__  MODULE_SLAB


/**************************************************************************//**
 @Description   Memory allocation owner.
*//***************************************************************************/
typedef enum e_slab_alloc_owner {
    SLAB_ALLOC_OWNER_LOCAL,
    SLAB_ALLOC_OWNER_LOCAL_SMART,
    SLAB_ALLOC_OWNER_EXTERNAL
} e_slab_alloc_owner;


/**************************************************************************//*
 @Description   Memory Segment structure
*//***************************************************************************/
typedef struct slab {
    char        name[MEM_MAX_NAME_LENGTH];
                                    /* The segment's name */
    uint8_t     **p_bases;          /* Base addresses of the segments */
    uint8_t     **p_blocks_stack;    /* Array of pointers to blocks */
    fsl_handle_t    lock;
    uint16_t    buff_size;           /* Size of each data block */
    uint16_t    prefix_size;         /* How many bytes to reserve before the data */
    uint16_t    postfix_size;        /* How many bytes to reserve after the data */
    uint16_t    alignment;          /* Requested alignment for the data field */
    e_slab_alloc_owner alloc_owner;         /* Memory allocation owner */
    uint32_t    get_failures;        /* Number of times get failed */
    uint32_t    num_buffs;                /* Number of blocks in segment */
    uint32_t    current;            /* Current block */
    int         consecutive;     /* Allocate consecutive data blocks memory */
#ifdef DEBUG_MEM_LEAKS
    void        *p_slab_dbg;          /* MEM debug database (MEM leaks detection) */
    uint32_t    block_offset;
    uint32_t    block_size;
#endif /* DEBUG_MEM_LEAKS */
} slab_t;

#ifdef DEBUG_MEM_LEAKS
/**************************************************************************//**
 @Description   MEM block information for leaks detection.
*//***************************************************************************/
typedef struct t_slab_dbg {
    uintptr_t   owner_address;
} t_slab_dbg;
#endif /* DEBUG_MEM_LEAKS */

int sw_slab_create(char name[],
                struct slab **slab,
                uint32_t num_buffs,
                uint16_t buff_size,
                uint16_t prefix_size,
                uint16_t postfix_size,
                uint16_t alignment,
                uint8_t mem_partition_id,
                int consecutive);
int sw_slab_create_by_address(char name[],
                           struct slab **slab,
                           uint32_t num_buffs,
                           uint16_t buff_size,
                           uint16_t prefix_size,
                           uint16_t postfix_size,
                           uint16_t alignment,
                           uint8_t *address);
void sw_slab_free(struct slab *slab);
int sw_slab_acquire(struct slab *slab, uint64_t *buff);
int sw_slab_release(struct slab *slab, uint64_t buff);
uint32_t sw_slab_get_buff_size(struct slab *slab);
uint32_t sw_slab_get_num_buffs(struct slab *slab);


#endif /* __SW_SLAB_H */
#endif

