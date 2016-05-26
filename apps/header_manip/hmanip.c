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

#include "fsl_types.h"
#include "fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_l2.h"
#include "fsl_evmng.h"
#include "fsl_stdlib.h"
#include "fsl_parser.h"
#include "fsl_fdma.h"
#include "fsl_ip.h"
#include "fsl_cdma.h"
#include "hmanip.h"

#define AIOP_APP_NAME		"HEADER_MANIP"
/* L4 fields */
#define NH_FLD_L4_PORT_SRC	(1)
#define NH_FLD_L4_PORT_DST	(NH_FLD_L4_PORT_SRC << 1)

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg);
static inline void l2_eth_hw_swap(void);

#ifdef PRINT_FRAME_INFO
static uint64_t		print_frame_mutex;
static inline void print_frame_info(void);
#else
	#define print_frame_info()
#endif

/*
 * Basic GRE Structure
 *
 * For details see RFC 1701
 *
 * In case additional fields are required, one can simply append the
 * unsigned integers to the char array of the new header to be inserted
 */
struct grehdr {
	uint16_t	C: 1;		/**< Checksum Present (bit 0) */
	uint16_t	R: 1;		/**< Routing Present  (bit 1) */
	uint16_t	K: 1;		/**< Key Present (bit 2) */
	uint16_t	S: 1;		/**< Sequence Number Present (bit 3) */
	uint16_t	s: 1;		/**< Strict Source Route (bit 4) */
	uint16_t	recur: 3;	/**< Recursion Control (bits 5-7) */
	uint16_t	flags: 5;	/**< GRE Flags */
	uint16_t	version: 3;	/**< GRE version */
	uint16_t	encap_protocol;	/**< Encapsulated Next Protocol */
} __packed;

/* In case additional fields from GRE, such as checksum or sequence number
 * are required, add fields' size to this */
#define NEW_HDR_SIZE	(IPV4_HDR_LENGTH + sizeof(struct grehdr))

