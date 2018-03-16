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

/**************************************************************************//**
@File		fsl_fdma.h

@Description	This file contains the AIOP SW FDMA API

*//***************************************************************************/

#include "fsl_types.h"
#include "fsl_errors.h"
#include "fsl_frame_operations.h"
#include "fsl_fdma.h"
#include "fsl_parser.h"
#include "fsl_dpni_drv.h"
#include "net.h"

	/* WA for TKT254401 */
extern __PROFILE_SRAM struct storage_profile 
		storage_profile[SP_NUM_OF_STORAGE_PROFILES];

#define SP_BDI_MASK		0x00080000
#define SP_BP_ID_MASK		0x3FFF
#define SP_PBS_MASK		0xFFC0
#define FRAME_INITIAL_SIZE	1
	/* end of WA for TKT254401 */

FRAME_OP_CODE_PLACEMENT int create_frame(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size,
		uint8_t *frame_handle)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("frame_operations.c",(uint32_t)fd, ALIGNMENT_32B);
#endif
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	int32_t status;
	/* WA for TKT254401 */
	uint64_t fd_addr;
	uint16_t icid, bpid, offset;
	uint32_t flags;
	struct storage_profile *sp;
	uint16_t *ws_address_rs;
	uint16_t seg_size_rs;
	
	sp = &storage_profile[*((uint8_t *)HWC_SPID_ADDRESS)];
	icid = LH_SWAP(0, (uint16_t *)&(sp->ip_secific_sp_info)) & ADC_ICID_MASK;
	flags = (LW_SWAP(0, (uint32_t *)&(sp->ip_secific_sp_info)) & SP_BDI_MASK) ? FDMA_ACQUIRE_BDI_BIT : 0;
	bpid = LH_SWAP(0, &(sp->bpid1)) & SP_BP_ID_MASK;
	status = fdma_acquire_buffer(icid, flags, bpid, &fd_addr);
	if (status)
		return status;
	/* end of WA for TKT254401 */
	
	/* *fd = {0};*/
	fd->addr = 0;
	fd->control = 0;
	fd->flc = 0;
	fd->frc = 0;
	fd->length = 0;
	fd->offset = 0;
	/* WA for TKT254401 */	
	LDPAA_FD_SET_ADDR(fd, fd_addr);
	LDPAA_FD_SET_LENGTH(fd, FRAME_INITIAL_SIZE);
	LDPAA_FD_SET_BPID(fd, bpid);
	offset = (LH_SWAP(0, &(sp->pbs1)) & SP_PBS_MASK) - FRAME_INITIAL_SIZE;
	LDPAA_FD_SET_OFFSET(fd, offset);
	/* end of WA for TKT254401 */
	
	/* Set PTA for later use if ptar is set (it will not be possible to 
	 * change later) */
	if ((sp->mode_bits1) & 0x80)
		LDPAA_FD_SET_PTA(fd, 1);

	if ((uint32_t)fd == HWC_FD_ADDRESS) {
		PRC_SET_ASA_SIZE(0);
		PRC_SET_PTA_ADDRESS(PRC_PTA_NOT_LOADED_ADDRESS);
		/* WA for TKT254401 */
		PRC_SET_SEGMENT_LENGTH(FRAME_INITIAL_SIZE);
		/* end of WA for TKT254401 */
		
		PRC_SET_SEGMENT_OFFSET(0);
		PRC_SET_SEGMENT_ADDRESS((uint32_t)TLS_SECTION_END_ADDR +
						DEFAULT_SEGMENT_HEADROOM_SIZE);
		PRC_RESET_NDS_BIT();
		PRC_RESET_SR_BIT();
		/* present frame with empty segment */
		fdma_present_default_frame();
		/* Insert data to the frame */
		/* Update segment length in the presentation
		 * context and represent the segment in the presentation area */
		if (size > DEFAULT_SEGMENT_SIZE)
			PRC_SET_SEGMENT_LENGTH(DEFAULT_SEGMENT_SIZE);
		else
			PRC_SET_SEGMENT_LENGTH(size);
		/* WA for TKT254401 */
		ws_address_rs = (void *) PRC_GET_SEGMENT_ADDRESS();
		seg_size_rs = PRC_GET_SEGMENT_LENGTH();
		fdma_replace_default_segment_data(0, FRAME_INITIAL_SIZE, data, size, 
				ws_address_rs, seg_size_rs, 
				FDMA_REPLACE_SA_REPRESENT_BIT);
		/* end of WA for TKT254401 */
		
		/* Re-run parser */
		status = parse_result_generate_default(0);
		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;
		*frame_handle = PRC_GET_FRAME_HANDLE();
		return status;
	} else {
		/* WA for TKT254401 */
		return SUCCESS;
		/* end of WA for TKT254401 */
	}
}

