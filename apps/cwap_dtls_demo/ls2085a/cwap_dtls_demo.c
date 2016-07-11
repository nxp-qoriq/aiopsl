/*
 * Copyright 2016 Freescale Semiconductor, Inc.
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
#include "fsl_platform.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "fsl_dbg.h"
#include "fsl_cmdif_server.h"
#include "fsl_cdma.h"
#include "fsl_cwap_dtls.h"
#include "fsl_malloc.h"
#include "fsl_slab.h"
#include "fsl_evmng.h"
#include "apps.h"

#define LOCK(_a)	cdma_mutex_lock_take(_a, CDMA_MUTEX_WRITE_LOCK)
#define UNLOCK(_a)	cdma_mutex_lock_release(_a)

#define pr_lock_info(...)			\
	LOCK(print_lock);			\
	DBG(REPORT_LEVEL_INFO, __VA_ARGS__);	\
	UNLOCK(print_lock);

int app_early_init(void);
int app_init(void);
void app_free(void);
int cwap_dtls_app_init(uint16_t ni_id);
void cwap_dtls_print_frame(const char *title);
void cwap_dtls_print_stats(cwap_dtls_sa_handle_t sa_handle, int is_inbound);

/* #define CWAP_DTLS_DEBUG_PRINT_SP */
#ifdef CWAP_DTLS_DEBUG_PRINT_SP
void cwap_dtls_print_sp(uint16_t ni_spid);

extern __PROFILE_SRAM
struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];
#endif /* CWAP_DTLS_DEBUG_PRINT_SP */

/* Global CAPWAP/DTLS variables in Shared RAM */
cwap_dtls_instance_handle_t cwap_dtls_instance_handle;
cwap_dtls_sa_handle_t sa_outbound, sa_inbound;
uint32_t frame_number = 0;
uint64_t print_lock;

