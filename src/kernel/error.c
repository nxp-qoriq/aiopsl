/**************************************************************************//**
            Copyright 2013 Freescale Semiconductor, Inc.

 @File          error.c

 @Description   General errors and events reporting utilities.
*//***************************************************************************/
#if (defined(DEBUG_ERRORS) && (DEBUG_ERRORS > 0))
#include "common/errors.h"


const char *dbg_level_strings[] = {
     "CRITICAL"
    ,"MAJOR"
    ,"MINOR"
    ,"WARNING"
    ,"INFO"
    ,"TRACE"
};

char * err_type_strings (enum error_type err) {
    switch (err) {
        case (E_OK):                    return "OK";
        case (E_WRITE_FAILED):          return "write access failed";
        case (E_NO_DEVICE):             return "no device";
        case (E_NOT_AVAILABLE):         return "resource is unavailable";
        case (E_NO_MEMORY):             return "memory allocation failed";
        case (E_INVALID_ADDRESS):       return "invalid address";
        case (E_BUSY):                  return "resource is busy";
        case (E_ALREADY_EXISTS):        return "resource already exists";
        case (E_INVALID_OPERATION):     return "invalid operation";
        case (E_INVALID_VALUE):         return "invalid value";
        case (E_NOT_IN_RANGE):          return "value out of range";
        case (E_NOT_SUPPORTED):         return "unsupported operation";
        case (E_TIMEOUT):               return "operation timed out";
        case (E_INVALID_STATE):         return "invalid state";
        case (E_INVALID_SELECTION):     return "invalid selection";
        case (E_CONFLICT):              return "conflict in settings";
        case (E_NOT_FOUND):             return "resource not found";
        case (E_FULL):                  return "resource is full";
        case (E_EMPTY):                 return "resource is empty";
        case (E_ALREADY_FREE):          return "resource already free";
        case (E_READ_FAILED):           return "read access failed";
        case (E_INVALID_FRAME):         return "invalid frame";
        default:
            break;
    }
    return NULL;
}
#endif /* (defined(DEBUG_ERRORS) && (DEBUG_ERRORS > 0)) */
