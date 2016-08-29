/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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
#include "common/fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_platform.h"
#include "kernel/fsl_io.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"
#include "fsl_cmdif_server.h"
#include "fsl_cdma.h"
#include "fsl_ipsec.h"
#include "fsl_malloc.h"
#include "lib/fsl_slab.h"
#include "system.h" // TMP
#include "fsl_evmng.h"
#include "apps.h"
#include "fsl_tman.h"

int app_early_init(void);
int app_init(void);
void app_free(void);
int ipsec_app_init(uint16_t ni_id);
void ipsec_print_frame(void);
static void ipsec_print_stats(ipsec_handle_t desc_handle, uint8_t is_encap);
void ipsec_print_sp (uint16_t ni_spid);
void user_lifetime_callback(uint64_t opaque1, uint8_t opaque2);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

//#define IPSEC_DEBUG_PRINT_SP
#ifdef IPSEC_DEBUG_PRINT_SP
extern __PROFILE_SRAM struct storage_profile
		storage_profile[SP_NUM_OF_STORAGE_PROFILES];
#endif

/*******************************************************************************
/* Performance measurement case. Define PERF_MEASUREMENT in order to measure
 * the IPsec performances
 ******************************************************************************/
/*#define PERF_MEASUREMENT*/

/* Set ENCRYPT_ONLY to 1 in order the application performs only packet
 * encryption */
#define ENCRYPT_ONLY		0

/* Set DECRYPT_ONLY to 1 in order the application performs only packet
 * decryption */
#define DECRYPT_ONLY		0

/* If none of above macros is defined as 1 application performs packet
 * encryption followed by decryption */

/*******************************************************************************
 * Number of created SA pairs
 ******************************************************************************/
#ifdef PERF_MEASUREMENT
	#define TEST_NUM_OF_SA		64 /*1*/ /*6*/
#else
	/* Only 1 SA pair must be created */
	#define TEST_NUM_OF_SA		1
#endif

/*******************************************************************************
 * SA control parameters configuration
 ******************************************************************************/
/* 1 - Cipher and authentication algorithms */
enum app_key_types {
	NULL_MD5 = 0,
	NULL_SHA512,
	AES128_SHA1,
	AES128_SHA256,
	AES128_SHA512,
	AES128_NULL
};
/* Use a value from the above enumeration */
#define CIPHER_AUTH_ALGS	AES128_SHA1

/* 2 - Tunnel/Transport mode */
/* IPSEC_FLG_TUNNEL_MODE - Tunnel, 0 - Transport */
#define TUNEL_TRANSPORT_MODE	IPSEC_FLG_TUNNEL_MODE

/* 3 - Buffer mode */
/* 0 - New Buffer Mode,  IPSEC_FLG_BUFFER_REUSE - Buffer Reuse Mode */
#define BUFFER_MODE		IPSEC_FLG_BUFFER_REUSE

/* 4 - IP version */
/* 0 - IPv4,  IPSEC_OPTS_ESP_IPVSN - IPv6*/
#define IP_VERSION	0
/* Define IP source address offset as ETH header size plus the offset of the
 * least significant 4 bytes of the IP source address */
#if (IP_VERSION == 0)	/* IPv4 */
	#define IP_SRC_OFF	(14 + 12)
#else			/* IPv6 */
	#define IP_SRC_OFF	(14 + 20)
#endif

/* 5 - ENC/DEC lifetime timers */
/* 0 - Disabled, 1 - Enabled */
#ifndef PERF_MEASUREMENT
	#define LIFETIME_TIMERS_ENABLE	1
#else
	#define LIFETIME_TIMERS_ENABLE	0
#endif
#if (LIFETIME_TIMERS_ENABLE == 1)
	/* Seconds lifetime duration. A non-zero value must be larger than 10.
	Soft and hard pairs must have valid values : hard_seconds >=
	soft_seconds. No check is done. In order to run the application with
	this feature enabled, and without SA expiration, define the values as
	0xffffffff */
	#define ENCAP_SOFT_SECONDS	0xffffffff
	#define ENCAP_HARD_SECONDS	0xffffffff
	#define DECAP_SOFT_SECONDS	0xffffffff
	#define DECAP_HARD_SECONDS	0xffffffff
#ifdef TEST
	/* Values used to test SA expiration */
	#define ENCAP_SOFT_SECONDS	15
	#define ENCAP_HARD_SECONDS	16
	#define DECAP_SOFT_SECONDS	17
	#define DECAP_HARD_SECONDS	18
#endif
#endif

/* 6 - ENC/DEC Kilobytes limit */
/* 0 - Disabled, 1 - Enabled */
#ifndef PERF_MEASUREMENT
	#define KB_LIMIT_ENABLE		1
#else
	#define KB_LIMIT_ENABLE		0
#endif
#if (KB_LIMIT_ENABLE == 1)
	/* Soft and hard pairs must have valid values : hard_kb_limit >=
	soft_kb_limit. No check is done. In order to run the application with
	this feature enabled, and without SA expiration, define the values as
	0xffffffffffffffff */
	#define ENCAP_SOFT_KB_LIMIT	0xffffffffffffffff
	#define ENCAP_HARD_KB_LIMIT	0xffffffffffffffff
	#define DECAP_SOFT_KB_LIMIT	0xffffffffffffffff
	#define DECAP_HARD_KB_LIMIT	0xffffffffffffffff
#ifdef TEST
	/* Values used to test SA expiration */
	#define ENCAP_SOFT_KB_LIMIT	(64 * 1024)
	#define ENCAP_HARD_KB_LIMIT	(65 * 1024)
	#define DECAP_SOFT_KB_LIMIT	(66 * 1024)
	#define DECAP_HARD_KB_LIMIT	(67 * 1024)
#endif
#endif

/* 7 - ENC/DEC packets limit*/
/* 0 - Disabled, 1 - Enabled */
#ifndef PERF_MEASUREMENT
	#define PKT_LIMIT_ENABLE	1
#else
	#define PKT_LIMIT_ENABLE	0