static inline int ipv4_gre_encapsulation(void)
{
	struct ipv4hdr	*inner_ip, *outer_ip_ptr;
	struct ipv4hdr	new_outer_ip;
	struct grehdr	gre_hdr;
	char		new_frame_hdr[NEW_HDR_SIZE];
	uint16_t	outer_ip_offset, eth_hdr_len;
	int		err;

	print_frame_info();

	/* Get outer IPv4 offset */
	outer_ip_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
	eth_hdr_len = outer_ip_offset -
			(uint16_t)PARSER_GET_ETH_OFFSET_DEFAULT();

	/* Save current IP header. This will be the new inner IP header */
	inner_ip = (struct ipv4hdr *)((uint8_t *)PRC_GET_SEGMENT_ADDRESS() +
			outer_ip_offset);

	/* Copy these flags from current IP header */
	new_outer_ip.tos = inner_ip->tos;
	new_outer_ip.total_length = inner_ip->total_length + NEW_HDR_SIZE;
	new_outer_ip.id = inner_ip->id;
	new_outer_ip.flags_and_offset = inner_ip->flags_and_offset;
	new_outer_ip.ttl = inner_ip->ttl;

	/* Checksum needs to be computed by FDMA */
	new_outer_ip.hdr_cksum = 0x0000;

	/* In case of encapsulation we assume that source IP and destination IP
	 * are placed by default correct according to sending to a port, so it
	 * is not needed to swap any IP source and IP destination. GRE tunnel
	 * endpoint is responsible for processing the inner IP header and
	 * decapsulate outer IP header */
	new_outer_ip.src_addr = TUNNEL_SRC_IP;
	new_outer_ip.dst_addr = TUNNEL_DST_IP;

	/* Support only for IPv4 minimal header (20 bytes) for tunneling */
	new_outer_ip.vsn_and_ihl = IPV4_MIN_VSN_IHN;

	/* Next protocol is GRE */
	new_outer_ip.protocol = GRE_PROTOCOL;

	/* Using simplified GRE: all bits 0, GRE version 0 */
	gre_hdr.C	= 0;
	gre_hdr.K	= 0;
	gre_hdr.R	= 0;
	gre_hdr.S	= 0;
	gre_hdr.s	= 0;
	gre_hdr.recur	= 0;
	gre_hdr.version = GRE_VERSION;
	gre_hdr.flags	= 0;
	gre_hdr.encap_protocol = GRE_PTYPE_IP;

	/* In case of additional fields check GRE bits and append
	 * accordingly to new_frame_hdr */
	memcpy(new_frame_hdr, &new_outer_ip, IPV4_HDR_LENGTH);
	memcpy(new_frame_hdr + IPV4_HDR_LENGTH, &gre_hdr, sizeof(gre_hdr));

	/* Insert IPv4 + GRE header in the old IPv4 frame */
	fdma_insert_default_segment_data(outer_ip_offset, new_frame_hdr,
					 NEW_HDR_SIZE,
					 FDMA_REPLACE_SA_REPRESENT_BIT);

	/* Swap MAC source and destination addresses.Ethernet header remains at
	 *  the beginning of frame in case of GRE tunneling.
	 *  No need to re-run parser */
	l2_eth_hw_swap();

	/* FDMA insert updates the new segment address, so re-run of
	 * parser is not required to get new offset, just jump
	 * after Ethernet header size */
	outer_ip_ptr = (struct ipv4hdr *)((uint8_t *)
			PRC_GET_SEGMENT_ADDRESS() + eth_hdr_len);

	/* compute the new outer IP header's checksum */
	ip_cksum_calculate(outer_ip_ptr,
		IP_CKSUM_CALC_MODE_NONE | IP_CKSUM_CALC_MODE_DONT_UPDATE_FDMA);

	/* Run FDMA to update frame with MAC swap and IP checksum */
	fdma_modify_default_segment_full_data();

	/* Mark running sum as invalid */
	PARSER_CLEAR_RUNNING_SUM();

	/* Running the parser is mandatory in case user wants to post-process
	 * the frame after the FDMA insert operation of the new header took
	 * place. Otherwise if it is only needed to send frame to NI after
	 * header insert, re-running parser is not necessary and performance
	 * should increase */
#ifdef RERUN_PARSER
	err = parse_result_generate_default(PARSER_VALIDATE_L3_L4_CHECKSUM);
	if (err < 0) {
		fsl_print("%s : Cannot generate parse result\n",
			AIOP_APP_NAME);
		return err;
	}

	/* Mark running sum as invalid */
	PARSER_CLEAR_RUNNING_SUM();

	/* This print of information about new re-presented frame requires
	 * re-run of parser to get new offsets of headers */
	print_frame_info();
#endif

	return 0;
}

static inline int ipv4_gre_decapsulation(void)
{
	struct ipv4hdr		*p_outer_ipv4, *p_inner_ipv4;
	uint16_t		decap_size, outer_ip_offset, inner_ip_offset;
	uint32_t		ip_src_addr;
	int			err;

	print_frame_info();

	/* Get outer and inner offset IP headers. A tunnel frame should
	 * have format: ETH | Outer IP | GRE | Inner IP | Payload */
	inner_ip_offset = (uint16_t)PARSER_GET_INNER_IP_OFFSET_DEFAULT();
	outer_ip_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();

	decap_size = inner_ip_offset - outer_ip_offset;

	p_outer_ipv4 = (struct ipv4hdr *)((uint8_t *)
		PRC_GET_SEGMENT_ADDRESS() + outer_ip_offset);

	p_inner_ipv4 = (struct ipv4hdr *)((uint8_t *)
		PRC_GET_SEGMENT_ADDRESS() + inner_ip_offset);

	/* Swap MAC source and destination before decapsulation */
	l2_eth_hw_swap();

	/* store inner IP source address before changing it */
	ip_src_addr = p_inner_ipv4->src_addr;
	/* swap IP source & destination addresses */
	p_inner_ipv4->src_addr = p_inner_ipv4->dst_addr;
	p_inner_ipv4->dst_addr = ip_src_addr;

	/* Inform FDMA about the swap of ETH and IPv4 update to change frame */
	fdma_modify_default_segment_full_data();

	/* Mark running sum as invalid */
	PARSER_CLEAR_RUNNING_SUM();

	/* Remove outer IP header and GRE header from frame */
	fdma_delete_default_segment_data(outer_ip_offset, decap_size,
					 FDMA_REPLACE_SA_REPRESENT_BIT);

#ifdef RERUN_PARSER
	/* Re-run parser in case user wants post-processing of frame.
	 * Validate L3 and L4 checksums when running parser */
	err = parse_result_generate_default(PARSER_VALIDATE_L3_L4_CHECKSUM);
	if (err < 0) {
		fsl_print("%s : Cannot generate parse result\n",
			AIOP_APP_NAME);
		return err;
	}

	/* Mark running sum as invalid */
	PARSER_CLEAR_RUNNING_SUM();

	/* This print of information about new re-presented frame requires
	 * re-run of parser to get new offsets of headers */
	print_frame_info();
#endif

	return 0;
}

