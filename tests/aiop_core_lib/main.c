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

#include "aiop_verification.h"
#include "fsl_smp.h"

//#define __wait()	asm ("wait  \n"	)

#pragma push
#pragma section code_type ".verif_text"
#pragma force_active on
#pragma function_align 256
#pragma require_prototypes off

extern __VERIF_GLOBAL uint8_t verif_prpid_valid;
extern __VERIF_GLOBAL uint8_t verif_only_1_task_complete;
extern __VERIF_GLOBAL uint8_t verif_spin_lock;

int verif_main()
{
	/* initialize stack pointer */

	/* TODO Duplication is because verification using old SR EPID - this
	 * should be removed */
	aiop_verification_fm();
	aiop_verification_fm();
	return 0;
}

int main()
{
	uint32_t ctscsr_value;
	/* Todo -
	 * 1. enable next line
	 * 2. enable project files under ppc + kernel */
	/*if (sys_is_master_core())*/
		/*aiop_verif_init_parser();*/
	/*uint32_t ctscsr_value = 0;
	SET_CTSCSR0(ctscsr_value);
         The init parser code
	aiop_verif_init_parser();
         CTSEN = 1, set task number to 18, Core Task Scheduler Enable
        ctscsr_value = CTSCSR_16_TASKS | CTSCSR_ENABLE;
        SET_CTSCSR0(ctscsr_value);*/
	verif_prpid_valid = 0;
	verif_only_1_task_complete = 0;
	verif_spin_lock = 0;
	ctscsr_value = CTSCSR_8_TASKS | CTSCSR_ENABLE;
	SET_CTSCSR0(ctscsr_value);
	__e_hwacceli(YIELD_ACCEL_ID); /* Yield */

	return 0;
}
#pragma pop
