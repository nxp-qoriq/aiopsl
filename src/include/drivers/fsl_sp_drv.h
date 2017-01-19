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
@File		fsl_sp_drv.h

@Description	Storage Profile Interface API
*//***************************************************************************/
#ifndef __FSL_SP_DRV_H
#define __FSL_SP_DRV_H

/**************************************************************************//**
@Group		sp_drv_g SP DRV

@Description	Contains initialization and runtime control APIs for Storage
		Profiles management. It's not recommended to call any of the
		API functions on the packet processing data-path.

		Application reserves a number of Storage profiles by setting
		the value of the "spid_count" count in the "aiop_app_init_info"
		global structure. Up to 256 storage profiles may be reserved.
		Be aware for each DPNI belonging to the AIOP, one or two storage
		profiles are reserved. AIOP initialization fails if the
		"spid_count" value exceeds the number of available storage
		profiles.
		The driver manages an array of reserved storage profiles with
		indexes up to spid_count - 1. In order to get the SPID of a
		storage profile use sp_drv_get_spid() API function.

		On each SP one may configure 1 or 2 buffer pools. To do that
		one should call the sp_drv_register_bp_requirements() once or
		twice in the early stage of the application initialization.
		AIOP initialization fails if there are no sufficient buffer
		pools objects declared in the AIOP resources container of the
		DPL or there is not enough space left in the memory partitions
		used for buffer pools allocation.

		When a SP is initialized the following defaults are used :
		1. AIOP specific storage profile information
			- ICID is set with the value of the AIOP context ICID,
			- PL bit is set,
			- BDI bit is set.
		Use :
			sp_drv_set_aiop_icid(),
			sp_drv_set_aiop_pl(),
			sp_drv_set_aiop_bdi(),
		API functions to change the defaults.
		2. Storage profile frame format and data placement controls
			- FF(Frame Format), VA(Virtual Address), PTAR(Pass
			Through Annotation Room), SGHR(Scatter/Gather HeadRoom)
			and ASAR(Accelerator Specific Annotation Room) fields
			are cleared,
			- DHR(Data Head Room) is set to 256 bytes.
		DL(Data Length), BS(Buffer Source) and DLC(Data Length Control)
		fields are not used (cleared).
		Use :
			sp_drv_set_ff(),
			sp_drv_set_va(),
			sp_drv_set_ptar(),
			sp_drv_set_sghr(),
			sp_drv_set_asar(),
			sp_drv_set_dhr(),
		API functions to change the defaults.
		3. SP Buffer Pool controls
			- BMT(Bypass Memory Translation) is set with the value
			of the AIOP BMT bit in each configured BP,
			- BPV(Buffer Pool Valid) is set in each configured BP.
			- SR(Scarce Resource) is cleared,
			- BP(Backup Pool) is cleared.
		BDP(Buffer Depletion Pause) field is not used (cleared).
		Use :
			sp_drv_set_bp_sr(),
			sp_drv_set_bp_bp(),
		API functions to change the defaults.

		Note: If two BPs are configured on a SP, the setting of a BP bit
		associated with the second pool, configures the buffer selection
		policy for the profile as either "best fit" or "back-up only".
		Use the sp_drv_set_bp_bp() API in order to change the default 0
		value of the BP bit as application needs.
			- Best fit: With each allocation FDMA tries to use the
		first buffer pool (which for this policy must contain smaller
		buffers than the second). If the smaller buffers of the first
		pool can be used to store the full/remaining data then it will
		be used. If not, then the second buffer pool is used.
		Both buffer pool must have their BP bit cleared to be in this
		mode.
			- Backup Pool Only: With each allocation first buffer
		pool is used unless it is exhausted. Otherwise the second buffer
		pool will be used. To be in this mode, the first buffer pool
		must have BP cleared and the second buffer pool must have BP
		set. This mode could be used for high performance when the first
		buffer pool is using PEB and the second buffer pool is using
		external system memory buffers.
@{
*//***************************************************************************/

/**************************************************************************//**
@enum sp_frame_format

@Description	Storage Profile driver frame format types

@{
*//***************************************************************************/

enum sp_frame_format {
	/** Single buffer or SG buffer */
	SP_FF_SINGLE_OR_SG_BUFFER = 0x0,
	/** Single buffer */
	SP_FF_SINGLE_BUFFER = 0x20000000
};

/* @} end of enum sp_frame_format */

/**************************************************************************//**
@Function	sp_drv_register_bp_requirements

@Description	Register the Storage Profile requirements of the application.
		An application may configure up to 2 Buffer Pools on each SP.
		On the first call the parameters are registered for the Buffer
		Pool #1, on the second one for the BP #2.
		Notes :
		1. Application should call this function on its "early
		initialization" phase.
		2. Application initialization fails if the requested resources
		are not available. Resources are allocated and the BPs are seed
		in the initialization phase of the SP driver.

@Param[in]	sp_idx : Index of a reserved SP( 0 to spid_count).

@Param[in]	buffer_cnt : Number of packet buffers to be populated in a SP
		BMan Buffer Pool.

@Param[in]	buffer_size : Size of the buffers to be populated in a SP
		BMan Buffer Pool. It is recommended a 64 bytes multiple value.

@Param[in]	alignment : Required alignment for the packet buffer.

@Param[in]	mem_pid : Memory partition for the packet buffers. The following
		partition are allowed on all platforms : MEM_PART_SYSTEM_DDR,
		MEM_PART_PEB. The MEM_PART_DP_DDR partition is not allowed on
		LS1088A platforms.

@Return		0 : Success. Function returns success when application reserved
		no storage profiles (spid_count = 0).
		-EINVAL : Wrong parameter(s)

*//***************************************************************************/
int sp_drv_register_bp_requirements(uint8_t sp_idx, uint32_t buffer_cnt,
				    uint16_t buffer_size, uint16_t alignment,
				    enum memory_partition_id mem_pid);