__HOT_CODE ENTRY_POINT static void app_process_packet(void)
{
	cwap_dtls_sa_handle_t ws_desc_handle_outbound, ws_desc_handle_inbound;
	uint8_t *eth_pointer_byte;
	uint32_t handle_high, handle_low;
	uint32_t frame_len, original_frame_len;
	int err, i, local_test_error = 0, enc_status, dec_status;
	uint16_t seg_len, original_seg_addr;
	uint8_t frame_before_encr[256] = {0};

	sl_prolog();

	fsl_print("CAPWAP/DTLS Demo: ENTERED app_process_packet\n");

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	original_frame_len = frame_len;
	seg_len = PRC_GET_SEGMENT_LENGTH();
	original_seg_addr = PRC_GET_SEGMENT_ADDRESS();

	/* CAPWAP/DTLS initialization happens with the first frame received */
	if (frame_number == 0)
		/* Call with NI ID = 0 */
		cwap_dtls_app_init(0);

	ws_desc_handle_outbound = sa_outbound;
	ws_desc_handle_inbound = sa_inbound;

	fsl_print("CAPWAP/DTLS Demo: Core %d Received Frame number %d\n",
		  core_get_id(), ++frame_number);

	handle_high = (uint32_t)((ws_desc_handle_outbound &
				  0xffffffff00000000) >> 32);
	handle_low = (uint32_t)(ws_desc_handle_outbound & 0x00000000ffffffff);
	fsl_print("Encryption handle = 0x%x_%x\n", handle_high, handle_low);

	handle_high = (uint32_t)((ws_desc_handle_inbound &
				  0xffffffff00000000) >> 32);
	handle_low = (uint32_t)(ws_desc_handle_inbound & 0x00000000ffffffff);
	fsl_print("Decryption handle = 0x%x_%x\n", handle_high, handle_low);

	/* preserve original frame */
	for (i = 0; ((i < frame_len) && (i < seg_len)); i ++) {
		frame_before_encr[i] =  *eth_pointer_byte;
		eth_pointer_byte++;
	}

	cwap_dtls_print_frame("CAPWAP/DTLS Demo: Full frame before encryption");

	fsl_print("CAPWAP/DTLS Demo: Starting Encryption\n");
	enc_status = cwap_dtls_frame_encrypt(ws_desc_handle_outbound);
	if (enc_status) {
		local_test_error = enc_status;
		fsl_print("ERROR: cwap_dtls_frame_encrypt() failed\n");
		if (enc_status > 0)
			fsl_print("ERROR: SEC Encryption Failed (enc_status = 0x%x)\n",
				  enc_status);
	} else {
		fsl_print("cwap_dtls_frame_encrypt() completed successfully\n");
	}

	if (!local_test_error) {
		/* close-open segment, for the print */
		fdma_close_default_segment();
		fdma_present_default_frame_segment(
			0, (void *)PRC_GET_SEGMENT_ADDRESS(), 0, seg_len);

		cwap_dtls_print_frame("CAPWAP/DTLS Demo: Full frame after encryption");

		fsl_print("CAPWAP/DTLS Demo: Starting Decryption\n");

		dec_status = cwap_dtls_frame_decrypt(ws_desc_handle_inbound);
		if (dec_status) {
			local_test_error = dec_status;
			fsl_print("ERROR: cwap_dtls_frame_decrypt() failed\n");
			if (dec_status > 0)
				fsl_print("ERROR: SEC Decryption Failed (dec_status = 0x%x)\n",
					  dec_status);
		} else {
			fsl_print("cwap_dtls_frame_decrypt() completed successfully\n");
		}
	}

	if (!local_test_error) {
		/* Due to the parser unaligned segment WA, represent again */
		fdma_close_default_segment();

		err = fdma_present_default_frame_segment(
			FDMA_PRES_NO_FLAGS, (void *)original_seg_addr, 0, seg_len);

		fsl_print("STATUS: fdma_present_default_frame_segment returned %d\n", err);

		cwap_dtls_print_frame("CAPWAP/DTLS Demo: Full frame after decryption");

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
			for (i = 0; ((i < frame_len) && (i < seg_len)); i ++) {
				if (frame_before_encr[i] != *eth_pointer_byte) {
					fsl_print("ERROR: frame after decryption differs from origin starting from offset %d\n",
						  i);
					err = 1;
					local_test_error |= err;
					break;
				}
				eth_pointer_byte++;
			}
		}

		cwap_dtls_print_stats(ws_desc_handle_outbound, 0);
		cwap_dtls_print_stats(ws_desc_handle_inbound, 1);

		fsl_print("CAPWAP/DTLS Demo: Core %d Sending Frame number %d\n",
			  core_get_id(), frame_number);
		err = dpni_drv_send(task_get_receive_niid(), DPNI_DRV_SEND_MODE_NONE);
		if (err) {
			fsl_print("ERROR = %d: dpni_drv_send(ni_id)\n", err);
			local_test_error |= err;
			if(err == -ENOMEM)
				fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
			else
				/* (err == -EBUSY) */
				fdma_discard_fd((struct ldpaa_fd *)HWC_FD_ADDRESS, FDMA_DIS_NO_FLAGS);
		}
	} 

	if (!local_test_error) {
		/* No error found during injection of packets*/
		fsl_print("Finished SUCCESSFULLY\n");
		fsl_print("\nFrame after decryption the same as origin\n\n");
		fsl_print("CAPWAP/DTLS Demo: Done Sending Frame\n\n");
	} else {
		fsl_print("Finished with ERRORS\n");
	}

	/* MUST call fdma_terminate task in the end of cb function */
	fdma_terminate_task();
}

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
	fsl_print("ctrl_cb cmd = 0x%x, size = %d, data 0x%x\n", cmd, size,
		  (uint32_t)data);
	return 0;
}

static struct cmdif_module_ops ops = {
	.open_cb = open_cb,
	.close_cb = close_cb,
	.ctrl_cb = ctrl_cb
};

int app_early_init(void)
{
	int err;

	/* CWAP/DTLS resources reservation */
	err = cwap_dtls_early_init(10, 20, 40);
	if (err)
		return err;

	/* Reserve some general buffers for keys etc. */
	err = slab_register_context_buffer_requirements(10, 10, 512, 8,
							CWAP_DTLS_DEMO_MEM_ID,
							0, 0);
	if (err)
		return err;

	/* Set DHR to 256 in the default storage profile */
	return dpni_drv_register_rx_buffer_layout_requirements(256, 0, 0);
}

