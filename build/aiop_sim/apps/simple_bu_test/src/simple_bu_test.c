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
#include "platform.h"
#include "fsl_io.h"
#include "fsl_parser.h"
#include "general.h"
#include "fsl_dbg.h"
/*#include "fsl_cmdif_server.h"*/
#include "dplib/fsl_cdma.h"
#include "dplib/fsl_fdma.h"
#include "dplib/fsl_ip.h"
#include "dplib/fsl_l4.h"
#include "ls2085_aiop/fsl_platform.h"
#include "fsl_tman.h"
#include "fsl_slab.h"
#include "fsl_malloc.h"
#include "fsl_frame_operations.h"
#include "fsl_ipr.h"
#include "ipr.h"

int app_early_init(void);
int app_init(void);
void app_free(void);
void ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags);

#define APP_NI_GET(ARG)   ((uint16_t)((ARG) & 0x0000FFFF))
/**< Get NI from callback argument, it's demo specific macro */
#define APP_FLOW_GET(ARG) (((uint16_t)(((ARG) & 0xFFFF0000) >> 16)
/**< Get flow id from callback argument, it's demo specific macro */

#define FRAME_SIZE	124

int app_early_init(void){
	//slab_register_context_buffer_requirements(100, 100, 2688, 64, MEM_PART_DP_DDR, 0, 0);
	return 0;
}

int app_init(void)
{
	int        err  = 0;
	uint64_t tmi_mem_base_addr;
	struct ipr_params ipr_demo_params;
	ipr_instance_handle_t ipr_instance = 0;
	ipr_instance_handle_t *ipr_instance_ptr = &ipr_instance;
	struct ipr_instance	ipr_instance_read;
	struct slab *slab_handle = NULL;
	uint64_t cipher_key_addr;
	uint8_t cipher_key[16] = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26};
	uint8_t cipher_key_read[16];
	int i;

	fsl_os_print("Running simple bring-up test\n");

	ipr_demo_params.max_open_frames_ipv4 = 0x10;
	ipr_demo_params.max_open_frames_ipv6 = 0x10;
	ipr_demo_params.max_reass_frm_size = 0xf000;
	ipr_demo_params.min_frag_size_ipv4 = 0x40;
	ipr_demo_params.min_frag_size_ipv6 = 0x40;
	ipr_demo_params.timeout_value_ipv4 = 0xffe0;
	ipr_demo_params.timeout_value_ipv6 = 0xffe0;
	ipr_demo_params.ipv4_timeout_cb = ipr_timout_cb;
	ipr_demo_params.ipv6_timeout_cb = ipr_timout_cb;
	ipr_demo_params.cb_timeout_ipv4_arg = 0x66;
	ipr_demo_params.cb_timeout_ipv6_arg = 0x66;
	ipr_demo_params.flags = IPR_MODE_TABLE_LOCATION_PEB;
	fsl_os_get_mem( 0x20*64, MEM_PART_DP_DDR, 64, &tmi_mem_base_addr);

	tman_create_tmi(tmi_mem_base_addr , 0x20, &ipr_demo_params.tmi_id);

	fsl_os_print("bring-up test: Creating IPR instance\n");
	err = ipr_create_instance(&ipr_demo_params, ipr_instance_ptr);
	if (err)
	{
		fsl_os_print("ERROR: ipr_create_instance() failed %d\n",err);
		return err;
	}
	
	/* cdma read the data and check if it is correct */
	cdma_read(&ipr_instance_read, ipr_instance, IPR_INSTANCE_SIZE);
	if (ipr_instance_read.extended_stats_addr != ipr_demo_params.extended_stats_addr)
		err = -EINVAL;

	if (ipr_instance_read.max_reass_frm_size != ipr_demo_params.max_reass_frm_size)
		err = -EINVAL;
	if (ipr_instance_read.min_frag_size_ipv4 != ipr_demo_params.min_frag_size_ipv4)
			err = -EINVAL;
	if (ipr_instance_read.min_frag_size_ipv6 != ipr_demo_params.min_frag_size_ipv6)
			err = -EINVAL;
	if (ipr_instance_read.timeout_value_ipv4 != ipr_demo_params.timeout_value_ipv4)
			err = -EINVAL;
	if (ipr_instance_read.timeout_value_ipv6 != ipr_demo_params.timeout_value_ipv6)
			err = -EINVAL;
	if (ipr_instance_read.ipv4_timeout_cb != ipr_demo_params.ipv4_timeout_cb)
			err = -EINVAL;
	if (ipr_instance_read.ipv6_timeout_cb != ipr_demo_params.ipv6_timeout_cb)
			err = -EINVAL;
	if (ipr_instance_read.cb_timeout_ipv4_arg != ipr_demo_params.cb_timeout_ipv4_arg)
			err = -EINVAL;
	if (ipr_instance_read.cb_timeout_ipv6_arg != ipr_demo_params.cb_timeout_ipv6_arg)
			err = -EINVAL;
	if (ipr_instance_read.tmi_id != ipr_demo_params.tmi_id)
			err = -EINVAL;

	if (err)
	{
		fsl_os_print("ERROR:ipr instance was not read successfully\n");
		return err;
	}
	
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
	err = slab_acquire(
			slab_handle, /* struct slab *slab */
			&cipher_key_addr /* uint64_t *buff */
			);

	/* Copy the Keys to external memory with CDMA */
	cdma_write(
			cipher_key_addr, /* ext_address */
			&cipher_key, /* ws_src */
			16); /* uint16_t size */
	cdma_read(&cipher_key_read, cipher_key_addr, 16);
	
	for (i=0; i<16 ; i++)
		if (cipher_key_read[i] != cipher_key[i])
			err = -EINVAL;
	if (err)
	{
		fsl_os_print("ERROR: key info was not read successfully\n");
		return err;
	}

	fsl_os_print("Simple bring-up test completed successfully\n");
	
	// run create_frame on default frame
	{
		struct ldpaa_fd *fd = (struct ldpaa_fd *)HWC_FD_ADDRESS;
		uint8_t frame_data[FRAME_SIZE] = {0x00,0x00,0x01,0x00,0x00,0x01,0x00,\
				0x10,0x94,0x00,0x00,0x02,0x08,0x00,0x45,0x00,\
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

		uint8_t frame_handle;
		
		err = create_frame(fd, frame_data, FRAME_SIZE, &frame_handle);
		if (err)
			fsl_os_print("ERROR: create frame failed!\n");
	}
	

	return 0;
}

void app_free(void)
{
	/* TODO - complete!*/
}

void ipr_timout_cb(ipr_timeout_arg_t arg,
		uint32_t flags)
{
	UNUSED(arg);
	UNUSED(flags);
	fdma_terminate_task();
}
