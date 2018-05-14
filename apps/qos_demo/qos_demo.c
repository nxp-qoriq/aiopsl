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
#include "qos_demo.h"

/* Enable PFC pause frame transmission */
/*#define ENABLE_PFC_PAUSE*/

/* Print link configuration */
#define PRINT_LINK_CFG

/* Print QoS statistics on a 60 seconds timer expiration */
#define PRINT_QOS_STATISTICS

/* Print information about every received packet */
#define PRINT_RX_PKT_INFO

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

uint64_t		print_lock __attribute__ ((aligned(8)));
#define LOCK_PRINT()	lock_spinlock(&print_lock)
#define UNLOCK_PRINT()	unlock_spinlock(&print_lock)

uint32_t	first_task_id;
int64_t		num_tasks  __attribute__ ((aligned(8)));
int64_t		term_tasks  __attribute__ ((aligned(8)));
uint64_t	is_congested;
uint64_t	is_depleted;
/* Size of State Change Notification information (Do not change it) */
#define SCN_SIZE			64
/* SCN information alignment (Do not change it) */
#define SCN_ALIGN			16
/* State offset in IOVA */
#define SCN_STATE_OFFSET		0x02

/* CSCN congestion test */
#define CSCN_CONGESTION_TEST
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
	/* Congestion is traced on this DPNI */
	uint32_t	dpni_fqid;
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

/*#define BPSCN_DEPLETION_TEST*/
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

/******************************************************************************/
static void app_fill_qos_kg_profile(struct dpkg_profile_cfg *kg_cfg)
{
	/* Configure Initial Order Scope */
	memset(kg_cfg, 0x0, sizeof(struct dpkg_profile_cfg));

	kg_cfg->num_extracts = 1;
	/* PROTO */
	kg_cfg->extracts[0].type = DPKG_EXTRACT_FROM_HDR;
	kg_cfg->extracts[0].extract.from_hdr.prot = NET_PROT_IP;
	kg_cfg->extracts[0].extract.from_hdr.type = DPKG_FULL_FIELD;
	kg_cfg->extracts[0].extract.from_hdr.field = NET_HDR_FLD_IP_PROTO;
}

/******************************************************************************/
/* fmt = 0 - AIOP input frame (as it is received from DPNI, TMAN, ...
 * fmt = 1 - to GPP format
 * fmt = 2 - to SEC format
 */
