#include "common/fsl_malloc.h"
#include "common/fsl_stdarg.h"
#include "kernel/fsl_spinlock.h"
#include "common/time.h"
#include "common/io.h"
#include "kernel/smp.h"
#include "inc/console.h"
#include "inc/mem_mng.h"

__TASK uint32_t seed_32bit;

#define __ERR_MODULE__ MODULE_UNKNOWN

#define MAX_UDELAY   50000000

#define SIGN	2
#define LARGE	4

#define BUF_SIZE    128

static int vsnprintf_light(char *buf, size_t size, const char *fmt, va_list args);
static char *number(char *str, uint64_t num, int base, int size, int type, size_t *max_size);
static int num_digits(uint32_t num);
/*****************************************************************************/
void fsl_os_exit(int status)
{
    exit(status);
}

/*****************************************************************************/
void fsl_os_print(char *format, ...)
{
#ifndef EMULATOR_FINAL
    va_list args;
    char    tmp_buf[BUF_SIZE];
    int err;

    va_start(args, format);
#ifdef SYS_64BIT_ARCH
#if defined(__GNUC__)
extern void msr_enable_fp(void);
msr_enable_fp();
#endif /* defined(__GNUC__) */
#endif /* SYS_64BIT_ARCH */
	err = vsnprintf_light (tmp_buf, BUF_SIZE, format, args);
	va_end(args);

	if(err < 0)
		sys_print("error while printing\n");
	else
		sys_print(tmp_buf);
#endif /* EMULATOR */
}
#pragma optimization_level 0
static int vsnprintf_light(char *buf, size_t size, const char *fmt, va_list args)
{
	uint64_t num;
	int  base;
	char *str;
	int flags = 0;
	int field_width;
	size--;
	for(str = buf; *fmt && size; ++fmt) {
		if(*fmt != '%') {
			*str++ = *fmt;
			size--;
			continue;
		}

		++fmt;
		field_width = 0;
		base = 10;
		switch(*fmt) {
		case 'c':
			field_width = 1;
			if(size)
			{
				*str++ = (unsigned char)va_arg(args,int); size--;
			}
			continue;


		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
		case 'd':
		case 'l':
			if(*fmt == 'l'){
				switch(*(fmt + 1)){

				case 'l':/*support %ll*/
					++fmt;
					num = va_arg(args,unsigned long long);
					if ((-num >= 10000000000) ||
						(num >= 10000000000)) /*print all long long digits*/
						field_width = 18;
					else
						field_width = num_digits((uint32_t)num);
					break;
				case 'u':/*support %lu*/
					++fmt;
					num = va_arg(args,unsigned long);
					field_width = num_digits((uint32_t)num);
					break;
				default: /*support l*/
					num = va_arg(args,unsigned long);
					field_width = num_digits((uint32_t)num);
					break;
				}



				switch(*(fmt+1)){
				case 'X':
					flags |= LARGE;
				case 'x':
					base = 16;
					++fmt;
					break;
				default:
					break;
				}


			}
			else{	/*d, x, X*/
				if (*(fmt) == 'd'){
					flags |= SIGN;
					num = va_arg(args,long);
				}else{
					num = va_arg(args,unsigned long);
				}
				field_width = num_digits((uint32_t)num);
			}

			break; /*start convert number to string*/

		default: /*special inputs are not supported*/
			return -EINVAL;
		}

		/*start convert number to string*/

		str = number(str,num,base,field_width,flags,&size);
		flags = 0;
	}

	*str = '\0';
	return str - buf;
}

static char *number(char *str, uint64_t num, int base, int size, int type, size_t *max_size)
{
	char sign,tmp[20] = {'\0'};
	const char *digits="0123456789abcdef";
	int i;
	size_t msize;

	msize = *max_size;

	if(type & LARGE)
		digits = "0123456789ABCDEF";

	sign = 0;
	if(type & SIGN) {
		if(num < 0) {
			sign = '-';
			num = -num;
			size--;
		}
	}

	if(base == 16)
		size -= 2;

	i = 0;
	if(num == 0)
		tmp[i++] = '0';
	else while(num != 0) {
		tmp[i++] = digits[(num) % (unsigned) base];
		num /= base;
	}


	if(sign && msize)
	{ *str++ = sign; msize--; }


	while(i-- > 0 && msize)
	{ *str++ = tmp[i]; msize--; }

	while(size-- > 0 && msize)
	{ *str++ = ' '; msize--; }

	*max_size = msize;
	return str;
}



