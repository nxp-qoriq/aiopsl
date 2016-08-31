/*
 * Copyright 2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
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

/* AIOP Classification headers */
#include "fsl_table.h"
#include "fsl_keygen.h"
#include "statistics.h"
/* DPCI, DPNI headers */
#include "fsl_dpci_drv.h"
#include "fsl_dpci_mng.h"
#include "fsl_dprc.h"
#include "fsl_dpni_drv.h"
#include "fsl_l4.h"
/* Memory management headers */
#include "fsl_malloc.h"
#include "fsl_slab.h"
/* General purpose headers */
#include "common/fsl_stdio.h"
#include "fsl_dbg.h"
#include "fsl_string.h"
/* Platform headers */
#include "fsl_endian.h"
#include "fsl_fdma.h"
#include "fsl_sl_evmng.h"
#include "fsl_platform.h"
/* Required for statistics */
#include "fsl_ste.h"
/* Required for timer */
#include "fsl_tman.h"
#include "fsl_gen.h"
#include "fsl_icontext.h"

#define AIOP_APP_NAME		"STATISTICS"

/*
 * The application swaps source & destination addresses for L2 and IPv4
 * protocols, so it needs only 64 bytes from the frame.
 * The Segment Presentation Size (SPS) is set to 64
 * as smaller SPS can improve performance.
 */
#define PRESENTATION_LENGTH 64

#define STATS_MEM_SIZE			512
#define APP_SLAB_COMMITED_BUFFERS	10
#define	APP_SLAB_MAX_BUFFS		20
#define APP_SLAB_ALIGNMENT		16

/* IPV4: Protocol(1),  */
#define TABLE_KEY_LEN			1
#define TABLE_COMMIT_RULES		10
#define TABLE_MAX_RULES			20
#define CONN_TABLE_SIZE			3

/* L4 fields */
#define NH_FLD_L4_PORT_SRC	(1)
#define NH_FLD_L4_PORT_DST	(NH_FLD_L4_PORT_SRC << 1)

struct app_stats {
	/* Total number of packets received by AIOP */
	uint64_t	received_pkts __attribute__((aligned(16)));
	/* Total number of bytes received by AIOP */
	uint64_t	total_bytes_rx;
	/* Total number of packets accepted by CTLU */
	uint64_t	accepted_pkts __attribute__((aligned(16)));
	/* Total number of dropped packets due to CTLU miss */
	uint64_t	dropped_pkts __attribute__((aligned(16)));
	/* Total number of successfully transmitted packets */
	uint64_t	transmitted_pkts __attribute__((aligned(16)));
};

struct flow_stats {
	/* Total number of packets */
	uint64_t	num_pkts __attribute__((aligned(16)));
	/* Total number of bytes */
	uint64_t	num_bytes;
};

/* Number of app's timers. Must be bigger than 4 and smaller than (2^24)-1 */
#define APP_NUM_OF_TIMERS		10

/* Timer duration. Assume granularity in seconds */
#define APP_TMAN_TIMER_DURATION		60

/* Application statistics */
struct app_stats	stats;
struct slab		*slab;
uint64_t		stats_addr;

/* Application timers */
uint64_t		tmi_addr;
uint8_t			tmi_id;
uint32_t		stats_timer_handle;

/* Packet Classification table */
static uint16_t		table_id = 0xffff;
static uint8_t		key_id = 0xff;

/* Function to retrieve information about a classified frame */
static struct classif_cmd get_frame_info(void);
/* Create classification table for per-flow statistics */
static int create_flow_tables_rules(void);

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg);
static inline void l2_ip_src_dst_swap(void);

/* Classifier rule */
struct classif_connection {
	uint8_t		key[TABLE_KEY_LEN];
	uint64_t	slab_ptr_stats;
};

static struct classif_connection conn[CONN_TABLE_SIZE] = {
	/* PROTO */
	{{ 0x06 }, 0},
	{{ 0x11 }, 0},
	{{ 0x84 }, 0}
};


