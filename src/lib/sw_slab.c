#include "common/types.h"
#include "common/errors.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"
#include "kernel/fsl_spinlock.h"

#include "sw_slab.h"


#define PAD_ALIGNMENT(align, x) (((x)%(align)) ? ((align)-((x)%(align))) : 0)

#define ALIGN_BLOCK(p_block, prefix_size, alignment)                 \
    do {                                                             \
        p_block += (prefix_size);                                    \
        p_block += PAD_ALIGNMENT((alignment), (uintptr_t)(p_block)); \
    } while (0)

#if defined(__GNUC__)
#define GET_CALLER_ADDR \
    __asm__ ("mflr  %0" : "=r" (caller_addr))
#elif defined(__MWERKS__)
/* NOTE: This implementation is only valid for CodeWarrior for PowerPC */
#define GET_CALLER_ADDR \
    __asm__("add  %0, 0, %0" : : "r" (caller_addr))
#endif /* defined(__GNUC__) */


/*****************************************************************************/
static uint32_t compute_partition_size(uint32_t num_buffs,
                                       uint16_t buff_size,
                                       uint16_t prefix_size,
                                       uint16_t postfix_size,
                                       uint16_t alignment)
{
    uint32_t  block_size = 0, pad1 = 0, pad2 = 0;

    /* Make sure that the alignment is at least 4 */
    if (alignment < 4)
        alignment = 4;

    pad1 = (uint32_t)PAD_ALIGNMENT(4, prefix_size);
    /* Block size not including 2nd padding */
    block_size = pad1 + prefix_size + buff_size + postfix_size;
    pad2 = PAD_ALIGNMENT(alignment, block_size);
    /* Block size including 2nd padding */
    block_size += pad2;

    return ((num_buffs * block_size) + alignment);
}

/*****************************************************************************/
static int create_sw_slab(char  name[],
                                struct slab   **slab,
                                uint32_t      num_buffs,
                                uint16_t      buff_size,
                                uint16_t      prefix_size,
                                uint16_t      postfix_size,
                                uint16_t      alignment,
                                uint8_t       mem_partition_id)
{
    uint8_t     *address;
    uint32_t    alloc_size;
    int   err_code;

    alloc_size = compute_partition_size(num_buffs,
                                             buff_size,
                                             prefix_size,
                                             postfix_size,
                                             alignment);

    address = (uint8_t *)fsl_os_xmalloc(alloc_size, mem_partition_id, 1);
    if (!address)
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment"));

    err_code = slab_create_by_address(name,
                                slab,
                                num_buffs,
                                buff_size,
                                prefix_size,
                                postfix_size,
                                alignment,
                                address);
    if (err_code != E_OK)
        RETURN_ERROR(MAJOR, err_code, NO_MSG);

    ((slab_t *)(*slab))->alloc_owner = SLAB_ALLOC_OWNER_LOCAL;

    return E_OK;
}

/*****************************************************************************/
static __inline__ void * get_buff(slab_t *pslab)
{
    uint8_t *p_block;

    /* check if there is an available block */
    if (pslab->current == pslab->num_buffs)
    {
        pslab->get_failures++;
        return NULL;
    }

    /* get the block */
    p_block = pslab->p_blocks_stack[pslab->current];
#ifdef DEBUG
    pslab->p_blocks_stack[pslab->current] = NULL;
#endif /* DEBUG */
    /* advance current index */
    pslab->current++;

    return (void *)p_block;
}

/*****************************************************************************/
static __inline__ int put_buff(slab_t *pslab, void *p_block)
{
    /* check if blocks stack is full */
    if (pslab->current > 0)
    {
        /* decrease current index */
        pslab->current--;
        /* put the block */
        pslab->p_blocks_stack[pslab->current] = (uint8_t *)p_block;
        return 0;
    }

    return -ENOSPC;
}

#ifdef DEBUG_MEM_LEAKS
/*****************************************************************************/
static int init_slab_debug_database(slab_t *pslab)
{
    pslab->p_slab_dbg = (void *)fsl_os_malloc(sizeof(t_slab_dbg) * pslab->num_buffs);
    if (!pslab->p_slab_dbg)
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory debug object"));

    memset(pslab->p_slab_dbg, ILLEGAL_BASE, sizeof(t_slab_dbg) * pslab->num_buffs);

    return E_OK;
}

