#include "aiop_verification.h"

#define __wait()	asm ("wait  \n"	)

int main()
{	
	__wait();
/* initialize stack pointer */
	aiop_verification();
	aiop_verification_fm();
	aiop_verification_fm_temp();
	
	return 0;
}
