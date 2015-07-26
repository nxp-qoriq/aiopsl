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

#ifndef __SYS_CONSOLE_H
#define __SYS_CONSOLE_H

#include "fsl_types.h"


/**************************************************************************//**
 @Group         sys_grp     System Interfaces

 @Description   Bare-board system programming interfaces.

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group         sys_console_grp     System Console Service

 @Description   Bare-board system interface for console operations.

 @{
*//***************************************************************************/


/**************************************************************************//**
 @Function      sys_register_console

 @Description   Register a new system console.

 @Param[in]     h_ConsoleDev    - Handle to console device (passed as the first
                                  parameter to \c f_ConsolePrint and \c f_ConsoleGet).
 @Param[in]     f_ConsolePrint  - Console routine for printing characters.
 @Param[in]     f_ConsoleGet    - Console routine for reading characters.

 @Return        0 on success; Error code otherwise.

 @Cautions      Only a single console is supported. The existing console must
                be unregistered before a new one is registered.
*//***************************************************************************/
int sys_register_console(fsl_handle_t    h_console_dev,
                         int (*f_console_print)(fsl_handle_t h_console_dev,
                        	 uint8_t *p_data, uint32_t size),
                         int (*f_console_get)(fsl_handle_t h_console_dev,
                        	 uint8_t *p_data, uint32_t size));

/**************************************************************************//**
 @Function      sys_unregister_console

 @Description   Unregister the existing system console.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int sys_unregister_console(void);

/**************************************************************************//**
 @Function      sys_print

 @Description   Prints a characters string to the console device.

                If no console has been registered yet, the function prints the
                string into an internal buffer. The buffer will be printed to
                the first registered console.

 @Param[in]     str - The characters string to print.

 @Return        None.
*//***************************************************************************/
void sys_print(char *str);

/** @} */ /* end of sys_console_grp */
/** @} */ /* end of sys_grp */
#endif /* __SYS_CONSOLE_H */
