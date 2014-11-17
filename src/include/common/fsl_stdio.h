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
 @File		fsl_stdio.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_STDIO_H
#define __FSL_STDIO_H

#include <stdio.h>


/**************************************************************************//**
@Group         fsl_os_g  FSL OS Interface (System call hooks)

@Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

@{
*//***************************************************************************/

/**************************************************************************//**
@Function      fsl_os_print

@Description   Print formated string with arguments received with it.
               The print function is limited to 1024 bytes of the formatted string length
               during the boot and 80 bytes during run time mode.
               This function has two modes of operation.  It automatically toggles
               between the two modes depending on whether the function is called
               during initialization or at run time.

               Initialization stage - During AIOP initialization the full formatting
               feature set is used, similar to printf from the C standard library.

               Run time stage - in this stage the AIOP uses a lite print and
               supports only a subset of the formatting features as follows:

               - %%c      - Prints a char.
               - %%d      - Prints an integer in decimal form
               - %%x      - Prints an integer in hexadecimal form
               - %%s      - Prints a string
               - %%l      - Prints unsigned 32 bit value.
               - %%ll     - Prints unsigned 64 bit value.
               - %%lx     - Prints a 32 bit value in hexadecimal form
               - %%llx    - Prints a 64 bit value in hexadecimal form
            
@note No support for special settings like %02x, etc at runtime.

@Param[in]     str - string to print.

@Return       none
*//***************************************************************************/

void fsl_os_print(char *str, ...);

/** @} *//* end of fsl_os_g FSL OS Interface (System call hooks) group */


#endif /* __FSL_STDIO_H */
