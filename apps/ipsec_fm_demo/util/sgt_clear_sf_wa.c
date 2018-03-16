/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/******************************************************************************/
/* Helper function to clear the SGE[SF] (Start of Fragment) field
* of the default FD (frame descriptor).
*
* If the Scatter/gather table and buffers are in the core virtual space use
* #define CORE_VIRTUAL_ACCESS
******************************************************************************/
static __HOT_CODE void sgt_clear_sf_wa(void)
{
	uint64_t	addr, dword1, dword2;
	uint16_t	off;
	uint8_t		fmt, sl, sf, final;

	#define SGE_GET_U64(_addr, _off)				    \
		(uint64_t)({register uint64_t __rR = 0;			    \
		uint64_t val64;						    \
		val64 = (LLLDW_SWAP(0, (uint64_t)(((char *)_addr) + _off)));\
		__rR = (uint64_t)val64; })

	#define SGE_SET_U64(_addr, _val)				    \
		({ LLSTDW_SWAP(_val, 0, ((char *)_addr)); })

	/* Scatter/Gather FD */
	#define FD_FMT_SG			2
	/* SGE defines */
	#define SGE_ADDR_SIZE			49
	#define SGE_FMT_SGTE			2
	#define SGE_SF_FLD_SHIFT		20
	#define SGE_IVP_FLD_SHIFT		(SGE_SF_FLD_SHIFT + 26)
	#define SGE_OFF_FLD_SHIFT		(SGE_IVP_FLD_SHIFT + 2)
	#define SGE_FMT_FLD_SHIFT		(SGE_OFF_FLD_SHIFT + 12)
	#define SGE_SL_FLD_SHIFT		(SGE_FMT_FLD_SHIFT + 2)
	#define SGE_F_FLD_SHIFT			(SGE_SL_FLD_SHIFT + 1)

#ifndef CORE_VIRTUAL_ACCESS
	uint8_t		sge[2 * sizeof(uint64_t)];

	/* Check for FD Scatter/Gather format */
	if (LDPAA_FD_GET_FMT(HWC_FD_ADDRESS) != FD_FMT_SG)
		return;
	addr = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
	off = LDPAA_FD_GET_OFFSET(HWC_FD_ADDRESS);
	do {
		cdma_read_with_no_cache(&sge, addr + off, 2 * sizeof(uint64_t));
		dword2 = SGE_GET_U64(sge, sizeof(uint64_t));
		/* Set IVP field if buffer address in SGE is 0. Do that to
		 * avoid an invalid buffer release in a BP */
		dword1 = SGE_GET_U64(sge, 0);
		if (!(dword1 & ((1 << SGE_ADDR_SIZE) - 1))) {
			dword2 |= 1ULL << SGE_IVP_FLD_SHIFT;
			SGE_SET_U64((sge + sizeof(uint64_t)), dword2);
			cdma_write(addr + off, &sge, 2 * sizeof(uint64_t));
		}
		sl = (uint8_t)(dword2 >> SGE_SL_FLD_SHIFT) & 0x01;
		final = (uint8_t)(dword2 >> SGE_F_FLD_SHIFT) & 0x01;
		fmt = (uint8_t)(dword2 >> SGE_FMT_FLD_SHIFT) & 0x03;
		if (sl) {
			sf = (uint8_t)(dword2 >> SGE_SF_FLD_SHIFT) & 0x01;
			if (sf) {
				dword2 &= ~(1 << SGE_SF_FLD_SHIFT);
				SGE_SET_U64((sge + sizeof(uint64_t)), dword2);
				cdma_write(addr + off, &sge,
					   2 * sizeof(uint64_t));
			}
		}
		if (fmt == SGE_FMT_SGTE) {
			/* SGT Extension */
			addr = dword1 & ((1 << SGE_ADDR_SIZE) - 1);
			ASSERT_COND(addr);
			off = (uint16_t)(dword2 >> SGE_OFF_FLD_SHIFT) & 0x0fff;
		} else {
			off += 2 * sizeof(uint64_t);
		}
	} while (!final);
#else
	uint32_t	ext_addr;

	/* Check for FD Scatter/Gather format */
	if (LDPAA_FD_GET_FMT(HWC_FD_ADDRESS) != FD_FMT_SG)
		return;
	addr = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
	off = LDPAA_FD_GET_OFFSET(HWC_FD_ADDRESS);
	/* Virtual address of the first SGE in SGT */
	ext_addr = (uint32_t)sys_phys_to_virt(addr);
	do {
		dword2 = SGE_GET_U64((ext_addr + off), sizeof(uint64_t));
		/* Set IVP field if buffer address in SGE is 0. Do that to
		 * avoid an invalid buffer release in a BP */
		dword1 = SGE_GET_U64((ext_addr + off), 0);
		if (!(dword1 & ((1 << SGE_ADDR_SIZE) - 1))) {
			dword2 |= 1ULL << SGE_IVP_FLD_SHIFT;
			SGE_SET_U64((ext_addr + off + sizeof(uint64_t)),
				    dword2);
		}
		sl = (uint8_t)(dword2 >> SGE_SL_FLD_SHIFT) & 0x01;
		final = (uint8_t)(dword2 >> SGE_F_FLD_SHIFT) & 0x01;
		fmt = (uint8_t)(dword2 >> SGE_FMT_FLD_SHIFT) & 0x03;
		if (sl) {
			sf = (uint8_t)(dword2 >> SGE_SF_FLD_SHIFT) & 0x01;
			if (sf) {
				dword2 &= ~(1 << SGE_SF_FLD_SHIFT);
				SGE_SET_U64((ext_addr + off + sizeof(uint64_t)),
					    dword2);
			}
		}
		if (fmt == SGE_FMT_SGTE) {
			/* SGT Extension */
			addr = dword1 & ((1 << SGE_ADDR_SIZE) - 1);
			ASSERT_COND(addr);
			off = (uint16_t)(dword2 >> SGE_OFF_FLD_SHIFT) & 0x0fff;
			ext_addr = (uint32_t)sys_phys_to_virt(addr);
		} else {
			off += 2 * sizeof(uint64_t);
		}
	} while (!final);
#endif
}

