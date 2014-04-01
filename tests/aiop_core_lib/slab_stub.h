/**************************************************************************//*
 @File          slab_stub.h

 @Description   To Be Removed!!!

 @Cautions      This file is private for AIOP.
*//***************************************************************************/
#ifndef __SLAB_STUB_H
#define __SLAB_STUB_H

#include "common/types.h"
#include "common/errors.h"
#include "dplib/fsl_ldpaa.h"

#define MEM_PART_1ST_DDR_NON_CACHEABLE 1

/**************************************************************************//**
 @Function      slab_find_and_fill_bpid

 @Description   Finds and fills buffer pool with new buffers

                This function is part of SLAB module therefore it should be called only after
                it has been initialized by slab_module_init()

 @Param[in]     num_buffs           Number of buffers in new pool.
 @Param[in]     buff_size           Size of buffers in pool.
 @Param[in]     alignment           Requested alignment for data field (in bytes).
                                    AIOP: HW pool supports up to 8 bytes alignment.
 @Param[in]     mem_partition_id    Memory partition ID for allocation.
                                    AIOP: HW pool supports only PEB and DPAA DDR.
 @Param[out]    num_filled_buffs    Number of buffers that we succeeded to fill.
 @Param[out]    bpid                Id if the buffer that was filled with new buffers.

 @Return        0       - on success,
               -ENAVAIL - could not release into bpid
               -ENOMEM  - not enough memory for mem_partition_id
 *//***************************************************************************/

int slab_find_and_fill_bpid(uint32_t num_buffs,
							uint16_t buff_size,
                            uint16_t alignment,
                            uint8_t  mem_partition_id,
                            int      *num_filled_buffs,
                            uint16_t *bpid);



#endif /* __SLAB_STUB_H */