static int num_digits(uint32_t num)
{
    if (num < 0) return num_digits(-num) + 1;

    if (num >= 10000) {
        if (num >= 10000000) {
            if (num >= 100000000) {
                if (num >= 1000000000)
                    return 10;
                return 9;
            }
            return 8;
        }
        if (num >= 100000) {
            if (num >= 1000000)
                return 7;
            return 6;
        }
        return 5;
    }
    if (num >= 100) {
        if (num >= 1000)
            return 4;
        return 3;
    }
    if (num >= 10)
        return 2;
    return 1;
}

#pragma optimization_level reset
/*****************************************************************************/
__HOT_CODE  uint32_t fsl_os_rand(void)
{
	seed_32bit = (seed_32bit>>1) ^ (-(seed_32bit & 1LL) &
			0xFBE16801);

	return seed_32bit;
}

/*****************************************************************************/
__HOT_CODE int fsl_os_gettimeofday(timeval *tv, timezone *tz)
{
	volatile uint32_t TSCRU1, TSCRU2, TSCRL;
	UNUSED(tz);

	TSCRU1 = ioread32(UINT_TO_PTR(SOC_PERIPH_OFF_AIOP_TILE +
	                              TSCRU_OFF));
	TSCRL = ioread32(UINT_TO_PTR(SOC_PERIPH_OFF_AIOP_TILE +
	                              TSCRL_OFF));
	if ((TSCRU2=ioread32(UINT_TO_PTR(SOC_PERIPH_OFF_AIOP_TILE +
		                              TSCRU_OFF))) > TSCRU1 )
		TSCRL = 0;
	else if(TSCRU2 < TSCRU1) /*something wrong while reading*/
		return -1;


	tv->tv_sec = TSCRU2;
	tv->tv_sec = (tv->tv_sec) << 32 | (TSCRL);

	return 0;
}

/*****************************************************************************/
/*                        Spinlock Service Routines                          */
/*****************************************************************************/
void lock_spinlock(register uint8_t *spinlock)
{
	register uint8_t temp1;
	register uint8_t temp2;
	asm{
/* Address is in r3, new value (non zero) in r4 and old in r5 */
	li	temp1, 1		/* prepare non-zero value */
spinlock_loop :
	lbarx	temp2, 0, spinlock	/* load and reserve */
	cmpwi	temp2, 0		/* check loaded value */
	bne -	spinlock_loop	/* not equal to 0 - already set */
	stbcx.	temp1, 0, spinlock	/* try to store non-zero */
	bne -	spinlock_loop		/* lost reservation */
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
 * atomic_incr8
***************************************************************************/
void atomic_incr8(register int8_t *var, register int8_t value)
{

	register int8_t orig_value;
	register int8_t new_value;
	asm{
atomic_loop:
		/* load and reserve. "var" is the address of the byte */
		lbarx orig_value, 0, var
		/* increment word. "value" is the value to add */
		add new_value, value, orig_value
		stbcx. new_value, 0, var /* store new value if still reserved */
		bne - atomic_loop /* loop if lost reservation */
	}
}

/***************************************************************************
 * atomic_incr16
***************************************************************************/
void atomic_incr16(register int16_t *var, register int16_t value)
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
		bne - atomic_loop /* loop if lost reservation */
	}
}

/***************************************************************************
 * atomic_incr32
 ***************************************************************************/
void atomic_incr32(register int32_t *var, register int32_t value)
{

	/* Fetch and Add
	* The "Fetch and Add" primitive atomically increments a word in storage.
	* In this example it is assumed that the address of the word
	* to be incremented is in GPR 3, the increment is in GPR 4,
	* and the old value is returned in GPR 5.
	*
	* Original Example from doc:
	* loop:
	* lwarx r5,0,r3 #load and reserve
	* add r0,r4,r5#increment word
	* stwcx. r0,0,r3 #store new value if still reserved
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
		bne - atomic_loop /* loop if lost reservation */
	}
}

/***************************************************************************
 * atomic_decr8
 ***************************************************************************/
