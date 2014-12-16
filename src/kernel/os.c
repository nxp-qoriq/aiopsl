/*
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Freescale Semiconductor nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_malloc.h"
#include "common/fsl_stdarg.h"
#include "kernel/fsl_spinlock.h"
#include "aiop_common.h"
#include "fsl_io.h"
#include "fsl_smp.h"
#include "inc/console.h"
#include "fsl_mem_mng.h"
#include "sys.h"
#include "general.h"
#include "fsl_fdma.h"
#include <string.h>

__TASK uint32_t seed_32bit;

#define __ERR_MODULE__ MODULE_UNKNOWN

#define MAX_UDELAY   50000000

#define SIGN	2

#define BUF_SIZE           1024  /*used for print in boot mode*/
#define RUNTIME_BUF_SIZE   80    /*used for print in runtime mode*/

static const char* digits="0123456789abcdef";

extern t_system sys;
static int vsnprintf_lite(char *buf, size_t size, const char *fmt, va_list args);
static char *number(char *str, uint64_t num, uint8_t base, uint8_t type, size_t *max_size);
static void fsl_os_print_boot(const char *format, va_list args);
/*****************************************************************************/
void fsl_os_exit(int status)
{
    exit(status);
}

/*****************************************************************************/
void fsl_os_print(char *format, ...)
{
	va_list args;
	char    tmp_buf[RUNTIME_BUF_SIZE];

	va_start(args, format);

	if(sys.runtime_flag){
		vsnprintf_lite(tmp_buf, RUNTIME_BUF_SIZE, format, args);
		va_end(args);
		sys_print(tmp_buf);
	}
	else
		fsl_os_print_boot(format, args);
}

__COLD_CODE static void fsl_os_print_boot(const char *format, va_list args){
	char    tmp_buf[BUF_SIZE];
	vsnprintf(tmp_buf, BUF_SIZE, format, args);
	va_end(args);
	sys_print(tmp_buf);
}


static int vsnprintf_lite(char *buf, size_t size, const char *fmt, va_list args)
{
	uint64_t num;
	uint8_t  base;
	char *str;
	const char *s;
	uint8_t flags = 0;
	size--;
	for(str = buf; *fmt && size; ++fmt) {
		if(*fmt != '%') {
			*str++ = *fmt;
			size--;
			continue;
		}

		++fmt;
		base = 10;
		switch(*fmt) {
		case 'c':
			if(size)
			{
				*str++ = (unsigned char)va_arg(args,int);
				size--;
			}
			continue;

		case 's':
			s = va_arg(args,char*);
			if(!s)
				break;
			while(*s != '\0' && size) {
				*str++ = *s++;
				size--;
			}
			continue;

		case 'x':
			base = 16;
		case 'd':
		case 'l':
			if(*fmt == 'l'){
				switch(*(fmt + 1)){
				case 'l':/*support %ll*/
					++fmt;
					num = va_arg(args,unsigned long long);
					break;
				default: /*support l*/
					num = va_arg(args,unsigned long);
					break;
				}



				if(*(fmt+1) == 'x'){
					base = 16;
					++fmt;
				}


			}
			else{	/*d, x*/
				if (*(fmt) == 'd'){
					flags |= SIGN;
					num = va_arg(args,int);
				}else{
					num = va_arg(args,unsigned long);
				}
			}

			break; /*start convert number to string*/

		default: /*special inputs are not supported*/
			if(size){
			*str++ = '%';
			size--;
			}
			if(size){
			*str++ = *fmt;
			size--;
			}
			continue;
		}

		/*start convert number to string*/

		str = number(str, num, base,flags, &size);
		flags = 0;
	}

	*str = '\0';
	return str - buf;
}


static char *number(char *str, uint64_t num, uint8_t base, uint8_t type, size_t *max_size)
{
	uint64_t tmp_num;
	char *ptr_start, *ptr_end, tmp_char;
	uint8_t i;
	size_t msize;

	msize = *max_size;
	tmp_num = num;


	if(type & SIGN) {
		if((int)num < 0 && msize) {
			*str++ = '-';
			num = -num;
			msize --;
		}
	}

	i = 0;
	if(msize){
		if(num == 0)
			*str++ = '0';
		else {
			while(tmp_num != 0) {	/*count number of digits*/
				tmp_num /= base;
				i++;
			}

			ptr_start = str; /*remember start pointer*/
			while(num != 0){

				if(i <= msize)
					*str++ = digits[(num) % (unsigned) base];
				else
					i--;

				num /= base;
			}
			msize -= i;	/* reduce wrote digits from total buf size*/
			ptr_end = str;  /*remember end pointer*/
			ptr_end --;
			while(ptr_start < ptr_end){
				tmp_char = *ptr_start;
				*ptr_start++ = *ptr_end;
				*ptr_end-- = tmp_char;
			}
		}
	}


	*max_size = msize;
	return str;
}

