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

/**

@File          duart.c

@Description   DUART specific routines.
 *//***************************************************************************/

#include "fsl_malloc.h"
#include "common/fsl_string.h"
#include "fsl_dbg.h"
#include "fsl_io.h"
#include "inc/fsl_gen.h"

#include "duart.h"
#ifdef AIOP
#include "fsl_mem_mng.h"
#endif

/************************************************************************/
/* Exception Routines                                                   */
/************************************************************************/
static void check_if_finished_transmission(t_duart_uart     *p_uart)
{
	uint8_t tmp_reg;
	if (p_uart->tx_buffer_pos == p_uart->tx_buffer_count) {
		tmp_reg = ioread8(&p_uart->p_mem_map->UIER);
		tmp_reg &= ~UIER_ETHREI;
		iowrite8(tmp_reg, &p_uart->p_mem_map->UIER);

		p_uart->tx_buffer_count = 0;
		p_uart->tx_buffer_pos  = 0;

		/* Call transmit confirm callback.*/
		if (p_uart->f_tx_conf)
			p_uart->f_tx_conf(p_uart->h_app,
			                  p_uart->p_tx_buffer);
	}
}
static void intr_handler_empty_threi(uint8_t intr_status, fsl_handle_t duart)
{
	t_duart_uart     *p_uart = duart;


	if ((intr_status & UIIR_THREI) != 0) {

		if (p_uart->initialized == 0)
			return;

		/* There are still bytes that need to be transmitted */
		if (p_uart->tx_buffer_pos < p_uart->tx_buffer_count) {
			/* FIFO mode */
			if (p_uart->enable_fifo) {
				if (p_uart->flow_control ==
					E_DUART_HW_FLOW_CONTROL) {
					/* Transmit until the FIFO is full */
					while ((ioread8(
						&p_uart->p_mem_map->udsr)
						& UDSR_TXRDY) == 0) {
						/* If peripheral isn't
						 * ready or all data has already
						 * been sent, exit the interrupt
						 * handler.*/
						if (((ioread8(
							&p_uart->p_mem_map->umsr) &
							UMSR_CTS) == 0)  ||
							(p_uart->tx_buffer_pos >=
								p_uart->tx_buffer_count))
							return;

						iowrite8(p_uart->p_tx_buffer[
						                             p_uart->tx_buffer_pos++],
						                             &p_uart->p_mem_map->UTHR);

						/* Check if received data is
						 * available */
						if ((ioread8(
							&p_uart->p_mem_map->ulsr) &
							ULSR_DR) != 0)
							return;
					}
				}
				else {/* No Flow Control */
					/* Transmit until the FIFO is full */
					while ((ioread8(
						&p_uart->p_mem_map->udsr) &
						UDSR_TXRDY) == 0) {
						/* If all data has been sent,
						 * exit the interrupt handler */
						if (p_uart->tx_buffer_pos >=
							p_uart->tx_buffer_count)
							return;

						iowrite8(p_uart->p_tx_buffer[
						                             p_uart->tx_buffer_pos++],
						                             &p_uart->p_mem_map->UTHR);

						/* Check if received data is
						 * available */
						if ((ioread8(
							&p_uart->p_mem_map->ulsr) &
							ULSR_DR) != 0)
							return;
					}
				}
				return;
			}
			else {/* Non FIFO mode - transmit only one byte */
				if (p_uart->flow_control ==
					E_DUART_HW_FLOW_CONTROL) {
					if ((ioread8(&p_uart->p_mem_map->umsr)
						& UMSR_CTS) == 0)
						return;
				}

				iowrite8(p_uart->p_tx_buffer[
				                             p_uart->tx_buffer_pos++],
				                             &p_uart->p_mem_map->UTHR);
			}

			return;
		}

		/* Check if finished transmission */
		check_if_finished_transmission(p_uart);
	}
}