static int app_dpni_event_added_cb(uint8_t generator_id, uint8_t event_id,
				   uint64_t app_ctx, void *event_data)
{
	uint16_t ni = (uint16_t)((uint32_t)event_data);
	uint16_t mfl = 0x2000; /* Maximum Frame Length */
	int err;

	UNUSED(generator_id);
	UNUSED(event_id);

	pr_info("Event received for AIOP NI ID %d\n", ni);

	err = dpni_drv_register_rx_cb(ni, (rx_cb_t *)app_ctx);
	if (err) {
		pr_err("dpni_drv_register_rx_cb for ni %d failed: %d\n", ni,
		       err);
		return err;
	}

	err = dpni_drv_set_max_frame_length(ni, mfl);
	if (err) {
		pr_err("dpni_drv_set_max_frame_length for ni %d failed: %d\n",
		       ni, err);
		return err;
	}

	err = dpni_drv_enable(ni);
	if (err) {
		pr_err("dpni_drv_enable for ni %d failed: %d\n", ni, err);
		return err;
	}

	return 0;
}

int app_init(void)
{
	int err;

	fsl_print("CAPWAP/DTLS Demo: running app_init()...\n");

#ifdef AIOP_STANDALONE
	/* This is temporal WA for stand alone demo only */
	epid_setup();
#endif

	err = evmng_register(EVMNG_GENERATOR_AIOPSL, DPNI_EVENT_ADDED, 1,
			     (uint64_t)app_process_packet,
			     app_dpni_event_added_cb);
	if (err) {
		pr_err("EVM registration for DPNI_EVENT_ADDED failed: %d\n",
		       err);
		return err;
	}

	err = cmdif_register_module("TEST0", &ops);
	if (err) {
		fsl_print("FAILED cmdif_register_module\n!");
		return err;
	}

	fsl_print("CAPWAP/DTLS Demo: initialization will occur when the first frame is received\n");
	fsl_print("CAPWAP/DTLS Demo: sample pcap files can be found in: \"aiopsl\\misc\\setup\\traffic_files\\capwap*\"\n");

	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}

