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

 @File          platform.h

 @Description   Prototypes, externals and typedefs for platform routines
****************************************************************************/
#ifndef __FSL_PLATFORM_H
#define __FSL_PLATFORM_H

#include "fsl_types.h"
#include "fsl_errors.h"
#include "fsl_soc.h"

#define PLATFORM_SINGLE_CORE 1
#define PLATFORM_MULTI_CORE 0

#ifdef ARENA_LEGACY_CODE
#define PLTFORM_NUM_OF_INIT_MODULES 4
#else
#define PLTFORM_NUM_OF_INIT_MODULES 3
#endif

/**************************************************************************//**
 @Group         platform_grp PLATFORM Application Programming Interface

 @Description   Generic Platform API, that must be implemented by each
                specific platform.

 @{
*//***************************************************************************/

#define NO_IRQ (~0)              /**< Set no IRQ even though IRQ is not supported in AIOP */

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
 @Description   Platform initialization module description
*//***************************************************************************/
struct pltform_module_desc {
	int (*init) (void * h_platform);
	int (*free) (void * h_platform);
	int is_single_core;
};

/**************************************************************************//**
 @Description   Descriptor memory-partition
*//***************************************************************************/
typedef struct platform_memory_info {
    int             mem_partition_id;
    uint64_t      phys_base_addr;
    uintptr_t       virt_base_addr;
    uint64_t        size;
    uint32_t        mem_attribute; /* malloc-able, cacheable */
    char            name[32];
} t_platform_memory_info;

typedef struct t_platform_ops {
    void * h_platform;
    struct pltform_module_desc modules[PLTFORM_NUM_OF_INIT_MODULES];
} t_platform_ops;


/**************************************************************************//**
 @Group         ls2085a_g LS2085A Application Programming Interface

 @Description   LS2085A Chip functions,definitions and enums.

 @{
*//***************************************************************************/

#define PLATFORM_MAX_MEM_INFO_ENTRIES   8

/**************************************************************************//**
 @Description   Platform Console Types
*//***************************************************************************/
typedef enum platform_console_type {
    PLTFRM_CONSOLE_NONE,        /**< Do not use platform console */
    PLTFRM_CONSOLE_DUART        /**< Use DUART-x as console port */
} e_platform_console_type;



/**************************************************************************//**
 @Description   Platform configuration parameters structure
*//***************************************************************************/
struct platform_param {
    struct platform_memory_info     mem_info[PLATFORM_MAX_MEM_INFO_ENTRIES];
    uint32_t                        num_of_mem_parts;
    enum cache_mode                 l1_cache_mode;
    enum platform_console_type      console_type;
    uint8_t                         console_id;
};


/**************************************************************************//**
 @Description   Platform application parameters structure
*//***************************************************************************/
struct platform_app_params {
	uint16_t dpni_num_buffs;	/**< number of buffers for dpni pool*/
	uint16_t dpni_buff_size;	/**< size of buffers for dpni pool*/
	uint16_t dpni_drv_alignment;	/**< dpni pool buffers alignment*/
	uint16_t app_arr_size;		/**< Maximal size of app init array */
	uint8_t  backup_pool_disable;   /**< If set, disables backup buffer pool usage */
};

/** @} */ /* end of ls2085a_g group */



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
int platform_free(void * h_platform);

/**************************************************************************//**
 @Function      platform_get_chip_rev_info

 @Description   Retrieves the revision information of the device.

 @Param[in]     h_platform      - Platform object handle.
 @Param[out]    p_chip_rev_info   - Returns the revision information structure.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_get_chip_rev_info(void * h_platform, t_chip_rev_info *p_chip_rev_info);

/**************************************************************************//**
 @Function      platform_get_clk

 @Description   Gets the platform clock of the device.

 @Param[in]     h_platform - Platform object handle.

 @Return        The clock in KHz.
*//***************************************************************************/
uint32_t platform_get_clk(void * h_platform);

/**************************************************************************//**
 @Function      platform_init_mac_for_mii_access

 @Description   Initialize MAC for MII access only.

 @Param[in]     h_platform  - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_init_mac_for_mii_access(void * h_platform);

/**************************************************************************//**
 @Function      platform_free_mac_for_mii_access

 @Description   Free a MAC device that was previously initialized for
                MII access only.

 @Param[in]     h_platform  - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_free_mac_for_mii_access(void * h_platform);

/**************************************************************************//**
 @Function      platform_enable_console

 @Description   Enables the platform's console.

 @Param[in]     h_platform - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_enable_console(void * h_platform);

/**************************************************************************//**
 @Function      platform_disable_console

 @Description   Disables the platform's console.

 @Param[in]     h_platform - Platform object handle.

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_disable_console(void * h_platform);

/**************************************************************************//**
 @Function      platform_set_serdes_loopback

 @Description   Sets SERDES loopback for the specified module.

 @Param[in]     h_platform - Platform object handle.
 @Param[in]     module     - Controller object type (usually a sub-module type)
 @Param[in]     id         - Controller object identifier

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_set_serdes_loopback (void *    h_platform,
                                    enum fsl_module module,
                                    uint32_t id);

/**************************************************************************//**
 @Function      platform_clear_serdes_loopback

 @Description   Clears SERDES loopback for the specified module.

 @Param[in]     h_platform - Platform object handle.
 @Param[in]     module     - Controller object type (usually a sub-module type)
 @Param[in]     id         - Controller object identifier

 @Return        0 on success; Error code otherwise.
*//***************************************************************************/
int platform_clear_serdes_loopback(void *    h_platform,
                                     enum fsl_module module,
                                     uint32_t    id);

/** @} */ /* end of platform_grp */

/**************************************************************************//**
 @Group         sys_platform_grp    System Platform Interface

 @Description   Bare-board system interface for platform-related operations.

 @{
*//***************************************************************************/
#include "fsl_sys.h"

/**************************************************************************//**
 @Function      sys_get_platform_clk

 @Description   Gets the platform clock

 @Return        The platform clock frequency (KHz)
*//***************************************************************************/
static inline uint32_t sys_get_platform_clk()
{
    return platform_get_clk(sys_get_unique_handle(FSL_MOD_SOC));
}

/**************************************************************************//**
 @Function      sys_get_sys_clk

 @Description   Gets the system bus frequency of the device

                It is the AIOP clock which has been feed by the platform clock

 @Return        The system bus frequency (KHz).
*//***************************************************************************/
static __inline__ uint32_t sys_get_sys_clk(void)
{
	/*
	 * AIOP gets clock of (platform clk / 1)
	*/
    return platform_get_clk(sys_get_unique_handle(FSL_MOD_SOC));
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
    return platform_get_chip_rev_info(sys_get_unique_handle(FSL_MOD_SOC),
                                   p_chip_rev_info);
}
#endif /* 0 */

/** @} */ /* end of sys_platform_grp */


#endif /* __FSL_PLATFORM_H */
