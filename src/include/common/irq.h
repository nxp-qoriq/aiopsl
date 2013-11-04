/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          irq.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_IRQ_H
#define __FSL_IRQ_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_os_g  FSL OS Interface (System call hooks)

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         fsl_os_irq_g  FSL OS IRQ API

 @Description   TODO

 @{
*//***************************************************************************/

#define NO_IRQ (~0)


/**************************************************************************//**
 @Description   General prototype of interrupt service routine (ISR).

 @Param[in]     handle - Optional handle of the module handling the interrupt.

 @Return        None
 *//***************************************************************************/
typedef void (isr_t)(fsl_handle_t handle);


/**************************************************************************//**
 @Function      fsl_os_set_irq

 @Description   Set an interrupt service routine for a specific interrupt source.

 @Param[in]     irq     - Interrupt ID (system-specific number).
 @Param[in]     isr_cb  - Callback routine that will be called when the interrupt occurs.
 @Param[in]     arg     - The argument for the user callback routine.

 @Return        E_OK on success; error code otherwise..
*//***************************************************************************/
int fsl_os_set_irq(int irq, isr_t *isr_cb, fsl_handle_t arg);

/**************************************************************************//**
 @Function      fsl_os_free_irq

 @Description   Free a specific interrupt and a specific callback routine.

 @Param[in]     irq - Interrupt ID (system-specific number).

 @Return        E_OK on success; error code otherwise..
*//***************************************************************************/
int fsl_os_free_irq(int irq);

/**************************************************************************//**
 @Function      fsl_os_enable_irq

 @Description   Enable a specific interrupt.

 @Param[in]     irq - Interrupt ID (system-specific number).

 @Return        E_OK on success; error code otherwise..
*//***************************************************************************/
int fsl_os_enable_irq(int irq);

/**************************************************************************//**
 @Function      fsl_os_disable_irq

 @Description   Disable a specific interrupt.

 @Param[in]     irq - Interrupt ID (system-specific number).

 @Return        E_OK on success; error code otherwise..
*//***************************************************************************/
int fsl_os_disable_irq(int irq);

/** @} */ /* end of fsl_os_irq_g group */
/** @} */ /* end of fsl_os_g group */


#endif /* __FSL_IRQ_H */