/* CAPWAP/DTLS initialization */
int cwap_dtls_app_init(uint16_t ni_id)
{
	struct cwap_dtls_sa_descriptor_params params;
	struct slab *slab_handle = NULL;
	cwap_dtls_instance_handle_t ws_instance_handle = 0;
	cwap_dtls_sa_handle_t ws_desc_handle_outbound = 0;
	cwap_dtls_sa_handle_t ws_desc_handle_inbound = 0;
	uint32_t handle_high, handle_low;
	enum tls_cipher_mode cipher_mode;
	int err, i;

	uint64_t cipher_key_addr;
	uint16_t cipher_keylen;
	/*
	 * uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	 */
	uint8_t cipher_key[16] = "1122334455667788";

	uint64_t auth_key_addr;
	uint8_t auth_key_id;
	uint16_t auth_keylen;
	/*
	 * uint8_t auth_key[128];
	 * uint8_t auth_key[128] = "12345678123456781234"; // 20 bytes
	 * uint8_t auth_key[128] = "12345678123456781234567812345678"; // 32 bytes
	 */
	uint8_t auth_key[128] = "1234567812345678123456781234567812345678123456781234567812345678"; // 64 bytes

	uint16_t ni_spid;
	uint32_t reuse_buffer_mode;

	fsl_print("\n++++\n  CAPWPAP/DTLS Demo: Doing CAPWAP/DTLS Initialization...\n++++\n");

	/**********************************************************/
	/*                    Control Parameters                  */
	/**********************************************************/
	/* Set the required algorithm parameters here */
	params.protcmd.protid = OP_PCLID_DTLS;
	params.protcmd.protinfo = OP_PCL_TLS_RSA_WITH_AES_256_GCM_SHA384;
	cipher_keylen = 16;
	auth_keylen = 0;

	/* 0/1 - keep/overwrite the initial key array value */
	auth_key_id = 0;

	/*
	 * Buffer Mode
	 * 0 - new buffer mode
	 * CWAP_DTLS_FLG_BUFFER_REUSE - buffer reuse mode
	 */
	reuse_buffer_mode = 0;

	/**********************************************************/

	cipher_mode = rta_tls_cipher_mode(params.protcmd.protinfo);
	if (cipher_mode == RTA_TLS_CIPHER_INVALID)
		fsl_print("CAPWAP/DTLS Demo: Invalid/unsupported TLS cipher suite: 0x%x\n",
			  params.protcmd.protinfo);

	if (auth_key_id)
		for (i = 0; i < 128; i++)
			auth_key[i] = (uint8_t)i;

	if (reuse_buffer_mode == CWAP_DTLS_FLG_BUFFER_REUSE)
		fsl_print("CAPWAP/DTLS Demo: Reuse Buffer Mode\n");
	else
		fsl_print("CAPWAP/DTLS Demo: New Buffer Mode\n");

	dpni_drv_get_spid(ni_id, &ni_spid);
	fsl_print("CAPWPAP/DTLS Demo: SPID = %d\n", ni_spid);

#ifdef CWAP_DTLS_DEBUG_PRINT_SP
	cwap_dtls_print_sp(ni_spid);
#endif

	/* Allocate buffers for the Keys */
	err = slab_create(10, 10, 512,	8, CWAP_DTLS_DEMO_MEM_ID, 0, NULL,
			  &slab_handle);
	if (err)
		fsl_print("ERROR: slab_create() failed\n");
	else
		fsl_print("slab_create() completed successfully\n");

	/* Acquire the Cipher key buffer */
	err = slab_acquire(slab_handle,	&cipher_key_addr);
	if (err)
		fsl_print("ERROR: slab_acquire() failed\n");
	else
		fsl_print("slab_acquire() completed successfully\n");

	/* Acquire the Authentication key buffer */
	err = slab_acquire(slab_handle,	&auth_key_addr);
	if (err) {
		fsl_print("ERROR: slab_acquire() failed\n");
	} else {
		fsl_print("slab_acquire() completed successfully\n");
		handle_high =
			(uint32_t)((auth_key_addr & 0xffffffff00000000) >> 32);
		handle_low =
			(uint32_t)(auth_key_addr & 0x00000000ffffffff);
		fsl_print("Auth key addr = 0x%x_%x\n", handle_high, handle_low);
	}

	err = cwap_dtls_create_instance(10, 20,	&ws_instance_handle);
	if (err)
		fsl_print("ERROR: cwap_dtls_create_instance() failed with status = %d (0x%x)\n",
			  err, err);
	else
		fsl_print("cwap_dtls_create_instance() completed successfully\n");

	cwap_dtls_instance_handle = ws_instance_handle;

	/* Copy the Keys to external memory with CDMA */
	cdma_write(cipher_key_addr, &cipher_key, cipher_keylen);
	cdma_write(auth_key_addr, &auth_key, auth_keylen);

	/* Outbound (encryption) parameters */
	params.protcmd.optype = OP_TYPE_ENCAP_PROTOCOL;

	/* Miscellaneous control flags */
	params.flags = reuse_buffer_mode;

	/* DTLS Encap PDB */
	if (cipher_mode == RTA_TLS_CIPHER_CBC) {
		params.pdb.cbc.dtls_enc.type = 23;
		params.pdb.cbc.dtls_enc.version[0] = 253;
		params.pdb.cbc.dtls_enc.version[1] = 254;
		params.pdb.cbc.dtls_enc.options = 0;
		params.pdb.cbc.dtls_enc.epoch = 0x0123;
		params.pdb.cbc.dtls_enc.seq_num_hi = 0x4567;
		params.pdb.cbc.dtls_enc.seq_num_lo = 0x89abcdef;
		for (i = 0; i < ARRAY_SIZE(params.pdb.cbc.iv); i++)
			params.pdb.cbc.iv[i] = (uint8_t)(255 - i);
	} else if (cipher_mode == RTA_TLS_CIPHER_GCM) {
		params.pdb.gcm.dtls_enc.type = 23;
		params.pdb.gcm.dtls_enc.version[0] = 253;
		params.pdb.gcm.dtls_enc.version[1] = 254;
		params.pdb.gcm.dtls_enc.options = 0;
		params.pdb.gcm.dtls_enc.epoch = 0x0123;
		params.pdb.gcm.dtls_enc.seq_num_hi = 0x4567;
		params.pdb.gcm.dtls_enc.seq_num_lo = 0x89abcdef;
		for (i = 0; i < ARRAY_SIZE(params.pdb.gcm.salt); i++)
			params.pdb.gcm.salt[i] = (uint8_t)(i + 1);
	}

	params.cipherdata.algtype = OP_ALG_ALGSEL_AES;
	params.cipherdata.key = cipher_key_addr;
	params.cipherdata.keylen = cipher_keylen;
	params.cipherdata.key_enc_flags = 0x0;

	params.authdata.algtype = OP_ALG_ALGSEL_SHA1;
	params.authdata.key = auth_key_addr;
	params.authdata.keylen = auth_keylen;
	params.authdata.key_enc_flags = 0x0;

	params.spid = ni_spid;

	/* Create Outbound (encryption) Descriptor */
	err = cwap_dtls_add_sa_descriptor(&params, ws_instance_handle,
					  &ws_desc_handle_outbound);

	handle_high = (uint32_t)((ws_desc_handle_outbound &
				  0xffffffff00000000) >> 32);
	handle_low = (uint32_t)(ws_desc_handle_outbound & 0x00000000ffffffff);
	if (err) {
		fsl_print("ERROR: cwap_dtls_add_sa_descriptor(encryption) failed with status = %d (0x%x)\n",
			  err, err);
	} else {
		fsl_print("cwap_dtls_add_sa_descriptor(encryption) succeeded\n");
		fsl_print("Encryption handle = 0x%x_%x\n", handle_high, handle_low);
	}

	sa_outbound = ws_desc_handle_outbound;

	/* Inbound (decryption) parameters */
	params.protcmd.optype = OP_TYPE_DECAP_PROTOCOL;

	/* Miscellaneous control flags */
	params.flags = reuse_buffer_mode;

	/* DTLS Decap PDB */
	if (cipher_mode == RTA_TLS_CIPHER_CBC) {
		params.pdb.cbc.dtls_dec.rsvd[0] = 0;
		params.pdb.cbc.dtls_dec.rsvd[1] = 0;
		params.pdb.cbc.dtls_dec.rsvd[2] = 0;
		params.pdb.cbc.dtls_dec.options = DTLS_PDBOPTS_ARS128;
		params.pdb.cbc.dtls_dec.epoch = 0x0123;
		params.pdb.cbc.dtls_dec.seq_num_hi = 0x4567;
		params.pdb.cbc.dtls_dec.seq_num_lo = 0x89abcdef;
		for (i = 0; i < ARRAY_SIZE(params.pdb.cbc.iv); i++)
			params.pdb.cbc.iv[i] = 0;
		for (i = 0; i < ARRAY_SIZE(params.pdb.cbc.anti_replay); i++)
			params.pdb.cbc.anti_replay[i] = 0;
	} else if (cipher_mode == RTA_TLS_CIPHER_GCM) {
		params.pdb.gcm.dtls_dec.rsvd[0] = 0;
		params.pdb.gcm.dtls_dec.rsvd[1] = 0;
		params.pdb.gcm.dtls_dec.rsvd[2] = 0;
		params.pdb.gcm.dtls_dec.options = DTLS_PDBOPTS_ARS128;
		params.pdb.gcm.dtls_dec.epoch = 0x0123;
		params.pdb.gcm.dtls_dec.seq_num_hi = 0x4567;
		params.pdb.gcm.dtls_dec.seq_num_lo = 0x89abcdef;
		for (i = 0; i < ARRAY_SIZE(params.pdb.gcm.salt); i++)
			params.pdb.gcm.salt[i] = (uint8_t)(i + 1);
		for (i = 0; i < ARRAY_SIZE(params.pdb.gcm.anti_replay); i++)
			params.pdb.gcm.anti_replay[i] = 0;
	}

	params.cipherdata.algtype = 0; /* not used */
	params.cipherdata.key = cipher_key_addr;
	params.cipherdata.keylen = cipher_keylen;
	params.cipherdata.key_enc_flags = 0x0;

	params.authdata.algtype = 0; /* not used */
	params.authdata.key = auth_key_addr;
	params.authdata.keylen = auth_keylen;
	params.authdata.key_enc_flags = 0x0;

	params.spid = ni_spid;

	/* create inbound (decryption) SA descriptor */
	err = cwap_dtls_add_sa_descriptor(&params, ws_instance_handle,
					  &ws_desc_handle_inbound);
	if (err) {
		fsl_print("ERROR: cwap_dtls_add_sa_descriptor(decryption) failed with status = %d (0x%x)\n",
			  err, err);
	} else {
		handle_high = (uint32_t)((ws_desc_handle_inbound &
					  0xffffffff00000000) >> 32);
		handle_low = (uint32_t)(ws_desc_handle_inbound & 0x00000000ffffffff);
		fsl_print("cwap_dtls_add_sa_descriptor(decryption) succeeded\n");
		fsl_print("Decryption handle = 0x%x_%x\n", handle_high, handle_low);
	}

	sa_inbound = ws_desc_handle_inbound;

	if (!err)
		fsl_print("CAPWAP/DTLS Demo: CAPWAP/DTLS initialization completed\n");
	else
		fsl_print("CAPWAP/DTLS Demo: Initialization failed\n");

	return err;
}