/*****************************************************************************/
static int debug_slab_get_local(struct slab *slab, void *p_block, uintptr_t owner_address)
{
    slab_t *pslab = (slab_t *)slab;
    t_slab_dbg        *p_slab_dbg = (t_slab_dbg *)pslab->p_slab_dbg;
    uint32_t        block_index;

    ASSERT_COND(owner_address != ILLEGAL_BASE);

    /* Find block num_buffs */
    if (pslab->consecutive)
        block_index =
            (((uint8_t *)p_block - (pslab->p_bases[0] + pslab->block_offset)) / pslab->block_size);
    else
        block_index = *(uint32_t *)((uint8_t *)p_block - 4);

    ASSERT_COND(block_index < pslab->num_buffs);
    ASSERT_COND(p_slab_dbg[block_index].owner_address == ILLEGAL_BASE);

    p_slab_dbg[block_index].owner_address = owner_address;

    return E_OK;
}

/*****************************************************************************/
static int debug_slab_put_local(struct slab *slab, void *p_block)
{
    slab_t *pslab = (slab_t *)slab;
    t_slab_dbg        *p_slab_dbg = (t_slab_dbg *)pslab->p_slab_dbg;
    uint32_t        block_index;
    uint8_t         *p_temp;

    /* Find block num_buffs */
    if (pslab->consecutive)
    {
        block_index =
            (((uint8_t *)p_block - (pslab->p_bases[0] + pslab->block_offset)) / pslab->block_size);

        if (block_index >= pslab->num_buffs)
            RETURN_ERROR(MAJOR, E_INVALID_ADDRESS,
                         ("freed address (0x%08x) does not belong to this slab", p_block));
    }
    else
    {
        block_index = *(uint32_t *)((uint8_t *)p_block - 4);

        if (block_index >= pslab->num_buffs)
            RETURN_ERROR(MAJOR, E_INVALID_ADDRESS,
                         ("freed address (0x%08x) does not belong to this slab", p_block));

        /* Verify that the block matches the corresponding base */
        p_temp = pslab->p_bases[block_index];

        ALIGN_BLOCK(p_temp, pslab->prefix_size, pslab->alignment);

        if (p_temp == pslab->p_bases[block_index])
            p_temp += pslab->alignment;

        if (p_temp != p_block)
            RETURN_ERROR(MAJOR, E_INVALID_ADDRESS,
                         ("freed address (0x%08x) does not belong to this slab", p_block));
    }

    if (p_slab_dbg[block_index].owner_address == ILLEGAL_BASE)
        RETURN_ERROR(MAJOR, E_ALREADY_FREE,
                     ("attempt to free unallocated address (0x%08x)", p_block));

    p_slab_dbg[block_index].owner_address = (uintptr_t)ILLEGAL_BASE;

    return E_OK;
}
#endif /* DEBUG_MEM_LEAKS */

