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

#include "common/types.h"
#include "fsl_errors.h"
#include "fsl_stdio.h"
#include "inc/fsl_gen.h"
#include "common/fsl_string.h"
#include "fsl_io.h"

void *memcpy32(void* p_dst, void* p_src, uint32_t size)
{
    uint32_t left_align;
    uint32_t right_align;
    uint32_t last_word;
    uint32_t curr_word;
    uint32_t *p_src32;
    uint32_t *p_dst32;
    uint8_t  *p_src8;
    uint8_t  *p_dst8;

    p_src8 = (uint8_t*)(p_src);
    p_dst8 = (uint8_t*)(p_dst);
    /* first copy byte by byte till the source first alignment
     * this step is necessary to ensure we do not even try to access
     * data which is before the source buffer, hence it is not ours.
     */
    while((PTR_TO_UINT(p_src8) & 3) && size) /* (pSrc mod 4) > 0 and size > 0 */
    {
        *p_dst8++ = *p_src8++;
        size--;
    }

    /* align destination (possibly disaligning source)*/
    while((PTR_TO_UINT(p_dst8) & 3) && size) /* (pDst mod 4) > 0 and size > 0 */
    {
        *p_dst8++ = *p_src8++;
        size--;
    }

    /* dest is aligned and source is not necessarily aligned */
    left_align = (uint32_t)((PTR_TO_UINT(p_src8) & 3) << 3); /* leftAlign = (pSrc mod 4)*8 */
    right_align = 32 - left_align;


    if (left_align == 0)
    {
        /* source is also aligned */
        p_src32 = (uint32_t*)(p_src8);
        p_dst32 = (uint32_t*)(p_dst8);
        while (size >> 2) /* size >= 4 */
        {
            *p_dst32++ = *p_src32++;
            size -= 4;
        }
        p_src8 = (uint8_t*)(p_src32);
        p_dst8 = (uint8_t*)(p_dst32);
    }
    else
    {
        /* source is not aligned (destination is aligned)*/
        p_src32 = (uint32_t*)(p_src8 - (left_align >> 3));
        p_dst32 = (uint32_t*)(p_dst8);
        last_word = *p_src32++;
        while(size >> 3) /* size >= 8 */
        {
            curr_word = *p_src32;
            *p_dst32 = (last_word << left_align) | (curr_word >> right_align);
            last_word = curr_word;
            p_src32++;
            p_dst32++;
            size -= 4;
        }
        p_dst8 = (uint8_t*)(p_dst32);
        p_src8 = (uint8_t*)(p_src32) - 4 + (left_align >> 3);
    }

    /* complete the left overs */
    while (size--)
        *p_dst8++ = *p_src8++;

    return p_dst;
}

void * memcpy64(void* p_dst,void* p_src, uint32_t size)
{
    uint32_t left_align;
    uint32_t right_align;
    uint64_t last_word;
    uint64_t curr_word;
    uint64_t *p_src64;
    uint64_t *p_dst64;
    uint8_t  *p_src8;
    uint8_t  *p_dst8;

    p_src8 = (uint8_t*)(p_src);
    p_dst8 = (uint8_t*)(p_dst);
    /* first copy byte by byte till the source first alignment
     * this step is necessarily to ensure we do not even try to access
     * data which is before the source buffer, hence it is not ours.
     */
    while((PTR_TO_UINT(p_src8) & 7) && size) /* (pSrc mod 8) > 0 and size > 0 */
    {
        *p_dst8++ = *p_src8++;
        size--;
    }

    /* align destination (possibly disaligning source)*/
    while((PTR_TO_UINT(p_dst8) & 7) && size) /* (pDst mod 8) > 0 and size > 0 */
    {
        *p_dst8++ = *p_src8++;
        size--;
    }

    /* dest is aligned and source is not necessarily aligned */
    left_align = (uint32_t)((PTR_TO_UINT(p_src8) & 7) << 3); /* leftAlign = (pSrc mod 8)*8 */
    right_align = 64 - left_align;


    if (left_align == 0)
    {
        /* source is also aligned */
        p_src64 = (uint64_t*)(p_src8);
        p_dst64 = (uint64_t*)(p_dst8);
        while (size >> 3) /* size >= 8 */
        {
            *p_dst64++ = *p_src64++;
            size -= 8;
        }
        p_src8 = (uint8_t*)(p_src64);
        p_dst8 = (uint8_t*)(p_dst64);
    }
    else
    {
        /* source is not aligned (destination is aligned)*/
        p_src64 = (uint64_t*)(p_src8 - (left_align >> 3));
        p_dst64 = (uint64_t*)(p_dst8);
        last_word = *p_src64++;
        while(size >> 4) /* size >= 16 */
        {
            curr_word = *p_src64;
            *p_dst64 = (last_word << left_align) | (curr_word >> right_align);
            last_word = curr_word;
            p_src64++;
            p_dst64++;
            size -= 8;
        }
        p_dst8 = (uint8_t*)(p_dst64);
        p_src8 = (uint8_t*)(p_src64) - 8 + (left_align >> 3);
    }

    /* complete the left overs */
    while (size--)
        *p_dst8++ = *p_src8++;

    return p_dst;
}

