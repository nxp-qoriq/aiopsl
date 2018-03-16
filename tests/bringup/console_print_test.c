/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
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

#include "fsl_system.h"
#include "fsl_malloc.h"
#include "fsl_platform.h"

extern t_system sys;

static void * fsl_malloc__(size_t size,uint32_t alignment);
static void fsl_free__(void *mem);

#define fsl_malloc fsl_malloc__
#define fsl_free fsl_free__
#define duart_config duart_config__
#define duart_init duart_init__
#include "../../../drivers/serial/duart/duart.c"

int platform_enable_console__(void * h_platform);
#define s_pltfrm s_pltfrm__
#define pltfrm_init_console_cb pltfrm_init_console_cb__
#define platform_enable_console platform_enable_console__
#undef __ERR_MODULE__
#include "../../../arch/platform/platform.c"

static t_duart_uart p_uart;
static t_duart_driver_param  p_driver_param;
static uint8_t rx_buffer[DEFAULT_RX_BUFFER_SIZE];
static uint8_t tx_buffer[DEFAULT_TX_BUFFER_SIZE];

static void * fsl_malloc__(size_t size, uint32_t alignment)
{
	UNUSED(alignment);
	static int cnt[3] = {0, 0, 0};

	if ((size == sizeof(t_duart_uart)) && (cnt[0] == 0)) {
		cnt[0]++;
		return &p_uart;
	} else if ((size == sizeof(t_duart_driver_param)) && (cnt[1] == 0)) {
		cnt[1]++;
		return &p_driver_param;
	} else if (cnt[2] == 0) {
		cnt[2]++;
		return &rx_buffer[0];
	} else if (cnt[2] == 1) {
		cnt[2]++;
		return &tx_buffer[0];
	}
	return NULL;
}

static void fsl_free__(void *mem)
{
	UNUSED(mem);
	return;
}

void __get_mem_partitions_addr(int size, struct platform_memory_info* mem_info);
void __get_mem_partitions_addr(int size, struct platform_memory_info* mem_info)
{
	if (mem_info == NULL) {
		return;
	}
	int i= 0;
	for (; i < size; i++) {
		fill_mem_partition_info(&s_pltfrm__, &mem_info[i]);
	}
}

/*****************************************************************************/
int console_print_init();
int console_print_test();

int console_print_init()
{
	int err = 0;

	// sys_init() -> global_sys_init() -> fill_platform_parameters()
	s_pltfrm__.param.console_type = PLTFRM_CONSOLE_DUART;
	s_pltfrm__.param.console_id = (uint8_t)g_init_data.sl_info.uart_port_id;
	if (s_pltfrm__.param.console_id == 0) {
		s_pltfrm__.param.console_type = PLTFRM_CONSOLE_NONE;
	}

	// sys_init() -> global_sys_init() -> platform_init()
	s_pltfrm__.platform_clk = g_init_data.sl_info.platform_clk;
	if (s_pltfrm__.platform_clk == 0) {
		s_pltfrm__.platform_clk = 800000;
	}
	sys.platform_ops.h_platform = &s_pltfrm__;

	// sys_init_platform() -> pltfrm_init_mem_partitions_cb() -> build_mem_partitions_table() 
	s_pltfrm__.ccsr_base = (uint32_t)g_init_data.sl_info.ccsr_vaddr;

	// sys_init_platform() -> pltfrm_init_console_cb()
	err = pltfrm_init_console_cb__(sys.platform_ops.h_platform);
	if (err != 0)
		return err;

	return 0;
}


/*****************************************************************************/
int console_print_test()
{
	pr_info("AIOP core %d: Console Print Test passed\n", core_get_id());
	return 0;
}
