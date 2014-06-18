#include "common/types.h"
#include "common/fsl_stdio.h"
#include "dpni/drv.h"
#include "fsl_ip.h"
#include "kernel/platform.h"
#include "io.h"
#include "aiop_common.h"
#include "fsl_parser.h"
#include "general.h"
#include "dbg.h"
#include "fsl_cmdif_server.h"
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_ipsec.h"

int app_init(void);
void app_free(void);
int ipsec_app_init(void);

/* Global IPsec vars in Shared RAM */
__SHRAM ipsec_instance_handle_t ipsec_instance_handle;
__SHRAM ipsec_instance_handle_t ipsec_sa_desc_outbound;
__SHRAM ipsec_instance_handle_t ipsec_sa_desc_inbound; 

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */



__HOT_CODE static void app_process_packet_flow0 (dpni_drv_app_arg_t arg)
{
	int      err = 0;
	const uint16_t ipv4hdr_length = sizeof(struct ipv4hdr);
	uint16_t ipv4hdr_offset = 0;
	uint8_t *p_ipv4hdr = 0;
	uint32_t src_addr = 0x10203040;// new ipv4 src_addr
	
	fsl_os_print("IPSEC: starting encryption\n");

	
	if (PARSER_IS_OUTER_IPV4_DEFAULT())
	{
		fsl_os_print
		("app_process_packet:Core %d received packet with ipv4 header:\n",
	    core_get_id());
		ipv4hdr_offset = (uint16_t)PARSER_GET_OUTER_IP_OFFSET_DEFAULT();
		p_ipv4hdr = UINT_TO_PTR((ipv4hdr_offset + PRC_GET_SEGMENT_ADDRESS()));
		for( int i = 0; i < ipv4hdr_length ;i ++)
		{
			fsl_os_print(" %x",p_ipv4hdr[i]);
		}
		fsl_os_print("\n");
	}
	err = ip_set_nw_src(src_addr);
	if (err) {
		fsl_os_print("ERROR = %d: ip_set_nw_src(src_addr)\n", err);
	}
	if (!err && PARSER_IS_OUTER_IPV4_DEFAULT())
	{
		fsl_os_print
		("app_process_packet: Core %d will send a modified packet with ipv4 header:\n"
		, core_get_id());
		for( int i = 0; i < ipv4hdr_length ;i ++)
		{
			fsl_os_print(" %x",p_ipv4hdr[i]);
		}
		fsl_os_print("\n");
	}

	dpni_drv_send(APP_NI_GET(arg));
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
	
	/* IPsec Initialization */
	err = ipsec_app_init();
	if (err) {
		fsl_os_print("ERROR: IPsec initialization failed\n");
		return err;
	}

#ifdef AIOP_STANDALONE
	/* This is temporal WA for stand alone demo only */
	epid_setup();
#endif /* AIOP_STANDALONE */

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
	
	fsl_os_print("\n++++\n   Doing IPsec Initialization\n+++\n");
	err = ipsec_create_instance(
			10, /* committed sa num */
			20, /* max sa num */
			0, /* instance flags */
			0, /* tmi id */
			&ipsec_instance_handle);
	if (err)
		fsl_os_print("ERROR: ipsec_create_instance() failed\n");
	
	//ipsec_sa_desc_outbound;
	//ipsec_sa_desc_inbound;
	
	/*
	 * Encryption Descriptor Parameters

outeripHdr_type:0x0 (outer header is IPv4 type)
ipsec_direction:0x2      (0x2 is encryption)
flags:0x  (tunnel mode)
cipherdata.algtype:0xb00  (IPSEC_CIPHER_NULL)
cipherdata.keylen:0x0   (keylen is 0)
cipherdata.key_enc_flags:0x0                                                                 
authdata.algtype:0x1 (IPSEC_AUTH_HMAC_MD5_96)
authdata.keylen:0x20  (kenlen is 32)
enc_descriptor.cipherdata.key :0x00000000 e0a51580 (example of external address which stores key data)
authdata.key_enc_flags:0x0
soft_seconds_limit:0x0
hard_seconds_limit:0x0
spid:0x0  (sp id can use 1 or 0)
enc_descriptor.authdata.key:0x00000000 e0a50500 (example of external address which stores key data)
soft_kilobytes_limit:0xffffffff ffffffff 
hard_kilobytes_limit:0xffffffff ffffffff 
soft_packet_limit:0xffffffff ffffffff 
hard_packet_limit:0xffffffff ffffffff 
encparams.ip_nh:0x0
encparams.options:0x0
encparams.seq_num_ext_hi:0x0
encparams.seq_num:0x0
encparams.spi:0x0
encparams.ip_hdr_len:0x14 (outer header length is 20 bytes)
encparams.outer_hdr: pointer to AIOP stack memory holding the outer header 
encparams.cbc.iv[0]:0x0
encparams.cbc.iv[1]:0x0
encparams.cbc.iv[2]:0x0
encparams.cbc.iv[3]:0x0
	 * 
	 */
	
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
	params.cipherdata.key = NULL; // TMP external address which stores key data
	params.cipherdata.keylen = 0x0; 
	params.cipherdata.key_enc_flags = 0x0;
	
	params.authdata.algtype = IPSEC_AUTH_HMAC_MD5_96;
	params.authdata.key = NULL; // TMP external address which stores key data
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
	
	err = ipsec_add_sa_descriptor(
			&params,
			ipsec_instance_handle,
			&ipsec_sa_desc_outbound);

	if (err) {
		fsl_os_print("ERROR: ipsec_add_sa_descriptor() failed\n");
		fsl_os_print("ipsec_add_sa_descriptor return status = %d (0x%x)\n", err, err);
	}
		
	if (!err)
		fsl_os_print("IPsec Initialization completed\n");
	
	return err;
} /* End of ipsec_app_init */
