/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/******************************************************************************

 @File          platform_ext.h

 @Description   Prototypes, externals and typedefs for platform routines
****************************************************************************/
#ifndef __FSL_PLATFORM_H
#define __FSL_PLATFORM_H

#include "common/types.h"
#include "fsl_errors.h"
#include "fsl_soc.h"



/**************************************************************************//**
 @Group         platform_grp PLATFORM Application Programming Interface

 @Description   Generic Platform API, that must be implemented by each
                specific platform.

 @{
*//***************************************************************************/

#define NO_IRQ (~0)              /**< Set no IRQ even though IRQ is not supported in AIOP */
/*TODO: Need to be removed when dynamic support for number of pools used for dpni's will be added*/
#define BPIDS_USED_FOR_POOLS_IN_DPNI         2

#define PLATFORM_IO_MODE_ANY            (~0)    /**< Unspecified I/O mode */

#define MAX_CHIP_NAME_LEN               10   /**< Maximum chip name length (including null character) */


/**************************************************************************//**
 @Description   Cache Operation Mode
*//***************************************************************************/
typedef enum cache_mode {
    E_CACHE_MODE_DISABLED       = 0x00000000,   /**< Cache is disabled */
    E_CACHE_MODE_DATA_ONLY      = 0x00000001,   /**< Cache is enabled for data only */
    E_CACHE_MODE_INST_ONLY      = 0x00000002,   /**< Cache is enabled for instructions only */
    E_CACHE_MODE_DATA_AND_INST  = 0x00000003    /**< Cache is enabled for data and instructions */
} e_cache_mode;

/**************************************************************************//**
 @Description   Mapped memory type for obtaining module's base address
*//***************************************************************************/
typedef enum mapped_mem_type {
    E_MAPPED_MEM_TYPE_GEN_REGS,
    E_MAPPED_MEM_TYPE_MII_MNG_REGS,
    E_MAPPED_MEM_TYPE_MC_PORTAL,
    E_MAPPED_MEM_TYPE_CE_PORTAL,
    E_MAPPED_MEM_TYPE_CI_PORTAL
} e_mapped_mem_type;

/**************************************************************************//**
 @Description   Chip Type and Revision Information Structure
*//***************************************************************************/
typedef struct t_chip_rev_info {
    char        chip_name[MAX_CHIP_NAME_LEN];
                /**< Chip name (e.g. "MPC8360E", "MPC8360") */
    uint16_t    rev_major;
                /**< Major chip revision */
    uint16_t    rev_minor;
                /**< Minor chip revision */
} t_chip_rev_info;

/**************************************************************************//**
 @Description   Descriptor memory-partition
*//***************************************************************************/
typedef struct platform_memory_info {
    int             mem_partition_id;
    dma_addr_t      phys_base_addr;
    uintptr_t       virt_base_addr;
    uint64_t        size;
    uint32_t        mem_attribute; /* malloc-able, cacheable */
    char            name[32];
} t_platform_memory_info;

typedef struct t_platform_ops {
    fsl_handle_t h_platform;

    /* Platform initialization functions */
    int (*f_init_core)           (fsl_handle_t h_platform);
    int (*f_init_timer)          (fsl_handle_t h_platform);
    int (*f_init_intr_ctrl)      (fsl_handle_t h_platform);
    int (*f_init_soc)            (fsl_handle_t h_platform);  /**< For master partition only */
    int (*f_init_mem_partitions) (fsl_handle_t h_platform);
    int (*f_init_console)        (fsl_handle_t h_platform);
    int (*f_init_private)        (fsl_handle_t h_platform);

    /* Platform termination functions */
    int (*f_free_core)           (fsl_handle_t h_platform);
    int (*f_free_timer)          (fsl_handle_t h_platform);
    int (*f_free_intr_ctrl)      (fsl_handle_t h_platform);
    int (*f_free_soc)            (fsl_handle_t h_platform);  /**< For master partition only */
    int (*f_free_mem_partitions) (fsl_handle_t h_platform);
    int (*f_free_console)        (fsl_handle_t h_platform);
    int (*f_free_private)        (fsl_handle_t h_platform);

    /* Enable/disable functions */
    void    (*f_enable_cores)          (fsl_handle_t h_platform, uint64_t core_mask);
    void    (*f_enable_local_irq)      (fsl_handle_t h_platform);
    void    (*f_disable_local_irq)     (fsl_handle_t h_platform);
} t_platform_ops;


#if defined(MC)
#include "platform_mc_spec.h"
#elif defined(AIOP)
#include "platform_aiop_spec.h"
#else
#error "Platform specific header file not defined"
#endif /* specific platform select */


