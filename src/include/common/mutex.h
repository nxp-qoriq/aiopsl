/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          mutex.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_MUTEX_H
#define __FSL_MUTEX_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_os_g  FSL OS Interface (System call hooks)

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         fsl_os_mutex_g  FSL OS Mutex API

 @Description   TODO

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Function      mutex_create

 @Description   Creates a mutex.

 @Return        A Mutex handle is returned on success; NULL otherwise.
*//***************************************************************************/
fsl_handle_t mutex_create(void);

/**************************************************************************//**
 @Function      mutex_free

 @Description   Frees the memory allocated for the mutex creation.

 @Param[in]     lock - A handle to a mutex.
*//***************************************************************************/
void mutex_free(fsl_handle_t lock);

/**************************************************************************//**
 @Function      mutex_lock

 @Description   Locks a mutex.

 @Param[in]     lock - A handle to a mutex.
*//***************************************************************************/
void mutex_lock(fsl_handle_t lock);

/**************************************************************************//**
 @Function      mutex_unlock

 @Description   Unlocks a mutex.

 @Param[in]     lock - A handle to a mutex.
*//***************************************************************************/
void mutex_unlock(fsl_handle_t lock);

/**************************************************************************//**
 @Function      mutex_trylock

 @Description   TODO

 @Param[in]     lock - A handle to a mutex.

 @Return        TODO
*//***************************************************************************/
int mutex_trylock(fsl_handle_t lock);

/** @} */ /* end of fsl_os_mutex_g group */
/** @} */ /* end of fsl_os_g group */


#endif /* __FSL_MUTEX_H */