static void intr_handler(fsl_handle_t duart)
{
	t_duart_uart     *p_uart = duart;
	uint8_t         intr_status, umcr, ulsr, data;



	intr_status = ioread8(&p_uart->p_mem_map->UIIR);
	intr_status = (uint8_t)(intr_status & (uint8_t)(DUART_INTR_MASK));

	/* Receiver Line Status Interrupt */
	if (intr_status == UIIR_RLSI) {
		ulsr = ioread8(&p_uart->p_mem_map->ulsr);

		if (ulsr & ULSR_BI)
			ioread8(&p_uart->p_mem_map->URBR);

		/* Overrun error, Receiver FIFO error, Framing Error or Parity
		* Error */
		if (p_uart->f_exceptions &&
			(ulsr & (ULSR_OE | ULSR_RFE | ULSR_FE | ULSR_PE))) {
			p_uart->f_exceptions(p_uart->h_app, ulsr, 0);
		}
	}

	/* Either receiver data available or receiver timeout, call store */
	if ((intr_status == UIIR_CTOI) || (intr_status == UIIR_RDAI)) {
		do {
			data = ioread8(&p_uart->p_mem_map->URBR);

			if (p_uart->flow_control == E_DUART_HW_FLOW_CONTROL) {
				umcr = ioread8(&p_uart->p_mem_map->umcr);
				umcr |= UMCR_RTS;
				iowrite8(umcr, &p_uart->p_mem_map->umcr);
			}

			if (!p_uart->initialized)
				return;                 /* Ignore chars during
							Init */

			/* Put in buffer */
			p_uart->p_rx_buffer[p_uart->rx_buffer_put_pos++] = data;

			/*If past buffer size cycle over to zero position.*/
			if (p_uart->rx_buffer_put_pos >= p_uart->rx_buffer_size)
				p_uart->rx_buffer_put_pos = 0;

			/*Increment count. In case count equals size overwrite
			* buffer.*/
			if (p_uart->rx_buffer_count < p_uart->rx_buffer_size)
				p_uart->rx_buffer_count++;
			else
				p_uart->rx_buffer_get_pos++;

			if (p_uart->rx_buffer_size - p_uart->rx_buffer_count <=
				p_uart->low_space_alert) {
				if (p_uart->f_low_space_alert)
					p_uart->f_low_space_alert(
						p_uart->h_app);
			}
		} while ((ioread8(&p_uart->p_mem_map->ulsr) & ULSR_DR) != 0);
	}


	/* Transmitter holding register empty */
	if (intr_status == 0){
		data = ioread8(&p_uart->p_mem_map->umsr);
		UNUSED(data);
	}

	intr_handler_empty_threi(intr_status, duart);


}

/************************************************************************/
/* Config Internal Routines                                             */
/************************************************************************/

static int check_driver_init_parameters(t_duart_driver_param *p_driver_param)
{

	if ((p_driver_param->baud_rate < BAUD_RATE_MIN_VAL) ||
			(p_driver_param->baud_rate > BAUD_RATE_MAX_VAL))
			RETURN_ERROR(MAJOR, ERANGE, ("rate should be [%d-%d]",
				BAUD_RATE_MIN_VAL, BAUD_RATE_MAX_VAL));

	if ((p_driver_param->parity != E_DUART_PARITY_ODD)   &&
			(p_driver_param->parity != E_DUART_PARITY_EVEN)  &&
			(p_driver_param->parity != E_DUART_PARITY_SPACE) &&
			(p_driver_param->parity != E_DUART_PARITY_MARK)  &&
			(p_driver_param->parity != E_DUART_PARITY_NONE))
			RETURN_ERROR(MAJOR, E_INVALID_SELECTION, ("parity"));

	if ((p_driver_param->stop_bits != E_DUART_STOP_BITS_1) &&
			(p_driver_param->stop_bits != E_DUART_STOP_BITS_2))
			RETURN_ERROR(MAJOR, E_INVALID_SELECTION, ("stop bits"));

	if ((p_driver_param->data_bits != E_DUART_DATA_BITS_5) &&
			(p_driver_param->data_bits != E_DUART_DATA_BITS_6) &&
			(p_driver_param->data_bits != E_DUART_DATA_BITS_7) &&
			(p_driver_param->data_bits != E_DUART_DATA_BITS_8))
			RETURN_ERROR(MAJOR, E_INVALID_SELECTION, ("data bits"));
	return 0;

}

