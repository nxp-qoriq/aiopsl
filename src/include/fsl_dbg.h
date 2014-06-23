/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          fsl_dbg.h

 @Description   Debug mode definitions.
*//***************************************************************************/

#ifndef __FSL_DBG_H
#define __FSL_DBG_H

#include "inc/dbg.h"

#ifdef pr_debug
#undef pr_debug
#endif
/**************************************************************************//**
 @Function      pr_debug(...)

 @Description   Macro to add level trace, CPU number and other parameters
 	 	 to the print. used to print debug info.
 	 	 Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String an VA ARGS to print.
*//***************************************************************************/
#define pr_debug(...) 	DBG(REPORT_LEVEL_TRACE, __VA_ARGS__)

#ifdef pr_info
#undef pr_info
#endif
/**************************************************************************//**
 @Function      pr_info(...)

 @Description   Macro to add level trace, CPU number and other parameters
 	 	 to the print. used to print debug info.
 	 	 Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String an VA ARGS to print.
*//***************************************************************************/
#define pr_info(...) 	DBG(REPORT_LEVEL_INFO, __VA_ARGS__)

#ifdef pr_warn
#undef pr_warn
#endif
/**************************************************************************//**
 @Function      pr_warn(...)

 @Description   Macro to add level trace, CPU number and other parameters
 	 	 to the print. used to print debug info.
 	 	 Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String an VA ARGS to print.
*//***************************************************************************/
#define pr_warn(...) 	DBG(REPORT_LEVEL_WARNING, __VA_ARGS__)

#ifdef pr_err
#undef pr_err
#endif
/**************************************************************************//**
 @Function      pr_err(...)

 @Description   Macro to add level trace, CPU number and other parameters
 	 	 to the print. used to print debug info.
 	 	 Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String an VA ARGS to print.
*//***************************************************************************/
#define pr_err(...) 	DBG(REPORT_LEVEL_MAJOR, __VA_ARGS__)

#ifdef pr_crit
#undef pr_crit
#endif
/**************************************************************************//**
 @Function      pr_err(...)

 @Description   Macro to add level trace, CPU number and other parameters
 	 	 to the print. used to print debug info.
 	 	 Debug Levels for Errors and Events are attached to the print.

 @Param[in]     String an VA ARGS to print.
*//***************************************************************************/
#define pr_crit(...)	DBG(REPORT_LEVEL_CRITICAL, __VA_ARGS__)

/** @} */ /* end of dump_g group */
/** @} */ /* end of gen_g group */


#endif /* __FSL_DBG_H */