static void dump_fd(const char *what, uint8_t fmt)
{
	uint64_t	val64;
	uint8_t		val8, sl;

	#define FD_RCSUM_OFFSET		0x1a
	#define LDPAA_FD_GET_RCSUM(_fd)					\
		(LH_SWAP(0, (uint32_t)(((char *)_fd) +			\
				FD_RCSUM_OFFSET)))

	#ifndef FD_HASH_OFFSET
		#define FD_HASH_OFFSET	0x1c
	#endif
	#define LDPAA_FD_GET_HASH(_fd)					\
		(uint32_t)({register uint32_t *__rR = 0;		\
		uint32_t hash = (LW_SWAP(0, (uint32_t)(((char *)_fd) +	\
				FD_HASH_OFFSET)));			\
		__rR = (uint32_t *)hash; })

	dbg_print("\n Frame Descriptor at 0x%x : %s\n", HWC_FD_ADDRESS, what);
	sl = LDPAA_FD_GET_SL(HWC_FD_ADDRESS);

	/* The FD format is little-endian */
	val64 = LDPAA_FD_GET_ADDR(HWC_FD_ADDRESS);
	/* Buffer address */
	dbg_print("\t Buffer address      = 0x%x-%08x\n",
		  (uint32_t)(val64 >> 32), (uint32_t)(val64));
	/* Frame data length */
	if (!sl) {
		dbg_print("\t Frame data length   = %d (Long)\n",
			  LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
	} else {
		dbg_print("\t Frame data length   = %d (Short)\n",
			  LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS));
		dbg_print("\t 12-bit MEM          = %03x\n",
			  LDPAA_FD_GET_MEM(HWC_FD_ADDRESS));
	}
	/* BPID */
	dbg_print("\t BPID                = %d\n",
		  LDPAA_FD_GET_BPID(HWC_FD_ADDRESS));
	/* IVP */
	dbg_print("\t IVP (Invalid Pool)  = %d\n",
		  LDPAA_FD_GET_IVP(HWC_FD_ADDRESS));
	/* BMT */
	dbg_print("\t BMT                 = %d\n",
		  LDPAA_FD_GET_BMT(HWC_FD_ADDRESS));
	/* Frame data offset */
	dbg_print("\t Frame data offset   = %d\n",
		  LDPAA_FD_GET_OFFSET(HWC_FD_ADDRESS));
	/* FMT */
	val8 = LDPAA_FD_GET_FMT(HWC_FD_ADDRESS);
	dbg_print("\t FMT(2b)             = 0x%x (%s)\n", val8,
		  val8 == 0 ? "Single Buffer" :
		  val8 == 1 ? "Frame List" :
		  val8 == 2 ? "Scatter/Gather" :
		  val8 == 3 ? "Proprietary" : "Unknown");
	/* SL */
	dbg_print("\t SL                  = %d\n", sl);
	/* Frame Context */
	dbg_print("\t Frame Context (FRC) = 0x%08x\n",
		  LDPAA_FD_GET_FRC(HWC_FD_ADDRESS));
	/* Fame control fields */
	dbg_print("\t ERR(8b)             = 0x%02x\n",
		  LDPAA_FD_GET_ERR(HWC_FD_ADDRESS));
	/* VA */
	dbg_print("\t VA                  = %d\n",
		  LDPAA_FD_GET_VA(HWC_FD_ADDRESS));
	/* CBMT */
	dbg_print("\t CBMT                = %d\n",
		  LDPAA_FD_GET_CBMT(HWC_FD_ADDRESS));
	/* ASAL */
	dbg_print("\t ASAL(4b)            = 0x%x\n",
		  LDPAA_FD_GET_ASAL(HWC_FD_ADDRESS));
	/* PTV2 */
	dbg_print("\t PTV2                = %d\n",
		  LDPAA_FD_GET_PTV2(HWC_FD_ADDRESS));
	/* PTV1 */
	dbg_print("\t PTV1                = %d\n",
		  LDPAA_FD_GET_PTV1(HWC_FD_ADDRESS));
	/* PTA */
	dbg_print("\t PTA                 = %d\n",
		  LDPAA_FD_GET_PTA(HWC_FD_ADDRESS));
	/* DROPP */
	dbg_print("\t DROPP(3b)           = 0x%x\n",
		  LDPAA_FD_GET_DROPP(HWC_FD_ADDRESS));
	/* SC */
	dbg_print("\t SC (Stash Control)  = %d\n",
		  LDPAA_FD_GET_SC(HWC_FD_ADDRESS));
	/* DD */
	dbg_print("\t DD(4b)              = 0x%x\n",
		  LDPAA_FD_GET_DD(HWC_FD_ADDRESS));
	if (fmt == 0) {
		/* FLC fields for WRIOP to AIOP */
		/* Hash */
		dbg_print("\t Hash             = 0x%08x\n",
			  LDPAA_FD_GET_HASH(HWC_FD_ADDRESS));
		/* Running Checksum */
		dbg_print("\t Running Checksum = 0x%04x\n",
			  LDPAA_FD_GET_RCSUM(HWC_FD_ADDRESS));
		/* EPID */
		dbg_print("\t EPID             = 0x%04x\n",
			  LDPAA_FD_GET_EPID(HWC_FD_ADDRESS));
	} else if (fmt == 1) {
		/* FLC fields for WRIOP to GPP */
		/* CS */
		dbg_print("\t CS(2b)              = 0x%x\n",
			  LDPAA_FD_GET_CS(HWC_FD_ADDRESS));
		/* AS */
		dbg_print("\t AS(2b)              = 0x%x\n",
			  LDPAA_FD_GET_AS(HWC_FD_ADDRESS));
		/* DS */
		dbg_print("\t DS(2b)              = 0x%x\n",
			  LDPAA_FD_GET_DS(HWC_FD_ADDRESS));
		/* Flow Context */
		val64 = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
		dbg_print("\t Flow Context (FLC)  = 0x%08x-%08x\n",
			  (uint32_t)(val64 >> 32), (uint32_t)(val64));
	} else {
		/* Flow Context */
		val64 = LDPAA_FD_GET_FLC(HWC_FD_ADDRESS);
		dbg_print("\t Flow Context (FLC)  = 0x%08x-%08x\n",
			  (uint32_t)(val64 >> 32), (uint32_t)(val64));
	}
	dbg_print("\n");
}

