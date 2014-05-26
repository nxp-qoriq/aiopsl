#if 0

/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          timer.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_TIMER_H
#define __FSL_TIMER_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_os_g  FSL OS Interface (System call hooks)

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         fsl_os_timer_g  FSL OS Timer API

 @Description   TODO

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      fsl_os_create_timer

 @Description   Creates a timer.

 @Return        Timer handle is returned on success; NULL otherwise.
*//***************************************************************************/
fsl_handle_t fsl_os_create_timer(void);

/**************************************************************************//**
 @Function      fsl_os_free_timer

 @Description   Frees the memory allocated for the timer creation.

 @Param[in]     tmr - A handle to a timer.
*//***************************************************************************/
void fsl_os_free_timer(fsl_handle_t tmr);

/**************************************************************************//**
 @Function      fsl_os_start_timer

 @Description   Starts a timer.

                The user can select to start the timer as periodic timer or as
                one-shot timer. The user should provide a callback routine that
                will be called when the timer expires.

 @Param[in]     tmr         - A handle to a timer.
 @Param[in]     msecs       - Timer expiration period (in milliseconds).
 @Param[in]     periodic    - '1' for a periodic timer;
                              '0' for a one-shot timer..
 @Param[in]     expired_cb  - A callback routine to be called when the
                              timer expires.
 @Param[in]     arg         - The argument to pass in the timer-expired
                              callback routine.

 @Return        TODO
*//***************************************************************************/
int fsl_os_start_timer(fsl_handle_t   tmr,
                       uint32_t       msecs,
                       int            periodic,
                       void           (*expired_cb)(fsl_handle_t),
                       fsl_handle_t   arg);

/**************************************************************************//**
 @Function      fsl_os_stop_timer

 @Description   Frees the memory allocated for the timer creation.

 @Param[in]     tmr - A handle to a timer.
*//***************************************************************************/
void fsl_os_stop_timer(fsl_handle_t tmr);

/**************************************************************************//**
 @Function      fsl_os_mod_timer

 @Description   Updates the expiration time of a timer.

                This routine adds the given time to the current system time,
                and sets this value as the new expiration time of the timer.

 @Param[in]     tmr     - A handle to a timer.
 @Param[in]     msecs   - The new interval until timer expiration
                          (in milliseconds).

 @Return        None.
*//***************************************************************************/
void fsl_os_mod_timer(fsl_handle_t tmr, uint32_t msecs);

/**************************************************************************//**
 @Function      fsl_os_current_time

 @Description   Returns current system time.

 @Return        Current system time (in milliseconds).
*//***************************************************************************/
uint32_t fsl_os_current_time(void);

/**************************************************************************//**
 @Function      fsl_os_sleep

 @Description   Non-busy wait until the desired time (in milliseconds) has passed.

 @Param[in]     msecs - The requested sleep time (in milliseconds).

 @Return        Zero if the requested time has elapsed; Otherwise, the value
                returned will be the unslept amount) in milliseconds.

 @Cautions      This routine enables interrupts during its wait time.
*//***************************************************************************/
uint32_t fsl_os_sleep(uint32_t msecs);

/**************************************************************************//**
 @Function      fsl_os_udelay

 @Description   Busy-wait until the desired time (in microseconds) has passed.

 @Param[in]     usecs - The requested delay time (in microseconds).

 @Return        None.

 @Cautions      It is highly unrecommended to call this routine during interrupt
                time, because the system time may not be updated properly during
                the delay loop. The behavior of this routine during interrupt
                time is unexpected.
*//***************************************************************************/
void fsl_os_udelay(uint32_t usecs);

/** @} */ /* end of fsl_os_timer_g group */
/** @} */ /* end of fsl_os_g group */


#endif /* __FSL_TIMER_H */

#endif