/************************************************************************/
static int check_init_parameters(t_duart_uart *p_uart, fsl_handle_t params)
{
	t_duart_driver_param  *p_driver_param = p_uart->p_driver_param;
	int err;
	UNUSED(params);

	ASSERT_COND(p_driver_param);

	err = check_driver_init_parameters(p_driver_param);
	if(err)
		return err;

	if ((p_uart->flow_control != E_DUART_HW_FLOW_CONTROL) &&
		(p_uart->flow_control != E_DUART_NO_FLOW_CONTROL))
		RETURN_ERROR(MAJOR, E_INVALID_SELECTION, ("flow control"));

	if (p_uart->system_clock_mhz == 0)
		RETURN_ERROR(MAJOR, EDOM,
				("system bus frequency should be positive"));

	return 0;
}

/************************************************************************/
/* Init Internal Routines                                               */
/************************************************************************/

/************************************************************************/
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
/* RunTime Internal Routines                                            */
/************************************************************************/

/************************************************************************/
static int duart_poll_tx(fsl_handle_t duart, uint8_t *data, uint32_t size)
{
	t_duart_uart *p_uart = duart;
	uint8_t     status;
	int         count;
	uint32_t    tries = 0;

	for (count = 0; count < size; count++) {
		tries = 0;

		/* Make sure transmitter is ready */
		do {
			status = ioread8(&p_uart->p_mem_map->ulsr);
			tries++;
			if (tries >= 1000000000)
				return ERROR_CODE(EBUSY);
		} while ((status & ULSR_THRE) == 0);

		/* In HW handshake make sure ready to transmit */
		if (p_uart->flow_control == E_DUART_HW_FLOW_CONTROL) {
			tries = 0;
			do {
				status = ioread8(&p_uart->p_mem_map->umsr);
				tries++;
				if (tries >= 10000)
					return ERROR_CODE(EBUSY);
			} while ((status & UMSR_CTS) == 0);
		}

		/* Write actual data to transmitter */
		if (p_uart->lf2crlf && (*data == '\n')) {
			iowrite8('\r', &p_uart->p_mem_map->UTHR);

			tries = 0;
			do {
				status = ioread8(&p_uart->p_mem_map->ulsr);
				tries++;
				if (tries >= 1000000000)
					return ERROR_CODE(EBUSY);
			} while ((status & ULSR_THRE) == 0);

			iowrite8(*data++, &p_uart->p_mem_map->UTHR);
		} else
			iowrite8(*data++, &p_uart->p_mem_map->UTHR);
	}

	/* Call transmit confirm callback.*/
	if (p_uart->f_tx_conf)
		p_uart->f_tx_conf(p_uart->h_app, data);

	return 0;
}

/************************************************************************/
static uint32_t duart_poll_rx(
	fsl_handle_t duart,
	uint8_t *buffer,
	uint32_t size)
{
	t_duart_uart *p_uart = duart;
	uint8_t     data, umcr, status;
	uint32_t    count = 0;
	int32_t     timeout;
	uint32_t    i;

	while (count < size) {
		timeout = p_uart->rx_timeout;

		/* Wait for input char */
		status = ioread8(&p_uart->p_mem_map->ulsr);

		if (status & ULSR_BI) {
#ifdef MPC837x
			/* Workaround for unexplained behavior in
			* 837x optimized code */
			data = ioread8(p_uart->p_mem_map->umsr);
#endif /* MPC837x */
			/* Ignore break signal */
			data = ioread8(&p_uart->p_mem_map->URBR);
			UNUSED(data);
			continue;
		}

		while ((status & ULSR_DR) == 0) {
			if (timeout > 0)
				timeout--;

			if (timeout == 0)
				return count;

			status = ioread8(&p_uart->p_mem_map->ulsr);
		}

		/* Read the data */
		data = ioread8(&p_uart->p_mem_map->URBR);

		if (p_uart->flow_control == E_DUART_HW_FLOW_CONTROL) {
			umcr = ioread8(&p_uart->p_mem_map->umcr);
			umcr |= UMCR_RTS;
			iowrite8(umcr, &p_uart->p_mem_map->umcr);
		}
		/* Check for control char and handle.*/
		for (i = 0; i < p_uart->numcc_entries; i++) {
			if (data == p_uart->p_control_char[i]) {
				if (!p_uart->p_control_char_reject[i]) {
					count++;
					*buffer++ = data;
				}
				return count;
			}
		}

		/* Put in buffer */
		*buffer++ = data;
		count++;
	}
	return count;
}

