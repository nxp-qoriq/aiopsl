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

#include "common/types.h"
#include "common/fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "platform.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"
#include "kernel/fsl_cmdif_server.h"
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_ipsec.h"
#include "ls2085_aiop/fsl_platform.h"
#include "lib/fsl_slab.h"
#include "system.h" // TMP

int app_init(void);
void app_free(void);
int ipsec_app_init(uint16_t ni_id);
void ipsec_print_frame(void);
void ipsec_print_stats (ipsec_handle_t desc_handle);
void ipsec_print_sp (uint16_t ni_spid);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

/* Global IPsec vars in Shared RAM */
ipsec_instance_handle_t ipsec_instance_handle;
ipsec_handle_t ipsec_sa_desc_outbound;
ipsec_handle_t ipsec_sa_desc_inbound;
uint32_t frame_number = 0;

__declspec(entry_point) static void app_process_packet_flow0 (void)
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

	/* IPsec Initialization, happens with the first frame received */
	if (frame_number == 0) {
		err = ipsec_app_init(0); /* Call with NI ID = 0 */
		if (err) {
			fsl_os_print("ERROR: IPsec initialization failed\n");
		}
	}

	ipsec_handle_t ws_desc_handle_outbound = ipsec_sa_desc_outbound;
	ipsec_handle_t ws_desc_handle_inbound = ipsec_sa_desc_inbound;

	frame_number ++;

	fsl_os_print("IPsec Demo: Core %d Received Frame number %d\n",
			core_get_id(), frame_number);

	handle_high =
			(uint32_t)((ws_desc_handle_outbound & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ws_desc_handle_outbound & 0x00000000ffffffff);
	fsl_os_print("Encryption handle = 0x%x_%x\n", handle_high, handle_low);

	handle_high =
			(uint32_t)((ws_desc_handle_inbound & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ws_desc_handle_inbound & 0x00000000ffffffff);
	fsl_os_print("Decryption handle = 0x%x_%x\n", handle_high, handle_low);

	/* preserve original frame */
	for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
	{
		frame_before_encr[i] =  *eth_pointer_byte;
		eth_pointer_byte++;
	}

	fsl_os_print("IPSEC: frame header before encryption\n");
	/* Print header */
	ipsec_print_frame();

	fsl_os_print("\n");

	fsl_os_print("IPSEC: Starting Encryption\n");
	err = ipsec_frame_encrypt(
			ws_desc_handle_outbound,
			&enc_status
			);

	if (err)
	{
		fsl_os_print("ERROR: ipsec_frame_encrypt() failed\n");
		local_test_error |= err;
	}
	else
		fsl_os_print("ipsec_frame_encrypt() completed successfully\n");

	if (enc_status)
	{
		fsl_os_print("ERROR: SEC Encryption Failed (enc_status = 0x%x)\n",
				enc_status);
		local_test_error |= enc_status;
	}

	fsl_os_print("IPSEC: frame header after encryption\n");
	/* Print header */
	ipsec_print_frame();
	fsl_os_print("\n");

	fsl_os_print("IPSEC: Starting Decryption\n");
	err = ipsec_frame_decrypt(
			ws_desc_handle_inbound,
			&dec_status
			);

	if (err)
	{
		fsl_os_print("ERROR: ipsec_frame_decrypt() failed\n");
		local_test_error |= err;
	}
	else
		fsl_os_print("ipsec_frame_decrypt() completed successfully\n");

	if (dec_status)
	{
		fsl_os_print("ERROR: SEC Decryption Failed (dec_status = 0x%x)\n",
				dec_status);
		local_test_error |=dec_status;
	}

	fsl_os_print("IPSEC: frame header after decryption\n");
	/* Print header */
	ipsec_print_frame();
	fsl_os_print("\n");

	/* Compare decrypted frame to original frame */
	err = 0;
	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

	if (frame_len != original_frame_len) {
		fsl_os_print("ERROR: incorrect frame length (FD[length] = %d)\n",
				frame_len);
		err = 1;
		local_test_error |= err;
	} else {
		for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
		{
			if (frame_before_encr[i] !=  *eth_pointer_byte) {
				fsl_os_print(
						"ERROR: frame after decryption differ from origin\n");
				err = 1;
				local_test_error |= err;
				break;
			}
			eth_pointer_byte++;
		}
	}

	if(!local_test_error) /* No error found during injection of packets*/
	{
		fsl_os_print("Finished SUCCESSFULLY\n");
		fsl_os_print("\nFrame after decryption the same as origin\n\n");
	}
	else
		fsl_os_print("Finished with ERRORS\n");

	/* Read statistics */
	fsl_os_print("IPsec Demo: Encryption Statistics:\n");
	ipsec_print_stats(ws_desc_handle_outbound);

	fsl_os_print("IPsec Demo: Decryption Statistics:\n");
	ipsec_print_stats(ws_desc_handle_inbound);

	fsl_os_print("IPsec Demo: Core %d Sending Frame number %d\n",
			core_get_id(), frame_number);

	dpni_drv_send(dpni_get_receive_niid());

	fsl_os_print("IPsec Demo: Done Sending Frame\n\n");
	/*MUST call fdma_terminate task in the end of cb function*/
	fdma_terminate_task();
}

#ifdef AIOP_STANDALONE
/* This is temporal WA for stand alone demo only */
#define WRKS_REGS_GET \
	(sys_get_memory_mapped_module_base(FSL_OS_MOD_CMGW,            \
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
	fsl_os_print("open_cb inst_id = 0x%x\n", instance_id);
	return 0;
}

static int close_cb(void *dev)
{
	UNUSED(dev);
	fsl_os_print("close_cb\n");
	return 0;
}

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size, void *data)
{
	UNUSED(dev);
	UNUSED(size);
	UNUSED(data);
	fsl_os_print("ctrl_cb cmd = 0x%x, size = %d, data 0x%x\n",
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

int app_init(void)
{
	int        err  = 0;
	uint32_t   ni   = 0;
	dma_addr_t buff = 0;

	fsl_os_print("Running app_init()\n");

#ifdef AIOP_STANDALONE
	/* This is temporal WA for stand alone demo only */
	epid_setup();
#endif /* AIOP_STANDALONE */


	for (ni = 0; ni < dpni_get_num_of_ni(); ni++)
	{
		err = dpni_drv_register_rx_cb((uint16_t)ni /*ni_id*/,
				app_process_packet_flow0 /* callback */);
		if (err) return err;
	}

	err = cmdif_register_module("TEST0", &ops);
	if (err)
	{
		fsl_os_print("FAILED cmdif_register_module\n!");
		return err;
	}

	/* IPsec Initialization */
	//err = ipsec_app_init(0); /* Call with NI ID = 0 */
	//if (err) {
	//	fsl_os_print("ERROR: IPsec initialization failed\n");
	//	return err;
	//}

	fsl_os_print("To start test inject packets: \"eth_ipv4_udp.pcap\"\n");
	fsl_os_print("(IPsec initialization will occur when the first frame is received)\n");
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
	uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	//uint8_t auth_key[128];
	uint8_t auth_key[128] = "1234567812345678";
	uint8_t auth_key_id = 0;
	
	uint32_t cipher_alg;
	uint32_t cipher_keylen;
	uint32_t auth_alg;
	uint32_t auth_keylen;
	uint32_t algs;
	uint32_t outer_header_ip_version;
	uint16_t ni_spid;

	enum key_types {
		NULL_ENCRYPTION = 0,
		 AES128_SHA256
	};

	/**********************************************************/
	/*                    Control Parameters                  */
	/**********************************************************/
	/* Set the required algorithms here */
	algs = NULL_ENCRYPTION;
	//algs = AES128_SHA256;

	/* Set the outer IP header type here */
	outer_header_ip_version = 4; /* 4 or 6 */
	
	auth_key_id = 1; /* Overwrite the initial key array value */ 
	/**********************************************************/

	ipsec_instance_handle_t ws_instance_handle = 0;
	ipsec_handle_t ws_desc_handle_outbound = 0;
	ipsec_handle_t ws_desc_handle_inbound = 0;

	if (auth_key_id == 1) {
		for (i=0; i<128; i++) {
			auth_key[i] = (uint8_t)i;
		}
	}

	frame_number = 0;

	fsl_os_print("\n++++\n  IPsec Demo: Doing IPsec Initialization\n+++\n");

	dpni_drv_get_spid(
		ni_id, /* uint16_t ni_id */
		&ni_spid /* uint16_t *spid */
		);

	fsl_os_print("IPsec Demo: SPID = %d\n", ni_spid);

#define IPSEC_DEBUG_PRINT_SP
#ifdef IPSEC_DEBUG_PRINT_SP
	ipsec_print_sp (ni_spid);
#endif

	err = ipsec_create_instance(
			10, /* committed sa num */
			20, /* max sa num */
			0, /* instance flags */
			0, /* tmi id */
			&ws_instance_handle);
	if (err) {
		fsl_os_print("ERROR: ipsec_create_instance() failed\n");
		fsl_os_print("ipsec_create_instance return status = %d (0x%x)\n",
				err, err);
	} else {
		fsl_os_print("ipsec_create_instance() completed successfully\n");
	}

	ipsec_instance_handle = ws_instance_handle;

	/* Allocate buffers for the Keys */
	err = slab_create(
			10, /* uint32_t    num_buffs */
			10, /* uint32_t    max_buffs */
			512, /* uint16_t    buff_size */
			8, /*uint16_t    alignment */
			MEM_PART_DP_DDR, /* uint8_t     mem_partition_id */
			0, /* uint32_t    flags */
			NULL, /* slab_release_cb_t release_cb */
			&slab_handle /* struct slab **slab */
			);

	if (err)
		fsl_os_print("ERROR: slab_create() failed\n");
	else
		fsl_os_print("slab_create() completed successfully\n");

	/* Acquire the Cipher key buffer */
	err = 0;
	err = slab_acquire(
			slab_handle, /* struct slab *slab */
			&cipher_key_addr /* uint64_t *buff */
			);

	if (err)
		fsl_os_print("ERROR: slab_acquire() failed\n");
	else
		fsl_os_print("slab_acquire() completed successfully\n");

	/* Acquire the Authentication key buffer */
	err = 0;
	err = slab_acquire(
			slab_handle, /* struct slab *slab */
			&auth_key_addr /* uint64_t *buff */
			);

	if (err)
		fsl_os_print("ERROR: slab_acquire() failed\n");
	else
		fsl_os_print("slab_acquire() completed successfully\n");



	switch (algs) {
		case NULL_ENCRYPTION:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_NULL\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_MD5_96\n");
			cipher_alg = IPSEC_CIPHER_NULL;
			cipher_keylen = 0x0;
			auth_alg = IPSEC_AUTH_HMAC_MD5_96;
			auth_keylen = 16;
			break;
		case AES128_SHA256:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA2_256_128\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_SHA2_256_128;
			auth_keylen = 64;
			break;
		default:
			fsl_os_print("Cipher Algorithm (default): IPSEC_CIPHER_NULL\n");
			fsl_os_print("Authentication Algorithm (default): IPSEC_AUTH_HMAC_MD5_96\n");
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

	/* Outer IP header */
	if (outer_header_ip_version == 4) {
		outer_ip_header[0] = 0x45db0014;
		outer_ip_header[1] = 0x12340000;
		outer_ip_header[2] = 0xff32386f;
		outer_ip_header[3] = 0x45a4e14c;
		outer_ip_header[4] = 0xed035c45;

		params.encparams.ip_hdr_len = 0x14; /* outer header length is 20 bytes */
	}
	else if (outer_header_ip_version == 6) {

		outer_ip_header[0] = 0x60000000;
		outer_ip_header[1] = 0x002032ff;
		outer_ip_header[2] = 0xfe800000;
		outer_ip_header[3] = 0x00000000;
		outer_ip_header[4] = 0x021125ff;
		outer_ip_header[5] = 0xfe8295b5;
		outer_ip_header[6] = 0xff020000;
		outer_ip_header[7] = 0x00000000;
		outer_ip_header[8] = 0x00000001;
		outer_ip_header[9] = 0xff8295b5;

		params.encparams.ip_hdr_len = 0x28; /* outer header length is 40 bytes */
	}

	/* Outbound (encryption) parameters */
	params.direction = IPSEC_DIRECTION_OUTBOUND; /**< Descriptor direction */
	params.flags = IPSEC_FLG_TUNNEL_MODE |
			IPSEC_FLG_LIFETIME_KB_CNTR_EN | IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
			/**< Miscellaneous control flags */

	params.encparams.ip_nh = 0x0;
	params.encparams.options = 0x0;
	params.encparams.seq_num_ext_hi = 0x0;
	params.encparams.seq_num = 0x0;
	params.encparams.spi = 0x0;
	params.encparams.outer_hdr = (uint32_t *)&outer_ip_header;

	for (i=0; i<sizeof(params.encparams.cbc.iv); i++) {
		params.encparams.cbc.iv[i] = 0;
	}

	params.cipherdata.algtype = cipher_alg;
	params.cipherdata.key = cipher_key_addr;
	params.cipherdata.keylen = cipher_keylen;
	params.cipherdata.key_enc_flags = 0x0;

	params.authdata.algtype = auth_alg;
	params.authdata.key = auth_key_addr;
	params.authdata.keylen = auth_keylen;
	params.authdata.key_enc_flags = 0x0;

	params.soft_kilobytes_limit = 0xffffffffffffffff;
	params.hard_kilobytes_limit = 0xffffffffffffffff;
	params.soft_packet_limit = 0xffffffffffffffff;
	params.hard_packet_limit = 0xffffffffffffffff;
	params.soft_seconds_limit = 0x0;
	params.hard_seconds_limit = 0x0;

	params.lifetime_callback = NULL;
	params.callback_arg = NULL;

	params.spid = ni_spid;

	/* Create Outbound (encryption) Descriptor */
	err = ipsec_add_sa_descriptor(
			&params,
			ws_instance_handle,
			&ws_desc_handle_outbound);

	handle_high =
			(uint32_t)((ws_desc_handle_outbound & 0xffffffff00000000)>>32);
	handle_low =
			(uint32_t)(ws_desc_handle_outbound & 0x00000000ffffffff);

	if (err) {
		fsl_os_print("ERROR: ipsec_add_sa_descriptor(encryption) failed\n");
		fsl_os_print("ipsec_add_sa_descriptor return status = %d (0x%x)\n",
				err, err);
	} else {
		fsl_os_print("ipsec_add_sa_descriptor(encryption) succeeded\n");
		fsl_os_print("Encryption handle = 0x%x_%x\n", handle_high, handle_low);
	}

	ipsec_sa_desc_outbound = ws_desc_handle_outbound;

	/* Inbound (decryption) parameters */
	params.direction = IPSEC_DIRECTION_INBOUND; /**< Descriptor direction */
	params.flags = IPSEC_FLG_TUNNEL_MODE |
			IPSEC_FLG_LIFETIME_KB_CNTR_EN | IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
			/**< Miscellaneous control flags */

	params.decparams.options = 0x0;
	//params.decparams.options = IPSEC_DEC_OPTS_ARS32; /* Anti Replay 32 bit enabled */

	params.decparams.seq_num_ext_hi = 0x0;
	params.decparams.seq_num = 0x0;

	params.cipherdata.algtype = cipher_alg;
	params.cipherdata.key = cipher_key_addr;
	params.cipherdata.keylen = cipher_keylen;
	params.cipherdata.key_enc_flags = 0x0;

	params.authdata.algtype = auth_alg;
	params.authdata.key = auth_key_addr;
	params.authdata.keylen = auth_keylen;
	params.authdata.key_enc_flags = 0x0;

	params.soft_kilobytes_limit = 0xffffffffffffffff;
	params.hard_kilobytes_limit = 0xffffffffffffffff;
	params.soft_packet_limit = 0xffffffffffffffff;
	params.hard_packet_limit = 0xffffffffffffffff;
	params.soft_seconds_limit = 0x0;
	params.hard_seconds_limit = 0x0;

	params.lifetime_callback = NULL;
	params.callback_arg = NULL;

	params.spid = ni_spid;

	/* Create Inbound (decryption) Descriptor */
	err = ipsec_add_sa_descriptor(
			&params,
			ws_instance_handle,
			&ws_desc_handle_inbound);

	handle_high = (uint32_t)((ws_desc_handle_inbound & 0xffffffff00000000)>>32);
	handle_low = (uint32_t)(ws_desc_handle_inbound & 0x00000000ffffffff);

	if (err) {
		fsl_os_print("ERROR: ipsec_add_sa_descriptor(decryption) failed\n");
		fsl_os_print("ipsec_add_sa_descriptor return status = %d (0x%x)\n",
				err, err);
	} else {
		fsl_os_print("ipsec_add_sa_descriptor(decryption) succeeded\n");
		fsl_os_print("Decryption handle = 0x%x_%x\n", handle_high, handle_low);
	}

	ipsec_sa_desc_inbound = ws_desc_handle_inbound;

	if (!err)
		fsl_os_print("IPsec Demo: IPsec Initialization completed\n");

	return err;
} /* End of ipsec_app_init */

/* Print the frame in a Wireshark-like format */
void ipsec_print_frame(void) {
	uint8_t *eth_pointer_byte = 0;
	int i;
	uint16_t seg_len = PRC_GET_SEGMENT_LENGTH();
	uint32_t frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);

	fsl_os_print("Printing Frame. FD[len] = %d, Seg Len = %d\n",
			frame_len, seg_len);

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();

	for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
	{
		if ((i%16) == 0) {
			fsl_os_print("00");
			if (i<16)
				fsl_os_print("0");
			fsl_os_print("%x  ",(i));
		}

		if ((*eth_pointer_byte) < 16)
			fsl_os_print("0");

		fsl_os_print("%x ", *eth_pointer_byte);

		if ((i%8) == 7)
			fsl_os_print(" ");

		if ((i%16) == 15)
			fsl_os_print("\n");

		eth_pointer_byte++;
	}

	if ((i%16) != 0)
		fsl_os_print("\n");
} /* End of ipsec_print_frame */

void ipsec_print_stats (ipsec_handle_t desc_handle) {
	int err = 0;
	uint64_t kilobytes;
	uint64_t packets;
	uint32_t sec;
	uint32_t sequence_number;
	uint32_t extended_sequence_number;
	uint32_t anti_replay_bitmap[4];
	uint32_t val_high, val_low;

	/* Read statistics */
	err = ipsec_get_lifetime_stats(
		desc_handle,
		&kilobytes,
		&packets,
		&sec);
	fsl_os_print("IPsec Demo: ipsec_get_lifetime_stats():\n");

	val_high =
		(uint32_t)((kilobytes & 0xffffffff00000000)>>32);
	val_low =
		(uint32_t)(kilobytes & 0x00000000ffffffff);
	fsl_os_print("kilobytes = 0x%x_0x%x,", val_high, val_low);

	val_high =
		(uint32_t)((packets & 0xffffffff00000000)>>32);
	val_low =
		(uint32_t)(packets & 0x00000000ffffffff);
	fsl_os_print("packets = 0x%x_0x%x, seconds = %d\n",
		val_high, val_low, sec);

	err = ipsec_get_seq_num(
		desc_handle,
		&sequence_number,
		&extended_sequence_number,
		anti_replay_bitmap);
	fsl_os_print("IPsec Demo: ipsec_get_seq_num():\n");
	fsl_os_print("sequence_number = 0x%x, esn = 0x%x\n",
			sequence_number, extended_sequence_number);
	fsl_os_print("bitmap[0:3] = 0x%x, 0x%x, 0x%x, 0x%x\n",
		anti_replay_bitmap[0], anti_replay_bitmap[1],
		anti_replay_bitmap[2], anti_replay_bitmap[3]);
} /* End of ipsec_print_stats */

void ipsec_print_sp (uint16_t ni_spid) {
	extern struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];
	struct storage_profile *sp_addr = &storage_profile[0];

	/* Debug - Print the storage profile */
	sp_addr += ni_spid; /* TMP for printing the SP */

	fsl_os_print("*** Debug: storage_profile (0): 0x%x\n", *((uint32_t *)sp_addr + 0));
	fsl_os_print("*** Debug: storage_profile (1): 0x%x\n", *((uint32_t *)sp_addr + 1));
	fsl_os_print("*** Debug: storage_profile (2): 0x%x\n", *((uint32_t *)sp_addr + 2));
	fsl_os_print("*** Debug: storage_profile (3): 0x%x\n", *((uint32_t *)sp_addr + 3));
	fsl_os_print("*** Debug: storage_profile (4): 0x%x\n", *((uint32_t *)sp_addr + 4));
	fsl_os_print("*** Debug: storage_profile (5): 0x%x\n", *((uint32_t *)sp_addr + 5));
	fsl_os_print("*** Debug: storage_profile (6): 0x%x\n", *((uint32_t *)sp_addr + 6));
	fsl_os_print("*** Debug: storage_profile (7): 0x%x\n", *((uint32_t *)sp_addr + 7));
} /* End of ipsec_print_sp */

