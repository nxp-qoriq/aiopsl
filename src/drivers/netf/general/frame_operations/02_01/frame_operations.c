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

extern __PROFILE_SRAM struct storage_profile 
		storage_profile[SP_NUM_OF_STORAGE_PROFILES];

#define SP_BDI_MASK		0x00080000
#define SP_BP_ID_MASK		0x3FFF
#define SP_PBS_MASK		0xFFC0

int create_frame(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size,
		uint8_t spid,
		uint8_t *frame_handle)
{
	
#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("frame_operations.c",(uint32_t)fd, ALIGNMENT_32B);
#endif	
	struct fdma_present_frame_params present_frame_params;
	struct fdma_insert_segment_data_params insert_params;

	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;
	uint64_t fd_addr;
	uint32_t flags;
	uint16_t icid, bpid, offset;
	int32_t status;

	/* *fd = {0};*/
	fd->addr = 0;
	fd->control = 0;
	fd->flc = 0;
	fd->frc = 0;
	fd->length = 0;
	fd->offset = 0;

	LDPAA_FD_SET_IVP(fd, 1);
	
	/* In case we will need PTA we cannot generate a frame with FDMA command
	 * implicitly. We must Form the FD manually. */
	if ((storage_profile[spid].mode_bits1) & 0x80){
		icid = LH_SWAP(0, 
			(uint16_t *)&(storage_profile[spid].ip_secific_sp_info))
				& ADC_ICID_MASK;
		flags = (LW_SWAP(0, 
			(uint32_t *)&(storage_profile[spid].ip_secific_sp_info))
				& SP_BDI_MASK) ? FDMA_ACQUIRE_BDI_BIT : 0;
		/* set the offset to the end of the buffer */
		offset = (LH_SWAP(0, 
				&(storage_profile[spid].pbs1)) & SP_PBS_MASK);
		bpid = LH_SWAP(0, &(storage_profile[spid].bpid1)) 
				& SP_BP_ID_MASK;
		status = fdma_acquire_buffer(icid, flags, bpid, &fd_addr);
		if (status)
			return status;
		
		LDPAA_FD_SET_ADDR(fd, fd_addr);
		LDPAA_FD_SET_BPID(fd, bpid);
		LDPAA_FD_SET_OFFSET(fd, offset);
		LDPAA_FD_SET_PTA(fd, 1);
	}
	
	
	if ((uint32_t)fd == HWC_FD_ADDRESS) {	
		PRC_SET_SEGMENT_LENGTH(0);
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

		fdma_insert_default_segment_data(0, data, size,
				FDMA_REPLACE_SA_REPRESENT_BIT);
		/* Re-run parser */
		status = parse_result_generate_default(0);
		/* Mark running sum as invalid */
		pr->gross_running_sum = 0;
		*frame_handle = PRC_GET_FRAME_HANDLE();
		return status;
	} else {
		present_frame_params.fd_src = (void *)fd;
		present_frame_params.asa_size = 0;
		present_frame_params.flags = FDMA_INIT_NO_FLAGS;
		present_frame_params.pta_dst = (void *)
				PTA_NOT_LOADED_ADDRESS;
		present_frame_params.present_size = 0;
		present_frame_params.seg_offset = 0;

		fdma_present_frame(&present_frame_params);

		insert_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
		insert_params.frame_handle = present_frame_params.frame_handle;
		insert_params.from_ws_src = data;
		insert_params.insert_size = size;
		insert_params.seg_handle = present_frame_params.seg_handle;
		insert_params.to_offset = 0;

		fdma_insert_segment_data(&insert_params);

		*frame_handle = present_frame_params.frame_handle;

		return SUCCESS;
	}
}