int create_fd(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size,
		uint8_t spid)
{
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("frame_operations.c",(uint32_t)fd, ALIGNMENT_32B);
#endif
	
	/* WA for TKT254401 */
	int32_t status;
	uint64_t fd_addr;
	uint16_t icid, bpid, offset;
	uint32_t flags;
	struct storage_profile *sp;
	
	spid = 0;
	sp = &storage_profile[*((uint8_t *)HWC_SPID_ADDRESS)];
	icid = LH_SWAP(0, (uint16_t *)&(sp->ip_secific_sp_info)) & ADC_ICID_MASK;
	flags = (LW_SWAP(0, (uint32_t *)&(sp->ip_secific_sp_info)) & SP_BDI_MASK) ? FDMA_ACQUIRE_BDI_BIT : 0;
	bpid = LH_SWAP(0, &(sp->bpid1)) & SP_BP_ID_MASK;
	status = fdma_acquire_buffer(icid, flags, bpid, &fd_addr);
	if (status)
		return status;
	/* end of WA for TKT254401 */

	/* *fd = {0};*/
	fd->addr = 0;
	fd->control = 0;
	fd->flc = 0;
	fd->frc = 0;
	fd->length = 0;
	fd->offset = 0;
	/* WA for TKT254401 */	
	LDPAA_FD_SET_ADDR(fd, fd_addr);
	LDPAA_FD_SET_LENGTH(fd, FRAME_INITIAL_SIZE);
	LDPAA_FD_SET_BPID(fd, bpid);
	offset = (LH_SWAP(0, &(sp->pbs1)) & SP_PBS_MASK) - FRAME_INITIAL_SIZE;
	LDPAA_FD_SET_OFFSET(fd, offset);
	/* end of WA for TKT254401 */
	
	/* Set PTA for later use if ptar is set (it will not be possible to 
	 * change later) */
	if ((sp->mode_bits1) & 0x80)
		LDPAA_FD_SET_PTA(fd, 1);

	if ((uint32_t)fd == HWC_FD_ADDRESS) {
		PRC_SET_ASA_SIZE(0);
		PRC_SET_PTA_ADDRESS(PRC_PTA_NOT_LOADED_ADDRESS);
		/* WA for TKT254401 */
		PRC_SET_SEGMENT_LENGTH(FRAME_INITIAL_SIZE);
		/* end of WA for TKT254401 */
		PRC_SET_SEGMENT_OFFSET(0);
		PRC_RESET_NDS_BIT();
		fdma_present_default_frame();

		/* WA for TKT254401 */	
		fdma_replace_default_segment_data(0, FRAME_INITIAL_SIZE, data, 
				size, 0, 0, FDMA_REPLACE_SA_CLOSE_BIT);
		/* end of WA for TKT254401 */

		return fdma_store_default_frame_data();
	} else {
		/* WA for TKT254401 */
		return SUCCESS;
		/* end of WA for TKT254401 */
	}
}

int create_arp_request_broadcast(
		struct ldpaa_fd *fd,
		uint32_t local_ip,
		uint32_t target_ip,
		uint8_t *frame_handle)
{

#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("frame_operations.c",(uint32_t)fd, ALIGNMENT_32B);
#endif
	
	uint8_t target_eth[6];
	*((uint32_t *)target_eth) = (uint32_t)BROADCAST_MAC;
	*((uint16_t *)(target_eth+4)) = (uint16_t)BROADCAST_MAC;

	return create_arp_request(
		fd, local_ip, target_ip, (uint8_t *)target_eth, frame_handle);
}

