/**************************************************************************//**
@File		general.c

@Description	This file contains the AIOP SW task default params.

		Copyright 2013 Freescale Semiconductor, Inc.
*//***************************************************************************/
#include "general.h"
#include "dplib/fsl_fdma.h"

/** Global task params */
__TASK struct aiop_default_task_params default_task_params;

/* TODO - once the ARENA implementation is ready move this (verification)
 * implementation (the chosen implementation between the following 2) to
 * aiop_verification_data.c. keep the declaration in place*/
void handle_fatal_error(char *message)
{
       uint32_t status;
       status = -1 + (uint32_t)message;
       fdma_terminate_task();
}

void exception_handler(char *filename, uint32_t line, char *message)
{
       uint32_t status;
       status = -1 + (uint32_t)message + (uint32_t)filename + line;
       fdma_terminate_task();
}
