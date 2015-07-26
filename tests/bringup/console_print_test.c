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
#include "fsl_types.h"
#include "sys.h"
#include "platform.h"
#include "fsl_malloc.h"
#include "console.h"
#include "../../../tests/bringup/console_print_test.h"
#include "fsl_string.h"


int console_print_init();
int console_print_test();

extern t_system sys;

char pre_console_buf[1024];
uint8_t tx_buffer[1024];
uint8_t rx_buffer[1024];


/* Global variables*/
t_platform pltfrm;
t_duart_uart p_uart;
t_duart_driver_param  p_driver_param;

/*****************************************************************************/
static int console_print_cb(fsl_handle_t h_console_dev,
				uint8_t *p_data, uint32_t size)
{
	int err;

	err = duart_tx(h_console_dev, p_data, size);
	if (err != 0)
		return 0;

	return (int)size;
}

/*****************************************************************************/
static int console_get_line_cb(fsl_handle_t h_console_dev,
				uint8_t *p_data, uint32_t size)
{
	uint32_t count;

	count = duart_rx(h_console_dev, p_data, size);

	return (int)count;
}

static void set_default_param(t_duart_driver_param *p_driver_param)
{
	p_driver_param->rec_trigger_level      = DEFAULT_REC_TRIGGER_LEVEL;
	p_driver_param->low_space_alert        = DEFAULT_LOW_SPACE_ALERT;
	p_driver_param->rx_buffer_size         = DEFAULT_RX_BUFFER_SIZE;
	p_driver_param->rx_timeout             = DEFAULT_RX_TIMEOUT;
	p_driver_param->tx_buffer_size         = DEFAULT_TX_BUFFER_SIZE;
	p_driver_param->en_modem_stat_intr     = DEFAULT_EN_MODEM_STAT_INTR;
	p_driver_param->en_rec_line_stat_intr  = DEFAULT_EN_REC_LINE_STAT_INTR;
	p_driver_param->en_trans_hold_reg_empty_intr =
		DEFAULT_EN_TRANS_HOLD_REG_EMPTY_INTR;
	p_driver_param->en_rec_data_avail_intr = DEFAULT_EN_REC_DATA_AVAIL_INTR;
	p_driver_param->dma_mode_select        = DEFAULT_DMA_MODE_SELECT;
	p_driver_param->enable_fifo            = DEFAULT_ENABLE_FIFO;
	p_driver_param->loop_back_mode         = DEFAULT_LOOP_BACK_MODE;
}


/************************************************************************/
static fsl_handle_t static_duart_config(t_duart_uart_param *p_duart_param)
{
	int                 i;

	/* Allocate memory for the DUART UART data structure */

	if (!(&p_uart)) {
		return NULL;
	}

	/* Allocate memory for the parameter structures */

	if (!(&p_driver_param)) {
		return NULL;
	}

	p_uart.p_driver_param = &p_driver_param;

	/* Set default parameters */
	set_default_param(&p_driver_param);

	/* Copy user params to relevant internal parameter structure(s) */
	p_uart.initialized         = 0;

	p_driver_param.baud_rate     = p_duart_param->baud_rate;
	p_driver_param.parity       = p_duart_param->parity;
	p_driver_param.stop_bits     = p_duart_param->stop_bits;
	p_driver_param.data_bits     = p_duart_param->data_bits;

	p_uart.p_mem_map            = (t_duart_mem_map *)UINT_TO_PTR(
		p_duart_param->base_address);
	p_uart.system_clock_mhz      = p_duart_param->system_clock_mhz;
	p_uart.irq                 = p_duart_param->irq;
	p_uart.h_app               = p_duart_param->h_app;
	p_uart.f_tx_conf            = p_duart_param->f_tx_conf;
	p_uart.f_low_space_alert     = p_duart_param->f_low_space_alert;
	p_uart.f_exceptions        = p_duart_param->f_exceptions;
	p_uart.flow_control         = p_duart_param->flow_control;
	p_uart.poll_mode            = DEFAULT_POLL_MODE;
	p_uart.lf2crlf             = DEFAULT_POLL_LF_2Crlf;
	p_uart.numcc_entries        = DEFAULT_NUM_cc_entries;

	strncpy(&(p_uart.p_control_char[0]), DEFAULT_CONTROL_CHAR,	p_uart.numcc_entries);

	{
		char tempb[] = DEFAULT_CONTROL_CHAR_REJECT;

		for (i = 0; i < DEFAULT_NUM_cc_entries; i++) {
			if (tempb[i] == '\0')
				p_uart.p_control_char_reject[i] = 0;
			else
				p_uart.p_control_char_reject[i] = 1;
		}
	}

	return &p_uart;
}