/* Performs frames processing */
__HOT_CODE ENTRY_POINT static void app_hmanip(void)
{
	int			err = 0;

	sl_prolog();

	if (PARSER_IS_OUTER_IPV4_DEFAULT() && !PARSER_IS_TUNNELED_IP_DEFAULT()
		&& !PARSER_IS_GRE_DEFAULT())
		/* Encapsulate using GRE regular IPv4 frames */
		ipv4_gre_encapsulation();
	else if (PARSER_IS_OUTER_IPV4_DEFAULT() &&
		 PARSER_IS_TUNNELED_IP_DEFAULT() && PARSER_IS_GRE_DEFAULT())
		/* Decapsulate GRE IPv4 frames */
		ipv4_gre_decapsulation();
	else {
		/* Discard non-IPV4 or non-GRE frame and terminate task */
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		fdma_terminate_task();
	}

	/* Reflect back the packet on the same interface
	 * from which it was received */
	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);
	if (!err)
		fdma_terminate_task();

	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else {/* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();
	}


	pr_err("Failed to send frame\n");
	fdma_terminate_task();
}

static inline void l2_eth_hw_swap(void)
{
	uint8_t  local_hw_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	uint8_t  *ethhdr = PARSER_GET_ETH_POINTER_DEFAULT();
	uint8_t	 *eth_src, *eth_dst;

	/* get ETH source and destination addresses */
	eth_dst = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT());
	eth_src = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT() +
					NET_HDR_FLD_ETH_ADDR_SIZE);

	/* store MAC_DST */
	*((uint32_t *)&local_hw_addr[0]) = *((uint32_t *)eth_dst);
	*((uint16_t *)&local_hw_addr[4]) = *((uint16_t *)(eth_dst + 4));

	/* set ETH destination address */
	*((uint32_t *)(&ethhdr[0])) = *((uint32_t *)(eth_src));
	*((uint16_t *)(&ethhdr[4])) = *((uint16_t *)(eth_src + 4));

	/* set ETH source address */
	*((uint32_t *)(&ethhdr[6])) = *((uint32_t *)&local_hw_addr[0]);
	*((uint16_t *)(&ethhdr[10])) = *((uint16_t *)&local_hw_addr[4]);
}

static int app_dpni_link_up_cb(uint8_t generator_id, uint8_t event_id,
				uint64_t app_ctx, void *event_data)
{
	uint16_t	ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("%s : ni%d link is UP\n", AIOP_APP_NAME, ni);
	return 0;
}

static int app_dpni_link_down_cb(uint8_t generator_id, uint8_t event_id,
				  uint64_t app_ctx, void *event_data)
{
	uint16_t	ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("%s : ni%d link is DOWN\n", AIOP_APP_NAME, ni);
	return 0;
}

