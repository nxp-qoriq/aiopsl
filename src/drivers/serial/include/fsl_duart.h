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

/**************************************************************************//**
 @File          fsl_duart.h

 @Description   DUART UART external structures and definitions.
*//***************************************************************************/

#ifndef __FSL_DUART_H
#define __FSL_DUART_H

#include "common/types.h"

/**************************************************************************//**
 @Group         duart_g DUART Application Programming Interface

 @Description   DUART functions, definitions and enums.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         duart_init_g DUART Initialization Unit

 @Description   DUART initialization functions.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   User callback function, called if driver Rx buffer is full.
                The user provides this function and the driver invokes it.

 @Param[in]     h_App - Application handle given by the user at driver configuration.
 *//***************************************************************************/
typedef void (t_duart_buffer_call_back)(fsl_handle_t h_app);

/**************************************************************************//**
 @Description   User callback function, called after all bytes are transferred.
                The user provides this function and the driver invokes it.
                If the function exists, it is called after all the bytes in
                UART Tx buffer are transferred.

 @Param[in]     h_App       - Application handle given by the user at driver
                              configuration.
 @Param[in]     p_Buffer    - Pointer to the UART internal Tx buffer.
*//***************************************************************************/
typedef void (t_duart_tx_conf)(fsl_handle_t h_app, uint8_t *p_buffer);

/**************************************************************************//**
 @Description   User callback function, called by driver for event handling.
                The user provides this function and the driver invokes it.

 @Param[in]     h_App   - Application handle given by the user at driver
                          configuration.
 @Param[in]     errors  - Events that caused this routine to be
                          invoked by the driver.
 @Param[in]     flags   - Protocol dependant indicator (unused).

 *//***************************************************************************/
typedef void (t_duart_exception)(fsl_handle_t h_app, uint32_t errors, uint32_t flags);

/**************************************************************************//**
 @Collection   Baud rate bounds values
 *//***************************************************************************/
#define BAUD_RATE_MIN_VAL       600
#define BAUD_RATE_MAX_VAL       512000
/* @} */

/**************************************************************************//**
 @Description   Data bits
 *//***************************************************************************/
typedef enum e_duart_uart_data_bits {
    E_DUART_DATA_BITS_5 = 0x00, /**< 5 bit word */
    E_DUART_DATA_BITS_6 = 0x01, /**< 6 bit word */
    E_DUART_DATA_BITS_7 = 0x02, /**< 7 bit word */
    E_DUART_DATA_BITS_8 = 0x03  /**< 8 bit word */
} e_duart_uart_data_bits;

/**************************************************************************//**
 @Description   Stop bits
 *//***************************************************************************/
typedef enum e_duart_uart_stop_bits {
    E_DUART_STOP_BITS_1 = 0x00, /**< 1 stop bit      */
    E_DUART_STOP_BITS_2 = 0x04  /**< 1.5/2 stop bits */
} e_duart_uart_stop_bits;

/**************************************************************************//**
 @Description    Parity
 *//***************************************************************************/
typedef enum e_duart_uart_parity {
    E_DUART_PARITY_ODD,     /**< Odd parity   */
    E_DUART_PARITY_EVEN,    /**< Even parity  */
    E_DUART_PARITY_SPACE,   /**< Space parity */
    E_DUART_PARITY_MARK,    /**< Mark parity  */
    E_DUART_PARITY_NONE     /**< No parity    */
} e_duart_uart_parity;

/**************************************************************************//**
 @Description    Flow Control
 *//***************************************************************************/
typedef enum e_duart_flow_control {
    E_DUART_HW_FLOW_CONTROL,    /**< Hardware flow control mode */
    E_DUART_NO_FLOW_CONTROL     /**< No flow control mode       */
} e_duart_flow_control;

/**************************************************************************//**
 @Description    Size of UART fifo in DUART
 *//***************************************************************************/
typedef enum e_duart_uart_fifo_size {
    E_DUART_FIFO_01 = 0x00,     /**< Rx FIFO interrupt trigger level 1 */
    E_DUART_FIFO_04 = 0x40,     /**< Rx FIFO interrupt trigger level 4 */
    E_DUART_FIFO_08 = 0x80,     /**< Rx FIFO interrupt trigger level 8 */
    E_DUART_FIFO_14 = 0xc0      /**< Rx FIFO interrupt trigger level 14 */
} e_duart_uart_fifo_size;

