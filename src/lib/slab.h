/**************************************************************************//*
 @File          slab.h

 @Description   This is slab internal header file which includes all the 
                architecture specific implementation defines.

 @Cautions      This file is private for AIOP.
*//***************************************************************************/
#ifndef __SLAB_H
#define __SLAB_H

#include "common/fsl_slab.h"
#include "common/list.h"

#define __ERR_MODULE__  MODULE_SLAB

/**************************************************************************//**
 @Description   SLAB common internal macros 
*//***************************************************************************/
#define SLAB_HW_POOL_SET      0x00000001/**< Flag which indicates that this SLAB handle is HW pool */
#define SLAB_IS_HW_POOL(SLAB) ((SLAB) & SLAB_HW_POOL_SET)

/**************************************************************************//**
 @Description   SLAB AIOP HW pool internal macros 
*//***************************************************************************/
/*
 *  HW SLAB structure 
 *  
 * 31----------23--------------1--------0
 * | HW accel   |VP ID         |HW flg  |
 * ------------------------------------- 
 */
#define SLAB_VP_POOL_MASK      0x00FFFFFE
#define SLAB_VP_POOL_SHIFT     1
#define SLAB_HW_ACCEL_MASK     0xFF000000
#define SLAB_VP_POOL_GET(SLAB) (((SLAB) & SLAB_VP_POOL_MASK) >> 1) 
/**< Returns VP id to be used with virtual pools API */ 

#endif /* __SLAB_H */
