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
/**
@File          console_print_test.h

@Description   Internal header file for DUART unit routines
*//***************************************************************************/
#ifndef __CONSOLE_PRINT_TEST_H
#define __CONSOLE_PRINT_TEST_H

#include "common/types.h"
#include "fsl_errors.h"
#include "../../../drivers/serial/include/fsl_duart.h"

/**
 *	0 = duart1_0
 * 	1 = duart1_1
 *	2 = duart2_0
 * 	3 = duart2_1
*/
#define UART_ID      2


#define URBR         urbr_uthr_udlb  /**< receiver buffer register */
#define UTHR         urbr_uthr_udlb  /**< transmitter holding register */
#define UDLB         urbr_uthr_udlb  /**< divisor least significant byte
					register */

#define UIER         uier_udmb       /**< interrupt enable register */
#define UDMB         uier_udmb       /**< divisor most significant byte
					register */

#define UIIR         uiir_ufcr_uafr  /**< interrupt ID register */
#define UFCR         uiir_ufcr_uafr  /**< FIFO control register */
#define UAFR         uiir_ufcr_uafr  /**< alternate function register */


#define NUM_CONTROL_CHARS   8


/* UIIR (Interrupt Identification Register) */
#define  UIIR_RLSI          0x06   /* priority 1 - receiver line status */
#define  UIIR_RDAI          0x04   /* priority 2 - receiver data available*/
#define  UIIR_THREI         0x02   /* priority 3 - transmitter holding register
					empty */
#define  UIIR_MSI           0x00   /* priority 4 - modem status */
#define  UIIR_CTOI          0x0c   /* priority 2 - character time-out*/

/* ULSR (Line Status Register) */
#define  ULSR_RFE           E_DUART_RECEIVER_FIFO_ERROR
#define  ULSR_OE            E_DUART_OVERRUN_ERROR
#define  ULSR_PE            E_DUART_PARITY_ERROR
#define  ULSR_FE            E_DUART_FRAMING_ERROR
#define  ULSR_BI            0x10   /* Occurs when data of logic 0 is received
					for a defined time */
#define  ULSR_DR            0x01   /* Data ready */
#define  ULSR_THRE          0x20   /* transmitter holding register empty */
#define  ULSR_TEMT          0x40   /* transmitter empty */

/* UMCR (Modem Control Register) */
#define  UMCR_RTS           0x02   /* Ready to send */
#define  UMCR_LOOP          0x10   /* Loop enable */

/* UMSR (Modem Status Register) */
#define  UMSR_CTS           0x10   /* CTS status */

/* UIER (Interrupt Enable Register) */
#define  UIER_ERDAI         0x01   /* enable received data available int. */
#define  UIER_ETHREI        0x02   /* enable transmitter holding register empty
					int. */
#define  UIER_ERLSI         0x04   /* enable receiver line status interrupt */
#define  UIER_EMSI          0x08   /* enable modem status interrput */

/* ULCR (Line Contorl Register) */
#define  ULCR_NTSB          0x04   /* number of stop bits */
#define  ULCR_EPS           0x10   /* Even parity selected */
#define  ULCR_PEN           0x08   /* Parity enable */
#define  ULCR_SB            0x40   /* Set break */
#define  ULCR_SP            0x20   /* Stick parity */
#define  ULCR_DLAB          0x80   /* Divisor latch access bit */

/* UFCR (FIFO Control Register) - write only */
#define  UFCR_RFR           0x02   /* Receiver FIFO reset */
#define  UFCR_TFR           0x04   /* Transmitter FIFO clear */
#define  UFCR_DMS           0x08   /* DMA mode select */

/*UDSR (DMA Status Register)*/
#define UDSR_RXRDY          0x01   /* Receiver ready, status depends on DMA
					mode selected*/
#define UDSR_TXRDY          0x02   /* Tranmitter ready, status depends on DMA
					mode selected */

#define DUART_FIFO_SIZE     16     /* FIFO size in bytes */

#define DUART_B_OFFSET      0x100  /* offset between uart0 and uart1 */
#define DUART_INTR_MASK     0xe    /* mask to get relavent bits in the DUART
					interrupt status register */

#define DEFAULT_EN_MODEM_STAT_INTR         1     /* Default value for the
						EnModemStatIntr variable */
#define DEFAULT_EN_REC_LINE_STAT_INTR       1    /* Default value for the
						EnRecLineStatIntr variable */
#define DEFAULT_EN_TRANS_HOLD_REG_EMPTY_INTR 1   /* Default value for the
					EnTransHoldRegEmptyIntr variable */
#define DEFAULT_EN_REC_DATA_AVAIL_INTR      1    /* Default value for the
						EnRecDataAvailIntr variable */
