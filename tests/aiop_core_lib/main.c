#include "aiop_verification.h"
#include "dplib/fsl_dpni.h"
#include "drv.h"

#define __wait()	asm ("wait  \n"	)

int main()
{	
	__wait();
/* initialize stack pointer */
	init_stack_ptr();
	aiop_verification();
	aiop_verification_fm();
	
	return 0;
}
