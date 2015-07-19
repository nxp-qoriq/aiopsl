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

#include "common/types.h"
#include "common/fsl_stdio.h"
#include "fsl_dpni_drv.h"
#include "fsl_ip.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"
/*#include "fsl_cmdif_server.h"*/
#include "fsl_cdma.h"
#include "fsl_fdma.h"
#include "fsl_ip.h"
#include "fsl_l4.h"
#include "fsl_malloc.h"
#include "fsl_tman.h"
#include "fsl_slab.h"
#include "fsl_malloc.h"
#include "fsl_frame_operations.h"
#include "fsl_ipr.h"
#include "ipr.h"
#include "fsl_l2.h"
#include "fsl_table.h"
#include "fsl_keygen.h"
#include "fsl_ipsec.h"
#include "fsl_platform.h"
#include "simple_bu_test.h"


int ipsec_app_init_bu(uint16_t ni_id);
void ipsec_print_frame_bu(void);
void ipsec_print_stats_bu (ipsec_handle_t desc_handle);
void ipsec_print_sp_bu (uint16_t ni_spid);
void ipsec_encr_decr_bu(void);

extern struct  ipr_global_parameters ipr_global_parameters1;
extern __PROFILE_SRAM struct storage_profile storage_profile[SP_NUM_OF_STORAGE_PROFILES];

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

#define FRAME_SIZE	124
#define	COPY_SIZE	16
#define AIOP_SP_BDI     0x00080000
#define SP_BP_PBS_MASK  0x3FFF
#define PLAIN_PACKET_SIZE	128

#define BU_TMI_BUF_ALIGN 64 /* TMI address alignment (64 bytes) */
/**< Align a given address - equivalent to ceil(ADDRESS,ALIGNMENT) */
#define BU_TMI_ALIGN_64(ADDRESS, ALIGNMENT)           \
        ((((uint64_t)(ADDRESS)) + ((uint64_t)(ALIGNMENT)) - 1) & \
        								(~(((uint64_t)(ALIGNMENT)) - 1)))
#define BU_TMI_ADDR_ALIGN(ADDRESS) \
	BU_TMI_ALIGN_64((ADDRESS), BU_TMI_BUF_ALIGN)
/* Aligned Descriptor Address (parameters area start) */
#define BU_TMI_ADDR(ADDRESS) BU_TMI_ADDR_ALIGN(ADDRESS)

/* Global IPsec vars in Shared RAM */
ipsec_instance_handle_t ipsec_instance_handle_bu;
ipsec_handle_t ipsec_sa_desc_outbound_bu;
ipsec_handle_t ipsec_sa_desc_inbound_bu;
uint32_t frame_number_bu = 0;