/* Frames processing callback */
__HOT_CODE ENTRY_POINT static void app_frame_cb(void)
{
	struct table_lookup_result lk_result __attribute__((aligned(16)));
	int err;

	sl_prolog();

	if (!PARSER_IS_OUTER_IPV4_DEFAULT()) {
		/* Discard non IPV4 frame and terminate task */
		goto drop_frame;
	}

	/* This STE function increments two counters by single accelerator call:
	 *	 * first counter is incremented by 1
	 *	 * second counter is incremented by the value
	 *	 LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS)
	 * Address of first counter should be 16 bytes aligned in this case */
	ste_inc_and_acc_counters(stats_addr +
			offsetof(struct app_stats, received_pkts),
			LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS),
			(STE_MODE_COMPOUND_64_BIT_CNTR_SIZE |
				STE_MODE_COMPOUND_64_BIT_ACC_SIZE));

	/* Perform a lookup with the predefined key */
	if (table_lookup_by_keyid_default_frame(TABLE_ACCEL_ID_CTLU, table_id,
		key_id, &lk_result) == TABLE_STATUS_MISS) {
		ste_inc_counter(stats_addr +
				offsetof(struct app_stats, dropped_pkts),
				1, STE_MODE_64_BIT_CNTR_SIZE);
		goto drop_frame;
	}

	ste_inc_counter(stats_addr +
			offsetof(struct app_stats, accepted_pkts),
			1, STE_MODE_64_BIT_CNTR_SIZE);

	/* Increase per-flow statistics: num of packets + num of bytes */
	ste_inc_and_acc_counters(GET_USER_DATA(lk_result) +
			offsetof(struct flow_stats, num_pkts),
			LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS),
			(STE_MODE_COMPOUND_64_BIT_CNTR_SIZE |
				STE_MODE_COMPOUND_64_BIT_ACC_SIZE));

	/* Swap source & destination addresses for L2 and IP protocols */
	l2_ip_src_dst_swap();

	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);
	if (!err) {
		ste_inc_counter(stats_addr +
				offsetof(struct app_stats, transmitted_pkts),
				1, STE_MODE_64_BIT_CNTR_SIZE);
		fdma_terminate_task();
	}

	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();

	pr_err("Cannot send frame\n");
	fdma_terminate_task();

drop_frame:
	fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	fdma_terminate_task();
}


static inline void l2_ip_src_dst_swap(void)
{
	struct parse_result *pr = (struct parse_result *)HWC_PARSE_RES_ADDRESS;

	uint8_t  *ethhdr = PARSER_GET_ETH_POINTER_DEFAULT();
	uint8_t  dst_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	struct   ipv4hdr *ipv4_hdr;
	uint32_t ip_src_addr;
	uint8_t	 *eth_src, *eth_dst;

	/* get ETH source and destination addresses */
	eth_dst = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT());
	eth_src = (uint8_t *)((uint32_t)PARSER_GET_ETH_POINTER_DEFAULT() +
					NET_HDR_FLD_ETH_ADDR_SIZE);

	/* store MAC_DST */
	*((uint32_t *)&dst_addr[0]) = *((uint32_t *)eth_dst);
	*((uint16_t *)&dst_addr[4]) = *((uint16_t *)(eth_dst + 4));

	/* set ETH destination address */
	*((uint32_t *)(&ethhdr[0])) = *((uint32_t *)(eth_src));
	*((uint16_t *)(&ethhdr[4])) = *((uint16_t *)(eth_src + 4));

	/* set ETH source address */
	*((uint32_t *)(&ethhdr[6])) = *((uint32_t *)&dst_addr[0]);
	*((uint16_t *)(&ethhdr[10])) = *((uint16_t *)&dst_addr[4]);

	/* get IPv4 header */
	ipv4_hdr = (struct ipv4hdr *)((uint8_t *)
			PRC_GET_SEGMENT_ADDRESS() +
			(uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT());

	/* store IP source address before changing it */
	ip_src_addr = ipv4_hdr->src_addr;
	/* swap IP source & destination addresses */
	ipv4_hdr->src_addr = ipv4_hdr->dst_addr;
	ipv4_hdr->dst_addr = ip_src_addr;

	/* we do not need to update nor the IP, nor the L4 checksum, because
	 * the IP source & destination addresses were swapped and not replaced
	 * with other values */

	/* Mark running sum as invalid */
	pr->gross_running_sum = 0;

	/* Modify the data in the default Data Segment */
	fdma_modify_default_segment_full_data();

}

