/**************************************************************************//**
 @file          fsl_dbg.h

 @Description   Debug mode definitions
*//***************************************************************************/

#ifndef __FSL_DBG_H
#define __FSL_DBG_H

#include "inc/dbg.h"
#include "inc/fsl_core.h"
#include "fsl_smp.h"

/**************************************************************************//**
 @Group			FSL_DEBUG_GROUP debug functions

 @Description	FSL AIOP debug macros

 @{
*//***************************************************************************/


/**************************************************************************//**
 @Function      pr_debug(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print debug info.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String as VA ARGS to print.
*//***************************************************************************/
#define pr_debug(...) 	DBG(REPORT_LEVEL_TRACE, __VA_ARGS__)

/**************************************************************************//**
 @Function      pr_info(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print info messages.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String as VA ARGS to print.
*//***************************************************************************/
#define pr_info(...) 	DBG(REPORT_LEVEL_INFO, __VA_ARGS__)

/**************************************************************************//**
 @Function      pr_warn(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print warnings info.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String as VA ARGS to print.
*//***************************************************************************/
#define pr_warn(...) 	DBG(REPORT_LEVEL_WARNING, __VA_ARGS__)

/**************************************************************************//**
 @Function      pr_err(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print errors info.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String as VA ARGS to print.
*//***************************************************************************/
#define pr_err(...) 	DBG(REPORT_LEVEL_MAJOR, __VA_ARGS__)

/**************************************************************************//**
 @Function      pr_crit(...)

 @Description   Macro to add level trace, CPU number and other parameters
                to the print. used to print critical debug info.
                Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String as VA ARGS to print.
*//***************************************************************************/
#define pr_crit(...)	DBG(REPORT_LEVEL_CRITICAL, __VA_ARGS__)

/**************************************************************************//**
 @Function      dbg_get_core_id

 @Description   Returns the core ID in the system.

 @Return        Core ID.
*//***************************************************************************/
#define dbg_get_core_id()    core_get_id()

/**************************************************************************//**
 @Function      dbg_get_num_of_cores

 @Description   Returns the number of active cores in the system.

 @Return        number of active cores.
*//***************************************************************************/
#define dbg_get_num_of_cores()    sys_get_num_of_cores()

/**************************************************************************//**
 @Function      dbg_get_max_num_of_cores

 @Description   Returns the number of existing cores in the system.

 @Return        number of existing cores.
*//***************************************************************************/
#define dbg_get_max_num_of_cores()    sys_get_max_num_of_cores()

/** @} */ /* end of debug functions */

#endif /* __FSL_DBG_H */