int simple_bu_test_ipsec(void)
{
	int        err  = 0;
	uint8_t prpid;
	int i;
	volatile uint32_t data_arr_32bit[8];

	fsl_os_print("Running simple bring-up test\n");
	
	parser_init(&prpid);

	default_task_params.parser_profile_id = prpid;
	default_task_params.parser_starting_hxs = 0;
	
	default_task_params.current_scope_level = 1;
	default_task_params.scope_mode_level_arr[0] = EXCLUSIVE;

	/*
	 *  0000  00 00 01 00 00 01 00 10  94 00 00 02 08 00 45 00
		0010  00 6e 00 00 00 00 ff 11  3a 26 c0 55 01 02 c0 00
		0020  00 01 04 00 04 00 00 4e  05 63 00 00 00 00 00 00
		0030  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
		0040  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
		0050  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00
		0060  00 00 00 00 00 00 00 00  f5 d0 64 51 ac 9f 69 d4
	 */
	
	// run create_frame on default frame
	{
		struct ldpaa_fd *fd = (struct ldpaa_fd *)HWC_FD_ADDRESS;
		uint8_t frame_data[FRAME_SIZE] = {0x00,0x00,0x01,0x00,0x00,0x01,0x00,\
				0x10,0x94,0x00,0x00,0x02,0x08,0x00,0x45,0x00,\
				0x00,0x6e,0x00,0x00,0x00,0x00,0xff,0x11,0x3a,\
				0x26,0xc0,0x55,0x01,0x02,0xc0,0x00,0x00,0x01,\
				0x04,0x00,0x04,0x00,0x00,0x4e,0x05,0x63,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf5,0xd0,\
				0x64,0x51,0xac,0x9f,0x69,0xd4,0xd3,0xf7,0x39,\
				0x6e,0x20,0x0e,0x97,0xb7,0xe9,0xe4,0x56,0x3a};
		uint8_t frame_data_read[FRAME_SIZE+4] = {0x00,0x00,0x01,0x00,0x00,0x01,0x00,\
				0x10,0x94,0x00,0x00,0x02,0x81,0x00,0xaa,0xbb,0x08,0x00,0x45,0x00,\
				0x00,0x6e,0x00,0x00,0x00,0x00,0xff,0x11,0x3a,\
				0x26,0xc0,0x55,0x01,0x02,0xc0,0x00,0x00,0x01,\
				0x04,0x00,0x04,0x00,0x00,0x5a,0xff,0xff,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf5,0xd0,\
				0x64,0x51,0xac,0x9f,0x69,0xd4,0xd3,0xf7,0x39,\
				0x6e,0x20,0x0e,0x97,0xb7,0xe9,0xe4,0x56,0x3a};

		uint8_t plain_packet[PLAIN_PACKET_SIZE] = {0x00,0x00,0x01,0x00,\
				0x00,0x01,0x00,0x10,0x94,0x00,0x00,0x02,\
				0x08,0x00,0x45,0x00,0x00,0x72,0x00,0x00,\
				0x00,0x00,0xff,0x11,0x99,0x60,0x08,0x08,\
				0x08,0x05,0x09,0x09,0x09,0x05,0x04,0x00,\
				0x04,0x00,0x00,0x5e,0xcf,0x86,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
				0x00,0x00,0x00,0x00,0xf5,0xd0,0x64,0x51,\
				0xac,0x9f,0x69,0xd4,0xd3,0xf7,0x39,0x6e,\
				0x20,0x0e,0x97,0xb7,0xe9,0xe4,0x56,0x3a,\
				0x64,0x12,0x2b,0x9d};
		
		uint8_t frame_handle;
		uint32_t vlan = 0x8100aabb;
		//int parse_status;
		//uint8_t *frame_presented;
		//uint8_t *seg_addr;
		struct fdma_amq amq;
		uint16_t icid, flags = 0;
		uint8_t tmp;
		//uint32_t frame_length;
		/* setting SPID = 0 */
		*((uint8_t *)HWC_SPID_ADDRESS) = 0;
		icid = (uint16_t)(storage_profile[0].ip_secific_sp_info >> 48);
		icid = ((icid << 8) & 0xff00) | ((icid >> 8) & 0xff);
		tmp = (uint8_t)(storage_profile[0].ip_secific_sp_info >> 40);
		if (tmp & 0x08)
			flags |= FDMA_ICID_CONTEXT_BDI;
		if (tmp & 0x04)
			flags |= FDMA_ICID_CONTEXT_PL;
		if (storage_profile[0].mode_bits2 & sp1_mode_bits2_VA_MASK)
			flags |= FDMA_ICID_CONTEXT_VA;
		amq.icid = icid;
		amq.flags = flags;
		set_default_amq_attributes(&amq);
		*(uint32_t *)(&storage_profile[0].pbs2) = *(uint32_t *)(&storage_profile[0].pbs1);

		for (i=0; i<8 ; i++)
			fsl_os_print("storage profile arg %d: 0x%x \n", i, *((uint32_t *)(&(storage_profile[0]))+i));
		
		
		//err = create_frame(fd, frame_data, FRAME_SIZE, &frame_handle);
		err = create_frame(fd, plain_packet, PLAIN_PACKET_SIZE, &frame_handle);
		
		if (err)
			fsl_os_print("ERROR: create frame failed!\n");

		fsl_os_print("parse result before create frame - \n");
		
		fsl_os_print("ethernet offset %d %x\n", 
					PARSER_IS_ETH_MAC_DEFAULT(), PARSER_GET_ETH_OFFSET_DEFAULT());
		
		fsl_os_print("vlan offset %d %x\n",
					PARSER_IS_ONE_VLAN_DEFAULT(), PARSER_GET_FIRST_VLAN_TCI_OFFSET_DEFAULT());
		
		fsl_os_print("ipv4 offset %d %x\n", 
					PARSER_IS_IP_DEFAULT(), PARSER_GET_OUTER_IP_OFFSET_DEFAULT());
		
		fsl_os_print("udp offset %d %x\n", 
					PARSER_IS_UDP_DEFAULT(), PARSER_GET_L4_OFFSET_DEFAULT());

		for (i=0; i<16 ; i++)
			fsl_os_print("parse results arg %d: 0x%x \n", i, *((uint32_t *)(0x80)+i));

		// Set FQD:
		*((uint8_t *)(0x40 + 0)) = 0;
		*((uint8_t *)(0x40 + 1)) = 0;
		*((uint8_t *)(0x40 + 2)) = 0;
		*((uint8_t *)(0x40 + 3)) = 0;
		
		*((uint8_t *)(0x40 + 4)) = 0;
		*((uint8_t *)(0x40 + 5)) = 0;
		*((uint8_t *)(0x40 + 6)) = 0;
		*((uint8_t *)(0x40 + 7)) = 0;
		
		*((uint8_t *)(0x40 + 8)) = 0;
		*((uint8_t *)(0x40 + 9)) = 0;
		*((uint8_t *)(0x40 + 10)) = 0;
		*((uint8_t *)(0x40 + 11)) = 0;
		
		*((uint8_t *)(0x40 + 12)) = 1; // ICID
		*((uint8_t *)(0x40 + 13)) = 0;
		*((uint8_t *)(0x40 + 14)) = 0;
		*((uint8_t *)(0x40 + 15)) = 1; // BDI

		for (i=0; i<8 ; i++) {
			data_arr_32bit[i] = 0;
		}	
		

		tman_get_timestamp(
				(uint64_t *)(&data_arr_32bit[0])); /* uint64_t *timestamp) */
		//fsl_os_print("First tman_get_timestamp() high = %d\n", data_arr_32bit[0]);
		//fsl_os_print("First tman_get_timestamp() low  = %d\n", data_arr_32bit[1]);
		tman_get_timestamp(
				(uint64_t *)(&data_arr_32bit[2])); /* uint64_t *timestamp) */
		//fsl_os_print("Second tman_get_timestamp() high = %d\n", data_arr_32bit[2]);
		//fsl_os_print("Second tman_get_timestamp() low  = %d\n", data_arr_32bit[3]);
		
		/* IPsec Test */
		err = ipsec_app_init_bu(0); /* Call with NI ID = 0 */
		if (err) {
			fsl_os_print("ERROR: IPsec initialization failed\n");
		}
		
		/* Wait, to see seconds lifetime counter is not 0 */
		fsl_os_print("ipsec: waiting for few seconds\n");
		while ((data_arr_32bit[3] - data_arr_32bit[1]) < 2000000) {
			tman_get_timestamp(
				(uint64_t *)(&data_arr_32bit[2])); /* uint64_t *timestamp) */
			//fsl_os_print("In while tman_get_timestamp() low  = %d\n", data_arr_32bit[1]);
		}
		fsl_os_print("\nipsec: done waiting ...\n\n");

		fsl_os_print("First tman_get_timestamp() low  = %d\n", data_arr_32bit[1]);
		fsl_os_print("Second tman_get_timestamp() low  = %d\n", data_arr_32bit[3]);

		
		fsl_os_print("\nCalling ipsec_encr_decr_bu()\n");
		
		fdma_close_default_segment();
		fdma_present_default_frame_segment(0, (void *)PRC_GET_SEGMENT_ADDRESS(), 0, 256);
		
		ipsec_encr_decr_bu();
		fsl_os_print("ipsec_encr_decr_bu() completed\n");
		
		// Do another encr-decr
		//fsl_os_print("\nCalling ipsec_encr_decr_bu() for the second time\n");
		//fdma_close_default_segment();
		//fdma_present_default_frame_segment(0, (void *)PRC_GET_SEGMENT_ADDRESS(), 0, 256);
		//ipsec_encr_decr_bu();
		//fsl_os_print("ipsec_encr_decr_bu() second time completed\n");
		
		
		fdma_discard_default_frame(FDMA_DIS_NO_FLAGS);
		
	}
	
	if (!err) {
		fsl_os_print("Simple bring-up test completed successfully\n");
	}
	
	return err;
}