/*****************************************************************************/
uint32_t fsl_os_rand(void)
{
	seed_32bit = (seed_32bit>>1) ^ (-(seed_32bit & 1LL) &
			0xFBE16801);

	return seed_32bit;
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

#if 0
#ifdef DEBUG_FSL_OS_MALLOC
void * fsl_os_malloc_debug(size_t size, char *fname, int line);

void *fsl_os_xmalloc_debug(size_t size,
                           int      mem_partition_id,
                           uint32_t alignment,
                           char     *fname,
                           int      line);
#endif


#define fsl_os_malloc(sz) \
    fsl_os_malloc_debug((sz), __FILE__, __LINE__)

#define fsl_os_xmalloc(sz, memt, al) \
   fsl_os_xmalloc_debug((sz), (memt), (al), __FILE__, __LINE__)


/*****************************************************************************/
void * fsl_os_malloc_debug(size_t size, char *fname, int line)
{
    return sys_mem_alloc(size, 0, "", fname, line);
}

/*****************************************************************************/

void *fsl_os_xmalloc_debug(size_t     size,
                           int          partition_id,
                           uint32_t     alignment,
                           char         *fname,
                           int          line)
{
	return sys_mem_xalloc(partition_id, size, alignment, "", fname, line);
}
#endif
/*****************************************************************************/
void * fsl_malloc(size_t size,uint32_t alignment)
{
#ifdef DEBUG_FSL_OS_MALLOC
	return  sys_shram_alloc(size,alignment,"",__FILE__, __LINE__);
#else
	return sys_shram_alloc(size,alignment,"","", 0);
#endif
}
/*****************************************************************************/
void fsl_free(void *mem)
{
	sys_shram_free(mem);
}

/*****************************************************************************/
#if 0
#ifdef DEBUG_FSL_OS_MALLOC
void * fsl_os_malloc(size_t size)
{
	return  fsl_os_malloc_debug(size, __FILE__, __LINE__);
}
#else
void * fsl_os_malloc(size_t size)
{
    return sys_mem_alloc(size, 0, "", "", 0);
}
#endif

/*****************************************************************************/

#ifdef DEBUG_FSL_OS_MALLOC
void *fsl_os_xmalloc(size_t size, int partition_id, uint32_t alignment)
{
	return fsl_os_xmalloc_debug(size, partition_id, alignment, __FILE__, __LINE__);
}
#else
void *fsl_os_xmalloc(size_t size, int partition_id, uint32_t alignment)
{
    return sys_mem_xalloc(partition_id, size, alignment, "", "", 0);
}
#endif


/*****************************************************************************/
void fsl_os_free(void *p_memory)
{
    sys_mem_free(p_memory);
}

/*****************************************************************************/
void fsl_os_xfree(void *p_memory)
{
    sys_mem_xfree(p_memory);
}
#endif
/*****************************************************************************/
int fsl_os_get_mem(uint64_t size, int mem_partition_id, uint64_t alignment,
                   uint64_t* paddr)
{
	return sys_get_phys_mem(size, mem_partition_id, alignment,paddr);
}
/*****************************************************************************/
void fsl_os_put_mem(uint64_t paddr)
{
    sys_put_phys_mem(paddr);
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
#ifndef AIOP_VERIF /*TODO: Remove #ifndef AIOP_VERIF when the code will be separated */
void exception_handler(char *filename,
		       char *function_name,
		       uint32_t line,
		       char *message) __attribute__ ((noreturn))
{
#ifndef STACK_CHECK
	filename = strrchr(filename, '/') ?
			strrchr(filename, '/') + 1 : filename;
	pr_err("Fatal error encountered in file: %s, line: %d\n", filename, line);
	pr_err("function: %s\n", function_name);
	pr_err("exception error: %s\n", message);
#endif
	fdma_terminate_task();
	exit(-1); /* TODO This code is never reached and should be removed once
	fdma_terminate_task() is declared as noreturn*/
}
#endif


#ifdef ARENA_LEGACY_CODE
/*****************************************************************************/
/*                        Timers Service Routines                            */
/*****************************************************************************/

uint32_t fsl_os_current_time(void)
{
    REPORT_ERROR(MINOR, ENOTSUP, ("Timer!"));
    return 0;
}

fsl_handle_t fsl_os_create_timer(void)
{
    REPORT_ERROR(MINOR, ENOTSUP, ("Timer!"));
    return NULL;
}

void fsl_os_free_timer(fsl_handle_t tmr)
{
    UNUSED (tmr);
    REPORT_ERROR(MINOR, ENOTSUP, ("Timer!"));
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
    REPORT_ERROR(MINOR, ENOTSUP, ("Timer!"));
    return 0;
}

void fsl_os_stop_timer(fsl_handle_t tmr)
{
    UNUSED (tmr);
    REPORT_ERROR(MINOR, ENOTSUP, ("Timer!"));
}

void fsl_os_mod_timer(fsl_handle_t tmr, uint32_t msecs)
{
    UNUSED (tmr);
    UNUSED (msecs);
    REPORT_ERROR(MINOR, ENOTSUP, ("Timer!"));
}

void fsl_os_udelay(uint32_t usecs)
{
    UNUSED (usecs);
    REPORT_ERROR(MINOR, ENOTSUP, ("Timer!"));
}

uint32_t fsl_os_sleep(uint32_t msecs)
{
    UNUSED (msecs);
    REPORT_ERROR(MINOR, ENOTSUP, ("Timer!"));
    return 0;
}
#endif