/************************************************************************/
static void duart_free_local(fsl_handle_t duart)
{
	t_duart_uart *p_uart = duart;

	if (!duart)
		return;

	if (p_uart->p_rx_buffer) {
#ifdef AIOP
		fsl_free(p_uart->p_rx_buffer);
#else
		fsl_os_free(p_uart->p_rx_buffer);
#endif
		p_uart->p_rx_buffer = NULL;
	}

	if (p_uart->p_tx_buffer) {
#ifdef AIOP
		fsl_free(p_uart->p_tx_buffer);
#else
		fsl_os_free(p_uart->p_tx_buffer);
#endif
		p_uart->p_tx_buffer = NULL;
	}
#ifdef AIOP
	fsl_free(duart);
#else
	fsl_os_free(duart);
#endif
}


/************************************************************************/
/* Config Routines                                                      */
/************************************************************************/

/************************************************************************/
fsl_handle_t duart_config(t_duart_uart_param *p_duart_param)
{
	t_duart_uart         *p_uart;
	t_duart_driver_param  *p_driver_param;
	int                 i;

	SANITY_CHECK_RETURN_VALUE(p_duart_param, ENODEV, NULL);

	/* Allocate memory for the DUART UART data structure */
#ifdef AIOP
	p_uart = (t_duart_uart *)fsl_malloc(sizeof(t_duart_uart),0);
#else
	p_uart = (t_duart_uart *)fsl_os_malloc(sizeof(t_duart_uart));
#endif
	if (!p_uart) {
		REPORT_ERROR(MAJOR, ENOMEM, ("DUART driver structure"));
		return NULL;
	}
	memset(p_uart, 0, sizeof(t_duart_uart));

	/* Allocate memory for the parameter structures */
#ifdef AIOP
	p_driver_param = (t_duart_driver_param *)fsl_malloc(
			sizeof(t_duart_driver_param),0);
#else
	p_driver_param = (t_duart_driver_param *)fsl_os_malloc(
			sizeof(t_duart_driver_param));
#endif
	if (!p_driver_param) {
		REPORT_ERROR(MAJOR, ENOMEM, ("DUART driver parameters"));
#ifdef AIOP
	fsl_free(p_uart);
#else
	fsl_os_free(p_uart);
#endif
		return NULL;
	}
	memset(p_driver_param, 0, sizeof(t_duart_driver_param));

	p_uart->p_driver_param = p_driver_param;

	/* Set default parameters */
	set_default_param(p_driver_param);

	/* Copy user params to relevant internal parameter structure(s) */
	p_uart->initialized         = 0;

	p_driver_param->baud_rate     = p_duart_param->baud_rate;
	p_driver_param->parity       = p_duart_param->parity;
	p_driver_param->stop_bits     = p_duart_param->stop_bits;
	p_driver_param->data_bits     = p_duart_param->data_bits;

	p_uart->p_mem_map            = (t_duart_mem_map *)UINT_TO_PTR(
		p_duart_param->base_address);
	p_uart->system_clock_mhz      = p_duart_param->system_clock_mhz;
	p_uart->irq                 = p_duart_param->irq;
	p_uart->h_app               = p_duart_param->h_app;
	p_uart->f_tx_conf            = p_duart_param->f_tx_conf;
	p_uart->f_low_space_alert     = p_duart_param->f_low_space_alert;
	p_uart->f_exceptions        = p_duart_param->f_exceptions;
	p_uart->flow_control         = p_duart_param->flow_control;
	p_uart->poll_mode            = DEFAULT_POLL_MODE;
	p_uart->lf2crlf             = DEFAULT_POLL_LF_2Crlf;
	p_uart->numcc_entries        = DEFAULT_NUM_cc_entries;

	strncpy(p_uart->p_control_char, DEFAULT_CONTROL_CHAR,
		DEFAULT_NUM_cc_entries);

	{
		char tempb[] = DEFAULT_CONTROL_CHAR_REJECT;

		for (i = 0; i < DEFAULT_NUM_cc_entries; i++) {
			if (tempb[i] == '\0')
				p_uart->p_control_char_reject[i] = 0;
			else
				p_uart->p_control_char_reject[i] = 1;
		}
	}

	return p_uart;
}

