/* Copyright 2018 NXP
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
#include "fsl_aiop_common.h"
#include "fsl_io.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_parser.h"
#include "fsl_l2.h"
#include "fsl_osm.h"
#include "fsl_dbg.h"
#include "fsl_evmng.h"
#include "fsl_stdlib.h"
#include "fsl_spinlock.h"
#include "fsl_ep_mng.h"
#include "pfc_demo.h"
#include "fsl_frame_operations.h"
#include "fsl_malloc.h"
#include "fsl_slab.h"

static enum memory_partition_id		mem_partition_id;

/* Enable PFC pause frame transmission */
/*#define ENABLE_PFC_PAUSE*/

/* Print link configuration */
#define PRINT_LINK_CFG

#define AIOP_APP_NAME			"PFC_demo"

/* L4 fields */
#define NH_FLD_L4_PORT_SRC		(1)
#define NH_FLD_L4_PORT_DST		(NH_FLD_L4_PORT_SRC << 1)

#define PRESENTATION_LENGTH		64

/* Define CSCN_CONGESTION_TEST to perform the CSCN congestion test */
/* #define CSCN_CONGESTION_TEST */
/* Define BPSCN_DEPLETION_TEST to perform the BPSCN congestion test */
/* #define BPSCN_DEPLETION_TEST */
#if (defined(CSCN_CONGESTION_TEST) && defined(BPSCN_DEPLETION_TEST))
	#error "None or only one macro should be defined"
#endif

#if (defined(CSCN_CONGESTION_TEST) || defined(BPSCN_DEPLETION_TEST))
	uint64_t		print_lock __attribute__ ((aligned(8)));
	#define LOCK_PRINT()	lock_spinlock(&print_lock)
	#define UNLOCK_PRINT()	unlock_spinlock(&print_lock)

	uint32_t	first_task_id;
	int64_t		num_tasks __attribute__ ((aligned(8)));
	int64_t		term_tasks __attribute__ ((aligned(8)));
	uint64_t	is_congested;
	uint64_t	is_depleted;
	/* Congestion/Depletion is traced on this DPNI */
	uint32_t	dpni_fqid;

	/* Size of State Change Notification information (Do not change it) */
	#define SCN_SIZE			64
	/* SCN information alignment (Do not change it) */
	#define SCN_ALIGN			16
	/* State offset in IOVA */
	#define SCN_STATE_OFFSET		0x02

#endif	/* (defined(CSCN_CONGESTION_TEST) || defined(BPSCN_DEPLETION_TEST)) */

#ifdef CSCN_CONGESTION_TEST
	/* To get CSCN notifications in AIOP define CSCN_NOTIFY as
	 *	DPNI_CONG_OPT_NOTIFY_AIOP.
	 * To send CSCN notifications to DPNIs CSCN_NOTIFY as
	 *	DPNI_CONG_OPT_NOTIFY_WRIOP.
	/* To get CSCN notifications in AIOP and send them to DPNIs define
	 * CSCN_NOTIFY as
	 *	DPNI_CONG_OPT_NOTIFY_AIOP | DPNI_CONG_OPT_NOTIFY_WRIOP.
	 * For testing purposes set it to 0. In this case CSCN notifications
	 * should not occur. */
	#define CSCN_NOTIFY	DPNI_CONG_OPT_NOTIFY_AIOP
	/* Congestion is traced on this DPNI */
	uint64_t	dpni_cscn_iova;
	/* SYS DDR allocated pool of buffers */
	struct slab	*cscn_slab_handle;

	/* Above this threshold : enter the congestion state */
	#define	CSCN_ENTRY_THRES	6
	/* Below this threshold : exit the congestion state */
	#define	CSCN_EXIT_THRES		4

	struct congestion_info {
		/* Network interface */
		uint16_t	ni;
		/* Pointer to a SYS DDR buffer receiving the CSCN information */
		uint64_t	cscn_iova;
	};
	/* Number of DPNIs supporting congestion notifications */
	#define MAX_DPNI_ID	2
	struct congestion_info	cscn_info[MAX_DPNI_ID];
	/* Size of CSCN information (Do not change it) */
	#define CSCN_SIZE	SCN_SIZE
	/* CSCN information alignment (Do not change it) */
	#define CSCN_ALIGN	SCN_ALIGN

#endif	/* CSCN_CONGESTION_TEST */

