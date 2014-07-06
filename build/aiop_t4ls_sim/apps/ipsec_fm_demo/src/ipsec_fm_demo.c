#include "common/types.h"
#include "common/fsl_stdio.h"
#include "dpni/drv.h"
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

int app_init(void);
void app_free(void);
int ipsec_app_init(void);
void ipsec_print_frame(void);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

/* Global IPsec vars in Shared RAM */
__SHRAM ipsec_instance_handle_t ipsec_instance_handle;
__SHRAM ipsec_instance_handle_t ipsec_sa_desc_outbound;
__SHRAM ipsec_instance_handle_t ipsec_sa_desc_inbound; 
__SHRAM uint32_t frame_number; 

__HOT_CODE static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	int      err = 0;
	uint32_t enc_status = 0;
	uint32_t dec_status = 0;
	int i = 0;
	uint8_t frame_before_encr[256] = {0};
	uint8_t *eth_pointer_byte = 0;
	uint32_t handle_high, handle_low;

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	uint32_t frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	uint16_t seg_len = PRC_GET_SEGMENT_LENGTH();
	
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
			//ipsec_sa_desc_outbound,
			ws_desc_handle_outbound,
			&enc_status
			);

	if (err)
		fsl_os_print("ERROR: ipsec_frame_encrypt() failed\n");
	else
		fsl_os_print("ipsec_frame_encrypt() completed successfully\n");
	
	if (enc_status)
		fsl_os_print("ERROR: SEC Encryption Failed (enc_status = 0x%x)\n", 
				enc_status);

	/* Run parser on new (encrypted) frame */ 
	parse_result_generate_default (PARSER_NO_FLAGS);
	
	fsl_os_print("IPSEC: frame header after encryption\n");
	/* Print header */
	ipsec_print_frame();
	fsl_os_print("\n");
	
	fsl_os_print("IPSEC: Starting Decryption\n");
	err = ipsec_frame_decrypt(
			//ipsec_sa_desc_inbound,
			ws_desc_handle_inbound,
			&dec_status
			);

	if (err)
		fsl_os_print("ERROR: ipsec_frame_decrypt() failed\n");
	else
		fsl_os_print("ipsec_frame_decrypt() completed successfully\n");
	
	if (dec_status)
		fsl_os_print("ERROR: SEC Decryption Failed (dec_status = 0x%x)\n", 
				dec_status);

	fsl_os_print("IPSEC: frame header after decryption\n");
	/* Print header */
	ipsec_print_frame();
	fsl_os_print("\n");
	
	/* Compare decrypted frame to original frame */
	err = 0;
	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	frame_len = LDPAA_FD_GET_LENGTH(HWC_FD_ADDRESS);
	
	if (frame_len < 20) {
		fsl_os_print("ERROR: incorrect frame length (FD[length] = %d)\n", 
				frame_len);
		err = 1;
	} else {
		for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
		{
			if (frame_before_encr[i] !=  *eth_pointer_byte) {
				fsl_os_print(
						"ERROR: frame after decryption differ from origin\n");
				err = 1;
				break;
			}
			eth_pointer_byte++;
		}
	}
	
	if (!err)
		fsl_os_print("SUCCESS: frame after decryption the same as origin\n");
	
	
	fsl_os_print("IPsec Demo: Core %d Sending Frame number %d\n", 
			core_get_id(), frame_number);

	dpni_drv_send(APP_NI_GET(arg));
	
	fsl_os_print("IPsec Demo: Done Sending Frame\n\n");

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

static int ctrl_cb(void *dev, uint16_t cmd, uint32_t size, uint64_t data)
{
	UNUSED(dev);
	UNUSED(size);
	UNUSED(data);
	fsl_os_print("ctrl_cb cmd = 0x%x, size = %d, data high= 0x%x data low= 0x%x\n",
	             cmd,
	             size,
	             (uint32_t)((data & 0xFF00000000) >> 32),
	             (uint32_t)(data & 0xFFFFFFFF));
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

	
	/* IPsec Initialization */
	err = ipsec_app_init();
	if (err) {
		fsl_os_print("ERROR: IPsec initialization failed\n");
		//return err;
	}

	
	for (ni = 0; ni < 6; ni++)
	{
		/* Every ni will have 1 flow */
		uint32_t flow_id = 0;
		err = dpni_drv_register_rx_cb((uint16_t)ni/*ni_id*/,
		         (uint16_t)flow_id/*flow_id*/,
		         app_process_packet_flow0, /* callback for flow_id*/
		         (ni | (flow_id << 16)) /*arg, nic number*/);
		if (err) return err;
	}

	err = cmdif_register_module("TEST0", &ops);
	if (err)
		fsl_os_print("FAILED cmdif_register_module\n!");

	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}