/* Early initialization */
int app_early_init(void)
{
	int err;

	err = slab_register_context_buffer_requirements(
					APP_SLAB_COMMITED_BUFFERS,
					APP_SLAB_MAX_BUFFS,
					sizeof(struct flow_stats),
					APP_SLAB_ALIGNMENT,
					APP_MEM_PARTITION,
					0, /* flags */
					1 /* ddr backup pool */);

	if (err) {
		pr_err("Failed to register buffer requirements for stats\n");
		return -1;
	}

	fsl_print("%s : Successfully registered SLAB buffer requirements\n",
		AIOP_APP_NAME);
	return 0;
}

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

static int app_dpni_add_cb(uint8_t generator_id, uint8_t event_id,
			   uint64_t app_ctx, void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;
	struct ep_init_presentation init_presentation;

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

	/* Get DPNI ID for current Network Interface ID*/
	err = dpni_drv_get_dpni_id(ni, &dpni_id);
	if (err) {
		pr_err("Cannot get DPNI ID for NI %d\n", ni);
		return err;
	}

	/* Configure frame processing callback */
	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("Cannot configure callback on NI %d\n", ni);
		return err;
	}

	/* Default ordering mode should be Concurrent */
	err = dpni_drv_set_concurrent(ni);
	if (err) {
		pr_err("Cannot set Concurrent exec on NI %d\n", ni);
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

	/* Set the initial segment presentation size */
	err = dpni_drv_get_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot get initial presentation for NI %d\n", ni);
		return err;
	}

	init_presentation.options = EP_INIT_PRESENTATION_OPT_SPS;
	init_presentation.sps = PRESENTATION_LENGTH;
	err = dpni_drv_set_initial_presentation(ni, &init_presentation);
	if (err) {
		pr_err("Cannot set initial presentation for NI %d to %d\n",
				ni, init_presentation.sps);
		return err;
	}

	/* Enable NI to receive frames */
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

static int app_dpni_link_up_cb(uint8_t generator_id, uint8_t event_id,
				uint64_t app_ctx, void *event_data)
{
	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("%s : ni%d link is UP\n",
		  AIOP_APP_NAME, (uint16_t)((uint32_t)event_data));

	return 0;
}

static int app_dpni_link_down_cb(uint8_t generator_id, uint8_t event_id,
				  uint64_t app_ctx, void *event_data)
{
	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("%s : ni%d link is DOWN\n",
		  AIOP_APP_NAME, (uint16_t)((uint32_t)event_data));

	return 0;
}

