/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_stdio.h

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
 @Group         fsl_os_stdio_g  FSL OS STDIO API

 @Description   TODO

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      fsl_os_print

 @Description   print a string.

 @Param[in]     str - string to print.

 @Return        None.
*//***************************************************************************/
void fsl_os_print(char *str, ...);

/** @} */ /* end of fsl_os_stdio_g group */
/** @} */ /* end of fsl_os_g group */


#endif /* __FSL_STDIO_H */