/* IPsec Initialization */
int ipsec_app_init_bu(uint16_t ni_id)
{
	int err  = 0;
	struct ipsec_descriptor_params params;
	uint32_t outer_ip_header[10];
	struct slab *slab_handle = NULL;
	uint32_t handle_high, handle_low;
	int i;

	uint64_t cipher_key_addr;
	uint64_t auth_key_addr;
	//uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	uint8_t cipher_key[16] = "1122334455667788";
	//uint8_t auth_key[128];
	uint8_t auth_key[128] = "12345678123456781234";
	uint8_t auth_key_id = 0;
	
	uint32_t cipher_alg;
	uint32_t cipher_keylen;
	uint32_t auth_alg;
	uint32_t auth_keylen;
	uint32_t algs;
	uint32_t outer_header_ip_version;
	uint16_t ni_spid;
	uint32_t tunnel_transport_mode;

	enum key_types {
		NULL_ENCRYPTION = 0,
		AES128_SHA1,
		AES128_SHA256,
		AES128_NULL
	};

	/**********************************************************/
	/*                    Control Parameters                  */
	/**********************************************************/
	/* Set the required algorithms here */
	algs = NULL_ENCRYPTION;
	//algs = AES128_SHA256;
	//algs = AES128_SHA1;

	/* Set the outer IP header type here */
	//outer_header_ip_version = 4; /* 4 or 6 */
	outer_header_ip_version = 17; /* UDP */
	
	auth_key_id = 0; /* Keep the initial key array value */ 
	//auth_key_id = 1; /* Overwrite the initial key array value */ 
	
	tunnel_transport_mode = IPSEC_FLG_TUNNEL_MODE; /* Tunnel Mode */
	//tunnel_transport_mode = 0; /* Transport Mode */
	
	/**********************************************************/

	ipsec_instance_handle_t ws_instance_handle = 0;
	ipsec_handle_t ws_desc_handle_outbound = 0;
	ipsec_handle_t ws_desc_handle_inbound = 0;

	if (auth_key_id == 1) {
		for (i=0; i<128; i++) {
			auth_key[i] = (uint8_t)i;
		}
	}

	frame_number_bu = 0;

	fsl_os_print("\n++++\n  IPsec Demo: Doing IPsec Initialization\n+++\n");

	dpni_drv_get_spid(
		ni_id, /* uint16_t ni_id */
		&ni_spid /* uint16_t *spid */
		);
	
	fsl_os_print("IPsec Demo: Overriding NI SPID to 0\n");
	ni_spid = 0;
	
	fsl_os_print("IPsec Demo: SPID = %d\n", ni_spid);

#define IPSEC_DEBUG_PRINT_SP
#ifdef IPSEC_DEBUG_PRINT_SP
	ipsec_print_sp_bu (ni_spid);
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

	ipsec_instance_handle_bu = ws_instance_handle;

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
		case AES128_SHA1:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA1_96\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_SHA1_96;
			auth_keylen = 20;
			break;
		case AES128_SHA256:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_SHA2_256_128\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_SHA2_256_128;
			auth_keylen = 64;
			break;
		case AES128_NULL:
			fsl_os_print("Cipher Algorithm: IPSEC_CIPHER_AES_CBC\n");
			fsl_os_print("Authentication Algorithm: IPSEC_AUTH_HMAC_NULL\n");
			cipher_alg = IPSEC_CIPHER_AES_CBC;
			cipher_keylen = 16;
			auth_alg = IPSEC_AUTH_HMAC_NULL;
			auth_keylen = 0;
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
	// UDP ENCAP
	else if (outer_header_ip_version == 17) {

		outer_ip_header[0] = 0x45db001c;
		outer_ip_header[1] = 0x12340000;
		outer_ip_header[2] = 0xff11386f;
		outer_ip_header[3] = 0x45a4e14c;
		outer_ip_header[4] = 0xed035c45;
		outer_ip_header[5] = 0x11941194;
		outer_ip_header[6] = 0x00000000;

		params.encparams.ip_hdr_len = 0x1c; /* outer header length is 28 bytes */
		//params.encparams.ip_hdr_len = 0x18; /* outer header length is 24 bytes */

	}
	
	/* Outbound (encryption) parameters */
	params.direction = IPSEC_DIRECTION_OUTBOUND; /**< Descriptor direction */
	//params.flags = IPSEC_FLG_TUNNEL_MODE |
	params.flags = tunnel_transport_mode |
			IPSEC_FLG_LIFETIME_KB_CNTR_EN | IPSEC_FLG_LIFETIME_PKT_CNTR_EN;
			/**< Miscellaneous control flags */
	
	// UDP ENCAP
	if ((outer_header_ip_version == 17) && 	
			(tunnel_transport_mode == IPSEC_FLG_TUNNEL_MODE)) {
		params.flags |= IPSEC_ENC_OPTS_NAT_EN;
		params.flags |= IPSEC_ENC_OPTS_NUC_EN;
		fsl_os_print("IPSEC: Tunnel Mode UDP Encapsulation\n");
	}
	
	//params.encparams.ip_nh = 0x0;
	params.encparams.options = 0x0;
	params.encparams.seq_num_ext_hi = 0x0;
	params.encparams.seq_num = 0x0;
	params.encparams.spi = 0xaabb;
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

	ipsec_sa_desc_outbound_bu = ws_desc_handle_outbound;
	
	/* Inbound (decryption) parameters */
	params.direction = IPSEC_DIRECTION_INBOUND; /**< Descriptor direction */
	//params.flags = IPSEC_FLG_TUNNEL_MODE |
	params.flags = tunnel_transport_mode |
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

	ipsec_sa_desc_inbound_bu = ws_desc_handle_inbound;

	if (!err)
		fsl_os_print("IPsec Demo: IPsec Initialization completed\n");

	return err;
} /* End of ipsec_app_init_bu */