/*****************************************************************************/
int sw_slab_create_by_address(char           name[],
                                 struct slab    **slab,
                                 uint32_t       num_buffs,
                                 uint16_t       buff_size,
                                 uint16_t       prefix_size,
                                 uint16_t       postfix_size,
                                 uint16_t       alignment,
                                 uint8_t        *address)
{
    slab_t *pslab;
    uint32_t        i, block_size;
    uint16_t        align_pad, end_pad;
    uint8_t         *p_blocks;

     /* prepare in case of error */
    *slab = NULL;

    if (!address)
        return -EFAULT;

    p_blocks = address;

    /* make sure that the alignment is at least 4 and power of 2 */
    if (alignment < 4)
        alignment = 4;
    else if (!is_power_of_2(alignment))
        RETURN_ERROR(MAJOR, E_INVALID_VALUE, ("alignment (should be power of 2)"));

    /* first allocate the segment descriptor */
    pslab = (slab_t *)fsl_os_malloc(sizeof(slab_t));
    if (!pslab)
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment structure"));

    /* allocate the blocks stack */
    pslab->p_blocks_stack = (uint8_t **)fsl_os_malloc(num_buffs * sizeof(uint8_t*));
    if (!pslab->p_blocks_stack)
    {
        fsl_os_free(pslab);
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment block pointers stack"));
    }

    /* allocate the blocks bases array */
    pslab->p_bases = (uint8_t **)fsl_os_malloc(sizeof(uint8_t*));
    if (!pslab->p_bases)
    {
        slab_free(pslab);
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment base pointers array"));
    }
    memset(pslab->p_bases, 0, sizeof(uint8_t*));

    /* store info about this segment */
    pslab->num_buffs = num_buffs;
    pslab->current = 0;
    pslab->buff_size = buff_size;
    pslab->p_bases[0] = p_blocks;
    pslab->get_failures = 0;
    pslab->alloc_owner = SLAB_ALLOC_OWNER_EXTERNAL;
    pslab->consecutive = 1;
    pslab->prefix_size = prefix_size;
    pslab->postfix_size = postfix_size;
    pslab->alignment = alignment;
    /* store name */
    strncpy(pslab->name, name, MEM_MAX_NAME_LENGTH-1);

    pslab->lock = spin_lock_create();
    if (!pslab->lock)
    {
        slab_free(pslab);
        return -ENOMEM;
    }

    align_pad = (uint16_t)PAD_ALIGNMENT(4, prefix_size);
    /* Make sure the entire size is a multiple of alignment */
    end_pad = (uint16_t)PAD_ALIGNMENT(alignment, (align_pad + prefix_size + buff_size + postfix_size));

    /* The following manipulation places the data of block[0] in an aligned address,
       since block size is aligned the following block datas will all be aligned */
    ALIGN_BLOCK(p_blocks, prefix_size, alignment);

    block_size = (uint32_t)(align_pad + prefix_size + buff_size + postfix_size + end_pad);

    /* initialize the blocks */
    for (i=0; i < num_buffs; i++)
    {
        pslab->p_blocks_stack[i] = p_blocks;
        p_blocks += block_size;
    }

    /* return handle to caller */
    *slab = (fsl_handle_t)pslab;

#ifdef DEBUG_MEM_LEAKS
    {
        int err_code = init_slab_debug_database(pslab);

        if (err_code != E_OK)
            RETURN_ERROR(MAJOR, err_code, NO_MSG);

        pslab->block_offset = (uint32_t)(pslab->p_blocks_stack[0] - pslab->p_bases[0]);
        pslab->block_size = block_size;
    }
#endif /* DEBUG_MEM_LEAKS */

    return E_OK;
}