/*
 * Prints a frame in a format that may be imported in Wireshark
 * (FCS not included).
 */
static void cwap_dtls_print_frame(const char *title)
{
/* 16 bytes on a line */
#define DISP_LEN	16
#define DISP_HEAD	6
/* Segment presentation length */
#define MAX_SEGMENT_LENGTH	64

	struct fdma_present_segment_params present_segment_params;
	uint32_t len, left_len, i;
	uint16_t off, read_len;
	uint8_t *psrc, *pdst;
	char line[DISP_HEAD + 3 * DISP_LEN + 1];
	uint8_t ws_dst_dummy[MAX_SEGMENT_LENGTH];

	LOCK(print_lock);

	len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

	sprintf(line, "%s : len = %d bytes", (title) ? title  : "Packet", len);
	fsl_print("%s\n", line);

	if (!len) {
		fsl_print("\n");
		UNLOCK(print_lock);
		return;
	}

	present_segment_params.flags = FDMA_PRES_NO_FLAGS;
	present_segment_params.frame_handle = PRC_GET_FRAME_HANDLE();
	present_segment_params.ws_dst = &ws_dst_dummy;
	off = 0;
	left_len = len;

	do {
		read_len = (uint16_t)MIN(left_len, MAX_SEGMENT_LENGTH);
		present_segment_params.offset = off;
		present_segment_params.present_size = read_len;
		/* Present segment of the remaining frame */
		fdma_present_frame_segment(&present_segment_params);
		/* Point to the beginning of the current segment */
		psrc = ws_dst_dummy;
		line[0] = 0;
		/* Head of line */
		sprintf(line, "%04x: ", off);
		pdst = (uint8_t *)line + DISP_HEAD;
		for (i = 0; i < read_len; i++) {
			sprintf((void *)pdst, "%02x ", *psrc++);
			if (((i + 1) % DISP_LEN) == 0) {
				fsl_print("%s\n", line);
				line[0] = 0;
				off += DISP_LEN;
				/* Head of line */
				sprintf(line, "%04x: ", off);
				pdst = (uint8_t *)line + DISP_HEAD;
			} else {
				pdst += 3;
			}
		}

		/* Print the remainder */
		if (read_len % DISP_LEN)
			fsl_print("%s\n", line);

		left_len -= read_len;

		/* Close the currently presented segment */
		fdma_close_segment(present_segment_params.frame_handle,
				   present_segment_params.seg_handle);
	} while (left_len);

	UNLOCK(print_lock);
}