static int check_driver_init_parameters(t_duart_driver_param *p_driver_param)
{

	if ((p_driver_param->baud_rate < BAUD_RATE_MIN_VAL) ||
			(p_driver_param->baud_rate > BAUD_RATE_MAX_VAL)){
			return -EINVAL;
	}

	if ((p_driver_param->parity != E_DUART_PARITY_ODD)   &&
			(p_driver_param->parity != E_DUART_PARITY_EVEN)  &&
			(p_driver_param->parity != E_DUART_PARITY_SPACE) &&
			(p_driver_param->parity != E_DUART_PARITY_MARK)  &&
			(p_driver_param->parity != E_DUART_PARITY_NONE)){
			return -EINVAL;
	}

	if ((p_driver_param->stop_bits != E_DUART_STOP_BITS_1) &&
			(p_driver_param->stop_bits != E_DUART_STOP_BITS_2)){
			return -EINVAL;
	}

	if ((p_driver_param->data_bits != E_DUART_DATA_BITS_5) &&
			(p_driver_param->data_bits != E_DUART_DATA_BITS_6) &&
			(p_driver_param->data_bits != E_DUART_DATA_BITS_7) &&
			(p_driver_param->data_bits != E_DUART_DATA_BITS_8)){
			return -EINVAL;
	}
	return 0;

}

/************************************************************************/
static int check_init_parameters(t_duart_uart *p_uart, fsl_handle_t params)
{
	t_duart_driver_param  *p_driver_param = p_uart->p_driver_param;
	int err;
	UNUSED(params);

	ASSERT_COND_LIGHT(p_driver_param);

	err = check_driver_init_parameters(p_driver_param);
	if(err)
		return err;

	if ((p_uart->flow_control != E_DUART_HW_FLOW_CONTROL) &&
		(p_uart->flow_control != E_DUART_NO_FLOW_CONTROL)){
		return -EINVAL;
	}

	if (p_uart->system_clock_mhz == 0){
		return -EINVAL;
	}

	return 0;
}




