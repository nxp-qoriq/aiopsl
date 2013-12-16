/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_string.h

 @Description   Efficient functions for copying and setting blocks of memory.
*//***************************************************************************/

#ifndef __FSL_STRING_H
#define __FSL_STRING_H


#if defined(SYS_OS_LINUX) && defined(__KERNEL__)
#include <linux/kernel.h>
#include <linux/string.h>
extern char * strtok ( char * str, const char * delimiters );

#elif defined(SYS_OS_VXWORKS)
#include "string.h"

#else
#include <string.h>

#endif /* defined(SYS_OS_LINUX) && defined(__KERNEL__) */

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_lib_g   Utility Library Application Programming Interface

 @Description   External routines.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         fsl_lib_string_g Memory Copy

 @Description   Memory Copy module functions,definitions and enums.

 @{
*//***************************************************************************/

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
void * memcpy32(void* p_dst,void* p_src, uint32_t size);

/**************************************************************************//**
 @Function      memcpy64

 @Description   Copies one memory buffer into another one in 8-byte chunks!
                Which should be more efficient than byte by byte.

                For large buffers (over 60 bytes) this function is about 8 times
                more efficient than the trivial memory copy. For short buffers
                it is reduced to the trivial copy and may be a bit worse.

                Some testing suggests that MemCpy32() preforms better than
                MemCpy64() over small buffers. On average they break even at
                100 byte buffers. For buffers larger than that MemCpy64 is
                superior.

 @Param[in]     pDst    - The address of the destination buffer.
 @Param[in]     pSrc    - The address of the source buffer.
 @Param[in]     size    - The number of bytes that will be copied from pSrc to pDst.

 @Return        pDst (the address of the destination buffer).

 @Cautions      There is no parameter or boundary checking! It is up to the user
                to supply non null parameters as source & destination and size
                that actually fits into their buffer.

                Do not use under Linux.
*//***************************************************************************/
void * memcpy64(void* p_dst,void* p_src, uint32_t size);

/**************************************************************************//**
 @Function      memset32

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
void * memset32(void* p_dst, uint8_t val, uint32_t size);

/**************************************************************************//**
 @Function      memset64

 @Description   Sets all bytes of a memory buffer to a specific value, in
                8-byte chunks.

 @Param[in]     pDst    - The address of the destination buffer.
 @Param[in]     val     - Value to set destination bytes to.
 @Param[in]     size    - The number of bytes that will be set to val.

 @Return        pDst (the address of the destination buffer).

 @Cautions      There is no parameter or boundary checking! It is up to the user
                to supply non null parameter as destination and size
                that actually fits into the destination buffer.
*//***************************************************************************/
void * memset64(void* p_dst, uint8_t val, uint32_t size);

/**************************************************************************//**
 @Function      mem_disp

 @Description   Displays a block of memory in chunks of 32 bits.

 @Param[in]     addr    - The address of the memory to display.
 @Param[in]     size    - The number of bytes that will be displayed.

 @Return        None.

 @Cautions      There is no parameter or boundary checking! It is up to the user
                to supply non null parameter as destination and size
                that actually fits into the destination buffer.
*//***************************************************************************/
void mem_disp(uint8_t *addr, int size);

/** @} */ /* end of fsl_lib_string_g group */
/** @} */ /* end of fsl_lib_g group */


#endif /* __FSL_STRING_H */
