#ifndef __SYS_CONSOLE_H
#define __SYS_CONSOLE_H

#include "common/types.h"


/**************************************************************************//**
 @Group         sys_grp     System Interfaces

 @Description   Bare-board system programming interfaces.

 @{
*//***************************************************************************/
/**************************************************************************//**
 @Group         sys_console_grp     System Console Service

 @Description   Bare-board system interface for console operations.

 @{
*//***************************************************************************/


/**************************************************************************//**
 @Function      sys_register_console

 @Description   Register a new system console.

 @Param[in]     h_ConsoleDev    - Handle to console device (passed as the first
                                  parameter to \c f_ConsolePrint and \c f_ConsoleGet).
 @Param[in]     f_ConsolePrint  - Console routine for printing characters.
 @Param[in]     f_ConsoleGet    - Console routine for reading characters.

 @Return        E_OK on success; Error code otherwise.

 @Cautions      Only a single console is supported. The existing console must
                be unregistered before a new one is registered.
*//***************************************************************************/
int sys_register_console(fsl_handle_t    h_console_dev,
                            int         (*f_console_print)(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size),
                            int         (*f_console_get)(fsl_handle_t h_console_dev, uint8_t *p_data, uint32_t size));

/**************************************************************************//**
 @Function      sys_unregister_console

 @Description   Unregister the existing system console.

 @Return        E_OK on success; Error code otherwise.
*//***************************************************************************/
int sys_unregister_console(void);

/**************************************************************************//**
 @Function      sys_print

 @Description   Prints a characters string to the console device.

                If no console has been registered yet, the function prints the
                string into an internal buffer. The buffer will be printed to
                the first registered console.

 @Param[in]     str - The characters string to print.

 @Return        None.
*//***************************************************************************/
void sys_print(char *str);

/**************************************************************************//**
 @Function      sys_get_char

 @Description   Reads a single character from the console device.

 @Return        The character read from the console; Zero on failure or if no
                console has been registered yet.
*//***************************************************************************/
char sys_get_char(void);

/** @} */ /* end of sys_console_grp */
/** @} */ /* end of sys_grp */


#endif /* __SYS_CONSOLE_H */