static int static_duart_init(fsl_handle_t duart)
{
	t_duart_uart         *p_uart = (t_duart_uart *)duart;
	t_duart_driver_param  *p_driver_param;
	t_duart_mem_map       *p_mem_map;
	uint32_t            baud_rate_factor;
	uint8_t             tmp_reg;

	if (check_init_parameters(p_uart, p_uart->p_driver_param))
		return -EINVAL;


	p_driver_param = p_uart->p_driver_param;

	/* Get needed values from parameters */
	p_mem_map                = p_uart->p_mem_map;
	p_uart->low_space_alert   = p_driver_param->low_space_alert;
	p_uart->rx_buffer_size    = p_driver_param->rx_buffer_size;
	p_uart->rx_timeout       = p_driver_param->rx_timeout;
	p_uart->tx_buffer_size    = p_driver_param->tx_buffer_size;
	p_uart->enable_fifo      = p_driver_param->enable_fifo;

	/* global Rx buffer */

	p_uart->p_rx_buffer = &rx_buffer[0];

	if (!p_uart->p_rx_buffer) {
		return -ENOMEM;
	}

	/* global Tx buffer */

	p_uart->p_tx_buffer = &tx_buffer[0];

	if (p_uart->p_tx_buffer == 0) {
		return -ENOMEM;
	}

	p_uart->rx_buffer_put_pos = p_uart->rx_buffer_get_pos = 0;
	p_uart->rx_buffer_count = 0;

	/* Reset ulsr by reading the register*/
	tmp_reg = ioread8(&p_mem_map->ulsr);
	UNUSED(tmp_reg);
	/* Set parity parameters according to user choice. */
	switch (p_driver_param->parity) {
	case E_DUART_PARITY_NONE:
		p_driver_param->enable_parity = 0;
		p_driver_param->stick_parity  = 0;
		p_driver_param->even_parity   = 0;
		break;

	case E_DUART_PARITY_ODD:
		p_driver_param->enable_parity = 1 ;
		p_driver_param->stick_parity  = 0;
		p_driver_param->even_parity   = 0;
		break;

	case E_DUART_PARITY_EVEN:
		p_driver_param->enable_parity = 1 ;
		p_driver_param->stick_parity  = 0;
		p_driver_param->even_parity   = 1 ;
		break;

	case E_DUART_PARITY_SPACE:
		p_driver_param->enable_parity = 1 ;
		p_driver_param->stick_parity  = 1 ;
		p_driver_param->even_parity   = 1 ;
		break;

	case E_DUART_PARITY_MARK:
		p_driver_param->enable_parity = 1 ;
		p_driver_param->stick_parity  = 1 ;
		p_driver_param->even_parity   = 0;
		break;
	}

	/* Set baud rate */
	baud_rate_factor = p_uart->system_clock_mhz*1000000 /
		p_driver_param->baud_rate / 16;

	/* Set access to UDMB/UDLB/UAFR */
	iowrite8(ULCR_DLAB, &p_mem_map->ulcr);

	/* Make sure that that the data has been written */
	tmp_reg = ioread8(&p_mem_map->ulcr);
	UNUSED(tmp_reg);

	iowrite8((uint8_t)((baud_rate_factor >> 8) & 0xff), &p_mem_map->UDMB);
	iowrite8((uint8_t)(baud_rate_factor & 0xff), &p_mem_map->UDLB);

	/* Intialize registers */
	tmp_reg = (uint8_t)((p_driver_param->stick_parity * ULCR_SP)   |
		(p_driver_param->even_parity * ULCR_EPS)   |
		(p_driver_param->enable_parity * ULCR_PEN) |
		(p_driver_param->stop_bits * ULCR_NTSB)    |
		p_driver_param->data_bits);

	iowrite8(tmp_reg, &p_mem_map->ulcr);


	if (p_driver_param->enable_fifo) {
		tmp_reg = (uint8_t)(p_driver_param->enable_fifo);
		iowrite8(tmp_reg, &p_mem_map->UFCR);

		tmp_reg = (uint8_t)(p_driver_param->enable_fifo | UFCR_RFR |
			UFCR_TFR);
		iowrite8(tmp_reg, &p_mem_map->UFCR);

		tmp_reg = (uint8_t)(p_driver_param->rec_trigger_level |
			(p_driver_param->dma_mode_select * UFCR_DMS) |
			p_driver_param->enable_fifo);
	} else
		tmp_reg = 0;

	/* Reset FIFO and internal shift register */
	iowrite8(tmp_reg, &p_mem_map->UFCR);

	tmp_reg = ioread8(&p_mem_map->URBR);
	UNUSED(tmp_reg);
	tmp_reg = (uint8_t)(p_driver_param->loop_back_mode * UMCR_LOOP);

	if (p_uart->flow_control == E_DUART_HW_FLOW_CONTROL)
		tmp_reg |= (uint8_t)UMCR_RTS;

	iowrite8(tmp_reg, &p_mem_map->umcr);

	if (!p_uart->poll_mode)
		tmp_reg = (uint8_t)((p_driver_param->en_modem_stat_intr *
			UIER_EMSI)   |
			(p_driver_param->en_rec_line_stat_intr * UIER_ERLSI) |
			(p_driver_param->en_rec_data_avail_intr * UIER_ERDAI));
	else
		tmp_reg = 0;     /* Close all Interrupts in Poll Mode */

	iowrite8(tmp_reg, &p_mem_map->UIER);

	p_uart->p_driver_param = NULL;

	p_uart->initialized = 1 ;

	return 0;
}