/**************************************************************************//**
@Function	sp_drv_get_spid

@Description	Gets the Storage Profile ID. In order to use this SPID one
		should require one on more BPs for this SP.

@Param[in]	sp_idx : Index of a reserved SP (0 to spid_count).

@Param[out]	spid : Pointer to the Storage Profile ID value

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0

*//***************************************************************************/
int sp_drv_get_spid(uint8_t sp_idx, uint8_t *spid);

/**************************************************************************//**
@Function	sp_drv_get_bpid

@Description	Gets the Buffer Pool ID of a BMan pool configured on a SP.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	bp_idx : Index of a configured BP (0 or 1).

@Param[out]	bpid : Pointer to the Buffer Pool ID value

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_get_bpid(uint8_t sp_idx, uint8_t bp_idx, uint16_t *bpid);

/**************************************************************************//**
@Function	sp_drv_set_aiop_icid

@Description	Sets the AIOP specific Isolation Context ID value in the
		selected storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	icid : ICID value on at most 15 bits.

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_aiop_icid(uint8_t sp_idx, uint16_t icid);

/**************************************************************************//**
@Function	sp_drv_set_aiop_pl

@Description	Sets the AIOP specific Privilege Level bit in the selected
		storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	pl : PL value (0 or 1).

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_aiop_pl(uint8_t sp_idx, uint8_t pl);

/**************************************************************************//**
@Function	sp_drv_set_aiop_bdi

@Description	Sets the AIOP specific Bypass Data-path Isolation bit in the
		selected storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	bdi : BDI value (0 or 1).

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_aiop_bdi(uint8_t sp_idx, uint8_t bdi);

/**************************************************************************//**
@Function	sp_drv_set_dhr

@Description	Sets the AIOP specific Data Head-room value in the selected
		storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	dhr : DHR value on at most 12 bits.

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_dhr(uint8_t sp_idx, uint16_t dhr);

/**************************************************************************//**
@Function	sp_drv_set_ff

@Description	Sets the Frame Format value in the selected storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	ff : FF value (one of the enumerated values).

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_ff(uint8_t sp_idx, enum sp_frame_format ff);

/**************************************************************************//**
@Function	sp_drv_set_va

@Description	Sets the Virtual Address bit in the selected storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	va : VA value (0 or 1).

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_va(uint8_t sp_idx, uint8_t va);

/**************************************************************************//**
@Function	sp_drv_set_ptar

@Description	Sets the Pass Through Annotation Room bit in the selected
		storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	ptar : PTAR value (0 or 1). The pass through annotation room is
		encoded in terms of 64-byte and its value is used to optionally
		generate an additional space of 64 bytes for an anticipated pass
		through annotation in the front of the frame.

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_ptar(uint8_t sp_idx, uint8_t ptar);

/**************************************************************************//**
@Function	sp_drv_set_sghr

@Description	Sets the Scatter/Gather HeadRoom bit in the selected storage
		profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	sghr : SGHR value (0 or 1). The scatter/gather head-room is
		encoded in terms of 64-byte and its value is used to optionally
		generate an additional space of 64 bytes for anticipated SG
		table growth in the front of the frame.

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_sghr(uint8_t sp_idx, uint8_t sghr);

/**************************************************************************//**
@Function	sp_drv_set_asar

@Description	Sets the Accelerator Specific Annotation Room value in the
		selected storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	asar : ASAR value on at most 4 bits. The accelerator specific
		annotation room control field is encoded in terms of 64-byte
		increments and its value is used to generate HW annotation
		space in the frame.

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_asar(uint8_t sp_idx, uint8_t asar);

/**************************************************************************//**
@Function	sp_drv_set_bp_sr

@Description	Sets the Scarce Resource bit in the selected buffer pool of a
		selected storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	bp_idx : Index of a configured BP (0 or 1).

@Param[in]	sr : SR value (0 or 1). If the SR bit is set the amount of
		memory consumed by the buffer need to be reflected in the
		FD[MEM] encoding and FDs conveying such buffers need to set the
		SL bit in FD[FMT], reduce the FD[LENGTH] encoding to 18 bit and
		make room for a 12 bit FD[MEM] encoding. If the buffer is
		utilized in an SG FD the SGE entry's SL bit in SGE[FMT] and the
		SGE[SR] bit must be set.

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_bp_sr(uint8_t sp_idx, uint8_t bp_idx, uint8_t sr);

/**************************************************************************//**
@Function	sp_drv_set_bp_bp

@Description	Sets the Backup Pool bit in the selected buffer pool of a
		selected storage profile.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count).

@Param[in]	bp_idx : Index of a configured BP (0 or 1).

@Param[in]	bp : BP value (0 or 1).

@Return		0 : Success;
		-EINVAL : Wrong parameter(s) or spid_count = 0
		-EIO : No buffer pool on this SP

*//***************************************************************************/
int sp_drv_set_bp_bp(uint8_t sp_idx, uint8_t bp_idx, uint8_t bp);

#ifdef SL_DEBUG
/**************************************************************************//**
@Function	sp_dump

@Description	Dumps Storage Profile.
		Note : If application calls this function, application and
		AIOP_SL must be build with the SL_DEBUG macro defined.

@Param[in]	sp_idx : Index of the a reserved SP (0 to spid_count). No dump
		is performed if the index is outside the allowed range or no
		SP was reserved by the application.

@Return		None

*//***************************************************************************/
void sp_dump(uint8_t sp_idx);
#else
	#define sp_dump(_a)
#endif	/* SL_DEBUG */

/** @} */ /* end of sp_drv_g SP DRV group */

#endif /* __FSL_SP_DRV_H */