static void app_show_stats_cb(tman_arg_8B_t arg1, tman_arg_2B_t arg2)
{
	UNUSED(arg1);
	UNUSED(arg2);

	struct icontext	ic;
	uint64_t	l_stats_addr = stats_addr;
	uint64_t	ws_buf;

	/* Flush stats in request queue, so that user gets most recent
	 * data. STE counter is updated in a fire and forget manner
	 * and CDMA and STE modules are not synchronized */
	ste_barrier();

	/* Get isolation context ID for fetching data from system memory using
	 * Frame DMA */
	icontext_aiop_get(&ic);

	/* Read stats from memory using CDMA, which fetches a snapshot
	 * of the counters currently residing in memory */
	fdma_dma_data(sizeof(stats.received_pkts), ic.icid, &ws_buf,
		l_stats_addr + offsetof(struct app_stats, received_pkts),
		FDMA_DMA_DA_SYS_TO_WS_BIT);
	stats.received_pkts = ws_buf;

	fdma_dma_data(sizeof(stats.dropped_pkts), ic.icid, &ws_buf,
		l_stats_addr + offsetof(struct app_stats, dropped_pkts),
		FDMA_DMA_DA_SYS_TO_WS_BIT);
	stats.dropped_pkts = ws_buf;

	fdma_dma_data(sizeof(stats.transmitted_pkts), ic.icid, &ws_buf,
		l_stats_addr + offsetof(struct app_stats, transmitted_pkts),
		FDMA_DMA_DA_SYS_TO_WS_BIT);
	stats.transmitted_pkts = ws_buf;

	fdma_dma_data(sizeof(stats.accepted_pkts), ic.icid, &ws_buf,
		l_stats_addr + offsetof(struct app_stats, accepted_pkts),
		FDMA_DMA_DA_SYS_TO_WS_BIT);
	stats.accepted_pkts = ws_buf;

	fdma_dma_data(sizeof(stats.total_bytes_rx), ic.icid, &ws_buf,
		l_stats_addr + offsetof(struct app_stats, total_bytes_rx),
		FDMA_DMA_DA_SYS_TO_WS_BIT);
	stats.total_bytes_rx = ws_buf;

	fsl_print("AIOP received %ll packets (%ll bytes)\n",
			stats.received_pkts, stats.total_bytes_rx);
	fsl_print("\t%ll dropped packets, ", stats.dropped_pkts);
	fsl_print("%ll accepted packets, %ll transmitted packets\n",
			stats.accepted_pkts, stats.transmitted_pkts);

	for (uint8_t i = 0; i < CONN_TABLE_SIZE; i++) {
		fsl_print("\t * PROTO 0x%x: ", conn[i].key[0]);

		fdma_dma_data(sizeof(uint64_t), ic.icid, &ws_buf,
			conn[i].slab_ptr_stats +
				offsetof(struct flow_stats, num_pkts),
			FDMA_DMA_DA_SYS_TO_WS_BIT);
		fsl_print("%ll packets ", ws_buf);

		fdma_dma_data(sizeof(uint64_t), ic.icid, &ws_buf,
			conn[i].slab_ptr_stats +
				offsetof(struct flow_stats, num_bytes),
			FDMA_DMA_DA_SYS_TO_WS_BIT);
		fsl_print("(%ll bytes)\n", ws_buf);
	}
	fsl_print("\n");

	/* Confirm that timer callback finished execution */
	tman_timer_completion_confirmation(stats_timer_handle);
}

static void delete_tmi_cb(tman_arg_8B_t arg1, tman_arg_2B_t del_tmi_id)
{
	UNUSED(arg1);
	pr_info("TMAN Instance with ID %d deleted successfully\n", del_tmi_id);
}


static void slab_release_cb(uint64_t param)
{
	UNUSED(param);
	pr_info("SLAB release was called by application\n");
}

