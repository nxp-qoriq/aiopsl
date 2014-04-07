/**************************************************************************//**
@File		general.c

@Description	This file contains the AIOP SW task default params.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "general.h"

/** Global task params */
__TASK struct aiop_default_task_params default_task_params;
__TASK uint64_t random_64bit;

void update_random_64bit(void)
{
	random_64bit = (random_64bit>>1) ^ (-(random_64bit & 1LL) &
							0x9d206b8e1036b29fLL);
}
