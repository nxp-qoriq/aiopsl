#include "aiop_verification.h"
#include "kernel/smp.h"

#define __wait()	asm ("wait  \n"	)

#pragma push
#pragma section code_type ".verif_text"
#pragma force_active on
#pragma function_align 256  
#pragma require_prototypes off

int main()
{	
	/* Todo - 
	 * 1. enable next line
	 * 2. enable project files under ppc + kernel */
	/*if (sys_is_master_core())*/
		/*aiop_verif_init_parser();*/
	__wait();
/* initialize stack pointer */
	aiop_verification();
	aiop_verification_fm();
	aiop_verification_fm_temp();
	
	return 0;
}
#pragma pop