#ifdef BPSCN_DEPLETION_TEST
	/* To get BPSCN notifications in AIOP define BPSCN_NOTIFY as
	 *	DPNI_DRV_DEPL_NOTIF_OPT_AIOP.
	 * To send BPSCN notifications to DPNIs BPSCN_NOTIFY as
	 *	DPNI_DRV_DEPL_NOTIF_OPT_WRIOP.
	/* To get BPSCN notifications in AIOP and send them to DPNIs define
	 * BPSCN_NOTIFY as
	 *	DPNI_DRV_DEPL_NOTIF_OPT_AIOP | DPNI_DRV_DEPL_NOTIF_OPT_WRIOP.
	 * For testing purposes set it to 0. In this case BPSCN notifications
	 * should not occur. */
	#define BPSCN_NOTIFY	DPNI_DRV_DEPL_NOTIF_OPT_AIOP
	/* Number of buffers in a pool */
	#define NUM_BUFFERS	8
	/* SYS DDR allocated pool of buffers */
	struct slab	*bpscn_slab_handle;
	/* Below this threshold the pool is depleted */
	#define	BPSCN_ENTRY_THRES	4
	/* Greater than or equal to this threshold the pool exits the
	 * depleted state */
	#define	BPSCN_EXIT_THRES	6
	struct bp_notification_info {
		/* 1 if it is the backup pool */
		uint8_t		backup_pool;
		/* Pointer to a SYS DDR buffer receiving the BPSCN
		 * information */
		uint64_t	bpscn_iova;
		/* Number of buffers in pool */
		uint32_t	num_bufs;
		/* Pool ID */
		uint16_t	bpid;
	};
	/* Number of DPNI buffers pools */
	#define DPNI_MAX_POOLS		2
	struct bp_notification_info	bpscn_info[DPNI_MAX_POOLS];
#endif	/* BPSCN_DEPLETION_TEST */

#define AIOP_ETH_MAC_CTRL_LEN	64

/******************************************************************************/
static void send_flow_ctrl_pkt(void)
{
	int err = 0;

	uint8_t	eth_pfc_pause_pkt[AIOP_ETH_MAC_CTRL_LEN] = {
			0x01, 0x80, 0xc2, 0x00, 0x00, 0x01, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x07, 0x88, 0x08, 0x01, 0x01,
			0x00, 0xff, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03,
			0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00
	};

	uint8_t	frame_handle = 0;
	uint16_t	spid = 0;

	fsl_print("Sending PFC PKT\n");

	dpni_drv_get_spid(task_get_receive_niid(), &spid);

	*((uint8_t *)HWC_SPID_ADDRESS) = (uint8_t)spid;

	err = create_frame((struct ldpaa_fd *)HWC_FD_ADDRESS,
			   eth_pfc_pause_pkt, AIOP_ETH_MAC_CTRL_LEN + 4,
			   (uint8_t)spid, &frame_handle);
	if (err)
		fsl_print("ERROR = %d: create_frame()\n", err);

	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_FLAGS);

	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();

	pr_err("Failed to send frame\n");
}

/******************************************************************************/
static void app_fill_fs_kg_profile(struct dpkg_profile_cfg *kg_cfg)
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

#if (defined(CSCN_CONGESTION_TEST) || defined(BPSCN_DEPLETION_TEST))

/******************************************************************************/
static uint32_t get_task(void)
{
	register uint32_t	task;

	asm { mfdcr  task, dcr476;	/* TASKCSR0 */ }
	/*  AIOP z490 CPU Specification rev1.8.pdf
	 * Bits 24:31 - TASKID : This field is provided for software use to
	 * determine the active Task ID. This value is a global value for the
	 * AIOP Tile, thus the most significant four bits are determined by
	 * core cluster ID (bits 24:25) and core within cluster ID (bits 26:27).
	 * The local task number is indicated in bits 28:31.
	 * This field is read-only. */
	return task;
}

/******************************************************************************/
static __COLD_CODE int scn_iova_slab_allocate(uint32_t num_buffers)
{
	int	err;

	err = slab_register_context_buffer_requirements(num_buffers,
							num_buffers,
							SCN_SIZE, SCN_ALIGN,
							MEM_PART_SYSTEM_DDR,
							0, 0);
	if (err)
		return err;
	fsl_print("\t >>> SCN IOVAs pool : %d Allocated\n", num_buffers);
	return 0;
}

/******************************************************************************/
static __COLD_CODE int scn_iova_slab_create(uint32_t num, struct slab **handle)
{
	int		err;

	/* Allocate buffers for DPNI pools BPSCN iova */
	err = slab_create(num, num, SCN_SIZE, SCN_ALIGN, MEM_PART_SYSTEM_DDR,
			  0, NULL, handle);
	if (err) {
		pr_err("SCN slab_create failed\n");
		return err;
	}
	fsl_print("\t >>> SCN IOVAs pool : Created %d\n", num);
	return 0;
}

