#include "common/types.h"
#include "inc/fsl_gen.h"
#include "kernel/fsl_spinlock.h"
#include "common/fsl_string.h"
#include "common/fsl_malloc.h"

#include "inc/console.h"
#include "sys.h"

#define PRE_CONSOLE_BUF_SIZE    (4 * 1024)


/* Global System Object */
extern t_system sys;

#ifdef SIMULATOR
static int system_call(int num, int arg0, int arg1, int arg2, int arg3)
{
       int out;

       __asm__ volatile (
             "mr 0, %5"  "\n"
             "mr 3, %1" "\n"
             "mr 4, %2" "\n"
             "mr 5, %3" "\n"
             "mr 6, %4" "\n"
             "sc" "\n"
             "mr %0, 3" "\n"
             : "=r"(out)
             : "r"(arg0), "r"(arg1), "r"(arg2), "r"(arg3), "r"(num)
             : "r0", "r3", "r4", "r5", "r6", "r7"
       );

       return out;
}
#endif /* SIMULATOR */

/*****************************************************************************/
static int sys_debugger_print(fsl_handle_t unused,
                              uint8_t *p_data,
                              uint32_t size)
{
	int ret;
#ifdef AIOP
	lock_spinlock(&(sys.console_lock));
#else
	uint32_t int_flags;
	/* Disable interrupts to work-around CW bug */
	int_flags = spin_lock_irqsave(&(sys.console_lock));
#endif

#ifdef SIMULATOR
	ret = system_call(4, 1, (int)PTR_TO_UINT(p_data), (int)size, 0);
#else
	ret = printf((char *)p_data);
	fflush(stdout);
#endif /* SIMULATOR */

#ifdef AIOP
	unlock_spinlock(&(sys.console_lock));
#else
	spin_unlock_irqrestore(&(sys.console_lock), int_flags);
#endif

	UNUSED(unused);
	UNUSED(size);
	return ret;
}

/*****************************************************************************/
int sys_register_console(fsl_handle_t h_console_dev,
                               int(*f_console_print)(fsl_handle_t h_console_dev,
                                                     uint8_t *p_data,
                                                     uint32_t size),
                               int(*f_console_get)(fsl_handle_t h_console_dev,
                                                   uint8_t *p_data,
                                                   uint32_t size))
{
	if (sys.console)
		RETURN_ERROR(MINOR, E_ALREADY_EXISTS, ("system console"));

	/* Must have Print routine (Get routine is not mandatory) */
	ASSERT_COND(f_console_print);

	sys.console = h_console_dev;

	sys.f_console_print = f_console_print;

	sys.f_console_get = f_console_get;

#ifdef AIOP
	sys.console_lock = 0; /* spinlock init */
#else
    spin_lock_init(&(sys.console_lock));
#endif

	/* Flush pre-console printouts as necessary */
	if (h_console_dev && sys.p_pre_console_buf) {
		sys_print(sys.p_pre_console_buf);
		fsl_os_free(sys.p_pre_console_buf);
		sys.p_pre_console_buf = NULL;
		sys.pre_console_buf_pos = 0;
	}

	return E_OK;
}

/*****************************************************************************/
int sys_unregister_console(void)
{
	if (!sys.console)
		RETURN_ERROR(MINOR, E_NOT_AVAILABLE, ("console"));

	sys.console = NULL;

	return E_OK;
}

/*****************************************************************************/
void sys_print(char *str)
{
	uint32_t count;

#ifdef AIOP
	lock_spinlock(&(sys.console_lock));
#else
	uint32_t int_flags;
	int_flags = spin_lock_irqsave(&(sys.console_lock));
#endif
	count = (uint32_t)strlen(str);

	/* Print to the registered console, if exists */

	if (sys.console)
		sys.f_console_print(sys.console, (uint8_t *)str, count);
	else {
		if (!sys.p_pre_console_buf) {
			sys.p_pre_console_buf = (char *)fsl_os_malloc(
			        PRE_CONSOLE_BUF_SIZE);

			if (!sys.p_pre_console_buf) {
				/* Cannot print error message - will lead to recursion */
#ifdef AIOP
				unlock_spinlock(&(sys.console_lock));
#else
				spin_unlock_irqrestore(&(sys.console_lock), int_flags);
#endif
				fsl_os_exit(1);
			}

			memset(sys.p_pre_console_buf, 0, PRE_CONSOLE_BUF_SIZE);
		}

		if (count >= (PRE_CONSOLE_BUF_SIZE - sys.pre_console_buf_pos)) {
			/* Reached buffer end - overwrite from buffer start */
			sys.pre_console_buf_pos = 0;
			sys.pre_console_buf_pos += sprintf(
			        sys.p_pre_console_buf, "[TRUNCATED]...\n");
		}

		memcpy(&(sys.p_pre_console_buf[sys.pre_console_buf_pos]),
		       str/*sys.print_buf*/, count);
		sys.pre_console_buf_pos += count;
	}

#ifdef AIOP
				unlock_spinlock(&(sys.console_lock));
#else
				spin_unlock_irqrestore(&(sys.console_lock), int_flags);
#endif
}

#ifdef MC
/*****************************************************************************/
char sys_get_char(void)
{
	uint8_t temp_char;
	int read_chars;
	uint32_t int_flags;

	if (sys.console && sys.f_console_get) {
		int_flags = spin_lock_irqsave(&(sys.console_lock));

		read_chars = sys.f_console_get(sys.console, &temp_char, 1);

		spin_unlock_irqrestore(&(sys.console_lock), int_flags);

		if (read_chars == 1)
			return temp_char;
	}

	return ('\0');
}
#endif

/*****************************************************************************/
void sys_register_debugger_console(void)
{
	int err_code = E_OK;

	UNUSED(err_code);

	/* Register system console */
	err_code = sys_register_console(&sys, sys_debugger_print, NULL);
	ASSERT_COND(err_code == E_OK);
}