/******************************************************************************/
static void dump_adq(const char *what)
{
	uint8_t		ff, bdi, auc, fd_src, pri;
	uint16_t	pl_3_icid_3, pl_2_icid_2, pl_icid;
	uint32_t	fqid_auc_va_2_3, ch_fqid;
	struct additional_dequeue_context *adc;

	adc = (struct additional_dequeue_context *)HWC_ADC_ADDRESS;
	dbg_print("\n Additional Dequeue Context at 0x%x : %s\n",
		  (uint32_t)adc, what);
	/* Frame Queue Context as received from QMan via the AIOP DCP. */
	dbg_print("\t Frame Queue Context :\n");
	/* FQD Context
	At software portals, carries an opaque 8 byte value that is delivered
	in the dequeue responses from this FQ.

	At DCP portals, if FF = 0 this carries the return or output FQID for an
	accelerator, and possibly one or two extra sets of access management
	qualifiers (AMQ).
		AMQ_1 = {ICID,PL,VA} from the FQ
		AMQ_2 = {ICID_2,PL_2,VA_2} from FQD_CTX
		AMQ_3 = {ICID_3,PL_3,VA_3} from FQD_CTX
	Optionally, if FF = 1, this field may carry other data specific to the
	accelerator.

	If FF = 0, in a DCP portal dequeue response this field carries the
	following:

		FQD_CTX[23:0] = Return or output FQID. If BDI = 0, this FQID
		must be a virtual FQID authorized for use with either ICID or
		ICID_2 or ICID_3.

		FQD_CTX[25:24] = AMQ usage control (AUC).
			00 = AMQ_2 and AMQ_3 are not valid. AMQ_1 is used for
			all needed authorization (input, output, and control).

			01 = AMQ_2 is valid. AMQ_1 used for input and control,
			AMQ_2 used for output.

			10 = AMQ_2 is valid. AMQ_1 used for input, AMQ_2 used
			for control and output.

			11 = AMQ_2 and AMQ_3 are valid. AMQ_1 used for input,
			AMQ_2 used for control, AMQ_3 used for output.

		FQD_CTX[29:26] = Reserved, must be zero.
		FQD_CTX[30] = VA_2. Valid if AUC is non-zero.
		FQD_CTX[31] = VA_3. Valid if AUC = 3.
		FQD_CTX[46:32] = ICID_2. Valid if AUC is non-zero.
		FQD_CTX[47] = PL_2. Valid if AUC is non-zero.
		FQD_CTX[62:48] = ICID_3. Valid if AUC = 3.
		FQD_CTX[63] = PL_3. Valid if AUC = 3.

	If AMQ_2 and AMQ_3 are valid, they may be used for memory access
	authorization to control space or output data that is not authorized by
	the first ICID. Also, If BDI = 0, ICID_2 and ICID_3 may be used to
	authorize access to DPAA resources that are not authorized by the first
	ICID.

	Note also that WRIOP does not dequeue from regular FQs, it dequeues
	only from CEETM class queues, therefore the FQD_CTX defined here is
	not used by WRIOP.
	 */
	/* 8 byte opaque Frame Queue Context delivered from the QMAN in the
	dequeue responses from this FQ. At DCP portals, if FF = 0 this carries
	the return or output FQID for an accelerator, and possibly one or
	two extra sets of access management qualifiers (AMQ).
	Optionally, if FF = 1, this field may carry other data specific to the
	accelerator.*/

	/**
	 * - bits<0>: VA_3. Valid if AUC = 3.
	 * - bits<1>: VA_2. Valid if AUC is non-zero.
	 * - bits<6-7>: AUC - AMQ usage control:
	 *	00 = AMQ_2 and AMQ_3 are not valid.
	 *	     AMQ_1 is used for all needed authorization (input,
	 *	     output, and control).
	 *	01 = AMQ_2 is valid. AMQ_1 used for input and control,
	 *	     AMQ_2 used for output.
	 *	10 = AMQ_2 is valid. AMQ_1 used for input, AMQ_2 used
	 *	     for control and output.
	 *	11 = AMQ_2 and AMQ_3 are valid. AMQ_1 used for input,
	 *	AMQ_2 used for control, AMQ_3 used for output.
	 * - bits<8-31>: fqid - Return/Output FQID for an accelerator.
	 **/
	bdi = (adc->fdsrc_va_fca_bdi & 0x01) ? 1 : 0;
	fqid_auc_va_2_3 = LW_SWAP(0, &adc->fqd_ctx.fqid_auc_va_2_3);
	dbg_print("\t fqid_auc_va_2_3 = 0x%08x\n", fqid_auc_va_2_3);
	dbg_print("\t\t FQID = 0x%08x (%s)\n", fqid_auc_va_2_3 & 0x00ffffff,
		  bdi ? "Non Virtual Queue, BDI = 1" :
		  "Virtual Queue, BDI = 0");
	auc = (uint8_t)((fqid_auc_va_2_3 >> 24) & 0x03);
	dbg_print("\t\t AUC  = 0x%02x (%s)\n", auc,
		  auc == 0 ? "AMQ_1 : In, Out, Ctrl" :
		  auc == 1 ? "AMQ_1 : In, Ctrl - AMQ_2 : Out" :
		  auc == 2 ? "AMQ_1 : In - AMQ_2 : Out, Ctrl" :
		  "AMQ_1 : In - AMQ_2 : Ctrl - AMQ_3 : Out");
	dbg_print("\t\t res  = 0x%02x\n", (fqid_auc_va_2_3 >> 26) & 0x0f);
	dbg_print("\t\t VA_2 = %d (%s)\n", fqid_auc_va_2_3 & 0x40000000 ? 1 : 0,
		  auc ? "Valid, AUC != 0" : "Invalid, AUC = 0");
	dbg_print("\t\t VA_3 = %d (%s)\n", fqid_auc_va_2_3 & 0x80000000 ? 1 : 0,
		  auc == 3 ? "Valid, AUC = 3" : "Invalid, AUC != 3");
	/**
	 * - bits<0>: PL_3. Valid if AUC = 3.
	 * - bits<1-15>: ICID_3. Valid if AUC = 3.
	 * */
	pl_3_icid_3 = LH_SWAP(0, &adc->fqd_ctx.pl_3_icid_3);
	dbg_print("\t pl_3_icid_3     = 0x%04x\n", pl_3_icid_3);
	dbg_print("\t\t PL_3   = %d (%s)\n", pl_3_icid_3 & 0x80000000 ? 1 : 0,
		  auc == 3 ? "Valid, AUC = 3" : "Invalid, AUC != 3");
	dbg_print("\t\t ICID_3 = 0x%04x\n", pl_3_icid_3 & 0x7fff);
	/**
	 * - bits<0>: PL_2. Valid if AUC is non-zero.
	 * - bits<1-15>: ICID_2. Valid if AUC is non-zero.
	 **/
	pl_2_icid_2 = LH_SWAP(0, &adc->fqd_ctx.pl_2_icid_2);
	dbg_print("\t pl_2_icid_2     = 0x%04x\n", pl_2_icid_2);
	dbg_print("\t\t PL_2   = %d (%s)\n", pl_2_icid_2 & 0x8000 ? 1 : 0,
		  auc ? "Valid, AUC != 0" : "Invalid, AUC = 0");
	dbg_print("\t\t ICID_2 = 0x%04x\n", pl_2_icid_2 & 0x7fff);

	/* Channel & FQID */
	ch_fqid = LW_SWAP(0, &adc->channel_fqid);
	dbg_print("\t channel_fqid     = 0x%08x\n", ch_fqid);
	dbg_print("\t\t CHANNEL(8b) = 0x%02x\n", (ch_fqid >> 24) & 0xff);
	dbg_print("\t\t FQID(24b)   = 0x%08x\n", ch_fqid & 0x00ffffff);
	/* PL & ICID */
	pl_icid = LH_SWAP(0, &adc->pl_icid);
	dbg_print("\t pl_icid          = 0x%04x\n", pl_icid);
	dbg_print("\t\t PL(1b)    = %d\n", pl_icid & 0x8000 ? 1 : 0);
	dbg_print("\t\t ICID(15b) = 0x%04x\n", pl_icid & 0x7fff);
	/* WQID & PRI */
	dbg_print("\t wqid_pri         = 0x%02x\n", adc->wqid_pri);
	dbg_print("\t\t reserved(1b) = %d\n", (adc->wqid_pri & 0x80) ? 1 : 0);
	dbg_print("\t\t WQID(3b)     = 0x%02x\n",
		  (adc->wqid_pri >> 4) & 0x07);
	dbg_print("\t\t reserved(1b) = %d\n", (adc->wqid_pri & 0x08) ? 1 : 0);
	pri = adc->wqid_pri & 0x07;
	dbg_print("\t\t PRI(3b)      = 0x%02x (%s)\n", adc->wqid_pri & 0x07,
		  pri == 0 ? "Low" : pri == 1 ? "Medium" : pri == 2 ?
		  "Critical" : "Undefined");
	/* SRC & VA & FCF & BDI */
	dbg_print("\t fdsrc_va_fca_bdi = 0x%02x\n", adc->fdsrc_va_fca_bdi);
	dbg_print("\t\t reserved(1b) = %d\n",
		  (adc->fdsrc_va_fca_bdi & 0x80) ? 1 : 0);
	fd_src = (adc->fdsrc_va_fca_bdi >> 4) & 0x07;
	dbg_print("\t\t SRC(3b)      = 0x%02x (From %s)\n", fd_src,
		  fd_src == 0 ? "QMan" : fd_src == 1 ? "TMan" :
		  fd_src == 2 ? "CSCN" : fd_src == 3 ? "BPSCN" :
		  fd_src == 4 ? "CMCGW" : "Reserved");
	dbg_print("\t\t reserved(1b) = %d\n",
		  (adc->fdsrc_va_fca_bdi & 0x08) ? 1 : 0);
	dbg_print("\t\t VA(1b)       = %d\n",
		  (adc->fdsrc_va_fca_bdi & 0x04) ? 1 : 0);
	/* FQD_CTX Format. Used only in DCP portal dequeue responses.
		0: FQD_CTX carries a return FQID and possibly one or two extra
	ICID values, as defined in the FQD_CTX field.
		1: FQD_CTX carries some other accelerator specific context not
	defined in this table.*/
	ff = (adc->fdsrc_va_fca_bdi & 0x02) ? 1 : 0;
	dbg_print("\t\t FF(1b)       = %d (%s)\n", ff,
		  !ff ? "FQ CTX : Output FQID + extra AMQ" :
		  "FQ CTX : Output FQID + extra AMQ + accelerator data");
	dbg_print("\t\t BDI(1b)      = %d\n", bdi);
}