void cwap_dtls_print_stats(cwap_dtls_sa_handle_t sa_handle, int is_inbound)
{
	uint64_t sequence_number;
	uint32_t anti_replay_bitmap[4];

	cwap_dtls_get_ar_info(sa_handle, &sequence_number, anti_replay_bitmap);

	fsl_print("CAPWAP/DTLS Demo: statistics for %s:\n",
		  is_inbound ? "decryption" : "encryption");
	fsl_print("epoch[63:48], sequence_number[47:0] = 0x%llx\n",
		  sequence_number);
	if (is_inbound)
		fsl_print("anti-replay bitmap[0:3] = 0x%x, 0x%x, 0x%x, 0x%x\n",
			  anti_replay_bitmap[0], anti_replay_bitmap[1],
			  anti_replay_bitmap[2], anti_replay_bitmap[3]);
}

#ifdef CWAP_DTLS_DEBUG_PRINT_SP
void cwap_dtls_print_sp(uint16_t ni_spid)
{
	struct storage_profile *sp_addr = &storage_profile[0];

	sp_addr += ni_spid;

	fsl_print("*** Debug: storage_profile (0): 0x%x\n", *((uint32_t *)sp_addr + 0));
	fsl_print("*** Debug: storage_profile (1): 0x%x\n", *((uint32_t *)sp_addr + 1));
	fsl_print("*** Debug: storage_profile (2): 0x%x\n", *((uint32_t *)sp_addr + 2));
	fsl_print("*** Debug: storage_profile (3): 0x%x\n", *((uint32_t *)sp_addr + 3));
	fsl_print("*** Debug: storage_profile (4): 0x%x\n", *((uint32_t *)sp_addr + 4));
	fsl_print("*** Debug: storage_profile (5): 0x%x\n", *((uint32_t *)sp_addr + 5));
	fsl_print("*** Debug: storage_profile (6): 0x%x\n", *((uint32_t *)sp_addr + 6));
	fsl_print("*** Debug: storage_profile (7): 0x%x\n", *((uint32_t *)sp_addr + 7));
}
#endif
