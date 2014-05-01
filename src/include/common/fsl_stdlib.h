/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_stdlib.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_STDLIB_H
#define __FSL_STDLIB_H

#include <stdlib.h>


/**************************************************************************//**
 @Group         fsl_os_g  FSL OS Interface (System call hooks)

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         fsl_os_stdlib_g  FSL OS STDLIB API

 @Description   TODO

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      fsl_os_exit

 @Description   Stop execution and report status (where it is applicable)

 @Param[in]     status - exit status
*//***************************************************************************/
void    fsl_os_exit(int status);

/** @} */ /* end of fsl_os_stdlib_g group */
/** @} */ /* end of fsl_os_g group */

/**************************************************************************//**
 @Function      fsl_os_rand

 @Description   returns random number

 @Return        pseudo random number (uint32)
*//***************************************************************************/
uint32_t fsl_os_rand(void);

#endif /* __FSL_STDLIB_H */
