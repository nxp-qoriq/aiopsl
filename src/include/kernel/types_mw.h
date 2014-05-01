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


/** Task global variables area */
#define __TASK __declspec(section ".tdata")

/** Shared-SRAM code location */
#pragma section RX ".stext_vle"
#define __WARM_CODE __declspec(section ".stext_vle")

/** Shared-SRAM global variables */
#pragma section RW ".shdata" ".shbss"
#define __SHRAM __declspec(section ".shdata")

/** i-RAM code location */
#pragma section RX ".itext_vle"
#define __HOT_CODE __declspec(section ".itext_vle")


#endif /* __TYPES_MW_H */