#endif
#if (PKT_LIMIT_ENABLE == 1)
	/* Soft and hard pairs must have valid values : hard_pkt_limit >=
	 * soft_pkt_limit. No check is done. In order to run the application
	 * with this feature enabled, and without SA expiration, define the
	 * values as 0xffffffffffffffff */
	#define ENCAP_SOFT_PKT_LIMIT	0xffffffffffffffff
	#define ENCAP_HARD_PKT_LIMIT	0xffffffffffffffff
	#define DECAP_SOFT_PKT_LIMIT	0xffffffffffffffff
	#define DECAP_HARD_PKT_LIMIT	0xffffffffffffffff
#ifdef TEST
	/* Values used to test SA expiration */
	#define ENCAP_SOFT_PKT_LIMIT	(64 * 1024)
	#define ENCAP_HARD_PKT_LIMIT	(65 * 1024)
	#define DECAP_SOFT_PKT_LIMIT	(66 * 1024)
	#define DECAP_HARD_PKT_LIMIT	(67 * 1024)
#endif
#endif

/* 8 - Extended Sequence Number*/
/* IPSEC_OPTS_ESP_ESN - Enabled, 0 - Disabled */
#define ESN_ENABLE	0
/* Usual values */
#define SEQNO_L		0x1
#define SEQNO_H		0x0
/* For testing purposes
#define SEQNO_L		0xfffffffe
#define SEQNO_H		0xffffffff
*/

/* 9 - ENCAP Sequence Number Roll Over*/
/* 0 - Disabled, IPSEC_ENC_OPTS_SNR_EN - Enabled */
#define ENCAP_ROLL_ENABLE	0

/* 10 - DECAP anti-replay window size */
/* IPSEC_DEC_OPTS_ARSNONE - Disabled, ARS32/64/128 - Enabled */
#if (DECRYPT_ONLY == 1)
	/* Disable ARW in the Decrypt only case */
	#define DECAP_ARW_SIZE	IPSEC_DEC_OPTS_ARSNONE
#else
	#define DECAP_ARW_SIZE	IPSEC_DEC_OPTS_ARS32
#endif

/* 11 - DECAP padding check */
/* 0 - Disabled,  IPSEC_FLG_TRANSPORT_PAD_CHECK - Enabled */
#define DECAP_PAD_CHECK		0

/*******************************************************************************
 * Global IPsec variables in Shared RAM
 ******************************************************************************/
ipsec_instance_handle_t ipsec_instance_handle;

ipsec_handle_t	ipsec_sas_desc_outbound[TEST_NUM_OF_SA];
ipsec_handle_t	ipsec_sas_desc_inbound[TEST_NUM_OF_SA];

/*******************************************************************************
/* Performance measurement case. Define PERF_MEASUREMENT in order to enable it.
 ******************************************************************************/
#ifdef PERF_MEASUREMENT
/* Better performances are obtained if counters are disabled : see
 * KB_LIMIT_ENABLE (data path processing), PKT_LIMIT_ENABLE  (data path
 * processing) and LIFETIME_TIMERS_ENABLE (expiration task occurs, hence
 * less data path processing task, concurrence while accessing DDR in order
 * to update the timers). */

/* Set IPSEC_DEBUG_PRINT to 1 in order have printed messages */
#define IPSEC_DEBUG_PRINT	0

/* Periodically statistics print */
#define IPSEC_STATS_PRINT	0
#if (IPSEC_STATS_PRINT == 1)
	/* Statistics timer duration. Must be greater than 10 */
	#define STATS_TIMER_PERIOD	11	/* seconds */

	/* STATS_PRINT_FROM_SA < TEST_NUM_OF_SA */
	#define STATS_PRINT_FROM_SA	0
	/* STATS_PRINT_TO_SA <= TEST_NUM_OF_SA and
	 * STATS_PRINT_TO_SA > STATS_PRINT_FROM_SA */
	#define STATS_PRINT_TO_SA	1

	static void app_stats_timer_cb(tman_arg_8B_t arg1, tman_arg_2B_t arg2);

	uint32_t	stats_timer_handle;
#endif

__HOT_CODE ENTRY_POINT static void app_perf_process_packet(void)
{
	int		err;
	uint32_t	status = 0;
	uint8_t		*eth_pointer_byte;
	uint32_t	sa_idx;
#if (DECRYPT_ONLY == 0)
	ipsec_handle_t	ws_desc_handle_outbound;
#endif
#if (ENCRYPT_ONLY == 0)
	ipsec_handle_t	ws_desc_handle_inbound;
#endif

	sl_prolog();
	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();

	/* IP_SRC based distribution */
	sa_idx = (*((uint32_t *)(eth_pointer_byte + IP_SRC_OFF))) %
							TEST_NUM_OF_SA;
#if (DECRYPT_ONLY == 0)
	ws_desc_handle_outbound = ipsec_sas_desc_outbound[sa_idx];
#endif
#if (ENCRYPT_ONLY == 0)
	ws_desc_handle_inbound = ipsec_sas_desc_inbound[sa_idx];
#endif

#if (DECRYPT_ONLY == 0)
	err = ipsec_frame_encrypt(ws_desc_handle_outbound, &status);
	if (err) {
#if (IPSEC_DEBUG_PRINT == 1)
		fsl_print("SEC Encryption Failed (status = 0x%08x)\n", status);
#endif
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		fdma_terminate_task();
	}
#endif
#if (ENCRYPT_ONLY == 0)
	err = ipsec_frame_decrypt(ws_desc_handle_inbound, &status);
	if (err) {
#if (IPSEC_DEBUG_PRINT == 1)
		fsl_print("SEC Decryption Failed (status = 0x%08x)\n", status);
#endif
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		fdma_terminate_task();
	}
#endif
	err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);
	if (err) {
#if (IPSEC_DEBUG_PRINT == 1)
		fsl_print("ERROR = %d: dpni_drv_send(ni_id)\n", err);
#endif
		if (err == -ENOMEM)
			fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		else /* (err == -EBUSY) */
			fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS,
					FDMA_DIS_NO_FLAGS);
	}
	fdma_terminate_task();
}

#else

