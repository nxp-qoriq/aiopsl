
#ifndef KW_EXCLUDE



// The original booke.S does not pass compilation on CW 10.0.x
// This file is temporal to make the MC project compile. 
// The original src/arch/ppc/booke.S  should be adjusted to CW for DPAA 10.0.x 
// This function is copied from asm unsigned int Get_ProcessorID(void) 
// that is located in interupt.c . Should be verified !!!
asm unsigned int  get_cpu_id(void)
{
	mfspr r3, 286
    blr
}



#endif /* not KW_EXCLUDE */