/**************************************************************************//**
@Description   Duart error type.

               The error type is sent as a parameter in the f_Exceptions
               callback function.
*//***************************************************************************/
typedef enum e_duart_uarfsl_err_t_type {
    E_DUART_RECEIVER_FIFO_ERROR = 0x80, /**< One the characters in the received
                                             FIFO encountered an error */
    E_DUART_FRAMING_ERROR       = 0x08, /**< Invalid Stop Bit for received data*/
    E_DUART_PARITY_ERROR        = 0x04, /**< Unexpected parity value received*/
    E_DUART_OVERRUN_ERROR       = 0x02  /**< Received character was overrun
                                             before it was read*/
} e_duart_uarfsl_err_t_type;

/**************************************************************************//**
 @Description   DUART protocol initialization structure.

                Structure is used when calling DUART_UART_Config
 *//***************************************************************************/
typedef struct t_duart_uart_param {
    /* System integration parameters */
    uintptr_t               base_address;        /**< Base address of DUART registers in memory. */

    /* UART basic parameters */
    uint32_t                baud_rate;           /**< Baud-rate to use - same for Tx and Rx  */
    e_duart_uart_parity       parity;             /**< Parity - same for Tx and Rx */
    e_duart_uart_data_bits     data_bits;           /**< Data bits per char */
    e_duart_uart_stop_bits     stop_bits;           /**< Stop bits per char */
    e_duart_flow_control      flow_control;        /**< Flow control of the channel */

    /* System integration parameters - cont. */
    int                     irq;                /**< DUART interrupt ID (system-specific number) */
    uint32_t                system_clock_mhz;     /**< System clock frequency */

    /* Application interface */
    fsl_handle_t                h_app;              /**< Handle to the upper layer handle */
    t_duart_buffer_call_back   *f_low_space_alert;   /**< Overflow alert user callback function */
    t_duart_tx_conf           *f_tx_conf;          /**< Transmit finished user callback function */
    t_duart_exception        *f_exceptions;      /**< Callback invoked by driver on exception events;
                                                     If NULL, no exceptions are reported */
} t_duart_uart_param;

#ifdef MC
/**************************************************************************//**
 @Description   DUART protocol initialization structure.

                Structure is used when calling sys_get_desc
                stores the address used for specific uart
 *//***************************************************************************/
struct uart_desc {
    int disable;
    int uart_id;	/**< Defines the uart number [0 - 3] */
    phys_addr_t paddr;	/**< Physical address of uart memory map */
    void *vaddr;	/**< Virtual address of uart memory map */
};
#endif
/**************************************************************************//**
 @Function      duart_config

 @Description   Creates DUART channel descriptor according to user parameters.

                User parameters are the minimum necessary to specify duart channel
                behavior. Additional parameters can be specified later using
                DUART advanced configuration functions.

 @Param[in]     p_duart_uart_param - Pointer to structure of user-specified parameters.

 @Return        DUART channel descriptor. NULL if failed.
*//***************************************************************************/
fsl_handle_t duart_config(t_duart_uart_param *p_duart_uart_param);

__START_COLD_CODE
/**************************************************************************//**
 @Function      duart_init

 @Description   Fully initializes DUART channel and enables it.

 @Param[in]     duart - DUART channel descriptor returned by duart_config().

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_init(fsl_handle_t duart);

/**************************************************************************//**
 @Function      duart_free

 @Description   Frees all resources used by DUART.

 @Param[in]     duart - DUART descriptor returned by duart_config().

 @Return        None.

 @Cautions      Does not disable DUART.
*//***************************************************************************/
void duart_free(fsl_handle_t duart);


