/*
 * Copyright 2014-2015 Freescale Semiconductor, Inc.
 * Copyright 2017-2018 NXP
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the above-listed copyright holders nor the
 *     names of any contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_malloc.h"
#include "common/fsl_stdarg.h"
#include "kernel/fsl_spinlock.h"
#include "fsl_aiop_common.h"
#include "fsl_io.h"
#include "fsl_smp.h"
#include "fsl_console.h"
#include "fsl_mem_mng.h"
#include "fsl_system.h"
#include "fsl_log.h"
#include "fsl_stdlib.h"
#include "fsl_string.h"
#include "fsl_cdma.h"

__TASK uint32_t seed_32bit;
__TASK uint32_t exception_flag;


#define __ERR_MODULE__ MODULE_UNKNOWN

#define MAX_UDELAY   50000000

#define SIGN	2

/* Size of Accelerator Hardware Context to reserve when calling to accelerator*/
#define SIZE_OF_HWC_TO_RESERVE  32
/* Buffer size used for print in boot mode*/
#define BUF_SIZE                1024
/* Buffer size used for print in runtime mode*/
#define RUNTIME_BUF_SIZE        80

static const char* digits="0123456789abcdef";
static uint8_t g_hwc[SIZE_OF_HWC_TO_RESERVE];

void enable_print_protection();
void disable_print_protection();
void dbg_print(char *format, ...);
static int vsnprintf_lite(char *buf, size_t size, const char *fmt, va_list args);
static char *number(char *str, uint64_t num, uint8_t base, uint8_t type, size_t *max_size, uint8_t fix_size);
static void fsl_print_boot(const char *format, va_list args);


__COLD_CODE void enable_print_protection()
{
	uint8_t hwc[SIZE_OF_HWC_TO_RESERVE];
	/*Save Accelerator Hardware Context*/
	/**HWC_ACC_IN_ADDRESS - Address for passing parameters to accelerators
	 * The accelerator hardware context must be stored on stack first,
	 * because the call for mutex will overwrite those parameters.
	 * After the mutex is enabled and the global variable is protected, the
	 * content can be saved in global variable to save stack for print
	 * function.*/
	memcpy((void *) hwc, (const void *) HWC_ACC_IN_ADDRESS, SIZE_OF_HWC_TO_RESERVE);
	/*
	 * increment exception counter to avoid printing in exception handler
	 * in case we will get exception during the print
	 * */
	exception_flag += 1;
	cdma_mutex_lock_take((uint64_t)fsl_print, CDMA_MUTEX_WRITE_LOCK);
	/*g_hwc already locked by mutex*/
	memcpy((void *) g_hwc, (const void *) hwc, SIZE_OF_HWC_TO_RESERVE);
}

__COLD_CODE void disable_print_protection()
{
	uint8_t hwc[SIZE_OF_HWC_TO_RESERVE];
	/* g_hwc already locked by mutex*/
	/**HWC_ACC_IN_ADDRESS - Address for passing parameters to accelerators
	 * The accelerator hardware context must be restored to stack first,
	 * because the call for mutex will overwrite those parameters.
	 * After the mutex is disabled, the content can be restored.*/
	memcpy((void *)hwc, (const void *)g_hwc, SIZE_OF_HWC_TO_RESERVE);
	cdma_mutex_lock_release((uint64_t)fsl_print);
	/* decrement exception counter */
	exception_flag -= 1;
	/*Restore Accelerator Hardware Context*/
	/** Address for passing parameters to accelerators */
	memcpy((void *) HWC_ACC_IN_ADDRESS, (const void *) hwc,  SIZE_OF_HWC_TO_RESERVE);
}

static inline void fsl_print_runtime(const char *format, va_list args)
{
	char    buf[RUNTIME_BUF_SIZE];
	vsnprintf_lite(buf, RUNTIME_BUF_SIZE, format, args);
	va_end(args);
	sys_print(buf);
}