void atomic_decr8(register int8_t *var, register int8_t value)
{

	register int8_t orig_value;
	register int8_t new_value;

	asm{
atomic_loop:
		/* load and reserve. "var" is the address of the byte */
		lbarx orig_value, 0, var
		/* subtract word. "value" is the value to decrement */
		sub new_value, orig_value, value
		stbcx. new_value, 0, var /* store new value if still reserved */
		bne - atomic_loop /* loop if lost reservation */
	}
}

/***************************************************************************
 * atomic_decr16
 ***************************************************************************/
void atomic_decr16(register int16_t *var, register int16_t value)
{

	register int16_t orig_value;
	register int16_t new_value;

	asm{
atomic_loop:
		/* load and reserve. "var" is the address of the half-word */
		lharx orig_value, 0, var
		/* subtract word. "value" is the value to decrement */
		sub new_value, orig_value, value
		sthcx. new_value, 0, var /* store new value if still reserved */
		bne - atomic_loop /* loop if lost reservation */
	}
}

/***************************************************************************
 * atomic_decr32
 ***************************************************************************/
void atomic_decr32(register int32_t *var, register int32_t value)
{

	register int32_t orig_value;
	register int32_t new_value;

	asm{
atomic_loop:
		/* load and reserve. "var" is the address of the word */
		lwarx orig_value, 0, var
		/* subtract word. "value" is the value to decrement */
		sub new_value, orig_value, value
		stwcx. new_value, 0, var /* store new value if still reserved */
		bne - atomic_loop /* loop if lost reservation */
	}
}

#ifdef DEBUG_FSL_OS_MALLOC
/*****************************************************************************/
void * fsl_os_malloc_debug(size_t size, char *fname, int line)
{
    return sys_mem_alloc(SYS_DEFAULT_HEAP_PARTITION, size, 0, "", fname, line);
}

/*****************************************************************************/
void *fsl_os_xmalloc_debug(size_t     size,
                           int          partition_id,
                           uint32_t     alignment,
                           char         *fname,
                           int          line)
{
    return sys_mem_alloc(partition_id, size, alignment, "", fname, line);
}

#else /* not DEBUG_FSL_OS_MALLOC */
/*****************************************************************************/
void * fsl_os_malloc(size_t size)
{
    return sys_mem_alloc(SYS_DEFAULT_HEAP_PARTITION, size, 0, "", "", 0);
}

/*****************************************************************************/
void *fsl_os_xmalloc(size_t size, int partition_id, uint32_t alignment)
{
    return sys_mem_alloc(partition_id, size, alignment, "", "", 0);
}
#endif /* not DEBUG_FSL_OS_MALLOC */


/*****************************************************************************/
void fsl_os_free(void *p_memory)
{
    sys_mem_free(p_memory);
}

/*****************************************************************************/
void fsl_os_xfree(void *p_memory)
{
    sys_mem_free(p_memory);
}

/*****************************************************************************/
void * fsl_os_phys_to_virt(dma_addr_t addr)
{
    return sys_phys_to_virt(addr);
}

/*****************************************************************************/
dma_addr_t fsl_os_virt_to_phys(void *addr)
{
    return sys_virt_to_phys(addr);
}


#ifdef ARENA_LEGACY_CODE
/*****************************************************************************/
/*                        Timers Service Routines                            */
/*****************************************************************************/

uint32_t fsl_os_current_time(void)
{
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
    return 0;
}

fsl_handle_t fsl_os_create_timer(void)
{
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
    return NULL;
}

void fsl_os_free_timer(fsl_handle_t tmr)
{
    UNUSED (tmr);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
}

int fsl_os_start_timer(fsl_handle_t   tmr,
                       uint32_t       msecs,
                       int            periodic,
                       void           (*expired_cb)(fsl_handle_t),
                       fsl_handle_t   arg)
{
    UNUSED (arg);
    UNUSED (expired_cb);
    UNUSED (msecs);
    UNUSED (periodic);
    UNUSED (tmr);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
    return 0;
}

void fsl_os_stop_timer(fsl_handle_t tmr)
{
    UNUSED (tmr);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
}

void fsl_os_mod_timer(fsl_handle_t tmr, uint32_t msecs)
{
    UNUSED (tmr);
    UNUSED (msecs);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
}

void fsl_os_udelay(uint32_t usecs)
{
    UNUSED (usecs);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
}

uint32_t fsl_os_sleep(uint32_t msecs)
{
    UNUSED (msecs);
    REPORT_ERROR(MINOR, E_NOT_SUPPORTED, ("Timer!"));
    return 0;
}
#endif