/************************************************************************/
int duart_config_rx_buffer_size(fsl_handle_t duart, uint32_t new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->rx_buffer_size = new_val;

	return 0;
}

/************************************************************************/
int duart_config_en_modem_stat_intr(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->en_modem_stat_intr = new_val;

	return 0;
}

/************************************************************************/
int duart_config_en_rec_line_stat_intr(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->en_rec_line_stat_intr = new_val;

	return 0;
}

/************************************************************************/
int duart_config_en_trans_hold_reg_empty_intr(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->en_trans_hold_reg_empty_intr = new_val;

	return 0;
}

/************************************************************************/
int duart_config_en_rec_data_avail_intr(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->en_rec_data_avail_intr = new_val;

	return 0;
}

/************************************************************************/
int duart_config_rec_trigger_level(
	fsl_handle_t duart,
	e_duart_uart_fifo_size new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->rec_trigger_level = new_val;

	return 0;
}

/************************************************************************/
int duart_config_rx_timeout(fsl_handle_t duart, int32_t new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->rx_timeout = new_val;

	return 0;
}

/************************************************************************/
int duart_config_dma_mode_select(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->dma_mode_select = new_val;

	return 0;
}

/************************************************************************/
int duart_config_enable_fifo(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->enable_fifo = new_val;

	return 0;
}

/************************************************************************/
int duart_config_loop_back_mode(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	p_uart->p_driver_param->loop_back_mode = new_val;

	return 0;
}

/************************************************************************/
int duart_config_control_char(
	fsl_handle_t   duart,
	char *p_control_char,
	int *p_control_char_reject,
	uint8_t numcc_entries)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;
	uint8_t     i;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(p_uart->p_driver_param, E_INVALID_STATE);

	if (numcc_entries > 8)
		return EDOM;

	p_uart->numcc_entries = numcc_entries;

	for (i = 0; i < numcc_entries; i++) {
		p_uart->p_control_char[i] = p_control_char[i];
		p_uart->p_control_char_reject[i] = p_control_char_reject[i];
	}

	return 0;
}


/************************************************************************/
int duart_config_poll_mode(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);

	p_uart->poll_mode = new_val;

	return 0;
}

/************************************************************************/
int duart_config_poll_lf2crlf(fsl_handle_t duart, int new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);

	p_uart->lf2crlf = new_val;

	return 0;
}

/************************************************************************/
/* Init Routines                                                        */
/************************************************************************/

/************************************************************************/
int duart_init(fsl_handle_t duart)
{
	t_duart_uart         *p_uart = (t_duart_uart *)duart;
	t_duart_driver_param  *p_driver_param;
	t_duart_mem_map       *p_mem_map;
	uint32_t            baud_rate_factor;
	uint8_t             tmp_reg;

	SANITY_CHECK_RETURN_ERROR(p_uart, ENODEV);

	if (check_init_parameters(p_uart, p_uart->p_driver_param))
		RETURN_ERROR(MAJOR, E_INVALID_STATE, NO_MSG);

	p_driver_param = p_uart->p_driver_param;

	/* Get needed values from parameters */
	p_mem_map                = p_uart->p_mem_map;
	p_uart->low_space_alert   = p_driver_param->low_space_alert;
	p_uart->rx_buffer_size    = p_driver_param->rx_buffer_size;
	p_uart->rx_timeout       = p_driver_param->rx_timeout;
	p_uart->tx_buffer_size    = p_driver_param->tx_buffer_size;
	p_uart->enable_fifo      = p_driver_param->enable_fifo;

	/* Allocate the Rx buffer */
#ifdef AIOP
	p_uart->p_rx_buffer = fsl_malloc(p_uart->rx_buffer_size,0);
#else
	p_uart->p_rx_buffer = fsl_os_malloc(p_uart->rx_buffer_size);
#endif
	if (!p_uart->p_rx_buffer) {
		duart_free_local(p_uart);
		RETURN_ERROR(MAJOR, ENOMEM, ("rx buffer"));
	}

	/* Allocate the Tx buffer */
#ifdef AIOP
	p_uart->p_tx_buffer = fsl_malloc(p_uart->tx_buffer_size,0);
#else
	p_uart->p_tx_buffer = fsl_os_malloc(p_uart->tx_buffer_size);
#endif
	if (p_uart->p_tx_buffer == 0) {
		duart_free_local(p_uart);
		RETURN_ERROR(MAJOR, ENOMEM, ("rx buffer"));
	}
	memset(p_uart->p_rx_buffer, 0, p_uart->rx_buffer_size);

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

#ifdef AIOP
	fsl_free(p_uart->p_driver_param);
#else
	fsl_os_free(p_uart->p_driver_param);
#endif
	p_uart->p_driver_param = NULL;

	p_uart->initialized = 1 ;

	return 0;
}