__COLD_CODE void fsl_print(char *format, ...)
{
	va_list args;
	va_start(args, format);
	/*enable_print_protection() - calling to print must be under mutex, with
	 * exception handler flag on and Accelerator Hardware context backed up
	 * to restore the previous state*/
	enable_print_protection();
	if(sys.runtime_flag){
		fsl_print_runtime(format, args);
	}
	else{
		fsl_print_boot(format, args);
	}
	disable_print_protection();
}
/*dbg_print is called from pr_xxx functions which must call:
 * enable_print_protection() first*/
__COLD_CODE void dbg_print(char *format, ...)
{
	va_list args;
	va_start(args, format);
	if(sys.runtime_flag){
		fsl_print_runtime(format, args);
	}
	else{
		fsl_print_boot(format, args);
	}
}

/*pragma used to ignore stack check for the function below*/
#pragma stackinfo_ignore on
__COLD_CODE static void fsl_print_boot(const char *format, va_list args)
{
	char    buf[BUF_SIZE];
	vsnprintf(buf, BUF_SIZE, format, args);
	va_end(args);
	sys_print(buf);
}

__COLD_CODE static int vsnprintf_lite(char *buf, size_t size, const char *fmt, va_list args)
{
	uint64_t num;
	uint8_t  base;
	char *str;
	const char *s;
	uint8_t flags = 0;
	uint8_t fix_size;
	size--;/*save one character for null character*/
	for(str = buf; *fmt && size; ++fmt) {
		if(*fmt != '%') {
			*str++ = *fmt;
			size--;
			continue;
		}

		++fmt;
		base = 10;
		if(*fmt == '0'){
			++fmt;
			fix_size =(uint8_t) *(fmt) - 0x30;
			++fmt;
		}
		else
		{
			fix_size = 0;
		}

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
				continue;
			while(*s != '\0' && size) {
				*str++ = *s++;
				size--;
			}
			continue;

		case 'x':
		case 'X':
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



				if(*(fmt+1) == 'x' || *(fmt+1) == 'X') {
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

		str = number(str, num, base,flags, &size, fix_size);
		flags = 0;
	}

	*str = '\0';
	return str - buf;
}

__COLD_CODE static char *number(char *str, uint64_t num, uint8_t base, uint8_t type, size_t *max_size, uint8_t fix_size)
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
	if(msize)
	{
		do {	/*count number of digits*/
			tmp_num /= base;
			i++;
		} while(tmp_num != 0);

		if(fix_size > 0)
		{
			while(i < fix_size && msize){
				*str++ = '0';
				fix_size --;
				msize --;

			}
		}
		ptr_start = str; /*remember start pointer*/
		do {

			if(i <= msize)
				*str++ = digits[(num) % (unsigned) base];
			else
				i--;

			num /= base;
		}while(num != 0);
		msize -= i;	/* reduce wrote digits from total buf size*/
		ptr_end = str;  /*remember end pointer*/
		ptr_end --;
		while(ptr_start < ptr_end){
			tmp_char = *ptr_start;
			*ptr_start++ = *ptr_end;
			*ptr_end-- = tmp_char;
		}
	}


	*max_size = msize;
	return str;
}

/*****************************************************************************/
RAND_CODE_PLACEMENT uint32_t fsl_rand(void)
{
	seed_32bit = (seed_32bit>>1) ^ (-(seed_32bit & 1LL) &
		0xFBE16801);

	return seed_32bit;
}

/* A-010571: Shared/Main SRAM exclusive write lost in back-to-back writes
 * to consecutive sub-dword addresses within the same dword.
 * Impact: AIOP Software sees incorrect operation since a semaphore write
 * would report success when it is actually failed.
 * Workaround: Software should declare semaphore variable as type double
 * to prevent exclusive write at sub-dword granule.
 */