/**************************************************************************//**
 @Function      platform_early_init

 @Description   Initializes the platform object according to the user's
                parameters and the board design and constraints.

 @Param[in]     p_platform_params - Pointer to platform early configuration parameters.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_early_init(struct platform_param *p_platform_params);

/**************************************************************************//**
 @Function      platform_init

 @Description   Creates the platform object and fills the platform operations
                structure, for later use by the system.

 @Param[in]     p_platform_param - Pointer to platform configuration parameters.
 @Param[out]    p_platform_ops   - Pointer to platform operations structure,
                                  that should be filled by this routine.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_init(struct platform_param    *p_platform_param,
                        t_platform_ops             *p_platform_ops);

/**************************************************************************//**
 @Function      platform_free

 @Description   Releases the platform object and all its allocated resources.

 @Param[in]     h_platform - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_free(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_get_chip_rev_info

 @Description   Retrieves the revision information of the device.

 @Param[in]     h_platform      - Platform object handle.
 @Param[out]    p_chip_rev_info   - Returns the revision information structure.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_get_chip_rev_info(fsl_handle_t h_platform, t_chip_rev_info *p_chip_rev_info);

/**************************************************************************//**
 @Function      platform_get_core_clk

 @Description   Gets the core clock frequency of the device.

 @Param[in]     h_platform - Platform object handle.

 @Return        The core clock frequency (Hz).
*//***************************************************************************/
uint32_t platform_get_core_clk(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_get_system_bus_clk

 @Description   Gets the system bus frequency of the device.

 @Param[in]     h_platform - Platform object handle.

 @Return        The system bus frequency (KHz).
*//***************************************************************************/
uint32_t platform_get_system_bus_clk(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_get_ddr_clk

 @Description   Gets the local bus frequency of the device.

 @Param[in]     h_platform - Platform object handle.

 @Return        The DDR clock frequency (Hz).
*//***************************************************************************/
uint32_t platform_get_ddr_clk(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_get_local_bus_clk

 @Description   Gets the local bus frequency of the device.

 @Param[in]     h_platform - Platform object handle.

 @Return        The local bus frequency (Hz).
*//***************************************************************************/
uint32_t platform_get_local_bus_clk(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_get_controller_clk

 @Description   Gets the input clock frequency of a given controller object.

 @Param[in]     h_platform  - Platform object handle.
 @Param[in]     module      - Controller object type (usually a sub-module type)
 @Param[in]     id          - Controller object identifier

 @Return        The input clock frequency of a given controller object (Hz).
*//***************************************************************************/
uint32_t platform_get_controller_clk(fsl_handle_t     h_platform,
                                   enum fsl_os_module  module,
                                   uint32_t     id);

/**************************************************************************//**
 @Function      platform_init_mac_for_mii_access

 @Description   Initialize MAC for MII access only.

 @Param[in]     h_platform  - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_init_mac_for_mii_access(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_free_mac_for_mii_access

 @Description   Free a MAC device that was previously initialized for
                MII access only.

 @Param[in]     h_platform  - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_free_mac_for_mii_access(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_enable_console

 @Description   Enables the platform's console.

 @Param[in]     h_platform - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_enable_console(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_disable_console

 @Description   Disables the platform's console.

 @Param[in]     h_platform - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_disable_console(fsl_handle_t h_platform);

/**************************************************************************//**
 @Function      platform_set_serdes_loopback

 @Description   Sets SERDES loopback for the specified module.

 @Param[in]     h_platform - Platform object handle.
 @Param[in]     module     - Controller object type (usually a sub-module type)
 @Param[in]     id         - Controller object identifier

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_set_serdes_loopback (fsl_handle_t    h_platform,
                                    enum fsl_os_module module,
                                    uint32_t id);

/**************************************************************************//**
 @Function      platform_clear_serdes_loopback

 @Description   Clears SERDES loopback for the specified module.

 @Param[in]     h_platform - Platform object handle.
 @Param[in]     module     - Controller object type (usually a sub-module type)
 @Param[in]     id         - Controller object identifier

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_clear_serdes_loopback(fsl_handle_t    h_platform,
                                     enum fsl_os_module module,
                                     uint32_t    id);

/** @} */ /* end of platform_grp */

/**************************************************************************//**
 @Group         sys_platform_grp    System Platform Interface

 @Description   Bare-board system interface for platform-related operations.

 @{
*//***************************************************************************/
#include "inc/fsl_sys.h"

/**************************************************************************//**
 @Function      sys_get_controller_clk

 @Description   Gets the input clock frequency of a given controller object.

 @Param[in]     module  - Controller object type (usually a sub-module type)
 @Param[in]     id      - Controller object identifier

 @Return        The input clock frequency of a given controller object (Hz).
*//***************************************************************************/
static __inline__ uint32_t sys_get_controller_clk(enum fsl_os_module module, uint32_t id)
{
    return platform_get_controller_clk(sys_get_unique_handle(FSL_OS_MOD_SOC),
                                     module,
                                     id);
}

/**************************************************************************//**
 @Function      sys_get_core_clk

 @Description   Gets the core clock frequency of the device.

 @Return        The core clock frequency (Hz).
*//***************************************************************************/
static __inline__ uint32_t sys_get_core_clk(void)
{
    return platform_get_core_clk(sys_get_unique_handle(FSL_OS_MOD_SOC));
}

/**************************************************************************//**
 @Function      sys_get_system_bus_clk

 @Description   Gets the system bus frequency of the device.

 @Return        The system bus frequency (Hz).
*//***************************************************************************/
static __inline__ uint32_t sys_get_system_bus_clk(void)
{
    return platform_get_system_bus_clk(sys_get_unique_handle(FSL_OS_MOD_SOC));
}

#if 0
/**************************************************************************//**
 @Function      sys_get_chip_rev_info

 @Description   Retrieves the revision information of the device.

 @Param[out]    p_chip_rev_info - Returns the revision information structure.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
static __inline__ fsl_err_t sys_get_chip_rev_info(t_chip_rev_info *p_chip_rev_info)
{
    return platform_get_chip_rev_info(sys_get_unique_handle(FSL_OS_MOD_SOC),
                                   p_chip_rev_info);
}
#endif /* 0 */

/** @} */ /* end of sys_platform_grp */


#endif /* __FSL_PLATFORM_H */