void * memset32(void* p_dst, uint8_t val, uint32_t size)
{
    uint32_t val32;
    uint32_t *p_dst32;
    uint8_t  *p_dst8;

    p_dst8 = (uint8_t*)(p_dst);

    /* generate four 8-bit val's in 32-bit container */
    val32  = (uint32_t) val;
    val32 |= (val32 <<  8);
    val32 |= (val32 << 16);

    /* align destination to 32 */
    while((PTR_TO_UINT(p_dst8) & 3) && size) /* (pDst mod 4) > 0 and size > 0 */
    {
        *p_dst8++ = val;
        size--;
    }

    /* 32-bit chunks */
    p_dst32 = (uint32_t*)(p_dst8);
    while (size >> 2) /* size >= 4 */
    {
        *p_dst32++ = val32;
        size -= 4;
    }

    /* complete the leftovers */
    p_dst8 = (uint8_t*)(p_dst32);
    while (size--)
        *p_dst8++ = val;

    return p_dst;
}

void * memset64(void* p_dst, uint8_t val, uint32_t size)
{
    uint64_t val64;
    uint64_t *p_dst64;
    uint8_t  *p_dst8;

    p_dst8 = (uint8_t*)(p_dst);

    /* generate four 8-bit val's in 32-bit container */
    val64  = (uint64_t) val;
    val64 |= (val64 <<  8);
    val64 |= (val64 << 16);
    val64 |= (val64 << 24);
    val64 |= (val64 << 32);

    /* align destination to 64 */
    while((PTR_TO_UINT(p_dst8) & 7) && size) /* (pDst mod 8) > 0 and size > 0 */
    {
        *p_dst8++ = val;
        size--;
    }

    /* 64-bit chunks */
    p_dst64 = (uint64_t*)(p_dst8);
    while (size >> 4) /* size >= 8 */
    {
        *p_dst64++ = val64;
        size -= 8;
    }

    /* complete the leftovers */
    p_dst8 = (uint8_t*)(p_dst64);
    while (size--)
        *p_dst8++ = val;

    return p_dst;
}

void mem_disp(uint8_t *p, int size)
{
    uint32_t    space = (uint32_t)(PTR_TO_UINT(p) & 0x3);
    uint8_t     *p_limit;

    if (space)
    {
        p_limit = (p - space + 4);

        fsl_os_print("0x%08X: ", (p - space));

        while (space--)
        {
            fsl_os_print("--");
        }
        while (size  && (p < p_limit))
        {
            fsl_os_print("%02x", *(uint8_t*)p);
            size--;
            p++;
        }

        fsl_os_print(" ");
        p_limit += 12;

        while ((size > 3) && (p < p_limit))
        {
            fsl_os_print("%08x ", *(uint32_t*)p);
            size -= 4;
            p += 4;
        }
        fsl_os_print("\r\n");
    }

    while (size > 15)
    {
        fsl_os_print("0x%08X: %08x %08x %08x %08x\r\n",
                 p, *(uint32_t *)p, *(uint32_t *)(p + 4),
                 *(uint32_t *)(p + 8), *(uint32_t *)(p + 12));
        size -= 16;
        p += 16;
    }

    if (size)
    {
        fsl_os_print("0x%08X: ", p);

        while (size > 3)
        {
            fsl_os_print("%08x ", *(uint32_t *)p);
            size -= 4;
            p += 4;
        }
        while (size)
        {
            fsl_os_print("%02x", *(uint8_t *)p);
            size--;
            p++;
        }

        fsl_os_print("\r\n");
    }
}