/******************************************************************************/
static void dump_prc(const char *what)
{
	struct presentation_context *prc;

	prc = (struct presentation_context *)HWC_PRC_ADDRESS;
	dbg_print("\n Presentation Context at 0x%x : %s\n",
		  (uint32_t)prc, what);
	dbg_print("\t PARAM                                      = 0x%08x\n",
		  prc->param);
	dbg_print("\t SEGMENT_ADDRESS                            = 0x%04x\n",
		  prc->seg_address);
	dbg_print("\t SEGMENT_LENGTH                             = 0x%04x\n",
		  prc->seg_length);
	dbg_print("\t SEGMENT_OFFSET                             = 0x%04x\n",
		  prc->seg_offset);
	dbg_print("\t SR(Segment Reference)                      = %d\n",
		  prc->sr_nds & 0x02 ? 1 : 0);
	dbg_print("\t NDS(No Data Segment)                       = %d\n",
		  prc->sr_nds & 0x01 ? 1 : 0);
	dbg_print("\t SEGMENT_HANDLE                             = 0x%02x\n",
		  prc->seg_handle);
	dbg_print("\t OSM Entry Point Source(1b)                 = %d\n",
		  prc->osrc_oep_osel_osrm & 0x80 ? 1 : 0);
	dbg_print("\t OSM Entry Point Execution Phase(1b)        = %d\n",
		  prc->osrc_oep_osel_osrm & 0x40 ? 1 : 0);
	dbg_print("\t OSM Entry Point Select(2b)                 = 0x%02x\n",
		  (prc->osrc_oep_osel_osrm >> 4) & 0x03);
	dbg_print("\t Reserved(1b)                               = %d\n",
		  prc->osrc_oep_osel_osrm & 0x08 ? 1 : 0);
	dbg_print("\t OSM Entry Point Order Scope Range Mask(3b) = 0x%02x\n",
		  prc->osrc_oep_osel_osrm & 0x07);
	dbg_print("\t FRAME_HANDLE                               = 0x%02x\n",
		  prc->frame_handle);
	dbg_print("\t Initial Scope Value                        = 0x%04x\n",
		  prc->isv);
}

