/**************************************************************************//**
@File          fsl_stdlib.h

@Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_STDLIB_H
#define __FSL_STDLIB_H

#include <stdlib.h>


/**************************************************************************//**
@Group		fsl_os_g FSL OS Interface (System call hooks)

@Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

@{
*//***************************************************************************/

/**************************************************************************//**
@Function      fsl_os_rand

@Description   returns random number

@Return        pseudo random number (uint32)
*//***************************************************************************/
uint32_t fsl_os_rand(void);

/** @} *//* end of fsl_os_g FSL OS Interface (System call hooks) group */

#endif /* __FSL_STDLIB_H */