static int app_dpni_add_cb(uint8_t generator_id, uint8_t event_id,
			   uint64_t app_ctx, void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;

	UNUSED(generator_id);
	UNUSED(event_id);

	/* Get endpoint and skip DPNI-DPNI connection from app initialization */
	err = dpni_drv_get_connected_obj(ni, &dpni_ep_id,
					 dpni_ep_type, &link_state);
	if (err) {
		pr_err("Cannot get connected object for NI %d\n", ni);
		return err;
	}
	if (strcmp(dpni_ep_type, "dpni") == 0)
		return 0;

	/* Get DPNI ID for current Network Interface ID */
	err = dpni_drv_get_dpni_id(ni, &dpni_id);
	if (err) {
		pr_err("Cannot get DPNI ID for NI %d\n", ni);
		return err;
	}

	/* Configure frame processing callback */
	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("Cannot configure processing callback on NI %d\n", ni);
		return err;
	}

	/* Default ordering mode should be Concurrent */
	err = dpni_drv_set_concurrent(ni);
	if (err) {
		pr_err("Cannot set Concurrent Execution on NI %d\n", ni);
		return err;
	}

	/* Configure key generation for initial ordering scope */
	app_fill_kg_profile(&kg_cfg);
	err = dpni_drv_set_order_scope(ni, &kg_cfg);
	if (err) {
		pr_err("Cannot set order scope on NI %d\n", ni);
		return err;
	}

	/* Get DPNI MAC address  */
	err = dpni_drv_get_primary_mac_addr(ni, mac_addr);
	if (err) {
		pr_err("Cannot obtain primary MAC ADDR for NI %d\n", ni);
		return err;
	}

	/* Enable DPNI to receive frames */
	err = dpni_drv_enable(ni);
	if (err) {
		pr_err("Cannot enable NI %d for Rx/Tx\n", ni);
		return err;
	}

	fsl_print("%s : Successfully configured ni%d (dpni.%d)\n",
		  AIOP_APP_NAME, ni, dpni_id);
	fsl_print("%s : dpni.%d <---connected---> %s.%d ",
		  AIOP_APP_NAME, dpni_id, dpni_ep_type, dpni_ep_id);
	fsl_print("(MAC addr: %02x:%02x:%02x:%02x:%02x:%02x)\n",
		  mac_addr[0], mac_addr[1], mac_addr[2],
		  mac_addr[3], mac_addr[4], mac_addr[5]);
	return 0;
}

/* Early initialization */
int app_early_init(void)
{
	return 0;
}

/* Initializes the application */
int app_init(void)
{
	int		err;

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t) app_hmanip, app_dpni_add_cb);
	if (err) {
		pr_err("Cannot register DPNI ADD event\n");
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_UP, 1,
			     (uint64_t) NULL, app_dpni_link_up_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK UP event\n");
		return err;
	}

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_DOWN, 1,
			     (uint64_t) NULL, app_dpni_link_down_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK DOWN event\n");
		return err;
	}

	return 0;
}

/* Frees application allocated resources */
void app_free(void)
{}

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg)
{
	/* Configure Initial Order Scope */
	memset(kg_cfg, 0x0, sizeof(struct dpkg_profile_cfg));

	kg_cfg->num_extracts = 5;
	/* IP_SRC */
	kg_cfg->extracts[0].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[0].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[0].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[0].extract.from_hdr.field = NET_HDR_FLD_IP_SRC;
	/* IP_DST: mask last byte */
	kg_cfg->extracts[1].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[1].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[1].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[1].extract.from_hdr.field = NET_HDR_FLD_IP_DST;
	kg_cfg->extracts[1].num_of_byte_masks = 1;
	kg_cfg->extracts[1].masks[0].offset = 3;
	/* PROTO */
	kg_cfg->extracts[2].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[2].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[2].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[2].extract.from_hdr.field = NET_HDR_FLD_IP_PROTO;
	/* L4 SRC PORT */
	kg_cfg->extracts[3].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[3].extract.from_hdr.prot = NET_PROT_TCP;
	kg_cfg->extracts[3].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[3].extract.from_hdr.field = NH_FLD_L4_PORT_SRC;
	/* L4 DST PORT */
	kg_cfg->extracts[4].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[4].extract.from_hdr.prot = NET_PROT_TCP;
	kg_cfg->extracts[4].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[4].extract.from_hdr.field = NH_FLD_L4_PORT_DST;
}