/******************************************************************************/
static void dump_default_task_params(const char *what)
{
	dbg_print("Dump of AIOP default task parameters : %s\n", what);
	dbg_print("\t send_niid                = %d\n",
		  default_task_params.send_niid);
	dbg_print("\t parser_starting_hxs      = 0x%04x\n",
		  default_task_params.parser_starting_hxs);
	dbg_print("\t parser_profile_id        = %d\n",
		  default_task_params.parser_profile_id);
	dbg_print("\t qd_priority              = %d\n",
		  default_task_params.qd_priority);
	dbg_print("\t current_scope_level      = %d\n",
		  default_task_params.current_scope_level);
	for (int i = 0; i < 4; i++)
		dbg_print("\t scope_mode_level_arr[%d] = %d\n", i,
			  default_task_params.scope_mode_level_arr[i]);
}

/******************************************************************************/
static void dump_osm_registers(const char *what)
{
	struct aiop_tile_regs	*aiop_regs;
	struct aiop_osm_regs	*osm_regs;
	int			i;

	register uint32_t	task, tmp;

	aiop_regs = (struct aiop_tile_regs *)
			sys_get_handle(FSL_MOD_AIOP_TILE, 1);
	osm_regs = (struct aiop_osm_regs *)&aiop_regs->osm_regs;
	/*
	 * AIOP-z490 CPU
	 * 11.6.5 Task Control and Status Register 0, 1 (TASKCSR0, TASKCSR1)
	 */
	asm
	{
		mfdcr		tmp, dcr476	/* TASKCSR0 */
		e_clrlwi	task, tmp, 24	/* Clear top 24 bits */
	}
	/*
	 * Initialize TASK_ID in ORTAR to enable OSM registers.
	 *
	 * The order scope manager read task address register (ORTAR). Writing
	 * this register will populate the read task data registers with state
	 * information for the specified task number and will guarantee a
	 * consistent reading across all scopes.
	 * Use of this feature is not recommended during operation since it may
	 * have adverse effects on functional performance.*/
	iowrite32be(task, &osm_regs->ortar);

	dbg_print("Dump of OSM register : %s\n", what);
	dbg_print("\t [at %08x] OMR    = 0x%08x\n", (uint32_t)&osm_regs->omr,
		  ioread32be(&osm_regs->omr));
	dbg_print("\t [at %08x] OSR    = 0x%08x\n", (uint32_t)&osm_regs->osr,
		  ioread32be(&osm_regs->osr));
	dbg_print("\t [at %08x] ORTAR  = 0x%08x\n", (uint32_t)&osm_regs->ortar,
		  ioread32be(&osm_regs->ortar));
	for (i = 0; i < 8; i++) {
		dbg_print("\t [at %08x] ORTDR%d = 0x%08x ",
			  (uint32_t)&osm_regs->ortdr[i], i,
			  ioread32be(&osm_regs->ortdr[i]));
		if (i % 2)
			dbg_print("(Level %d - Scope_ID)\n", i / 2);
		else
			dbg_print("(Level %d - State)\n", i / 2);
	}
	dbg_print("\t [at %08x] OEMVR  = 0x%08x\n", (uint32_t)&osm_regs->oemvr,
		  ioread32be(&osm_regs->oemvr));
	dbg_print("\t [at %08x] OEMMR  = 0x%08x\n",
		  (uint32_t)&osm_regs->oemmr, ioread32be(&osm_regs->oemmr));
	dbg_print("\t [at %08x] OCR    = 0x%08x\n",
		  (uint32_t)&osm_regs->ocr, ioread32be(&osm_regs->ocr));
	dbg_print("\t [at %08x] OERR   = 0x%08x\n",
		  (uint32_t)&osm_regs->oerr, ioread32be(&osm_regs->oerr));
	dbg_print("\t [at %08x] OEDR   = 0x%08x\n",
		  (uint32_t)&osm_regs->oedr, ioread32be(&osm_regs->oedr));
	dbg_print("\t [at %08x] OEDDR  = 0x%08x\n",
		  (uint32_t)&osm_regs->oeddr, ioread32be(&osm_regs->oeddr));
	for (i = 0; i < 8; i++)
		dbg_print("\t [at %08x] OECR%d  = 0x%08x\n",
			  (uint32_t)&osm_regs->oecr[i], i,
			  ioread32be(&osm_regs->oecr[i]));
	dbg_print("\t [at %08x] OEUOMR = 0x%08x\n",
		  (uint32_t)&osm_regs->oeuomr, ioread32be(&osm_regs->oeuomr));
}

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
		(PARSER_IS_ICMP_DEFAULT() ? "ICMP" :
		(PARSER_IS_SCTP_DEFAULT() ? "SCTP" :"unknown"))));