#define DEFAULT_REC_TRIGGER_LEVEL         E_DUART_FIFO_01 /* Default value for
						the recTriggerLevel variable */
#define DEFAULT_DMA_MODE_SELECT           1      /* Default value for the
						dmaModeSelect variable */
#define DEFAULT_ENABLE_FIFO              1       /* Default value for the
						enableFifo variable */
#define DEFAULT_LOOP_BACK_MODE            0      /* Default value for the
						loopBackMode variable */
#define DEFAULT_RX_BUFFER_SIZE            1024   /* Default value for the
						rxBufferSize variable */
#define DEFAULT_RX_TIMEOUT               1000000 /* Default value for the
						rxTimeout variable */
#define DEFAULT_TX_BUFFER_SIZE            1024   /* Default value for the
						txBufferSize variable */
#define DEFAULT_LOW_SPACE_ALERT           128    /* Default value for the
						lowSpaceAlert variable */
#define default_f_tx_conf                NULL    /* Default TxConf callback
						function */
#define DEFAULT_POLL_MODE                0       /* Default poll mode NO */
#define DEFAULT_NUM_cc_entries            2
#define DEFAULT_CONTROL_CHAR          "\r\3\0\0\0\0\0\0"
#define DEFAULT_CONTROL_CHAR_REJECT   "\0\1\0\0\0\0\0\0"  /* 0=0 otherwise=1 */
#define DEFAULT_POLL_LF_2Crlf             0      /*Default value for lf2crlf
						is false*/


/**************************************************************************//**
@Description   structure representing DUART UART memory map

*//***************************************************************************/
typedef struct t_duart_mem_map {
		uint8_t urbr_uthr_udlb;    /**< combined register for URBR,
						UTHR and UDLB */
		uint8_t uier_udmb;         /**< combined register for UIER
						and UDMB */
		uint8_t uiir_ufcr_uafr;    /**< combined register for UIIR,
						UFCR and UAFR */
		uint8_t ulcr;              /**< line control register */
		uint8_t umcr;              /**< MODEM control register */
		uint8_t ulsr;              /**< line status register */
		uint8_t umsr;              /**< MODEM status register */
		uint8_t uscr;              /**< scratch register */
		uint8_t RESERVED0[8];
		uint8_t udsr;              /**< DMA status register */
} t_duart_mem_map;


/**************************************************************************//**
@Description   DUART protocol initialization structure
*//***************************************************************************/
typedef struct t_duart_driver_param {
	e_duart_uart_data_bits data_bits;        /**< data bits per char */
	e_duart_uart_stop_bits stop_bits;        /**< stop bits per char */
	e_duart_uart_parity   parity;            /**< Parity - sets three
						fields below */
	e_duart_uart_fifo_size rec_trigger_level;/**< How many words in FIFO
						mode before trigger.
						[DEFAULT_recTriggerLevel] */

	int                 stick_parity;        /**< 1 for stick parity (0) */
	int                 even_parity;         /**< 1 for even parity      */
	int                 enable_parity;       /**< 1 for parity generation
						and checking */
	uint32_t            baud_rate;           /**< Baud rate to use - same
						for both Tx and Rx */

	uint32_t            low_space_alert;     /**< The size in bytes left in
						rx buffer when an overflow
						alert function is called.
						[DEFAULT_lowSpaceAlert] */
	uint32_t            rx_buffer_size;      /**< The size in bytes of the
						Rx buffer.
						[DEFAULT_rxBufferSize] */
	int32_t             rx_timeout;          /**< Number of times the
						driver checks if a specific
						char was received.
						[DEFAULT_rxTimeout] */
	uint32_t            tx_buffer_size;      /**< The size in bytes of the
						Tx buffer.
						[DEFAULT_txBufferSize] */
	int                 en_modem_stat_intr;  /**< Enable Modem Status
						Interrupt.
						[DEFAULT_en_ModemStatIntr] */
	int                 en_rec_line_stat_intr;  /**< Enable Receiver Line
						Status Interrupt.
						[DEFAULT_en_RecLineStatIntr] */
	int                 en_trans_hold_reg_empty_intr; /**< Enable
						Transmitter Hold Register Empty
						Interrupt. [DEFAULT_en_Trans
						HoldRegEmptyIntr] */
	int                 en_rec_data_avail_intr; /**< Enable Receiver Data
						Available Interrupt.
						[DEFAULT_en_RecDataAvailIntr]*/
	int                 dma_mode_select;     /**< Enable DMA mode in FIFO
						mode. [DEFAULT_dmaModeSelect]*/
	int                 enable_fifo;         /**< Enable FIFO mode.
						[DEFAULT_enableFifo] */
	int                 loop_back_mode;      /**< Enable Loopback mode.
						[DEFAULT_loopBackMode] */
} t_duart_driver_param;