/* Initializes the application */
int app_init(void)
{
	uint64_t		l_stats_addr;
	int			err = 0;

	/* Register DPNI events */
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			(uint64_t)app_frame_cb, app_dpni_add_cb);
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

	/* Get contiguous block from DP-DDR memory for timers */
	err = fsl_get_mem(64 * (APP_NUM_OF_TIMERS + 1), APP_MEM_PARTITION,
			  64, &tmi_addr);
	if (err) {
		pr_err("Cannot get memory for timers: err=%d\n", err);
		return err;
	}
	/* Create TMI (Timer Instance) */
	err = tman_create_tmi(tmi_addr,
				APP_NUM_OF_TIMERS, &tmi_id);
	if (err) {
		pr_err("Cannot create TMI: err=%d\n", err);
		return err;
	}
	fsl_print("%s : Created TMI id=0x%x\n", AIOP_APP_NAME, tmi_id);
	/* Create periodic timer for showing AIOP statistics
	 * A task with low priority will be created, which performs CDMA
	 * read from statistics memory after flushing the statistics
	 * engine and displays the results
	 * */
	err = tman_create_timer(tmi_id, TMAN_CREATE_TIMER_MODE_TPRI |
				TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY |
				TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK,
				APP_TMAN_TIMER_DURATION, 0, 0,
				app_show_stats_cb, &stats_timer_handle);
	if (err) {
		pr_err("Cannot create timer for showing statistics\n");
		return err;
	}
	fsl_print("%s : Created timer for showing statistics, handle=0x%x\n",
		  AIOP_APP_NAME, stats_timer_handle);

	/* Create buffers for per flow-statistics using pool-based allocation
	 *
	 * At runtime, AIOPSL guarantees the availability of committed number of
	 * buffers. Buffers beyond the committed number may be shared
	 * with other pools and therefore availability can not be guaranteed.
	 * */	 
	err = slab_create(APP_SLAB_COMMITED_BUFFERS,
			  APP_SLAB_MAX_BUFFS,
			  sizeof(struct flow_stats),	/* buff size in bytes */
			  APP_SLAB_ALIGNMENT,
			  APP_MEM_PARTITION,
			  0,				/* flags */
			  slab_release_cb,
			  &slab);
	if (err) {
		pr_err("Cannot create new buffer pools using SLAB\n");
		return err;
	}

	/* Get contiguous block from DP-DDR memory for global statistics */
	err = fsl_get_mem(sizeof(struct app_stats), APP_MEM_PARTITION,
			16, &l_stats_addr);
	if (err) {
		pr_err("Cannot get memory for global stats, err=%d\n", err);
		return err;
	}
	/* Initialize global statistics counters */
	memset(&stats, 0, sizeof(stats));
	cdma_write(l_stats_addr, &stats, sizeof(stats));
	stats_addr = l_stats_addr;

	/* Initialize table classification for per-flow statistics */
	err = create_flow_tables_rules();
	if (err) {
		pr_err("Cannot create exact match table for per-flow stats\n");
		return err;
	}
	
#ifdef SLAB_DEBUG
	struct slab_debug_info			slab_info;
	struct bman_debug_info			bman_info;

	/* Print SLAB debug info, such as number of buffers or buffer size */
	err = slab_debug_info_get(slab, &slab_info);
	if (err) {
		pr_err("Cannot get SLAB debug info\n");
		return err;
	}

	fsl_print("Slab pool info: Slab is from BPID=%d\n"
			"\tallocated_buffs=%d\n"
			"\tfailed_allocs=%d\n"
			"\tfree_buffers=%d\n"
			"\tcommited_buffs=%d\n"
			"\tmax_buffs=%d\n"
			"\talignment=%d\n"
			"\tmem_pid=%d\n",
	slab_info.pool_id,
	slab_info.allocated_buffs,
	slab_info.num_failed_allocs,
	slab_info.num_buff_free,
	slab_info.committed_buffs,
	slab_info.max_buffs, 
	slab_info.alignment,
	slab_info.mem_pid);

	slab_bman_debug_info_get(slab_info.pool_id, &bman_info);
	fsl_print("Buffer Pool INFO: BPID=%d\n"
			"\tallocated_buffs=%d\n"
			"\tfailed_allocs=%d\n"
			"\tfree_buffers=%d\n"
			"\tmax_buffs=%d\n", 

	bman_info.bpid,
	bman_info.num_buffs_alloc,
	bman_info.num_failed_allocs,
	bman_info.num_buffs_free,
	bman_info.total_num_buffs);

	fsl_print("\tbuff_size=%d\n \talignment=%d\n \tmem_pid=%d\n",
		bman_info.size, bman_info.alignment, bman_info.mem_pid);
#endif

	fsl_print("%s : Application initialized successfully\n", AIOP_APP_NAME);

	return 0;
}

