#include "aiop_verification.h"

#define __wait()	asm ("wait  \n"	)

#pragma push
#pragma section code_type ".verif_text"
#pragma force_active on
#pragma function_align 256  
#pragma require_prototypes off

int main()
{	
	__wait();
/* initialize stack pointer */
	aiop_verification();
	aiop_verification_fm();
	aiop_verification_fm_temp();
	
	return 0;
}
#pragma pop
