/*---------------------------------------------------------------------
 *
 * Miscellaneous definitions for BOOKE assembler
  ----------------------------------------------------------------------*/
#ifndef __BOOKE_H
#define __BOOKE_H


/* Assembly syntax - different for 64 and 32-bit */
#ifdef ARENA_64BIT_ARCH
#define asm_prefix(_name)    .##_name
#else
#define asm_prefix(_name)    _name
#endif

/*******************
* interrupt vector
********************/

/*
 * Branch conditions
 * -----------------
 */
#define ALWAYS          0x14 /* Branch always                             */

/*
 * Miscellaneous
 * -------------
 */
#define NUM_OF_GREG     11  /* 11 is the Number of general purpose registers to be saved by the interrupt routine
                               NOTE: there are 32 general purpose registers, however, the routine saves only 11 */
#define NUM_OF_SREG     6  /* Number of special registers to save */


#endif /* __BOOKE_H */