__HOT_CODE ENTRY_POINT static void app_process_packet(void)
{
	int      err = 0;
	uint32_t enc_status = 0;
	uint32_t dec_status = 0;
	int i = 0;
	uint8_t frame_before_encr[256] = {0};
	uint8_t *eth_pointer_byte = 0;
	uint32_t handle_high, handle_low;
	int local_test_error = 0;
	uint32_t original_frame_len;

	sl_prolog();

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	uint32_t frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	original_frame_len = frame_len;
	uint16_t seg_len = PRC_GET_SEGMENT_LENGTH();
	uint16_t original_seg_addr = PRC_GET_SEGMENT_ADDRESS();

	ipsec_handle_t ws_desc_handle_outbound = ipsec_sas_desc_outbound[0];
	ipsec_handle_t ws_desc_handle_inbound = ipsec_sas_desc_inbound[0];

	fsl_print("IPsec Demo: Core %d Received Frame\n", core_get_id());

	handle_high =
			(uint32_t)((ws_desc_handle_outbound & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ws_desc_handle_outbound & 0x00000000ffffffff);
	fsl_print("Encryption handle = 0x%x_%x\n", handle_high, handle_low);

	handle_high =
			(uint32_t)((ws_desc_handle_inbound & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ws_desc_handle_inbound & 0x00000000ffffffff);
	fsl_print("Decryption handle = 0x%x_%x\n", handle_high, handle_low);

	/* preserve original frame */
	for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
	{
		frame_before_encr[i] =  *eth_pointer_byte;
		eth_pointer_byte++;
	}

	fsl_print("IPSEC: frame header before encryption\n");
	/* Print header */
	ipsec_print_frame();

	fsl_print("\n");

	fsl_print("IPSEC: Starting Encryption\n");
	err = ipsec_frame_encrypt(
			ws_desc_handle_outbound,
			&enc_status
			);

	if (err)
	{
		fsl_print("ERROR: ipsec_frame_encrypt() failed\n");
		local_test_error |= err;
	}
	else
		fsl_print("ipsec_frame_encrypt() completed successfully\n");

	if (enc_status)
	{
		fsl_print("ERROR: SEC Encryption Failed (enc_status = 0x%x)\n",
				enc_status);
		local_test_error |= enc_status;
	}

	if (!local_test_error) {
		/* close-open segment, for the print */
		fdma_close_default_segment();
		fdma_present_default_frame_segment(
				0, (void *)PRC_GET_SEGMENT_ADDRESS(), 0, seg_len);

		fsl_print("IPSEC: frame header after encryption\n");
		/* Print header */
		ipsec_print_frame();
		fsl_print("\n");

		fsl_print("IPSEC: Starting Decryption\n");
		
		err = ipsec_frame_decrypt(
			ws_desc_handle_inbound,
			&dec_status
			);
		
		if (err) {
			fsl_print("ERROR: ipsec_frame_decrypt() failed\n");
			local_test_error |= err;
		} 		else
		fsl_print("ipsec_frame_decrypt() completed successfully\n");

		if (dec_status) {
			fsl_print("ERROR: SEC Decryption Failed (dec_status = 0x%x)\n",
				dec_status);
			local_test_error |=dec_status;
		}
	}
	
	
	if (!local_test_error) {
		/* Due to the parser unaligned segment WA, represent again */
		fdma_close_default_segment();

		err = fdma_present_default_frame_segment(
    		FDMA_PRES_NO_FLAGS, /* uint32_t flags */
    		(void *)original_seg_addr, /* void *ws_dst */
    		0, /* uint16_t offset */
    		seg_len); /* uint16_t present_size */

		fsl_print("STATUS: fdma_present_default_frame_segment returned %d\n", err);

		fsl_print("IPSEC: frame header after decryption\n");
		/* Print header */
		ipsec_print_frame();
		fsl_print("\n");

		/* Compare decrypted frame to original frame */
		err = 0;
		eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
		frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

		if (frame_len != original_frame_len) {
			fsl_print("ERROR: incorrect frame length (FD[length] = %d)\n",
				frame_len);
			err = 1;
			local_test_error |= err;
		} else {
			for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
			{
				if (frame_before_encr[i] !=  *eth_pointer_byte) {
					fsl_print(
						"ERROR: frame after decryption differ from origin\n");
					err = 1;
					local_test_error |= err;
					break;
				}
				eth_pointer_byte++;
			}
		}
	
		/* Print statistics */
		ipsec_print_stats(ws_desc_handle_outbound, 1);
		ipsec_print_stats(ws_desc_handle_inbound, 0);

		fsl_print("IPsec Demo: Core %d Sending Frame\n", core_get_id());

		err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);
		if (err){
			fsl_print("ERROR = %d: dpni_drv_send(ni_id)\n",err);
			local_test_error |= err;
			if(err == -ENOMEM)
				fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			else /* (err == -EBUSY) */
				fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS, FDMA_DIS_NO_FLAGS);
		}
	} 
	
	if(!local_test_error) /* No error found during injection of packets*/
	{
		fsl_print("Finished SUCCESSFULLY\n");
		fsl_print("\nFrame after decryption the same as origin\n\n");
		fsl_print("IPsec Demo: Done Sending Frame\n\n");
	}
	else
		fsl_print("Finished with ERRORS\n");

	/*MUST call fdma_terminate task in the end of cb function*/
	fdma_terminate_task();
}

#endif		/* PERF_MEASUREMENT */

#ifdef AIOP_STANDALONE
/* This is temporal WA for stand alone demo only */
#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_MOD_CMGW,            \
	                                   0,                          \
	                                   E_MAPPED_MEM_TYPE_GEN_REGS) \
	                                   + SOC_PERIPH_OFF_AIOP_WRKS);
static void epid_setup()
{
	struct aiop_ws_regs *wrks_addr = (struct aiop_ws_regs *)WRKS_REGS_GET;

	/* EPID = 0 is saved for cmdif, need to set it for stand alone demo */
	iowrite32(0, &wrks_addr->epas);
	iowrite32((uint32_t)receive_cb, &wrks_addr->ep_pc);
}
#endif /* AIOP_STANDALONE */