/*****************************************************************************/
int sw_slab_create(char            name[],
                      struct slab     **slab,
                      uint32_t        num_buffs,
                      uint16_t        buff_size,
                      uint16_t        prefix_size,
                      uint16_t        postfix_size,
                      uint16_t        alignment,
                      uint8_t         mem_partition_id,
                      int             consecutive)
{
    slab_t *pslab;
    uint32_t        i, block_size;
    uint16_t        align_pad, end_pad;

    if (!consecutive)
        return create_sw_slab(name,
                              slab,
                              num_buffs,
                              buff_size,
                              prefix_size,
                              postfix_size,
                              alignment,
                              mem_partition_id);

    /* prepare in case of error */
    *slab = NULL;

    /* make sure that size is always a multiple of 4 */
    if (buff_size & 3)
    {
        buff_size &= ~3;
        buff_size += 4;
    }

    /* make sure that the alignment is at least 4 and power of 2 */
    if (alignment < 4)
        alignment = 4;
    else if (!is_power_of_2(alignment))
        RETURN_ERROR(MAJOR, E_INVALID_VALUE, ("alignment (should be power of 2)"));

    /* first allocate the segment descriptor */
    pslab = (slab_t *)fsl_os_malloc(sizeof(slab_t));
    if (!pslab)
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment structure"));

    /* allocate the blocks stack */
    pslab->p_blocks_stack = (uint8_t **)fsl_os_malloc(num_buffs * sizeof(uint8_t*));
    if (!pslab->p_blocks_stack)
    {
        slab_free(pslab);
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment block pointers stack"));
    }

    /* allocate the blocks bases array */
    pslab->p_bases = (uint8_t **)fsl_os_malloc(sizeof(uint8_t*));
    if (!pslab->p_bases)
    {
        slab_free(pslab);
        RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment base pointers array"));
    }
    memset(pslab->p_bases, 0, sizeof(uint8_t*));

    /* store info about this segment */
    pslab->num_buffs = num_buffs;
    pslab->current = 0;
    pslab->buff_size = buff_size;
    pslab->get_failures = 0;
    pslab->alloc_owner = SLAB_ALLOC_OWNER_LOCAL_SMART;
    pslab->consecutive = 1;
    pslab->prefix_size = prefix_size;
    pslab->postfix_size = postfix_size;
    pslab->alignment = alignment;

    pslab->lock = spin_lock_create();
    if (!pslab->lock)
    {
        slab_free(pslab);
        return -ENOMEM;
    }

    align_pad = (uint16_t)PAD_ALIGNMENT(4, prefix_size);
    /* Make sure the entire size is a multiple of alignment */
    end_pad = (uint16_t)PAD_ALIGNMENT(alignment, align_pad + prefix_size + buff_size + postfix_size);

    /* Calculate blockSize */
    block_size = (uint32_t)(align_pad + prefix_size + buff_size + postfix_size + end_pad);

    /* Now allocate the blocks */
    if (pslab->consecutive)
    {
        /* |alignment - 1| bytes at most will be discarded in the beginning of the
           received segment for alignment reasons, therefore the allocation is of:
           (alignment + (num_buffs * block size)). */
        uint8_t *p_blocks = (uint8_t *)
           fsl_os_xmalloc((uint32_t)((num_buffs * block_size) + alignment), mem_partition_id, 1);
        if (!p_blocks)
        {
            slab_free(pslab);
            RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment blocks"));
        }

        /* Store the memory segment address */
        pslab->p_bases[0] = p_blocks;

        /* The following manipulation places the data of block[0] in an aligned address,
           since block size is aligned the following block datas will all be aligned.*/
        ALIGN_BLOCK(p_blocks, prefix_size, alignment);

        /* initialize the blocks */
        for (i = 0; i < num_buffs; i++)
        {
            pslab->p_blocks_stack[i] = p_blocks;
            p_blocks += block_size;
        }

#ifdef DEBUG_MEM_LEAKS
        pslab->block_offset = (uint32_t)(pslab->p_blocks_stack[0] - pslab->p_bases[0]);
        pslab->block_size = block_size;
#endif /* DEBUG_MEM_LEAKS */
    }
    else
    {
        /* |alignment - 1| bytes at most will be discarded in the beginning of the
           received segment for alignment reasons, therefore the allocation is of:
           (alignment + block size). */
        for (i = 0; i < num_buffs; i++)
        {
            uint8_t *p_block = (uint8_t *)
               fsl_os_xmalloc((uint32_t)(block_size + alignment), mem_partition_id, 1);
            if (!p_block)
            {
                slab_free(pslab);
                RETURN_ERROR(MAJOR, E_NO_MEMORY, ("memory segment blocks"));
            }

            /* Store the memory segment address */
            pslab->p_bases[i] = p_block;

            /* The following places the data of each block in an aligned address */
            ALIGN_BLOCK(p_block, prefix_size, alignment);

#ifdef DEBUG_MEM_LEAKS
            /* Need 4 bytes before the meaningful bytes to store the block index.
               We know we have them because alignment is at least 4 bytes. */
            if (p_block == pslab->p_bases[i])
                p_block += alignment;

            *(uint32_t *)(p_block - 4) = i;
#endif /* DEBUG_MEM_LEAKS */

            pslab->p_blocks_stack[i] = p_block;
        }
    }

    /* store name */
    strncpy(pslab->name, name, MEM_MAX_NAME_LENGTH-1);

    /* return handle to caller */
    *slab = (fsl_handle_t)pslab;

#ifdef DEBUG_MEM_LEAKS
    {
        int err_code = init_slab_debug_database(pslab);

        if (err_code != E_OK)
            RETURN_ERROR(MAJOR, err_code, NO_MSG);
    }
#endif /* DEBUG_MEM_LEAKS */

    return E_OK;
}

/*****************************************************************************/
void sw_slab_free(struct slab *slab)
{
    slab_t *pslab = (slab_t*)slab;
    uint32_t        num_buffs, i;

    /* Check MEM leaks */
    slab_check_leaks(slab);

    if (pslab)
    {
        num_buffs = pslab->consecutive ? 1 : pslab->num_buffs;

        if (pslab->alloc_owner == SLAB_ALLOC_OWNER_LOCAL_SMART)
            for (i=0; i < num_buffs; i++)
                if (pslab->p_bases[i])
                    fsl_os_xfree(pslab->p_bases[i]);
        else if (pslab->alloc_owner == SLAB_ALLOC_OWNER_LOCAL)
            for (i=0; i < num_buffs; i++)
                if (pslab->p_bases[i])
                    fsl_os_free(pslab->p_bases[i]);

        if (pslab->lock)
            spin_lock_free(pslab->lock);

        if (pslab->p_bases)
            fsl_os_free(pslab->p_bases);

        if (pslab->p_blocks_stack)
            fsl_os_free(pslab->p_blocks_stack);

#ifdef DEBUG_MEM_LEAKS
        if (pslab->p_slab_dbg)
            fsl_os_free(pslab->p_slab_dbg);
#endif /* DEBUG_MEM_LEAKS */

       fsl_os_free(pslab);
    }
}

