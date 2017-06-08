/* Copyright 2017 NXP
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


/* AIOP Classification headers */
#include "fsl_types.h"
#include "fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_parser.h"
#include "fsl_l2.h"
#include "fsl_osm.h"
#include "fsl_dbg.h"
#include "fsl_evmng.h"
#include "fsl_stdlib.h"
#include "qos_demo.h"

/* Enable PFC pause frame transmission */
/*#define ENABLE_PFC_PAUSE*/
/* Print link configuration */
/*#define PRINT_LINK_CFG */
/* Print QoS statistics on a 60 seconds timer expiration */
/*#define PRINT_QOS_STATISTICS*/
/* Print information about every received packet */
/*#define PRINT_RX_PKT_INFO*/

#ifdef PRINT_QOS_STATISTICS
	#include "fsl_malloc.h"
	#include "fsl_slab.h"
	#include "fsl_tman.h"

	static enum memory_partition_id		mem_partition_id;
	static uint16_t				num_timers;

	/* Committed buffers */
	#define COMMITTED_BUFFERS		1
	/* Maximum buffers */
	#define MAXIMUM_BUFFERS			1
	/* Buffers alignment */
	#define BUFFERS_ALIGNMENT		64
	/* Timer duration in seconds */
	#define TIMER_DURATION			60
#endif	/* PRINT_QOS_STATISTICS */

#define AIOP_APP_NAME			"QoS_demo"

/* L4 fields */
#define NH_FLD_L4_PORT_SRC		(1)
#define NH_FLD_L4_PORT_DST		(NH_FLD_L4_PORT_SRC << 1)

#define PRESENTATION_LENGTH		64

static void app_fill_kg_profile(struct dpkg_profile_cfg *kg_cfg);

#ifdef PRINT_LINK_CFG
static __COLD_CODE void print_link_cfg(uint16_t ni)
{
	struct dpni_drv_link_state	state;
	int				err;

	err = dpni_drv_get_link_state(ni, &state);
	if (err) {
		pr_err("dpni_drv_get_link_state failed\n");
		return;
	}
	fsl_print("\t Rate             : %d MHz\n", state.rate);
	fsl_print("\t Auto-negotiation : %s\n",
		  state.options & DPNI_DRV_LINK_OPT_AUTONEG ?
		  "Enabled" : "Disabled");
	fsl_print("\t Half-duplex mode : %s\n",
		  state.options & DPNI_DRV_LINK_OPT_HALF_DUPLEX ?
		  "Enabled" : "Disabled");
	fsl_print("\t Pause frame      : %s\n",
		  state.options & DPNI_DRV_LINK_OPT_PAUSE ?
		  "Enabled" : "Disabled");
	fsl_print("\t Asymmetric pause : %s\n",
		  state.options & DPNI_DRV_LINK_OPT_ASYM_PAUSE ?
		  "Enabled" : "Disabled");
	fsl_print("\t PFC pause        : %s\n",
		  state.options & DPNI_LINK_OPT_PFC_PAUSE ?
		  "Enabled" : "Disabled");
}
#else
	#define print_link_cfg(_a)
#endif	/* PRINT_LINK_CFG */

#ifdef PRINT_QOS_STATISTICS
/******************************************************************************/
static __COLD_CODE int allocate_statistics_timer(void)
{
	int	err;

	if (fsl_mem_exists(MEM_PART_DP_DDR))
		mem_partition_id = MEM_PART_DP_DDR;
	else
		mem_partition_id = MEM_PART_SYSTEM_DDR;
	/* The number of created timers should be 3 timers larger than actually
	 * needed and larger than 4. The memory region used to hold the timers
	 * must be of at least 64 * (num_of_timers + 1) bytes, and 64 bytes
	 * aligned. */
	/* Place for 5 (minimum possible) timers is reserved. */
	num_timers = 5;
	err = slab_register_context_buffer_requirements(COMMITTED_BUFFERS,
							MAXIMUM_BUFFERS,
							64 * (num_timers + 1),
							BUFFERS_ALIGNMENT,
							mem_partition_id, 0, 0);
	if (err)
		return err;
	return 0;
}