#endif	/* PRINT_RX_PKT_INFO */

	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_FLAGS);
	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();
	pr_err("Failed to send frame\n");
	fdma_terminate_task();
}

/******************************************************************************/
static uint32_t get_stack_pointer(void)
{
	register uint32_t	sp;

	asm { stw rsp, sp; }
	return sp;
}

/******************************************************************************/
static uint32_t get_task(void)
{
	register uint32_t	task;

	asm { mfdcr  task, dcr476;	/* TASKCSR0 */ }
	/* C:\E_Content\LinuxSDK\AIOP-SL\AIOP - e200
	 * AIOP z490 CPU Specification rev1.8.pdf
	 *
	 * Bits 24:31 - TASKID : This field is provided for software use to
	 * determine the active Task ID. This value is a global value for the
	 * AIOP Tile, thus the most significant four bits are determined by
	 * core cluster ID (bits 24:25) and core within cluster ID (bits 26:27).
	 * The local task number is indicated in bits 28:31.
	 * This field is read-only. */
	return task;
}

/******************************************************************************/
static void print_rt_task_info(void)
{
	uint32_t	task, sp;

	sp = get_stack_pointer();
	task = get_task();
	fsl_print("\t cluster = %d core = %d id = %d\n",
		  (task >> 6) & 0x3, (task >> 4) & 0x3, task & 0xf);
	fsl_print("\t [SP = 0x%08x] Stack size %s\n", sp,
		  sp > 1 * 1024 && sp <= 2 * 1024 ? "2 kB" :
		  sp > 2 * 1024 && sp <= 4 * 1024 ? "4 kB" :
		  sp > 4 * 1024 && sp <= 8 * 1024 ? "8 kB" :
		  sp > 8 * 1024 && sp <= 16 * 1024 ? "16 kB" :
		  "32 kB");
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
	print_rt_task_info();
	/*dump_fd("BPSCN task", 2);
	dump_adq("BPSCN task");
	dump_prc("BPSCN task");
	dump_default_task_params("BPSCN task");
	dump_osm_registers("BPSCN task");*/
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
	/*dump_fd("Process task", 0);
	dump_adq("Process task");
	dump_prc("Process task");
	dump_default_task_params("Process task");
	dump_osm_registers("Process task");*/
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
	/*print_rt_task_info();
	dump_fd("CSCN task", 2);
	dump_adq("CSCN task");
	dump_prc("CSCN task");
	dump_default_task_params("CSCN task");
	dump_osm_registers("CSCN task");
	cscn_iova_print(dpni_cscn_iova);*/
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
		fdma_xon(1, dpni_fqid, 1, 0x0018, 0);
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
		/*dump_adq("RX task");*/
		if (task_get_receive_niid() == 1)
			fdma_xoff(1, dpni_fqid, 1, 0x0018, 0);
	}
	atomic_incr64(&num_tasks, 1);
	/* Send the packet and do not terminate the task. Buffer depletion
	 * should't occur */
	err = dpni_drv_send(task_get_receive_niid(), 0);
	if (err == -ENOMEM)
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
	else /* (err == -EBUSY) */
		ARCH_FDMA_DISCARD_FD();
	/*fsl_print(" tasks = %ll\n", num_tasks);*/
	/*print_rt_task_info();*/
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
				fdma_xon(1, dpni_fqid, 1, 0x0018, 0);
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

