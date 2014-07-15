/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

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

 @Description   print a formated string.

 @Param[in]     str - string to print.

		The format string is a character string, beginning and ending
		in its initial shift state, if any. The format string is
		composed of zero or more directives: ordinary characters (not
		%), which are copied unchanged to the output stream; and
		conversion specifications, each of which results in fetching
		zero or more subsequent arguments. Each conversion
		specification is introduced by the character %.
		The arguments must correspond properly (after type promotion)
		with the conversion specifier.
		The following command characters are supported:
		%d	Prints an integer in decimal form
		%x 	Prints and integer in hexadecimal form
		%s 	Prints a string
		%llx 	Prints a 32 bit value in hexadecimal form
		%lx 	Prints a 64 bit value in hexadecimal form

 @Return       none
*//***************************************************************************/
void fsl_os_print(char *str, ...);

/** @} *//* end of fsl_os_g FSL OS Interface (System call hooks) group */


#endif /* __FSL_STDIO_H */
