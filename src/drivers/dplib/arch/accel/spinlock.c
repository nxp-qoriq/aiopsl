/**************************************************************************//**
@File		spinlock.c

@Description	This file contains the AIOP Spinlock Operations API
		implementation.
*//***************************************************************************/

#include "fsl_spinlock.h"


void lock_spinlock(register uint8_t *spinlock)
{
	register uint8_t temp1;
	register uint8_t temp2;
	asm{
/* Address is in r3, new value (non zero) in r4 and old in r5 */
	li	temp1, 1		/* prepare non-zero value */
spinlock_loop:
	lbarx	temp2, 0, spinlock	/* load and reserve */
	cmpwi	temp2, 0		/* check loaded value */
	bne-	spinlock_loop	/* not equal to 0 - already set */
	stbcx.	temp1, 0, spinlock	/* try to store non-zero */
	bne-	spinlock_loop		/* lost reservation */
	}
}


/* void lock_spinlock(uint8_t *spinlock) {
	uint8_t temp1;
	uint8_t temp2 = 1;

spinlock_loop:
	do {
		__lbarx(temp1 , 0, spinlock)
	} while(temp1 == 1);

	__stbcx(temp2, 0, spinlock)
	 asm
	{
		bne-	spinlock_loop
	}
} */


void unlock_spinlock(
		uint8_t *spinlock) {
	*spinlock = 0;
}


/***************************************************************************
 *            Atomic Increment and Decrement macros
 *            Using the relevant Core instructions: 
 *            lbarx, lharx, lwarx, 
 *            stbcx., sthcx., stwcx. 
***************************************************************************/
/***************************************************************************
 * aiop_atomic_incr8
***************************************************************************/
void aiop_atomic_incr8(register int8_t *var, register int8_t value) 
{
	
	register int8_t orig_value;
	register int8_t new_value;
	asm{
	atomic_loop:
		/* load and reserve. "var" is the address of the byte */	
		lharx orig_value, 0, var 
		/* increment word. "value" is the value to add */
		add new_value, value, orig_value 
		sthcx. new_value, 0, var /* store new value if still reserved */
		bne- atomic_loop /* loop if lost reservation */
	}	
}

/***************************************************************************
 * aiop_atomic_incr16
***************************************************************************/
void aiop_atomic_incr16(register int16_t *var, register int16_t value) 
{
	
	register int16_t orig_value;
	register int16_t new_value;
	asm{
	atomic_loop:
		/* load and reserve. "var" is the address of the half-word */	
		lharx orig_value, 0, var 
		/* increment word. "value" is the value to add */
		add new_value, value, orig_value 
		sthcx. new_value, 0, var /* store new value if still reserved */
		bne- atomic_loop /* loop if lost reservation */
	}	
}

/***************************************************************************
 * aiop_atomic_incr32
 ***************************************************************************/
void aiop_atomic_incr32(register int32_t *var, register int32_t value) 
{
	
	/* Fetch and Add
	* The “Fetch and Add” primitive atomically increments a word in storage.
	* In this example it is assumed that the address of the word 
	* to be incremented is in GPR 3, the increment is in GPR 4, 
	* and the old value is returned in GPR 5.
	* 
	* Original Example from doc:
	* loop:
	* lwarx r5,0,r3 #load and reserve
	* add r0,r4,r5#increment word
	* stwcx. r0,0,r3 #store new value if still res’ved
	* bne- loop #loop if lost reservation
	*/
	
	register int32_t orig_value;
	register int32_t new_value;
	asm{
	atomic_loop:
		/* load and reserve. "var" is the address of the word */	
		lwarx orig_value, 0, var 
		/* increment word. "value" is the value to add */
		add new_value, value, orig_value 
		stwcx. new_value, 0, var /* store new value if still reserved */
		bne- atomic_loop /* loop if lost reservation */
	}	
}

/***************************************************************************
 * aiop_atomic_decr8
 ***************************************************************************/
void aiop_atomic_decr8(register int8_t *var, register int8_t value)
{
	
	register int8_t orig_value;
	register int8_t new_value;
	
	asm{
	atomic_loop:
		/* load and reserve. "var" is the address of the byte */
		lharx orig_value, 0, var
		/* subtract word. "value" is the value to decrement */
		sub new_value, value, orig_value 
		sthcx. new_value, 0, var /* store new value if still reserved */
		bne- atomic_loop /* loop if lost reservation */
	}	
}

/***************************************************************************
 * aiop_atomic_decr16
 ***************************************************************************/
void aiop_atomic_decr16(register int16_t *var, register int16_t value)
{
	
	register int16_t orig_value;
	register int16_t new_value;
	
	asm{
	atomic_loop:
		/* load and reserve. "var" is the address of the half-word */
		lharx orig_value, 0, var
		/* subtract word. "value" is the value to decrement */
		sub new_value, value, orig_value 
		sthcx. new_value, 0, var /* store new value if still reserved */
		bne- atomic_loop /* loop if lost reservation */
	}	
}

/***************************************************************************
 * aiop_atomic_decr32
 ***************************************************************************/
void aiop_atomic_decr32(register int32_t *var, register int32_t value)
{
	
	register int32_t orig_value;
	register int32_t new_value;
	
	asm{
	atomic_loop:
		/* load and reserve. "var" is the address of the word */
		lwarx orig_value, 0, var
		/* subtract word. "value" is the value to decrement */
		sub new_value, value, orig_value 
		stwcx. new_value, 0, var /* store new value if still reserved */
		bne- atomic_loop /* loop if lost reservation */
	}	
}

		
		