/******************************************************************************/
static __COLD_CODE int scn_iova_get(struct slab	*handle, uint64_t *scn_iova)
{
	int		err;

	*scn_iova = 0;
	err = slab_acquire(handle, scn_iova);
	if (err) {
		pr_err("SCN_IOVA slab_acquire failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
static void scn_iova_init(uint64_t iova)
{
	uint8_t		ws_iova[SCN_SIZE];
	int		i;

	for (i = 0; i < SCN_SIZE; i++)
		ws_iova[i] = 0;
	cdma_write(iova, &ws_iova[0], SCN_SIZE);
}

#endif /* (defined(CSCN_CONGESTION_TEST) || defined(BPSCN_DEPLETION_TEST)) */

#ifdef PRINT_LINK_CFG
/******************************************************************************/
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

#ifdef BPSCN_DEPLETION_TEST
/******************************************************************************/
static inline void bpscn_iova_print(uint16_t bpid)
{
	uint64_t	iova;
	uint8_t		ws_iova[16];
	int		i;

	/* Find BPID related iova */
	for (i = 0; i < DPNI_MAX_POOLS; i++)
		if (bpscn_info[i].bpid == bpid) {
			iova = bpscn_info[i].bpscn_iova;
			break;
		}
	/* Not found */
	if (i == DPNI_MAX_POOLS)
		return;
	cdma_read_with_no_cache(&ws_iova[0], iova, 16);
	dbg_print("\t ");
	/* Only the first 16 bytes are relevant */
	for (i = 0; i < 16; i++, iova++)
		dbg_print("%02x ", ws_iova[i]);
	dbg_print("\n");
}

/******************************************************************************/
static inline uint8_t get_bpscn_state_from_iova(uint16_t bpid)
{
	int		i;
	uint8_t		ws_iova[SCN_SIZE], state;
	uint16_t	val16;
	uint64_t	iova, val64;

	/* Find BPID related iova */
	for (i = 0; i < DPNI_MAX_POOLS; i++)
		if (bpscn_info[i].bpid == bpid) {
			iova = bpscn_info[i].bpscn_iova;
			break;
		}
	/* Not found */
	if (i == DPNI_MAX_POOLS)
		return 0;
	/* State Change Notification Message Format is in the LE byte order
	 * in chunks of 16 bytes :
	 * 15-8	: CTX/ICNT
	 * 7	: TOK
	 * 6-4	: RID
	 * 3	: reserved
	 * 2	: STATE
	 *	Returns the current state of the buffer pool
	 *		Bit 0: Buffer Availability State:
	 *			0 = Free buffers are available,
	 *			1 = No free buffers available
	 *		Bit 1: Buffer Depletion State:
	 *			0 = Buffer pool is not depleted,
	 *			1 = Buffer pool is depleted
	 *		Bit 2: Buffer Surplus State (Not available on AIOP DCP):
	 *			0 = Buffer pool is not in surplus,
	 *			1 = Buffer pool is in surplus
	 *		Bit 7-3 (msbs): Reserved
	 * 1	: STAT
	 * 0	: VERB
	 *		29h: BPSCN (Buffer Pool State Change Notification)
	 * 31-16 : reserved
	 * 47-32 : reserved
	 * 63-48 : reserved
	 */
	cdma_read_with_no_cache(ws_iova, iova, SCN_SIZE);
	state = BPSCN_GET_DEPLETION_STATE(&ws_iova[0]);
	val16 = BPSCN_GET_BPID(&ws_iova[0]);
	val64 = BPSCN_GET_MESSAGE_CTX(&ws_iova[0]);
	/*fsl_print("\t >>> State = %d BPID = %d Ctx = 0x%x-%x\n",
		  state, val16, (uint32_t)(val64 >> 32),
		  (uint32_t)val64);*/
	return state;
}

/******************************************************************************/
__HOT_CODE void bpscn_callback(void)
{
	uint16_t	depleted, bpid;

	/* Hardware Depletion detected. First, the depleted state is reported.
	 * Here the application should wait for the "depletion exit" state by
	 * polling the depletion status in the BP related SCN iova buffer. */
	osm_task_init();
	depleted = LDPAA_FD_GET_SCN_STATE(HWC_FD_ADDRESS);
	bpid = LDPAA_FD_GET_SCN_BPID(HWC_FD_ADDRESS);
	dbg_print("APP - BPSCN notification received : STATE = %d BPID = %d\n",
		  depleted, bpid);
	bpscn_iova_print(bpid);
	/* Here one must perform the needed actions (applications dependent) to
	 * determine the exit from the "depleted" state.
	 *
	 * In this application, some processing is performed and then the
	 * is_depleted variable is set.
	 * The packets processing task (app_bpscn_depletion_process_cb) will
	 * send the received packets over a DPNI and the buffers are released in
	 * the BP. The "depletion exit" status should be detected by reading the
	 * IOVA information corresponding to the depleted buffer pool. */
	if (depleted) {
		is_depleted = 1;
		term_tasks = 0;
	}
	while (depleted && is_depleted) {
		/* Let other tasks to be scheduled on this core */
		__e_hwacceli(YIELD_ACCEL_ID);
		dbg_print("");
		/* End of "depleted" state processing */
		depleted = get_bpscn_state_from_iova(bpid);
		if (!depleted)
			bpscn_iova_print(bpid);
	}
	fdma_terminate_task();
}

/******************************************************************************/
static __HOT_CODE ENTRY_POINT void app_bpscn_depletion_process_cb(void)
{
	int			err;
	uint32_t		id;

	sl_prolog();
	id = get_task();
	atomic_incr64(&num_tasks, 1);
	/* Tasks are not terminated till the depleted state isn't detected. */
#if (BPSCN_NOTIFY == 0)
	/* Add the BPSCN detection, by poling the IOVA information. The code
	 * bellow is based only on BPSCN event detection */
	fsl_print(" tasks = %ll id = 0x%x\n", num_tasks, id);
#endif
	while (!is_depleted) {
		/* Let other tasks to be scheduled on this core */
		__e_hwacceli(YIELD_ACCEL_ID);
		fsl_print("");
	}
	atomic_decr64(&num_tasks, 1);
	if (num_tasks == 0)
		is_depleted = 0;
	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_FLAGS);
	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();
	fdma_terminate_task();
}

/******************************************************************************/
static int app_bpscn_depletion_dpni_add_cb(uint8_t generator_id,
					   uint8_t event_id, uint64_t app_ctx,
					   void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni;
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;
	uint16_t		mfl = 0x2000;
	struct ep_init_presentation			init_presentation;

	UNUSED(generator_id);
	UNUSED(event_id);
	ni = (uint16_t)((uint32_t)event_data);
	fsl_print("\n\t >>> Configure NI %d\n", ni);
	/* Get end-point and skip initialization of connected DPNIs */
	err = dpni_drv_get_connected_obj(ni, &dpni_ep_id,
					 dpni_ep_type, &link_state);
	if (err) {
		pr_err("Cannot get connected object for NI %d\n", ni);
		return err;
	}
	/* Get endpoint and skip DPNI-DPNI connection from app initialization */
	if (strcmp(dpni_ep_type, "dpni") == 0) {
		fsl_print("\t >>> NI %d skipped (DPNI-DPNI link)\n", ni);
		return 0;
	}
	/* Configure frame processing callback */
	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("Cannot configure processing call-back on NI %d\n", ni);
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
	app_fill_fs_kg_profile(&kg_cfg);
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
	err = dpni_drv_set_max_frame_length(ni, mfl);
	if (err) {
		pr_err("Cannot configure maximum frame length on NI %d\n", ni);
		return err;
	}
	/*fsl_print("\t >>> %d : Maximum frame length on NI\n", mfl, ni);*/
	/* Set promiscuous mode */
	err = dpni_drv_set_unicast_promisc(ni, 1);
	if (err) {
		pr_err("Set promiscuous mode on NI %d\n", ni);
		return err;
	}
	/*fsl_print("\t >>> Promiscuous mode set on NI %d\n", ni);*/

	/* This interface is tracked for depletion state */
	if (ni == 1) {
		fsl_print("\t >>> Trace depletion on NI %d\n", ni);
		err = dpni_drv_get_fqid(ni, DPNI_DRV_QUEUE_RX, 0, &dpni_fqid);
		if (err) {
			pr_err("Get FQID for NI %d, err = %d\n", ni, err);
			return err;
		}
		fsl_print("\t >>> FQID = 0x%x\n", dpni_fqid);
	}
	/* Enable DPNI to receive frames */
	err = dpni_drv_enable(ni);
	if (err) {
		pr_err("Cannot enable NI %d for Rx/Tx\n", ni);
		return err;
	}
	fsl_print("%s : dpni.%d <---connected---> %s.%d ",
		  AIOP_APP_NAME, dpni_id, dpni_ep_type, dpni_ep_id);
	fsl_print("(MAC addr: %02x:%02x:%02x:%02x:%02x:%02x)\n",
		  mac_addr[0], mac_addr[1], mac_addr[2],
		  mac_addr[3], mac_addr[4], mac_addr[5]);
	fsl_print("\t >>> Successfully configured  NI %d (dpni.%d)\n",
		  ni, dpni_id);
	return 0;
}
#endif	/* BPSCN_DEPLETION_TEST */

#ifdef CSCN_CONGESTION_TEST
/******************************************************************************/
static inline uint8_t get_cscn_state_from_iova(uint64_t iova)
{
	uint8_t		ws_iova[SCN_SIZE], state;
	uint16_t	val16;
	uint64_t	val64;

	/* State Change Notification Message Format is in the LE byte order
	 * in chunks of 16 bytes :
	 * 15-8	: CTX/ICNT
	 * 7	: TOK
	 * 6-4	: RID
	 * 3	: reserved
	 * 2	: STATE
	 *		Bit 0 (lsbit): State of the Congested State bit in the
	 *		Congestion Group (CG):
	 *			0: The CG is not congested.
	 *			1: The CG is congested.
	 *		Bit 7-1 (msbs): Reserved
	 * 1	: STAT
	 * 0	: VERB
	 *		27h: CSCN-to-memory (Congestion State Change
	 *			Notification written to memory)
	 *		2Ah: CSCN-to-WQ (Congestion State Change Notification
	 *		delivered to a WQ)
	 * 31-16 : reserved
	 * 47-32 : reserved
	 * 63-48 : reserved
	 */
	cdma_read_with_no_cache(ws_iova, iova, SCN_SIZE);
	state = CSCN_GET_CONGESTION_STATE(&ws_iova[0]);
	val16 = CSCN_GET_CGID(&ws_iova[0]);
	val64 = CSCN_GET_MESSAGE_CTX(&ws_iova[0]);
	/*fsl_print("\t >>> State = %d CGID = %d Ctx = 0x%x-%x\n",
		  state, val16, (uint32_t)(val64 >> 32),
		  (uint32_t)val64);*/
	return state;
}

/******************************************************************************/
static __COLD_CODE int cscn_iova_slab_allocate(void)
{
	int	err;

	err = slab_register_context_buffer_requirements(MAX_DPNI_ID,
							MAX_DPNI_ID,
							CSCN_SIZE,
							CSCN_ALIGN,
							MEM_PART_SYSTEM_DDR,
							0, 0);
	if (err)
		return err;
	fsl_print("\t >>> CSCN IOVAs pool : Allocated\n");
	return 0;
}

/******************************************************************************/
static __COLD_CODE int slab_create_cscsn_iova(void)
{
	int		err;

	cscn_slab_handle = NULL;
	/* Allocate buffers for DPNIs CSCN iova */
	err = slab_create(MAX_DPNI_ID, MAX_DPNI_ID, CSCN_SIZE,
			  CSCN_ALIGN, MEM_PART_SYSTEM_DDR,
			  0, NULL, &cscn_slab_handle);
	if (err) {
		pr_err("CSCN slab_create failed\n");
		return err;
	}
	fsl_print("\t >>> CSCN IOVAs pool : Created\n");
	return 0;
}

/******************************************************************************/
static __COLD_CODE int get_cscsn_iova(uint64_t *cscn_iova)
{
	int		err;

	*cscn_iova = 0;
	err = slab_acquire(cscn_slab_handle, cscn_iova);
	if (err) {
		pr_err("CSCN_IOVA slab_acquire failed\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
static void cscn_iova_init(uint64_t iova)
{
	uint8_t		ws_iova[CSCN_SIZE];
	int		i;

	for (i = 0; i < CSCN_SIZE; i++)
		ws_iova[i] = 0;
	cdma_write(iova, &ws_iova[0], CSCN_SIZE);
}

/******************************************************************************/
static inline void cscn_iova_print(uint64_t iova)
{
	uint8_t		ws_iova[CSCN_SIZE];
	int		i;

	cdma_read_with_no_cache(&ws_iova[0], iova, CSCN_SIZE);
	LOCK_PRINT();
	fsl_print("\t ");
	/* Only the first 16 bytes are relevant */
	for (i = 0; i < 16; i++, iova++) {
		fsl_print("%02x ", ws_iova[i]);
		if (!((i + 1) % 16))
			fsl_print("\n\t ");
	}
	fsl_print("\n");
	UNLOCK_PRINT();
}

/******************************************************************************/
__HOT_CODE void cscn_callback(void)
{
	uint16_t	congested, cgid;

	/* Congestion state change detected. First, the congested state is
	 * reported.
	 * Here the application should wait for the "congested exit" state by
	 * polling the congestion status in the CG related SCN iova buffer. */
	osm_task_init();
	congested = LDPAA_FD_GET_SCN_STATE(HWC_FD_ADDRESS);
	cgid = LDPAA_FD_GET_SCN_CGID(HWC_FD_ADDRESS);
	dbg_print("APP - CSCN notification received : STATE = %d CGID = %d\n",
		  congested, cgid);
	/* Here one must perform the needed actions (applications dependent) to
	 * determine the exit from the "congested" state.
	 *
	 * In this application, some processing is performed and then the
	 * is_congested variable is set.
	 * The packets processing task (app_cscn_congestion_process_cb) will
	 * send the accumulated received packets over a DPNI.
	 * The "congestion exit" status should be detected by reading the
	 * IOVA information corresponding to the congested CG. */
	if (congested) {
		is_congested = 1;
		term_tasks = 0;
		/* Let accumulated packets to be consumed */
		fdma_fq_xon(dpni_fqid);
	}
	while (congested && is_congested) {
		/* Let other tasks to be scheduled on this core */
		__e_hwacceli(YIELD_ACCEL_ID);
		dbg_print("");
		/* End of "congested" state processing */
		congested = get_cscn_state_from_iova(dpni_cscn_iova);
		if (!congested) {
			/*cscn_iova_print(dpni_cscn_iova);*/
			is_congested = 0;
		}
	}
	fdma_terminate_task();
}

/******************************************************************************/
static __HOT_CODE ENTRY_POINT void app_cscn_congestion_process_cb(void)
{
	int			err = 0;
	uint32_t		id;
	uint8_t			cs = 0;

	sl_prolog();
	id = get_task();
	if (first_task_id == (uint32_t)-1) {
		first_task_id = id;
		if (task_get_receive_niid() == 1)
			fdma_fq_xoff(dpni_fqid);
	}
	atomic_incr64(&num_tasks, 1);
	/* Send the packet and do not terminate the task. Buffer depletion
	 * should't occur */
	err = dpni_drv_send(task_get_receive_niid(), 0);
	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();
#if (CSCN_NOTIFY == 0)
	/* CSCN detection by poling the IOVA information.
	 * Congestion enter detection. Tasks are not terminated till
	 * the CS state is detected. */
	while (!is_congested) {
		/* Let other tasks to be scheduled on this core */
		__e_hwacceli(YIELD_ACCEL_ID);
		fsl_print("");
		/* Let only this task detect the CS */
		cs = get_cscn_state_from_iova(dpni_cscn_iova);
		if (cs && id == first_task_id &&
		    first_task_id != (uint32_t)-1) {
			fsl_print("\t >>> [%ll] CS enter detected !\n",
				  num_tasks);
			/*cscn_iova_print(dpni_cscn_iova);*/
			/* Do not perform this operation if you want to keep
			 * the congested state */
			is_congested = cs;
			term_tasks = 0;
			if (task_get_receive_niid() == 1)
				fdma_xon(FLOW_CONTROL_FQID, dpni_fqid, 0);
		}
	}
	if (id != first_task_id) {
		atomic_decr64(&num_tasks, 1);
		atomic_incr64(&term_tasks, 1);
	} else {
		/* After terminating CSCN_ENTRY_THRES - CSCN_EXIT_THRES tasks
		 * the CS exit condition should occur */
		do {
			__e_hwacceli(YIELD_ACCEL_ID);
			fsl_print("");
		} while (term_tasks < (CSCN_ENTRY_THRES - CSCN_EXIT_THRES) + 1);
		/*fsl_print("\t >>> Terminated %ll tasks\n", term_tasks);*/
		atomic_decr64(&num_tasks, 1);
		term_tasks = 0;
		cs = get_cscn_state_from_iova(dpni_cscn_iova);
		if (!cs) {
			fsl_print("\t >>> [%ll] CS exit detected !\n",
				  num_tasks);
			/*cscn_iova_print(dpni_cscn_iova);*/
			is_congested = cs;
		}
		first_task_id = (uint32_t)-1;
	}
#else
	UNUSED(cs);
	if (id == first_task_id) {
		/* Wait till all accumulated packets are consumed */
		do {
			__e_hwacceli(YIELD_ACCEL_ID);
			fsl_print("");
		} while (term_tasks < CSCN_ENTRY_THRES - 1);
		first_task_id = (uint32_t)-1;
	}
	atomic_decr64(&num_tasks, 1);
	atomic_incr64(&term_tasks, 1);
#endif
	fdma_terminate_task();
}

/******************************************************************************/
static int app_cscn_congestion_dpni_add_cb(uint8_t generator_id,
					   uint8_t event_id,
					   uint64_t app_ctx, void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni;
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;
	uint16_t		mfl = 0x2000;
	uint64_t		loc_iova;
	struct ep_init_presentation			init_presentation;
	struct dpni_drv_congestion_notification_cfg	cscn_cfg;

	UNUSED(generator_id);
	UNUSED(event_id);
	ni = (uint16_t)((uint32_t)event_data);
	/* Get end-point and skip initialization of connected DPNIs */
	err = dpni_drv_get_connected_obj(ni, &dpni_ep_id,
					 dpni_ep_type, &link_state);
	if (err) {
		pr_err("Cannot get connected object for NI %d\n", ni);
		return err;
	}
	/* Configure frame processing callback */
	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("Cannot configure processing call-back on NI %d\n", ni);
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
	app_fill_fs_kg_profile(&kg_cfg);
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
	err = dpni_drv_set_max_frame_length(ni, mfl);
	if (err) {
		pr_err("Cannot configure maximum frame length on NI %d\n", ni);
		return err;
	}
	/*fsl_print("\t >>> %d : Maximum frame length on NI\n", mfl, ni);*/
	/* Set promiscuous mode */
	err = dpni_drv_set_unicast_promisc(ni, 1);
	if (err) {
		pr_err("Set promiscuous mode on NI %d\n", ni);
		return err;
	}
	/*fsl_print("\t >>> Promiscuous mode set on NI %d\n", ni);*/
	/* Set congestion notification at Rx */
	cscn_cfg.notification_mode = DPNI_CONG_OPT_NOTIFY_DEST_ON_ENTER |
				   DPNI_CONG_OPT_NOTIFY_DEST_ON_EXIT |
				   DPNI_CONG_OPT_WRITE_MEM_ON_ENTER |
				   DPNI_CONG_OPT_WRITE_MEM_ON_EXIT |
				   CSCN_NOTIFY;
	cscn_cfg.units = DPNI_DRV_CONGESTION_UNIT_FRAMES;
	/* Above this threshold : enter the congestion state */
	cscn_cfg.threshold_entry = CSCN_ENTRY_THRES;
	/* Below this threshold : exit the congestion state */
	cscn_cfg.threshold_exit = CSCN_EXIT_THRES;
	err = get_cscsn_iova(&loc_iova);
	if (err)
		return err;
	ASSERT_COND(ni < MAX_DPNI_ID);
	cscn_info[ni].ni = ni;
	cscn_info[ni].cscn_iova = loc_iova;
	cscn_iova_init(loc_iova);
	cscn_cfg.message_iova = loc_iova;
	cscn_cfg.message_ctx = 0x0123456789ABCDEF;
	/* This interface is tracked for congestion state */
	if (ni == 1) {
		fsl_print("\t >>> Trace congestion on NI %d\n", ni);
		dpni_cscn_iova = loc_iova;
		err = dpni_drv_get_fqid(ni, DPNI_DRV_QUEUE_RX, 0, &dpni_fqid);
		if (err) {
			pr_err("Get FQID for NI %d, err = %d\n", ni, err);
			return err;
		}
		fsl_print("\t >>> FQID = 0x%x\n", dpni_fqid);
	}
	err = dpni_drv_set_congestion_notification(ni, 0, DPNI_DRV_QUEUE_RX,
						   &cscn_cfg);
	if (err) {
		pr_err("Set congestion notification for NI %d, err = %d\n",
		       ni, err);
		return err;
	}
	#define PRINT_CONGESTION_CFG
#ifdef PRINT_CONGESTION_CFG
	err = dpni_drv_get_congestion_notification(ni, 0, DPNI_DRV_QUEUE_RX,
						   &cscn_cfg);
	if (err) {
		pr_err("Get congestion notification for NI %d, err = %d\n",
		       ni, err);
		return err;
	}
	fsl_print("\t >>> cscn_cfg.threshold_entry   = %d\n",
		  cscn_cfg.threshold_entry);
	fsl_print("\t >>> cscn_cfg.threshold_exit    = %d\n",
		  cscn_cfg.threshold_exit);
	fsl_print("\t >>> cscn_cfg.units             = 0x%x\n",
		  (uint32_t)cscn_cfg.units);
	fsl_print("\t >>> cscn_cfg.message_iova      = 0x%x-0x%x\n",
		  (uint32_t)(cscn_cfg.message_iova >> 32),
		  (uint32_t)(cscn_cfg.message_iova));
	fsl_print("\t >>> cscn_cfg.message_ctx       = 0x%x-0x%x\n",
		  (uint32_t)(cscn_cfg.message_ctx >> 32),
		  (uint32_t)(cscn_cfg.message_ctx));
	fsl_print("\t >>> cscn_cfg.notification_mode = 0x%x\n",
		  (uint32_t)cscn_cfg.notification_mode);
#endif	/* PRINT_CONGESTION_CFG */
	fsl_print("\t >>> Congestion notification set on NI %d\n", ni);
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

#endif	/* CSCN_CONGESTION_TEST */

/******************************************************************************/
static int app_dpni_link_up_cb(uint8_t generator_id, uint8_t event_id,
			       uint64_t app_ctx, void *event_data)
{
	uint16_t			ni;

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);
	ni = (uint16_t)((uint32_t)event_data);
	fsl_print("%s : NI_%d link is UP\n", AIOP_APP_NAME, ni);
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
		/*cfg.options = state.options | DPNI_LINK_OPT_PFC_PAUSE;*/
		cfg.options = state.options | DPNI_DRV_LINK_OPT_PAUSE;
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

/******************************************************************************/
static int app_dpni_link_down_cb(uint8_t generator_id, uint8_t event_id,
				 uint64_t app_ctx, void *event_data)
{
	uint16_t	ni = (uint16_t)((uint32_t)event_data);

	UNUSED(generator_id);
	UNUSED(event_id);
	UNUSED(app_ctx);
	fsl_print("%s : NI_%d link is DOWN\n", AIOP_APP_NAME, ni);
	return 0;
}

/******************************************************************************/
int app_early_init(void)
{
	int	err;

#ifdef BPSCN_DEPLETION_TEST
	g_app_params.dpni_num_buffs = NUM_BUFFERS;
	err = scn_iova_slab_allocate(DPNI_MAX_POOLS);
	if (err)
		return err;
#endif
	fsl_print("DPNI buffers = %d\n", g_app_params.dpni_num_buffs);
	fsl_print("Buffer size  = %d\n", g_app_params.dpni_buff_size);
#ifdef CSCN_CONGESTION_TEST
	err = cscn_iova_slab_allocate();
#endif
	return err;
}

/******************************************************************************/
int app_init(void)
{
	int	err;

#ifdef CSCN_CONGESTION_TEST
	first_task_id = (uint32_t)-1;
	num_tasks = 0;
	term_tasks = 0;
	is_congested = 0;
	dpni_cscn_iova = 0;
	cscn_slab_handle = 0;
	for (int i = 0; i < MAX_DPNI_ID; i++) {
		cscn_info[i].ni = (uint16_t)-1;
		cscn_info[i].cscn_iova = 0;
	}
	err = slab_create_cscsn_iova();
	if (err)
		return err;
	/* Register DPNI events */
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t)app_cscn_congestion_process_cb,
			     app_cscn_congestion_dpni_add_cb);
	if (err) {
		pr_err("Cannot register DPNI ADD event\n");
		return err;
	}
#endif
#ifdef BPSCN_DEPLETION_TEST

	uint64_t				loc_iova;
	struct dpni_drv_bpscn_cfg		bpscn_cfg;
	struct dpni_drv_free_bufs		free_buffs;

	num_tasks = 0;
	term_tasks = 0;
	is_depleted = 0;
	bpscn_slab_handle = 0;
	for (int i = 0; i < DPNI_MAX_POOLS; i++) {
		bpscn_info[i].backup_pool = 0;
		bpscn_info[i].bpscn_iova = 0;
		bpscn_info[i].num_bufs = 0;
		bpscn_info[i].bpid = (uint16_t)-1;
	}
	/* All DPNIs share the same pools of buffers. Configure the notification
	 * on a buffer pool state change notification once for all DPNIs */
	err = scn_iova_slab_create(DPNI_MAX_POOLS, &bpscn_slab_handle);
	if (err)
		return err;
	err = scn_iova_get(bpscn_slab_handle, &loc_iova);
	if (err)
		return err;
	scn_iova_init(loc_iova);
	bpscn_info[0].backup_pool = 0;
	bpscn_info[0].bpscn_iova = loc_iova;
	err = dpni_drv_get_num_free_bufs(DPNI_DRV_PEB_FREE_BUFS, &free_buffs);
	if (err)
		return err;
	bpscn_info[0].num_bufs = free_buffs.peb_bp_free_bufs;
	/* BPID info is used to find the bpscn_info when the depletion
	 * notification is received */
	bpscn_info[0].bpid = free_buffs.peb_bpid;
	/* This pool is tracked for depleted state */
	fsl_print("\t >>> Trace depletion on PEB : BPID = %d, buffers = %d\n",
		  bpscn_info[0].bpid, bpscn_info[0].num_bufs);
	bpscn_cfg.depletion_entry = BPSCN_ENTRY_THRES;
	bpscn_cfg.depletion_exit = BPSCN_EXIT_THRES;
	bpscn_cfg.message_iova = loc_iova;
	bpscn_cfg.message_ctx = loc_iova;
	bpscn_cfg.options = BPSCN_NOTIFY;
	err = dpni_drv_set_pool_depletion(DPNI_DRV_DEFAULT_POOL_NOTIF,
					  &bpscn_cfg);
	if (err) {
		pr_err("Notification on DEFAULT_POOL, err = %d\n", err);
		return err;
	}
	fsl_print("\t >>> Notification on DEFAULT_POOL set\n");
	/*#define PRINT_DEPLETION_CFG*/
#ifdef PRINT_DEPLETION_CFG
	err = dpni_drv_get_pool_depletion(DPNI_DRV_DEFAULT_POOL_NOTIF,
					  &bpscn_cfg);
	if (err) {
		pr_err("Get notification of DEFAULT_POOL, err = %d\n",
		       err);
		return err;
	}
	fsl_print("\t >>> bpscn_cfg.depletion_entry = %d\n",
		  bpscn_cfg.depletion_entry);
	fsl_print("\t >>> bpscn_cfg.depletion_exit  = %d\n",
		  bpscn_cfg.depletion_exit);
	fsl_print("\t >>> bpscn_cfg.message_iova    = 0x%x-0x%x\n",
		  (uint32_t)(bpscn_cfg.message_iova >> 32),
		  (uint32_t)(bpscn_cfg.message_iova));
	fsl_print("\t >>> bpscn_cfg.options         = 0x%x\n",
		  bpscn_cfg.options);
#endif	/* PRINT_DEPLETION_CFG */
	/* Register DPNI events */
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t)app_bpscn_depletion_process_cb,
			     app_bpscn_depletion_dpni_add_cb);
	if (err) {
		pr_err("Cannot register DPNI ADD event\n");
		return err;
	}
#endif
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_UP, 1,
			     (uint64_t)NULL, app_dpni_link_up_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK UP event\n");
		return err;
	}
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_LINK_DOWN, 1,
			     (uint64_t)NULL, app_dpni_link_down_cb);
	if (err) {
		pr_err("Cannot register DPNI LINK DOWN event\n");
		return err;
	}
	return 0;
}

/******************************************************************************/
void app_free(void)
{}