/************************************************************************/
/* RunTime Routines                                                     */
/************************************************************************/

/************************************************************************/
void duart_free(fsl_handle_t h_duart)
{
	duart_free_local(h_duart);
}


/************************************************************************/
int duart_tx(fsl_handle_t duart, uint8_t *data, uint32_t size)
{
	t_duart_uart     *p_uart = duart;
	uint32_t        tries = 0;
	uint8_t         tmp_reg;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);
	SANITY_CHECK_RETURN_ERROR(data , ENODEV);

	if (size == 0)
		return 0;

	if (p_uart->poll_mode)
		return duart_poll_tx(duart, data, size);

	/* Wait for previous transmit if needed.*/
	tries = 0;
	while (p_uart->tx_buffer_count > 0) {
		if (tries++ >= 100000000)
			RETURN_ERROR(MINOR, EBUSY, NO_MSG);
	}

	/* If size > buffer size use poll mode.
	* Poll mode uses the user buffer.*/
	if (size > p_uart->tx_buffer_size)
		return duart_poll_tx(duart, data, size);

	/* Copy data into internal buffer so calling
	* function can use buffer safely */

	memcpy(p_uart->p_tx_buffer, data, size);

	p_uart->tx_buffer_count = size;
	p_uart->tx_buffer_pos   = 0;

	/* Open transmit interrupt so the transmit will work */
	tmp_reg = ioread8(&p_uart->p_mem_map->UIER);
	tmp_reg |= UIER_ETHREI;

	iowrite8(tmp_reg, &p_uart->p_mem_map->UIER);

	/* Make sure the data has been written */
	tmp_reg = ioread8(&p_uart->p_mem_map->UIER);
	UNUSED(tmp_reg);

	return 0;
}

static int duat_rx_sanity_check(fsl_handle_t duart, uint8_t *buffer)
{
	SANITY_CHECK_RETURN_VALUE(duart , EDOM, -1);
	SANITY_CHECK_RETURN_VALUE(buffer, ENODEV, -1);
	return 0;
}

/************************************************************************/
uint32_t duart_rx(fsl_handle_t duart, uint8_t *buffer, uint32_t size)
{
	t_duart_uart   *p_uart = duart;
	uint32_t      actual_bytes = 0;
	uint8_t       data;
	int32_t       timeout = p_uart->rx_timeout;
	int           i, j;
	uint32_t      int_flags;

	if(duat_rx_sanity_check(duart,buffer))
		return 0;
	if (!size)
		return 0;

	if (p_uart->poll_mode)
		return duart_poll_rx(duart, buffer, size);

	while (actual_bytes < size) {
		/* Loop till chars arrive.*/
		if (p_uart->rx_buffer_count == 0) {
			if (timeout == 0)
				return actual_bytes;
			else {
				if (timeout > 0)
					timeout--;
				continue;
			}
		}

		timeout = p_uart->rx_timeout;

		int_flags = core_local_irq_save();        /* Lock Interrupts */

		for (j = 0; j < p_uart->rx_buffer_count; j++) {
			data = p_uart->p_rx_buffer[
			p_uart->rx_buffer_get_pos++];

			/* Cycle around if needed */
			if (p_uart->rx_buffer_get_pos >= p_uart->rx_buffer_size)
				p_uart->rx_buffer_get_pos = 0;

			p_uart->rx_buffer_count--;

			/* Check for control char and handle */
			for (i = 0; i < p_uart->numcc_entries; i++) {
				if (data == p_uart->p_control_char[i]) {
					if (!p_uart->p_control_char_reject[i]) {
						actual_bytes++;
						*buffer++ = data;
					}
					core_local_irq_restore(int_flags);
					goto DONE;
				}
			}
			/* Regular data - put in buffer */
			*buffer++ = data;
			actual_bytes++;

			if (actual_bytes == size)
				break;   /* More chars in buffer than needed */
		}

		core_local_irq_restore(int_flags);
	}

DONE:
	return actual_bytes;
}