int create_arp_request(
		struct ldpaa_fd *fd,
		uint32_t local_ip,
		uint32_t target_ip,
		uint8_t *target_eth,
		uint8_t *frame_handle)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("frame_operations.c",(uint32_t)fd, ALIGNMENT_32B);
#endif
	
	uint8_t arp_data[ARP_PKT_MIN_LEN];
	uint8_t *ethhdr = arp_data;
	struct arphdr *arp_hdr =
		(struct arphdr *)(arp_data + sizeof(struct ethernethdr));
	uint8_t local_hw_addr[NET_HDR_FLD_ETH_ADDR_SIZE];

	/* get local HW address */
	dpni_drv_get_primary_mac_addr(
			(uint16_t)task_get_receive_niid(), local_hw_addr);

	/* set ETH destination address */
	*((uint32_t *)(&ethhdr[0])) = *((uint32_t *)target_eth);
	*((uint16_t *)(&ethhdr[4])) = *((uint16_t *)(target_eth+4));
	/* set ETH source address */
	*((uint32_t *)(&ethhdr[6])) = *((uint32_t *)local_hw_addr);
	*((uint16_t *)(&ethhdr[10])) = *((uint16_t *)(local_hw_addr+4));
	/* set ETH ARP EtherType */
	*((uint16_t *)(&ethhdr[12])) = ARP_ETHERTYPE;

	/* set ARP HW type */
	arp_hdr->hw_type = ARPHDR_ETHER_PRO_TYPE;
	/* set ARP protocol (IPv4) type */
	arp_hdr->pro_type = ARPHDR_IPV4_PRO_TYPE;
	/* set ARP HW address length */
	arp_hdr->hw_addr_len = NET_HDR_FLD_ETH_ADDR_SIZE;
	/* set ARP protocol (IPv4) address length */
	arp_hdr->pro_addr_len = ARPHDR_IPV4_ADDR_LEN;
	/* set ARP operation (ARP REQUEST) */
	arp_hdr->operation = ARP_REQUEST_OP;
	/* set ARP sender HW address */
	*((uint32_t *)arp_hdr->src_hw_addr) = *((uint32_t *)local_hw_addr);
	*((uint16_t *)(arp_hdr->src_hw_addr + 4)) =
			*((uint16_t *)(local_hw_addr+4));
	/* set ARP sender protocol (IPv4) address */
	arp_hdr->src_pro_addr = local_ip;
	/* set ARP destination HW address (unknown at this point) */
	*((uint32_t *)arp_hdr->dst_hw_addr) = 0;
	*((uint16_t *)(arp_hdr->dst_hw_addr + 4)) = 0;
	/* set ARP protocol (IPv4) address */
	arp_hdr->dst_pro_addr = target_ip;

	/* zero additional packet data since ARP has a minimum packet length of
	 * 64 bytes (ARP_PKT_MIN_LEN). */
	*((uint16_t *)&arp_data[sizeof(struct ethernethdr) + ARP_HDR_LEN]) = 0;
	*((uint32_t *)&arp_data[sizeof(struct ethernethdr) + ARP_HDR_LEN + 2]) = 0;
	*((uint32_t *)&arp_data[sizeof(struct ethernethdr) + ARP_HDR_LEN + 6]) = 0;
	*((uint32_t *)&arp_data[sizeof(struct ethernethdr) + ARP_HDR_LEN + 10]) = 0;
	*((uint32_t *)&arp_data[sizeof(struct ethernethdr) + ARP_HDR_LEN + 14]) = 0;
	*((uint32_t *)&arp_data[sizeof(struct ethernethdr) + ARP_HDR_LEN + 18]) = 0;

	return create_frame(
			fd, (void *)arp_data, ARP_PKT_MIN_LEN, frame_handle);
}
