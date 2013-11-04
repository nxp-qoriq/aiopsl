/**************************************************************************//**
 Copyright 2013 Freescale Semiconductor, Inc.

 @File          io.h

 @Description   TODO
 *//***************************************************************************/
#ifndef __FSL_IO_H
#define __FSL_IO_H

#include "common/types.h"

/************************/
/* I/O access macros */
/************************/

#define GET_UINT8(arg)              *(volatile uint8_t *)(&(arg))
#define GET_UINT16(arg)             *(volatile uint16_t*)(&(arg))
#define GET_UINT32(arg)             *(volatile uint32_t*)(&(arg))
#define GET_UINT64(arg)             *(volatile uint64_t*)(&(arg))

#define _WRITE_UINT8(arg, data)     *(volatile uint8_t *)(&(arg)) = (data)
#define _WRITE_UINT16(arg, data)    *(volatile uint16_t*)(&(arg)) = (data)
#define _WRITE_UINT32(arg, data)    *(volatile uint32_t*)(&(arg)) = (data)
#define _WRITE_UINT64(arg, data)    *(volatile uint64_t*)(&(arg)) = (data)

#ifdef DEBUG_IO_WRITE
#define WRITE_UINT8(arg, data)  \
    do { fsl_os_print("ADDR: 0x%08x, VAL: 0x%02x\r\n",    (uint32_t)&(arg), (data)); _WRITE_UINT8((arg), (data)); } while (0)
#define WRITE_UINT16(arg, data) \
    do { fsl_os_print("ADDR: 0x%08x, VAL: 0x%04x\r\n",    (uint32_t)&(arg), (data)); _WRITE_UINT16((arg), (data)); } while (0)
#define WRITE_UINT32(arg, data) \
    do { fsl_os_print("ADDR: 0x%08x, VAL: 0x%08x\r\n",    (uint32_t)&(arg), (data)); _WRITE_UINT32((arg), (data)); } while (0)
#define WRITE_UINT64(arg, data) \
    do { fsl_os_print("ADDR: 0x%08x, VAL: 0x%016llx\r\n", (uint32_t)&(arg), (data)); _WRITE_UINT64((arg), (data)); } while (0)

#else  /* not DEBUG_IO_WRITE */
#define WRITE_UINT8     _WRITE_UINT8
#define WRITE_UINT16    _WRITE_UINT16
#define WRITE_UINT32    _WRITE_UINT32
#define WRITE_UINT64    _WRITE_UINT64
#endif /* not DEBUG_IO_WRITE */

#define ioread8(_addr)              GET_UINT8(*_addr)
#define ioread16be(_addr)           GET_UINT16(*_addr)
#define ioread32be(_addr)           GET_UINT32(*_addr)

#define iowrite8(_val, _addr)       WRITE_UINT8(*_addr, _val)
#define iowrite16be(_val, _addr)    WRITE_UINT16(*_addr, _val)
#define iowrite32be(_val, _addr)    WRITE_UINT32(*_addr, _val)

/**************************************************************************//**
 @Function      memcpy32

 @Description   Copies one memory buffer into another one in 4-byte chunks!
 Which should be more efficient than byte by byte.

 For large buffers (over 60 bytes) this function is about 4 times
 more efficient than the trivial memory copy. For short buffers
 it is reduced to the trivial copy and may be a bit worse.

 @Param[in]     pDst    - The address of the destination buffer.
 @Param[in]     pSrc    - The address of the source buffer.
 @Param[in]     size    - The number of bytes that will be copied from pSrc to pDst.

 @Return        pDst (the address of the destination buffer).

 @Cautions      There is no parameter or boundary checking! It is up to the user
 to supply non-null parameters as source & destination and size
 that actually fits into the destination buffer.
 *//***************************************************************************/
void * io2iocpy32(void* p_dst, void* p_src, uint32_t size);
void * io2memcpy32(void* p_dst, void* p_src, uint32_t size);
void * mem2iocpy32(void* p_dst, void* p_src, uint32_t size);

/**************************************************************************//**
 @Function      iomemset32

 @Description   Sets all bytes of a memory buffer to a specific value, in
 4-byte chunks.

 @Param[in]     pDst    - The address of the destination buffer.
 @Param[in]     val     - Value to set destination bytes to.
 @Param[in]     size    - The number of bytes that will be set to val.

 @Return        pDst (the address of the destination buffer).

 @Cautions      There is no parameter or boundary checking! It is up to the user
 to supply non null parameter as destination and size
 that actually fits into the destination buffer.
 *//***************************************************************************/
void * iomemset32(void* p_dst, uint8_t val, uint32_t size);

/**************************************************************************//**
 @Function      fsl_os_phys_to_virt

 @Description   Translates a physical address to the matching virtual address.

 @Param[in]     addr - The physical address to translate.

 @Return        Virtual address.
 *//***************************************************************************/
void * fsl_os_phys_to_virt(dma_addr_t addr);

/**************************************************************************//**
 @Function      fsl_os_virt_to_phys

 @Description   Translates a virtual address to the matching physical address.

 @Param[in]     addr - The virtual address to translate.

 @Return        Physical address.
 *//***************************************************************************/
dma_addr_t fsl_os_virt_to_phys(void *addr);

#endif /* __FSL_IO_H */
