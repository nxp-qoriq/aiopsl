#include "aiop_verification.h"

#define __wait()	asm ("wait  \n"	)

int main()
{	
	__wait();
	aiop_verification();
	aiop_verification_fm();
	
	return 0;
}
