/**
@File          leds.h

@Description   header file to turn on leds from AIOP.
               atu_fpga_window_for_leds.patch should be installed on MC
*//***************************************************************************/
#ifndef __LED_H
#define __LED_H

#include "common/types.h"

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
#endif /* __LED_H */

