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
/**
@File          leds.h

@Description   header file to turn on leds from AIOP.
               atu_fpga_window_for_leds.patch should be installed on MC
*//***************************************************************************/
#ifndef __LED_H
#define __LED_H

#include "fsl_types.h"

#define FPGA_VIRT_ADDRESS     0x8010000
#define LED_CTL_OFFSET        0x04 /*led ctl is in offset 05*/
#define LED_SWITCH_OFFSET     0x0C /*led switch is in offset 0x0E*/


#define TURN_ON_LEDS_CTRL                                             \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_CTL_OFFSET);              \
		*ptr ^= 0x00020000;                                   \
	}
#define SWITCH_LED_1                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00000100;                                   \
	}
#define SWITCH_LED_2                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00000200;                                   \
	}
#define SWITCH_LED_3                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00000400;                                   \
	}
#define SWITCH_LED_4                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00000800;                                   \
	}
#define SWITCH_LED_5                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00001000;                                   \
	}
#define SWITCH_LED_6                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00002000;                                   \
	}
#define SWITCH_LED_7                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00004000;                                   \
	}
#define SWITCH_LED_8                                                  \
	{                                                             \
		uint32_t *ptr = (uint32_t *)(FPGA_VIRT_ADDRESS +      \
					LED_SWITCH_OFFSET);           \
		*ptr ^= 0x00008000;                                   \
	}
/**
 * @Function turn_led_configuration;
 */
static inline void turn_led_configuration(void)
{
	TURN_ON_LEDS_CTRL;
}
/**
 * @Function turn_led_on;
 * @param[in] led_num bitmask to choose which leds to turn on (8 bits = 8 leds)
 */
static inline void turn_led_on(uint8_t led_num)
{
	if(led_num | 0x01)
		SWITCH_LED_1;
	if(led_num | 0x02)
		SWITCH_LED_2;
	if(led_num | 0x04)
		SWITCH_LED_3;
	if(led_num | 0x08)
		SWITCH_LED_4;
	if(led_num | 0x10)
		SWITCH_LED_5;
	if(led_num | 0x20)
		SWITCH_LED_6;
	if(led_num | 0x40)
		SWITCH_LED_7;
	if(led_num | 0x80)
		SWITCH_LED_8;
}

#endif /* __LED_H */

