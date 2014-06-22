/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_core_debug.h

 @Description   Generic interface to basic core operations.

                The system integrator must ensure that this interface is
                mapped to a specific core implementation, by including the
                appropriate header file.
*//***************************************************************************/
#ifndef __FSL_CORE_DEBUG_H
#define __FSL_CORE_DEBUG_H

/**************************************************************************//**
 @Function      core_get_id_debug

 @Description   Returns the core ID in the system.

 @Return        Core ID.
*//***************************************************************************/
uint32_t core_get_id_debug(void);

#endif /* __FSL_CORE_DEBUG_H */