/**************************************************************************//**
 @Group         duart_advanced_init_grp DUART Advanced Configuration Unit

 @Description   Configuration functions used to change default values.

                These functions can be called only after duart_config() and
                before DUART_Init().
 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      duart_config_rx_buffer_size

 @Description   Configures the Rx Buffer Size.

                Default value is [DEFAULT_rxBufferSize].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - New value for rx BufferSize (in bytes).

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_rx_buffer_size(fsl_handle_t duart, uint32_t new_val);

/**************************************************************************//**
 @Function      duart_config_rx_timeout

 @Description   Configures the Rx Timeout.

                If timeout is reached, the driver will return from the Rx routine.
                Default value is [DEFAULT_rxTimeout].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - New value for Rx Timeout. The value sets how many
                          times the driver will check for new chars before all
                          bytes are received.
                          - If 0, the driver won't wait for chars to be received.
                          - If -1, the driver will wait infinitely until all chars
                            are received.
                          - For any other value, the number of times to check for
                            each new char equals the new value.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_rx_timeout(fsl_handle_t duart, int32_t new_val);

/**************************************************************************//**
 @Function      duart_config_en_modem_stat_intr

 @Description   Enables or disables the Modem Status Interrupt.

                Default value is [DEFAULT_enModemStatIntr].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, enables the interrupt.
                          If 0, disables the interrupt.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_en_modem_stat_intr(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_en_rec_line_stat_intr

 @Description   Enable or disables the Receiver Line Status Interrupt.

                Default value is [DEFAULT_enRecLineStatIntr].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, enables the interrupt.
                          If 0, disables the interrupt.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_en_rec_line_stat_intr(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_en_trans_hold_reg_empty_intr

 @Description   Enables or disables the Transmitter Holding Register Empty Interrupt.

                Default value is [DEFAULT_enTransHoldRegEmptyIntr].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, enables the interrupt.
                          If 0, disables the interrupt.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_en_trans_hold_reg_empty_intr(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_en_rec_data_avail_intr

 @Description   Enables or disables the Receiver Data Available Interrupt.

                Default value is [DEFAULT_enRecDataAvailIntr].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, enables the interrupt.
                          If 0, disables the interrupt.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_en_rec_data_avail_intr(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_rec_trigger_level

 @Description   Configures the number of chars in FIFO before triggering
                an interrupt.

                Default value is [DEFAULT_recTriggerLevel].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - Number of chars before triggering the interrupt.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_rec_trigger_level(fsl_handle_t duart, e_duart_uart_fifo_size new_val);

/**************************************************************************//**
 @Function      duart_config_dma_mode_select

 @Description   Enables or disables DMA mode in FIFO mode.

                In DMA mode the transmitter is ready only when the FIFO is full
                and the receiver is ready only when the trigger level has been
                reached.
                Default value is [DEFAULT_dmaModeSelect].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, enables DMA mode.
                          If 0, disables DMA mode.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_dma_mode_select(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_enable_fifo

 @Description   Enables or disables the FIFO.

                Default value is [DEFAULT_enableFifo].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, enables FIFO mode.
                          If 0, disables FIFO mode.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_enable_fifo(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_loop_back_mode

 @Description   Enables or disables loopback mode.

                In loopback mode all chars sent from each UART are transferred
                back to the same UART.

                Default value is [DEFAULT_loopBackMode].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, enables loopback mode.
                          If 0, disables loopback mode.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_loop_back_mode(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_poll_mode

 @Description   Enables or disables poll mode. If poll mode is disabled then
                the driver works on interrupt mode.

                Default value is [DEFAULT_pollMode].

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, system operates in poll mode.
                          If 0, system operates in interrupt mode.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_poll_mode(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_poll_lf2crlf

 @Description   Enables or disables Lf2CrLf mode.

                Default value is [DEFAULT_pollLf2Crlf].

                When enabled in Poll mode, converts LF to CR LF.

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val  - If 1, system converts LF to CR LF.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_poll_lf2crlf(fsl_handle_t duart, int new_val);

/**************************************************************************//**
 @Function      duart_config_control_char

 @Description   Configures the control characters.

                When a control char is received Rx stops receiving bytes. Every
                control char can be received with the data before it or rejected
                from the buffer.
                Default values are [DEFAULT_controlChar], [DEFAULT_controlCharReject],
                [DEFAULT_numCCEntries].

 @Param[in]     duart              - DUART channel descriptor returned by duart_config().
 @Param[in]     *control_char        - The 8 control characters.
 @Param[in]     *control_char_reject  - 8 Control Char Reject values. If a value
                                      is zero then the control char in the same
                                      index will not be rejected from the buffer.
                                      If the value is non-zero, the control char
                                      will be rejected.
 @Param[in]     numcc_entries        - Number of control chars. Must be <= 8.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_config_control_char(fsl_handle_t duart, char *control_char, int *control_char_reject, uint8_t numcc_entries);

/** @} */ /* end of duart_advanced_init_grp group */
/** @} */ /* end of duart_init_g group */