static int open_cb(uint8_t instance_id, void **dev)
{
	UNUSED(dev);
	fsl_print("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	fsl_print("close_cb\n");
	return 0;
}

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size, void *data)
{
	UNUSED(dev);
	UNUSED(size);
	UNUSED(data);
	fsl_print("ctrl_cb cmd = 0x%x, size = %d, data 0x%x\n",
	             cmd,
	             size,
	             (uint32_t)data);
	return 0;
}

static struct cmdif_module_ops ops = {
                               .open_cb = open_cb,
                               .close_cb = close_cb,
                               .ctrl_cb = ctrl_cb
};

static uint16_t get_buffer_size(uint16_t *timers)
{
	uint16_t	buff_size = 0, num_timers = 3;

#if (IPSEC_STATS_PRINT == 1)
	num_timers++;
#endif

#if (LIFETIME_TIMERS_ENABLE == 1)
/* In this case the buff_size must be computed function of number of created
 * SAs. For each SA pair, 4 timers are created : 2 soft and 2 hard.
 * The number of created timers should be 3 timers larger than actually needed
 * and larger than 4.
 *
 * The memory region used to hold the timers must be of at least
 * 64 * (num_of_timers + 1) bytes, and 64 bytes aligned. */

	num_timers += 4 * TEST_NUM_OF_SA;
	buff_size = 64 * (num_timers + 1);
#endif
	if (timers)
		*timers = num_timers + 1;
	return (buff_size < 512) ? 512 : buff_size;
}

int app_early_init(void)
{
	int		err;
	uint16_t	num;

	/* IPsec resources reservation */
	err = ipsec_early_init(10, 2 * TEST_NUM_OF_SA, 2 * TEST_NUM_OF_SA, 0);
	if (err)
		return err;
	/* Reserve some general buffers for keys, timers, etc. */
	err = slab_register_context_buffer_requirements(10, 10,
							get_buffer_size(&num),
							8, IPSEC_DEMO_MEM_ID,
							0, 0);
	if (err)
		return err;
	fsl_print("Place for %d timers was reserved\n", num - 4);
	/* Set DHR to 256 in the default storage profile */
	err = dpni_drv_register_rx_buffer_layout_requirements(256, 0, 0);

#if (BUFFER_MODE != IPSEC_FLG_BUFFER_REUSE)
	if (g_app_params.app_config_flags & IPSEC_BUFFER_ALLOCATE_ENABLE) {
		/* Change the buffer size. There is no sufficient space to
		 * allocate the default number of buffers. */
		pr_warn("%s : Buffer size changed from %d to %d bytes !\n",
			__func__, g_app_params.dpni_buff_size,
			g_app_params.dpni_buff_size / 2);
		g_app_params.dpni_buff_size /= 2;
	} else {
		/* If the application did not configured the creation of the
		 * dedicated IPSec buffer pool, just print a warning messages.
		 * Buffers will be taken from the DPNI PEB BP. BP depletion
		 * occurs at high input traffic rates : packets are dropped by
		 * the input port. */
		pr_warn("%s : Buffer allocate mode not enabled !\n", __func__);
	}
#else
	if (g_app_params.app_config_flags & IPSEC_BUFFER_ALLOCATE_ENABLE) {
		g_app_params.app_config_flags &= ~IPSEC_BUFFER_ALLOCATE_ENABLE;
		pr_warn("%s : Buffer allocate enabled, forced to 0 !\n",
			__func__);
	}
#endif
	return err;
}

static int app_dpni_event_added_cb(
			uint8_t generator_id,
			uint8_t event_id,
			uint64_t app_ctx,
			void *event_data)
{
	uint16_t ni = (uint16_t)((uint32_t)event_data);
	uint16_t    mfl = 0x2000; /* Maximum Frame Length */
	int err;

	UNUSED(generator_id);
	UNUSED(event_id);
	pr_info("Event received for AIOP NI ID %d\n",ni);
	err = dpni_drv_register_rx_cb(ni/*ni_id*/,
	                              (rx_cb_t *)app_ctx);
	if (err){
		pr_err("dpni_drv_register_rx_cb for ni %d failed: %d\n", ni, err);
		return err;
	}
	err = dpni_drv_set_max_frame_length(ni/*ni_id*/,
	                                    mfl /* Max frame length*/);
	if (err){
		pr_err("dpni_drv_set_max_frame_length for ni %d failed: %d\n", ni, err);
		return err;
	}
	
	err = dpni_drv_enable(ni);
	if(err){
		pr_err("dpni_drv_enable for ni %d failed: %d\n", ni, err);
		return err;
	}
	
	if (!ni) {
		/* IPsec Initialization occurs once for the first NI */
		err = ipsec_app_init(0); /* Call with NI ID = 0 */
		if (err) {
			fsl_print("ERROR: IPsec initialization failed\n");
			return err;
		}
	}
	return 0;
}
int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	uint64_t buff = 0;

	fsl_print("Running app_init()\n");

#ifdef AIOP_STANDALONE
	/* This is temporal WA for stand alone demo only */
	epid_setup();
#endif /* AIOP_STANDALONE */

#ifdef PERF_MEASUREMENT
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t) app_perf_process_packet,
			     app_dpni_event_added_cb);
#else
	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t) app_process_packet,
			     app_dpni_event_added_cb);
#endif
	if (err){
		pr_err("EVM registration for DPNI_EVENT_ADDED failed: %d\n", err);
		return err;
	}


	err = cmdif_register_module("TEST0", &ops);
	if (err)
	{
		fsl_print("FAILED cmdif_register_module\n!");
		return err;
	}

	fsl_print("To start test inject packets: \"eth_ipv4_udp.pcap\"\n");
	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}

