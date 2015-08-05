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

#ifndef __FSL_ICONTEXT_H
#define __FSL_ICONTEXT_H

/**************************************************************************//**
@Group         ic_g  Isolation Context

@Description	API to be used for memory and BMAN pool accesses
		using the specific isolation context attributes.

@{
 *//***************************************************************************/

#define ICONTEXT_INVALID ((uint16_t)-1) /**< Isolation context is not valid */

/**************************************************************************//**
@Description	Isolation context structure.

		Do not modify the content of this structure, it must be set by
		icontext_get().
*//***************************************************************************/
struct icontext {
	uint32_t dma_flags;
	/**< Flags that will be used for DMA */
	uint32_t bdi_flags;
	/**< Flags that will be used for BMAN pool */
	uint16_t icid;
	/**< Isolation context id that will be used for DMA and BMAN pool 
	 *   If icid = #ICONTEXT_INVALID the values are not valid */
};

/**************************************************************************//**
@Function	icontext_cmd_get

@Description	Copy isolation context parameters for current command.

@Param[out]	ic	- Isolation context structure to be used
			with icontext dependent API.

*//***************************************************************************/
void icontext_cmd_get(struct icontext *ic);

/**************************************************************************//**
@Function	icontext_aiop_get

@Description	Copy isolation context parameters for AIOP.

@Param[out]	ic	- Isolation context structure to be used
			with icontext dependent API.

*//***************************************************************************/
void icontext_aiop_get(struct icontext *ic);

/**************************************************************************//**
@Function	icontext_get

@Description	Copy isolation context parameters for DPCI id.

@Param[in]	dpci_id	- ID of DPCI device or its peer ID.
@Param[out]	ic	- Isolation context structure to be used
			with icontext dependent API.

@Return		0	 - on success,
		-ENAVAIL - DPCI id was not found.

@Cautions	This API must be called after cmdif_open() was triggered
		by GPP otherwise it will result in empty icontext structure.
*//***************************************************************************/
int icontext_get(uint16_t dpci_id, struct icontext *ic);

/**************************************************************************//**
@Function	icontext_dma_read

@Description	DMA read into workspace location.

@Param[in]	ic	- Isolation context structure to be used
			with icontext dependent API.
@Param[in]	src	- System memory source for DMA data.
@Param[in]	size	- The number of bytes to be copied into dest buffer.
@Param[out]	dest	- Pointer to workspace location to where data should
			be copied.

@Return		0	- on success, POSIX error otherwise.
*//***************************************************************************/
int icontext_dma_read(struct icontext *ic, uint16_t size, uint64_t src, void *dest);

/**************************************************************************//**
@Function	icontext_dma_write

@Description	DMA write from workspace location.

@Param[in]	ic	- Isolation context structure to be used
			with icontext dependent API.
@Param[in]	src	- Pointer to workspace location from where data should
 			be copied.
@Param[in]	size	- The number of bytes to be copied into dest buffer.
@Param[out]	dest	- System memory target address for DMA data.

@Return		0	- on success, POSIX error otherwise.
*//***************************************************************************/
int icontext_dma_write(struct icontext *ic, uint16_t size, void *src, uint64_t dest);

/**************************************************************************//**
@Function	icontext_acquire

@Description	Acquire buffer from BMAN pool.

@Param[in]	ic	- Isolation context structure to be used
			with icontext dependent API.
@Param[in]	bpid	- BMAN pool id that matches ic->icid.
@Param[out]	addr	- Buffer address as returned by BMAN pool.

@Return		0	- on success, POSIX error otherwise.
*//***************************************************************************/
int icontext_acquire(struct icontext *ic, uint16_t bpid, uint64_t *addr);

/**************************************************************************//**
@Function	icontext_release

@Description	Release buffer into BMAN pool.

@Param[in]	ic	- Isolation context structure to be used
			with icontext dependent API.
@Param[in]	bpid	- BMAN pool id that matches ic->icid.
@Param[in]	addr	- Address to be released into BMAN pool.

@Return		0	- on success, POSIX error otherwise.
*//***************************************************************************/
int icontext_release(struct icontext *ic, uint16_t bpid, uint64_t addr);

/**************************************************************************//**
@Function	icontext_ws_set

@Description	Update the Additional Dequeue Context that has been copied to 
		the workspace with the new isolation context.

		It is useful if you must close the command default frame and
		reopen it again:
		- icontext_ws_set(&cmd_ic);
		- fdma_present_default_frame();
		- icontext_ws_set(&aiop_ic);
		- continue working with command as usual 
		
@Param[in]	ic	- Isolation context to be set in the workspace.

*//***************************************************************************/
void icontext_ws_set(struct icontext *ic);

/** @} *//* end of ic_g group */

#endif
