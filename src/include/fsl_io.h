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

/**************************************************************************//**
 @File          fsl_io.h

 @Description   I/O accessors
 *//***************************************************************************/
#ifndef __FSL_IO_H
#define __FSL_IO_H

#include "common/types.h"
#include "fsl_endian.h"

/**************************************************************************//**
@Group		accessor_g Accessor API

 @Description   Endian Conversion functions to read/write with endianess treatment

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      ioread8

 @Description   Function to read from supplied address pointer.

 @Param[in]     addr - address pointer to read from.

 @Return        value from the supplied address pointer.
 *//***************************************************************************/
static inline uint8_t ioread8(const volatile uint8_t *addr)
{
	uint8_t ret = *addr;
	core_memory_barrier();
	return ret;
}

/**************************************************************************//**
 @Function      ioread16

 @Description   Function to read from supplied address pointer with endianess treatment
		using little endianess.

 @Param[in]     addr - address pointer to read from.

 @Return        value from the supplied address pointer.
 *//***************************************************************************/
static inline uint16_t ioread16(const volatile uint16_t *addr)
{
	uint16_t ret = LOAD_LE16_TO_CPU(addr);
	core_memory_barrier();
	return ret;
}

/**************************************************************************//**
 @Function      ioread16be

 @Description   Function to read from supplied address pointer with endianess treatment
		using big endianess.
 @Param[in]     addr - address pointer to read from.

 @Return        value from the supplied address pointer.
 *//***************************************************************************/
static inline uint16_t ioread16be(const volatile uint16_t *addr)
{
	uint16_t ret = LOAD_BE16_TO_CPU(addr);
	core_memory_barrier();
	return ret;
}

/**************************************************************************//**
 @Function      ioread32

 @Description   Function to read from supplied address pointer with endianess treatment
		using little endianess.

 @Param[in]     addr - address pointer to read from.

 @Return        value from the supplied address pointer.
 *//***************************************************************************/
static inline uint32_t ioread32(const volatile uint32_t *addr)
{

	uint32_t ret = LOAD_LE32_TO_CPU(addr);
	core_memory_barrier();
	return ret;
}

/**************************************************************************//**
 @Function      ioread32be

 @Description   Function to read from supplied address pointer with endianess treatment
		using big endianess.
 @Param[in]     addr - address pointer to read from.

 @Return        value from the supplied address pointer.
 *//***************************************************************************/
static inline uint32_t ioread32be(const volatile uint32_t *addr)
{
	uint32_t ret = LOAD_BE32_TO_CPU(addr);
	core_memory_barrier();
	return ret;
}

/**************************************************************************//**
 @Function      ioread64

 @Description   Function to read from supplied address pointer with endianess treatment
		using little endianess.

 @Param[in]     addr - address pointer to read from.

 @Return        value from the supplied address pointer.
 *//***************************************************************************/
static inline uint64_t ioread64(const volatile uint64_t *addr)
{
	uint64_t ret = LOAD_LE64_TO_CPU(addr);
	core_memory_barrier();
	return ret;
}

/**************************************************************************//**
 @Function      ioread64be

 @Description   Function to read from supplied address pointer with endianess treatment
		using big endianess.
 @Param[in]     addr - address pointer to read from.

 @Return        value from the supplied address pointer.
 *//***************************************************************************/
static inline uint64_t ioread64be(const volatile uint64_t *addr)
{
	uint64_t ret = LOAD_BE64_TO_CPU(addr);
	core_memory_barrier();
	return ret;
}

/**************************************************************************//**
 @Function      iowrite8

 @Description   Function to write the supplied value to address pointer.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite8(uint8_t val, volatile uint8_t *addr)
{
	*addr = val;
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite8_wt

 @Description   Function to write the supplied value to address pointer.
                Store with Writethrough.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite8_wt(uint8_t val, volatile uint8_t *addr)
{
	register uint8_t rval = val;
	register volatile uint8_t *raddr = addr;
	asm {stbwtx rval, 0, raddr}	
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite16

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using little endian.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite16(uint16_t val, volatile uint16_t *addr)
{
	STORE_CPU_TO_LE16(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite16_wt

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	            using little endian.
 	            Store with Writethrough.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite16_wt(uint16_t val, volatile uint16_t *addr)
{
	STORE_CPU_TO_LE16_WT(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite16be

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using big endian.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite16be(uint16_t val, volatile uint16_t *addr)
{
	STORE_CPU_TO_BE16(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite16be_wt

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using big endian.
 	 	 Store with Writethrough.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite16be_wt(uint16_t val, volatile uint16_t *addr)
{
	STORE_CPU_TO_BE16_WT(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite32

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using little endian.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite32(uint32_t val, volatile uint32_t *addr)
{
	STORE_CPU_TO_LE32(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite32_wt

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using little endian.
 	 	 Store with Writethrough.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite32_wt(uint32_t val, volatile uint32_t *addr)
{
	STORE_CPU_TO_LE32_WT(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite32be

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using big endian.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite32be(uint32_t val, volatile uint32_t *addr)
{
	STORE_CPU_TO_BE32(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite32be_wt

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using big endian.
 	 	 Store with Writethrough.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite32be_wt(uint32_t val, volatile uint32_t *addr)
{
	STORE_CPU_TO_BE32_WT(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite64

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using little endian.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite64(uint64_t val, volatile uint64_t *addr)
{
	STORE_CPU_TO_LE64(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite64_wt

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using little endian.
 	 	 Store with Writethrough.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite64_wt(uint64_t val, volatile uint64_t *addr)
{
	STORE_CPU_TO_LE64_WT(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite64be

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using big endian.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite64be(uint64_t val, volatile uint64_t *addr)
{
	STORE_CPU_TO_BE64(val, addr);
	core_memory_barrier();
}

/**************************************************************************//**
 @Function      iowrite64be_wt

 @Description   Function to write the supplied value to address pointer with endianess treatment
 	 	 using big endian.
 	 	 Store with Writethrough.

 @Param[in]     val  - value to write.
 @Param[in]     addr - address pointer write to.

 *//***************************************************************************/
static inline void iowrite64be_wt(uint64_t val, volatile uint64_t *addr)
{
	STORE_CPU_TO_BE64_WT(val, addr);
	core_memory_barrier();
}


/** @} */ /* end of accessor_g Accessor API group */

#endif /* __FSL_IO_H */