/* IPsec Initialization */
int ipsec_app_init(uint16_t ni_id)
{
	int err  = 0;
	struct ipsec_descriptor_params params;
	uint32_t outer_ip_header[10];
	struct slab *slab_handle = NULL;
	uint32_t handle_high, handle_low;
	int i;
	uint64_t cipher_key_addr;
	uint64_t auth_key_addr;
	uint8_t cipher_key[16] = "1122334455667788";
	/* 64 bytes */
	uint8_t auth_key[128];
	uint32_t cipher_alg;
	uint32_t cipher_keylen;
	uint32_t auth_alg;
	uint32_t auth_keylen;
	uint32_t algs;
	uint32_t outer_header_ip_version;
	uint16_t ni_spid;
	uint32_t tunnel_transport_mode;
	uint32_t reuse_buffer_mode;
	uint32_t lifetime_kb_cntr_en = 0;
	uint32_t lifetime_pkt_cntr_en = 0;
	uint32_t pad_check;
	uint32_t set_dscp = 0;
	uint16_t esn_enable;
	uint16_t roll_over_enable;
	uint16_t transport_ipvsn;
	uint32_t encap_soft_seconds = 0x0;
	uint32_t encap_hard_seconds = 0x0;
	uint32_t decap_soft_seconds = 0x0;
	uint32_t decap_hard_seconds = 0x0;
	ipsec_instance_handle_t ws_instance_handle = 0;
	ipsec_handle_t ws_desc_handle_outbound = 0;
	ipsec_handle_t ws_desc_handle_inbound = 0;
	int sa_index;
#if (LIFETIME_TIMERS_ENABLE == 1 || IPSEC_STATS_PRINT == 1)
	uint64_t tmi_buffer_handle, tmi_timer_addr;
#endif
	uint8_t		tmi_id = 0;
	uint16_t	num_timers;

	/**********************************************************/
	/*                    Control Parameters                  */
	/**********************************************************/
	/* Set the required algorithms here */
	algs = CIPHER_AUTH_ALGS;

	/* Set the outer IP header type here */
	outer_header_ip_version = 4; /* 4 or 6 */
	//outer_header_ip_version = 17; /* UDP encap - Tunnel mode */
	//outer_header_ip_version = 18; /* UDP encap - Transport mode */
	
	/* "01234567..." */
	for (i = 0; i < 128; i++)
		auth_key[i] = (uint8_t)(0x30 + i % 8);

	tunnel_transport_mode = TUNEL_TRANSPORT_MODE;

	/* DSCP setting, valid only for tunnel mode */
	if (tunnel_transport_mode) {
		//set_dscp = IPSEC_FLG_ENC_DSCP_SET; /* Set DSCP in outer header */
		set_dscp = 0; /* Copy DSCP from inner to outer header */
	}

	esn_enable = ESN_ENABLE;
	roll_over_enable = ENCAP_ROLL_ENABLE;
	reuse_buffer_mode = BUFFER_MODE;
	transport_ipvsn = IP_VERSION;
	
#if (LIFETIME_TIMERS_ENABLE == 1)
	encap_soft_seconds = ENCAP_SOFT_SECONDS;
	encap_hard_seconds = ENCAP_HARD_SECONDS;
	decap_soft_seconds = DECAP_SOFT_SECONDS;
	decap_hard_seconds = DECAP_HARD_SECONDS;
#endif

	fsl_print("\n++++\n  IPsec Demo: Doing IPsec Initialization\n++++\n");

	if (tunnel_transport_mode == IPSEC_FLG_TUNNEL_MODE) {
		fsl_print("IPsec Demo: Tunnel Mode\n");
	} else {
		fsl_print("IPsec Demo: Transport Mode\n");
	}

	if (reuse_buffer_mode == IPSEC_FLG_BUFFER_REUSE) {
		fsl_print("IPsec Demo: Reuse Buffer Mode\n");
	} else {
		fsl_print("IPsec Demo: New Buffer Mode\n");
	}


	dpni_drv_get_spid(
		ni_id, /* uint16_t ni_id */
		&ni_spid /* uint16_t *spid */
		);

	fsl_print("IPsec Demo: SPID = %d\n", ni_spid);

#ifdef IPSEC_DEBUG_PRINT_SP
	ipsec_print_sp (ni_spid);
#endif

	/* Allocate buffers for the keys, timers, etc */
	err = slab_create(10, 10, get_buffer_size(&num_timers), 8,
			  IPSEC_DEMO_MEM_ID, 0, NULL, &slab_handle);
	if (err) {
		fsl_print("ERROR: slab_create() failed\n");
		return err;
	}
	fsl_print("slab_create() completed successfully\n");
	fsl_print("Place for %d timers was created\n", num_timers - 4);
	/* Acquire the Cipher key buffer */
	err = slab_acquire(slab_handle, &cipher_key_addr);
	if (err) {
		fsl_print("ERROR: slab_acquire() failed\n");
		return err;
	}
	fsl_print("slab_acquire() completed successfully\n");
	/* Acquire the Authentication key buffer */
	err = slab_acquire(slab_handle, &auth_key_addr);
	if (err) {
		fsl_print("ERROR: slab_acquire() failed\n");
		return err;
	}
	fsl_print("slab_acquire() completed successfully\n");
	handle_high = (uint32_t)((auth_key_addr & 0xffffffff00000000)>>32);
	handle_low = (uint32_t)(auth_key_addr & 0x00000000ffffffff);
	fsl_print("Auth key addr = 0x%x_%x\n", handle_high, handle_low);
	/* Create a TMI instance only if it is needed */
#if (LIFETIME_TIMERS_ENABLE == 1 || IPSEC_STATS_PRINT == 1)
	/* Acquire the TMan buffer */
	err = slab_acquire(slab_handle, &tmi_buffer_handle);
	/* TODO Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
	#define IPSEC_ALIGN_64(ADDRESS, ALIGNMENT) \
		((((uint64_t)(ADDRESS)) + ((uint64_t)(ALIGNMENT)) - 1) & \
		(~(((uint64_t)(ALIGNMENT)) - 1)))
	/* Align to 64 bytes */
	tmi_timer_addr = IPSEC_ALIGN_64(tmi_buffer_handle, 64);
	err = tman_create_tmi(tmi_timer_addr, num_timers, &tmi_id);
	if (err) {
		fsl_print("%d ERROR: tman_create_tmi() failed\n", err);
		return err;
	}
	fsl_print("IPsec: tmi_id = 0x%x created for %d timers\n",
		  tmi_id, num_timers - 4);
#endif
	err = ipsec_create_instance(2 * TEST_NUM_OF_SA, 2 * TEST_NUM_OF_SA,
				    0, tmi_id, &ws_instance_handle);
	if (err) {
		fsl_print("[%d] ERROR: ipsec_create_instance() failed\n", err);
		return err;
	}
	fsl_print("ipsec_create_instance() completed successfully\n");

	ipsec_instance_handle = ws_instance_handle;
	
	switch (algs) {
		case NULL_MD5:
			fsl_print("Cipher Algorithm: IPSEC_CIPHER_NULL\n");
			fsl_print("Authentication Algorithm: IPSEC_AUTH_HMAC_MD5_96\n");
			cipher_alg = IPSEC_CIPHER_NULL;
			cipher_keylen = 0x0;
			auth_alg = IPSEC_AUTH_HMAC_MD5_96;
			auth_keylen = 16;
			break;
		case NULL_SHA512:
			fsl_print("Cipher Algorithm: IPSEC_CIPHER_NULL\n");
			fsl_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA2_512_256\n");
			cipher_alg = IPSEC_CIPHER_NULL;
			cipher_keylen = 0x0;
			auth_alg = IPSEC_AUTH_HMAC_SHA2_512_256;
			auth_keylen = 64;
			break;
		case AES128_SHA1:
			fsl_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA1_96\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_SHA1_96;
			auth_keylen = 20;
			break;
		case AES128_SHA256:
			fsl_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA2_256_128\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_SHA2_256_128;
			//auth_keylen = 64;
			auth_keylen = 32;
			break;
		case AES128_SHA512:
			fsl_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA2_512_256\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_SHA2_512_256;
			auth_keylen = 64;
			break;
		case AES128_NULL:
			fsl_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_print("Authentication Algorithm: IPSEC_AUTH_HMAC_NULL\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_NULL;
			auth_keylen = 0;
			break;
		default:
			fsl_print("Cipher Algorithm (default): IPSEC_CIPHER_NULL\n");
			fsl_print("Authentication Algorithm (default): IPSEC_AUTH_HMAC_MD5_96\n");
			cipher_alg = IPSEC_CIPHER_NULL;
			cipher_keylen = 0x0;
			auth_alg = IPSEC_AUTH_HMAC_MD5_96;
			auth_keylen = 16;
	}

	/* Copy the Keys to external memory with CDMA */
	cdma_write(
			cipher_key_addr, /* ext_address */
			&cipher_key, /* ws_src */
			16); /* uint16_t size */

	cdma_write(
			auth_key_addr, /* ext_address */
			&auth_key, /* ws_src */
			(uint16_t)auth_keylen); /* uint16_t size */

	/* Create all SAs */
	for (sa_index = 0; sa_index < TEST_NUM_OF_SA; sa_index++) {
		/* Outbound (encryption) parameters */
		if (outer_header_ip_version == 4) {
			/* Outer IPv4 header */
			outer_ip_header[0] = 0x45db0014;
			outer_ip_header[1] = 0x12340000;
			outer_ip_header[2] = 0xff32386f;
			outer_ip_header[3] = 0x45a4e14c;
			outer_ip_header[4] = 0xed035c45;
			 /* Outer header length is 20 bytes */
			params.encparams.ip_hdr_len = 0x14;
		} else if (outer_header_ip_version == 6) {
			/* Outer IPv6 header */
			outer_ip_header[0] = 0x6db00000;
			outer_ip_header[1] = 0x000032ff;
			outer_ip_header[2] = 0xfe800000;
			outer_ip_header[3] = 0x00000000;
			outer_ip_header[4] = 0x021125ff;
			outer_ip_header[5] = 0xfe8295b5;
			outer_ip_header[6] = 0xff020000;
			outer_ip_header[7] = 0x00000000;
			outer_ip_header[8] = 0x00000001;
			outer_ip_header[9] = 0xff8295b5;
			 /* Outer header length is 40 bytes */
			params.encparams.ip_hdr_len = 0x28;
		} else if (outer_header_ip_version == 17) {
			/* Outer UDP ENCAP header */
			outer_ip_header[0] = 0x45db001c;
			outer_ip_header[1] = 0x12340000;
			outer_ip_header[2] = 0xff11386f;
			outer_ip_header[3] = 0x45a4e14c;
			outer_ip_header[4] = 0xed035c45;
			outer_ip_header[5] = 0x11941194;
			outer_ip_header[6] = 0x00000000;
			 /* Outer header length is 28 bytes */
			params.encparams.ip_hdr_len = 0x1c;
		} else if (outer_header_ip_version == 18) {
			/* UDP transport mode encap */
			outer_ip_header[0] = 0x11941194;
			outer_ip_header[1] = 0x00000000;
		}
		if (tunnel_transport_mode == 0) {
			/* Transport Mode */
			params.encparams.ip_hdr_len = 0;
		}
		params.direction = IPSEC_DIRECTION_OUTBOUND;
		/* Miscellaneous control flags */
		params.flags = tunnel_transport_mode |
				IPSEC_FLG_LIFETIME_KB_CNTR_EN |
				IPSEC_FLG_LIFETIME_PKT_CNTR_EN |
				set_dscp | reuse_buffer_mode;
		if (outer_header_ip_version == 18)
			params.flags |= IPSEC_ENC_OPTS_NAT_EN;
		if (encap_soft_seconds | encap_hard_seconds) {
			params.flags |= IPSEC_FLG_LIFETIME_SEC_CNTR_EN;
			fsl_print("ENCAP lifetime : soft = %u, hard = %u\n",
				  encap_soft_seconds, encap_hard_seconds);
		}
		/* UDP ENCAP (tunnel mode)*/
		if (outer_header_ip_version == 17 &&
		    tunnel_transport_mode == IPSEC_FLG_TUNNEL_MODE) {
			params.flags |= IPSEC_ENC_OPTS_NAT_EN;
			params.flags |= IPSEC_ENC_OPTS_NUC_EN;
			fsl_print("IPSEC: Tunnel Mode UDP Encapsulation\n");
		}
		params.encparams.options = roll_over_enable | esn_enable |
							transport_ipvsn;
		/* Sequence Number */
		params.encparams.seq_num_ext_hi = SEQNO_H;
		params.encparams.seq_num = SEQNO_L;
		params.encparams.spi = 0x1234;
		params.encparams.outer_hdr = (uint32_t *)&outer_ip_header;
		/* Clear IV */
		for (i = 0; i < sizeof(params.encparams.cbc.iv); i++)
			params.encparams.cbc.iv[i] = 0;
		/* Cipher algorithm parameters */
		params.cipherdata.algtype = cipher_alg;
		params.cipherdata.key = cipher_key_addr;
		params.cipherdata.keylen = cipher_keylen;
		params.cipherdata.key_enc_flags = 0x0;
		/* Authentication algorithm parameters */
		params.authdata.algtype = auth_alg;
		params.authdata.key = auth_key_addr;
		params.authdata.keylen = auth_keylen;
		params.authdata.key_enc_flags = 0x0;
		/* Bytes/Packets limits */
#if (KB_LIMIT_ENABLE == 1)
		params.soft_kilobytes_limit = ENCAP_SOFT_KB_LIMIT;
		params.hard_kilobytes_limit = ENCAP_HARD_KB_LIMIT;
		lifetime_kb_cntr_en = IPSEC_FLG_LIFETIME_KB_CNTR_EN;
#endif
#if (PKT_LIMIT_ENABLE == 1)
		params.soft_packet_limit = ENCAP_SOFT_PKT_LIMIT;
		params.hard_packet_limit = ENCAP_HARD_PKT_LIMIT;
		lifetime_pkt_cntr_en = IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
#endif
		/* Lifetime limits */
#if (LIFETIME_TIMERS_ENABLE == 1)
		params.soft_seconds_limit = encap_soft_seconds;
		params.hard_seconds_limit = encap_hard_seconds;
		/* User call-back on lifetime timers (forced)expiration */
		params.lifetime_callback = &user_lifetime_callback;
		params.callback_arg = 0xe;
#else
		params.lifetime_callback = 0;
#endif
		/* Storage profile */
		params.spid = ni_spid;
		/* Create Outbound (encryption) Descriptor */
		err = ipsec_add_sa_descriptor(&params, ws_instance_handle,
					      &ws_desc_handle_outbound);
		handle_high = (uint32_t)((ws_desc_handle_outbound &
					  0xffffffff00000000) >> 32);
		handle_low = (uint32_t)(ws_desc_handle_outbound &
					0x00000000ffffffff);
		if (err) {
			fsl_print("Add ENCAP SA failed : err = %d\n", err);
			break;
		}
		fsl_print("[%d] Add ENCAP SA succeeded : handle = 0x%x:%08x\n",
			  sa_index, handle_high, handle_low);
		ipsec_sas_desc_outbound[sa_index] = ws_desc_handle_outbound;
		/* Inbound (decryption) parameters */
		params.direction = IPSEC_DIRECTION_INBOUND;
		/* Flags */
		params.flags = tunnel_transport_mode |
				IPSEC_FLG_LIFETIME_KB_CNTR_EN |
				IPSEC_FLG_LIFETIME_PKT_CNTR_EN |
				reuse_buffer_mode;
		if (decap_soft_seconds | decap_hard_seconds) {
			params.flags |= IPSEC_FLG_LIFETIME_SEC_CNTR_EN;
			fsl_print("DECAP lifetime : soft = %u, hard = %u\n",
				  decap_soft_seconds, decap_hard_seconds);
		}
		pad_check = DECAP_PAD_CHECK;
		params.flags |= pad_check;
		params.decparams.options = esn_enable | transport_ipvsn |
				DECAP_ARW_SIZE;
		/* Sequence Number */
		params.decparams.seq_num_ext_hi = SEQNO_H;
		params.decparams.seq_num = SEQNO_L;
		/* Cipher algorithm parameters */
		params.cipherdata.algtype = cipher_alg;
		params.cipherdata.key = cipher_key_addr;
		params.cipherdata.keylen = cipher_keylen;
		params.cipherdata.key_enc_flags = 0x0;
		/* Authentication algorithm parameters */
		params.authdata.algtype = auth_alg;
		params.authdata.key = auth_key_addr;
		params.authdata.keylen = auth_keylen;
		params.authdata.key_enc_flags = 0x0;
		/* Bytes/Packets limits */
#if (KB_LIMIT_ENABLE == 1)
		params.soft_kilobytes_limit = DECAP_SOFT_KB_LIMIT;
		params.hard_kilobytes_limit = DECAP_HARD_KB_LIMIT;
		lifetime_kb_cntr_en = IPSEC_FLG_LIFETIME_KB_CNTR_EN;
#endif
#if (PKT_LIMIT_ENABLE == 1)
		params.soft_packet_limit = DECAP_SOFT_PKT_LIMIT;
		params.hard_packet_limit = DECAP_HARD_PKT_LIMIT;
		lifetime_pkt_cntr_en = IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
#endif
		/* Lifetime limits */
#if (LIFETIME_TIMERS_ENABLE == 1)
		params.soft_seconds_limit = decap_soft_seconds;
		params.hard_seconds_limit = decap_hard_seconds;
		/* User call-back on lifetime timers (forced)expiration */
		params.lifetime_callback = &user_lifetime_callback;
		params.callback_arg = 0xd;
#else
		params.lifetime_callback = 0;
#endif
		/* Storage profile */
		params.spid = ni_spid;
		/* Create Inbound (decryption) Descriptor */
		err = ipsec_add_sa_descriptor(&params, ws_instance_handle,
					      &ws_desc_handle_inbound);
		if (err) {
			fsl_print("Add DECAP SA failed : err = %d\n", err);
			break;
		}
		handle_high = (uint32_t)((ws_desc_handle_inbound &
					  0xffffffff00000000) >> 32);
		handle_low = (uint32_t)(ws_desc_handle_inbound &
					0x00000000ffffffff);
		fsl_print("[%d] Add DECAP SA succeeded : handle = 0x%x:%08x\n",
			  sa_index, handle_high, handle_low);
		ipsec_sas_desc_inbound[sa_index] = ws_desc_handle_inbound;
	}
	if (!err)
		fsl_print("IPsec Demo: IPsec Initialization completed\n");
	else
		return err;
#if (IPSEC_STATS_PRINT == 1)
	/* Create statistics print periodic timer */
	err = tman_create_timer(tmi_id,
				TMAN_CREATE_TIMER_MODE_TPRI |
				TMAN_CREATE_TIMER_MODE_SEC_GRANULARITY |
				TMAN_CREATE_TIMER_MODE_LOW_PRIORITY_TASK,
				STATS_TIMER_PERIOD, 0, 0,
				app_stats_timer_cb, &stats_timer_handle);
	if (err) {
		pr_err("%d : Cannot create Statistics print timer\n", err);
		return err;
	}
	fsl_print("\t Statistics print timer created handle = 0x%08x\n",
		  stats_timer_handle);
#endif
	return 0;
} /* End of ipsec_app_init */

/* Print the frame in a Wireshark-like format */
void ipsec_print_frame(void) {
	uint8_t *eth_pointer_byte = 0;
	int i;
	uint16_t seg_len = PRC_GET_SEGMENT_LENGTH();
	uint32_t frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

	fsl_print("Printing Frame. FD[len] = %d, Seg Len = %d\n",
			frame_len, seg_len);

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();

	for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
	{
		if ((i%16) == 0) {
			fsl_print("00");
			if (i<16)
				fsl_print("0");
			fsl_print("%x  ",(i));
		}

		if ((*eth_pointer_byte) < 16)
			fsl_print("0");

		fsl_print("%x ", *eth_pointer_byte);

		if ((i%8) == 7)
			fsl_print(" ");

		if ((i%16) == 15)
			fsl_print("\n");

		eth_pointer_byte++;
	}

	if ((i%16) != 0)
		fsl_print("\n");
} /* End of ipsec_print_frame */

static void ipsec_print_stats(ipsec_handle_t desc_handle, uint8_t is_encap)
{
	int		err = 0;
	uint64_t	kilobytes, packets;
	uint32_t	sec, sequence_number, extended_sequence_number;
	uint32_t	anti_replay_bitmap[4];

	fsl_print("IPsec Demo: %s Statistics:\n", (is_encap) ?
		  "Encryption" : "Decryption");
	/* Read statistics */
	err = ipsec_get_lifetime_stats(desc_handle, &kilobytes, &packets, &sec);
	fsl_print("\t bytes     = %ll\n", kilobytes);
	fsl_print("\t packets   = %ll\n", packets);
	fsl_print("\t seconds   = %d\n", sec);

	err = ipsec_get_seq_num(desc_handle, &sequence_number,
				&extended_sequence_number, anti_replay_bitmap);
	fsl_print("\t sequence_number = 0x%x:%08x\n",
			extended_sequence_number, sequence_number);
	if (!is_encap) {
		fsl_print("\t bitmap[0:3]     = %08x %08x %08x %08x\n",
			anti_replay_bitmap[0], anti_replay_bitmap[1],
			anti_replay_bitmap[2], anti_replay_bitmap[3]);
	}
}

void ipsec_print_sp (uint16_t ni_spid) {

	struct storage_profile *sp_addr = &storage_profile[0];

	/* Debug - Print the storage profile */
	sp_addr += ni_spid; /* TMP for printing the SP */

	fsl_print("*** Debug: storage_profile (0): 0x%x\n", *((uint32_t *)sp_addr + 0));
	fsl_print("*** Debug: storage_profile (1): 0x%x\n", *((uint32_t *)sp_addr + 1));
	fsl_print("*** Debug: storage_profile (2): 0x%x\n", *((uint32_t *)sp_addr + 2));
	fsl_print("*** Debug: storage_profile (3): 0x%x\n", *((uint32_t *)sp_addr + 3));
	fsl_print("*** Debug: storage_profile (4): 0x%x\n", *((uint32_t *)sp_addr + 4));
	fsl_print("*** Debug: storage_profile (5): 0x%x\n", *((uint32_t *)sp_addr + 5));
	fsl_print("*** Debug: storage_profile (6): 0x%x\n", *((uint32_t *)sp_addr + 6));
	fsl_print("*** Debug: storage_profile (7): 0x%x\n", *((uint32_t *)sp_addr + 7));
} /* End of ipsec_print_sp */

#if (IPSEC_STATS_PRINT == 1)
static uint32_t get_tman_task_handle(void)
{
	return LW_SWAP(16, (uint32_t *)HWC_FD_ADDRESS);
}

static void app_stats_timer_cb(tman_arg_8B_t arg1, tman_arg_2B_t arg2)
{
	int	i;

	UNUSED(arg1);
	UNUSED(arg2);

	ASSERT_COND(STATS_PRINT_FROM_SA < TEST_NUM_OF_SA);
	ASSERT_COND(STATS_PRINT_TO_SA <= TEST_NUM_OF_SA);
	ASSERT_COND(STATS_PRINT_FROM_SA < STATS_PRINT_TO_SA);

	/* Confirm that timer callback finished execution */
	tman_timer_completion_confirmation(get_tman_task_handle());

	for (i = STATS_PRINT_FROM_SA; i < STATS_PRINT_TO_SA; i++) {
		ipsec_print_stats(ipsec_sas_desc_outbound[i], 1);
#if (ENCRYPT_ONLY == 0)
		ipsec_print_stats(ipsec_sas_desc_inbound[i], 0);
#endif
	}
}
#endif	/* IPSEC_STATS_PRINT */

#if (LIFETIME_TIMERS_ENABLE == 1)
void user_lifetime_callback(uint64_t opaque1, uint8_t opaque2)
{
	fsl_print("\nIn user_lifetime_callback()\n");
	
	if (opaque1 == 0xe)
		fsl_print("Encryption : ");
	else 
		fsl_print("Decryption : ");

	if (opaque2 == IPSEC_SOFT_SEC_LIFETIME_EXPIRED)
		fsl_print("Soft lifetime expired\n");
	else if (opaque2 == IPSEC_HARD_SEC_LIFETIME_EXPIRED)
		fsl_print("Hard lifetime expired\n");
	else if (opaque2 == IPSEC_FORCE_SOFT_SEC_LIFETIME_EXPIRED)
		fsl_print("Soft lifetime forced expired\n");
	else
		fsl_print("Hard lifetime forced expired\n");
}
#endif
