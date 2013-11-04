#ifndef __FSL_SYS_TIMER_H
#define __FSL_SYS_TIMER_H

#include "common/types.h"


/**************************************************************************//**
 @Group         sys_grp     System Interfaces

 @Description   Bare-board system programming interfaces.

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group         sys_timer_grp   System Timer Service

 @Description   Bare-board system interface for timer operations.

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Description   Timers operation mode
*//***************************************************************************/
typedef enum e_timer_mode {
    E_TIMER_MODE_INVALID = 0,
    E_TIMER_MODE_FREE_RUN,    /**< Free run - counter continues to increase
                                   after reaching the reference value. */
    E_TIMER_MODE_PERIODIC,    /**< Periodic - counter restarts counting from 0
                                   after reaching the reference value. */
    E_TIMER_MODE_SINGLE       /**< Single (one-shot) - counter stops counting
                                   after reaching the reference value. */
} e_timer_mode;


/**************************************************************************//**
 @Description   Physical timer operations

                These operations must be supported by any physical timer that
                is operated by the TIMER module.
*//***************************************************************************/
typedef struct t_timer_ops {
    fsl_handle_t    tmr;
    uint8_t         id;
    e_timer_mode    mode;

    int       (*f_start) (fsl_handle_t      tmr,
                                uint8_t       id,
                                void (*f_expired_callback) (fsl_handle_t h_app, uint8_t id),
                                fsl_handle_t      h_app,
                                uint64_t      period,
                                e_timer_mode   mode);

    int       (*f_stop) (fsl_handle_t      tmr,
                               uint8_t       id);

    int       (*f_set_time) (fsl_handle_t      tmr,
                                   uint8_t       id,
                                   uint64_t      period);

    int       (*f_get_time) (fsl_handle_t      tmr,
                                   uint8_t       id,
                                   uint64_t      *p_period);
} t_timer_ops;


/**************************************************************************//**
 @Function      sys_register_timer

 @Description   Register a physical timer as the system timer.

 @Param[in]     p_TimerOps      - Pointer to the timer interface structure.
 @Param[in]     allowUserTimers - '1' to allow user-owned timers;
                                  '0' to permit only system-owned timers.

 @Return        E_OK on success; Error code otherwise.
*//***************************************************************************/
int sys_register_timer(t_timer_ops *p_timer_ops, int allow_user_timers);

/**************************************************************************//**
 @Function      sys_unregister_timer

 @Description   Unregister the system timer.

 @Param[in]     h_Timer - The handle to the timer object, as was provided in
                          the \c p_TimerOps parameter of SYS_RegisterTimer().

 @Return        E_OK on success; Error code otherwise.
*//***************************************************************************/
int sys_unregister_timer(fsl_handle_t tmr);

/**************************************************************************//**
 @Function      sys_current_time

 @Description   Get the current time in the system.

                The routine read the global time value.

 @Return        Current time (In milli-seconds).
*//***************************************************************************/
uint32_t sys_current_time(void);

/**************************************************************************//**
 @Function      sys_sleep

 @Description   This routine is used for relatively long delays with interrupts enabled.

 @Param[in]     msecs   - The time to wait (in milli-seconds).

 @Return        Always zero.

 @Cautions      This routine is enabling interrupts.
*//***************************************************************************/
uint32_t sys_sleep(uint32_t msecs);

/**************************************************************************//**
 @Function      sys_udelay

 @Description   This routine is used for relatively short delays with busy waitting.

 @Param[in]     usecs   - The time to wait (in micro-seconds).

 @Return        None.

 @Cautions      This routine performs busy waiting!
*//***************************************************************************/
void sys_udelay(uint32_t usecs);

/** @} */ /* end of sys_timer_grp */
/** @} */ /* end of sys_grp */


#endif /* __FSL_SYS_TIMER_H */