/**************************************************************************//**
@Description   DUART UART unit descriptor structure
 *//***************************************************************************/
typedef struct t_duart_uart {
	fsl_handle_t                h_app;       /**< Handle to the upper
						layer handle    */
	int                     irq;             /**< DUART interrupt ID
						(system-specific number) */
	t_duart_driver_param      *p_driver_param;/**< Pointer to the driver
						initialization parameters */
	t_duart_mem_map           *p_mem_map;    /**< Pointer to the device
						memory map */
	int                     initialized;     /**< flag to know if device
						was initialized yet */
	uint32_t                system_clock_mhz;/**< System bus frequence
						(in MHz) */

	uint8_t                 *p_rx_buffer;    /**< Start Address of the data
						buffer */
	uint32_t                rx_buffer_size;  /**< The size in bytes needed
						for the receiver buffer*/
	uint32_t                rx_buffer_put_pos;/**< current position where
						to put next char*/
	uint32_t                rx_buffer_get_pos;/**< current position where
						to get next char */
	uint32_t                rx_buffer_count;/**< number of chars in read
						buffer */
	int32_t                 rx_timeout;     /**< Number of times the driver
						checks if a specific char was
						received before returning from
						Rx. If '-1' the driver will
						check for chars an infinite
						number of times until all
						bytes are received */
	t_duart_buffer_call_back   *f_low_space_alert;/**< sw overflow alert
						user callback function */
	t_duart_tx_conf           *f_tx_conf;   /**< end of transmit callback*/
	t_duart_exception        *f_exceptions; /**< Callback invoked by driver
						on exception events. If NULL,
						no exceptions are reported */
	uint32_t                low_space_alert;/**< The size in bytes left in
						rxbuffer when an overflow alert
						function is called */
	int                     enable_fifo;    /**< Enable FIFO mode */
	int                     poll_mode;      /**< flag true if running
						in poll mode else intr mode */
	int                     lf2crlf;        /**< In poll mode convert [LF]
						to [CR LF]  */
	e_duart_flow_control      flow_control; /**< Flow control of the
						channel */
	uint8_t                 *p_tx_buffer;   /**< Current transmitted
						buffer */
	uint32_t                tx_buffer_size; /**< The size in bytes needed
						for the buffer */
	uint32_t                tx_buffer_count;/**< Current number of bytes
						in the transmitted buffer*/
	uint32_t                tx_buffer_pos;  /**< Current transmitted
						buffer position */
	uint8_t                 numcc_entries;  /**< Number of entries in
						control char table.*/
	char                    p_control_char[NUM_CONTROL_CHARS+1];
	/**< Control Character table entry. Table contains up to 8 entries.
	Size of table is configured with UCC_UART_ConfigNumControlCharEntries()
	MSBs padded with zeros for characters smaller than 8 bits.
	[DEFAULT_controlChar0], [DEFAULT_controlChar1],
	[DEFAULT_controlChar2], [DEFAULT_controlChar3],
	[DEFAULT_controlChar4], [DEFAULT_controlChar5],
	[DEFAULT_controlChar6], [DEFAULT_controlChar7] */
	int                    p_control_char_reject[NUM_CONTROL_CHARS];
	/**< Whether to reject received character that matches this Control
	Character table entry. Table contains up to 8 entries. Size of table is
	configured with UCC_UART_ConfigNumControlCharEntries().
	1 to reject. Matching char can be read via
	UCC_UART_GetMatchedControlChar. Maskable interrupt is genereted.
	UCC_UART_UCCE_CCR. 0 to accept normally. [DEFAULT_controlCharReject0],
	[DEFAULT_controlCharReject1], [DEFAULT_controlCharReject2],
	[DEFAULT_controlCharReject3], [DEFAULT_controlCharReject4],
	[DEFAULT_controlCharReject5], [DEFAULT_controlCharReject6],
	[DEFAULT_controlCharReject7] */
} t_duart_uart;

typedef struct t_platform {
	/* Copy of platform parameters */
	struct platform_param   param;

	/* Platform-owned module handles */
	fsl_handle_t            h_part;
	fsl_handle_t            clock[INTG_MAX_NUM_OF_CORES];
	fsl_handle_t            pic;

	/* Memory-related variables */
	e_memory_partition_id   prog_runs_from;
	int                     num_of_mem_parts;
	int                     registered_partitions[PLATFORM_MAX_MEM_INFO_ENTRIES];

	/* Interrupt management variables */
	int                     pic_intr_id_base;

	/* Console-related variables */
	fsl_handle_t            uart;
	uint32_t                duart_id;

	uintptr_t               ccsr_base;
	dma_addr_t              mc_mem_phys_base;
} t_platform;

#endif /* __CONSOLE_PRINT_TEST_H */