/******************************************************************************/
static __COLD_CODE void statistics_timer_cb(tman_arg_8B_t arg1,
					    tman_arg_2B_t arg2)
{
	uint8_t		tc;
	uint16_t	ni;
	uint32_t	tman_task_handle;
	int		err;
	uint64_t	val64;

	/* Confirm that timer callback finished execution */
	tman_task_handle = LW_SWAP(16, (uint32_t *)HWC_FD_ADDRESS);
	tman_timer_completion_confirmation(tman_task_handle);
	/* Get ni and tc from the parameters */
	ni = (uint16_t)arg1;
	tc = (uint8_t)arg2;
	fsl_print("Statistics on DPNI_%d, TC_%d\n", ni, tc);
	/********************/
	/* CEETM statistics */
	/********************/
	/* Dequeued bytes */
	err = dpni_drv_get_qos_counter(ni, tc,
				       DPNI_DRV_QOS_CNT_EGR_TC_DEQUEUE_BYTE,
				       &val64);
	if (err)
		pr_err("Get dequeued bytes\n");
	fsl_print("\t Dequeued bytes          = %ll\n", val64);
	/* Dequeued frames */
	err = dpni_drv_get_qos_counter(ni, tc,
				       DPNI_DRV_QOS_CNT_EGR_TC_DEQUEUE_FRAME,
				       &val64);
	if (err)
		pr_err("Get dequeued frames\n");
	fsl_print("\t Dequeued frames         = %ll\n", val64);
	/* Enqueue rejected bytes */
	err = dpni_drv_get_qos_counter(ni, tc,
				       DPNI_DRV_QOS_CNT_EGR_TC_REJECT_BYTE,
				       &val64);
	if (err)
		pr_err("Get enqueue rejected bytes\n");
	fsl_print("\t Enqueue rejected bytes  = %ll\n", val64);
	/* Enqueue rejected frames */
	err = dpni_drv_get_qos_counter(ni, tc,
				       DPNI_DRV_QOS_CNT_EGR_TC_REJECT_FRAME,
				       &val64);
	if (err)
		pr_err("Get enqueue rejected frames\n");
	fsl_print("\t Enqueue rejected frames = %ll\n", val64);
	fsl_print("\n");
}

/******************************************************************************/
static __COLD_CODE int create_statistics_timer(uint16_t ni, uint8_t tc)
{
	struct slab	*slab_handle;
	uint64_t	tmi_timer_addr;
	uint8_t		tmi_id;
	uint32_t	timer_handle;
	int		err;

	slab_handle = NULL;
	/* Allocate buffers for the keys, timers, etc */
	err = slab_create(COMMITTED_BUFFERS, MAXIMUM_BUFFERS,
			  64 * (num_timers + 1),
			  BUFFERS_ALIGNMENT, mem_partition_id, 0, NULL,
			  &slab_handle);
	if (err) {
		pr_err("slab_create failed\n");
		return err;
	}
	/* Create a TMI instance */
	/* Acquire the a 64 bytes aligned buffer */
	tmi_timer_addr = 0;
	err = slab_acquire(slab_handle, &tmi_timer_addr);
	if (err) {
		pr_err("slab_acquire failed\n");
		return err;
	}
	tmi_id = 0;
	err = tman_create_tmi(tmi_timer_addr, num_timers, &tmi_id);
	if (err) {
		pr_err("tman_create_tmi failed\n");
		return err;
	}
	/* Create statistics print periodic timer */
	timer_handle = 0;
	err = tman_create_timer(tmi_id,
				TMAN_CREATE_TIMER_MODE_TPRI |
				TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY |
				TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK,
				TIMER_DURATION, ni, tc,
				statistics_timer_cb, &timer_handle);
	if (err) {
		pr_err("tman_create_timer failed\n");
		return err;
	}
	return 0;
}
#else
	#define allocate_statistics_timer()		0
	#define create_statistics_timer(_a, _b)		0
#endif	/* PRINT_QOS_STATISTICS */

/* Performs frames processing */
/******************************************************************************/
__HOT_CODE ENTRY_POINT static void app_frame_cb(void)
{
	int		err;

	sl_prolog();
#ifdef PRINT_RX_PKT_INFO
	fsl_print("\n%s: RX on NI %d | CORE:%d | TC = %d | %s | %s\n",
		AIOP_APP_NAME,
		(uint16_t)PRC_GET_PARAMETER(), core_get_id(), task_get_tx_tc(),
		PARSER_IS_OUTER_IPV4_DEFAULT() ? "IPv4" :
		(PARSER_IS_OUTER_IPV6_DEFAULT() ? "IPv6" :
		(PARSER_IS_ARP_DEFAULT() ? "ARP" : "unknown")),
		PARSER_IS_TCP_DEFAULT() ||
		PARSER_IS_TCP_OPTIONS_DEFAULT() ||
		PARSER_IS_TCP_CONTROLS_6_11_SET_DEFAULT() ||
		PARSER_IS_TCP_CONTROLS_3_5_SET_DEFAULT() ? "TCP" :
		(PARSER_IS_UDP_DEFAULT() ? "UDP" :
		(PARSER_IS_ICMP_DEFAULT() ? "ICMP" : "unknown")));
#endif
	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_FLAGS);
	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();
	pr_err("Failed to send frame\n");
	fdma_terminate_task();
}