/* Make all following data go into DDR */
/******************************************************************************/
#pragma push
#pragma section data_type ".exception_data"
static uint8_t tmp_buf[256] = {0};
#pragma pop

/******************************************************************************/
static int app_dpni_add_cb(uint8_t generator_id, uint8_t event_id,
			   uint64_t app_ctx, void *event_data)
{
	struct dpkg_profile_cfg kg_cfg;
	uint16_t		dpni_id, ni = (uint16_t)((uint32_t)event_data);
	uint8_t			mac_addr[NET_HDR_FLD_ETH_ADDR_SIZE];
	char			dpni_ep_type[16];
	int			dpni_ep_id, err, link_state;
	struct ep_init_presentation init_presentation;
	struct			dpni_drv_qos_tbl qos_cfg = {0};
	struct			dpni_drv_qos_rule qos_rule = {0};
	uint16_t		mfl = 0x2000; /* Maximum Frame Length */
	dpni_drv_attr		attr;

	UNUSED(generator_id);
	UNUSED(event_id);
	/* Get endpoint and skip DPNI-DPNI connection from app initialization */
	err = dpni_drv_get_connected_obj(ni, &dpni_ep_id,
					 dpni_ep_type, &link_state);
	if (err) {
		pr_err("Cannot get connected object for NI %d\n", ni);
		return err;
	}

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
	if (ni == 1) {
		uint16_t qos_tbl_idx = 0;
		struct dpni_drv_rx_tc_policing_cfg cfg;

		/* QoS settings */
		err = dpni_drv_get_attributes(ni, &attr);
		if (err) {
			pr_err("Cannot get attributes for ni:%d\n", ni);
			return err;
		}
		if (attr.num_rx_tcs < 4) {
			pr_err("We need at least 4 TCs for ni:%d\n", ni);
			return err;
		}

		/* Sets the Rx priorities for TCs.
		 * Lower index TCs take precedence over higher index TCs*/
		err = dpni_drv_set_rx_priorities(ni);
		if (err) {
			pr_err("Fail to configure RX FQs priorities for NI %d\n", ni);
			return err;
		}

		/* Initialize key format for the QoS table */
		app_fill_qos_kg_profile(&kg_cfg);

		memset(tmp_buf, 0, 256);
		err = dpni_drv_prepare_key_cfg(&kg_cfg, tmp_buf);
		if (err) {
			pr_err("Cannot prepare key_cfg\n", ni);
			return err;
		}

		qos_cfg.key_cfg_iova = (uint64_t)tmp_buf;
		qos_cfg.discard_on_miss = 0;
		/* lowest priority */
		qos_cfg.default_tc = attr.num_rx_tcs - 1;
		err = dpni_drv_set_qos_table(ni, &qos_cfg);
		if (err) {
			pr_err("Cannot set qos table on NI %d\n", ni);
			return err;
		}

		/* Classification rules */
		/* TCP is TC0 (high priority 0) */
		qos_rule.key_iova = (uint64_t)tmp_buf;
		qos_rule.mask_iova = NULL;
		qos_rule.key_size = 1;
		tmp_buf[0] = 0x6;
		err = dpni_drv_add_qos_entry(ni, &qos_rule, 0, qos_tbl_idx++);
		if (err) {
			pr_err("Cannot add 1st qos entry on NI %d\n", ni);
			return err;
		}

		/* UDP is TC1 (high priority 1) */
		tmp_buf[0] = 0x11;
		err = dpni_drv_add_qos_entry(ni, &qos_rule, 1, qos_tbl_idx++);
		if (err) {
			pr_err("Cannot add 2nd qos entry on NI %d\n", ni);
			return err;
		}

		/* SCTP is TC2 (medium priority) */
		tmp_buf[0] = 0x84;
		err = dpni_drv_add_qos_entry(ni, &qos_rule, 2, qos_tbl_idx++);
		if (err) {
			pr_err("Cannot add 3rd qos entry on NI %d\n", ni);
			return err;
		}

		/* Policing configuration */
		/* Discard SCTP */
		cfg.options = DPNI_DRV_POLICER_OPT_COLOR_AWARE |
				DPNI_DRV_POLICER_OPT_DISCARD_RED;
		cfg.mode = DPNI_DRV_POLICER_MODE_PASS_THROUGH;
		cfg.unit = DPNI_DRV_POLICER_UNIT_PACKETS;
		cfg.default_color = DPNI_DRV_POLICER_COLOR_RED;
		cfg.cir = 0x1000;
		cfg.cbs = 0x800;
		cfg.eir = 0x1000;
		cfg.ebs = 0x800;
		err = dpni_drv_set_rx_tc_policing(ni, 2, &cfg);
		if (err) {
			pr_err("Cannot configure policer on NI %d\n", ni);
			return err;
		}
	}
	/* Egress QoS configuration */
	if (ni == 1) {
		uint8_t				tc;
		struct dpni_drv_tx_shaping	cr;
		struct dpni_drv_tx_shaping	er;

		/* QoS statistics */
		tc = 0;
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

/******************************************************************************/
int app_early_init(void)
{
	int	err;

#if (!defined(CSCN_CONGESTION_TEST) && !defined(BPSCN_DEPLETION_TEST))
	err = allocate_statistics_timer();
	return err;
#endif
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
#endif
#if (!defined(CSCN_CONGESTION_TEST) && !defined(BPSCN_DEPLETION_TEST))
	/* Register DPNI events */
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t)app_frame_cb, app_dpni_add_cb);
#endif
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

/******************************************************************************/
void app_free(void)
{}
