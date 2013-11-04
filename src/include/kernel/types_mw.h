 /**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          types_mw.h

 @Description   TODO
*//***************************************************************************/
#ifndef __TYPES_MW_H
#define __TYPES_MW_H


#include <stdint.h>
#include <stddef.h>

//#define __inline__      inline
#define _prepacked
#define _packed

/* temporary, due to include issues */
typedef uint32_t uintptr_t;
typedef int32_t intptr_t;

typedef uint64_t            dma_addr_t;


#ifndef NULL
#define NULL ((0L))
#endif /* NULL */



#endif /* __TYPES_MW_H */