static int create_flow_tables_rules(void)
{
	struct kcr_builder		kb	__attribute__((aligned(16)));
	struct table_rule		rule	__attribute__((aligned(16)));
	struct table_create_params	tbl_params;
	struct flow_stats		f_stats;
	uint64_t			slab_buff;
	uint64_t			tmp_rule_id;
	uint16_t			l_table_id;
	uint8_t				l_key_id;
	int				ret;

	/* Create key composition rule */
	memset(&kb, 0, sizeof(struct kcr_builder));
	keygen_kcr_builder_init(&kb);
	/* PTYPE */
	ret = keygen_kcr_builder_add_protocol_specific_field(
			KEYGEN_KCR_PTYPE_1_FECID, NULL, &kb);
	if (ret) {
		pr_err("Cannot add PTYPE to key rule: ret = %d\n", ret);
		return ret;
	}
	/* Generate key */
	ret = keygen_kcr_create(KEYGEN_ACCEL_ID_CTLU,
				(uint8_t *)&kb.kcr, &l_key_id);
	if (ret) {
		pr_err("Cannot create exact match key composition rule\n");
		return ret;
	}

	/* Create exact match table */
	memset(&tbl_params, 0, sizeof(struct table_create_params));
	tbl_params.attributes = APP_TABLE_PARAMS_ATTRIBUTES;
	tbl_params.key_size = TABLE_KEY_LEN;
	tbl_params.committed_rules = TABLE_COMMIT_RULES;
	tbl_params.max_rules = TABLE_MAX_RULES;
	ret = table_create(TABLE_ACCEL_ID_CTLU, &tbl_params, &l_table_id);
	if (ret) {
		keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU, l_key_id);
		pr_err("Cannot create exact match table : ret = %d\n", ret);
		return ret;
	}

	/* Create rules for exact match table */
	for (uint8_t i = 0; i < CONN_TABLE_SIZE; i++) {
		memset(&rule, 0, sizeof(struct table_rule));
		memcpy(&rule.key_desc.em.key, (const void *)&conn[i].key,
			TABLE_KEY_LEN);

		/* Prepare per-flow stats buffer. Initialize statistics to 0 */
		slab_acquire(slab, &slab_buff);
		memset(&f_stats, 0, sizeof(f_stats));
		cdma_write(slab_buff, &f_stats, sizeof(f_stats));
		
		rule.result.type = ARCH_TABLE_RESULT_TYPE;
		SET_USER_DATA(rule, slab_buff);

		conn[i].slab_ptr_stats = slab_buff;
		rule.options = TABLE_RULE_TIMESTAMP_NONE;
		
		ARCH_TABLE_RULE_CREATE(l_table_id, rule, 
					TABLE_KEY_LEN, tmp_rule_id);

		if (ret != TABLE_STATUS_SUCCESS) {
			pr_err("Cannot add rule to table (table_id=%d)\n",
				l_table_id);
			goto rule_create_err;
		}
	}

	key_id = l_key_id;
	table_id = l_table_id;

	return 0;

rule_create_err:
	keygen_kcr_delete(KEYGEN_ACCEL_ID_CTLU, l_key_id);
	table_delete(TABLE_ACCEL_ID_CTLU, l_table_id);

	return ret;
}


/* Frees application allocated resources */
void app_free(void)
{
	int		err;

	/* Flush all pending statistics operations */
	ste_barrier();

	/* Free statistics counter memory */
	fsl_put_mem(stats_addr);

	/* Free flow counters memory */
	for (uint8_t i = 0; i < CONN_TABLE_SIZE; i++)
		slab_release(slab, conn[i].slab_ptr_stats);

	/* Delete timer and TMAN Instance */
	err = tman_delete_timer(stats_timer_handle,
				TMAN_TIMER_DELETE_MODE_WO_EXPIRATION);
	if (err)
		pr_err("Cannot delete timer with handle %d\n",
			stats_timer_handle);
	tman_delete_tmi(delete_tmi_cb, TMAN_INS_DELETE_MODE_FORCE_EXP,
			      tmi_id, 0, tmi_id);

	/* Free timer memory */
	fsl_put_mem(tmi_addr);

	/* Free SLAB */
	slab_free(&slab);
}