int create_fd(
		struct ldpaa_fd *fd,
		void *data,
		uint16_t size,
		uint8_t spid)
{
	struct fdma_present_frame_params present_frame_params;
	struct fdma_insert_segment_data_params insert_params;
	struct fdma_amq amq;
	uint64_t fd_addr;
	uint32_t flags;
	uint16_t icid, bpid, offset;
	int32_t status;

#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("frame_operations.c",(uint32_t)fd, ALIGNMENT_32B);
#endif
	
	/* *fd = {0};*/
	fd->addr = 0;
	fd->control = 0;
	fd->flc = 0;
	fd->frc = 0;
	fd->length = 0;
	fd->offset = 0;
	
	LDPAA_FD_SET_IVP(fd, 1);
	
	/* In case we will need PTA we cannot generate a frame with FDMA command
	 * implicitly. We must Form the FD manually. */
	if ((storage_profile[spid].mode_bits1) & 0x80){
		icid = LH_SWAP(0, 
			(uint16_t *)&(storage_profile[spid].ip_secific_sp_info))
				& ADC_ICID_MASK;
		flags = (LW_SWAP(0, 
			(uint32_t *)&(storage_profile[spid].ip_secific_sp_info))
				& SP_BDI_MASK) ? FDMA_ACQUIRE_BDI_BIT : 0;
		/* set the offset to the end of the buffer */
		offset = (LH_SWAP(0, 
				&(storage_profile[spid].pbs1)) & SP_PBS_MASK);
		bpid = LH_SWAP(0, &(storage_profile[spid].bpid1)) 
				& SP_BP_ID_MASK;
		status = fdma_acquire_buffer(icid, flags, bpid, &fd_addr);
		if (status)
			return status;
		
		LDPAA_FD_SET_ADDR(fd, fd_addr);
		LDPAA_FD_SET_BPID(fd, bpid);
		LDPAA_FD_SET_OFFSET(fd, offset);
		LDPAA_FD_SET_PTA(fd, 1);
	}

	if ((uint32_t)fd == HWC_FD_ADDRESS) {	
		PRC_SET_SEGMENT_LENGTH(0);
		PRC_SET_SEGMENT_OFFSET(0);
		PRC_RESET_NDS_BIT();
		fdma_present_default_frame();
		fdma_insert_default_segment_data(0, data, size,
				FDMA_REPLACE_SA_CLOSE_BIT);

		return fdma_store_frame_data(PRC_GET_FRAME_HANDLE(),
						spid, &amq);
	} else {
		present_frame_params.fd_src = (void *)fd;
		present_frame_params.asa_size = 0;
		present_frame_params.flags = FDMA_INIT_NO_FLAGS;
		present_frame_params.pta_dst = (void *)
				PTA_NOT_LOADED_ADDRESS;
		present_frame_params.present_size = 0;
		present_frame_params.seg_offset = 0;

		fdma_present_frame(&present_frame_params);

		insert_params.flags = FDMA_REPLACE_SA_CLOSE_BIT;
		insert_params.frame_handle = present_frame_params.frame_handle;
		insert_params.from_ws_src = data;
		insert_params.insert_size = size;
		insert_params.seg_handle = present_frame_params.seg_handle;
		insert_params.to_offset = 0;

		fdma_insert_segment_data(&insert_params);

		return fdma_store_frame_data(present_frame_params.frame_handle,
				spid, &amq);
	}
}

int create_arp_request_broadcast(
		struct ldpaa_fd *fd,
		uint32_t local_ip,
		uint32_t target_ip,
		uint8_t spid,
		uint8_t *frame_handle)
{

#ifdef CHECK_ALIGNMENT 	
	DEBUG_ALIGN("frame_operations.c",(uint32_t)fd, ALIGNMENT_32B);
#endif
	
	uint8_t target_eth[6];
	*((uint32_t *)target_eth) = (uint32_t)BROADCAST_MAC;
	*((uint16_t *)(target_eth+4)) = (uint16_t)BROADCAST_MAC;

	return create_arp_request(
		fd, local_ip, target_ip, (uint8_t *)target_eth, 
		spid, frame_handle);
}

int create_arp_request(
		struct ldpaa_fd *fd,
		uint32_t local_ip,
		uint32_t target_ip,
		uint8_t *target_eth,
		uint8_t spid,
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
			fd, (void *)arp_data, ARP_PKT_MIN_LEN, 
			spid, frame_handle);
}