/*****************************************************************************/
int sw_slab_acquire(struct slab *slab, uint64_t *buff)
{
    slab_t *pslab = (slab_t *)slab;
    uint8_t         *p_block;
    uint32_t        int_flags;
#ifdef DEBUG_MEM_LEAKS
    uintptr_t       caller_addr = 0;

    GET_CALLER_ADDR;
#endif /* DEBUG_MEM_LEAKS */

    ASSERT_COND(pslab);

    int_flags = spin_lock_irqsave(pslab->lock);
    /* check if there is an available block */
    if ((p_block = (uint8_t *)get_buff(pslab)) == NULL)
    {
        spin_unlock_irqrestore(pslab->lock, int_flags);
        return E_NO_MEMORY;
    }

#ifdef DEBUG_MEM_LEAKS
    debug_slab_get_local(pslab, p_block, caller_addr);
#endif /* DEBUG_MEM_LEAKS */
    spin_unlock_irqrestore(pslab->lock, int_flags);

    *buff = (uint64_t)PTR_TO_UINT(p_block);
    return 0;
}

/*****************************************************************************/
int sw_slab_release(struct slab *slab, uint64_t buff)
{
    slab_t      *pslab = (slab_t *)slab;
    uint8_t     *p_block = UINT_TO_PTR(buff);
    int         rc;
    uint32_t    int_flags;

    ASSERT_COND(pslab);

    int_flags = spin_lock_irqsave(pslab->lock);
    /* check if blocks stack is full */
    if ((rc = put_buff(pslab, p_block)) != E_OK)
    {
        spin_unlock_irqrestore(pslab->lock, int_flags);
        RETURN_ERROR(MAJOR, rc, NO_MSG);
    }

#ifdef DEBUG_MEM_LEAKS
    debug_slab_put_local(pslab, p_block);
#endif /* DEBUG_MEM_LEAKS */
    spin_unlock_irqrestore(pslab->lock, int_flags);

    return E_OK;
}

uint32_t sw_slab_get_buff_size(struct slab *slab)
{
    slab_t *pslab = (slab_t *)slab;

    ASSERT_COND(pslab);

    return pslab->buff_size;
}

uint32_t sw_slab_get_num_buffs(struct slab *slab)
{
    slab_t *pslab = (slab_t *)slab;

    ASSERT_COND(pslab);

    return pslab->num_buffs;
}

#ifdef DEBUG_MEM_LEAKS
/*****************************************************************************/
void slab_check_leaks(struct slab *slab)
{
    slab_t *pslab = (slab_t *)slab;
    t_slab_dbg        *p_slab_dbg = (t_slab_dbg *)pslab->p_slab_dbg;
    uint8_t         *p_block;
    int             i;

    ASSERT_COND(pslab);

    if (pslab->consecutive)
    {
        for (i=0; i < pslab->num_buffs; i++)
        {
            if (p_slab_dbg[i].owner_address != ILLEGAL_BASE)
            {
                /* Find the block address */
                p_block = ((pslab->p_bases[0] + pslab->block_offset) +
                           (i * pslab->block_size));

                fsl_os_print("MEM leak: 0x%08x, caller address: 0x%08x\n",
                         p_block, p_slab_dbg[i].owner_address);
            }
        }
    }
    else
    {
        for (i=0; i < pslab->num_buffs; i++)
        {
            if (p_slab_dbg[i].owner_address != ILLEGAL_BASE)
            {
                /* Find the block address */
                p_block = pslab->p_bases[i];

                ALIGN_BLOCK(p_block, pslab->prefix_size, pslab->alignment);

                if (p_block == pslab->p_bases[i])
                    p_block += pslab->alignment;

                fsl_os_print("MEM leak: 0x%08x, caller address: 0x%08x\n",
                         p_block, p_slab_dbg[i].owner_address);
            }
        }
    }
}
#endif /* DEBUG_MEM_LEAKS */