/* IPsec Initialization */
int ipsec_app_init(void)
{
	int err  = 0;
	struct ipsec_descriptor_params params;
	uint32_t outer_ip_header[5];
	struct slab *slab_handle = NULL;
	uint32_t handle_high, handle_low;

	uint64_t cipher_key_addr;
	uint64_t auth_key_addr; 
	uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	uint8_t auth_key[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
	
	ipsec_instance_handle_t ws_instance_handle = 0;

	ipsec_handle_t ws_desc_handle_outbound = 0;
	ipsec_handle_t ws_desc_handle_inbound = 0; 
	
	frame_number = 0;
	
	fsl_os_print("\n++++\n  IPsec Demo: Doing IPsec Initialization\n+++\n");
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
			0, /* uint16_t    prefix_size */
			0, /* uint16_t    postfix_size */
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

	/* Copy the Keys to external memory with CDMA */
	cdma_write(
			cipher_key_addr, /* ext_address */
			&cipher_key, /* ws_src */
			16); /* uint16_t size */
		
	cdma_write(
			auth_key_addr, /* ext_address */
			&auth_key, /* ws_src */
			16); /* uint16_t size */
	
	/* Encryption Descriptor Parameters */
	
	/* Outer IP header */
	outer_ip_header[0] = 0x45db0014;
	outer_ip_header[1] = 0x12340000;
	outer_ip_header[2] = 0xff32386f;
	outer_ip_header[3] = 0x45a4e14c;
	outer_ip_header[4] = 0xed035c45;
	
	/* Outbound (encryption) parameters */
	params.direction = IPSEC_DIRECTION_OUTBOUND; /**< Descriptor direction */
	params.flags = IPSEC_FLG_TUNNEL_MODE; /**< Miscellaneous control flags */
	
	params.encparams.ip_nh = 0x0;
	params.encparams.options = 0x0;
	params.encparams.seq_num_ext_hi = 0x0;
	params.encparams.seq_num = 0x0;
	params.encparams.spi = 0x0;
	params.encparams.ip_hdr_len = 0x14; /* outer header length is 20 bytes */
	params.encparams.outer_hdr = (uint32_t *)&outer_ip_header;
	params.encparams.cbc.iv[0] = 0;
	params.encparams.cbc.iv[1] = 0;
	params.encparams.cbc.iv[2] = 0;
	params.encparams.cbc.iv[3] = 0;

	params.cipherdata.algtype = IPSEC_CIPHER_NULL;
	params.cipherdata.key = cipher_key_addr;
	params.cipherdata.keylen = 0x0; 
	params.cipherdata.key_enc_flags = 0x0;
	
	params.authdata.algtype = IPSEC_AUTH_HMAC_MD5_96;
	params.authdata.key = auth_key_addr;
	params.authdata.keylen = 16; 
	params.authdata.key_enc_flags = 0x0;
	
	params.soft_kilobytes_limit = 0xffffffffffffffff; 
	params.hard_kilobytes_limit = 0xffffffffffffffff;
	params.soft_packet_limit = 0xffffffffffffffff;
	params.hard_packet_limit = 0xffffffffffffffff;
	params.soft_seconds_limit = 0x0;
	params.hard_seconds_limit = 0x0;

	params.lifetime_callback = NULL;
	params.callback_arg = NULL;
	
	params.spid = 0x0; 
	
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
	params.flags = IPSEC_FLG_TUNNEL_MODE; /**< Miscellaneous control flags */
	
	params.decparams.options = 0x0;
	params.decparams.seq_num_ext_hi = 0x0;
	params.decparams.seq_num = 0x0;
	
	params.cipherdata.algtype = IPSEC_CIPHER_NULL;
	params.cipherdata.key = cipher_key_addr;
	params.cipherdata.keylen = 0x0; 
	params.cipherdata.key_enc_flags = 0x0;
	
	params.authdata.algtype = IPSEC_AUTH_HMAC_MD5_96;
	params.authdata.key = auth_key_addr;
	params.authdata.keylen = 16; 
	params.authdata.key_enc_flags = 0x0;
	
	params.soft_kilobytes_limit = 0xffffffffffffffff; 
	params.hard_kilobytes_limit = 0xffffffffffffffff;
	params.soft_packet_limit = 0xffffffffffffffff;
	params.hard_packet_limit = 0xffffffffffffffff;
	params.soft_seconds_limit = 0x0;
	params.hard_seconds_limit = 0x0;

	params.lifetime_callback = NULL;
	params.callback_arg = NULL;
	
	params.spid = 0x0; 
	
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

	eth_pointer_byte = (uint8_t *)PARSER_GET_ETH_POINTER_DEFAULT();
	
	for(i = 0; ((i<frame_len) && (i<seg_len));i ++)
	{
		if ((i%16) == 0) {
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
