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
/*!
 *  @file    fsl_log.h
 *  @brief   Log API
 *
 */
#ifndef __FSL_LOG_H
#define __FSL_LOG_H



#define LOG_INIT_SIGN				"START\nEND\n"
#define LOG_END_SIGN_LENGTH                     4


/*******************************************************************
 External Routines
 *******************************************************************/

/**************************************************************************//**
 @Function      log_init

 @Description   initialize logger to print to buffer

 @Cautions      Only master CORE may call this function. (must be called once).
*//***************************************************************************/
int log_init(void);

/**************************************************************************//**
 @Function      log_print_to_buffer

 @Description  	function to print the log to buffer..

 @Param[in]     str - pointer to generated string which will be printed. 

 @Param[in]     str_length - length of the string given by the pointer.

 @Cautions      This function use fdma accelerator and cann't be called under
                spinlock.
*//***************************************************************************/
void log_print_to_buffer(char *str, uint16_t str_length);

#endif /* __FSL_LOG_H */