/*****************************************************************************/
/*                        Spinlock Service Routines                          */
/*****************************************************************************/
__HOT_CODE void lock_spinlock(register uint64_t *spinlock)
{
	uint64_t temp;

spinlock_loop:
	do {
		__llldwar(&temp, 0, spinlock);
	} while (temp != 0);

	__llstdwc_((uint64_t)1, 0, spinlock);
	asm {
		bne - spinlock_loop
	}
}

__HOT_CODE void unlock_spinlock(
	uint64_t *spinlock) {
	*spinlock = 0;
}

/***************************************************************************
 * atomic_incr64
 ***************************************************************************/
__HOT_CODE void atomic_incr64(register int64_t *var, register int64_t value)
{
	uint64_t orig_value;

atomic_loop:
	__llldwar(&orig_value, 0, var);
	orig_value = (uint64_t)((int64_t)orig_value + value);
	__llstdwc_(orig_value, 0, var);
	asm {
		bne - atomic_loop
	}
}

/***************************************************************************
 * atomic_decr64
 ***************************************************************************/
__HOT_CODE void atomic_decr64(register int64_t *var, register int64_t value)
{
	uint64_t orig_value;

atomic_loop:
	__llldwar(&orig_value, 0, var);
	orig_value = (uint64_t)((int64_t)orig_value - value);
	__llstdwc_(orig_value, 0, var);
	asm {
		bne - atomic_loop
	}
}



/*****************************************************************************/
MEM_MNG_CODE_PLACEMENT void * fsl_malloc(size_t size,uint32_t alignment)
{
#ifdef DEBUG_FSL_MALLOC
	return  sys_shram_alloc(size,alignment,"",__FILE__, __LINE__);
#else
	return sys_shram_alloc(size,alignment,"","", 0);
#endif
}
/*****************************************************************************/
MEM_MNG_CODE_PLACEMENT void fsl_free(void *mem)
{
	sys_shram_free(mem);
}
/*****************************************************************************/
MEM_MNG_CODE_PLACEMENT int fsl_get_mem(uint64_t size, int mem_partition_id, uint64_t alignment,
                   uint64_t* paddr)
{
	return sys_get_phys_mem(size, mem_partition_id, alignment,paddr);
}
/*****************************************************************************/
MEM_MNG_CODE_PLACEMENT void fsl_put_mem(uint64_t paddr)
{
	sys_put_phys_mem(paddr);
}

int fsl_mem_exists(int mem_partition_id)
{
    return sys_mem_exists(mem_partition_id);
}

#ifdef ARENA_LEGACY_CODE
/*****************************************************************************/
/*                        Timers Service Routines                            */
/*****************************************************************************/

uint32_t fsl_os_current_time(void)
{
	pr_warn("Timer!");
	return 0;
}

void * fsl_os_create_timer(void)
{
	pr_warn("Timer!");
	return NULL;
}

void fsl_free_timer(void * tmr)
{
	UNUSED (tmr);
	pr_warn("Timer!");
}

int fsl_os_start_timer(void *   tmr,
                       uint32_t       msecs,
                       int            periodic,
                       void           (*expired_cb)(void *),
                       void *   arg)
{
	UNUSED (arg);
	UNUSED (expired_cb);
	UNUSED (msecs);
	UNUSED (periodic);
	UNUSED (tmr);
	pr_warn("Timer!");
	return 0;
}

void fsl_os_stop_timer(void * tmr)
{
	UNUSED (tmr);
	pr_warn("Timer!");
}

void fsl_os_mod_timer(void * tmr, uint32_t msecs)
{
	UNUSED (tmr);
	UNUSED (msecs);
	pr_warn("Timer!");
}

void fsl_os_udelay(uint32_t usecs)
{
	UNUSED (usecs);
	pr_warn("Timer!");
}

uint32_t fsl_os_sleep(uint32_t msecs)
{
	UNUSED (msecs);
	pr_warn("Timer!");
	return 0;
}
#endif