/**************************************************************************//**
 @Group         duart_runtime DUART Runtime Unit

 @Description   DUART Run Time functions.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      duart_tx

 @Description   Sends a buffer through the specified DUART channel.

                This functions is called when the user wishes to transmit data.

 @Param[in]     duart  - DUART channel descriptor returned by duart_config().
 @Param[in]     p_data  - Pointer to a transmit buffer.
 @Param[in]     size    - Number of characters to send.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_tx(fsl_handle_t duart, uint8_t *p_data, uint32_t size);

/**************************************************************************//**
 @Function      duart_rx

 @Description   Collects the received buffer.

                This function is called by the user to collect data which has
                been received by the UART.

                The function will return when one of the following occurs:
                - The number of characters set by the user has been received.
                - RxTimeout has been reached.
                - A control character has been received.

 @Param[in]     duart      - DUART channel descriptor returned by duart_config().
 @Param[in]     p_buffer    - Points to a buffer which will store the received data.
 @Param[in]     size        - Number of characters to receive.

 @Return        Size of received buffer.
*//***************************************************************************/
uint32_t duart_rx(fsl_handle_t duart, uint8_t *p_buffer, uint32_t size);

/** @} */ /* end of duart Runtime group */


/**************************************************************************//**
 @Group         duart_control DUART Control Unit

 @Description   DUART Control functions.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      DUART_GetNumOfRxCharInBuffer

 @Description   Returns the number of chars in the received buffer.

                This function can be useful to know what value to set
                in the 'size' parameter of the DUART_Rx() routine.

 @Param[in]     duart - DUART channel descriptor returned by duart_config().

 @Return        Number of chars in the received buffer.
*//***************************************************************************/
uint32_t duart_get_num_of_rx_char_in_buffer(fsl_handle_t duart);

/**************************************************************************//**
 @Function      duart_set_low_space_alert

 @Description   Sets the triggel level for the overflow alert callback function.
                Default value is [DEFAULT_lowSpaceAlert].

 @Param[in]     duart - DUART channel descriptor returned by duart_config().
 @Param[in]     new_val - The number of empty bytes in the Rx Buffer which will
                         cause an overflow. Once the Rx buffer has this number
                         of empty bytes or less the overflow alert callback
                         function is called.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_set_low_space_alert(fsl_handle_t duart, uint32_t new_val);

/**************************************************************************//**
 @Function      duart_set_break_signal

 @Description   Sets the break signal.

 @Param[in]     duart - DUART channel descriptor returned by duart_config().
 @Param[in]     break_signal - indication if to set or clear the break signal option.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_set_break_signal (fsl_handle_t duart, int break_signal);

/**************************************************************************//**
 @Function      duart_set_baud_rate

 @Description   Sets a new value of the baud rate divisor.

 @Param[in]     duart   - DUART channel descriptor returned by duart_config().
 @Param[in]     baud_rate - New baud rate value

 @Return        0 on success; Error code otherwise.

 @Cautions      To ensure proper operation, the baud rate can only be modified
                when no data transfer or data reception are in progress.
*//***************************************************************************/
int duart_set_baud_rate (fsl_handle_t duart, uint32_t baud_rate);

#if (DEBUG_ERRORS > 0)
/**************************************************************************//**
 @Function      duart_dump_regs

 @Description   Dumps the memory of DUART module

 @Param[in]     duart - DUART channel descriptor returned by duart_config().

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int duart_dump_regs(fsl_handle_t duart);
#endif /* (DEBUG_ERRORS > 0) */
__END_COLD_CODE
/** @} */ /* end of Control group */
/** @} */ /* end of duart_g Programming Interface group */


#endif /* __FSL_DUART_H */