#ifdef PRINT_FRAME_INFO
static inline void print_frame_info(void)
{
	struct ipv4hdr  *outer_ip, *inner_ip;
	struct grehdr	*gre;
	uint16_t	outer_ihl, inner_ihl;
	uint8_t		*out_ip_src, *out_ip_dst, *in_ip_src, *in_ip_dst;

	outer_ip = (struct ipv4hdr *)((uint8_t *)PRC_GET_SEGMENT_ADDRESS() +
		(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
	/* Get IP source/destination and Internet Header Length (IHL) */
	out_ip_src = (void *)&outer_ip->src_addr;
	out_ip_dst = (void *)&outer_ip->dst_addr;
	outer_ihl = ((uint16_t)((*((uint8_t *)outer_ip) &
					IPV4_HDR_IHL_MASK) << 2));

	if (PARSER_IS_GRE_DEFAULT())
		gre = (struct grehdr *)((uint8_t *)PRC_GET_SEGMENT_ADDRESS() +
			(uint16_t)PARSER_GET_GRE_OFFSET_DEFAULT());

	if (PARSER_IS_TUNNELED_IP_DEFAULT()) {
		inner_ip = (struct ipv4hdr *)((uint8_t *)
			PRC_GET_SEGMENT_ADDRESS() +
			(uint16_t)PARSER_GET_INNER_IP_OFFSET_DEFAULT());
		/* Get IP source/destination and Internet Header Length (IHL) */
		in_ip_src = (void *)&inner_ip->src_addr;
		in_ip_dst = (void *)&inner_ip->dst_addr;
		inner_ihl = ((uint16_t)((*((uint8_t *)inner_ip) &
					IPV4_HDR_IHL_MASK) << 2));
	}

	cdma_mutex_lock_take(print_frame_mutex, CDMA_MUTEX_WRITE_LOCK);

	/* Print frame length and segment length */
	if (PARSER_IS_TUNNELED_IP_DEFAULT())
		fsl_print("Encapsulated Frame | Frame Len %d | SEG Len %d\n",
			LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS),
			PRC_GET_SEGMENT_LENGTH());
	else
		fsl_print("Decapsulated Frame | Frame Len %d | SEG Len %d\n",
			LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS),
			PRC_GET_SEGMENT_LENGTH());

	/* Print information from outer IP header */
	fsl_print("  OUTER IP HEADER\n");
	fsl_print("    PROTO: %d", outer_ip->protocol);
	fsl_print(" IP_SRC: %d.%d.%d.%d, IP_DST: %d.%d.%d.%d, ",
		out_ip_src[0], out_ip_src[1], out_ip_src[2], out_ip_src[3],
		out_ip_dst[0], out_ip_dst[1], out_ip_dst[2], out_ip_dst[3]);
	fsl_print("TOTAL LEN: %d, IHL: %d\n",
		outer_ip->total_length, outer_ihl);
	/* Print information from GRE header, if exists */
	if (PARSER_IS_GRE_DEFAULT()) {
		fsl_print("  Generic Routing Encapsulation\n");
		fsl_print("  FLAGS: 0x%05x, VERSION: 0x%03x, PTYPTE: 0x%04x\n",
			gre->flags, gre->version, gre->encap_protocol);
	}
	/* Print information from inner IP if exists */
	if (PARSER_IS_TUNNELED_IP_DEFAULT()) {
		fsl_print("  INNER IP HEADER\n");
		fsl_print("    PROTO: %d", inner_ip->protocol);
		fsl_print(" IP_SRC: %d.%d.%d.%d, IP_DST: %d.%d.%d.%d, ",
			in_ip_src[0], in_ip_src[1], in_ip_src[2], in_ip_src[3],
			in_ip_dst[0], in_ip_dst[1], in_ip_dst[2], in_ip_dst[3]);
		fsl_print("TOTAL LEN: %d, IHL: %d\n",
			inner_ip->total_length, inner_ihl);
	}

	fsl_print("\n");

	cdma_mutex_lock_release(print_frame_mutex);
}
#endif
