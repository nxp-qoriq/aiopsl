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
               The print function is limited to 80 bytes of the formatted string length.
               This function has two modes of operation.  It automatically toggles
               between the two modes depending on whether the function is called
               during initialization or at run time.

               Initialization stage - During AIOP initialization the full formatting
               feature set is used, similar to printf from the C standard library.

               Run time stage - in this stage the AIOP uses a lite print and
               supports only a subset of the formatting features as follows:

               %c     Prints a char.
               %d     Prints an integer in decimal form
               %x     Prints an integer in hexadecimal form
               %s     Prints a string
               %l     Prints unsigned 32 bit value.
               %ll    Prints unsigned 64 bit value.
               %lx    Prints a 32 bit value in hexadecimal form
               %llx   Prints a 64 bit value in hexadecimal form
               *No support for special settings like %02x, etc at runtime.

@Param[in]     str - string to print.

@Return       none
*//***************************************************************************/

void fsl_os_print(char *str, ...);

/** @} *//* end of fsl_os_g FSL OS Interface (System call hooks) group */


#endif /* __FSL_STDIO_H */