/* Print the frame in a Wireshark-like format */
void ipsec_print_frame_bu(void) {
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
} /* End of ipsec_print_frame_bu */

void ipsec_print_stats_bu (ipsec_handle_t desc_handle) {
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
} /* End of ipsec_print_stats_bu */

void ipsec_print_sp_bu (uint16_t ni_spid) {
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
} /* End of ipsec_print_sp_bu */

void ipsec_encr_decr_bu(void)
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

	//sl_prolog();

	fsl_os_print("In ipsec_encr_decr_bu()\n");

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	uint32_t frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	original_frame_len = frame_len;
	uint16_t seg_len = PRC_GET_SEGMENT_LENGTH();
	uint16_t original_seg_addr = PRC_GET_SEGMENT_ADDRESS();

	ipsec_handle_t ws_desc_handle_outbound = ipsec_sa_desc_outbound_bu;
	ipsec_handle_t ws_desc_handle_inbound = ipsec_sa_desc_inbound_bu;

	frame_number_bu ++;

	fsl_os_print("IPsec Demo: Core %d Received Frame number %d\n",
			core_get_id(), frame_number_bu);

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
	ipsec_print_frame_bu();

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

	fdma_close_default_segment(); // yariv - BU
	fdma_present_default_frame_segment(0, (void *)PRC_GET_SEGMENT_ADDRESS(), 0, 256); // yariv - BU
	
	fsl_os_print("IPSEC: frame header after encryption\n");
	/* Print header */
	ipsec_print_frame_bu();
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

    /* Due to parser not aligned segment WA need to represent again*/
    fdma_close_default_segment();
    
    err = fdma_present_default_frame_segment(
    		FDMA_PRES_NO_FLAGS, /* uint32_t flags */
    		(void *)original_seg_addr, /* void *ws_dst */
    		0, /* uint16_t offset */
    		seg_len); /* uint16_t present_size */
    
   	fsl_os_print("STATUS: fdma_present_default_frame_segment returned %d\n", err);
 	
	fsl_os_print("IPSEC: frame header after decryption\n");
	/* Print header */
	ipsec_print_frame_bu();
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
	ipsec_print_stats_bu(ws_desc_handle_outbound);

	fsl_os_print("IPsec Demo: Decryption Statistics:\n");
	ipsec_print_stats_bu(ws_desc_handle_inbound);

	fsl_os_print("IPsec Demo: Core %d Sending Frame number %d\n",
			core_get_id(), frame_number_bu);

	//fdma_terminate_task();
}