/******************************************************************************/
static int app_dpni_link_up_cb(uint8_t generator_id, uint8_t event_id,
			       uint64_t app_ctx, void *event_data)
{
	uint16_t			ni;

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	ni = (uint16_t)((uint32_t)event_data);
	fsl_print("NI_%d link is UP\n", ni);
	/* Enable PFC pause */
#ifdef ENABLE_PFC_PAUSE
	{
		struct dpni_drv_link_state	state;
		struct dpni_drv_link_cfg	cfg;
		int				err;

		err = dpni_drv_get_link_state(ni, &state);
		if (err) {
			pr_err("dpni_drv_get_link_state failed\n");
			return err;
		}
		cfg.rate = state.rate;
		cfg.options = state.options | DPNI_LINK_OPT_PFC_PAUSE;
		err = dpni_drv_set_link_cfg(ni, &cfg);
		if (err) {
			pr_err("dpni_drv_set_link_cfg failed\n");
			return err;
		}
		print_link_cfg(ni);
	}
#endif	/* ENABLE_PFC_PAUSE */
	return 0;
}

static int app_dpni_link_down_cb(uint8_t generator_id, uint8_t event_id,
				  uint64_t app_ctx, void *event_data)
{
	uint16_t	ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);

	fsl_print("NI_%d link is DOWN\n", ni);
	return 0;
}

extern uint8_t order_scope_buf[256];
static int app_dpni_add_cb(uint8_t generator_id, uint8_t event_id,
			   uint64_t app_ctx, void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;
	struct ep_init_presentation init_presentation;
	uint8_t			*tmp_buf = order_scope_buf;
	struct			dpni_drv_qos_tbl qos_cfg = {0};
	struct			dpni_drv_qos_rule qos_rule = {0};
	uint16_t		mfl = 0x2000; /* Maximum Frame Length */

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

	/* Configure frame processing callback */
	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("Cannot configure processing callback on NI %d\n", ni);
		return err;
	}

	/* Get DPNI ID for current Network Interface ID */
	err = dpni_drv_get_dpni_id(ni, &dpni_id);
	if (err) {
		pr_err("Cannot get DPNI ID for NI %d\n", ni);
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
	if (ni == 1) {
		/* QoS settings */
		memset(tmp_buf, 0, 256);
		err = dpni_drv_prepare_key_cfg(&kg_cfg, tmp_buf);
		if (err) {
			pr_err("Cannot prepare key_cfg\n", ni);
			return err;
		}

		qos_cfg.key_cfg_iova = (uint64_t)tmp_buf;
		qos_cfg.discard_on_miss = 0;
		qos_cfg.default_tc = 2;
		err = dpni_drv_set_qos_table(ni, &qos_cfg);
		if (err) {
			pr_err("Cannot set qos table on NI %d\n", ni);
			return err;
		}

		/* TCP QoS enty */
		qos_rule.key_iova = (uint64_t)tmp_buf;
		qos_rule.mask_iova = NULL;
		qos_rule.key_size = 1;
		tmp_buf[0] = 0x6;
		err = dpni_drv_add_qos_entry(ni, &qos_rule, 0, 0);
		if (err) {
			pr_err("Cannot add 1st qos entry on NI %d\n", ni);
			return err;
		}

		/* UDP QoS enty */
		tmp_buf[0] = 0x11;
		err = dpni_drv_add_qos_entry(ni, &qos_rule, 1, 1);
		if (err) {
			pr_err("Cannot add 2nd qos entry on NI %d\n", ni);
			return err;
		}
	}
	/* Egress QoS configuration */
	if (ni == 1) {
		uint8_t				tc;
		struct dpni_drv_tx_shaping	cr;
		struct dpni_drv_tx_shaping	er;

		/* QoS statistics */
		tc = task_get_tx_tc();
		/* Network interface and traffic class are passed to the created
		 * timer as arguments */
		err = create_statistics_timer(ni, tc);
		if (err)
			return err;
		/* Committed rate shaper */
		cr.rate_limit = 100;
		cr.max_burst_size = 0x1000;
		/* Excess rate shaper */
		er.rate_limit = 0;
		er.max_burst_size = 0;
		err = dpni_drv_set_tx_shaping(ni, &cr, &er, 0);
		if (err) {
			pr_err("Cannot set shapers on NI %d\n", ni);
			return err;
		}
	}
	err = dpni_drv_set_max_frame_length(ni, mfl);
	if (err) {
		pr_err("Cannot configure maximum frame length on NI %d\n", ni);
		return err;
	}

	err = dpni_drv_set_unicast_promisc(ni, TRUE);
	if (err) {
		pr_err("Cannot configure promiscuous mode on NI %d\n", ni);
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
	return allocate_statistics_timer();
}

/* Initializes the application */
int app_init(void)
{
	int err;

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
	/* PROTO */
	kg_cfg->extracts[0].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[0].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[0].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[0].extract.from_hdr.field = NET_HDR_FLD_IP_PROTO;
	/* IP_SRC */
	kg_cfg->extracts[1].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[1].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[1].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[1].extract.from_hdr.field = NET_HDR_FLD_IP_SRC;
	/* IP_DST */
	kg_cfg->extracts[2].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[2].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[2].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[2].extract.from_hdr.field = NET_HDR_FLD_IP_DST;
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