static int platform_en_console(fsl_handle_t h_platform)
{
	t_platform          *pltfrm = (t_platform *)h_platform;
	t_duart_uart_param  duart_uart_param;
	fsl_handle_t        uart;
	int           err = 0;

	extern struct aiop_init_info g_init_data;
	uint32_t ccsr_vaddr = (uint32_t)g_init_data.sl_info.ccsr_vaddr;
	uint32_t uart_port_offset[] = {
	                               0,
	                               SOC_PERIPH_OFF_DUART1,
	                               SOC_PERIPH_OFF_DUART2,
	                               SOC_PERIPH_OFF_DUART3,
	                               SOC_PERIPH_OFF_DUART4
	};
	SANITY_CHECK_RETURN_ERROR(pltfrm, ENODEV);

	if (pltfrm->param.console_type == PLTFRM_CONSOLE_NONE)
		return 0;

	SANITY_CHECK_RETURN_ERROR((pltfrm->param.console_type == PLTFRM_CONSOLE_DUART), ENOTSUP);

	if( g_init_data.sl_info.uart_port_id > 4 )
		RETURN_ERROR(MAJOR, EAGAIN, ("DUART"));

	/* Fill DUART configuration parameters */
	duart_uart_param.irq                = -1;
	/*Use offset for ccsr */
	duart_uart_param.base_address       = ccsr_vaddr + uart_port_offset[ g_init_data.sl_info.uart_port_id];
	duart_uart_param.system_clock_mhz   = (platform_get_system_bus_clk(pltfrm) / 1000);
	duart_uart_param.baud_rate          = 115200;
	duart_uart_param.parity             = E_DUART_PARITY_NONE;
	duart_uart_param.data_bits          = E_DUART_DATA_BITS_8;
	duart_uart_param.stop_bits          = E_DUART_STOP_BITS_1;
	duart_uart_param.flow_control       = E_DUART_NO_FLOW_CONTROL;
	duart_uart_param.h_app              = NULL;
	duart_uart_param.f_low_space_alert  = NULL;
	duart_uart_param.f_exceptions       = NULL;
	duart_uart_param.f_tx_conf          = NULL;

	/* Configure and initialize DUART driver */
	uart = static_duart_config(&duart_uart_param);
	if (!uart) return -EINVAL;

	/* Configure polling mode */
	err = duart_config_poll_mode(uart, 1);
	if (err != 0) return -EINVAL;

	/* Convert end-of-line indicators */
	err = duart_config_poll_lf2crlf(uart, 1);
	if (err != 0) return -EINVAL;

	/* Prevent blocking */
	err = duart_config_rx_timeout(uart, 0);
	if (err != 0) return -EINVAL;

	err = static_duart_init(uart);
	if (err != 0) return -EINVAL;

	/* Lock DUART handle in system */
/*err = sys_add_handle(uart, FSL_OS_MOD_UART, 1, pltfrm->param.console_id);
	if (err != 0) return -EINVAL;
*/
	pltfrm->uart = uart;
	pltfrm->duart_id = pltfrm->param.console_id;

	return 0;
}

/*****************************************************************************/
static int pltfrm_init_console_cb(fsl_handle_t h_platform)
{
	t_platform  *pltfrm = (t_platform *)h_platform;
	int     err;

	ASSERT_COND_LIGHT(pltfrm);

	/* Master partition - register DUART console */
	err = platform_en_console(pltfrm);
	if (err != 0)
		return -EINVAL;

	err = sys_register_console(pltfrm->uart, console_print_cb,
					console_get_line_cb);
	if (err != 0)
		return -EINVAL;

	return 0;
}


int console_print_init()
{
	//t_sys_param sys_param = {0};
	t_platform_ops           *pltfrm_ops;
	int err;
	extern struct aiop_init_info g_init_data;
	//t_platform *pltfrm;
	sys.p_pre_console_buf = &pre_console_buf[0];
//	pltfrm = fsl_os_malloc(sizeof(t_platform));
//	if(!pltfrm) return -ENOMEM;
	pltfrm.param.clock_in_freq_khz = g_init_data.sl_info.sys_clk; //TODO check value
	if(pltfrm.param.clock_in_freq_khz == 0)
		pltfrm.param.clock_in_freq_khz = 400000;
	pltfrm.param.console_type = PLTFRM_CONSOLE_DUART;
	pltfrm.param.console_id = UART_ID;

	pltfrm_ops = &(sys.platform_ops);

	pltfrm_ops->f_init_console = pltfrm_init_console_cb;
	pltfrm_ops->h_platform = &pltfrm;
	//sys->platform_ops->f_free_console = pltfrm_free_console_cb;

	if (sys.platform_ops.f_init_console) {
		err = sys.platform_ops.f_init_console(sys.platform_ops.h_platform);
		if (err != 0) return err;
	}

	return 0;
}


/*****************************************************************************/
int console_print_test()
{
	pr_info("AIOP core %d: Console Print Test passed\n", core_get_id());
	return 0;
}