/************************************************************************/
/* Runtime Control Routines                                             */
/************************************************************************/

/************************************************************************/
uint32_t duart_get_num_of_rx_char_in_buffer(fsl_handle_t duart)
{
	t_duart_uart   *p_uart = duart;
	uint8_t       status;

	SANITY_CHECK_RETURN_VALUE(duart , EDOM, 0);

	if (p_uart->poll_mode) {
		status = ioread8(&p_uart->p_mem_map->ulsr);
		if ((status & ULSR_DR) == 0)
			return 0;
		else
			return 1;
	} else
		return p_uart->rx_buffer_count;
}

/************************************************************************/
int duart_set_low_space_alert(fsl_handle_t duart, uint32_t new_val)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);

	p_uart->low_space_alert = new_val;

	return 0;
}


/************************************************************************/
int duart_set_break_signal(fsl_handle_t duart, int break_signal)
{
	t_duart_uart     *p_uart = (t_duart_uart *)duart;
	uint8_t         ulcr_val;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);

	ulcr_val = ioread8(&p_uart->p_mem_map->ulcr);
	if (break_signal)
		iowrite8((uint8_t)(ulcr_val | ULCR_SB),
		&p_uart->p_mem_map->ulcr);
	else
		iowrite8((uint8_t)(ulcr_val & (~ULCR_SB)),
		&p_uart->p_mem_map->ulcr);
	core_memory_barrier();

	return 0;
}

/************************************************************************/
int duart_set_baud_rate(fsl_handle_t duart, uint32_t baud_rate)
{
	t_duart_uart     *p_uart = (t_duart_uart *)duart;
	t_duart_mem_map   *p_mem_map;
	uint32_t        baud_rate_factor;
	uint8_t         ulcr_val;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);


	if ((baud_rate < 600) || (baud_rate > 512000))
		RETURN_ERROR(MAJOR, ERANGE,
		("baud rate should be [600-512000]"));

	p_mem_map = p_uart->p_mem_map;

	baud_rate_factor = p_uart->system_clock_mhz*1000000 / baud_rate / 16;

	/* Storing ulcr */
	ulcr_val = ioread8(&p_mem_map->ulcr);

	/* Set access to UDMB/UDLB/UAFR */
	iowrite8(ULCR_DLAB, &p_mem_map->ulcr);

	iowrite8((uint8_t)((baud_rate_factor >> 8) & 0xff), &p_mem_map->UDMB);
	iowrite8((uint8_t)(baud_rate_factor & 0xff), &p_mem_map->UDLB);

	/* Restoring ulcr */
	iowrite8(ulcr_val, &p_mem_map->ulcr);

	return 0;
}

#if (DEBUG_ERRORS > 0)
/************************************************************************/
int duart_dump_regs(fsl_handle_t duart)
{
	t_duart_uart *p_uart = (t_duart_uart *)duart;

	DECLARE_DUMP;

	SANITY_CHECK_RETURN_ERROR(duart, EDOM);

	DUMP_TITLE(p_uart->p_mem_map, ("DUART registers"));
	fsl_os_print("\r\n");

	DUMP_VAR((p_uart->p_mem_map), urbr_uthr_udlb);
	DUMP_VAR((p_uart->p_mem_map), uier_udmb);
	DUMP_VAR((p_uart->p_mem_map), uiir_ufcr_uafr);
	DUMP_VAR((p_uart->p_mem_map), ulcr);
	DUMP_VAR((p_uart->p_mem_map), umcr);
	DUMP_VAR((p_uart->p_mem_map), ulsr);
	DUMP_VAR((p_uart->p_mem_map), umsr);
	DUMP_VAR((p_uart->p_mem_map), uscr);
	DUMP_VAR((p_uart->p_mem_map), udsr);

	return 0;
}
#endif /* (DEBUG_ERRORS > 0) */
