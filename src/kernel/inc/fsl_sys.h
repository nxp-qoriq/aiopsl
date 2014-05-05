/*!
 *  @file    sys.h
 *  @brief   general system configuratoins and declarations
 *
 *  @details 
 *
 *  @internal
 *  @requirements TODO: Put the CQ PRD record   
 *  @implements   TODO: Add link to the design chapter/description
 */

#ifndef __FSL_SYS_H
#define __FSL_SYS_H

#include "common/types.h"
#include "common/errors.h"


/**************************************************************************//**
 @Description   Platform Configuration Parameters Structure.

                This structure must be specifically defined by each platform.
*//***************************************************************************/


/**************************************************************************//**
 @Collection    Object Identifier Macros
 @{
*//***************************************************************************/
#define SYS_NULL_OBJECT_ID          0xffffffff
                                    /**< Object ID representing no object */
#define SYS_MAX_NUM_OF_MODULE_IDS   3

#define CAST_ID_TO_HANDLE(_id)      (UINT_TO_PTR(_id))
                                    /**< Macro for casting an object ID to a handle */
#define CAST_HANDLE_TO_ID(_h)       (PTR_TO_UINT(_h))
                                    /**< Macro for casting a handle to an object ID */
/* @} */


struct platform_param;

/*!
 * @ingroup aiopapp_init 
 * Struct defining the AIOP Applicatoins initialization and de-initialliation 
 * functions. 
 * Each AIOP Applicaton requiring initializatoin shall provide a
 * sys_module_desc entry.
 */
struct sys_module_desc {
    int     (*init) (void);
    void    (*free) (void);
};

/**************************************************************************//**
 @Description   System initialization parameters structure
*//***************************************************************************/
typedef struct t_sys_param {
    uint8_t                 partition_id;           /**< Partition ID - set to SYS_MASTER_PART_ID
                                                         on master partition */
    uint64_t                partition_cores_mask;   /**< Cores belonging to this partition,
                                                         used by SMP configuration for releasing
                                                         partition's cores from spin table and
                                                         for execution synchronization via SYS_Barrier() */
    uint64_t                master_cores_mask;      /**< Master cores on all partitions,
                                                         used by AMP configuration for releasing
                                                         guest partition masters from spin table */
    int                     use_cli;                /**< Flag specifying whether CLI can be used;
                                                         actual interactive vs non-interactive mode
                                                         is determined during system start-up */
    int                     use_ipc;                /**< Flag specifying whether IPC (Inter-Partition
                                                         Communication) service can be used; when set
                                                         to '0' IPC service will not be initialized
                                                         by the system */
    struct platform_param   *platform_param;      /**< Platform specific parameters */
} t_sys_param;


/**************************************************************************//**
 @Function      sys_init

 @Description   System initialization routine.

                This routine initializes the internal system structures and
                services, such as memory management, objects repository and
                more, as well as the platform object.

 @Return        Zero for success; Non-zero value on error.
*//***************************************************************************/
int sys_init(void);

/**************************************************************************//**
 @Function      sys_free

 @Description   System termination routine.

                This routine releases all internal structures that were
                initialized by the sys_init() routine.

 @Return        None.
*//***************************************************************************/
void sys_free(void);

/**************************************************************************//**
 @Function      sys_add_handle

 @Description   Forces a handle for a specific object in the system.

                This routine allows forcing an object handle into the system
                and thus bypassing the normal initialization flow.

                The forced handle must be removed as soon as it is not valid
                anymore, using the sys_remove_handle() routine.

 @Param[in]     h_module    - The object handle;
 @Param[in]     module      - The object (module/sub-module) type.
 @Param[in]     num_of_ids    - Number of IDs that are passed in the variadic-argument;
 @Param[in]     ...         - Unique object IDs sequence;

 @Return        E_OK on success; Error code otherwise.

 @Cautions      This routine must not be used in normal flow - it serves only
                rare and special cases in platform initialization.
*//***************************************************************************/
int sys_add_handle(fsl_handle_t h_module, enum fsl_os_module module, int num_of_ids, ... );

/**************************************************************************//**
 @Function      sys_remove_handle

 @Description   Removes a previously forced handle of a specific object.

                This routine must be called to remove object handles that
                were previously forced using the SYS_ForceHandle() routine.

 @Param[in]     module      - The object (module/sub-module) type.
 @Param[in]     num_of_ids  - Number of IDs that are passed in
							the variadic-argument.
 @Param[in]     ...         - Unique object IDs sequence;

 @Return        E_OK on success; Error code otherwise.

 @Cautions      This routine must not be used in normal flow - it serves only
                rare and special cases in platform initialization.
*//***************************************************************************/
int sys_remove_handle(enum fsl_os_module module, int num_of_ids, ... );

/**************************************************************************//**
 @Function      sys_get_handle

 @Description   Returns a specific object handle.

                This routine may be used to get the handle of any module or
                sub-module in the system.

                For singleton objects, it is recommended to use the
                sys_get_unique_handle() routine.

 @Param[in]     module  - Module/sub-module type.
 @Param[in]     num_of_ids  - Number of IDs that are passed in
								the variadic-argument.
 @Param[in]     ...     - Unique object IDs sequence. For sub-modules,
                this is the unique object ID; For modules,
                this value must always be zero.

 @Return        The handle of the specified object if exists;
                NULL if the object is not known or is not initialized.
*//***************************************************************************/
fsl_handle_t sys_get_handle(enum fsl_os_module module, int num_of_ids, ...);

/**************************************************************************//**
 @Function      sys_get_unique_handle

 @Description   Returns a specific object handle (for singleton objects).

                This routine may be used to get the handle of any singleton
                module or sub-module in the system.

                This routine simply calls the sys_get_handle() routine with
                the \c id parameter set to zero.

 @Param[in]     module - Module/sub-module type.

 @Return        The handle of the specified object if exists;
                NULL if the object is not known or is not initialized.
*//***************************************************************************/
static __inline__ fsl_handle_t sys_get_unique_handle(enum fsl_os_module module)
{
    return sys_get_handle(module, 1, 0);
}


#endif /* __FSL_SYS_H */
