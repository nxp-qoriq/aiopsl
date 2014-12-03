/*
 * Copyright 2014 Freescale Semiconductor, Inc.
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

/**

 @File          platform_aiop_spec.h

 @Description   LS2085A Pltform external definitions and structures.
*//***************************************************************************/
#ifndef __FSL_PLATFORM_AIOP_SPEC_H
#define __FSL_PLATFORM_AIOP_SPEC_H


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
    uint32_t                        clock_in_freq_hz;
    enum cache_mode                 l1_cache_mode;
    enum platform_console_type      console_type;
    uint8_t                         console_id;

    struct platform_memory_info     mem_info[PLATFORM_MAX_MEM_INFO_ENTRIES];
};


/**************************************************************************//**
 @Description   Platform application parameters structure
*//***************************************************************************/
struct platform_app_params {
	uint16_t dpni_num_buffs;	/**< number of buffers for dpni pool*/
	uint16_t dpni_buff_size;	/**< size of buffers for dpni pool*/
	uint16_t dpni_drv_alignment;	/**< dpni pool buffers alignment*/
	uint16_t app_arr_size;		/**< Maximal size of app init array */
};

/** @} */ /* end of ls2085a_g group */



#endif /* __FSL_PLATFORM_AIOP_SPEC_H */

