#include "aiop_verification.h"
#include "kernel/smp.h"

#define __wait()	asm ("wait  \n"	)

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
		aiop_verification_sr();
		aiop_verification_fm();

		return 0;
}

int main()
{
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

	__wait();

	return 0;
}
#pragma pop
