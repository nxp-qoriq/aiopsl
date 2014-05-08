/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          spinlock.h

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines
*//***************************************************************************/

#ifndef __FSL_SYS_LAYOUT_H
#define __FSL_SYS_LAYOUT_H

#include "common/types.h"


/**************************************************************************//**
 @Group         fsl_os_g  FSL OS Interface (System call hooks)

 @Description   Prototypes, externals and typedefs for system-supplied
                (external) routines

 @{
*//***************************************************************************/

/**************************************************************************//**
 @Group         fsl_os_lo_g  FSL OS Layout API

 @Description   TODO

 @{
*//***************************************************************************/

#define SYS_LO_MAX_COMPATIBLE_NAME  20
#define SYS_LO_MAX_COMPATIBLES      4


struct node;


/**************************************************************************//**
 @Description   Layout Modules Registration Parameters.

                This structure provides registration parameters for a set of
                sub-modules. Each module should register its own sub-modules
                in the system using the sys_lo_register_module() routine.
*//***************************************************************************/
typedef struct sys_lo_mod_params {
    int             num_compats;
    char            **compatibles;
        /**< The type of the owner module */

    int             (*f_prob_module)(struct node *node);
        /**< modules initialization routine */
    int             (*f_remove_module)(struct node *node);
        /**< modules free routine */
} t_sys_lo_mod_params;

/**************************************************************************//**
 @Function      sys_lo_register_module

 @Description   TODO

 @Param[in]     lo_mod_params   TODO

 @Return        E_OK on success; error code otherwise..
*//***************************************************************************/
int sys_lo_register_module (struct sys_lo_mod_params *lo_mod_params);

struct node *sys_build_dpl (unsigned int *dpl_bin); /* TODO - review */

/** @} */ /* end of fsl_os_lo_g group */
/** @} */ /* end of fsl_os_g group */


#endif /* __FSL_SYS_LAYOUT_H */
